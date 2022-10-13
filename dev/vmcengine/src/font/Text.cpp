// text.cpp : implementation of graphic text class
//
//  Developer   Date    Comments
//  yann        030996  reversion 1.3
//              021897  drew to virtual page
//              062697  multi-font supported
//              102899  16-bits mode supported
//              120399  renamed some functions 
//                      font cache supported
//              120499  moved the font class to a single file
//              011800  rewrote display window of text
//              030400  string table supported
//              031800  slowing output supported
//              041401  fixed bugs in window output
//
// NOTE:
// . CXXOO.FNT: 408(Full SBCS  &Symbol) + 63(Blank) + 5401(BIG5) + ...
// . Exx00.FNT: 96(Visiable SBCS)
// . future enhancement:  1. Add variable english font

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pf-debug.h>

#include <windows.h>
#include <malloc.h>

#include "Text.h"

#include <assert.h>
//#include <pfstdio.h>

char       **StrTable = NULL;   // table of all language string
int          StrMaxID = -1; 

static char* StrBuff  = NULL;   // buffer of string table


/////////////////////////////////////////////////////////////////////////////
// Font class

class TextFont : public Font
{
    Text* m_pText;

public:
    TextFont(Text* pText){m_pText=pText;}
    void OnBlitRender(int x, int y, ubyte_t* BitMap)                 {m_pText->OnBlitRender(x, y, BitMap);};
    void OnTransRender(int x, int y, int colorkey, ubyte_t* BitMap)  {m_pText->OnTransRender(x, y, colorkey, BitMap);};

};


class TextSmoothFont : public SmoothFont
{
    Text* m_pText;

public:
    TextSmoothFont(Text* pText){m_pText=pText;}

    void OnBlitRender(int x, int y, ubyte_t* BitMap)                 {m_pText->OnBlitRender(x, y, BitMap);};
    void OnTransRender(int x, int y, int colorkey, ubyte_t* BitMap)  {m_pText->OnTransRender(x, y, colorkey, BitMap);};
    void OnMask16Render(int x, int y, int maskcolor, ubyte_t* BitMap){m_pText->OnMask16Render(x, y, maskcolor, BitMap);};
};

/////////////////////////////////////////////////////////////////////////////
// Text class

Text::Text() 
{
    SBCS = NULL;
    DBCS = NULL;
    
    Left   = 0;
    Top    = 0;
    Width  = 640;
    Height = 480;

    HSpace = 0;
    VSpace = 0;
    
    GridW  = 8;
    GridH  = 8;

    NextCh = NULL;

    SetDelay(8, 8);
}

Text::~Text()
{
    if (SBCS != NULL)
        SAFE_DELETE(SBCS);

    if (DBCS != NULL)
        SAFE_DELETE(DBCS);
}

int  Text::LoadFont(int set, const tchar_t* filename, int w, int h, int d, int fmt, int cache)
{

    Font  *font=NULL;

    // Create character set
    if (fmt == FONT_FMT_1)
    {
        font = new TextSmoothFont(this);
    }
    else if (fmt == FONT_FMT_0)
    {
        font = new TextFont(this);
    }
    else
        return 0;   // error font format


    if (!font->Create(filename, w, h, d, cache))
    {
        delete font;
        return 0;   // create failed
    }


    // Set font 
    if (set == FONT_SET_SBCS)
    {
        if (SBCS != NULL)
            delete SBCS;

        SBCS = font;
    }
    else if (set == FONT_SET_DBCS)
    {
        if (DBCS != NULL)
            delete DBCS;

        DBCS = font;
    }
    else
    {
        delete font;
        return 0;   // error font set
    }
    
    return 1;
}

void Text::SetColor(int fg, int bg, int opacity)
{
    if(SBCS != NULL)
        SBCS->SetColor(fg, bg, opacity);
    if(DBCS != NULL)
        DBCS->SetColor(fg, bg, opacity);
}

void Text::GetColor(int &fg, int &bg)
{
    if(SBCS != NULL)
        fg = SBCS->GetFGColor();
    if(DBCS != NULL)
        bg = DBCS->GetBGColor();
}

void Text::SetOpacity(int opacity)
{
    if(SBCS != NULL)
        SBCS->SetOpacity(opacity);
    if(DBCS != NULL)
        DBCS->SetOpacity(opacity);
}

int  Text::GetOpacity(void)
{
    if(SBCS != NULL)
        return SBCS->GetOpacity();
    return 0;
}

void Text::SetDisplayPage(int pageno)
{
    if(SBCS)
        SBCS->SetDisplayPage(pageno);
    if(DBCS)
        DBCS->SetDisplayPage(pageno);
}

void Text::EnableDouble(int enable)
{
    if(SBCS)
        SBCS->EnableDouble(enable);
    if(DBCS)
        DBCS->EnableDouble(enable);
}

void Text::EnableDropShadow(int enable)
{
    if(SBCS)
        SBCS->EnableDropShadow(enable);
    if(DBCS)
        DBCS->EnableDropShadow(enable);
}

void Text::SetDropShadow(int color, int dist)
{
    if(SBCS)
        SBCS->SetDropShadow(color, dist);
    if(DBCS)
        DBCS->SetDropShadow(color, dist);
}

#ifndef UNICODE
void Text::PrintfXY(int x, int y, const tchar_t* str,...)
{
    static char  dlin[256];

    ubyte_t    *dl = (ubyte_t *) dlin;
    va_list  ap;

    va_start(ap, str);
    vsnprintf(dlin, 256, str, ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
        if (*dl > 0x19 &&*dl < 0x7F)
        {
            x += PutcXY(x, y, *dl);
            dl++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *dl, *(dl + 1));
            dl += 2;
        }
    }
}

void Text::PutsXY(int x, int y, const tchar_t* str)
{
    ubyte_t  *s = (ubyte_t *) str;
    while (*s)
    {
        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            x += PutcXY(x, y, *s);
            s++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }
    }
}

int  Text::PutcXY(int x, int y, int ch)
{
    // This character is a SBCS English character set
    if(SBCS)
    {
        SBCS->Draw(x, y, ch - 0x20);

        //return SBCS->GetWidth() + HSpace;
        return SBCS->GetWidth() + HSpace;
    }
    return 0;
}

int  Text::PutcXY(int x, int y, int highByte, int lowByte)
// BIG-5 Chinese code:
//  Full Symbols         0xA140 ~ 0xA3BF            408 
//                                0xA3C0 ~ 0xA3FE   63  (reserved)
//  Frequent Char. Set   0xA440 ~ 0xC67E            5401
//  Added Symbols                 0xC6A1 ~ 0xC8FE   408
//  Infrequent Char. Set 0xC940 ~ 0xF9D5            7652
//                                            above 13932 
//  Eten Char. Set                0xF9D6 ~ 0xF9FE   41  (system dep. don't use)
//  Custom Char. Set     0xFA40 ~ 0xFEFE            785 (system dep. don't use)
//                                            above 14758
//  Valid hi bytes  is 0xA1 ~ 0xFE
//  Valid low bytes is 0x40 ~ 0x7E, 0xA1 ~ 0xFE
{
    int  serial;

    // This character is a special control code
    if (highByte < 0x20)        // 0x01 <= hb <= 0x1F
    {
//      ControlCode(highByte, lowByte);
        return 0;
    }

    // This character is a BIG5 Chinese character set
    //  transfers it to serial num 0, 1, 2... 
    //  serial = (hb - 0xA1) * 0x9D + (lb - 0x40)        if 0x40 <= lb <= 0x7E
    //  serial = (hb - 0xA1) * 0x9D + (lb - 0xA1) + 0x3F if 0xA1 <= lb <= 0xFE
    
    serial = (highByte - 0xA1) * 0x9D;

    if (lowByte  &0x80)         // 0xA1 <= lowByte <= 0xFE
        serial = serial + lowByte - 0x62;
    else                        // 0x40 <= lowByte <= 0x7E
        serial = serial + lowByte - 0x40;

    DBCS->Draw(x, y, serial);

    return DBCS->GetWidth() + HSpace;
}

void Text::PrintfXY(int x, int y, const int strID,...)
{ 
    assert(strID <= StrMaxID);

    static char  dlin[256];

    ubyte_t    *dl = (ubyte_t *) dlin;
    va_list  ap;

    va_start(ap, strID);
    vsnprintf(dlin, 256, StrTable[strID], ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
        if (*dl > 0x19 &&*dl < 0x7F)
        {
            x += PutcXY(x, y, *dl);
            dl++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *dl, *(dl + 1));
            dl++;
            if((*dl) == 0)
                return;
            dl++;

        }
    }
}

void Text::PutsXY(int x, int y, int strID)
{
    assert(strID <= StrMaxID);
    
    ubyte_t  *s = (ubyte_t *) StrTable[strID];

    while (*s)
    {
        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            x += PutcXY(x, y, *s);
            s++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }
    }
}

void Text::Printf(const tchar_t* str,...)
{
    static char  dlin[256];

    ubyte_t    *dl = (ubyte_t *) dlin;
    va_list  ap;

    va_start(ap, str);
    vsnprintf(dlin, 256, str, ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
        if (*dl > 0x19 &&*dl < 0x7F)
        {
            Putc(*dl);
            dl++;
        }
        else // this is a two bytes code char.   
        {
            Putc(*dl, *(dl + 1));
            dl++;
            if((*dl) == 0)
                return;
            dl++;

        }
    }
}

void Text::Puts(const tchar_t* str)
{
    ubyte_t *s = (ubyte_t *) str;

    while (*s)
    {
        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }
    }
}

int  Text::Putc(int ch)
{
    int  move;
    int  x, y;

    move = SBCS->GetWidth() + HSpace;

    // Check output position
    if ((CursorX + move) >= Width)
        LineFeed();

    x = Left + CursorX; 
    y = Top  + CursorY; 

    if (!PutcXY(x, y, ch))
        return 0;

    CursorCol++;
    CursorX += move;

    return move;
}

int  Text::Putc(int highByte, int lowByte)
{
    int  move;
    int  x, y;
    
    move = DBCS->GetWidth() + HSpace;

    // Check output position
    if ((CursorX + move) >= Width)
        LineFeed();

    x = Left + CursorX; 
    y = Top  + CursorY; 

    if (!PutcXY(x, y, highByte, lowByte))
        return 0;

    CursorCol++;
    CursorX += move;

    return move;
}

void Text::Printf(int strID,...)
{
    assert(strID <= StrMaxID);

    static char  dlin[256];

    ubyte_t    *dl = (ubyte_t *) dlin;
    va_list  ap;

    va_start(ap, strID);
    vsnprintf(dlin, 256, StrTable[strID], ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
        if (*dl > 0x19 &&*dl < 0x7F)
        {
            Putc(*dl);
            dl++;
        }
        else // this is a two bytes code char.   
        {
            Putc(*dl, *(dl + 1));
            dl++;
            if((*dl) == 0)
                return;
            dl++;
        }
    }
}

void Text::Puts(int strID)
{
    assert(strID <= StrMaxID);

    ubyte_t *s = (ubyte_t *) StrTable[strID];

    while (*s)
    {
        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }
    }
}

void Text::Putss(const tchar_t* str)
{
    if (strcmp(str, (char*) SlowingStr))
    {
        assert(strlen(str) < MAX_SLOWING_STR_LEN);

        strcpy((char*) SlowingStr, str);                
        NextCh = SlowingStr;
          DelayCount = DelayInit;    
    }

    ubyte_t *s = SlowingStr;

    SetOpacity(16);

    while (*s &&s < NextCh)
    {      
        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }
    }
        
    if (*s)
    {
        SetOpacity(DelayCount);

        // Check if it's a SBCS char.
        if (*s > 0x19 &&*s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s++;
            if((*s) == 0)
                return;
            s++;
        }

        if (DelayCount < 16)
            DelayCount += DelayInc;
        else
        {           
            DelayCount = DelayInit;    
            NextCh = s + 2;       
        }       
    }
    else
        NextCh = s;
}
#endif    // UNICODE

void Text::SetWindow(int l, int t, int r, int b)
{
    // Set display area
    Left   = l;
    Top    = t;
    Width  = r - l + 1;
    Height = b - t + 1;

    CursorRow = 0;
    CursorCol = 0;
    CursorX   = 0;

    SetGrid(SBCS->GetWidth(), SBCS->GetHeight());
}

void Text::SetSpace(int h, int v)
{ 
    HSpace = h; 
    VSpace = v; 

    GridW += HSpace; 
    GridH += VSpace;
}

void Text::SetGrid(int w, int h)  
{ 
    GridW = w + HSpace; 
    GridH = h + VSpace;   
}

void Text::SetCursorPos(int row, int col)
{ 
    CursorRow = row; 
    CursorCol = col;

    CursorX   = col * GridW;
    CursorY   = row * GridH;
}

void Text::ClearWindow(int color)
{
//  FILLBOX(Left, Top, Left + Width - 1, Top + Height - 1, color);

    SetCursorPos(0, 0);
}

void Text::ScrollUp(int lines)
{
/*  lines = lines * GridH - VSpace;
  
    GetKey(WAIT);

    for (int i = 0; i < lines; i++)
        ShiftBlock(Left, Top + 1, Right, Bottom - i, Left, Top); 
*/
}

void Text::LineFeed(void)
{
    CursorCol = 0;
    CursorX   = 0;

    if ((CursorY + GridH) >= Height)
        ScrollUp();
    else
    {
        CursorRow++;
        CursorY += GridH;        
    }
}


int  Text::GetTextHeight(const tchar_t* str)
{
    return __max(SBCS->GetHeight(), DBCS->GetHeight()) + VSpace;
}

int  Text::GetTextWidth(const tchar_t* str)
{
    ubyte_t  ch;

    int   sb = 0;
    int   db = 0;

    while ((ch = (ubyte_t) *(str++)) != 0x00)
    {       
        // Check if it's a SBCS char.        
        if (ch > 0x19 &&ch < 0x7F)
            sb++;
        else // this is a two bytes code char.
        {
            db++;
            str++;
        }
    }

    int space = 0;

    if (sb) space = (sb - 1) * HSpace;
    if (db) space += (db - 1) * HSpace;

    return sb * SBCS->GetWidth() + db * DBCS->GetWidth() + space;
}


int  Text::IsSlowing(void) const
{
    if (NextCh == NULL)
        return 0;

    return (*NextCh) ? 1 : 0;
}

int Text::EndOfSlowing(void)
{
    if (NextCh == NULL)
        return 0;

    if (*NextCh)
    {
        NextCh = SlowingStr + MAX_SLOWING_STR_LEN - 1;

        *NextCh = 0;

        return 1;
    }
    else
        return 0;
}

/*
void Text::SetRenderFunc(PutImageCB funcA, PutTransCB funcB, PutColormask16 func4bits, void* pdata)
{
    if(SBCS)
    {
        SBCS->SetRenderFunc(funcA,
            funcB, 
            func4bits, pdata);
    }

    if(DBCS)
    {
        DBCS->SetRenderFunc(funcA,
            funcB, 
            func4bits, pdata);
    }
}
*/

/*
void Text::ControlCode(int operateType, int value)
// NOTE: the value can't be 0x00 because it is a EOS char.
{
    switch (operateType)
    {
    case 0x01:                  // draw SBCS code character
        break;

    case 0x02:                  // scroll up and carriage return
        ScrollUp();             // or ScrollUp(value);
        break;

    case 0x03:                  // clear all and carriage return
        ClearWindow();
        break;

    case 0x04:                  // delay or pause
        if (value > 1)
            delay(25 * value);
        else                    // value == 0
            getkey(WAIT);
        break;

    case 0x05:                  // change foreground color
        SetColor(value, DBCS->GetBGColor());
        break;

    case 0x06:                  // change background color
        SetColor(DBCS->GetFGColor(), value);
        break;

    case 0x07:                  // change fonts set
        break;

    case 0x08:                  // initializes setting
        break;

    case 0x09:                  // special effects
        break;

    case 0x0D:                  // carriage return
    case 0x0A:                  // line feed
        LineFeed();
        break;
    }
}
*/


int  LoadStringTable(const tchar_t* filename)
{
    FILE *infile;
    long  num, len;
    long  i;

    infile = fopen(filename, "rb");

    if (infile == NULL)
        return 0;

    num = _filelength(_fileno(infile));

    StrBuff = new char [num + 2];   // add two that maybe we will add 0D0A at
                                    // the end of buffer    
    if (StrBuff == NULL)
    {   
        fclose(infile);
        return 0;
    }

#ifdef _DEBUG
    memset(StrBuff, 0xCC, num);
#endif

    len = fread(StrBuff, sizeof(char), num, infile);

    fclose(infile);

    // Deal speical ending of text file
    if (StrBuff[len - 1] == 0x1A) // banish the EOF
        len--;

    if (StrBuff[len - 2] != 0x0D &&StrBuff[len - 1] != 0x0A)
    {
        StrBuff[len++] = 0x0D;  // last line must end of 0D0A
        StrBuff[len++] = 0x0A;
    }

    // Count lines 
    num = 0;

    for (i = 0; i < len - 1; i++)
    {
        if (StrBuff[i] == 0x0D &&StrBuff[i + 1] == 0x0A)
        {
            num++;
            i++;
        }
    }

    // Allocate string table
    typedef char*  SRT_PTR;

    StrTable = new SRT_PTR [num];

    if (StrTable == NULL)
    {
        delete [] StrBuff;        
        StrBuff = NULL;

        StrMaxID = -1;

        return 0;
    }

    StrMaxID = num - 1;

    // Assign each line to StrTable
    num = 0;

    char* head = StrBuff;

    for (i = 0; i < len - 1; i++)
    {
        if (StrBuff[i] == 0x0D &&StrBuff[i+1] == 0x0A)
        {
            StrBuff[i] = 0;
            StrBuff[i+1] = 0;

            StrTable[num] = head;
            
            head = &StrBuff[i+2];

            num++;
            i++;
        }
    }

    return 1;
}

int Text::GetFontWidth(int set)
{
    if (set == FONT_SET_SBCS)
    {
        return SBCS->GetWidth();
    }
    else if (set == FONT_SET_DBCS)
    {
        return DBCS->GetWidth();
    }
    return 0;
}


int Text::GetFontHeight(int set)
{
    if (set == FONT_SET_SBCS)
    {
        return SBCS->GetHeight();
    }
    else if (set == FONT_SET_DBCS)
    {
        return DBCS->GetHeight();
    }
    return 0;
}

void UnloadStringTable(void)
{
    if (StrBuff)
    {
        delete [] StrBuff;
        StrBuff = NULL;
    }

    if (StrTable)
    {
        delete [] StrTable;
        StrTable = NULL;
    }
    StrMaxID = -1; 
}

// New for test by Wood =====================================================================
#ifdef UNICODE
// 將 Ansi 文字轉入 unicode 文字
bool Ansi2Unicode(unsigned codepage,const char* ansistr,wchar_t* output)
{
    int l;
    l=strlen(ansistr);

    // output is assumed to be as twice as ansistr in length
    l=MultiByteToWideChar(codepage, MB_PRECOMPOSED, ansistr, l, output, l);
    if (l >= 0)
        output[l] = 0;
    return l > 0;
}

bool Unicode2Ansi(unsigned codepage, const wchar_t* unistr, char* buffer, int buflen)
{
    bool usedefault;
    int l=wcslen(unistr);
    if(l>buflen-1)    l=buflen-1;
    l=WideCharToMultiByte(codepage, WC_COMPOSITECHECK, unistr, l, buffer, l,"?", &usedefault);
    if(l >= 0)
        buffer[l]=0;
    return l>0;
}

// 這邊是用來判斷該 code 是否適用全形, 而非半形
// 因為印字的時候可能需要
bool Text::isCharWide(int code)
{
    // 如果沒有 SBCD, 那麼就全程使用 DBCS. DBCS is essential
    return SBCS == NULL || code>=128;
}

int Text::PutcXYW(int x, int y, int code)
{
    int  serial;
    Font *usingfont;

    if(code<32)    return 0;    // control code
/*    if(code>=128){
        usingfont = DBCS;
        serial = code;
    } else if(code>=32){
        usingfont = SBCS;
        serial = code-32;
    } else
        return 0; */
    if(isCharWide(code)){
        usingfont = DBCS;
        serial = code;
    } else {
        usingfont = SBCS;
        serial = code-32;
    }

    usingfont->Draw(x, y, serial);
    return usingfont->GetWidth() + HSpace;
}

void Text::PutsXYW(int x, int y, const  unsigned short *str)
{
    while(*str !=0)
    {
        x += PutcXYW(x, y, *str);
        str++;
    }
}

void  Text::PutsXY(int x, int y, const char* str)
{
    wchar_t* newstr=(wchar_t*)alloca((strlen(str)+1)*sizeof(wchar_t));
    Ansi2Unicode(CP_ACP,str,newstr);
    PutsXYW(x,y,newstr);
}

#endif
