// VirtualPage.h : header file
//
// Author:         Yann Chou
//

#ifndef _VIRTUAL_PAGE_H_
#define _VIRTUAL_PAGE_H_

#ifndef __AFXWIN_H__
#include <windows.h>
#endif


RGBQUAD *ReadPCXPalette(char *szPCXFile);

LPLOGPALETTE Make256LOGPALETTE(RGBQUAD *pRGBQ = NULL);
void _RGBToRGBQUAD(BYTE *pRGB, RGBQUAD *pRGBQ, int shift = 2, int num = 256);
void _RGBQUADToPALETTEENTRY(RGBQUAD *pRGBQ, PALETTEENTRY *pPE, int num = 256);

/////////////////////////////////////////////////////////////////////////////
// CVirtualPage

class CVirtualPage
{
// Attributes
protected:
    BYTE  *m_pBits;     // a pointer to the virtual page buffer byte that maps to the point (0,0)
    long   m_lStride;   // the number of bytes between the buffer byte that maps to point (x,y) and the buffer byte representing (x,y+1)
    int    m_nWidth;    // the width in pixels of the rectangle represented by the virtual page buffer
    int    m_nHeight;   // the height in pixels of the rectangle represented by the virtual page buffer

    int    m_nLeft;     // the upper-left corner of the rectangle in client window
    int    m_nTop;    

    int    m_nBitCount; // the number of bits per pixel


// Windows implementation data
protected:
    HWND        m_hWnd;
    // Device Context attributes
    HDC         m_hMemDC;
    HDC         m_hWinDC;

    // Palette attributes
    RGBQUAD     m_rgbsPalette[256];
    HPALETTE    m_hPalette;
    HPALETTE    m_hOldPalette;

    // Bitmap attributes
//  VOID       *m_pBits,        // address of bitmap bits (move this line to above)   
    BITMAPINFO *m_pBmInfo;      // address of bitmap data
    HBITMAP     m_hBitmap;      // handle of bitmap
    HBITMAP     m_hOldBitmap;

    // Font attributes
    HFONT       m_hFont;
    HFONT       m_hOldFont;


// Construction and Destruction
public:
    CVirtualPage();
    virtual ~CVirtualPage();    // NOTE: If you declared a global CVirtualPage
                                //       object then you must call Destroy() before 
                                //       the window of m_hWnd been killed else this
                                //       dtor can't release m_hWinDC successfully!

// Operations
protected:
    BOOL  CreateAttachedPalette(RGBQUAD *pColors = NULL);
    BOOL  CreateAttachedBitmap(const RECT &rect, int nBitCount);


// Common operations
public:
    // Basic information access
    BYTE *GetBits(void) const       { return m_pBits;	  }
    long  GetStride(void) const     { return m_lStride;	  }
    int   GetWidth(void) const      { return m_nWidth; 	  }
    int   GetHeight(void) const     { return m_nHeight;   }
    int   GetLeft(void) const       { return m_nLeft; 	  }
    int   GetTop(void) const        { return m_nTop; 	  }
    int   GetBitCount(void) const   { return m_nBitCount; }

    // Pixel access control
    void  Lock(void) const          {}
    void  Unlock(void) const        {}

    // Special access
    HPALETTE  GetPalette();
    void     *GetMemDC()            { return m_hMemDC; }


public:
    // Create 8-bits virtual page w/ 256 colors palette
    BOOL  Create(void *hWnd, RGBQUAD *pColors);
    BOOL  Create(void *hWnd, const RECT &rect, RGBQUAD *pColors);

    // Create 16/24/32-bits virtual page w/o palette
    BOOL  Create(void *hWnd, int nBitCount);
    BOOL  Create(void *hWnd, const RECT &rect, int nBitCount);

    void  Destroy();

    void  Paint(void *hDC, int x, int y, int nWidth, int nHeight);
    void  Paint(void *hDC, int x, int y, int nWidth, int nHeight,
                int xSrc, int ySrc, int nSrcWidth, int nSrcHeight);

    void  Paint(void *hDC);       
    void  Paint(void *hDC, int x, int y);
    void  Paint(void *hDC, const RECT &rectDest);
    void  Paint(void *hDC, const RECT &rectDest, const RECT &rectSrc);

    // Set Palette at 256 colors (8 bits) mode
    void  SetPalette(void *hDC, RGBQUAD *pColors);

    void  OnQueryNewPalette(void *hDC);

    // Print text
    BOOL  ChangeFont(const char *FontName, int Width, int Height, int Weight = FW_NORMAL);
    BOOL  ChangeFont(const LOGFONT &LogFont);
    void  TextXY(int x, int y, COLORREF col, LPCTSTR szString);
    void  TextXY(int x, int y, COLORREF col, COLORREF bg, LPCTSTR szString);
	void  GetTextSize(LPCTSTR szString, int &width, int &height);

    void  WTextXY(int x, int y, COLORREF col, LPCWSTR szString);
    void  WTextXY(int x, int y, COLORREF col, COLORREF bg, LPCWSTR szString);
	void  GetWTextSize(LPCWSTR szString, int &width, int &height);
    


    // Simple drawing    
    void  GetRAWImage(int x0, int y0, int x1, int y1, void *pRaw);
    void  PutRAWImage(int x, int y, void *pRaw);
};


inline void CVirtualPage::Paint(void *hDC)       
{ 
    Paint(hDC, m_nLeft, m_nTop, m_nWidth, m_nHeight); 
}

inline void CVirtualPage::Paint(void *hDC, int x, int y)
{ 
    Paint(hDC, x, y, m_nWidth, m_nHeight); 
}

inline void CVirtualPage::Paint(void *hDC, const RECT &rectDest)
{ 
    Paint(hDC, 
          rectDest.left, rectDest.top,
          rectDest.right - rectDest.left, rectDest.bottom - rectDest.top); 
}

inline void CVirtualPage::Paint(void *hDC, const RECT &rectDest, const RECT &rectSrc)
{ 
    Paint(hDC, 
          rectDest.left, rectDest.top,
          rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, 
          rectSrc.left, rectSrc.top,
          rectSrc.right - rectSrc.left, rectSrc.bottom - rectSrc.top);
}

/////////////////////////////////////////////////////////////////////////////

#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)
#define DWORDALIGN(x)    (((x) + 3) & (~3))

#endif  // _VIRTUAL_PAGE_H_
