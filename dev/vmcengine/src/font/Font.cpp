// font.cpp : implementation of font class
//
//  Developer   Date    Comments
//  yann        991204  moved from text.cpp  r1.3
//              991204  smooth font (Yann Font Format 1) supported
//                      but only in 16 bits mode, draw to vpage
//              991216  enhanced the drawing of smooth font
//              000113  added effects for drawing font 
//                              Font  SmoothFont              
//                      Double   V        V 
//                      Shadow            V
//                      Outline 
//              000318  added opacity for smooth font

#include <stdio.h>
//#include <io.h>       // for filelength
#include <string.h>     // for memcpy
#include "Font.h"
#include <pf-stdlib.h>


unsigned long  RBitMask;
unsigned long  GBitMask;
unsigned long  BBitMask;


void DoubleExpand8(ubyte_t *image);
void DoubleExpand16(ubyte_t *image);
//void vputimage(int x, int y, char* BitMap){};
//void vputimagetrans(int x, int y, char* BitMap, int t){};
//void vputcolormask16(int x, int y, ubyte_t* BitMap, int ds){};

/*
namespace GlobalFont
{
};
*/


/////////////////////////////////////////////////////////////////////////////
// Font class

Font::Font(void)
{
    BitMap = NULL;
    PackedFont = NULL;
    FontFile  = NULL;
    FontCache = NULL;
    Filename = NULL;

    EnBitsPerPixel  = 1;
    DeBytesPerPixel = 2;


    FontNum   = 0;

    Opacity   = 16;

//  DisplayPage = -1;       // default is drawing to screen

    Double   = 0;
    DropDist = 0;      
    OutDist  = 0;

    Fmt      = 0;
    //m_pData  = NULL;
    //vputimage=NULL;
    //vputimagetrans=NULL;
    //vputcolormask16=NULL;

    _pfInit();
}

Font::~Font()
{
    Destroy();
}

void Font::Destroy(void)
{
    if (PackedFont != NULL)
    {
        delete [] PackedFont;
        PackedFont = NULL;
    }

    if (BitMap != NULL)
    {
        delete [] BitMap;
        BitMap = NULL;
    }
    
    if (FontFile != NULL)
    {
        _pfclose(_pffile(FontFile));
        FontFile = NULL;
    }

    if (FontCache != NULL)
    {
        delete [] FontCache;
        FontCache = NULL;
    }

    if(Filename != NULL)
    {
        delete [] Filename;
        Filename = NULL;
    }

}

int  Font::Create(const tchar_t* filename, int width, int height, int depth, int cache)
{
    int  len;

    // Clear old data
    Destroy();


    Filename = new tchar_t[strlen(filename) + 1];
    strcpy(Filename, filename);

    len = width * height;   // length by pixels
    DeBytesPerPixel= depth;

    

    // Raw bitmap struct for drawing a font (NOTE: make double size for expanding)
    BitMap = new ubyte_t [(len * DeBytesPerPixel * 4) + 4];

    if (BitMap == NULL)
        return 0;           // failed! memory not enough

    Width  = width;
    Height = height;

    *(word_t *) BitMap = (word_t) Width * DeBytesPerPixel;  // this is the BMP width in bytes
    *(word_t *) (BitMap + 2) = (word_t) Height;             // height


    // Buffer for loading native (packed as pixel by pixel) font data
    len = len * EnBitsPerPixel;     // length by bits

    PackedSize = (len + 7) / 8;     // bytes to store these bits
    PackedFont = new ubyte_t [PackedSize];
    memset(PackedFont, 0, PackedSize);

    if (PackedFont == NULL)
        return 0;           // failed! memory not enough

    _OctanSize = len / 8;
    _Exceeding = (PackedSize == _OctanSize) ? 0 : (8 - len % 8);

    // Set default colors
#if defined    _8BITS
    SetColor(15, 0);
#elif defined  _16BITS
    SetColor(_RGB(255,255,255), 0);
#else
    //#error Can't determine in which video mode
#endif


    // Load character fonts
    if ((FontFile = _pftopen(filename, "rb")) == NULL)
        return 0;           // failed! file not found

    len = _pffilelength(_pffile(FontFile));

    FontNum = len / PackedSize;

    if (cache)
    {
        // If memory is enougth then load fonts from file to memory
        FontCache = new ubyte_t [len];
        if (FontCache)
        {
            _pfread(FontCache, sizeof(ubyte_t), len, _pffile(FontFile));
            _pfclose(_pffile(FontFile));
            FontFile = NULL;
        }
        // else use file handle for fonts
    }

    return 1;
}

void Font::Draw(int x, int y, int serial)
{
    // Verify the serial number
    if (serial >= FontNum) 
        return;

    // Load packed font to buffer (it's a packed font)
    long   offset;
    ubyte_t  *pk;

    offset = (long) serial * PackedSize;

    if (FontCache)
    {
        // From cache
        pk = FontCache + offset;
    }
    else
    {
        // From font file
        _pfseek(_pffile(FontFile), offset, SEEK_SET);
        _pfread(PackedFont, sizeof(ubyte_t), PackedSize, _pffile(FontFile));

        pk = PackedFont;
    }

    // Unpack this font to bitmap
    ubyte_t        byte;
    switch(DeBytesPerPixel)
    {
    case 4:
        {
            int  *data;       // point to the data of bitmap

            data = (int*) (BitMap + 4);

            for (int i = 0; i < _OctanSize; i++)
            {
                byte = *(pk++);

                // Test each bit patterns
                *(data++) = (long) (byte  &0x80 ? ForeGround : BackGround);    // bit 7
                *(data++) = (long) (byte  &0x40 ? ForeGround : BackGround);    // bit 6
                *(data++) = (long) (byte  &0x20 ? ForeGround : BackGround);    // bit 5
                *(data++) = (long) (byte  &0x10 ? ForeGround : BackGround);    // bit 4
                *(data++) = (long) (byte  &0x08 ? ForeGround : BackGround);    // bit 3
                *(data++) = (long) (byte  &0x04 ? ForeGround : BackGround);    // bit 2
                *(data++) = (long) (byte  &0x02 ? ForeGround : BackGround);    // bit 1
                *(data++) = (long) (byte  &0x01 ? ForeGround : BackGround);    // bit 0
            }

            if (_Exceeding)
            {
                byte = *pk;

                // Test each bit patterns
                for (int i = 7; i >= _Exceeding; i--)
                {
                    *(data++) = (long) (byte  &(1 << i) ? ForeGround : BackGround);
                }
            }
        }
        break;
    case 2:
        {
            short  *data;       // point to the data of bitmap

            data = (short *) (BitMap + 4);

            for (int i = 0; i < _OctanSize; i++)
            {
                byte = *(pk++);

                // Test each bit patterns
                *(data++) = (short) (byte  &0x80 ? ForeGround : BackGround);    // bit 7
                *(data++) = (short) (byte  &0x40 ? ForeGround : BackGround);    // bit 6
                *(data++) = (short) (byte  &0x20 ? ForeGround : BackGround);    // bit 5
                *(data++) = (short) (byte  &0x10 ? ForeGround : BackGround);    // bit 4
                *(data++) = (short) (byte  &0x08 ? ForeGround : BackGround);    // bit 3
                *(data++) = (short) (byte  &0x04 ? ForeGround : BackGround);    // bit 2
                *(data++) = (short) (byte  &0x02 ? ForeGround : BackGround);    // bit 1
                *(data++) = (short) (byte  &0x01 ? ForeGround : BackGround);    // bit 0
            }

            if (_Exceeding)
            {
                byte = *pk;

                // Test each bit patterns
                for (int i = 7; i >= _Exceeding; i--)
                {
                    *(data++) = (short) (byte  &(1 << i) ? ForeGround : BackGround);
                }
            }
        }
        break;
    default:
        break;
    }
    

    // Draw this bitmap to virtual page
    // switchactvpage(DisplayPage);

    if (Double)
        DoubleExpand16(BitMap);

    if (BackGround)
        OnBlitRender(x, y, BitMap);
    else
        OnTransRender(x, y, 0, BitMap);

    if (Double)
    {
        // Reset the W  &H of Bitmap
        *(word_t *) BitMap = (word_t) Width * DeBytesPerPixel;  
        *(word_t *) (BitMap + 2) = (word_t) Height;             
    }   
}

void Font::SetColor(int fg, int bg, int opacity)
{
    ForeGround = fg;
    BackGround = bg;

    Opacity = opacity;
}

void Font::EnableDropShadow(int enable)
{
    SetDropShadow(0, enable);
}

void Font::SetDropShadow(int color, int dist)
{
    DropShadow = color;      
    DropDist   = dist;       
}

/*
void Font::SetRenderFunc(PutImageCB funcA, PutTransCB funcB, PutColormask16 func4bits, void* pdata)
{
    m_pData = pdata;
    vputimage=funcA;
    vputimagetrans=funcB;
    vputcolormask16=func4bits;

}
*/


/////////////////////////////////////////////////////////////////////////////
// SmoothFont class

SmoothFont::SmoothFont() : Font()
{
    EnBitsPerPixel  = 4;
    DeBytesPerPixel = 1;    // always 1 byte for bitmap is a 8-bits alpha mask  
    Fmt             = 1;
}

void SmoothFont::Draw(int x, int y, int serial)
{
    // Verify the serial
    if (serial >= FontNum) 
        return;

    // Load packed font to buffer (it's a packed font)
    long   offset;
    ubyte_t  *pk;

    offset = (long) serial * PackedSize;

    if (FontCache)
    {
        // From cache
        pk = FontCache + offset;
    }
    else
    {
        // From font file
        _pfseek(_pffile(FontFile), offset, SEEK_SET);
        _pfread(PackedFont, sizeof(ubyte_t), PackedSize, _pffile(FontFile));

        pk = PackedFont;
    }

    // Unpack this font to bitmap
    ubyte_t  *data;        // point to the data of bitmap

    data = (ubyte_t *) (BitMap + 4);

    for (int i = 0; i < _OctanSize; i++)
    {
        // Test each bit patterns
        *(data++) = ((*pk) >> 4);
        *(data++) = ((*pk)  &0x0F);

        pk++;
    }

    if (_Exceeding)
    {
        *(data++) = ((*pk) >> 4);
    }

    // Draw bitmap to virtual page
//  switchactvpage(DisplayPage);

/*  if (BackGround)     // NOTE: Usually I'll do once vfillrect before drawing fonts,
    {                   //       so I removed these for speed.
                        //       (else if u like each font to fill a box ^^)
        vfillrect(x, y, x + Width - 1, y + Height - 1, BackGround);
    } */

    if (Opacity < 16)
    {
        if (Opacity == 0)
            return;

        pk = (ubyte_t *) (BitMap + 4);

        while (pk != data)
        {
            *pk = (((*pk) * Opacity) >> 4);
            pk++;
        } 
    }

    if (Double)
        DoubleExpand8(BitMap);

    if (DropDist)
        OnMask16Render(x + DropDist, y + DropDist, DropShadow, (ubyte_t *)BitMap);

    OnMask16Render(x, y, ForeGround, (ubyte_t *) BitMap);

    if (Double)
    {
        // Reset W  &H of Bitmap
        *(word_t *) BitMap = (word_t) Width * DeBytesPerPixel;  
        *(word_t *) (BitMap + 2) = (word_t) Height;             
    }
}

void SmoothFont::SetColor(int fg, int bg, int opacity)
{
    ForeGround = fg;
    BackGround = bg;

    ForeR = (fg  &RBitMask);
    ForeG = (fg  &GBitMask);
    ForeB = (fg  &BBitMask);

    BackR = (bg  &RBitMask);
    BackG = (bg  &GBitMask);
    BackB = (bg  &BBitMask);

    Opacity = opacity;
}


/* This is the first version to draw the smooth font.

   It decoded the font (and computed alpha in the same time) to a 16-bits
   raw image then show it, this way slow and...   Yann 12/16/1999

SmoothFont::SmoothFont() : Font()
{
    EnBitsPerPixel  = 4;
    DeBytesPerPixel = PIXELSIZE;
}

void SmoothFont::Draw(int x, int y, int serial)
{
    // Load packed font to buffer (it's a packed font)
    long   offset;
    ubyte_t  *pk;

    offset = (long) serial * PackedSize;

    if (FontCache)
    {
        // From cache
        pk = FontCache + offset;
    }
    else
    {
        // From font file
        _pfseek(FontFile, offset, SEEK_SET);
        _pfread(PackedFont, sizeof(ubyte_t), PackedSize, FontFile);

        pk = PackedFont;
    }

    // Unpack this font to bitmap
    ubyte_t        a1;
    ubyte_t        a2;
    PIXELDATA  *data;       // point to the data of bitmap
    int         r, g, b;

    data = (PIXELDATA *) (BitMap + 4);

    for (int i = 0; i < _OctanSize; i++)
    {
        // Test each bit patterns
        a1 = (*pk) >> 4;    // bit 5-7
        a2 = 15 - a1;

        r = ((ForeR * a1 + BackR * a2) / 15)  &RBitMask;
        g = ((ForeG * a1 + BackG * a2) / 15)  &GBitMask;
        b = ((ForeB * a1 + BackB * a2) / 15);

        *(data++) = (PIXELDATA) (r | g | b);

        a1 = (*pk)  &0x0F;  // bit 0-4
        a2 = 15 - a1;

        r = ((ForeR * a1 + BackR * a2) / 15)  &RBitMask;
        g = ((ForeG * a1 + BackG * a2) / 15)  &GBitMask;
        b = ((ForeB * a1 + BackB * a2) / 15);

        *(data++) = (PIXELDATA) (r | g | b);

        pk++;
    }

    if (_Exceeding)
    {
        a1 = (*pk) >> 4;    // bit 5-7
        a2 = 15 - a1;

        r = ((ForeR * a1 + BackR * a2) / 15)  &RBitMask;
        g = ((ForeG * a1 + BackG * a2) / 15)  &GBitMask;
        b = ((ForeB * a1 + BackB * a2) / 15);

        *(data++) = (PIXELDATA) (r | g | b);
    }


    // Draw bitmap to virtual page
//  switchactvpage(DisplayPage);

    if (BackGround)
        vputimage(x, y, BitMap);
    else
        vputimagetrans(x, y, BitMap);   // NOTE: ...and this is not correct!
                                        //       You got a dirty edge!
}
*/

void DoubleExpand8(ubyte_t *image)
{
    int    x, y, p;
    int    width, height;
    int    width2;
    
    ubyte_t  *dest, *src;    

    register char  c;        

    width  = *(word_t *) image;
    height = *(word_t *) (image + 2);    

       width2 = width << 1;

    *(word_t *) image       = (word_t) width2;
      *(word_t *) (image + 2) = (word_t) (height << 1);    

    image += 4;

    // Move to left-top corner of four
    src  = image + width * (height - 1);
    dest = image + width2 * (height - 1);

    for (y = 0; y < height; y++)
    {
        memcpy(dest, src, width);

        src  -= width;
        dest -= width2;
    } 

    // Expand horizontally
    for (x = width - 1; x >= 0; x--)
    {
        dest = image;

        for (y = 0; y < height; y++)
        {
            p = (x << 1);

            c = *(dest + x);

            *(dest + p++) = c;
            *(dest + p)   = c;

            dest += width2;
        }
    } 

    // Expand vertically
    dest = image;
     
    for (y = height - 1; y >= 0; y--)
    {
        x = y * width2;
        p = (x << 1);
        src = dest + x;

        memcpy(dest + p, src, width2);
        p += width2;
        memcpy(dest + p, src, width2); 
    } 
}

void DoubleExpand16(ubyte_t *image)
{
    int    x, y, p;
    int    width, height;
    int    width2;
    
    ubyte_t  *dest, *src;    

    register word_t  c;        

    width  = *(word_t *) image;
    height = *(word_t *) (image + 2);    

       width2 = width << 1;

    *(word_t *) image       = (word_t) width2;
      *(word_t *) (image + 2) = (word_t) (height << 1);    

    image += 4;

    // Move to left-top corner of four
    src  = image + width * (height - 1);
    dest = image + width2 * (height - 1);

    for (y = 0; y < height; y++)
    {
        memcpy(dest, src, width);

        src  -= width;
        dest -= width2;
    } 

    // Expand horizontally
    width >>= 1;

    for (x = width - 1; x >= 0; x--)
    {
        dest = image;

        for (y = 0; y < height; y++)
        {
            p = (x << 2);

            c = *(word_t *) (dest + (x << 1));

            *(word_t *) (dest + p) = c;
            p += 2;
            *(word_t *) (dest + p) = c;

            dest += width2;
        }
    } 

    // Expand vertically
    dest = image;
     
    for (y = height - 1; y >= 0; y--)
    {
        x = y * width2;
        p = (x << 1);
        src = dest + x;

        memcpy(dest + p, src, width2);
        p += width2;
        memcpy(dest + p, src, width2); 
    } 
}
