// text.h : header file of graphic text class
//
//          by Yann Chou

#ifndef _TEXT_H_
#define _TEXT_H_

#include "Font.h" 

// Definations of set in CreateFont() of Text
#define FONT_SET_ASCII  0
#define FONT_SET_DBCS   1

// Definations of fmt in CreateFont() of Text
#define FONT_FMT_0      0   // font format 0 (bit font)   
#define FONT_FMT_1      1   // font format 1 (smooth font)

// Definations of maximum len of slowing string in Text
#define MAX_SLOWING_STR_LEN  800


/////////////////////////////////////////////////////////////////////////////
// Globals

extern char **StrTable;     // table of all language strings
extern int    StrMaxID;    

int  LoadStringTable(const char *filename);
void UnloadStringTable(void);


/////////////////////////////////////////////////////////////////////////////
// Text - text class

class Text
{
private:
    Font  *Ascii;       // Single Bytes Character Set (ASCII)
    Font  *Dbcs;        // Double Bytes Character Set

    // Display window
    int    Left;        // diaplay area
    int    Top;  
    int    Width;
    int    Height;
    
    int    HSpace;      // horizontal space
    int    VSpace;      // vertical space

    int    RowHeight;   // row height
    
    int    CurPosX;     // current/cursor position to left-top
    int    CurPosY;                   
    
    // Slowing output
    BYTE   SlowingStr[MAX_SLOWING_STR_LEN];
    BYTE  *NextCh;
    int    DelayCount;   
    int    DelayInit, DelayInc;
        

public:
    Text();      
    ~Text();

    int  LoadFont(int set, const char *filename, int w, int h, int fmt, int cache);
    
    void SetColor(int fg, int bg, int opacity = 16);
    void GetColor(int &fg, int &bg);

    void SetOpacity(int opacity = 16);
    int  GetOpacity(void);

    void SetDisplayPage(int pageno);

    void EnableDouble(int enable);

    void EnableDropShadow(int enable);
    void SetDropShadow(int color, int dist);    // will EnableDropShadow(1)
    
    void PrintfXY(int x, int y, const char *str,...);
    void PutsXY(int x, int y, const char *str);
    int  PutcXY(int x, int y, int highByte, int lowByte);
    int  PutcXY(int x, int y, int ch);

    void PrintfXY(int x, int y, int strID,...);
    void PutsXY(int x, int y, int strID);

    // Display window
    void SetWindow(int l, int t, int r, int b);

    void ClearWindow(int color = 0);
    void ScrollUp(int lines = 1);
    void LineFeed(void);    

    void SetSpace(int h, int v)         { HSpace = h; VSpace = v; }

    void SetCurPos(int x, int y)        { CurPosX = x; CurPosY = y; }
    int  GetCurPosX(void) const         { return CurPosX; }
    int  GetCurPosY(void) const         { return CurPosY; }
    
    void Printf(const char *str,...);
    void Puts(const char *str);
    int  Putc(int ch);
    int  Putc(int hightByte, int lowByte);

    void Printf(int strID,...);
    void Puts(int strID);

    int  GetTextHeight(const char *str);
    int  GetTextWidth(const char *str);
//  void ControlCode(BYTE operateType, BYTE value);

    // Slowing output
    void Putss(const char *str);

    void SetDelay(int init, int inc)    { DelayInit = init; DelayInc = inc; }

    int  IsSlowing(void) const;
    int  EndOfSlowing(void);    // 1: succeed 0: already end
};


#endif  // _TEXT_H_
