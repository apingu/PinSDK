//////////////////////////////////////////////////////////////////////////
//
// Create by Kent 2004/06/02
//
//
//
//
//
//
//          2008/7/12  : 我盡力改了 可是kent作的這個機制 太難綁進去了 很醜也沒辦法了by Pin
//
//
//
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//
#include <graphic\icon.h>
#include <graphic\vmc-text.h>

//03. == 其他 使用 函式庫的.h 文件

//4
#include "text_node.h"


#ifdef UNICODE
// 將 Ansi 文字轉入 unicode 文字, defined in Text.cpp
extern bool Ansi2Unicode( uint_t codepage, const tchar_t* ansistr, wchar_t* output );
#endif



// typedef unsigned int (*fpTextNodeFuncTransform)(unsigned int);
// 
// static PixelFormat::  s_ColorFmt=(PixelFormat::)0;
// 
// // ====================================================================
// 
// static fpTextNodeFuncTransform    s_TextNodeFuncPixelTransform=_RGBTo565;
// void TextNodeSelectRGBTransform()
// {
//     PixelFormat:: f=CPIcon::GetPixelFmt();
//     if(s_ColorFmt!=f)
//     {
//         switch(CPIcon::GetPixelFmt())
//         {
//         case PIXEL_R5G6B5:
//             s_TextNodeFuncPixelTransform = _RGBTo565;
//             break;
//         case PIXEL_R5G5B5:
//             s_TextNodeFuncPixelTransform = _RGBTo555;
//             break;
//         case PIXEL_R8G8B8:
//         case PIXEL_A8R8G8B8:
//             break;
//         default:
//             {
//                 char buf[256];
//                 sprintf(buf,"Unsupported video format 0x%x",f);
//                 ::MessageBox(NULL,buf,"Fatal Error",MB_OK|MB_ICONERROR);
//                 abort();
//             }
//         }
//         s_ColorFmt=f;
//     }
// 
// 
// }



//////////////////////////////////////////////////////////////////////////
//建構子
//////////////////////////////////////////////////////////////////////////
cText_Node::cText_Node()
{
    m_TextNode.clear();

    m_TextMethod = TMNormal;
    m_DrawTextLine = 0;
    m_ShadowColor = 0X400808;
    m_Mask = false;
}

cText_Node::~cText_Node()
{
    ReleaseAllTextNode();
}

void cText_Node::ReleaseAllTextNode()
{
    //if(m_TextNode.empty())
    //    return;
    NodeList::iterator iter = m_TextNode.begin();

    while( iter != m_TextNode.end() )
    {
        delete ( *iter );
        iter++;
    }
    m_TextNode.clear();
}

void cText_Node::SetTextColor( int color, int shadow, int index )
{
    if( m_TextNode.empty() )
        return;
    else
    {
        cText_Node::tNode* n = GetTextNode( index );
        if( n != NULL )
        {
            n->m_ForeColor = color;
        }
    }
    m_ShadowColor = shadow;
}


//////////////////////////////////////////////////////////////////////////
//新增(ANSI)文字
//////////////////////////////////////////////////////////////////////////
void cText_Node::AddTextNode( const tchar_t* str, int top, int bottom, int left, int right,
    int fg, int bg,
    int x_space, int y_space, int x_size, int y_size,
    bool next_line )
{
    tNode *node = new tNode;

    node->m_TextRect( left, top, right, bottom );
    node->m_FontSize( x_size, y_size );
    node->m_TextSpace( x_space, y_space );
    //node->m_StartPos    = 0;
    node->m_ForeColor = fg;
    node->m_BackColor = bg;

    node->m_AutoNextLine = next_line;

    //node->m_InsertPos    = 0;
    node->m_DefaultText = '\0';
    node->m_CursorSign = '_';
    node->m_Cursor = false;

    node->m_MaxLength = 65535;//((right - left) / (x_size + x_space)) * ((bottom-top) / (y_size + y_space));
    node->m_OverMaxLen = false;

    //node->m_InsertPos = _STRLEN(str);
    //node->m_StartPos = 0;

    m_TextNode.push_back( node );

#ifdef UNICODE
    wchar_t* wstr = new wchar_t[ strlen( str ) + 1 ];
    Ansi2Unicode( s_CodePage, str, wstr );
    node->m_Text = stdstring( wstr );
    delete wstr;
#else
    //node->m_Text        = str;
#endif

    SetText( str, m_TextNode.size() - 1 );
}

/*
#ifdef UNICODE
//////////////////////////////////////////////////////////////////////////
//新增(UNICODE)文字
//////////////////////////////////////////////////////////////////////////
void cText_Node::AddTextNode(    const wchar_t* str, int top, int bottom, int left, int right,
                                int fg, int bg, int x_size, int y_size,
                                int x_space, int y_space, bool next_line            )
{
    tNode *node = new tNode;

    node->m_TextRect(left,top,right,bottom);
    node->m_FontSize(x_size,y_size);
    node->m_TextSpace(x_space,y_space);
    node->m_StartPos    = 0;
    node->m_ForeColor    = fg;
    node->m_BackColor    = bg;
    node->m_Text        = str;
    node->m_AutoNextLine= next_line;

    node->m_InsertPos    = 0;
    node->m_DefaultText    = '\0';
    node->m_CursorSign    = '_';
    node->m_Cursor        = false;

    node->m_MaxLength    = (right - left) / (x_size + x_space);
    node->m_OverMaxLen    = false;
    m_TextNode.push_back(node);
}
#endif
*/

//////////////////////////////////////////////////////////////////////////
//修改文字(ANSI)
//////////////////////////////////////////////////////////////////////////
void cText_Node::SetTextNode( unsigned int index,
    const tchar_t* str,
    int top, int bottom, int left, int right,
    int fg, int bg,
    int x_space, int y_space, int x_size, int y_size,
    bool over, int max_len )
{
    // todo 
    if( index >= m_TextNode.size() )
    {
        if( index == 0 )
        {
            AddTextNode( str, top, bottom, left, right, fg, bg, x_space, y_space, x_size, y_size );//, over, max_len);
        }
        return;
    }

    tNode* node = m_TextNode[ index ];
    if( str != NULL )
    {
#ifndef UNICODE
        node->m_Text = str;
#else
        wchar_t* wstr = new wchar_t[ strlen( str ) + 1 ];
        Ansi2Unicode( s_CodePage, str, wstr );
        node->m_Text = wstr;
        delete wstr;
#endif
    }
    if( fg != -2 )
        node->m_ForeColor = fg;
    if( bg != -2 )
        node->m_BackColor = bg;

    node->m_TextRect( left, top, right, bottom );
    //     if (max_len < 0)
    //         node->m_MaxLength = ((right - left) / (node->m_FontSize.cx + node->m_TextSpace.cx)) * ((bottom-top) / (node->m_FontSize.cy + node->m_TextSpace.cy));
    //     else
    //         node->m_MaxLength = max_len;
    node->m_OverMaxLen = over;
}

/*
#ifdef UNICODE
//////////////////////////////////////////////////////////////////////////
//修改文字(Unicode)
//////////////////////////////////////////////////////////////////////////
void cText_Node::SetTextNode(    unsigned int index, const wchar_t* str,
                                int top, int bottom, int left, int right,
                                int fg, int bg, bool over, int max_len        )
{
    // todo
    if (index >= m_TextNode.size())
        return;

    tNode* node = m_TextNode[index];
    if (str != NULL)
        node->m_Text = str;
    if (fg != -2)
        node->m_ForeColor = fg;
    if (bg != -2)
        node->m_BackColor = bg;

    node->m_TextRect(left, top, right, bottom);
    if (max_len < 0)
        node->m_MaxLength = (node->m_TextRect.right);
    else
        node->m_MaxLength = max_len;
    node->m_OverMaxLen = over;
}
#endif
*/

//////////////////////////////////////////////////////////////////////////
//修改文字狀態(ANSI)
//////////////////////////////////////////////////////////////////////////
void cText_Node::SetTextNodeState( unsigned int index, const tchar_t* str, bool cursor, int insert,
    BYTE def_char, BYTE cursor_sign )
{
    if( index >= m_TextNode.size() )
        return;

    tNode* node = m_TextNode[ index ];
    if( !node )
        return;
    if( str != NULL )
    {
#ifndef UNICODE
        node->m_Text = str;
#else
        wchar_t* wstr = new wchar_t[ strlen( str ) + 1 ];
        Ansi2Unicode( s_CodePage, str, wstr );
        node->m_Text = wstr;
        delete wstr;
#endif
    }
    if( insert > -1 )
        node->m_InsertPos = insert;
    if( def_char != 1 )
        node->m_DefaultText = def_char;
    node->m_CursorSign = cursor_sign;
    node->m_Cursor = cursor;
    node->m_StartPos = 0;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void cText_Node::ReverseCursor( unsigned int index )
{
    if( index >= m_TextNode.size() )
        return;

    m_TextNode[ index ]->m_Cursor = !m_TextNode[ index ]->m_Cursor;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
bool cText_Node::SetText( const TCHAR* data, unsigned int index )
{
    cText_Node::tNode* n = GetTextNode( index );
    if( n == NULL )
        return false;
    n->m_Text = data;
    n->m_InsertPos = strlen( data );
    n->m_StartPos = 0;
    return true;
}

//////////////////////////////////////////////////////////////////////////
//取得索引到的字串資訊
//////////////////////////////////////////////////////////////////////////
cText_Node::tNode* cText_Node::GetTextNode( unsigned int index )
{
    if( index >= m_TextNode.size() )
        return NULL;

    return m_TextNode[ index ];
}

//////////////////////////////////////////////////////////////////////////
//取出索引到的字串資訊(原資料將被移除)
//////////////////////////////////////////////////////////////////////////
cText_Node::tNode* cText_Node::PopTextNode( unsigned int index )
{
    if( index >= m_TextNode.size() )
        return NULL;

    tNode *p = m_TextNode[ index ];

    m_TextNode.erase( m_TextNode.begin() + index );

    return p;
}

//////////////////////////////////////////////////////////////////////////
//使用本物件,可找出傳入點所對應的資料行,列
//LG::Point x:行 y:列  //目前x的功能尚未寫
//    沒選到有資料的地方傳回-1
//////////////////////////////////////////////////////////////////////////

LG::Point cText_Node::GetLineNumberByPoint( LG::Point pos, int start_line, LG::Rect rect )
{
    LG::Point row( -1, -1 );

    if( m_TextNode.empty() )
        return row;

    if( start_line < 0 )
        return row;

    if( ( ( int )m_TextNode.size() ) <= start_line )
        return row;

    NodeList::iterator i = ( m_TextNode.begin() + start_line );
    int    line = rect.top;

    int lineheight = ( *i )->m_FontSize.cy + ( *i )->m_TextSpace.cy;

    row.y = ( pos.y / lineheight ) + start_line;
    if( row.y >= m_TextNode.size() )
        row.y = -1;
    //row.y=m_TextNode.size()-1;
    else if( row.y < 0 )
        row.y = 0;
    return row;

    for( int count = 0; i != m_TextNode.end(); ++i, ++count )
    {
        if( count < start_line )
            continue;

        if( pos.y > rect.top )
        {
            rect.top += ( *i )->m_FontSize.cy + ( ( *i )->m_TextSpace.cy*( count + 1 ) );
            row.y = count;
        }
        else
        {
            return row;
        }
    }
    return row;
}


//////////////////////////////////////////////////////////////////////////
//切割文字,考慮到BSD的限制
//
//傳入想切割的長度,傳回可切的長度
// return == len  or return == len+1
//////////////////////////////////////////////////////////////////////////
int cText_Node::GetMBCSCharLength( const tchar_t* str, int len )
{
    int i = 0;
    for( i = 0; i < len; ++i )
    {
        if( ( ( *str ) > 0x19 ) && ( ( *str ) < 0x7F ) )
        {
            str++;
        }
        else
        {
            str += 2;
            i++;
        }
    }
    return i;
}

/*
void cText_Node::EraseBackspace(tNode* node)
{
    // 1) 不用講，大家都知道
    // 2) 如果cursor在起頭的地方
    if ((node == NULL) || (node->m_InsertPos == 0))
        return;

#ifndef UNICODE
    // 取要刪除的字的字元長度(byte數)
    int temp = GetMBCSCharLength(node->m_Text.c_str(), node->m_InsertPos - 1);
    //
    if (temp == node->m_InsertPos)
        temp -= 2;
#else
    // utf-16都平等
    int temp = node->m_InsertPos - 1;
#endif

    // 從temp處，刪node->m_InsertPos - temp個
    node->m_Text.erase(temp, node->m_InsertPos - temp);
    node->m_InsertPos = temp;
}*/


uint_t cText_Node::s_CodePage = CP_ACP;    // system default
void cText_Node::SetEncoding( const tchar_t* encoding )    // 輸入字串如 "big5","sjis" 等等
{
    static const tchar_t* encodings[] = 
    {
        _T( "big5" ), _T( "sjis" ),    _T( "gb" ), _T( "ksc" ),
        NULL
    };
    static uint_t codepage[] = 
    {
        950, 932, 936, 949
    };
    int i;
    for( i = 0; encodings[ i ] != NULL; i++ )
    {
        if( !strnicmp( encoding, encodings[ i ], strlen( encodings[ i ] ) ) )
        {
            cText_Node::s_CodePage = codepage[ i ];
            return;
        }
    }
    // unknown encoding, no effect
}


void cText_Node::SetMask( bool m )
{
    m_Mask = m;
}

