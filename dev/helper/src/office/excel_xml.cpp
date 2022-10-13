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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
#include <parser\html-parser.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "excel_xml.h"


//_mstring s_ExcelXMLEOFChar = _EOL_;

//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
ExcelXML::ExcelXML()
{
}


//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
ExcelXML::~ExcelXML()
{
}


//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
void ExcelXML::SetEOLChar( const char_t* eol )
{
    html::Parser::SetEOLChar( eol );
}


//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
_mstring ExcelXML::Encode( const char_t* datastr )
{
    //_mstring content = HTML::Parser::ToStringData(datastr);
    //int replen = (strlen(content.c_str())*5)+1;
    //char* outs = new char[replen];
    //strreplace(outs, replen, content.c_str(), s_ExcelXMLEOFChar.c_str(), "&#10;");
    //content = outs;
    //delete [] outs;
    //return content;
    // 因為excel不給用 \t
    unsigned int pos = 0;
    _mstring text = html::Parser::Encode( datastr );

    while ( ( pos = text.find( "\t", pos ) ) != text.npos )
    {
        text.replace( pos, 1, "&#09;" );
        pos += 5;
    }

    return text;
    //return HTML::Parser::Encode(text.c_str());
    /*
    if(datastr == NULL)
        return "";
    if(strcmp(datastr, "") == 0)
        return "";
    _mstring datacontent = datastr;
    int replen = (datacontent.size()*5)+1;
    char* repline = new char[replen];
    strreplace(repline, replen, datacontent.c_str(), "\r\n", "&#10;");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), "\"", "&quot;");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), "<", "&lt;");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), ">", "&qt;");
    datacontent = repline;
    SAFE_DELETE_ARRAY(repline);
    return datacontent;
    */
}

//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
_mstring ExcelXML::Decode( const char_t* datastr )
{
    //int replen = (strlen(datastr)*5)+1;
    //char* outs = new char[replen];
    //strreplace(outs, replen, datastr, "&#10;", s_ExcelXMLEOFChar.c_str());
    //_mstring content = HTML::Parser::FromStringData(outs);
    //delete [] outs;
    //return content;
    unsigned int pos = 0;
    _mstring text = datastr;

    while ( ( pos = text.find( "&#09;", pos ) ) != text.npos )
    {
        text.replace( pos, 5, "\t" );
    }

    return html::Parser::Decode( datastr );
    /*
    if(datastr == NULL)
        return "";
    if(strcmp(datastr, "") == 0)
        return "";
    _mstring datacontent = datastr;
    int replen = (datacontent.size()*5)+1;
    char* repline = new char[replen];
    strreplace(repline, replen, datacontent.c_str(), "&#10;", "\r\n");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), "&quot;", "\"");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), "&lt;", "<");
    datacontent = repline;
    strreplace(repline, replen, datacontent.c_str(), "&qt;", ">");
    datacontent = repline;
    SAFE_DELETE_ARRAY(repline);
    return datacontent;
    */
}

//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
_mstring GetChildrenText( XMLNode* cell )
{
    _mstring content;
    XMLNode* child = cell->FindFirstNode( NULL );

    if ( child != NULL )
    {
        while ( child )
        {
            content = content + GetChildrenText( child );
            child = cell->FindNextNode( NULL );
        }

        return content;
    }

    return cell->text_;
}

_mstring ExcelXML::GetDataNode( XMLNode* cell )
{
    _mstring datacontent = GetChildrenText( cell );
    /*
    _mstring datacontent;
    XMLNode* data = cell->FindFirstNode("Data");
    if(data !=NULL )
    {
        datacontent = data->text_;
    }
    else
    {
        data = cell->FindFirstNode("ss:Data");
        if(data !=NULL )
        {
            XMLNode* font = data->FindFirstNode("Font");
            while(font)
            {
                datacontent = datacontent + font->text_;
                font = data->FindNextNode("Font");
            }
        }
    }
    */
    return ExcelXML::Decode( datacontent.c_str() );
}

//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
int ExcelXML::Load( _IN const char_t* inpath )
{
    if ( !root_xml_.LoadFromFile( inpath ) )
    {
        return -1;
    }

    XMLNode* workbook = root_xml_.FindNode( "Workbook" );

    if ( workbook )
    {
        XMLNode* worksheet = workbook->FindFirstNode( "Worksheet" );

        while ( worksheet != NULL )
        {
            //XMLAttribute* attr = worksheet->FindAttribute( "ss:Name" );
            XMLNode* table = worksheet->FindNode( "Table" );

            if ( table )
            {
                XMLNode* row = table->FindFirstNode( "Row" );

                while ( row )
                {
                    XMLNode* cell = row->FindFirstNode( "Cell" );

                    while ( cell )
                    {
                        _mstring datas = GetDataNode( cell );
                    }

                    row = table->FindNextNode( "Row" );
                }
            }

            worksheet = workbook->FindNextNode( "Worksheet" );
        }
    }

    return 0;
}


//=====================================================================================
// NAME:
// DESC:
//=====================================================================================
int ExcelXML::Save( _IN const char_t* inpath )
{
    return 0;
}


