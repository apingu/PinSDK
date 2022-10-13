/*********************************************************************
 * Filename: ZonaXml.h
 * Original Author: Martyn C Brown
 ********************************************************************/
	
/** Include only once */
#ifndef ZONA_XML_Included
#define ZONA_XML_Included

/** Include all necessary include files */
//#include <tchar.h>
#include <pdef.h>



namespace ZonaXml {

/** Enumeration used in union to decipher what type a node is. */
enum XMLNodeType
{
	eNodeEmpty = 0,
	eNodeAttribute,	
	eNodeElement,
	eNodeText,
	eNodeClear

};

/** Structure for XML attribute. */
struct XMLAttribute
{
	TCHAR *lpszName;
	TCHAR *lpszValue;
};

/** Structure used to manage list of nodes */
struct XMLElement
{
	TCHAR *lpszName;  /* Element name */
	int		 nSize;			/* Num of child nodes */
	int		 nMax;			/* Num of allocated nodes	*/
	int		 nIsDeclaration;	/* Whether node is an XML	 */
											    /* declaration - '<?xml ?>'	 */
	struct XMLNode *pEntries;		/* Array of child nodes */
	struct XMLElement *pParent;		/* Pointer to parent element */
};

/** Structure for XML text */
struct XMLText
{
	TCHAR *lpszValue;
};

/** Structure for XML clear (unformatted) node */
struct XMLClear
{
	TCHAR *lpszOpenTag;
	TCHAR *lpszValue;
	TCHAR *lpszCloseTag;
};


/** Structure used for a node */
struct XMLNode
{
  /** 
   * This dictates what the node is (and consequently which pointer should
   * be used to obtain the appropriate node). */
	XMLNodeType type;

  /** Union to access the appropriate node. */
	union
	{
		XMLAttribute	*pAttrib;
		XMLElement		*pElement;
		XMLText		*pText;
		XMLClear		*pClear;
	} node;

#ifdef STOREOFFSETS
	int nStringOffset;
#endif /* STOREOFFSETS */

};

/** Enumeration for XML parse errors. */
enum XMLError
{
	eXMLErrorNone = 0,
	eXMLErrorEmpty,
	eXMLErrorFirstNotStartTag,
	eXMLErrorMissingTagName,
	eXMLErrorMissingEndTagName,
	eXMLErrorNoMatchingQuote,
	eXMLErrorUnmatchedEndTag,
	eXMLErrorUnexpectedToken,
	eXMLErrorInvalidTag,
	eXMLErrorNoElements
};

/** Structure used to obtain error details if the parse fails. */
struct XMLResults
{
	enum XMLError error;
	int  nLine;
	int  nColumn;
};

/** Construct/delete root element */
XMLElement *CreateRoot();
void DeleteRoot(XMLElement *pElement);

/** Obtain error information in a string. */
TCHAR* GetError(XMLError error);

/**
 * Parse XML string into elements.  This returns a pointer to the first 
 * element (created on the heap) if successful.  This must be deleted by 
 * first calling DeleteElement() to recursively delete child nodes then 
 * calling free on the element to cleanup the heap.
 * If the function fails then 0 will be returned.  If the results pointer
 * given to the function was not 0 the error, line and column can be 
 * obtained.
 *****************************************************************************/
XMLElement* ParseXML(TCHAR *lpszXML, XMLResults *pResults);

XMLElement* AddElement(XMLElement *pEntry, TCHAR *lpszName, 
							 int nIsDeclaration, int nGrowBy);

/** Clears an element (deletes its children and the memory which belongs to 
 * it). */
void DeleteElement(XMLElement *pEntry);

/**
 * Enumerate nodes in the list returning a pointer to a node.  The index 
 * pointer should be initialised to zero initially - this will be incremented 
 * for each subsequent node that is obtained.
 * 0 will be returned when all nodes have been obtained.
 *****************************************************************************/
XMLNode* EnumNodes(XMLElement *pEntry, int *pnIndex);

/**
 * Recursively search the tree for the required element based on the given
 * path.
 *****************************************************************************/
XMLElement* FindElement(XMLElement *pHead, TCHAR *lpszName);

/**
 * Search the given element for an attribute.
 *****************************************************************************/
XMLAttribute* FindAttribute(XMLElement *pEntry, TCHAR *lpszAttrib);

/**
 * Enumerate elements on the given element.
 *****************************************************************************/
XMLElement * EnumElements(XMLElement *pEntry, int *pnIndex);

/**
 * Add an attribute to the element
 *****************************************************************************/
XMLAttribute* AddAttribute(XMLElement *pEntry, TCHAR *lpszName, 
	TCHAR *lpszValue, int nGrowBy);

XMLText* AddText(XMLElement *pEntry, TCHAR *lpszValue, int nGrowBy);

/**
 * Enumerate attributes on the given element.
 *****************************************************************************/
XMLAttribute* EnumAttributes(XMLElement *pEntry, int *pnIndex);

/**
 * Create elements in the list based on the path, returning the final node.
 **************************************************************************** */
XMLElement* CreateElements(XMLElement *pEntry, TCHAR *lpszPath);

/**
 * Duplicate a string.
 *****************************************************************************/
TCHAR *Strdup(TCHAR *lpszData, int cbData);

/**
 * Create an XML string from the head element.
 * If successful this returns the XML string representation of the specified 
 * XML element node and its subnodes.  This string must be deleted with 
 * free().
 *
 * If the size pointer is not 0 then the total size of the returned string
 * NOT INCLUDING THE NULL TERMINATOR will be updated.
 *****************************************************************************/
TCHAR *CreateXMLString(XMLElement* pHead, int nFormat, int *pnSize);

int CreateXMLStringR(XMLElement* pEntry, TCHAR *lpszMarker, 
	int nFormat);

/**
 * Add clear unformatted data to the node.
 *****************************************************************************/
XMLClear* AddCData(XMLElement *pEntry, TCHAR *lpszValue, int nGrowBy);

} // end ZonaXml namespace
using namespace ZonaXml;

#endif /* XML_Included */
