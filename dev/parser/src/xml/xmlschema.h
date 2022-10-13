//////////////////////////////////////////////////////////
// File Name: xmlschema.h
//
// Author : Little Chio
// File created : 2003/5/9
// Last modified : 2003/6/29
//
// Copyright (c) 2003, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __XMLSCHEMA_H__
#define __XMLSCHEMA_H__

#include "xmlnode.h"

class XSComplexType;
class XSDocument;

/**
* class XSElement
* This class describes a form of values how to map into database. It's the base
* unit in XML schema with simple types, or it can be referenced to complex types which
* are defined by multiple simple-type elements.
*/
class XSElement
{
public:

    /** Enumerations of simple types. */
    enum SimpleType
    {
        stString,    /**< _tstring type. */
        stByte,         /**< Byte type or one character. */
        stBinary,       /**< Binary type, or byte array. */
        stInteger,      /**< Integer type. */
        stFloat,        /**< Float type. */
        stBoolean,      /**< Boolean type with value 0 or 1. */
        stDateTime,     /**< DateTime type, in form of yy/mm/dd hh:mm:ss. */
        stCurrency,     /**< Currency (money) type. */
        stIdentity,     /**< Integer type for identification. */
        stUnknown       /**< Unsupported type, or a complex type. */
    };
    
    /** Constructor. */
    XSElement();
    
    /** Copy constructor. */
    XSElement( const XSElement& rhs );
    
    /** Constructor by given parameters. */
    XSElement( const _tstring& name, const _tstring& field,
               const _tstring& type_name, bool is_index );
               
    /** Copy operator. */
    XSElement& operator=( const XSElement& rhs );
    
    /**
     * Serialization. Import or export element by XML.
     * @param xml The source or destination XMLNode.
     * @param is_storing The flag for saving or loading element into/from the XMLNode.
     * @return True if serializaiton is succeeded.
     */
    bool Serialize( XMLNode& xml, bool is_storing );
    
    /**
     * Convert string of type name to SimpleType value.
     * @param type_name The string of type name which is going to be converted.
     * @return Return enum SimpleType. If type name is invalid, stUnknown will be returned.
     */
    static SimpleType StringToSimpleType( const _tstring& type_name );
    
    /**
     * Convert enum SimpleType to type name.
     * @param type enum SimpleType which is going to be converted.
     * @return Return the string of type name.
     */
    static const TCHAR* SimpleTypeToString( SimpleType type );
    
    /**
     * Convert XMLNode to the string in the form of 'field=value' pair. The field part
     * is taken from m_FieldName, and the value part is from text_ of XMLNode.
     * @param node The source XMLNode to be extracted.
     * @param result The destination string receiving the field-value pair.
     * @param bNoField The flag determines if the field part will be appended.
     * @return True if the conversion is succeeded.
     * @note The string text_ of XMLNode will be converted by the element's type
     * to match the value format of database.
     */
    bool NodeToString( const XMLNode& node, _tstring& result, bool bNoField = false );
    
    /** Get the name of element. */
    const TCHAR* GetName() const { return name_.c_str(); }
    
    /**
     * Get the field name of element. The field name are supposed to include the table
     * name and column name of database, in the form of 'table_name.column_name'.
     */
    const TCHAR* GetFieldName() const { return m_FieldName.c_str(); }
    
    const TCHAR* GetJoinFieldName() const { return m_JoinFieldName.c_str(); }
    
    /** Get the table name according to the field name. */
    const TCHAR* GetTableName() const { return m_TableName.c_str(); }
    
    /** Get the column name according to the field name. */
    const TCHAR* GetColumnName() const { return m_pColumnName; }
    
    /** Get the type name of element. */
    const TCHAR* GetTypeName() const { return m_TypeName.c_str(); }
    
    /** Get the simple type of element. */
    SimpleType GetSimpleType() const { return m_SimpleType; }
    
    /** Check if the element is simple type or not. */
    bool IsSimpleType() const { return m_SimpleType != stUnknown && m_pComplexType == NULL; }
    
    /** Check if the element is complex type or not. */
    bool IsComplexType() const { return m_pComplexType != NULL ; }
    
    /** Check if the element is indexed in database. */
    bool IsIndexed() const { return m_bIndex; }
    
    bool IsJoined() const { return m_JoinFieldName.size() > 0; }
    
    /** Get the complex-type object if the element is a complex type. */
    XSComplexType* GetComplexType() const { return m_pComplexType; }
    
    /** Check if the element has parent relationship with another type. */
    bool HasParent() const { return m_pParent != NULL ; }
    
    /** Get the element's parent. */
    XSElement* GetParent() const { return m_pParent; }
    
private:

    // Build table/column name by field name.
    bool BuildTableName();
    
    _tstring name_;
    _tstring m_FieldName;
    _tstring m_TableName;
    const TCHAR* m_pColumnName;
    _tstring m_JoinFieldName;
    _tstring m_TypeName;
    SimpleType m_SimpleType;
    XSComplexType* m_pComplexType;
    _tstring m_ParentTypeName;
    _tstring m_ParentKeyName;
    XSElement* m_pParent;
    bool m_bIndex;
    
    static const TCHAR* m_SimpleTypeNames[];
    friend class XSComplexType;
};

/**
* class XSComplexType
* The class is a collection of multiple elements and describes how a object stored in
* the form of database. Users can use it to retrieve SQL commands to access database
* and convert the data from database to XML form.
*/
class XSComplexType
{
public:

    /** Options for displaying fields. */
    enum FieldOption
    {
        foHasAlias = 0x01, /**< Add alias after each field. */
        foHasIndex = 0x02, /**< Show indexed fields. */
        foHasIdentity = 0x04,   /**< Show the fields with identity type. */
        foHasColumnName = 0x08, /**< Show column name. */
        foHasTableName = 0x10  /**< Show full field name (with table name).*/
    };
    
    
    /** Constructor. */
    XSComplexType();
    
    /** Constructor by name. */
    XSComplexType( const _tstring& name );
    
    /**
     * Serialization. Import or export complex-type by XML.
     * @param xml The source or destination XMLNode.
     * @param is_storing The flag for saving or loading elements into/from the XMLNode.
     * @return True if serializaiton is succeeded.
     */
    bool Serialize( XMLNode& xml, bool is_storing );
    
    /**
     * Save all the field names of elements to stream.
     * @param outs The destination stream.
     * @param options The flag determines how to output the fields. See enum FieldOptions.
     * @param filter The table name to filter the elements. If NULL, then any element will pass.
     * @param nFields The number of fields saved to the stream.
     * @return Return the total number of fields saved to the stream.
     */
    int SaveFieldNamesToStream( OutStream& outs, int options, const TCHAR* filter = NULL, int nFields = 0 );
    
    /**
     * Save all the condition strings (from indexed elements) to stream.
     * @param outs The destination stream.
     * @param xml The source XMLNode which stores the values corresponded to the elements.
     * @param filter The table name to filter the elements. If NULL, then any element will pass.
     * @param nCons The number of condition strings saved to the stream.
     * @return Return the total number of strings saved to the stream.
     */
    int SaveConditionsToStream( OutStream& outs, XMLNode& xml, int options, const TCHAR* filter = NULL, int nCons = 0 );
    
    /**
     * Save all the field-value pairs from the elements to stream.
     * @param out The destination stream.
     * @param xml The source XMLNode which stores the values corresponded to the elements.
     * @param options The flag determines how to output the fields. See enum FieldOptions.
     * @param filter The table name to filter the elements. If NULL, then any element will pass.
     * @param nFields The number of fields saved to the stream.
     * @return Return the total number of pair strings to the stream.
     */
    int SaveFieldValuePairsToStream( OutStream& outs, XMLNode& xml, int options, const TCHAR* filter = NULL, int nFields = 0 );
    
    /**
     * Convert the flat XML from database to hierarchy-style XML.
     * @param flat The flat XMLNode.
     * @param result The destination XMLNode which will be converted in hierarchy form.
     */
    void ConvertFlatXML( XMLNode& flat, XMLNode& result );
    
    /**
     * Re-initialize the type of elements. It is supposed to be called after all the
     * complex-types are loaded.
     */
    void ReInitElementTypes();
    
    /** Collect the table names used in the complex-type object. */
    void BuildTableNameList( std::vector<_tstring>& names, bool bCleanFirst );
    
    /**
     * Add an element into complex-type object.
     * @param xse The element which is going to be added.
     * @param bOverride The flag determines if overrides an existed element with the same name.
     * @return True if the element is added successfully.
     */
    bool AddElement( const XSElement& xse, bool bOverride );
    
    /**  Delete an element from complex-type by name. */
    bool DeleteElement( const _tstring& name );
    
    /** Get number of elements in complex-type objects. */
    int GetElementCount() const { return (int)m_Elements.size(); }
    
    /** Get the element object by name. */
    XSElement* GetElement( const _tstring& name );
    
    /** Get the element which has relationship with given parent type. */
    XSElement* FindElementByParentType( const _tstring& parent_type );
    
    /** Get the name of the complex-type object. */
    const TCHAR* GetName() const { return name_.c_str(); }
    
private:

    _tstring name_;
    std::vector<XSElement> m_Elements;
    std::vector<_tstring> m_TableNames;
    XSDocument* m_pOwner;
    
    friend class XSDocument;
};

/**
* class XSDocument
* This class represents a schema which defines multiple complex types and provides a way
* of mapping objects between XML and database.
*/
class XSDocument
{
public:

    /**
     * Load XML schema from file.
     * @param filename The file name of XML Schema (.xsd)
     * @return True if the schema is loaded successfully.
     */
    bool LoadFromFile( const char* filename );
    
    /**
     * Save XML schema to file.
     * @param filename The file name of XML Schema (.xsd)
     * @return True if the schema is saved successfully.
     */
    bool SaveToFile( const char* filename );
    
    /**
     * Serialization. Import or export the schema by XML.
     * @param xml The source or destination XMLNode.
     * @param is_storing The flag for saving or loading complex-types into/from the XMLNode.
     * @return True if serializaiton is succeeded.
     */
    void Serialize( XMLNode& xml, bool is_storing );
    
    /**
     * Create the SELECT command by current XML's and schema's content.
     * @param xml The source XMLNode giving the indexed informations.
     * @param cmd The destination string receiving the SELECT command.
     * @param bFirstSelect True for the first call after inserting data.
     * @return True if a valid SELECT command is returned.
     */
    bool CreateSelectCommand( XMLNode& xml, _tstring& cmd, bool bFirstSelect = false );
    
    /**
     * Create the UPDATE command by current XML's and schema's content.
     * @param xml The source XMLNode storing the values to update.
     * @param cmd The destination string receiving the UPDATE command.
     * @return True if a valid UPDATE command is returned.
     */
    bool CreateUpdateCommand( XMLNode& xml, _tstring& cmd );
    
    /**
     * Create the INSERT command by current XML's and schema's content.
     * @param xml The source XMLNode storing the values to insert into database.
     * @param cmd The destination string receiving the INSERT command.
     * @return True if a valid INSERT command is returned.
     */
    bool CreateInsertCommand( XMLNode& xml, _tstring& cmd );
    
    /**
     * Create the DELETE command by current XML's and schema's content.
     * @param xml The source XMLNode storing the indexed values for deleting from database.
     * @param cmd The destination string receiving the DELETE command.
     * @return True if a valid DELETE command is returned.
     */
    bool CreateDeleteCommand( XMLNode& xml, _tstring& cmd );
    
    /**
     * Convert the flat XML from database to hierarchy-style XML.
     * @param flat The flat XMLNode.
     * @param result The destination XMLNode which will be converted in hierarchy form.
     * @return True if the flat XML is converted to hierarchy XML successfully.
     */
    bool ConvertFlatXML( XMLNode& flat, XMLNode& result );
    
    /**
     * Add a complex-type object into xsd.
     * @param xsc The complex-type object which is going to be added.
     * @param bOverride The flag determines if overrides an existed xsc with the same name.
     * @return True if the object is added successfully.
     */
    bool AddComplexType( const XSComplexType& xsc, bool bOverride );
    
    /** Delete a complex-type object by name. */
    bool DeleteComplexType( const _tstring& name );
    
    /** Re-initialize all complex-type objects. */
    void ReInitComplexTypes();
    
    /** Get number of complex types in document. */
    int GetComplexTypeCount() const { return m_ComplexTypes.size(); }
    
    /** Get the complex-type object by name. */
    XSComplexType* GetComplexType( const _tstring& name );
    
private:

    std::vector<XSComplexType> m_ComplexTypes;
};

#endif
