/*********************************************************************
 * <P> McbXML.c - implementation file for basic XML parser written in ANSI C 
 * for portability.
 * It works by using recursion and a node tree for breaking down the elements
 * of an XML document.  </P>
 *
 * @author Martyn C Brown
 *
 * @changeHistory  
 *	21st August    	2001	 - 	(V1.0) Creation (MCB)
 *  Mark Harmon: cpp file required changes, ZonaXml namespace and functionality
 *
 * Filename: ZonaXml.cpp
 ********************************************************************/

/* Include all necessary include files */
#include "ZonaXml.h"
#include <malloc.h>
//#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#if defined(_PS2) || defined(_XBOX)
#include <string.h>
#define _tfopen fopen
#define _T(x) x
#define _tcscpy strcpy
#define _tcslen strlen
#define _tcsstr strstr
#define _stprintf sprintf
#define _ttoi atoi
#define _tcsnicmp strncmp
#define _tcschr strchr
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/** Enumeration used to decipher what type a token is */
enum TokenType
{
	eTokenText = 0,
	eTokenQuotedText,
	eTokenTagStart,			/* "<"			*/
	eTokenTagEnd,			/* "</"			*/
	eTokenCloseTag,			/* ">"			*/
	eTokenEquals,			/* "="			*/
	eTokenDeclaration,		/* "<?"			*/
	eTokenShortHandClose,	/* "/>"			*/
	eTokenClear,
	eTokenError
};

/** Defines to dictate grow by value when adding nodes. */
#define GROWBY			5

#define INDENTCHAR	_T('\t')

struct ClearTag
{
	TCHAR *lpszOpen;
	TCHAR *lpszClose;
};

struct NextToken
{
	ClearTag *pClr;
	TCHAR    *pStr;
};

/** Main structure used for parsing XML */
struct XML
{
	TCHAR *lpXML;
	int nIndex;
	XMLError		error;
	TCHAR *lpEndTag;
	int cbEndTag;
	TCHAR *lpNewElement;
	int cbNewElement;
	int nFirst;
	ClearTag *pClrTags;
};

/** Enumeration used when parsing attributes */
enum Attrib
{
	eAttribName = 0,
	eAttribEquals,
	eAttribValue
};

/** Enumeration used when parsing elements to dictate whether we are currently
 * inside a tag */
enum Status
{
	eInsideTag = 0,
	eOutsideTag
};

/**
 * <P> Initialise an element.  </P>
 *
 * @methodName  InitElement
 *
 * @param       *pEntry		
 * @param       lpszName		
 * @return      void
 * @exception   none
 *****************************************************************************/
void InitElement(XMLElement *pEntry, XMLElement *pParent, 
				   TCHAR *lpszName, int nIsDeclaration)
{
	assert(pEntry);

	pEntry->nMax = 0;
	pEntry->nSize = 0;
	pEntry->pEntries = NULL;
	pEntry->pParent = pParent;
	pEntry->nIsDeclaration = nIsDeclaration;
	pEntry->lpszName = lpszName;

}/* InitElement */

/**
 * <P> Create the root element.  </P>
 *
 * @methodName  * CreateRoot
 *
 * @param       none
 * @return      XMLElement
 *****************************************************************************/
XMLElement* ZonaXml::CreateRoot()
{
	XMLElement * pElement;

	pElement = (XMLElement*)malloc(sizeof(XMLElement));
	InitElement(pElement, NULL, 0, FALSE);

	return pElement;

}/* CreateRoot */

/**
 * <P> Delete the root element and set it to NULL.  </P>
 *
 * @methodName  DeleteRoot
 *
 * @param       ** ppElement		
 * @return      void
 *****************************************************************************/
void ZonaXml::DeleteRoot(XMLElement * pElement)
{
  DeleteElement(pElement);
  free(pElement);
}/* DeleteRoot */

/**
 * <P> Delete an attribute.  </P>
 *
 * @methodName  DeleteAttribute
 *
 * @param       *pEntry		
 * @return      void
 *****************************************************************************/
void DeleteAttribute(XMLAttribute *pEntry)
{
	assert(pEntry);

	if (pEntry->lpszName) 
	{
		free(pEntry->lpszName);
		pEntry->lpszName = NULL;
	}

	if(pEntry->lpszValue) 
	{
		free(pEntry->lpszValue);
		pEntry->lpszValue=  NULL;
	}

}/* DeleteAttribute */

/**
 * <P> Attach attributes from one list to another.  </P>
 *
 * @methodName  AttributeAttach
 *
 * @param       *pDst		
 * @param       *pSrc		
 * @param       nNum		
 * @return      void
 *****************************************************************************/
void AttributeAttach(XMLAttribute *pDst, XMLAttribute *pSrc, int nNum)
{
	int n;

	for (n=0; n<nNum; n++)
	{
		pDst[n].lpszName = pSrc[n].lpszName;
		pDst[n].lpszValue = pSrc[n].lpszValue;

		pSrc[n].lpszName = NULL;
		pSrc[n].lpszValue = NULL;
	}

}/* AttributeAttach */

/**
 * <P> Obtain the next character from the string.  </P>
 *
 * @methodName  GetNextChar
 *
 * @param       *pXML		
 * @return      TCHAR
 *****************************************************************************/
TCHAR GetNextChar(XML *pXML)
{
	TCHAR ch;

	ch = pXML->lpXML[pXML->nIndex];
	
	if (ch != 0) pXML->nIndex++;

	return ch;

}/* GetNextChar */

/**
 * <P> Find next non-white space character.  </P>
 *
 * @methodName  FindNonWhiteSpace
 *
 * @param       *pXML		
 * @return      TCHAR
 *****************************************************************************/
TCHAR FindNonWhiteSpace(XML *pXML)
{
	TCHAR ch;
	TCHAR *lpXML = pXML->lpXML;
	int nExit = FALSE;

	assert(pXML);

   /*
    *************************************************************************
    * Iterate through characters in the string until we find a NULL or a
    * non-white space character
    *************************************************************************
    */
	while((nExit == FALSE) && (ch = GetNextChar(pXML)))
	{
		switch(ch)
		{
       /*
        *********************************************************************
        * Ignore white space
        *********************************************************************
        */
        case _T('\n'):
        case _T(' '):
		case _T('\t'):
		case _T('\r'):
			continue;			

		default:
			nExit = TRUE;

		}
	}

	return ch;

}/* FindNonWhiteSpace */

/**
 * <P> Duplicate a given string.  </P>
 *
 * @methodName  Strdup
 *
 * @param       lpszData		
 * @param       cbData		
 * @return      TCHAR*
 *****************************************************************************/
TCHAR *ZonaXml::Strdup(TCHAR *lpszData, int cbData)
{
	TCHAR *lpszNew;

	assert(lpszData);

	if (cbData == 0) cbData = strlen(lpszData);

	lpszNew = (TCHAR*) malloc((cbData+1) * sizeof(TCHAR));

	if (lpszNew)
	{
		memcpy(lpszNew, lpszData, (cbData) * sizeof(TCHAR));
		lpszNew[cbData] = NULL;
	}

	return lpszNew;

}/* Strdup */

/**
 * <P> Find the next token in a string.  </P>
 *
 * @methodName  GetNextToken
 *
 * @param       *pXML		
 * @param       *pcbToken		
 * @param       *pType		
 * @return      LPCTSTR
 *****************************************************************************/
NextToken GetNextToken(XML *pXML, int *pcbToken, TokenType *pType)
{
	NextToken		result;
	TCHAR			*lpXML;
	TCHAR			ch;
	TCHAR			chTemp;
	int				nSize;
	int				nFoundMatch;
	int				nExit;
	int				n;
	TCHAR 		*lpszOpen;
	int				cbOpen;
	int				nIsText = FALSE;

   /*
    *************************************************************************
    * Find next non-whte space character
    *************************************************************************
    */
	ch = FindNonWhiteSpace(pXML);

	if (ch)
	{
       /*
        *********************************************************************
        * Cache the current string pointer
        *********************************************************************
        */
		lpXML = pXML->lpXML;
		result.pStr = &lpXML[pXML->nIndex-1];

       /*
        *********************************************************************
        * First check whether the token is in the clear tag list (meaning it 
		* does not need formatting).
        *********************************************************************
        */
		n = 0;

		while(TRUE)
		{
           /*
            *****************************************************************
            * Obtain the name of the open part of the clear tag
            *****************************************************************
            */
			lpszOpen = pXML->pClrTags[n].lpszOpen;

			if (lpszOpen)
			{
               /*
                *************************************************************
                * Compare the open tag with the current token
                *************************************************************
                */
				cbOpen = strlen(lpszOpen);

				if (_tcsnicmp(lpszOpen, result.pStr, cbOpen) == 0)
				{
					result.pClr = &pXML->pClrTags[n];
					pXML->nIndex += cbOpen-1;
					*pType  = eTokenClear;
					return result;
				}

				n++;
			}
			else
			{
				break;
			}
		}

       /*
        *********************************************************************
        * If we didn't find a clear tag then check for standard tokens
        *********************************************************************
        */
		chTemp = 0;

		lpXML = pXML->lpXML;
		
		switch(ch)
		{
       /*
        *********************************************************************
        * Check for quotes
        *********************************************************************
        */
		case _T('\''):
		case _T('\"'):

           /*
            *****************************************************************
            * Type of token
            *****************************************************************
            */
			*pType = eTokenQuotedText;

			chTemp = ch;

           /*
            *****************************************************************
            * Set the size
            *****************************************************************
            */			
			nSize = 1;

			nFoundMatch = FALSE;

           /*
            *****************************************************************
            * Search through the string to find a matching quote
            *****************************************************************
            */
			while(ch = GetNextChar(pXML))
			{
				nSize++;

				if (ch == chTemp)
				{
					nFoundMatch = TRUE;
					break;
				}
			}

           /*
            *****************************************************************
            * If we failed to find a matching quote
            *****************************************************************
            */
			if (nFoundMatch == FALSE)
			{
               /*
                *************************************************************
                * Indicate error
                *************************************************************
                */
				pXML->error = eXMLErrorNoMatchingQuote;				
				*pType = eTokenError;
			}

			/*  4.02.2002 */
			if (FindNonWhiteSpace(pXML))
			{
				pXML->nIndex--;
			}

			break;

       /*
        *********************************************************************
        * Equals (used with attribute values)
        *********************************************************************
        */
		case _T('='):
			nSize = 1;
			*pType = eTokenEquals;
			break;

       /*
        *********************************************************************
        * Close tag
        *********************************************************************
        */
		case _T('>'):
			nSize = 1;
			*pType = eTokenCloseTag;
			break;

       /*
        *********************************************************************
        * Check for tag start and tag end
        *********************************************************************
        */
		case _T('<'):

           /*
            *****************************************************************
            * Peek at the next character to see if we have an end tag '</',
			* or an xml declaration '<?'
            *****************************************************************
            */
			chTemp = pXML->lpXML[pXML->nIndex];

           /*
            *****************************************************************
            * If we have a tag end...
            *****************************************************************
            */
			if (chTemp == _T('/'))
			{
               /*
                *************************************************************
                * Set the type and ensure we point at the next character 
                *************************************************************
                */
				GetNextChar(pXML);
				*pType = eTokenTagEnd;
				nSize = 2;
			}
           /*
            *****************************************************************
            * If we have an XML declaration tag
            *****************************************************************
            */
			else if (chTemp == _T('?'))
			{
               /*
                *************************************************************
                * Set the type and ensure we point at the next character 
                *************************************************************
                */
				GetNextChar(pXML);
				*pType = eTokenDeclaration;
				nSize = 2;
			}
           /*
            *****************************************************************
            * Otherwise we must have a start tag
            *****************************************************************
            */
            else 
			{
				*pType = eTokenTagStart;
				nSize = 1;
			}
			break;

       /*
        *********************************************************************
        * Check to see if we have a short hand type end tag ('/>').
        *********************************************************************
        */
		case _T('/'):
		   /*
            *****************************************************************
            * Peek at the next character to see if we have an end tag '</'
			* or an xml declaration '<?'
            *****************************************************************
            */
			chTemp = pXML->lpXML[pXML->nIndex];

           /*
            *****************************************************************
            * If we have a short hand end tag...
            *****************************************************************
            */
			if (chTemp == _T('>'))
			{
               /*
                *************************************************************
                * Set the type and ensure we point at the next character 
                *************************************************************
                */
				GetNextChar(pXML);
				*pType = eTokenShortHandClose;
				nSize = 2;
				break;
			}

           /*
            *****************************************************************
            * If we haven't found a short hand closing tag then drop into the
			* text process
            *****************************************************************
            */

       /*
        *********************************************************************
        * Other characters
        *********************************************************************
        */
		default:
			nIsText = TRUE;
		}

       /*
        *********************************************************************
        * If this is a TEXT node
        *********************************************************************
        */
		if (nIsText)
		{
           /*
            *****************************************************************
            * Indicate we are dealing with text
            *****************************************************************
            */
			*pType = eTokenText;

			nSize = 1;
			nExit = FALSE;

			while((nExit == FALSE) && (ch = GetNextChar(pXML)))
			{
                switch(ch)
                {
               /*
                *************************************************************
                * Break when we find white space
                *************************************************************
                */
                case _T('\n'):
                case _T(' '):
                case _T('\t'):
                case _T('\r'):
					nExit = TRUE;
                    break;

               /*
                *************************************************************
                * If we find a slash then this maybe text or a short hand end
				* tag.
                *************************************************************
                */
				case _T('/'):
                   /*
                    *********************************************************
                    * Peek at the next character to see it we have short hand
					* end tag
                    *********************************************************
                    */
					chTemp = pXML->lpXML[pXML->nIndex];

                   /*
                    *********************************************************
                    * If we found a short hand end tag then we need to exit 
					* the loop
                    *********************************************************
                    */
					if (chTemp == _T('>'))
					{
						pXML->nIndex--; /*  03.02.2002 */
						nExit = TRUE;
					}
					else
					{
						nSize++;
					}
					break;

               /*
                *************************************************************
                * Break when we find a terminator and decrement the index and
				* column count so that we are pointing at the right character
				* the next time we are called.
                *************************************************************
                */
				case _T('<'):
				case _T('>'):
				case _T('='):
					pXML->nIndex--;					
					nExit = TRUE;
					break;

				case 0:
					nExit = TRUE;
					break;

				default:
					nSize++;
                }
			}
		}

		*pcbToken = nSize;
	}
   /*
    *************************************************************************
    * If we failed to obtain a valid character
    *************************************************************************
    */
    else 
    {
   		*pcbToken = 0;
		*pType = eTokenError;
    }

	return result;

}/* GetNextToken */

/**
 * <P> Parse XML errors into a user friendly string.  </P>
 *
 * @methodName  GetError
 *
 * @param       error		
 * @return      LPCTSTR
 *****************************************************************************/
TCHAR *GetError(XMLError error)
{
	TCHAR *lpszErr = _T("Unknown");
	int n;

  /* Structure for errors array */
	struct ErrorList
	{
		XMLError err;
		TCHAR *lpszErr;
	};

  /* Static array containing helpful error text. */
	static ErrorList errs[] = 
	{
		{ eXMLErrorNone, _T("No error") },
		{ eXMLErrorEmpty, _T("No XML data") },
		{ eXMLErrorFirstNotStartTag,	_T("First token not start tag")	},
		{ eXMLErrorMissingTagName,		_T("Missing start tag name") },
		{ eXMLErrorMissingEndTagName,	_T("Missing end tag name") },
		{ eXMLErrorNoMatchingQuote,		_T("Unmatched quote") },
		{ eXMLErrorUnmatchedEndTag,		_T("Unmatched end tag") },
		{ eXMLErrorUnexpectedToken,		_T("Unexpected token found") },
		{ eXMLErrorInvalidTag,			  _T("Invalid tag found") },
		{ eXMLErrorNoElements,			  _T("No elements found") },
// MLH		{ 0, NULL }
	};

  /* Iterate through the list of errors to find a matching error */
	for(n = 0; errs[n].lpszErr; n++)
	{
		if(errs[n].err == error)
		{
			lpszErr = errs[n].lpszErr;
			break;
		}
	}

	return lpszErr;	

}/* GetError */

/**
 * <P> Delete memory associated with a text node.   </P>
 *
 * @methodName  DeleteText
 *
 * @param       *pText		
 * @return      void
 *****************************************************************************/
void DeleteText(XMLText *pText)
{
	assert(pText);

	if (pText->lpszValue)
	{
		free(pText->lpszValue);
		pText->lpszValue = NULL;
	}

}/* DeleteText */

/**
 * <P> Delete memory associated with a clear (unformatted) node.   </P>
 *
 * @methodName  DeleteClear
 *
 * @param       *pClear		
 * @return      void
 *****************************************************************************/
void DeleteClear(XMLClear *pClear)
{
	assert(pClear);

	if (pClear->lpszValue)
	{
		free(pClear->lpszValue);
		pClear->lpszValue = NULL;
	}

}/* DeleteClear */

/**
 * <P> Delete a given node.  </P>
 *
 * @methodName  DeleteNode
 *
 * @param       *pEntry		
 * @return      void
 *****************************************************************************/
void DeleteNode(XMLNode *pEntry)
{
	if (pEntry)
	{
		if (pEntry->type == eNodeEmpty)
		{
			return;
		}

       /*
        *********************************************************************
        * Delete the appropriate node
        *********************************************************************
        */
		switch(pEntry->type)
		{
		case eNodeAttribute:
			DeleteAttribute(pEntry->node.pAttrib);
			break;
			
		case eNodeElement:
			DeleteElement(pEntry->node.pElement);
			break;

		case eNodeText:
			DeleteText(pEntry->node.pText);
			break;

		case eNodeClear:
			DeleteClear(pEntry->node.pClear);
			break;

		default:
			assert(TRUE);
		}	

		free(pEntry->node.pAttrib);
		pEntry->type = eNodeEmpty;
	}

}/* DeleteNode */

/**
 * <P> Delete an element and all it's contained nodes.  </P>
 *
 * @methodName  DeleteElement
 *
 * @param       *pEntry		
 * @return      void
 *****************************************************************************/
void ZonaXml::DeleteElement(XMLElement *pEntry)
{
	int n;

	assert(pEntry);

   /*
    *************************************************************************
    * Delete each node (this may recurse)
    *************************************************************************
    */
	for(n = 0; n<pEntry->nSize; n++)
	{
		DeleteNode(&pEntry->pEntries[n]);
	}

   /*
    *************************************************************************
    * Cleanup the list of nodes
    *************************************************************************
    */
	pEntry->nMax = 0;
	pEntry->nSize = 0;

	free(pEntry->pEntries);
	pEntry->pEntries = NULL;
	
   /*
    *************************************************************************
    * Delete the name of the element
    *************************************************************************
    */
	if (pEntry->lpszName)
	{
		free(pEntry->lpszName);
		pEntry->lpszName = NULL;
	}

	/* free(pEntry); */

}/* DeleteElement */

/**
 * <P> Attach nodes from one list to another.  </P>
 *
 * @methodName  AttachNodes
 *
 * @param       *pDst		
 * @param       *pSrc		
 * @param       nNum		
 * @return      void
 *****************************************************************************/
void AttachNodes(XMLNode *pDst, XMLNode *pSrc, int nNum)
{
	int n;

	for (n=0; n<nNum; n++)
	{
		pDst[n] = pSrc[n];
		pSrc[n].type = eNodeEmpty;
	}

}/* AttachNodes */

/**
 * <P> Reserve memory for additional nodes.  </P>
 *
 * @methodName  AllocNodes
 *
 * @param       *pEntry		
 * @param       nGrowBy		
 * @return      void
 *****************************************************************************/
void AllocNodes(XMLElement *pEntry, int nGrowBy)
{
	int nMax;
	XMLNode * pNew;

	assert(pEntry);
	assert(nGrowBy > 0);

   /*
    *************************************************************************
    * Allocate storage for new nodes
    *************************************************************************
    */
    nMax = pEntry->nMax += nGrowBy;
    pNew = (XMLNode*) malloc(sizeof(XMLNode) * nMax);

   /*
    *************************************************************************
    * Attach old entries to the new storage
    *************************************************************************
    */
    AttachNodes(pNew, pEntry->pEntries, pEntry->nSize);

	if (pEntry->pEntries)
	{
		free(pEntry->pEntries);
	}

    pEntry->pEntries = pNew;

}/* AllocNodes */

/**
 * <P> Add an element node to the element.  </P>
 *
 * @methodName  AddElement
 *
 * @param       *pEntry		
 * @param       lpszName		
 * @param       nIsDeclaration
 * @param       nGrowBy		
 * @return      XMLElement *
 *****************************************************************************/
XMLElement* ZonaXml::AddElement(XMLElement *pEntry, TCHAR *lpszName, 
							 int nIsDeclaration, int nGrowBy)
{
	XMLNode *pNode;
	XMLElement *pElement;

	assert(pEntry);
	assert(nGrowBy > 0);

  /** Check we have enough storage */
	if (pEntry->nSize == pEntry->nMax)
	{
		AllocNodes(pEntry, nGrowBy);
	}

  /** Obtain the new node, set the type and initialise it. */
	pNode = &pEntry->pEntries[pEntry->nSize];
	pNode->type = eNodeElement;

	pElement = (XMLElement*)malloc(sizeof(XMLElement));
	pNode->node.pElement = pElement;

	InitElement(pElement, pEntry, lpszName, nIsDeclaration);

  /** Increment the number of contained nodes */
	pEntry->nSize++;

  /** Return the new element. */
	return pElement;

}/* AddElement */

/**
 * <P> Add an attribute to an element.  </P>
 *
 * @methodName  AddAttribute
 *
 * @param       *pEntry		
 * @param       lpszName		
 * @param       lpszValue		
 * @param       nGrowBy		
 * @return      XMLAttribute * 
 *****************************************************************************/
XMLAttribute* ZonaXml::AddAttribute(XMLElement *pEntry, TCHAR *lpszName, 
					TCHAR *lpszValue, int nGrowBy)
{
	XMLNode *pNode;
	XMLAttribute *pAttr;

	assert(pEntry);
	assert(nGrowBy > 0);

  /** Check we have enough storage */
	if (pEntry->nSize == pEntry->nMax)
	{
		AllocNodes(pEntry, nGrowBy);
	}

  /** Obtain the new node, set the type and initialise it. */
	pNode = &pEntry->pEntries[pEntry->nSize];
	pNode->type = eNodeAttribute;

	pAttr = (XMLAttribute*)malloc(sizeof(XMLAttribute));
	pNode->node.pAttrib = pAttr;

	pAttr->lpszName = lpszName;
	pAttr->lpszValue = lpszValue;

  /** Increment the number of contained nodes */
	pEntry->nSize++;

  /** Return the new attribute. */
	return pAttr;

}/* AddAttribute */


/**
 * <P> Add text to the element.  </P>
 *
 * @methodName  * AddText
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      XMLText
 *****************************************************************************/
XMLText* ZonaXml::AddText(XMLElement *pEntry, TCHAR *lpszValue, int nGrowBy)
{
	XMLNode *pNode;
	XMLText *pText;

	assert(pEntry);
	assert(nGrowBy > 0);

  /* Check we have enough storage */
	if (pEntry->nSize == pEntry->nMax)
	{
		AllocNodes(pEntry, nGrowBy);
	}

  /* Obtain the new node, set the type and initialise it. */
	pNode = &pEntry->pEntries[pEntry->nSize];
	pNode->type = eNodeText;

	pText = (XMLText*)malloc(sizeof(XMLText));
	pNode->node.pText = pText;
	pText->lpszValue = lpszValue;

  /* Increment the number of contained nodes */
	pEntry->nSize++;

  /* Return the new attribute. */
	return pText;

}/* AddText */


/**
 * <P> Add clear (unformatted) to the element.  </P>
 *
 * @methodName  * AddClear
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      XMLClear
 *****************************************************************************/
XMLClear* AddClear(XMLElement *pEntry, TCHAR *lpszValue, 
						ClearTag *pClear, int nGrowBy)
{
	XMLNode *pNode;
	XMLClear *pNewClear;

	assert(pEntry);
	assert(nGrowBy > 0);

   /* Check we have enough storage */
	if (pEntry->nSize == pEntry->nMax)
	{
		AllocNodes(pEntry, nGrowBy);
	}

   /* Obtain the new node, set the type and initialise it. */
	pNode = &pEntry->pEntries[pEntry->nSize];
	pNode->type = eNodeClear;

	pNewClear = (XMLClear*)malloc(sizeof(XMLClear));
	pNode->node.pClear = pNewClear;
	pNewClear->lpszValue = lpszValue;
	pNewClear->lpszOpenTag = pClear->lpszOpen;
	pNewClear->lpszCloseTag = pClear->lpszClose;

   /* Increment the number of contained nodes */
	pEntry->nSize++;

   /* Return the new attribute. */
	return pNewClear;

}/* AddClear */


/**
 * <P> Enumerate nodes in the list.  </P>
 *
 * @methodName  EnumNodes
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      XMLNode
 *****************************************************************************/
XMLNode* ZonaXml::EnumNodes(XMLElement *pEntry, int *pnIndex)
{	
	XMLNode * pResult = NULL;

	assert(pnIndex);
	assert(pEntry);

	if (*pnIndex < pEntry->nSize)
	{
		pResult = &pEntry->pEntries[*pnIndex];
		(*pnIndex)++;
	}

	return pResult;

}/* EnumNodes */

/**
 * <P> Enumerate elements on a base element.  </P>
 *
 * @methodName  EnumElements
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      XMLNode
 *****************************************************************************/
XMLElement* ZonaXml::EnumElements(XMLElement *pEntry, int *pnIndex)
{	
	XMLElement * pResult = NULL;
	int nIndex;

	assert(pnIndex);
	assert(pEntry);

	nIndex = *pnIndex;

	for (;nIndex < pEntry->nSize && !pResult; nIndex++)
	{
		if (pEntry->pEntries[nIndex].type == eNodeElement)
		{
			pResult = pEntry->pEntries[nIndex].node.pElement;
		}
	}

	*pnIndex = nIndex;

	return pResult;

}/* EnumElements */

/**
 * <P> Enumerate attributes on a base element.  </P>
 *
 * @methodName  EnumAttributes
 *
 * @param       *pEntry		
 * @param       *pnIndex		
 *
 * @return      XMLNode
 *****************************************************************************/
XMLAttribute* ZonaXml::EnumAttributes(XMLElement *pEntry, int *pnIndex)
{	
	XMLAttribute * pResult = NULL;
	int nIndex;

	assert(pnIndex);
	assert(pEntry);

	nIndex = *pnIndex;

	for (;nIndex < pEntry->nSize && !pResult; nIndex++)
	{
		if (pEntry->pEntries[nIndex].type == eNodeAttribute)
		{
			pResult = pEntry->pEntries[nIndex].node.pAttrib;
		}
	}

	*pnIndex = nIndex;

	return pResult;

}/* EnumAttributes */

/**
 * <P> Trim the end of the text to remove white space characters.  </P>
 *
 * @methodName  FindEndOfText
 *
 * @param       lpszToken		
 * @param       *pcbText		
 *
 * @return      void
 *****************************************************************************/
void FindEndOfText(TCHAR *lpszToken, int *pcbText)
{
	TCHAR	ch;
	int		cbText;

	assert(lpszToken);
	assert(pcbText);

	cbText = (*pcbText)-1;	

	while(TRUE)
	{
		assert(cbText >= 0);

		ch = lpszToken[cbText];

		switch(ch)
		{
		case _T('\r'):
		case _T('\n'):
		case _T('\t'):
		case _T(' '):
			cbText--;
			break;

		default:
			*pcbText = cbText+1;
			return;
		}
	}

}/* FindEndOfText */

/**
 * <P> Parse a clear (unformatted) type node.  </P>
 *
 * @methodName  ParseClearTag
 *
 * @param       *pXML		
 * @param       *pElement		
 * @param       lpszClose		
 * @param       lpszToken		
 *
 * @return      int
 *****************************************************************************/
int ParseClearTag(XML *pXML, XMLElement *pElement, ClearTag * pClear)
{
	int cbTemp = 0;

	TCHAR *lpszTemp;
	TCHAR *lpszXML = &pXML->lpXML[pXML->nIndex];

   /*
    *************************************************************************
    * Find the closing tag
    *************************************************************************
    */
	lpszTemp = _tcsstr(lpszXML, pClear->lpszClose);

   /*
    *************************************************************************
    * Iterate through the tokens until we find the closing tag.
    *************************************************************************
    */
    if (lpszTemp)
    {
      /* Cache the size and increment the index */
      cbTemp = lpszTemp - lpszXML;

		  pXML->nIndex += cbTemp;
		  pXML->nIndex += strlen(pClear->lpszClose);
  
      /* Add the clear node to the current element */
      lpszTemp = ZonaXml::Strdup(lpszXML, cbTemp);

      AddClear(pElement, lpszTemp, pClear, GROWBY);

#ifdef STOREOFFSETS
		  pElement->pEntries[pElement->nSize-1].nStringOffset = (lpszXML -
			strlen(pClear->lpszOpen)) - pXML->lpXML;			
#endif /* STOREOFFSETS */

      return TRUE;
    }
   /*
    *************************************************************************
    * If we failed to find the end tag
    *************************************************************************
    */
    else
    {
        pXML->error = eXMLErrorUnmatchedEndTag;
        return FALSE;
    }

}/* ParseClearTag */

/**
 * <P> Recursively parse an XML element.  </P>
 *
 * @methodName  ParseXMLElement
 *
 * @param       *pXML		
 * @param       * pElement		
 *
 * @return      int
 *****************************************************************************/
int ParseXMLElement(XML *pXML, XMLElement * pElement)
{
	TCHAR *lpszToken;
	int cbToken;

	TokenType type;
	NextToken token;

	TCHAR *lpszTemp;
	int cbTemp;

	int nDeclaration;

	TCHAR *lpszText = NULL;

	XMLElement *pNew;

	Status status;
	Attrib attrib = eAttribName;

	assert(pXML);
	assert(pElement);

  /** If this is the first call to the function */
  if (pXML->nFirst)
	{
    /** Assume we are outside of a tag definition */
		pXML->nFirst = FALSE;
		status = eOutsideTag;
	}
  /**
   * If this is not the first call then we should only be called when
   * inside a tag. */
  else 
  {
    status = eInsideTag;
  }

  /** Iterate through the tokens in the document */
	while(TRUE)
	{
    /**
     * Obtain the next token
     **********************************************************************/
		token = GetNextToken(pXML, &cbToken, &type);

 		if(type != eTokenError)
		{
      /** Check the current status */
			switch(status)
			{
      /** If we outside of a tag definition */
			case eOutsideTag:
        /** Check what type of token we obtained */
				switch(type)
				{
         /** If we have found text or quoted text */
				case eTokenText:
				case eTokenQuotedText:
				case eTokenEquals:
					if (!lpszText)
					{
						lpszText = token.pStr;
					}					
					break;				

        /** If we found a start tag '<' and declarations '<?' */
				case eTokenTagStart:
				case eTokenDeclaration:
          /** Cache whether this new element is a declaration or not */
					nDeclaration = type == eTokenDeclaration;

          /** If we have node text then add this to the element */
					if (lpszText)
					{
						cbTemp = token.pStr - lpszText;

						FindEndOfText(lpszText, &cbTemp);

            ZonaXml::AddText(pElement, 
                     ZonaXml::Strdup(lpszText, cbTemp), GROWBY);

#ifdef STOREOFFSETS
						pElement->pEntries[pElement->nSize-1].nStringOffset
							= lpszText - pXML->lpXML;
#endif /* STOREOFFSETS */

						lpszText = NULL;
					}

          /** Find the name of the tag */
					token = GetNextToken(pXML, &cbToken, &type);

         /**
          * Return an error if we couldn't obtain the next token orit wasnt text */
					if (type != eTokenText)
					{
						pXML->error = eXMLErrorMissingTagName;
						return FALSE;
					}

          /** If we found a new element which has the same as this 
           * element then we need to pass this back to the caller.. */
					if (pElement->lpszName && 
						_tcsnicmp(pElement->lpszName, token.pStr, 
						strlen(pElement->lpszName)) == 0)
					{
            /** Indicate to the caller that it needs to create a 
            * new element. */
            pXML->lpNewElement = token.pStr;
            pXML->cbNewElement = cbToken;

						return TRUE;
					}
          /** If the name of the new element differs from the name of
           * the current element we need to add the new element to 
           * the current one and recurse */
					else
					{
            /** Now we need to add the new element and recurse */
            pNew = ZonaXml::AddElement(pElement, 
              ZonaXml::Strdup(token.pStr, cbToken), nDeclaration, GROWBY);

#ifdef STOREOFFSETS
						pElement->pEntries[pElement->nSize-1].nStringOffset = 
							token.pStr - pXML->lpXML;
#endif /* STOREOFFSETS */

						while(pNew)
            {
              /**
               * Callself to process the new node.  If we return 
               * FALSE this means we dont have any more 
               * processing to do... */
              if (!ParseXMLElement(pXML, pNew))
              {
                return FALSE;
              }
              else
              {
                /**
                * If the call to recurse this function 
                * evented in a end tag specified in XML then 
                * we need to unwind the calls to this 
                * function until we find the appropriate node 
                * (the element name and end tag name must 
                * match) */
                if (pXML->cbEndTag)
                {
                /**
                 * If we are back at the root node then we 
                 * have an unmatched end tag */
                if (!pElement->lpszName)
                {
                  pXML->error = 
                  eXMLErrorUnmatchedEndTag;
                  return FALSE;
                }

                /**
                 * If the end tag matches the name of this 
                 * element then we only need to unwind once more...*/
                if (_tcsnicmp(pXML->lpEndTag, pElement->lpszName, 
                    strlen(pElement->lpszName)) == 0)
                {                                    
                  pXML->cbEndTag = 0;
                }

                return TRUE;
              }
              /**
               * If the call indicated a new element is to 
               * be created on THIS element. */
              else if (pXML->cbNewElement)
              {
              /**
              * If the name of this element matches the
              * name of the element we need to create 
              * then we need to return to the caller 
              * and let it process the element. */
              if(_tcsnicmp(pXML->lpNewElement, pElement->lpszName, 
										       strlen(pElement->lpszName)) == 0)
              {                                    
                return TRUE;
              }

              /** Add the new element and recurse */
              pNew = ZonaXml::AddElement(pElement, 
                ZonaXml::Strdup(pXML->lpNewElement, pXML->cbNewElement),
                FALSE, GROWBY);

#ifdef STOREOFFSETS
              pElement->pEntries[pElement->nSize-1].
              nStringOffset = 
              pXML->lpNewElement - pXML->lpXML;
#endif /* STOREOFFSETS */

              pXML->cbNewElement = 0;
            }
            /** If we didn't have a new element to create */
            else 
						{
							pNew = NULL;
						}
					}
				}
			}
			break;

      /** If we found an end tag */
		  case eTokenTagEnd:
        /** If we have node text then add this to the element */
        if (lpszText)
        {
          cbTemp = token.pStr - lpszText;

          FindEndOfText(lpszText, &cbTemp);

          ZonaXml::AddText(pElement, 
            ZonaXml::Strdup(lpszText, cbTemp), GROWBY);

#ifdef STOREOFFSETS
					pElement->pEntries[pElement->nSize-1].nStringOffset
						= lpszText - pXML->lpXML;
#endif /* STOREOFFSETS */

					lpszText = NULL;						
				}
					
        /** Find the name of the end tag */
        token = GetNextToken(pXML, &cbTemp, &type);					

        /** The end tag should be text */
        if (type != eTokenText)
        {
          pXML->error = eXMLErrorMissingEndTagName;
          return FALSE;
        }

				lpszTemp = token.pStr;

        /** After the end tag we should find a closing tag */
				token = GetNextToken(pXML, &cbToken, &type);

				if (type != eTokenCloseTag)
				{
					pXML->error = eXMLErrorMissingEndTagName;
					return FALSE;
				}

        /**
         * We need to return to the previous caller.  If the name 
         * of the tag cannot be found we need to keep returning to
         * caller until we find a match */
        if (_tcsnicmp(lpszTemp, pElement->lpszName, 
        strlen(pElement->lpszName)) != 0)
        {
          pXML->lpEndTag = lpszTemp;
          pXML->cbEndTag = cbTemp;
        }

				return TRUE;

     /* If we found a clear (unformatted) token */
			case eTokenClear:
        /** If we have node text then add this to the element */
        if (lpszText)
        {
          cbTemp = token.pStr - lpszText;

          FindEndOfText(lpszText, &cbTemp);

          ZonaXml::AddText(pElement, 
            ZonaXml::Strdup(lpszText, cbTemp), GROWBY);

#ifdef STOREOFFSETS
					pElement->pEntries[pElement->nSize-1].nStringOffset
						= lpszText - pXML->lpXML;
#endif /* STOREOFFSETS */

					lpszText = NULL;
				}

				if (!ParseClearTag(pXML, pElement, token.pClr))
				{
					return FALSE;
				}
				break;

      /** Errors... */				
			case eTokenCloseTag:		/* '>'		*/
			case eTokenShortHandClose:	/* '/>'		*/
				pXML->error = eXMLErrorUnexpectedToken;
				return FALSE;
			}
			break;

      /** If we are inside a tag definition we need to search for attributes */
		  case eInsideTag:
       /**
        * Check what part of the attribute (name, equals, value) we 
				* are looking for. */
        switch(attrib)
        {
          /** If we are looking for a new attribute */
          case eAttribName:
            /** Check what the current token type is */
            switch(type)
					  {
              /** If the current type is text...Eg.  'attribute' */
					    case eTokenText:
                /**
                * Cache the token then indicate that we are next to 
                * look for the equals */
                lpszTemp = token.pStr;
                cbTemp = cbToken;
                attrib = eAttribEquals;
                break;

              /** If we found a closing tag... Eg.  '>'*/
              case eTokenCloseTag:
                /** We are now outside the tag */
                status = eOutsideTag;
                break;

              /**
               * If we found a short hand '/>' closing tag then we can 
               * return to the caller */
              case eTokenShortHandClose:
                return TRUE;

              /** Errors... */
              case eTokenQuotedText:	/* '"SomeText"'	*/
              case eTokenTagStart:	/* '<'			*/
              case eTokenTagEnd:		/* '</'			*/
              case eTokenEquals:		/* '='			*/
              case eTokenDeclaration:	/* '<?'			*/
              case eTokenClear:
                pXML->error = eXMLErrorUnexpectedToken;
                return FALSE;
            }
            break;

            /** If we are looking for an equals */
            case eAttribEquals:
              /** Check what the current token type is */
              switch(type)
              {
                /** If the current type is text... Eg.  'Attribute AnotherAttribute'*/
					      case eTokenText:
                  /** Add the unvalued attribute to the list */
                  ZonaXml::AddAttribute(pElement, 
                    ZonaXml::Strdup(lpszTemp, cbTemp), NULL, GROWBY);

#ifdef STOREOFFSETS
						      pElement->pEntries[pElement->nSize-1].nStringOffset =
							      lpszTemp - pXML->lpXML;
#endif /* STOREOFFSETS */

                  /**
                   * Cache the token then indicate.  We are next to 
                   * look for the equals attribute */
                  lpszTemp = token.pStr;
                  cbTemp = cbToken;
                  break;

                /**
                 * If we found a closing tag 'Attribute >' or a short hand 
                 * closing tag 'Attribute />' */
                case eTokenShortHandClose:
                case eTokenCloseTag:
                  /**
                   * If we are a declaration element '<?' then we need
                   * to remove extra closing '?' if it exists*/
                  if(pElement->nIsDeclaration && (lpszTemp[cbTemp-1]) == _T('?'))
                  {
                    cbTemp--;
                  }

                  if (cbTemp)
                  {
                    /** Add the unvalued attribute to the list */
                    ZonaXml::AddAttribute(pElement, 
                      ZonaXml::Strdup(lpszTemp, cbTemp), NULL, GROWBY);

#ifdef STOREOFFSETS
							      pElement->pEntries[pElement->nSize-1].
								      nStringOffset = lpszTemp - pXML->lpXML;
#endif /* STOREOFFSETS */
      						}

                  /** If this is the end of the tag then return to the caller */
                  if(type == eTokenShortHandClose) return TRUE;

                  /** We are now outside the tag */
                  status = eOutsideTag;
                  break;

                /** If we found the equals token... Eg.  'Attribute =' */
                case eTokenEquals:
                  /**
                   * Indicate that we next need to search for the value
                   * for the attribute */
                  attrib = eAttribValue;
                  break;

                /** Errors...*/
                case eTokenQuotedText:	/* 'Attribute "InvalidAttr"'*/
                case eTokenTagStart:	/* 'Attribute <'			*/
                case eTokenTagEnd:		/* 'Attribute </'			*/
                case eTokenDeclaration:	/* 'Attribute <?'			*/
                case eTokenClear:
                  pXML->error = eXMLErrorUnexpectedToken;
                  return FALSE;
              }
              break;

            /** If we are looking for an attribute value */
            case eAttribValue:
              /** Check what the current token type is*/
              switch(type)
              {
                /**
                 * If the current type is text or quoted text...
                 * Eg.  'Attribute = "Value"' or 'Attribute = Value' or
                 * 'Attribute = 'Value''. */
                case eTokenText:
                case eTokenQuotedText:
                  /**
                   * If we are a declaration element '<?' then we need
                   * to remove extra closing '?' if it exists */
                  if (pElement->nIsDeclaration && (token.pStr[cbToken-1]) == _T('?'))
                  {
                   cbToken--;
                  }

						      if (cbTemp) lpszToken = ZonaXml::Strdup(token.pStr, cbToken);
						      else lpszToken = NULL;

                  /** Add the valued attribute to the list */
                  ZonaXml::AddAttribute(pElement, 
                    ZonaXml::Strdup(lpszTemp, cbTemp), (TCHAR*)lpszToken, GROWBY);

#ifdef STOREOFFSETS
						      pElement->pEntries[pElement->nSize-1].nStringOffset
							      = lpszTemp - pXML->lpXML;
#endif /* STOREOFFSETS */

                  /** Indicate we are searching for a new attribute */
                  attrib = eAttribName;
                  break;

                /** Errors... */
                case eTokenTagStart:		/* 'Attr = <'			*/
                case eTokenTagEnd:			/* 'Attr = </'			*/
                case eTokenCloseTag:		/* 'Attr = >'			*/
                case eTokenShortHandClose:	/* "Attr = />"			*/
                case eTokenEquals:			/* 'Attr = ='			*/
                case eTokenDeclaration:		/* 'Attr = <?'			*/
                case eTokenClear:
                  pXML->error = eXMLErrorUnexpectedToken;
                  return FALSE;
                  break;
					}
				}
			}
		}
   /* If we failed to obtain the next token */
    else 
    {
      return FALSE;	
    }
	}

}/* ParseXMLElement */

/**
 * <P> Count the number of lines and columns in an XML string.  </P>
 *
 * @methodName  CountLinesAndColumns
 *
 * @param       lpXML		
 * @param       nUpto		
 * @param       *pResults		
 *
 * @return      void
 *****************************************************************************/
void CountLinesAndColumns(TCHAR *lpXML, int nUpto, XMLResults *pResults)
{
	TCHAR ch;
	int n;

	assert(lpXML);	
	assert(pResults);

	pResults->nLine = 1;
	pResults->nColumn = 1;

	for(n=0; n<nUpto; n++)
	{
		ch = lpXML[n];

		assert(ch);

		if (ch == _T('\n'))
		{
			pResults->nLine++;
			pResults->nColumn = 1;
		}
		else
		{
			pResults->nColumn++;
		}
	}
}/* CountLinesAndColumns */

/**
 * <P> Obtain tags used for unformatted text within elements.  </P>
 *
 * @methodName  * GetClearTags
 *
 * @param       none
 *
 * @return      ClearTag
 *****************************************************************************/
ClearTag* GetClearTags()
{
	static ClearTag tags[] =
	{
		{	_T("<![CDATA["),	_T("]]>")		},
		{	_T("<PRE>"),		_T("</PRE>")	},
		{	_T("<Script>"),		_T("</Script>")	},
		{	_T("<!--"),			_T("-->")		},
		{	_T("<!DOCTYPE"),	_T(">")			},
		{	NULL,				NULL			}
	};

	return tags;
}/* GetClearTags */

#define GETCDATA() &GetClearTags()[0]

/**
 * <P> Parse XML and return the root element.  </P>
 *
 * @methodName  * ParseXML
 *
 * @param       lpszXML		
 * @param       *pResults		
 *
 * @return      XMLElement
 *****************************************************************************/
XMLElement* ZonaXml::ParseXML(TCHAR *lpszXML, XMLResults *pResults)
{
	XMLError error;

	XMLElement * pElement = NULL;

	XML xml = { NULL, 0, eXMLErrorNone, NULL, 0, NULL, 0, TRUE };

	xml.lpXML = lpszXML;
	xml.pClrTags = GetClearTags();

  /* Create header element */
	pElement = ZonaXml::CreateRoot();

	ParseXMLElement(&xml, pElement);

	error = xml.error;

  /* If an error occurred */
	if (error != eXMLErrorNone)
	{
    /* Cleanup */
		ZonaXml::DeleteRoot(pElement);
		pElement = NULL;
	}

   /* If we hae been given somewhere to place results */
	if (pResults)
	{
		pResults->error = error;

    /* If we have an error */
		if (error != eXMLErrorNone)
		{
      /* Find which line and column it starts on. */
			CountLinesAndColumns(xml.lpXML, xml.nIndex, pResults);
		}
	}

	return pElement;
}/* ParseXML */


/**
 * <P> Search for an element in the list.  </P>
 *
 * @methodName  * FindElement
 *
 * @param       lpszName
 *
 * @return      XMLElement
 *****************************************************************************/
XMLElement* ZonaXml::FindElement(XMLElement *pEntry, TCHAR *lpszPath)
{
	XMLElement * pChild;
	TCHAR *lpszName;
	TCHAR *lpszNext;
	int cbName;
	int nIndex;

	assert(lpszPath);
	assert(pEntry);

  /* Find the next name in the path in case we need to recurse */
	lpszNext = _tcschr(lpszPath, _T('/'));

  /* Calculate the length of the current name we are searching for */
	if (!lpszNext)
	{
		cbName = strlen(lpszPath);		
	}
	else
	{
		cbName = lpszNext - lpszPath;

		if (lpszNext[1])
		{
			lpszNext++;
		}
		else
		{
			lpszNext = NULL;
		}
	}

	if (cbName)
	{
    /** Enumerate child elements */
    nIndex = 0;
    while(pChild = ZonaXml::EnumElements(pEntry, &nIndex))
    {
      /** Obtain the name of the child element */
      lpszName = pChild->lpszName;

      if (lpszName)
      {
        /** If the name of the element is what we are looking for */
        if (_tcsnicmp(lpszPath, lpszName, cbName) == 0)
        {
          /** Check if this is the last element to search for */
          if (!lpszNext)
          {
            return pChild;
          }
          /** If we have further nodes to find then recurse. */
          else 
          {
            return ZonaXml::FindElement(pChild, lpszNext);
          }
        }
      }
    }
  }

	return NULL;

}/* FindElement */

/**
 * <P> Find an attribute on an element.  </P>
 *
 * @methodName  * FindAttribute
 *
 * @param       *pEntry		
 * @param       lpszAttrib		
 *
 * @return      XMLAttribute
 *****************************************************************************/
XMLAttribute* ZonaXml::FindAttribute(XMLElement *pEntry, TCHAR *lpszAttrib)
{
	XMLAttribute * pAttr;
	int cbAttrib;
	int nIndex;

	assert(pEntry);
	assert(lpszAttrib);

	cbAttrib = strlen(lpszAttrib);
	nIndex = 0;

	while(pAttr = ZonaXml::EnumAttributes(pEntry, &nIndex))
	{
		if (_tcsnicmp(pAttr->lpszName, lpszAttrib, cbAttrib) == 0)
		{
			return pAttr;
		}
	}

	return NULL;

}/* FindAttribute */

/**
 * <P> Add CData to the element.  </P>
 *
 * @methodName  * AddCData
 *
 * @param       *pEntry		
 * @param       lpszValue		
 * @param       nGrowBy		
 *
 * @return      XMLClear
 *****************************************************************************/
XMLClear * AddCData(XMLElement *pEntry, TCHAR *lpszValue, int nGrowBy)
{
	return AddClear(pEntry, lpszValue, GETCDATA(), nGrowBy);

}/* AddCData */

/**
 * <P> Add elements to the list if they do not exist, return the final node.
 * </P>
 *
 * @methodName  * CreateElements
 *
 * @param       *pEntry		
 * @param       lpszPath		
 *
 * @return      XMLElement
 *****************************************************************************/
XMLElement* ZonaXml::CreateElements(XMLElement *pEntry, TCHAR *lpszPath)
{
	XMLElement * pChild;
	XMLElement * pNew;
	TCHAR *lpszName;
	TCHAR *lpszNext;
	int cbName;
	int nIndex;

	assert(lpszPath);
	assert(pEntry);

   /** Find the next name in the path in case we need to recurse */
	lpszNext = _tcschr(lpszPath, _T('/'));

   /** Calculate the length of the current name we are searching for */
	if (!lpszNext)
	{
		cbName = strlen(lpszPath);		
	}
	else
	{
		cbName = lpszNext - lpszPath;

		if (lpszNext[1])
		{
			lpszNext++;
		}
		else
		{
			lpszNext = NULL;
		}
	}

	if (cbName)
	{
    /** Enumerate child elements */
    nIndex = 0;
    while(pChild = ZonaXml::EnumElements(pEntry, &nIndex))
    {
      /* Obtain the name of the child element */
      lpszName = pChild->lpszName;

      if (lpszName)
      {
        /* If the name of the element is what we are looking for */
        if (_tcsnicmp(lpszPath, lpszName, cbName) == 0)
        {
          /* Check if this is the last element to search for */
          if (!lpszNext)
          {
            return pChild;
          }
          /* If we have further nodes to find then recurse. */
          else 
          {
            return ZonaXml::CreateElements(pChild, lpszNext);
          }
        }
      }
    }

    /* If we got this far then we couldn't find the required element so we
     * need to add a new element to the current element */
		pNew = ZonaXml::AddElement(pEntry, ZonaXml::Strdup(lpszPath, cbName), FALSE, GROWBY);

    /* If there are no more entries then return the node we just created.*/
		if (!lpszNext)
		{
			return pNew;
		}
    /* If there are more elements to search */
    else 
		{
      /* Recurse to add the remaining nodes */
			return ZonaXml::CreateElements(pNew, lpszNext);
		}
	}

	return NULL;
}/* CreateElements */

/**
 * <P> Creates an user friendly XML string from a given element with 
 * appropriate white space and carriage returns.
 *
 * This recurses through all subnodes then adds contents of the nodes to the
 * string.
 * </P>
 *
 * @methodName  CreateXMLStringR
 *
 * @param       XMLElement * pEntry - XML Element
 * @param       TCHAR *lpszMarker	  - String to create results into, this 
 *										can be zero if you want to calculate
 *										the size of the returned string.
 * @param       int nFormat			  -	Specify -1 for no formatting or the
 *										indent level (0 initially).
 * @return      int					  - Size of the returned string, not 
 *										including the NULL terminator.
 *****************************************************************************/
int ZonaXml::CreateXMLStringR(XMLElement * pEntry, TCHAR *lpszMarker, int nFormat)
{
	int nResult = 0;
	int cb;
	int cbElement;
	int nIndex;
	int nChildFormat;
	int bHasChildren = FALSE;
	XMLNode * pChild;
	XMLAttribute * pAttr;

	assert(pEntry);

#define LENSTR(lpsz) (lpsz ? strlen(lpsz) : 0)

   /* If the element has no name then assume this is the head node. */
	cbElement = LENSTR(pEntry->lpszName);

	if (cbElement)
	{
       /* "<elementname " */
		cb = nFormat == -1 ? 0 : nFormat;

    if (lpszMarker)
    {
      if (cb)
      {
        memset(lpszMarker, INDENTCHAR, sizeof(TCHAR)*cb);
      }

      nResult = cb;

      lpszMarker[nResult++] = _T('<');        
      _tcscpy(&lpszMarker[nResult], pEntry->lpszName);
      nResult += cbElement;
      lpszMarker[nResult++] = _T(' ');        
    }                
    else
    {
      nResult += cbElement + 2 + cb;
    }
        
    /* Enumerate attributes and add them to the string */
    nIndex = 0;
    while (pChild = ZonaXml::EnumNodes(pEntry, &nIndex))
    {
      switch(pChild->type)
      {
        /* Add attributes */
        case eNodeAttribute:
        {
          pAttr = pChild->node.pAttrib;

          /* "Attrib */
          cb = LENSTR(pAttr->lpszName);

          if(cb)
          {
            if (lpszMarker)
            {
              _tcscpy(&lpszMarker[nResult], pAttr->lpszName);             
            }

            nResult += cb;

            /* "Attrib=Value " */
            cb = LENSTR(pAttr->lpszValue);

            if (cb)
            {
              if (lpszMarker)
              {
                lpszMarker[nResult] = _T('=');
                _tcscpy(&lpszMarker[nResult+1], 
                    pAttr->lpszValue);                  
              }
              nResult += cb + 1;
            }

            if (lpszMarker)
            {
              lpszMarker[nResult] = _T(' ');                              
            }

            nResult++;              
          }
        }
        break;

        case eNodeEmpty:
            continue;

        default:
        /* If this node isn't an attribute then flag that this element has children. */
          bHasChildren = TRUE;
      }
    }
        
    /* If there are child nodes we need to terminate the start tag */
    if (bHasChildren)
    {   
      if (lpszMarker)
      {
        lpszMarker[nResult-1] = _T('>');
      }

      if (nFormat != -1)
      {
        if (lpszMarker)
        {
          lpszMarker[nResult] = _T('\n');
        }

        nResult++;
      }
    }
    else
    {
      nResult--;
    }
	}

  /* Calculate the child format for when we recurse.  This is used to 
   * determine the number of spaces used for prefixes. */
	if (nFormat == -1)
	{
		nChildFormat = -1;
	}
	else
	{
		if (cbElement) 
		{
			nChildFormat = nFormat + 1;
		}
		else
		{
			nChildFormat = nFormat;
		}	
	}

   /* Enumerate through remaining children */
	nIndex = 0;	
	while (pChild = ZonaXml::EnumNodes(pEntry, &nIndex))
	{
		switch(pChild->type)
		{
       /* Text nodes */
		case eNodeText:
			{
        /* "Text" */
        cb = LENSTR(pChild->node.pText->lpszValue);

        if (cb)
        {
					if (nFormat != -1)
					{
            if (lpszMarker)
            {
              memset(&lpszMarker[nResult], INDENTCHAR, 
                sizeof(TCHAR)*(nFormat + 1));                       

              _tcscpy(&lpszMarker[nResult + nFormat + 1], 
                pChild->node.pText->lpszValue);

              lpszMarker[nResult + nFormat + 1 + cb] = 
                _T('\n');
            }

            nResult += cb + nFormat + 2;
					}
					else
					{
						if (lpszMarker)
            {
              _tcscpy(&lpszMarker[nResult], pChild->node.pText->lpszValue);
            }

						nResult += cb;
					}
        }
			}
			break;

      /* Clear type nodes */
		  case eNodeClear:
		  {
        /* "OpenTag" */
        cb = LENSTR(pChild->node.pClear->lpszOpenTag);

        if(cb)
        {
          if(nFormat != -1)
          {
            if (lpszMarker)
            {
            memset(&lpszMarker[nResult], INDENTCHAR, sizeof(TCHAR)*(nFormat + 1));                       

            _tcscpy(&lpszMarker[nResult + nFormat + 1], pChild->node.pClear->lpszOpenTag);

              /* lpszMarker[nResult + nFormat + 1 + cb] = 
              _T('\n'); */
            }

            /* nResult += cb + nFormat + 2; */
            nResult += cb + nFormat + 1;
				  }
				  else
				  {
            if (lpszMarker)
            {
              _tcscpy(&lpszMarker[nResult],  pChild->node.pClear->lpszOpenTag);
            }

            nResult += cb;
          }
        }

        /* "OpenTag Value" */
        cb = LENSTR(pChild->node.pClear->lpszValue);

        if (cb)
        {
          if (lpszMarker)
          {
            _tcscpy(&lpszMarker[nResult], pChild->node.pClear->lpszValue);
          }

          nResult += cb;
        }

        /* "OpenTag Value CloseTag" */
        cb = LENSTR(pChild->node.pClear->lpszCloseTag);

        if (cb)
        {
          if (lpszMarker)
          {
            _tcscpy(&lpszMarker[nResult], pChild->node.pClear->lpszCloseTag);
          }

				  nResult += cb;
			  }

			  if (nFormat != -1)
			  {
				  if (lpszMarker)
				  {
					  lpszMarker[nResult] = _T('\n');
				  }

				  nResult++;
			  }
		  }
		  break;

      /* Element nodes */
		  case eNodeElement:
		  {
        /* Recursively add child nodes */
        nResult += ZonaXml::CreateXMLStringR(pChild->node.pElement, 
          lpszMarker ? lpszMarker + nResult : 0, nChildFormat);
	    }
    }
	}

	if (cbElement)
	{
       /*
        *********************************************************************
        * If we have child entries we need to use long XML notation for 
		* closing the element - "<elementname>blah blah blah</elementname>"
        *********************************************************************
        */
        if (bHasChildren)
        {
           /*
            *****************************************************************
            * "</elementname>\0"
            *****************************************************************
            */
            if (lpszMarker)
            {
                if (nFormat != -1)
                {
                    if (nFormat)
                    {
                        memset(&lpszMarker[nResult], INDENTCHAR, 
                            sizeof(TCHAR)*nFormat);

                        nResult+=nFormat;
                    }
                }       

                _tcscpy(&lpszMarker[nResult], _T("</"));        
                nResult += 2;
                _tcscpy(&lpszMarker[nResult], pEntry->lpszName);
                nResult += cbElement;
                
                if (nFormat == -1)
                {
                    _tcscpy(&lpszMarker[nResult], _T(">"));
                    nResult++;
                }
                else
                {
                    _tcscpy(&lpszMarker[nResult], _T(">\n"));
                    nResult += 2;
                }
            }
            else
            {
                if (nFormat != -1)
                {
                    nResult += cbElement + 4 + nFormat;
                }
                else
                {
                    nResult += cbElement + 3;
                }
            }       
        }
       /*
        *********************************************************************
        * If there are no children we can use shorthand XML notation - 
        * "<elementname/>"
        *********************************************************************
        */
        else 
        {
           /*
            *****************************************************************
            * "/>\0"
            *****************************************************************
            */
            if (lpszMarker)
            {
                if (nFormat == -1)
                {
                    _tcscpy(&lpszMarker[nResult], _T("/>")); 
                    nResult += 2;
                }
                else
                {
                    _tcscpy(&lpszMarker[nResult], _T("/>\n")); 
                    nResult += 3;
                }
            }
            else
            {
                nResult += nFormat == -1 ? 2 : 3;
            }
        }
	}

	return nResult;

}/* CreateXMLStringR */

/**
 * <P> Create an XML string from the head element.  </P>
 *
 * @methodName  CreateXMLString
 *
 * @param       XMLElement * pHead -	head element
 * @param       int nFormat			 -  0 if no formatting is required 
 *										otherwise nonzero for formatted text
 *										with carriage returns and indentation.
 * @param       int *pnSize			 -	[out] pointer to the size of the 
 *										returned string not including the 
 *										NULL terminator.
 *
 * @return      PTCHAR	-	Allocated XML string, you must free	this with free().
 *****************************************************************************/
TCHAR *CreateXMLString(XMLElement *pHead, int nFormat, int *pnSize)
{
	TCHAR *lpszResult = NULL;
	int cbStr;

	if (pHead)
	{
    /** Recursively Calculate the size of the XML string */
		nFormat = nFormat ? 0 : -1;
		cbStr = ZonaXml::CreateXMLStringR(pHead, 0, nFormat);
		assert(cbStr);
			
    /** Alllocate memory for the XML string + the NULL terminator and
     * create the recursively XML string. */
		lpszResult = (TCHAR*)malloc((cbStr+1) * sizeof(TCHAR));

		if (lpszResult)
		{
			ZonaXml::CreateXMLStringR(pHead, lpszResult, nFormat);
    	if (pnSize) *pnSize = cbStr;
		}
	}
	return lpszResult;
}/* CreateXMLString */