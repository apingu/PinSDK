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

extern char* *StrTable;     // table of all language strings
extern int    StrMaxID;    

int  LoadStringTable(const tchar_t* filename);
void UnloadStringTable(void);


/////////////////////////////////////////////////////////////////////////////
// Text - text class

class Text
{
private:

    Font   *SBCS;        // Single Bytes Character Set (ASCII)
    Font   *DBCS;        // Double Bytes Character Set

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
    ubyte_t   SlowingStr[MAX_SLOWING_STR_LEN];
    ubyte_t  *NextCh;
    int    DelayCount;   
    int    DelayInit, DelayInc;
        

public:
    Text();      
    ~Text();
    
    Font* GetSBCS(void){ return SBCS; }
    Font* GetDBCS(void){ return DBCS; }

    int   LoadFont(int set, const tchar_t* filename, int w, int h, int d, int fmt, int cache);
    
    void  SetColor(int fg, int bg, int opacity = 16);
    void  GetColor(int &fg, int &bg);

    void  SetOpacity(int opacity = 16);
    int   GetOpacity(void);

    void  SetDisplayPage(int pageno);

    void  EnableDouble(int enable);

    void  EnableDropShadow(int enable);
    void  SetDropShadow(int color, int dist);   // will EnableDropShadow(1)

#ifndef UNICODE
    void  PrintfXY(int x, int y, const tchar_t* str,...);
    void  PutsXY(int x, int y, const tchar_t* str);
    int   PutcXY(int x, int y, int highByte, int lowByte);    // 低層呼叫(其他Put*XY都呼叫這兩個), for DBCS
    int   PutcXY(int x, int y, int ch);                        // 低層呼叫(其他Put*XY都呼叫這兩個), for SBCS

    void  PrintfXY(int x, int y, int strID,...);
    void  PutsXY(int x, int y, int strID);

    void  Printf(const tchar_t* str,...);
    void  Puts(const tchar_t* str);
    int   Putc(int ch);                            // 似乎是當成 console 模式用, 但仍然呼叫 PutcXY
    int   Putc(int hightByte, int lowByte);        // 和上面類似

    void  Printf(int strID,...);
    void  Puts(int strID);
    // Slowing output
    void  Putss(const tchar_t* str);
#else    // UNICODE
    // 臨時測試用的 For UniCode. 由於 Unicode 並沒有 SBCS/DBCS 差別, 所以一個字元傳一個int字碼
    // 暫時只使用 DBCS 部分(SBCS不使用)
    void  PutsXYW(int x, int y, const unsigned short *str);
    int   PutcXYW(int x, int y, int wchr);
    void  PutsXY(int x, int y, const tchar_t* str);
    bool  isCharWide(int code);    // 這邊是用來判斷該 code 是否適用全形, 而非半形
#endif    // UNICODE

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


    int   GetFontWidth(int set);
    int   GetFontHeight(int set);

    int   GetTextHeight(const tchar_t* str);
    int   GetTextWidth(const tchar_t* str);
//  void ControlCode(ubyte_t operateType, ubyte_t value);

    void  SetDelay(int init, int inc)   { DelayInit = init; DelayInc = inc; }

    int   IsSlowing(void) const;
    int   EndOfSlowing(void);   // 1: succeed 0: already end

    virtual void OnBlitRender(int x, int y, ubyte_t* BitMap){};
    virtual void OnTransRender(int x, int y, int colorkey, ubyte_t* BitMap){};
    virtual void OnMask16Render(int x, int y, int maskcolor, ubyte_t* BitMap){};

    //void  SetRenderFunc(PutImageCB funcA,
    //                      PutTransCB funcB,
    //                      PutColormask16 func4bits, void* pdata);
};


#endif  // _TEXT_H_
