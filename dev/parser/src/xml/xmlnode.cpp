//////////////////////////////////////////////////////////
// File Name: xmlnode.cpp
//
// Author : Little Chio
// Created : 2002/7/15
// Last Modified : 2003/6/29
//
// Copyright (c) 2002-03, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include "xmlnode.h"

#ifdef _WINDOWS
    //#include <Windows.h>
#endif
#ifdef _LINUX
#include <pf-stdlib.h>  //GetTickCount
#endif
//int t1=0;
//int t2=0;
//int t3=0;
//int t4=0;
//int t5=0;
//int t6=0;
//int t7=0;
//int t8=0;
//int t9=0;


//int a0=0;
//int a1=0;
//int a2=0;
//int a3=0;
//int a4=0;
//int a5=0;

//===========================
//  class XMLNode
//===========================

XMLNode::XMLNode()
{
    header_ = _T( "?xml" );
    last_node_index_ = -1;
    last_attrbute_index_ = -1;
}

XMLNode::~XMLNode()
{
    for ( size_t i = 0; i < child_nodes_.size(); i++ )
    {
        delete child_nodes_[ i ];
    }

    child_nodes_.clear();

    for ( size_t i = 0; i < attriutes_.size(); i++ )
    {
        delete attriutes_[ i ];
    }

    attriutes_.clear();
}

bool XMLNode::ReadAttribute( xnstring_t name )
{
    xnchar_t* ptempname = new xnchar_t[ name.size() + 1 ];
    strcpy( ptempname, name.c_str() );

    if ( name[ name.size() - 1 ] == _T( '/' ) )
    {
        ptempname[ name.size() - 1 ] = 0;
    }

    xnchar_t* p = strtok( ptempname, " =\"\r\n" );

    if ( p != NULL )
    {
        name_ = ptempname;
    }

    while ( p = strtok( NULL, " =" ) )
    {
        XMLAttribute* attri = new XMLAttribute();

        if ( p )
        {
            attri->name_ = p;
        }

        // get attribue value
        p = strtok( NULL, "\"'" );

        if ( p )
        {
            attri->value_ = p;
        }

        attriutes_.push_back( attri );
    }

    delete[] ptempname;
    return true;
}

// Read XML's tag from stream. 'isbegin' indicates if the tag is begin tag or end tag
bool XMLNode::ReadTag( InStream& ins, xnstring_t& name, bool& isbegin, bool& isEnd )
{
    //longlong_t ll1 = GetTickCount();
    TCHAR buffer[ 512 ];
    xnstring_t tag_name;
    isEnd = false;

    do
    {
        ins.clear();
        ins.getline( buffer, sizeof( buffer ), _T( '<' ) ); // try to get tag's mark '<'
    } while ( ins.fail() && !ins.eof() );

    do
    {
        buffer[ 0 ] = 0;
        ins.clear();
        ins.getline( buffer, sizeof( buffer ), _T( '>' ) ); // try to get tag's mark '>'
        tag_name += buffer;
    } while ( ins.fail() && !ins.eof() );

    int taglen = tag_name.size();

    if ( ins.good() && taglen > 0 )
    {
        if ( ( taglen > 4 ) && memcmp( tag_name.c_str(), header_.c_str(), header_.size() ) == 0 ) // check if it's XML header
        {
            return ReadTag( ins, name, isbegin, isEnd ); // skip the header and read the next node
        }
        else if ( ( taglen ) > 4 && memcmp( tag_name.c_str(), _T( "!--" ), 3 ) == 0 ) // check if it's comment
        {
            while ( tag_name[ taglen - 2 ] != '-' || tag_name[ taglen - 1 ] != '-' )
            {
                do
                {
                    buffer[ 0 ] = 0;
                    ins.clear();
                    ins.getline( buffer, sizeof( buffer ), _T( '>' ) ); // try to get tag's mark '>'
                    tag_name += buffer;
                } while ( ins.fail() && !ins.eof() );

                taglen = tag_name.size();
            }

            comment_.append( tag_name, 3, taglen - 5 );
            return ReadTag( ins, name, isbegin, isEnd ); // read the next node
        }
        else if ( tag_name[ 0 ] == _T( '/' ) ) // check if it's node's end
        {
            name = tag_name.c_str() + 1;
            isbegin = false;
        }
        else if ( tag_name[ taglen - 1 ] == _T( '/' ) ) // check if it's node's immediate end
        {
            xnchar_t* endbuffer = new xnchar_t[ tag_name.size() + 1 ];
            strcpy( endbuffer, tag_name.c_str() );
            endbuffer[ taglen - 1 ] = 0;
            name = endbuffer;
            delete[] endbuffer;
            isbegin = true;
            isEnd = true;
            //return ReadTag(ins, name, isbegin); // skip this node and read the next one
        }
        else       // otherwise, it should be the node's begin
        {
            name = tag_name;
            isbegin = true;
        }

        return true;
    }

    return false;
}



//#include <windows.h>
// Read the node from stream
bool XMLNode::LoadFromStream( InStream& ins )
{
    //xnchar_t buff[512];
    //long64_t bt = 0;
    //long64_t et = 0;
    //longlong_t ll1 = GetTickCount();
    //long64_t bn = 0;
    //long64_t en = 0;
    //bn = GetTickCount();
    //_stprintf(buff, "beginN %lld\n", bn);
    //PfOutputDebugString(buff);
    name_ = text_ = comment_ = _T( "" );

    // clear all members first
    for ( size_t i = 0; i < child_nodes_.size(); i++ )
    {
        delete child_nodes_[ i ];
    }

    child_nodes_.clear();

    for ( size_t i = 0; i < attriutes_.size(); i++ )
    {
        delete attriutes_[ i ];
    }

    attriutes_.clear();
    xnstring_t name;
    bool begin_tag;
    bool end_tag;
    int prev_pos = ins.tellg();  // push the current stream's position

    //bt = GetTickCount();
    //_stprintf(buff, "begin0 %lld\n", bt);
    //PfOutputDebugString(buff);
    //t1 += GetTickCount()-ll1;

    //ll1 = GetTickCount();
    if ( ReadTag( ins, name, begin_tag, end_tag ) && begin_tag ) // read tag's begin first
    {
        //et = GetTickCount();
        //_stprintf(buff, "End0 %lld  => %lld\n", et, et-bt);
        //PfOutputDebugString(buff);
        ReadAttribute( name );

        // single tag and immediate end
        if ( end_tag )
        {
            return true;
        }

        XMLNode* node = new XMLNode();

        while ( node->LoadFromStream( ins ) ) // try to retrive the child nodes
        {
            child_nodes_.push_back( node );
            node = new XMLNode();
        }

        delete node;
        //ll1 = GetTickCount();
        int data_start = ins.tellg();

        //std::string endtag = '/'+name_;
        // find ending tag
        //bt = GetTickCount();
        //_stprintf(buff, "begin3 %lld\n", bt);
        //PfOutputDebugString(buff);

        if ( ReadTag( ins, name, begin_tag, end_tag ) && name == name_ && !begin_tag )
        {
            //t5 += GetTickCount()-ll1;
            if ( child_nodes_.size() == 0 ) // end node, so we should catch the node's text
            {
                //ll1 = GetTickCount();
                int cur_pos = ins.tellg();
                //int data_len = cur_pos - data_start - name.size() - 3;
                int data_len = cur_pos - data_start - name_.size() - 3; // '/'
                TCHAR* text = new TCHAR[ data_len + 1 ];
                ins.seekg( data_start );
                ins.read( text, data_len ); // read the node's text
                ins.seekg( cur_pos );
                text[ data_len ] = 0;
                text_ = Decode( text ); // decode the text
                delete[] text;
                //t6 += GetTickCount()-ll1;
            }
        }

        //t4 += GetTickCount() - ll1;
        //t8 += GetTickCount() - ll2;
        return true;
        //et = GetTickCount();
        //_stprintf(buff, "End3 %lld  => %lld\n", et, et-bt);
        //PfOutputDebugString(buff);
    }

    //ll1 = GetTickCount();
    ins.seekg( prev_pos );
    //t7 += GetTickCount()-ll1;
    //en = GetTickCount();
    //_stprintf(buff, "EndN %lld  => %lld\n", en, en-bn);
    //PfOutputDebugString(buff);
    return false;
}

// Write the node to stream
void XMLNode::SaveToStream( OutStream& outs, int indent ) const
{
    // prepare the indent string
    //TCHAR* indent_str = new TCHAR[indent+1];

    //for(int i = 0; i< indent; i++) indent_str[i] = _T(' ');
    //indent_str[indent] = 0;

    // write out the comment first
    if ( comment_.size() != 0 )
    {
        std::string indstr;
        indstr = _T( "<!--" ) + comment_ + _T( "-->\r\n" );
        outs << indstr.c_str();
        //outs << indent_str << _T("<!--") + comment_ + _T("-->\r\n");
    }

    xnstring_t nodename;
    nodename = name_;

    for ( unsigned int i = 0; i < attriutes_.size(); i++ )
    {
        // NodeA name="AAA"
        nodename = nodename + _T( " " ) + attriutes_[ i ]->name_ + _T( "=" ) + _T( "\"" ) + attriutes_[ i ]->value_ + _T( "\"" );
    }

    if ( child_nodes_.size() != 0 ) // it's the way we output the node with child nodes
    {
        std::string indstr;
        indstr = _T( "<" ) + nodename + _T( ">\r\n" );
        outs << indstr.c_str();
        //outs << indent_str << _T("<") + nodename + _T(">\r\n");

        for ( unsigned int i = 0; i < child_nodes_.size(); i++ )
        {
            child_nodes_[ i ]->SaveToStream( outs, indent + 2 );
        }

        indstr = _T( "</" ) + name_ + _T( ">\r\n" );
        outs << indstr.c_str();
        //outs << indent_str << _T("</") + name_ + _T(">\r\n");
    }
    else
    {
        std::string indstr;
        indstr = _T( "<" ) + nodename + _T( ">" ) + Encode( text_ ) + _T( "</" ) + name_ + _T( ">\r\n" );
        outs << indstr.c_str();
        //outs << indent_str << _T("<") + nodename + _T(">") + Encode(text_) + _T("</") + name_ + _T(">\r\n");
    }

    //delete [] indent_str;
}

// Decode the XML-style string into plain text
xnstring_t XMLNode::Decode( const xnstring_t& str ) const
{
    std::string::size_type pos = 0;
    xnstring_t text = str;

    while ( ( pos = text.find( _T( "&lt;" ), pos ) ) != text.npos )
    {
        text.replace( pos, 4, _T( "<" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( "&gt;" ), pos ) ) != text.npos )
    {
        text.replace( pos, 4, _T( ">" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( "&amp;" ), pos ) ) != text.npos )
    {
        text.replace( pos, 5, _T( "&" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( "&quot;" ), pos ) ) != text.npos )
    {
        text.replace( pos, 6, _T( "\"" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( "&nbsp;" ), pos ) ) != text.npos )
    {
        text.replace( pos, 6, _T( " " ) );
    }

    return text;
}

// Encode the plain text into the XML-style string
xnstring_t XMLNode::Encode( const xnstring_t& str ) const
{
    unsigned int pos = 0;
    xnstring_t text = str;

    while ( ( pos = text.find( _T( "&" ), pos ) ) != text.npos )
    {
        text.replace( pos, 1, _T( "&amp;" ) );
        pos += 4;
    }

    pos = 0;

    while ( ( pos = text.find( _T( "<" ), pos ) ) != text.npos )
    {
        text.replace( pos, 1, _T( "&lt;" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( ">" ), pos ) ) != text.npos )
    {
        text.replace( pos, 1, _T( "&gt;" ) );
    }

    pos = 0;

    while ( ( pos = text.find( _T( "\"" ), pos ) ) != text.npos )
    {
        text.replace( pos, 1, _T( "&quot;" ) );
    }

    return text;
}

// Load XML from file
bool XMLNode::LoadFromFile( const xnchar_t* filename )
{
    //longlong_t ll0 = GetTickCount();
    FILE* fp = fopen( filename, "rb" );

    if ( fp )
    {
        fseek( fp, 0, SEEK_END );
        int size = ftell( fp );
        TCHAR* buf = new TCHAR[ size + 1 ];
        fseek( fp, 0, SEEK_SET );
        fread( buf, 1, size, fp );
        buf[ size ] = 0;
        fclose( fp );
        InStrStream iss( buf );
        LoadFromStream( iss );
        delete[] buf;
        //longlong_t ttt = GetTickCount() - ll0;
        return true;
    }

    return false;
}

// Save XML to file
bool XMLNode::SaveToFile( const xnchar_t* filename ) const
{
    FILE* fp = fopen( filename, "wb" );

    if ( fp )
    {
        OutStrStream outs;
        // write out the header first
        outs << _T( "<?xml version='1.0'?>\r\n" );
        SaveToStream( outs );
#ifdef _UNICODE
        // write out the header of unicoded file
        int header = 0xfeff;
        fwrite( &header, 1, 2, fp );
#endif
        fwrite( outs.str().c_str(), sizeof( TCHAR ), outs.tellp(), fp );
        fclose( fp );
        return true;
    }

    return false;
}

// Reference the child nodes by name
// gcc 上這樣做會有問題
XMLNode& XMLNode::operator[]( const xnstring_t& name )
{
    // find the node by name
    for ( unsigned int i = 0; i < child_nodes_.size(); i++ )
    {
        if ( child_nodes_[ i ]->name_ == name )
        {
            return ( *child_nodes_[ i ] );
        }
    }

    // we should create a new node if no match node
    XMLNode* node = new XMLNode();
    node->name_ = name;
    child_nodes_.push_back( node );
    return ( *child_nodes_.back() );
}

//XMLNode* XMLNode::operator[](const xnstring_t& name)
//{
//    // find the node by name
//    for (unsigned int i = 0; i < child_nodes_.size(); i++)
//    {
//        if (child_nodes_[i]->name_ == name)
//        {
//            return child_nodes_[i];
//        }
//    }
//
//    // we should create a new node if no match node
//    XMLNode* node = new XMLNode();
//    node->name_ = name;
//    child_nodes_.push_back(node);
//    return child_nodes_.back();
//}


// Find child node by name. Return 0 if no match node found.
XMLNode* XMLNode::FindNode( const xnstring_t& name )
{
    last_node_index_ = -1;   // reset the index

    for ( unsigned int i = 0; i < child_nodes_.size(); i++ )
    {
        if ( child_nodes_[ i ]->name_ == name )
        {
            last_node_index_ = i;
            return child_nodes_[ i ];
        }
    }

    return FindFirstNode( name.c_str() );
}

// Find child node by index. Return 0 if no match node found.
XMLNode* XMLNode::FindNode( int idx )
{
    last_node_index_ = -1;   // reset the index

    if ( idx >= child_nodes_.size() )
    {
        return NULL;
    }

    last_node_index_ = idx;
    return child_nodes_[ idx ];
}

XMLNode* XMLNode::FindFirstNode( const xnchar_t* name )
{
    last_node_index_ = -1;   // reset the index
    if ( name )
    {
        for ( unsigned int i = 0; i < child_nodes_.size(); i++ )
        {
            //if(child_nodes_[i].name_==name)
            if ( strcmp( child_nodes_[ i ]->name_.c_str(), name ) == 0 )
            {
                last_node_index_ = i;
                return child_nodes_[ i ];
            }
        }
    }
    else
    {
        if ( !child_nodes_.empty() )
        {
            last_node_index_ = 0;
            return child_nodes_[ last_node_index_ ];
        }
    }

    return NULL;
}


// Find next child node if there're more child nodes with the same name
XMLNode* XMLNode::FindNextNode( const xnchar_t* name )
{
    if ( name )
    {
        // find the node by name
        for ( unsigned int i = last_node_index_ + 1; i < child_nodes_.size(); i++ )
        {
            if ( strcmp( child_nodes_[ i ]->name_.c_str(), name ) == 0 ) //==child_nodes_[last_node_index_].name_)
            {
                last_node_index_ = i;
                return child_nodes_[ i ];
            }
        }
        //last_node_index_ = -1;
    }
    else
    {
        if ( ( last_node_index_ + 1 ) < (int)child_nodes_.size() )
        {
            last_node_index_++;
            return child_nodes_[ last_node_index_ ];
        }
    }

    last_node_index_ = -1;
    return NULL;
}

void XMLNode::ForAllNode( OnChildNode func, void* data )
{
    if ( !func( this, data ) )
        return;

    for ( uint_t ni = 0; ni < child_nodes_.size(); ni++ )
    {
        child_nodes_[ ni ]->ForAllNode( func, data );
    }
}


XMLAttribute* XMLNode::FindAttribute( const xnstring_t& name )
{
    return FindFirstAttribute( name.c_str() );
}


XMLAttribute* XMLNode::FindFirstAttribute( const xnchar_t* name )
{
    last_attrbute_index_ = -1;   // reset the index

    if ( name )
    {
        for ( unsigned int i = 0; i < attriutes_.size(); i++ )
        {
            if ( strcmp( attriutes_[ i ]->name_.c_str(), name ) == 0 )
            {
                last_attrbute_index_ = i;
                return attriutes_[ i ];
            }
        }
    }
    else
    {
        if ( !child_nodes_.empty() )
        {
            last_attrbute_index_ = 0;
            if ( !attriutes_.empty() )
            {
                return attriutes_[ last_attrbute_index_ ];
            }
        }
    }

    return NULL;
}

XMLAttribute* XMLNode::FindNextAttribute( const xnchar_t* name )
{
    if ( name )
    {
        // find the node by name
        for ( unsigned int i = last_attrbute_index_ + 1; i < (int)attriutes_.size(); i++ )
        {
            if ( strcmp( attriutes_[ i ]->name_.c_str(), name ) == 0 ) //==child_nodes_[last_node_index_].name_)
            {
                last_attrbute_index_ = i;
                return attriutes_[ i ];
            }
        }

        //last_node_index_ = -1;
    }
    else
    {
        if ( ( last_attrbute_index_ + 1 ) < (int)attriutes_.size() )
        {
            last_attrbute_index_++;
            if ( last_attrbute_index_ < attriutes_.size() )
            {
                return attriutes_[ last_attrbute_index_ ];
            }
        }
    }

    last_attrbute_index_ = -1;
    return NULL;
}

unsigned int XMLNode::GetNodeSize( const xnchar_t* name )
{
    if ( name == NULL )
    {
        return child_nodes_.size();
    }

    int cnt = 0;

    for ( size_t i = 0; i < child_nodes_.size(); i++ )
    {
        if ( strcmp( name, child_nodes_[ i ]->name_.c_str() ) == 0 )
        {
            cnt++;
        }
    }

    return cnt;
}