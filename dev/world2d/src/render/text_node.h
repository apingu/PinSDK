#ifndef __TEXT_NODE_H__
#define __TEXT_NODE_H__

#pragma warning(disable: 4786)

#include <string>
#include <vector>
#include <Graphic2d.h>
//#include "sprite2d.h"
//#include "vmc-text.h"

#ifndef UNICODE
typedef std::string stdstring;
#else
typedef std::basic_string<wchar_t> stdstring;
#endif

namespace LGB  //game entity render
{
    class VMCText;
    class VMCRender2d;
}
/**
 管理文字的類別。
 */
class cText_Node
{
    friend class LGB::VMCRender2d;

public:

    enum  TextMethod  { TMNormal = 0, TMShadow, TMRange, TMUser = 25, TMMax = 50 };


    struct tNode
    {
        LG::Rect    m_TextRect;
        LG::Size    m_FontSize;
        LG::Size    m_TextSpace;
        int            m_StartPos;
        int            m_ForeColor;
        //int         m_ShadowColor;
        int            m_BackColor;
        bool        m_AutoNextLine;
        stdstring    m_Text;

        int            m_InsertPos;
        bool        m_Cursor;
        tchar_t        m_CursorSign;
        tchar_t        m_DefaultText;
        //給IMEEdit使用
        int            m_MaxLength;
        bool        m_OverMaxLen;
    };
    
    typedef std::vector<tNode*> NodeList;
public:
    /** 建構子。 */
    cText_Node();

    /** 解構子。 */
    virtual ~cText_Node();

    /** 以下三個, 非unicode模式下就是直接轉入, unicode模式下就是轉換 unicode 再轉入 */
    void AddTextNode(    const tchar_t* str,
                        int top = 0,
                        int bottom = 12,
                        int left = 0,
                        int right = 100,
                        int fg = 0X7FFFFFFF,//0xFF000008,
                        int bg = 0x00,
                        int x_space = 0,
                        int y_space = 6,
                        int x_size = 6,
                        int y_size = 12,
                        bool next_line = true);

    void SetTextNode(    unsigned int index,
                        const tchar_t* str,
                        int top = 0,
                        int bottom = 12,
                        int left = 0,
                        int right = 100,
                        int fg = 0X7FFFFFFF,//0xFF000008,
                        int bg = 0x00,
                        int x_space = 0,
                        int y_space = 6,
                        int x_size = 6,
                        int y_size = 12,
                        bool over = true,
                        int max_len = -1);

    void SetTextNodeState(    unsigned int index,
                            const tchar_t* str,
                            bool cursor = false,
                            int insert = -1,
                            BYTE def_char = 1,
                            BYTE cursor_sign = '_');

    bool SetText(const TCHAR* data,  unsigned int index);

// #ifdef UNICODE
//     void AddTextNode(    const wchar_t* str,
//                         int top = 0,
//                         int bottom = 12,
//                         int left = 0,
//                         int right = 100,
//                         int fg = 0x10ff,
//                         int bg = 0x0,
//                         int x_size = 6,
//                         int y_size = 12,
//                         int x_space = 0,
//                         int y_space = 0,
//                         bool next_line = true);
// 
//     void SetTextNode(    unsigned int index,
//                         const wchar_t* str,
//                         int top = 0,
//                         int bottom = 12,
//                         int left = 0,
//                         int right = 100,
//                         int fg = -2,
//                         int bg = -2,
//                         bool over = true,
//                         int max_len = -1);
// 
//     void SetTextNodeState(    unsigned int index,
//                             wchar_t* str,
//                             bool cursor = false,
//                             int insert = -1,
//                             wchar_t def_char = 1,
//                             wchar_t cursor_sign = '_');
// #endif

    void              ReverseCursor(unsigned int index);
    tNode*              GetTextNode(unsigned int index);
    tNode*              PopTextNode(unsigned int index);

    void              ReleaseAllTextNode(void);

    int                   PushText(LGB::VMCText *font, tNode *node, LG::Point* pos);
    void              DrawText(LGB::VMCText *font, LG::Point* pos);
    void              DrawText(LGB::VMCText *font, int start_line, LG::Rect rect);

    size32_t          GetLineSize(void) { return (size32_t)m_TextNode.size(); }

    bool              emtpy(void)         { return m_TextNode.empty(); }

    void              SetTextColor(int color, int shadow = 0X400808, int index = 0);


    //NodeList&        GetNodeList()    { return m_TextNode; }
    //////////////////////////////////////////////////////////////////////////
    //使用本物件,可找出傳入點所對應的資料行,列
    //LG::Point x:行 y:列  //目前x的功能尚未寫
    //    沒選到有資料的地方傳回-1
    //////////////////////////////////////////////////////////////////////////
    LG::Point          GetLineNumberByPoint(LG::Point pos, int start_line, LG::Rect rect);

    //tools
    static int          GetMBCSCharLength(const tchar_t* str, int len);
    //static void        EraseBackspace(tNode *node);

    static void          SetEncoding(const tchar_t* encoding);    // 輸入字串如 "big5","sjis" 等等

    void              SetMask(bool m);
    bool              GetMask() { return m_Mask;}



    TextMethod        m_TextMethod;   // 要繪製文字的func
    LG::Point         m_TextPosition;
    dword_t             m_ShadowColor;

      // TMRange 會用到
    LG::Rect          m_DrawTextRect;
    int               m_DrawTextLine;




protected:

    NodeList        m_TextNode;
    static uint_t        s_CodePage;
    static uint_t     s_ColorFmt;
    bool            m_Mask;

};

#endif
