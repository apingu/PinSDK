//////////////////////////////////////////////////////////
// File Name: xmlnode.h
//
// Author : Little Chio
// Created: 2002/7/15
// Last Modified : 2003/6/29
//
// Copyright (c) 2002-03, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __XMLNODE_H__
#define __XMLNODE_H__

#include <pf-def.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

typedef std::istream InStream;
typedef std::ostream OutStream;
typedef std::istringstream InStrStream;
typedef std::ostringstream OutStrStream;
#define COut  std::cout

//#include <list>
//#include <CharCode.h>

///////////////////////////////////////////////////////////////////
/// class XMLNode is a XML-parser and stores the pairs of 'name'
/// and 'text' in hierarchy structures. It is useful for serialization
/// because most of the data types can be represented in string
/// form. So most of Pyxis class use XMLNode as data stream.
///////////////////////////////////////////////////////////////////

typedef char          xnchar_t;
typedef std::string   xnstring_t;


class XMLAttribute
{
public:
    xnstring_t name_;    /**< The attribute's name. */
    xnstring_t value_;   /**< The attribute's value */
};

class XMLNode
{

public:

    XMLNode();
    ~XMLNode();
    
    /** Load the node from the in-stream object. */
    bool LoadFromStream( InStream& ins );
    
    /** Save the node to the out-stream object. */
    void SaveToStream( OutStream& outs, int indent = 0 ) const;
    
    /** Load the node from a file */
    bool LoadFromFile( const xnchar_t* filename );
    
    /** Save the node to a file. */
    bool SaveToFile( const xnchar_t* filename ) const;
    
    /**
     * Get the reference of the child node by name.
     * @param name The child node's name
     * @return Return the reference of node we found. If there's no match node
     * in the child list, a new child node will be added and return its
     * reference.
     */
    XMLNode& operator[]( const xnstring_t& name );
    //XMLNode* operator[](const xnstring_t& name);
    
    
    /**
      * Find a child node by name.
      * @param name The child node's name.
      * @return Return NULL if there's no match node found.
      */
    XMLNode* FindNode( const xnstring_t& name );
    
    
    /**
     * Find a child node by index.
     * @param idx The child node's index.
     * @return Return NULL if there's no match node found.
     */
    XMLNode* FindNode( int idx );
    
    
    /*
    * Find first node
    * @param name The child node's name. if is NULL, it will return the first node in child list
    * @return First child node
    */
    XMLNode* FindFirstNode( const xnchar_t* name );
    
    
    /**
     * Find the next child node.
     * @return Return NULL if no more child node after last found node.
     */
    XMLNode* FindNextNode( const xnchar_t* name );
    int      GetFindNodeIndex() { return last_node_index_; }


     /**
     * parse all childnode.
     */
    typedef bool ( _CALLBACK *OnChildNode )( XMLNode* node, void* data );
    void     ForAllNode( OnChildNode func, void* data );

    
    /**
    * Find attribute of node.
    * @return Return NULL if can't fine attribute.
    */
    XMLAttribute* FindAttribute( const xnstring_t& name );
    
    XMLAttribute* FindFirstAttribute( const xnchar_t* name );
    
    XMLAttribute* FindNextAttribute( const xnchar_t* name );
    
    unsigned int  GetNodeSize( const xnchar_t* name = NULL );
    int           GetFindAttributeIndex() { return last_attrbute_index_; }
    
    
    
    xnstring_t                  name_;        /**< The node's name. */
    xnstring_t                  text_;        /**< The node's text. */
    xnstring_t                  comment_;     /**< The node's comment. */
    std::vector<XMLNode*>       child_nodes_;  /**< The array of child nodes. */
    
private:

    std::vector<XMLAttribute*>  attriutes_;
    
    
    bool       ReadTag( InStream& ins, xnstring_t& name, bool& isbegin, bool& isEnd );
    bool       ReadAttribute( xnstring_t name );
    xnstring_t Decode( const xnstring_t& str ) const;
    xnstring_t Encode( const xnstring_t& str ) const;
    
    xnstring_t header_;
    int        last_node_index_;
    int        last_attrbute_index_;
};

///////////////////////////////////////////////////////////////////
/// special version of operator<< & >> for string
///////////////////////////////////////////////////////////////////
inline xnstring_t& operator<<( xnstring_t& str, const xnstring_t& value )
{
    return str = value;
}

inline xnstring_t& operator<<( xnstring_t& str, const xnchar_t* value )
{
    return str = value;
}

inline xnstring_t operator>>( const xnstring_t& str, xnstring_t& value )
{
    value = str;
    return str;
}

///////////////////////////////////////////////////////////////////
/// template for output value to string
///////////////////////////////////////////////////////////////////
template<class T>xnstring_t& operator<<( xnstring_t& str, const T& value )
{
    OutStrStream os;
    os << value;
    if( str.size() ) { str += _T( ' ' ); }
    return str += os.str();
}

///////////////////////////////////////////////////////////////////
/// template for input value from string
///////////////////////////////////////////////////////////////////
template<class T>xnstring_t operator>>( const xnstring_t& str, T& value )
{
    InStrStream is( str );
    is >> value;
    return xnstring_t( str.c_str() + ( int )is.tellg() );
}

#endif // end of __XMLNODE_H__

