// VirtualPage.cpp : implementation file
//
// Developer:   Date:   Comments:
// yann         970511  first build
//              970615  create w/ rectangle struct
//              971101  add ReadPaletteFile(), 256PalToRGBQUAD()
//              980120  fix delete[] (BYTE *) m_pBmInfo in wrong place
// taco         980211  found a bug memcpy(&m_rgbsPalette.. in CreatePalette()
//                      add a rough SetPalette()
// yann         980212  modify SetPalette()
//                      add Make256LOGPALETTE() and change some codes in
//                      CreatePalette()
//              980518  use WIN32 APIs instead of MFC codes (rewrite, because
//                      this work had been finished in April but missed by a
//                      personal fault ;-p)
//                      make codes compact and soild
//              980520  add 16/24/32-bits colors support
//                      A 5-5-5 16-bits image, each WORD in the bitmap array
//                      represents a single pixel. Where the the red mask is
//                      0x7C00, the green is 0x03E0 and the blue is 0x001F.
//                      The most significant bit is not used.
//                      A 8-8-8 24-bits image, each 3-bytes triplet in the
//                      bitmap array represents a R-G-B pixel.
//                      A 8-8-8 32-bits image, each DWORD in the bitmap array
//                      represents a pixel as 24-bits but the high byte in each
//                      DWORD is not used.
//              980609  add attributes to represent the upper-left corner of
//                      the vpage rectangle in client window
//              990818  reserve BITMAPINFO of m_hBitmap for DIB operation
//              990820  improve Paint() calling 
//              991025  change ChangeFont() and add another calling


#include <stdio.h>

#include "VirtualPage.h"

#include <assert.h>
#define ASSERT(cond)    assert(cond)

inline HGDIOBJ _SelectObject(HDC hdc, HGDIOBJ hgdiobj)
{
    if (hdc == NULL || hgdiobj == NULL)
        return NULL;

    return SelectObject(hdc, hgdiobj);
}


/////////////////////////////////////////////////////////////////////////////
// CVirtualPage

CVirtualPage::CVirtualPage()
{
    m_pBits   = NULL;
    m_lStride = 0;
    m_nWidth  = 0;
    m_nHeight = 0;
    m_nLeft   = 0;
    m_nTop    = 0;

    m_hWnd = NULL;

    m_hMemDC = NULL;
    m_hWinDC = NULL;

    m_hPalette    = NULL;
    m_hOldPalette = NULL;
    m_pBmInfo     = NULL;
    m_hBitmap     = NULL;
    m_hOldBitmap  = NULL;
    m_hFont       = NULL;
    m_hOldFont    = NULL;
}

CVirtualPage::~CVirtualPage()
{
    Destroy();
}


/////////////////////////////////////////////////////////////////////////////
// Operations implementations

BOOL CVirtualPage::Create(void *hWnd, RGBQUAD *pColors)
{
    RECT rect;

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = GetSystemMetrics(SM_CXSCREEN) - 1;
    rect.bottom = GetSystemMetrics(SM_CYSCREEN) - 1;

    return Create(hWnd, rect, pColors);
/*
    {
        CWnd *pActiveWnd = CWnd::GetActiveWindow();

        if (pActiveWnd != NULL)
            pActiveWnd->SendMessage(WM_QUERYNEWPALETTE);
    }
*/
}

BOOL CVirtualPage::Create(void *hWnd, const RECT &rect, RGBQUAD *pColors)
{
    if (CreateAttachedPalette(pColors) == FALSE)
        return FALSE;

    return Create(hWnd, rect, 8);
}

BOOL CVirtualPage::Create(void *hWnd, int nBitCount)
{
    RECT rect;

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = GetSystemMetrics(SM_CXSCREEN) - 1;
    rect.bottom = GetSystemMetrics(SM_CYSCREEN) - 1;

    return Create(hWnd, rect, nBitCount);
}

BOOL CVirtualPage::Create(void *hWnd, const RECT &rect, int nBitCount)
{
    if (nBitCount != 8 && nBitCount != 16 && nBitCount != 24 && nBitCount != 32)
        return FALSE;   // wrong pixel format

    m_hWnd = (HWND) hWnd;

    return CreateAttachedBitmap(rect, nBitCount);
}

void CVirtualPage::Destroy()
{
    // Delete the device context, selecting back in the original object
    if (m_hMemDC != NULL)
    {
        _SelectObject(m_hMemDC, m_hOldBitmap);
        _SelectObject(m_hMemDC, m_hOldFont);
        DeleteDC(m_hMemDC);

        m_hMemDC = NULL;
        m_hOldBitmap = NULL;
        m_hOldFont = NULL;
    }

    if (m_hWinDC != NULL)
    {
        _SelectObject(m_hWinDC, m_hOldPalette);
        ReleaseDC(m_hWnd, m_hWinDC);

        m_hWinDC = NULL;
        m_hOldPalette = NULL;
    }
    
    // Delete the memory
    if (m_pBmInfo != NULL)
    {
        delete [] (BYTE *) m_pBmInfo;
        m_pBmInfo = NULL;
    }

    // Delete the objects
    if (m_hBitmap != NULL)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;
    }

    if (m_hPalette != NULL)
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }

    if (m_hFont != NULL)
    {
        DeleteObject(m_hFont);
        m_hFont = NULL;
    }
}

HPALETTE CVirtualPage::GetPalette()
{
//  if (m_hPalette != NULL)
        return m_hPalette;
//  else
//      return NULL;
}

BOOL CVirtualPage::CreateAttachedPalette(RGBQUAD *pColors)
{
    // Delete current palette object
    if (m_hPalette != NULL)
    {
        DeleteObject(m_hPalette);
        m_hPalette = NULL;
    }

    int i;

    if (pColors == NULL)
    {
        // Build a gray level palette as the default
/*      for (i = 0; i < 256; i++)
        {
            m_rgbsPalette[i].rgbRed   = i;
            m_rgbsPalette[i].rgbGreen = i;
            m_rgbsPalette[i].rgbBlue  = i;
        }
 */
        // Build a 332 palette as the default
        for (i = 0; i < 256; i++)
        {
            m_rgbsPalette[i].rgbRed   = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
            m_rgbsPalette[i].rgbGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
            m_rgbsPalette[i].rgbBlue  = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
        }
    }
    else
        // Copy values
        memcpy(m_rgbsPalette, pColors, sizeof(m_rgbsPalette));

    // Creating a Windows logical color palette
    LPLOGPALETTE pLogPal;

    pLogPal = Make256LOGPALETTE(m_rgbsPalette);

    ASSERT(pLogPal != NULL);

    if (pLogPal != NULL)
    {
        m_hPalette = CreatePalette((LOGPALETTE *) pLogPal);

        delete [] (BYTE *) pLogPal;
    }

    ASSERT(m_hPalette != NULL);

    if (m_hPalette == NULL)
        return FALSE;   // failed to create a palette

    return TRUE;
}

BOOL CVirtualPage::CreateAttachedBitmap(const RECT &rect, int nBitCount)
{
    // Release the using/previous objects
    if (m_hMemDC != NULL)
    {
        _SelectObject(m_hMemDC, m_hOldBitmap);
        _SelectObject(m_hMemDC, m_hOldFont);
        DeleteDC(m_hMemDC);

        m_hMemDC = NULL;
        m_hOldBitmap = NULL;
        m_hOldFont = NULL;
    }

    if (m_hWinDC != NULL)
    {
        _SelectObject(m_hWinDC, m_hOldPalette);
        ReleaseDC(m_hWnd, m_hWinDC);

        m_hWinDC = NULL;
        m_hOldPalette = NULL;
    }

    if (m_pBmInfo != NULL)
    {
        delete [] (BYTE *) m_pBmInfo;
        m_pBmInfo = NULL;
    }

    if (m_hBitmap != NULL)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;
    }

    // Reset the informations of the virtual page buffer
    m_pBits     = NULL;
    m_lStride   = 0;
    m_nWidth    = 0;
    m_nHeight   = 0;

    m_nBitCount = nBitCount;


    // Get a new display device context (DC) for the client area of this window
    m_hWinDC = GetDC(m_hWnd);

    ASSERT(m_hWinDC != NULL);

    if (m_hWinDC == NULL)
        return FALSE;   // failed to get a DC


    // Defining the dimensions and color information for a device-independent bitmap (DIB)
    int width;
    int height;

    width = sizeof(BITMAPINFOHEADER);
    if (nBitCount == 8)
        width += (256 * sizeof(RGBQUAD));

    m_pBmInfo = (LPBITMAPINFO) new BYTE [width];

    ASSERT(m_pBmInfo != NULL);

    if (m_pBmInfo == NULL)
        return FALSE;   // failed to make a bitmap information buffer

    // GetClientRect(m_hWnd, &rect);
    width  = rect.right - rect.left + 1;
    height = rect.bottom - rect.top + 1;

    // fill in the BITMAPINFOHEADER
    m_pBmInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    m_pBmInfo->bmiHeader.biWidth         = width;
    m_pBmInfo->bmiHeader.biHeight        = -height;    // up to bottom
    m_pBmInfo->bmiHeader.biPlanes        = 1;
    m_pBmInfo->bmiHeader.biBitCount      = nBitCount;
    m_pBmInfo->bmiHeader.biCompression   = BI_RGB;
    m_pBmInfo->bmiHeader.biSizeImage     = WIDTHBYTES((DWORD) width * nBitCount) * height;
    m_pBmInfo->bmiHeader.biXPelsPerMeter = 0;
    m_pBmInfo->bmiHeader.biYPelsPerMeter = 0;
    m_pBmInfo->bmiHeader.biClrUsed       = 0;
    m_pBmInfo->bmiHeader.biClrImportant  = 0;

    if (nBitCount == 8)
    {
        // fill in the color table
        memcpy(m_pBmInfo->bmiColors, m_rgbsPalette, sizeof(RGBQUAD) * 256);
    }
//  else
//      m_pBmInfo->bmiColors = NULL;


    // Create a device-independent bitmap (DIB) that applications can write to directly
    m_hBitmap = CreateDIBSection(m_hWinDC, m_pBmInfo, \
                                 DIB_RGB_COLORS, (void **) &m_pBits, NULL, 0);

    ASSERT(m_hBitmap != NULL);

    if (m_hBitmap == NULL)
        return FALSE;   // failed to create a bitmap

    // Create a memory device context (DC) compatible with the specified device
    m_hMemDC = CreateCompatibleDC(m_hWinDC);

    ASSERT(m_hMemDC != NULL);

    if (m_hMemDC == NULL)
        return FALSE;   // failed to create a memory DC

    // Select the DIB into the memory DC
    m_hOldBitmap = (HBITMAP) SelectObject(m_hMemDC, m_hBitmap);

    ASSERT(m_hOldBitmap != NULL);


    // Set information of the virtual page buffer
    // m_pBits = m_pBits;
    m_lStride = WIDTHBYTES((DWORD) width * nBitCount);
    m_nWidth  = width;
    m_nHeight = height;
    m_nLeft   = rect.left;
    m_nTop    = rect.top;

    // Adjust for the bottom-up DIB
    if (m_pBmInfo->bmiHeader.biHeight > 0)
    {
        // point to first scanline (end of buffer)
        m_pBits = m_pBits + (m_nHeight - 1) * m_lStride;
        // orient Stride from bottom to up
        m_lStride = -m_lStride;
    }

    // Set the palette
    if (nBitCount == 8 && m_hPalette != NULL)
    {
        m_hOldPalette = SelectPalette(m_hWinDC, m_hPalette, FALSE);
        RealizePalette(m_hWinDC);
    }

    // Set the font
    m_hOldFont = (HFONT) _SelectObject(m_hMemDC, m_hFont);

    return TRUE;
}

void CVirtualPage::Paint(void *hDC, int x, int y, int nWidth, int nHeight)
{
    if (m_hPalette != NULL)
    {
        SelectPalette((HDC) hDC, m_hPalette, FALSE);
        RealizePalette((HDC) hDC);
    }

    if (m_hBitmap == NULL)
        return;

    BitBlt((HDC) hDC, x, y, nWidth, nHeight, m_hMemDC, 0, 0, SRCCOPY);
/*
    SetDIBitsToDevice((HDC) hDC,                // hDC
                      x, y, nWidth, nHeight,    // rectDest
                      0,                        // SrcX
                      m_nHeight - nHeight,      // SrcY
                      0,                        // nStartScan
                      m_nHeight,                // nNumScans
                      m_pBits,                  // lpBits
                      m_pBmInfo,                // lpBitsInfo
                      DIB_RGB_COLORS);          // wUsage
*/
}

void CVirtualPage::Paint(void *hDC, int x, int y, int nWidth, int nHeight,
                         int xSrc, int ySrc, int nSrcWidth, int nSrcHeight)
{
    if (m_hPalette != NULL)
    {
        SelectPalette((HDC) hDC, m_hPalette, FALSE);
        RealizePalette((HDC) hDC);
    }

    if (m_hBitmap == NULL)            
        return;

    if (nWidth == nSrcWidth && nHeight == nSrcHeight)
    {
        BitBlt((HDC) hDC, x, y, nWidth, nHeight, 
               m_hMemDC, xSrc, ySrc, SRCCOPY);
/*
        SetDIBitsToDevice((HDC) hDC,                // hDC
                          x, y, nWidth, nHeight,    // rectDest
                          xSrc,                     // rectSrc
                          m_nHeight - ySrc - nSrcHeight,
                          0,                        // nStartScan
                          m_nHeight,                // nNumScans
                          m_pBits,                  // lpBits
                          m_pBmInfo,                // lpBitsInfo
                          DIB_RGB_COLORS);          // wUsage
*/
    }
    else
    {
        // Make sure to use the stretching mode best for color pictures 
        SetStretchBltMode((HDC) hDC, COLORONCOLOR);        

        StretchBlt((HDC) hDC, x, y, nWidth, nHeight,
                   m_hMemDC, xSrc, ySrc, nSrcWidth, nSrcHeight,
                   SRCCOPY);

/*      StretchDIBits((HDC) hDC,            // hDC
                      x, y, nWidth, nHeight,// rectDest
                      xSrc,                 // rectSrc
                      m_nHeight - ySrc - nSrcHeight, 
                      nSrcWidth, nSrcHeight,
                      m_pBits,              // lpBits
                      m_pBmInfo,            // lpBitsInfo
                      DIB_RGB_COLORS,       // wUsage
                      SRCCOPY);             // dwROP
*/
    }
}

void CVirtualPage::SetPalette(void *hDC, RGBQUAD *pColors)
{
    if (m_hPalette == NULL)
        return;     // plz call 'Create' funtion to generate the palette object

    memcpy(m_rgbsPalette, pColors, sizeof(m_rgbsPalette));

    // Creating a Windows logical color palette
    LPLOGPALETTE pLogPal;

    pLogPal = Make256LOGPALETTE(m_rgbsPalette);

    ASSERT(pLogPal != NULL);

    SetPaletteEntries(m_hPalette, 0, 256, (LPPALETTEENTRY) m_rgbsPalette);

    delete [] (BYTE *) pLogPal;

    // Set the color values in the color table of the DIB that is selected into a DC
    SetDIBColorTable(m_hMemDC, 0, 256, m_rgbsPalette);

    if (hDC == NULL)
    {
        hDC = m_hWinDC;
    }    
    
    SelectPalette((HDC) hDC, m_hPalette, TRUE);
    RealizePalette((HDC) hDC);
}

void CVirtualPage::OnQueryNewPalette(void *hDC)
{
    if (m_hPalette != NULL)
    {
        SelectPalette((HDC) hDC, m_hPalette, FALSE);
        RealizePalette((HDC) hDC);
    }
}

/*
BOOL CVirtualPage::OnQueryNewPalette()
{
    if (GetPalette() == NULL)
        return FALSE;

    {
        CClientDC dc(this);
        CPalette *pOldPalette = dc.SelectPalette(&m_hPalette,
                         GetCurrentMessage()->message == Wm_hPaletteCHANGED);
        UINT nChanged = dc.RealizePalette();

        dc.SelectPalette(pOldPalette, TRUE);

        if (nChanged == 0)      // no change to our mapping
            return FALSE;
    }

    // some changes have been made so invalidate it
    // Invalidate(FALSE);

    return TRUE;
}
*/
        
BOOL CVirtualPage::ChangeFont(const char *FontName, int Width, int Height, int Weight)
{
    LOGFONT lf;
                                                    // Example:
    lf.lfHeight         = Height;                   // -12(Size9), -15(Size11), -16(Size12)               
    lf.lfWidth          = Width;                    // 0
    lf.lfEscapement     = 0; 
    lf.lfOrientation    = 0;
    lf.lfWeight         = Weight;                   // 400
    lf.lfItalic         = FALSE;
    lf.lfUnderline      = FALSE;
    lf.lfStrikeOut      = FALSE;
//  lf.lfCharSet        = ANSI_CHARSET;
    lf.lfCharSet        = CHINESEBIG5_CHARSET;      // 136
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;       // 3
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;      // 2
    lf.lfQuality        = DEFAULT_QUALITY;          // 1
    lf.lfPitchAndFamily = DEFAULT_PITCH;            // 49
    strcpy(lf.lfFaceName, FontName);                // "細明體"

    return ChangeFont(lf);
}
        
BOOL CVirtualPage::ChangeFont(const LOGFONT &FogFont)     
{       
    if (m_hFont != NULL)
    {
        _SelectObject(m_hMemDC, m_hOldFont);

        m_hOldFont = NULL;

        DeleteObject(m_hFont);
    }

    m_hFont = CreateFontIndirect(&FogFont);                       

    if (m_hFont == NULL)
        return FALSE;       // failed to create a font

    m_hOldFont = (HFONT) SelectObject(m_hMemDC, m_hFont);

    return (m_hOldFont != NULL);
}

void CVirtualPage::TextXY(int x, int y, COLORREF col, LPCTSTR szString)
{
    SetBkMode(m_hMemDC, TRANSPARENT); 
    
    SetTextColor(m_hMemDC, col);

    TextOut(m_hMemDC, x, y, szString, strlen(szString));
}

void CVirtualPage::TextXY(int x, int y, COLORREF col, COLORREF bg, LPCTSTR szString)
{
    SetBkMode(m_hMemDC, OPAQUE);

    SetTextColor(m_hMemDC, col);
    SetBkColor(m_hMemDC, bg);

    TextOut(m_hMemDC, x, y, szString, strlen(szString));
}

void CVirtualPage::GetTextSize(LPCTSTR szString, int &width, int &height)
{
    SIZE size;

    if (GetTextExtentPoint(m_hMemDC, szString, strlen(szString), &size))
    {
        width  = size.cx;
        height = size.cy;
    }
    else  // the function fails
    {
        width  = 0;
        height = 0;
    }

//  TEXTMETRIC tm;
//  GetTextMetrics(m_hMemDC, &tm);
//  size.cy = tm.tmHeight;
}


void CVirtualPage::WTextXY(int x, int y, COLORREF col, LPCWSTR szString)
{
    SetBkMode(m_hMemDC, TRANSPARENT); 
    
    SetTextColor(m_hMemDC, col);

    TextOutW(m_hMemDC, x, y, szString, wcslen(szString));
}

void CVirtualPage::WTextXY(int x, int y, COLORREF col, COLORREF bg, LPCWSTR szString)
{
    SetBkMode(m_hMemDC, OPAQUE);

    SetTextColor(m_hMemDC, col);
    SetBkColor(m_hMemDC, bg);

    TextOutW(m_hMemDC, x, y, szString, wcslen(szString));
}

void CVirtualPage::GetWTextSize(LPCWSTR szString, int &width, int &height)
{
    SIZE size;

    if (GetTextExtentPointW(m_hMemDC, szString, wcslen(szString), &size))
    {
        width  = size.cx;
        height = size.cy;
    }
    else  // the function fails
    {
        width  = 0;
        height = 0;
    }

//  TEXTMETRIC tm;
//  GetTextMetrics(m_hMemDC, &tm);
//  size.cy = tm.tmHeight;
}





void  CVirtualPage::GetRAWImage(int x0, int y0, int x1, int y1, void *pRaw)
{   
}

void  CVirtualPage::PutRAWImage(int x, int y, void *pRaw)
{
}





/////////////////////////////////////////////////////////////////////////////

RGBQUAD *ReadPCXPalette(char *szPCXFile)
{
    int     length;
    FILE    *infile;
    BYTE    RGBArray[768];
    RGBQUAD *pColors;

    if ((infile = fopen(szPCXFile, "rb")) == NULL)
        return NULL;

    length = fread(&RGBArray, 1, 768, infile);
    fclose(infile);

    if (length != 768)
        return NULL;    // number of bytes read is invaild

    pColors = (RGBQUAD *) new char [sizeof(RGBQUAD) * 256];

    _RGBToRGBQUAD(RGBArray, pColors);

    return pColors;
}

LPLOGPALETTE Make256LOGPALETTE(RGBQUAD *pRGBQ)
{
    LPLOGPALETTE pPaletteColors;

    pPaletteColors = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE) + \
        ((256) * sizeof(PALETTEENTRY))];

    if (pPaletteColors == NULL)
        return NULL;

    pPaletteColors->palVersion = 0x0300;
    pPaletteColors->palNumEntries = 256;

    if (pRGBQ != NULL)
        _RGBQUADToPALETTEENTRY(pRGBQ, pPaletteColors->palPalEntry);

    return pPaletteColors;
}

void _RGBToRGBQUAD(BYTE *pRGB, RGBQUAD *pRGBQ, int shift, int num)
{
    int n = 0;

    for (int i = 0; i < num; i++)
    {
        pRGBQ[i].rgbRed      = pRGB[n++] << shift;
        pRGBQ[i].rgbGreen    = pRGB[n++] << shift;
        pRGBQ[i].rgbBlue     = pRGB[n++] << shift;
        pRGBQ[i].rgbReserved = 0;
    }
}

void _RGBQUADToPALETTEENTRY(RGBQUAD *pRGBQ, PALETTEENTRY *pPE, int num)
{
    for (int i = 0; i < num; i++)
    {
        pPE[i].peRed   = pRGBQ[i].rgbRed;
        pPE[i].peGreen = pRGBQ[i].rgbGreen;
        pPE[i].peBlue  = pRGBQ[i].rgbBlue;
        pPE[i].peFlags = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
