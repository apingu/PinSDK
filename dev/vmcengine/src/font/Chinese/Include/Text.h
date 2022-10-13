// text.h : header file of graphic text class
//
//          by Yann Chou

#ifndef _TEXT_H_
#define _TEXT_H_

#include "Font.h" 

// Definations of set in CreateFont() of Text
#define FONT_SET_SBCS   0
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
    Font  *SBCS;        // Single Bytes Character Set (ASCII)
    Font  *DBCS;        // Double Bytes Character Set

    // Display only in window
    int    Left;        // diaplay area (by pixels)
    int    Top;  
    int    Width;
    int    Height;
    
    int    HSpace;      // horizontal space (by pixels)
    int    VSpace;      // vertical space

    int    GridW;       // grid width (by pixels)
    int    GridH;       // grid height (by pixels)
    
    int    CursorRow;   // cursor position to left-top (by grids)
    int    CursorCol;                   
    int    CursorX;     // by pixels
    int    CursorY;
    
    // Slowing output
    BYTE   SlowingStr[MAX_SLOWING_STR_LEN];
    BYTE  *NextCh;
    int    DelayCount;   
    int    DelayInit, DelayInc;
        

public:
    Text();      
    ~Text();

    int   LoadFont(int set, const char *filename, int w, int h, int fmt, int cache);

	void  SetCanvas(HPINMAP* ca);

    void  SetColor(int fg, int bg, int opacity = 16);
    void  GetColor(int &fg, int &bg);

    void  SetOpacity(int opacity = 16);
    int   GetOpacity(void);

    void  SetDisplayPage(int pageno);

    void  EnableDouble(int enable);

    void  EnableDropShadow(int enable);
    void  SetDropShadow(int color, int dist);   // will EnableDropShadow(1)
    
    void  PrintfXY(int x, int y, const char *str,...);
    void  PutsXY(int x, int y, const char *str);
    int   PutcXY(int x, int y, int highByte, int lowByte);
    int   PutcXY(int x, int y, int ch);

    void  PrintfXY(int x, int y, int strID,...);
    void  PutsXY(int x, int y, int strID);

    // Display window
    void  SetWindow(int l, int t, int r, int b);

    void  SetSpace(int h, int v);
    void  SetGrid(int w, int h);

    void  SetCursorPos(int row, int col);
    int   GetCursorRow(void) const      { return CursorRow; }
    int   GetCursorCol(void) const      { return CursorCol; }    

    void  ClearWindow(int color = 0);
    void  ScrollUp(int lines = 1);
    void  LineFeed(void);

    void  Printf(const char *str,...);
    void  Puts(const char *str);
    int   Putc(int ch);
    int   Putc(int hightByte, int lowByte);

    void  Printf(int strID,...);
    void  Puts(int strID);

    int   GetTextHeight(const char *str);
    int   GetTextWidth(const char *str);
//  void ControlCode(BYTE operateType, BYTE value);

    // Slowing output
    void  Putss(const char *str);

    void  SetDelay(int init, int inc)   { DelayInit = init; DelayInc = inc; }

    int   IsSlowing(void) const;
    int   EndOfSlowing(void);   // 1: succeed 0: already end
};


#endif  // _TEXT_H_
