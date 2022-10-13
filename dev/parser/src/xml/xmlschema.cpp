//////////////////////////////////////////////////////////
// File Name: xmlschema.cpp
//
// File created : 2003/5/9
// Last modified : 2003/6/29
//
// Copyright (c) 2003, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#include "xmlschema.h"

//======================================================
//  class XSElement
//======================================================
// Reserved type names for simple type
const TCHAR* XSElement::m_SimpleTypeNames[] =
{
    _T( "string" ),
    _T( "byte" ),
    _T( "binary" ),
    _T( "integer" ),
    _T( "float" ),
    _T( "boolean" ),
    _T( "datetime" ),
    _T( "currency" ),
    _T( "identity" ),
    NULL
};

// Constructor
XSElement::XSElement() :
    m_pColumnName( NULL ),
    m_SimpleType( stUnknown ),
    m_pComplexType( NULL ),
    m_pParent( NULL ),
    m_bIndex( false )
{}

// Copy constructor
XSElement::XSElement( const XSElement& rhs )
{
    *this = rhs;
}

// Constructor by given parameters
XSElement::XSElement( const _tstring& name, const _tstring& field,
                      const _tstring& type_name, bool is_index ) :
    name_( name ),
    m_FieldName( field ),
    m_TypeName( type_name ),
    m_pComplexType( NULL ),
    m_pParent( NULL ),
    m_bIndex( is_index )
{
    // init simple-type and table,column names
    m_SimpleType = StringToSimpleType( m_TypeName );
    BuildTableName();
}

// Copy operator
XSElement& XSElement::operator=( const XSElement& rhs )
{
    name_ = rhs.name_;
    m_FieldName = rhs.m_FieldName;
    m_TableName = rhs.m_TableName;
    m_JoinFieldName = rhs.m_JoinFieldName;
    m_TypeName = rhs.m_TypeName;
    m_SimpleType = rhs.m_SimpleType;
    m_pComplexType = NULL;
    m_ParentTypeName = rhs.m_ParentTypeName;
    m_ParentKeyName = rhs.m_ParentKeyName;
    m_pParent = NULL;
    m_bIndex = rhs.m_bIndex;
    BuildTableName();
    return *this;
}

// Serialization
bool XSElement::Serialize( XMLNode& xml, bool is_storing )
{
    if ( is_storing )
    {
        xml.name_ = _T( "element" );
        xml[_T( "name" )].text_ = name_;
        xml[_T( "type" )].text_ = m_TypeName;
        
        if ( m_pComplexType == NULL )
        {
            xml[_T( "field" )].text_ = m_FieldName;
            
            if ( IsJoined() ) // save joined-field
            {
                XMLNode* node = new XMLNode();
                node->name_ = _T( "field" );
                node->text_ = m_JoinFieldName;
                xml.child_nodes_.push_back( node );
            }
            
            xml[_T( "index" )].text_ = m_bIndex ? _T( "true" ) : _T( "false" );
            
            // save relationship
            if ( m_pParent )
            {
                XMLNode& node = xml[_T( "parent" )];
                node[_T( "type" )].text_ = m_ParentTypeName;
                node[_T( "element" )].text_ = m_pParent->name_;
            }
        }
        
        return true;
    }
    else
    {
        XMLNode* node;
        
        if ( ( node = xml.FindNode( _T( "name" ) ) ) != NULL )
        {
            name_ = node->text_;
            
            if ( ( node = xml.FindNode( _T( "type" ) ) ) != NULL )
            {
                m_TypeName = node->text_;
                m_SimpleType = StringToSimpleType( m_TypeName ); // find simple type
                
                if ( m_SimpleType == stUnknown ) { return true; } // it may be a complex type
                
                if ( ( node = xml.FindNode( _T( "index" ) ) ) != NULL )
                {
                    m_bIndex = ( node->text_ == _T( "true" ) );
                }
                
                // load relationship
                if ( ( node = xml.FindNode( _T( "parent" ) ) ) != NULL )
                {
                    m_ParentTypeName = node->operator[]( _T( "type" ) ).text_;
                    m_ParentKeyName = node->operator[]( _T( "element" ) ).text_;
                }
                
                // load field name if it's simple type
                if ( ( node = xml.FindNode( _T( "field" ) ) ) != NULL )
                {
                    m_FieldName = node->text_;
                    // check if there's a join field
                    node = xml.FindNextNode( _T( "field" ) );
                    
                    if ( node ) { m_JoinFieldName = node->text_; }
                    
                    return BuildTableName();
                }
            }
        }
    }
    
    return false;
}

// Convert string to simple type
XSElement::SimpleType XSElement::StringToSimpleType( const _tstring& type_name )
{
    for ( int i = 0; m_SimpleTypeNames[i] != NULL; i++ )
    {
        if ( type_name == m_SimpleTypeNames[i] )
        {
            return ( SimpleType )i;
        }
    }
    
    return stUnknown;
}

// Convert simple type to string
const TCHAR* XSElement::SimpleTypeToString( XSElement::SimpleType type )
{
    return m_SimpleTypeNames[type];
}

// Convert XMLNode to field-value string
bool XSElement::NodeToString( const XMLNode& node, _tstring& result, bool bNoField )
{
    if ( name_ == node.name_ && m_SimpleType != stUnknown )
    {
        result = bNoField ? _T( "" ) : m_FieldName + _T( "=" );
        
        switch ( m_SimpleType )
        {
        case stString:
        case stByte:
        case stDateTime:
            {
                _tstring str = node.text_;
                const TCHAR quote = _T( '\'' );
                unsigned int pos = 0;
                
                // subtitude the quote characters with double quotes
                while ( ( pos = str.find( quote, pos ) ) != str.npos )
                {
                    str.insert( pos, 1, quote );
                    pos += 2;
                }
                
                result += quote + str + quote; // we need to add quote signs
                break;
            }
            
        case stBinary:
            {
                // convert binary to string
                result += _T( "0x" );
                InStrStream is( node.text_ );
                unsigned int value;
                unsigned char digit;
                is >> std::hex;
                
                while ( !is.eof() ) // append digit characters together
                {
                    is >> value;
                    digit = value >> 4;
                    result += digit > 9 ? TCHAR( digit + 'A' - 10 ) : TCHAR( digit + '0' );
                    digit = value & 0x0f;
                    result += digit > 9 ? TCHAR( digit + 'A' - 10 ) : TCHAR( digit + '0' );
                }
                
                break;
            }
            
        default:
            result += node.text_;  // no need to add quotes
        }
        
        return true;
    }
    
    return false;
}

// Build table/column name by field name.
bool XSElement::BuildTableName()
{
    unsigned int pos = m_FieldName.find( _T( '.' ) );
    
    if ( pos != m_FieldName.npos && pos > 0 )
    {
        m_TableName.assign( m_FieldName, 0, pos );
        m_pColumnName = &m_FieldName[pos + 1];
        return true;
    }
    
    m_pColumnName = NULL;
    return false;
}

//======================================================
//  class XSComplexType
//======================================================
// Constructor
XSComplexType::XSComplexType() :
    m_pOwner( NULL )
{}

// Constructor by name
XSComplexType::XSComplexType( const _tstring& name ) :
    name_( name ),
    m_pOwner( NULL )
{}

// Serialization
bool XSComplexType::Serialize( XMLNode& xml, bool is_storing )
{
    if ( is_storing )
    {
        xml.name_ = _T( "complexType" );
        xml[_T( "name" )].text_ = name_;
        
        for ( unsigned int i = 0; i < m_Elements.size(); i++ )
        {
            XMLNode* node = new XMLNode();
            
            if ( m_Elements[i].Serialize( *node, true ) )
            {
                xml.child_nodes_.push_back( node );
            }
        }
    }
    else
    {
        XMLNode* node;
        
        if ( ( node = xml.FindNode( _T( "name" ) ) ) != NULL )
        {
            name_ = node->text_;
            
            for ( unsigned int i = 0; i < xml.child_nodes_.size(); i++ )
            {
                XMLNode* node = xml.child_nodes_[i];
                
                if ( node->name_ == _T( "element" ) )
                {
                    XSElement xse;
                    
                    if ( xse.Serialize( *node, false ) && GetElement( xse.name_ ) == NULL )
                    {
                        m_Elements.push_back( xse );
                    }
                }
            }
        }
        else { return false; }
    }
    
    return true;
}

// Save all the field names of elements to stream.
int XSComplexType::SaveFieldNamesToStream( OutStream& outs, int options, const TCHAR* filter, int nFields )
{
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        if ( xse.IsSimpleType() )
        {
            // show index?
            // show element with specific table or indexed element?
            if ( ( filter == NULL || xse.m_TableName == filter ) &&
                    ( !xse.IsIndexed() || ( options & foHasIndex ) != 0 ) && ( xse.GetSimpleType() != XSElement::stIdentity || ( options & foHasIdentity ) != 0 ) )
            {
                if ( nFields > 0 ) { outs << _T( "," ); } // connect fields
                
                outs << ( ( options & foHasTableName ) != 0 ? xse.m_FieldName.c_str() : xse.m_pColumnName );
                
                if ( options & foHasAlias ) { outs << _T( " '" ) << xse.m_FieldName.c_str() << _T( "'" ); }
                
                nFields++;
            }
        }
        else if ( xse.IsComplexType() )
        {
            nFields += xse.m_pComplexType->SaveFieldNamesToStream(
                           outs, options, filter, nFields );
        }
    }
    
    return nFields;
}

// Save all the condition strings (from indexed elements) to stream.
int XSComplexType::SaveConditionsToStream( OutStream& outs, XMLNode& xml, int options, const TCHAR* filter, int nCons )
{
    XMLNode* node;
    _tstring pair_str;
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        if ( ( node = xml.FindNode( xse.name_ ) ) != NULL )
        {
            if ( xse.IsSimpleType() )
            {
                // show element with specific table, and only show indexed elements
                if ( ( filter == NULL || xse.m_TableName == filter ) &&
                        ( xse.IsIndexed() && ( options & foHasIndex ) != 0 ) &&
                        ( xse.GetSimpleType() != XSElement::stIdentity || ( options & foHasIdentity ) != 0 ) )
                {
                    if ( xse.NodeToString( *node, pair_str, true ) )
                    {
                        if ( nCons > 0 ) { outs << _T( " AND " ); } // connect condition strings
                        
                        outs << ( filter != NULL ? xse.m_pColumnName : xse.m_FieldName.c_str() );
                        std::string tmp;
                        tmp << _T( "=" ) << pair_str;
                        outs << tmp.c_str();
                        nCons++;
                    }
                }
            }
            else if ( xse.IsComplexType() )
            {
                nCons += xse.m_pComplexType->SaveConditionsToStream(
                             outs, *node, options, filter, nCons );
            }
        }
        
        if ( xse.IsJoined() && filter == NULL )
        {
            if ( nCons > 0 ) { outs << _T( " AND " ); } // connect condition strings
            
            outs << xse.m_FieldName.c_str() << _T( "=" ) << xse.m_JoinFieldName.c_str();
            nCons++;
        }
    }
    
    return nCons;
}

// Save all the field-value pairs from the elements to stream.
int XSComplexType::SaveFieldValuePairsToStream( OutStream& outs, XMLNode& xml, int options, const TCHAR* filter, int nFields )
{
    XMLNode* node;
    _tstring pair_str;
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        if ( ( node = xml.FindNode( xse.name_ ) ) != NULL )
        {
            if ( xse.IsSimpleType() )
            {
                // show element with specific table or indexed element?
                if ( ( filter == NULL || xse.m_TableName == filter ) &&
                        ( !xse.IsIndexed() || ( options & foHasIndex ) != 0 ) &&
                        ( xse.GetSimpleType() != XSElement::stIdentity || ( options & foHasIdentity ) != 0 ) )
                {
                    if ( nFields > 0 ) { outs << _T( "," ); } // connect field-value pairs
                    
                    if ( options & foHasTableName ) // show 'field_name=value'
                    {
                        xse.NodeToString( *node, pair_str );
                        outs << pair_str.c_str();
                    }
                    else
                    {
                        xse.NodeToString( *node, pair_str, true );
                        
                        if ( options & foHasColumnName ) // show 'column_name=value'
                        {
                            outs << xse.m_pColumnName << _T( "=" ) << pair_str.c_str();
                        }
                        else { outs << pair_str.c_str(); }
                    }
                    
                    nFields++;
                }
            }
            else if ( xse.IsComplexType() )
            {
                nFields += xse.m_pComplexType->SaveFieldValuePairsToStream(
                               outs, *node, options, filter, nFields );
            }
        }
    }
    
    return nFields;
}

// Convert the flat XML from database to hierarchy-style XML.
void XSComplexType::ConvertFlatXML( XMLNode& flat, XMLNode& result )
{
    XMLNode* node;
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        if ( xse.IsSimpleType() )
        {
            if ( ( node = flat.FindNode( xse.m_FieldName ) ) != NULL )
            {
                result.child_nodes_.push_back( new XMLNode() );
                XMLNode* new_node = result.child_nodes_.back();
                new_node->name_ = xse.name_;
                new_node->text_ = node->text_;
            }
        }
        else if ( xse.IsComplexType() )
        {
            result.child_nodes_.push_back( new XMLNode() );
            XMLNode* new_node = result.child_nodes_.back();
            new_node->name_ = xse.name_;
            xse.m_pComplexType->ConvertFlatXML( flat, *new_node );
        }
    }
}

// Re-initialize element's data type
void XSComplexType::ReInitElementTypes()
{
    if ( !m_pOwner ) { return; }
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        // initialize the pointer of complex type
        if ( xse.m_SimpleType == XSElement::stUnknown )
        {
            // try to reference complex types
            xse.m_pComplexType = m_pOwner->GetComplexType( xse.m_TypeName );
            
            // reinit simple type
            if ( xse.m_pComplexType == NULL )
            {
                xse.m_SimpleType = XSElement::StringToSimpleType( xse.m_TypeName );
            }
        }
        
        // initialize the pointer of parent type
        if ( xse.m_ParentTypeName.size() > 0 )
        {
            XSComplexType* xsc = m_pOwner->GetComplexType( xse.m_ParentTypeName );
            
            if ( xsc ) { xse.m_pParent = xsc->GetElement( xse.m_ParentKeyName ); }
        }
    }
}

// Get element by name
XSElement* XSComplexType::GetElement( const _tstring& name )
{
    XSElement* xse = NULL;
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
        if ( m_Elements[i].name_ == name )
        {
            xse = &m_Elements[i];
            break;
        }
        
    return xse;
}

XSElement* XSComplexType::FindElementByParentType( const _tstring& parent_type )
{
    XSElement* xse = NULL;
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        if ( m_Elements[i].m_ParentTypeName == parent_type )
        {
            xse = &m_Elements[i];
            break;
        }
    }
    
    return xse;
}

// Collect the table names used in the complex-type object.
void XSComplexType::BuildTableNameList( std::vector<_tstring>& names, bool bCleanFirst )
{
    if ( bCleanFirst ) { names.clear(); }
    
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        XSElement& xse = m_Elements[i];
        
        if ( xse.IsSimpleType() )
        {
            //if(xse.IsIndexed())  // only check indexed elements
            {
                unsigned int j;
                
                for ( j = 0; j < names.size(); j++ )
                {
                    // table name already recorded
                    if ( names[j] == xse.m_TableName ) { break; }
                }
                
                // new table name, save it
                if ( j == names.size() ) { names.push_back( xse.m_TableName ); }
            }
        }
        else if ( xse.IsComplexType() )
        {
            xse.m_pComplexType->BuildTableNameList( names, false );
        }
    }
}

// Add an element into complex type
bool XSComplexType::AddElement( const XSElement& xse, bool bOverride )
{
    XSElement* p = GetElement( xse.name_ );
    
    if ( p != NULL  && bOverride ) { *p = xse; } // override
    else if ( p == NULL ) { m_Elements.push_back( xse ); } // add into list
    else { return false; }
    
    if ( m_pOwner ) { m_pOwner->ReInitComplexTypes(); }
    
    return true;
}

// Add an element into complex type
bool XSComplexType::DeleteElement( const _tstring& name )
{
    for ( unsigned int i = 0; i < m_Elements.size(); i++ )
    {
        if ( name == m_Elements[i].name_ )
        {
            m_Elements.erase( m_Elements.begin() + i );
            
            if ( m_pOwner ) { m_pOwner->ReInitComplexTypes(); }
            
            return true;
        }
    }
    
    return false;
}

//======================================================
//  class XSDocument
//======================================================
// Load schema from file
bool XSDocument::LoadFromFile( const char* filename )
{
    XMLNode xml;
    
    if ( xml.LoadFromFile( filename ) )
    {
        Serialize( xml, false );
        return true;
    }
    else { return false; }
}

// Save schema to file
bool XSDocument::SaveToFile( const char* filename )
{
    XMLNode xml;
    Serialize( xml, true );
    return xml.SaveToFile( filename );
}

// Serialization
void XSDocument::Serialize( XMLNode& xml, bool is_storing )
{
    unsigned int i;
    
    if ( is_storing )
    {
        xml.name_ = _T( "schema" );
        xml.child_nodes_.clear();
        
        for ( i = 0; i < m_ComplexTypes.size(); i++ )
        {
            XMLNode* node = new XMLNode();
            
            if ( m_ComplexTypes[i].Serialize( *node, true ) )
            {
                xml.child_nodes_.push_back( node );
            }
        }
    }
    else
    {
        m_ComplexTypes.clear();
        
        for ( i = 0; i < xml.child_nodes_.size(); i++ )
        {
            XMLNode* node = xml.child_nodes_[i];
            
            if ( node->name_ == _T( "complexType" ) )
            {
                XSComplexType xsc;
                
                if ( xsc.Serialize( *node, false ) && GetComplexType( xsc.name_ ) == NULL )
                {
                    m_ComplexTypes.push_back( xsc );
                }
            }
        }
        
        ReInitComplexTypes();
    }
}

// Get complex type object by name
XSComplexType* XSDocument::GetComplexType( const _tstring& name )
{
    XSComplexType* xsc = NULL;
    
    for ( unsigned int i = 0; i < m_ComplexTypes.size(); i++ )
        if ( name == m_ComplexTypes[i].name_ )
        {
            xsc = &m_ComplexTypes[i];
            break;
        }
        
    return xsc;
}

// Create the SELECT command by current XML's and schema's content.
bool XSDocument::CreateSelectCommand( XMLNode& xml, _tstring& cmd, bool bFirstSelect )
{
    bool retval = false;
    XSComplexType* xsc = GetComplexType( xml.name_ );
    cmd = _T( "" );
    
    if ( xsc )
    {
        OutStrStream os;
        // write field names
        os << _T( "SELECT " );
        int options = XSComplexType::foHasAlias | XSComplexType::foHasIndex;
        
        if ( bFirstSelect ) { options |= XSComplexType::foHasIdentity; }
        
        if ( xsc->m_TableNames.size() > 1 ) { options |= XSComplexType::foHasTableName; }
        
        int nFields = xsc->SaveFieldNamesToStream( os, options );
        // write table names
        os << _T( " FROM " );
        
        for ( unsigned int i = 0; i < xsc->m_TableNames.size(); i++ )
        {
            if ( i > 0 ) { os << _T( "," ); }
            
            os << xsc->m_TableNames[i].c_str();
        }
        
        if ( nFields > 0 && xsc->m_TableNames.size() > 0 )
        {
            cmd = os.str(); // we have columns, now check condition strings
            OutStrStream os1;
            os1 << _T( " WHERE " );
            options = XSComplexType::foHasIndex | XSComplexType::foHasTableName;
            
            if ( !bFirstSelect ) { options |= XSComplexType::foHasIdentity; }
            
            int nCons = xsc->SaveConditionsToStream( os1, xml, options );
            
            if ( nCons > 0 )
            {
                cmd += os1.str();
            }
            
            retval = true;
        }
    }
    
    return retval;
}

// Create the UPDATE command by current XML's and schema's content.
bool XSDocument::CreateUpdateCommand( XMLNode& xml, _tstring& cmd )
{
    int nUpdates = 0;
    XSComplexType* xsc = GetComplexType( xml.name_ );
    cmd = _T( "" );
    
    if ( xsc )
    {
        for ( unsigned int i = 0; i < xsc->m_TableNames.size(); i++ )
        {
            const TCHAR* table_name = xsc->m_TableNames[i].c_str();
            OutStrStream os;
            // write table name
            os << _T( "UPDATE " ) << table_name << _T( " SET " );
            // write field-value pairs
            int options = XSComplexType::foHasColumnName;
            int nFields = xsc->SaveFieldValuePairsToStream( os, xml, options, table_name );
            // write conditions
            os << _T( " WHERE " );
            options = XSComplexType::foHasIndex | XSComplexType::foHasTableName;
            int nCons = xsc->SaveConditionsToStream( os, xml, options, table_name );
            
            // check if the command is valid
            if ( nFields > 0 && nCons > 0 )
            {
                if ( nUpdates > 0 ) { cmd += _T( ";" ); }
                
                cmd += os.str();
                nUpdates++;
            }
        }
    }
    
    return nUpdates > 0;
}

// Create the INSERT command by current XML's and schema's content.
bool XSDocument::CreateInsertCommand( XMLNode& xml, _tstring& cmd )
{
    int nInserts = 0;
    XSComplexType* xsc = GetComplexType( xml.name_ );
    cmd = _T( "" );
    
    if ( xsc )
    {
        for ( unsigned int i = 0; i < xsc->m_TableNames.size(); i++ )
        {
            const TCHAR* table_name = xsc->m_TableNames[i].c_str();
            OutStrStream os;
            // write table name
            os << _T( "INSERT INTO " ) << table_name << _T( " (" );
            // write field names
            int options = XSComplexType::foHasIndex;
            int nFields = xsc->SaveFieldNamesToStream( os, options, table_name );
            // write values
            os << _T( ") VALUES (" );
            int nPairs = xsc->SaveFieldValuePairsToStream( os, xml, options, table_name );
            os << _T( ")" );
            
            // check if the command is valid
            if ( nFields > 0 && nPairs )
            {
                if ( nInserts > 0 ) { cmd += _T( ";" ); }
                
                cmd += os.str();
                nInserts++;
            }
        }
    }
    
    return nInserts > 0;
}

// Create the DELETE command by current XML's and schema's content.
bool XSDocument::CreateDeleteCommand( XMLNode& xml, _tstring& cmd )
{
    int nDeletes = 0;
    XSComplexType* xsc = GetComplexType( xml.name_ );
    cmd = _T( "" );
    
    if ( xsc )
    {
        for ( unsigned int i = 0; i < xsc->m_TableNames.size(); i++ )
        {
            const TCHAR* table_name = xsc->m_TableNames[i].c_str();
            OutStrStream os;
            // write table name
            os << _T( "DELETE FROM " ) << table_name << _T( " WHERE " );
            // write conditions
            int options = XSComplexType::foHasIndex | XSComplexType::foHasTableName;
            int nCons = xsc->SaveConditionsToStream( os, xml, options, table_name );
            
            // check if the command is valid
            if ( nCons > 0 )
            {
                if ( nDeletes > 0 ) { cmd += _T( ";" ); }
                
                cmd += os.str();
                nDeletes++;
            }
        }
    }
    
    return nDeletes > 0;
}

// Convert the flat XML from database to hierarchy-style XML.
bool XSDocument::ConvertFlatXML( XMLNode& flat, XMLNode& result )
{
    XSComplexType* xsc = GetComplexType( flat.name_ );
    
    if ( xsc )
    {
        result.name_ = xsc->name_;
        xsc->ConvertFlatXML( flat, result );
        return true;
    }
    else { return false; }
}

// Add a complex-type object into xsd.
bool XSDocument::AddComplexType( const XSComplexType& xsc, bool bOverride )
{
    XSComplexType* p = GetComplexType( xsc.name_ );
    
    if ( p != NULL  && bOverride ) { *p = xsc; }
    else if ( p == NULL ) { m_ComplexTypes.push_back( xsc ); }
    else { return false; }
    
    ReInitComplexTypes();
    return true;
}

// Delete a complex-type object by name.
bool XSDocument::DeleteComplexType( const _tstring& name )
{
    for ( unsigned int i = 0; i < m_ComplexTypes.size(); i++ )
    {
        if ( name == m_ComplexTypes[i].name_ )
        {
            m_ComplexTypes.erase( m_ComplexTypes.begin() + i );
            ReInitComplexTypes();
            return true;
        }
    }
    
    return false;
}

// re-init complex-type objects
void XSDocument::ReInitComplexTypes()
{
    unsigned int i;
    
    // reinitialize complex type for its elements...
    for ( i = 0; i < m_ComplexTypes.size(); i++ )
    {
        m_ComplexTypes[i].m_pOwner = this;
        m_ComplexTypes[i].ReInitElementTypes();
    }
    
    // rebuild table name list
    for ( i = 0; i < m_ComplexTypes.size(); i++ )
    {
        m_ComplexTypes[i].BuildTableNameList( m_ComplexTypes[i].m_TableNames, true );
    }
}
