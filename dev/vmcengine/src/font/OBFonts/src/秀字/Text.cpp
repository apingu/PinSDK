// text.cpp : implementation of graphic text class
//
//  Developer   Date    Comments
//  yann        960309  reversion 1.3
//              970218  drew to virtual page
//              970626  multi-font supported
//              991028  16-bits mode supported
//              991203  renamed some functions 
//                      font cache supported
//              991204  moved the font class to a single file
//              000118  rewrote display window of text
//              000304  string table supported
//              000318  slowing output supported
//
// NOTE:
// . CXXOO.FNT: 408(Full ASCII & Symbol) + 63(Blank) + 5401(BIG5) + ...
// . Exx00.FNT: 96(Visiable ASCII)
// . future enhancement:  1. Add variable english font

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "Text.h"

char       **StrTable = NULL;   // table of all language string
int          StrMaxID = -1; 

static char *StrBuff  = NULL;   // buffer of string table


/////////////////////////////////////////////////////////////////////////////
// Text class

Text::Text() 
{
    Ascii = NULL;
    Dbcs  = NULL;
    
    Left = Top = Width = Height = 0;

    HSpace = VSpace = 0;

    RowHeight = 0;

    NextCh = NULL;

    SetDelay(8, 8);
}

Text::~Text()
{
    if (Ascii != NULL)  
        delete Ascii;

    if (Dbcs != NULL)
        delete Dbcs;
}

int  Text::LoadFont(int set, const char *filename, int w, int h, int fmt, int cache)
{
    Font *font;

    // Create character set
    if (fmt == FONT_FMT_1)
        font = new SmoothFont;
    else if (fmt == FONT_FMT_0)
        font = new Font;
    else
        return 0;   // error font format

    if (!font->Create(filename, w, h, cache))   
        return 0;   // create failed

    // Set font 
    if (set == FONT_SET_ASCII)
    {
        if (Ascii != NULL)
            delete Ascii;
        Ascii = font;
    }
    else if (set == FONT_SET_DBCS)
    {
        if (Dbcs != NULL)
            delete Dbcs;
        Dbcs = font;
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
    Ascii->SetColor(fg, bg, opacity);
    Dbcs->SetColor(fg, bg, opacity);
}

void Text::GetColor(int &fg, int &bg)
{
    fg = Ascii->GetFGColor();
    bg = Ascii->GetBGColor();
}

void Text::SetOpacity(int opacity)
{
    Ascii->SetOpacity(opacity);
    Dbcs->SetOpacity(opacity);
}

int  Text::GetOpacity(void)
{
    return Ascii->GetOpacity();
}

void Text::SetDisplayPage(int pageno)
{
    Ascii->SetDisplayPage(pageno);
    Dbcs->SetDisplayPage(pageno);
}

void Text::EnableDouble(int enable)
{
    Ascii->EnableDouble(enable);
    Dbcs->EnableDouble(enable);
}

void Text::EnableDropShadow(int enable)
{
    Ascii->EnableDropShadow(enable);
    Dbcs->EnableDropShadow(enable);
}

void Text::SetDropShadow(int color, int dist)
{
    Ascii->SetDropShadow(color, dist);
    Dbcs->SetDropShadow(color, dist);
}

void Text::PrintfXY(int x, int y, const char *str,...)
{
    static char  dlin[256];

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, str);
    vsprintf(dlin, str, ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a ascii char.
        if (*dl > 0x19 && *dl < 0x7F)
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

void Text::PutsXY(int x, int y, const char *str)
{
    BYTE *s = (BYTE *) str;

    while (*s)
    {
        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            x += PutcXY(x, y, *s);
            s++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *s, *(s + 1));
            s += 2;
        }
    }
}

int  Text::PutcXY(int x, int y, int ch)
{
    // This character is a ASCII English character set
    Ascii->Draw(x, y, ch - 0x20);

    return Ascii->GetWidth() + HSpace;
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

    if (lowByte & 0x80)         // 0xA1 <= lowByte <= 0xFE
        serial = serial + lowByte - 0x62;
    else                        // 0x40 <= lowByte <= 0x7E
        serial = serial + lowByte - 0x40;

    Dbcs->Draw(x, y, serial);

    return Dbcs->GetWidth() + HSpace;
}

void Text::PrintfXY(int x, int y, const int strID,...)
{ 
    assert(strID <= StrMaxID);

    static char  dlin[256];

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, strID);
    vsprintf(dlin, StrTable[strID], ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a ascii char.
        if (*dl > 0x19 && *dl < 0x7F)
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

void Text::PutsXY(int x, int y, int strID)
{
    assert(strID <= StrMaxID);

    BYTE *s = (BYTE *) StrTable[strID];

    while (*s)
    {
        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            x += PutcXY(x, y, *s);
            s++;
        }
        else // this is a two bytes code char.
        {
            x += PutcXY(x, y, *s, *(s + 1));
            s += 2;
        }
    }
}

void Text::SetWindow(int l, int t, int r, int b)
{
    // Set display area
    Left   = l;
    Top    = t;
    Width  = r - l + 1;
    Height = b - t + 1;

    CurPosX = CurPosY = 0;

    RowHeight = __max(Ascii->GetHeight(), Dbcs->GetHeight());
}

void Text::ClearWindow(int color)
{
//  vfillbox(Left, Top, Left + Width - 1, Top + Height - 1, color);

    CurPosX = CurPosY = 0;
}

void Text::ScrollUp(int lines)
{
    lines = lines * RowHeight;
/*
    getkey(WAIT);

    for (int i = 0; i < lines; i++)
        ShiftBlock(Left, Top + 1, Right, Bottom - i, \
                   Left, Top); */
}

void Text::LineFeed(void)
{
    int  space = RowHeight + VSpace;

    CurPosX = 0;

    CurPosY += space;

    if ((CurPosY + RowHeight) >= Height)
    {
        ScrollUp();
        CurPosY -= space;
    }
}

void Text::Printf(const char *str,...)
{
    static char  dlin[256];

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, str);
    vsprintf(dlin, str, ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a ascii char.
        if (*dl > 0x19 && *dl < 0x7F)
        {
            Putc(*dl);
            dl++;
        }
        else // this is a two bytes code char.   
        {
            Putc(*dl, *(dl + 1));
            dl += 2;
        }
    }
}

void Text::Puts(const char *str)
{
    BYTE *s = (BYTE *) str;

    while (*s)
    {
        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s += 2;
        }
    }
}

int  Text::Putc(int ch)
{
    int  move;
    int  x, y;

    move = Ascii->GetWidth() + HSpace;

    // Check output position
    if ((CurPosX + move) >= Width)
        LineFeed();

    x = CurPosX + Left;
    y = CurPosY + Top;

    if (!PutcXY(x, y, ch))
        return 0;

    CurPosX += move;

    return move;
}

int  Text::Putc(int highByte, int lowByte)
{
    int  move;
    int  x, y;

    move = Dbcs->GetWidth() + HSpace;

    // Check output position
    if ((CurPosX + move) >= Width)
        LineFeed();

    x = CurPosX + Left;
    y = CurPosY + Top;

    if (!PutcXY(x, y, highByte, lowByte))
        return 0;

    CurPosX += move;

    return move;
}

void Text::Printf(int strID,...)
{
    assert(strID <= StrMaxID);

    static char  dlin[256];

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, strID);
    vsprintf(dlin, StrTable[strID], ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a ascii char.
        if (*dl > 0x19 && *dl < 0x7F)
        {
            Putc(*dl);
            dl++;
        }
        else // this is a two bytes code char.   
        {
            Putc(*dl, *(dl + 1));
            dl += 2;
        }
    }
}

void Text::Puts(int strID)
{
    assert(strID <= StrMaxID);

    BYTE *s = (BYTE *) StrTable[strID];

    while (*s)
    {
        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s += 2;
        }
    }
}

int  Text::GetTextHeight(const char *str)
{
    return __max(Ascii->GetHeight(), Dbcs->GetHeight()) + VSpace;
}

int  Text::GetTextWidth(const char *str)
{
    BYTE  ch;

    int   sb = 0;
    int   db = 0;

    while ((ch = (BYTE) *(str++)) != 0x00)
    {       
        // Check if it's a ascii char.        
        if (ch > 0x19 && ch < 0x7F)
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

    return sb * Ascii->GetWidth() + db * Dbcs->GetWidth() + space;
}

/*
void Text::ControlCode(int operateType, int value)
// NOTE: the value can't be 0x00 because it is a EOS char.
{
    switch (operateType)
    {
    case 0x01:                  // draw ASCII code character
        break;

    case 0x02:                  // scroll up and carriage return
        ScrollUp();             // or ScrollUp( value );
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
        SetColor(value, Dbcs->GetBGColor());
        break;

    case 0x06:                  // change background color
        SetColor(Dbcs->GetFGColor(), value);
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


int  LoadStringTable(const char *filename)
{
    FILE *infile;
    long  num, len;
    long  i;

    infile = fopen(filename, "rb");

    if (infile == NULL)
        return 0;

    num = filelength(fileno(infile));

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

    if (StrBuff[len - 2] != 0x0D && StrBuff[len - 1] != 0x0A)
    {
        StrBuff[len++] = 0x0D;  // last line must end of 0D0A
        StrBuff[len++] = 0x0A;
    }

    // Count lines 
    num = 0;

    for (i = 0; i < len - 1; i++)
    {
        if (StrBuff[i] == 0x0D && StrBuff[i + 1] == 0x0A)
        {
            num++;
            i++;
        }
    }

    // Allocate string table
    typedef char * SRT_PTR;

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

    char *head = StrBuff;

    for (i = 0; i < len - 1; i++)
    {
        if (StrBuff[i] == 0x0D && StrBuff[i+1] == 0x0A)
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

void Text::Putss(const char *str)
{
	if (strcmp(str, (char *) SlowingStr))
	{
        assert(strlen(str) < MAX_SLOWING_STR_LEN);

		strcpy((char *) SlowingStr, str);				
		NextCh = SlowingStr;
  		DelayCount = DelayInit;	
	}

    BYTE *s = SlowingStr;

	SetOpacity(16);

    while (*s && s < NextCh)
    {  	
        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s += 2;
        }
    }
        
    if (*s)
    {
	    SetOpacity(DelayCount);

        // Check if it's a ascii char.
        if (*s > 0x19 && *s < 0x7F)
        {
            Putc(*s);
            s++;
        }
        else // this is a two bytes code char.
        {
            Putc(*s, *(s + 1));
            s += 2;
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

int  Text::IsSlowing(void) const
{
    if (NextCh == NULL)
        return 0;

    return (*NextCh) ? 1 : 0;
}

int  Text::EndOfSlowing(void)
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