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
// . CXXOO.FNT: 408(Full SBCS & Symbol) + 63(Blank) + 5401(BIG5) + ...
// . Exx00.FNT: 96(Visiable SBCS)
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

	//LoadFont(FONT_SET_SBCS, "..\\Em0612.fnt",  6, 12, FONT_FMT_0, 1);
	//LoadFont(FONT_SET_DBCS, "..\\Cm1212.fnt", 12, 12, FONT_FMT_0, 1);
}

Text::~Text()
{
    if (SBCS != NULL)
        delete SBCS;

    if (DBCS != NULL)
        delete DBCS;
}

int  Text::LoadFont(int set, const char *filename, int w, int h, int fmt, int cache)
{
    Font  *font;

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

void Text::SetCanvas(HPINMAP* ca)
{
	SBCS->SetCanvas(ca);
	DBCS->SetCanvas(ca);
	return;
}

void Text::SetColor(int fg, int bg, int opacity)
{
    SBCS->SetColor(fg, bg, opacity);
    DBCS->SetColor(fg, bg, opacity);
}

void Text::GetColor(int &fg, int &bg)
{
    fg = SBCS->GetFGColor();
    bg = SBCS->GetBGColor();
}

void Text::SetOpacity(int opacity)
{
    SBCS->SetOpacity(opacity);
    DBCS->SetOpacity(opacity);
}

int  Text::GetOpacity(void)
{
    return SBCS->GetOpacity();
}

void Text::SetDisplayPage(int pageno)
{
    SBCS->SetDisplayPage(pageno);
    DBCS->SetDisplayPage(pageno);
}

void Text::EnableDouble(int enable)
{
    SBCS->EnableDouble(enable);
    DBCS->EnableDouble(enable);
}

void Text::EnableDropShadow(int enable)
{
    SBCS->EnableDropShadow(enable);
    DBCS->EnableDropShadow(enable);
}

void Text::SetDropShadow(int color, int dist)
{
    SBCS->SetDropShadow(color, dist);
    DBCS->SetDropShadow(color, dist);
}

void Text::PrintfXY(int x, int y, const char *str,...)
{
	if( str == 0 )
		return;

    static char  dlin[256];

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, str);
    vsprintf(dlin, str, ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
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
	if( str == 0 )
		return;

    BYTE  *s = (BYTE *) str;

    while (*s)
    {
        // Check if it's a SBCS char.
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
    // This character is a SBCS English character set
    SBCS->Draw(x, y, ch - 0x20);

    return SBCS->GetWidth() + HSpace;
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

    DBCS->Draw(x, y, serial);

    return DBCS->GetWidth() + HSpace;
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
        // Check if it's a SBCS char.
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

    BYTE  *s = (BYTE *) StrTable[strID];

    while (*s)
    {
        // Check if it's a SBCS char.
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
        // Check if it's a SBCS char.
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
        // Check if it's a SBCS char.
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

    BYTE    *dl = (BYTE *) dlin;
    va_list  ap;

    va_start(ap, strID);
    vsprintf(dlin, StrTable[strID], ap);
    va_end(ap);

    while (*dl)
    {
        // Check if it's a SBCS char.
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
        // Check if it's a SBCS char.
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
    return __max(SBCS->GetHeight(), DBCS->GetHeight()) + VSpace;
}

int  Text::GetTextWidth(const char *str)
{
    BYTE  ch;

    int   sb = 0;
    int   db = 0;

    while ((ch = (BYTE) *(str++)) != 0x00)
    {       
        // Check if it's a SBCS char.        
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

    return sb * SBCS->GetWidth() + db * DBCS->GetWidth() + space;
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
        // Check if it's a SBCS char.
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

        // Check if it's a SBCS char.
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

/*
void Text::ControlCode(int operateType, int value)
// NOTE: the value can't be 0x00 because it is a EOS char.
{
    switch (operateType)
    {
    case 0x01:                  // draw SBCS code character
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
