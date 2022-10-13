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

#include "CharCode.h"
#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////
/// class XMLNode is a XML-parser and stores the pairs of 'name'
/// and 'text' in hierarchy structures. It is useful for serialization
/// because most of the data types can be represented in string 
/// form. So most of Pyxis class use XMLNode as data stream.
///////////////////////////////////////////////////////////////////


class XMLAttribute
{
public:
	basestring m_Name;    /**< The attribute's name. */
	basestring m_Value;   /**< The attribute's value */
};

class XMLNode
{

public:

     /** Load the node from the in-stream object. */
	 bool LoadFromStream(InStream &ins);

	 /** Save the node to the out-stream object. */
     void SaveToStream(OutStream &outs, int indent=0) const;

	 /** Load the node from a file */
     bool LoadFromFile(const char *filename);

	 /** Save the node to a file. */
     bool SaveToFile(const char *filename) const;

	 /** 
	  * Get the reference of the child node by name.
	  * @param name The child node's name
	  * @return Return the reference of node we found. If there's no match node
	  * in the child list, a new child node will be added and return its
	  * reference.
	  */
     XMLNode& operator[](const basestring &name);


     /**
	  * Find a child node by name.
	  * @param name The child node's name.
	  * @return Return NULL if there's no match node found.
	  */
	 XMLNode* FindNode(const basestring &name);


	 /**
	  * Find a child node by index.
	  * @param idx The child node's index.
	  * @return Return NULL if there's no match node found.
	  */
	 XMLNode* FindNode(int idx);


	 /*
	 * Find first node
	 * @param name The child node's name. if is NULL, it will return the first node in child list
	 * @return First child node
	 */
	 XMLNode* FindFirstNode(const char *name);


	 /**
	  * Find the next child node.
	  * @return Return NULL if no more child node after last found node.
	  */
	 XMLNode* FindNextNode(const char *name);


	 /**
	 * Find attribute of node.
	 * @return Return NULL if can't fine attribute.
	 */
	 XMLAttribute* FindAttribute(const basestring &name);

	 XMLAttribute* FindFirstAttribute(const char *name);

	 XMLAttribute* FindNextAttribute(const char *name);

	 

     basestring m_Name;       /**< The node's name. */
     basestring m_Text;       /**< The node's text. */
     basestring m_Comment;    /**< The node's comment. */

 	 std::vector<XMLNode>       m_ChildNodes;  /**< The array of child nodes. */

private:

	 std::vector<XMLAttribute>  m_Attriutes;

     bool ReadTag(InStream &ins, basestring &name, bool &isbegin, bool &isEnd);
	 bool ReadAttribute(basestring name);
     basestring Decode(const basestring &str) const;
     basestring Encode(const basestring &str) const;

	 int m_LastNodeIndex;
	 int m_LastAttrbuteIndex;
};

///////////////////////////////////////////////////////////////////
/// special version of operator<< & >> for string
///////////////////////////////////////////////////////////////////
inline
basestring &operator<<(basestring &str, const basestring &value)
{
    return str = value;
}

inline
basestring &operator<<(basestring &str, const TCHAR* value)
{
    return str = value;
}

inline
basestring operator>>(const basestring &str, basestring &value)
{
	value = str;
	return str;
}

///////////////////////////////////////////////////////////////////
/// template for output value to string
///////////////////////////////////////////////////////////////////
template<class T>
basestring &operator<<(basestring &str, const T &value)
{
	OutStrStream os;
    os << value;
    if(str.size()) str += _T(' ');
    return str += os.str();
}

///////////////////////////////////////////////////////////////////
/// template for input value from string
///////////////////////////////////////////////////////////////////
template<class T>
basestring operator>>(const basestring &str, T &value)
{
    InStrStream is(str);
    is >> value;
    return basestring(str.c_str() + (int)is.tellg());
}

#endif    // end of __XMLNODE_H__

