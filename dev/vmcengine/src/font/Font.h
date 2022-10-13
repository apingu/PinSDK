// font.h : header file of font class
//
//          by Yann Chou

#ifndef _FONT_H_
#define _FONT_H_

#include <stdio.h>      // for FILE
#include <pf-def.h>



/////////////////////////////////////////////////////////////////////////////
// Font - font class


class Font
{
protected:

    ubyte_t*    BitMap;             // buffer for font drawing

    int         EnBitsPerPixel;      // encode bits per pixel in packed font
    int         DeBytesPerPixel;    // decode bytes per pixel in bitmap

    int         PackedSize;
    ubyte_t*    PackedFont;         // font data of darwing

    int         _OctanSize;          // (W * H) / 8
    int         _Exceeding;           // (PackedSize == _OctanSize) ? 0 : 8 - (W * H % 8) 

    void*       FontFile;           // where r the fonts      
    ubyte_t*    FontCache;           // use extra memory to store fonts
    int         FontNum;

    // Properties
    int         Width;              // font size
    int         Height;

    int         ForeGround;       // foreground color
    int         BackGround;       // background color

    int         Opacity;          // opacity (0: No drawing ~ 16:100%)

    int         DisplayPage;      // display to which vpage (dud! always to active vpage this moment)

    // Effects
    int         Double;           // double size (0: No double, 1: Yes)

    int         DropShadow;       // with drop shadow color
    int         DropDist;         // drop distance (0: No drop shadow)

    int         Outline;          // with outline color
    int         OutDist;          // out distance (0: No outline)  

    tchar_t*    Filename;
    int         Fmt;

    //PutImageCB      vputimage;
    //PutTransCB      vputimagetrans;
    //PutColormask16  vputcolormask16;
    //void*           m_pData;          // user define data when register render function

public:
    Font();
    virtual ~Font();

    virtual int  Create( const tchar_t* filename, int width, int height, int depth = 2, int cache = 0 );
    virtual void Destroy( void );

    virtual void Draw( int x, int y, int serial );


    virtual void SetColor( int fg, int bg, int opacity = 16 );

    virtual void SetOpacity( int opacity = 16 )   { Opacity = opacity; }

    void EnableDouble( int enable )               { Double = enable; }
    void EnableDropShadow( int enable );
    void SetDropShadow( int color, int dist );

    int  GetFmt( void ) const       { return Fmt; }

    int  GetFGColor( void ) const     { return ForeGround; }
    int  GetBGColor( void ) const     { return BackGround; }

    int  GetOpacity( void ) const     { return Opacity; }

    int  GetWidth( void ) const       { return ( Width << Double ); }
    int  GetHeight( void ) const      { return ( Height << Double ); }
    int  GetBytesPerPixel( void ) const  { return DeBytesPerPixel; }
    //const char* GetFilename(void) const  { return Filename; }

    void SetDisplayPage( int pageno ) { DisplayPage = pageno; }

    const tchar_t* GetFilename( void ) { return Filename; }

    virtual void OnBlitRender( int x, int y, ubyte_t* BitMap ) = 0;
    virtual void OnTransRender( int x, int y, int colorkey, ubyte_t* BitMap ) = 0;

    /*
    void SetRenderFunc(PutImageCB funcA,
                        PutTransCB funcB,
                        PutColormask16 func4bits, void* pdata);*/
};


class SmoothFont: public Font
{
private:
    int  ForeR, ForeG, ForeB;
    int  BackR, BackG, BackB;

public:
    SmoothFont();
    virtual ~SmoothFont() {};

    virtual void Draw( int x, int y, int serial );

    virtual void SetColor( int fg, int bg, int opacity = 16 );

    virtual void OnMask16Render( int x, int y, int maskcolor, ubyte_t* BitMap ) = 0;

};


#endif  // _FONT_H_
