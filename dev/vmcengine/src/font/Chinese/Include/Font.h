// font.h : header file of font class
//
//          by Yann Chou

#ifndef _FONT_H_
#define _FONT_H_

#include <stdio.h>      // for FILE
#include "..\..\..\2DEngine\VMC.h"

typedef unsigned char   BYTE;
typedef unsigned short  WORD;



/////////////////////////////////////////////////////////////////////////////
// Font - font class

class Font
{
protected:

	VMC     vmc;
	HPINMAP *canvas;

	char *BitMap;           // buffer for font drawing

    int   EnBitsPerPixel;	// encode bits per pixel in packed font
    int   DeBytesPerPixel;	// decode bytes per pixel in bitmap

    int   PackedSize;
    BYTE *PackedFont;       // font data of darwing
    
    int   _OctanSize;   	// (W * H) / 8
    int   _Exceeding;   	// (PackedSize == _OctanSize) ? 0 : 8 - (W * H % 8) 

    FILE *FontFile;         // where r the fonts      
    BYTE *FontCache;    	// use extra memory to store fonts
    int   FontNum;

    // Properties
    int   Width;            // font size
    int   Height;

    int   ForeGround;       // foreground color
    int   BackGround;       // background color

    int   Opacity;          // opacity (0: No drawing ~ 16:100%)

    int   DisplayPage;      // display to which vpage (dud! always to active vpage this moment)

    // Effects
    int   Double;           // double size (0: No double, 1: Yes)
  
    int   DropShadow;       // with drop shadow color
    int   DropDist;         // drop distance (0: No drop shadow)

    int   Outline;          // with outline color
    int   OutDist;          // out distance (0: No outline)    

public:
    Font();
    virtual ~Font();

    virtual int  Create(const char *filename, int width, int height, int cache = 0);
    virtual void Destroy(void);

    virtual void Draw(int x, int y, int serial);

    virtual void SetColor(int fg, int bg, int opacity = 16);   

    virtual void SetOpacity(int opacity = 16)   { Opacity = opacity; }

	void SetCanvas(HPINMAP *ca)                 { canvas = ca;}
    void EnableDouble(int enable)               { Double = enable;   }
    void EnableDropShadow(int enable);
    void SetDropShadow(int color, int dist);

    int  GetFGColor(void) const     { return ForeGround; }
    int  GetBGColor(void) const     { return BackGround; }

    int  GetOpacity(void) const     { return Opacity;    }
    
    int  GetWidth(void) const       { return (Width << Double);  }
    int  GetHeight(void) const      { return (Height << Double); }

    void SetDisplayPage(int pageno) { DisplayPage = pageno; }
};


class SmoothFont : public Font
{
private:
    int  ForeR, ForeG, ForeB;
    int  BackR, BackG, BackB;

public:
    SmoothFont();
    virtual ~SmoothFont() {};

    virtual void Draw(int x, int y, int serial);

    virtual void SetColor(int fg, int bg, int opacity = 16);
    
};


#endif  // _FONT_H_
