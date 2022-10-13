//
//
//
//
//
//
//
//
//
//
#ifndef _EXCELXML_H
#define _EXCELXML_H

#include <pf-def.h>
#include <parser\xmlnode.h>

class ExcelXML
{
    XMLNode     root_xml_;
    _mstring    version_;
    _mstring    author_;
    _mstring    company_;

public:

    ExcelXML();
    ~ExcelXML();

    // Load excel 2003 xml file
    int Load( _IN const char* inpath );

    // Save excel 2003 xml file
    int Save( _IN const char* inpath );


    static void     SetEOLChar( const char_t* eol );
    static _mstring Encode( const char_t* ins );
    static _mstring Decode( const char_t* ins );

    static _mstring GetDataNode( XMLNode* cell );
};












#endif //_EXECLXML_H