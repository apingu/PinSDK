//VMCDC.cpp 2D engine for GDI mode
//
//
//
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    1999    Pin
//

#include "vmc-gdi.h"



namespace LGB
{
    
#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

//=============================================================================
// NAME: 
// DESC: Constructor func
//=============================================================================

VPage::VPage()
{
    //m_hPaintWnd        = 0;
    m_hdcPaintGC         = 0;
    m_hdcMemGC           = 0;
    m_hFont              = 0;    // a HFONT object describing the currently selected font
    m_DIBPtr             = 0;
    p_canvas_             = NULL;

    Create_Font("新細明體", 12, 6);

    //SetDisplay(0, 0, 16);
    SetBackColor( RGB(0, 0, 0 ) );
    
}



//=============================================================================
//
//
//=============================================================================
VPage::~VPage()
{
    Destroy();
    //delete [] m_DIBPtr;
}


//=============================================================================
// NAME: DestroyPaintDC() 
// DESC: Destroy a main paint drive canvas
//=============================================================================
void VPage::DestroyPaintDC(void)
{
    //if(m_hdcMemGC != 0)
    //    DeleteDC(m_hdcMemGC);
    //DeleteDC(m_hdcPaintGC);
      //ReleaseDC(_Widget.hWnd, m_hdcMemGC);
    if(m_hdcPaintGC != 0)
        ReleaseDC((HWND)vobj_hwidget_, m_hdcPaintGC);
    m_hdcPaintGC=0;
    //ReleaseDC(_Widget.hWnd, m_hFont);
}


//=============================================================================
// NAME: DestroyMemDC() 
// DESC: Destroy a menory drive canvas
//=============================================================================
void VPage::DestroyMemDC(void)
{
    if(m_hdcMemGC != 0)
        DeleteDC(m_hdcMemGC);
    m_hdcMemGC = 0;
    m_DIBPtr = 0;
    //DeleteDC(m_hdcPaintGC);
      //ReleaseDC(_Widget.hWnd, m_hdcMemGC);
    //if(m_hdcPaintGC != 0)
    //    ReleaseDC(_Widget.hWnd, m_hdcPaintGC);
    //ReleaseDC(_Widget.hWnd, m_hFont);
}

//=============================================================================
// NAME: VideoMask() 
// DESC: Set vedio mask
//=============================================================================
PixelFormat VPage::JudgeVideoMask (PixelFormat pixel_mode)
{
    PIXELFORMATDESCRIPTOR  pfd; 
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    int iPixelFormat = GetPixelFormat(m_hdcPaintGC);
    DescribePixelFormat(m_hdcPaintGC, iPixelFormat,  
        sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    return pixel_mode;
}


//=============================================================================
// NAME: CreateDC() 
// DESC: get main window drive canvas
//=============================================================================
bool VPage::Create(void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool screenflag)                         //DX Fullscreen or Windows
{
    DestroyPaintDC();

    if((width <=0) || (height <=0) || (depth < 0))
    {
        //MessageBox(NULL, "Argument error !!", "Create dc failure!!", MB_OK);
        return false;
    }    
    
    vobj_hwidget_ = hWnd;
    fullscreen_ = screenflag;

    //LG::Rect wndRect;
    m_hdcPaintGC = GetDC((HWND)hWnd);

    // 因為是GDI 所以你沒有指定的權利
    UINT uBPP = ::GetDeviceCaps(m_hdcPaintGC, BITSPIXEL);
    depth = (uBPP/8);
    //m_hdcPaintGC = GetDCEx(hWnd, DCX_EXCLUDERGN, DCX_WINDOW);


    PixelFormat pixel_mode = PixelFormat::R5G5B5;
    switch(depth)
    {
    case 2:     pixel_mode = PixelFormat::R5G5B5;      break;
    case 4:  pixel_mode = PixelFormat::A8R8G8B8; break;
    }

    m_PaintGCSize.cx = width;//wndRect.right  - wndRect.left;
    m_PaintGCSize.cy = height;//widthwndRect.bottom - wndRect.top;

    PixelFormat PM = JudgeVideoMask (pixel_mode);
    SetPixelMode(x, y, width, height, PM);

    if(p_canvas_ != 0)
    {
        //Canvas->PicHeaderPtr = 0;
        p_canvas_->pixels = 0;
        SAFE_DELETE(p_canvas_);
    }

    if(CreatMemDC(width, height, depth) == NULL)
    {
        //MessageBox(NULL,"BackBuffer create failed"," Surface Failed", MB_OK);
        return false;
    }

    if((width <=0) || (height <=0) || (depth < 0))
    {
        //MessageBox(NULL, "Argument error !!", "Create dc failure!!", MB_OK);
        return false;
    }

    p_canvas_ = new CPIcon(width, height, depth, m_DIBPtr) ;

    return true;
}

//=============================================================================
// NAME: Destroy()
// DESC:
//=============================================================================
void VPage::Destroy(void)
{
    DestroyMemDC();
    DestroyPaintDC();
    Destroy_Font();

    if(p_canvas_ != 0)
    {
        //Canvas->PicHeaderPtr = 0;
        p_canvas_->pixels = 0;
        SAFE_DELETE(p_canvas_);
    }
}

//=============================================================================
// NAME: CreatMemDC()
// DESC:
//=============================================================================
HDC VPage::CreatMemDC(int width, int height, int depth)
{
    //if memory DC has be create first to delete it
    DestroyMemDC();
    if((width <=0) || (height <=0) || (depth < 0))
    {
        //MessageBox(NULL, "Argument error !!", "Create dc failure!!", MB_OK);
        return NULL;
    }
    if(depth ==  0)
        depth = GetDepthb();

    //===================================================================================================
    //Create BackBuffer

    LPBITMAPINFO    pBmInfoB = (LPBITMAPINFO) new ubyte_t [sizeof(BITMAPINFOHEADER)];

    if (pBmInfoB == NULL)
        return 0;   // failed to make a bitmap information buffer

    // fill in the BITMAPINFOHEADER
    pBmInfoB->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    pBmInfoB->bmiHeader.biWidth         = width;
    pBmInfoB->bmiHeader.biHeight        = -height;    // up to bottom
    pBmInfoB->bmiHeader.biPlanes        = 1;
    pBmInfoB->bmiHeader.biBitCount      = depth * 8;  // nBitCount;
    pBmInfoB->bmiHeader.biCompression   = BI_RGB;
    pBmInfoB->bmiHeader.biSizeImage     = WIDTHBYTES((DWORD) width * pBmInfoB->bmiHeader.biBitCount) * height;
    pBmInfoB->bmiHeader.biXPelsPerMeter = 0;
    pBmInfoB->bmiHeader.biYPelsPerMeter = 0;
    pBmInfoB->bmiHeader.biClrUsed       = 0;
    pBmInfoB->bmiHeader.biClrImportant  = 0;

    HBITMAP m_hBitmap = CreateDIBSection(m_hdcPaintGC, pBmInfoB, DIB_RGB_COLORS, (void **) &m_DIBPtr, NULL, 0);
    m_hdcMemGC        = CreateCompatibleDC(m_hdcPaintGC);


    if (m_hdcMemGC == NULL)
    {
        //MessageBox(NULL,"BackBuffer create failed"," Surface Failed", MB_OK);
        exit(1);
    }

    m_MemGCSize.cx  = width;
    m_MemGCSize.cy  = height;

    //HBITMAP m_hOldBitmapB  = (HBITMAP)
    HBITMAP m_OldBitmap = (HBITMAP)SelectObject(m_hdcMemGC, m_hBitmap);

    delete [] pBmInfoB;

    //DeleteObject(m_hBitmap);

    return m_hdcMemGC;
}



/*
//=============================================================================
// NAME: CreatVMC()
// DESC:
//=============================================================================
void VPage::CanvasCreate(CMemCanvas** Canvas, int width, int height, int depth)    //create direction x virtual memory canvas
{
    CanvasDestroy((*Canvas));
    (*Canvas) = 0;
    
    if(CreatMemDC(width, height, depth) == NULL)
    {
        //MessageBox(NULL,"BackBuffer create failed"," Surface Failed", MB_OK);
        return;
    }

    if((width <=0) || (height <=0) || (depth < 0))
    {
        //MessageBox(NULL, "Argument error !!", "Create dc failure!!", MB_OK);
        return;
    }

    (*Canvas) = new CPIcon(width, height, depth, m_DIBPtr) ;
    
    return;
}



//=============================================================================
// NAME: DestroyCanvas()
// DESC:
//=============================================================================
void VPage::CanvasDestroy(CMemCanvas* Canvas)
{
    if(Canvas != 0)
    {
        //Canvas->PicHeaderPtr = 0;
        Canvas->pixels = 0;
        SAFE_DELETE(Canvas);
    }

    if(m_hdcMemGC != 0)
        DeleteDC(m_hdcMemGC);
    m_DIBPtr = 0;

    return;
}

*/

//=============================================================================
// NAME: ClearCanvas()
// DESC: draw back screen with backcolor
//=============================================================================
bool VPage::ClearDIB(int BackColor)
{
    if(m_DIBPtr != 0)
    {
         ubyte_t* Canvasptr = m_DIBPtr;
         memset(Canvasptr,BackColor, (m_MemGCSize.cx * m_MemGCSize.cy));
    }
    return true;
}

void VPage::FullCanvas(DWORD BackColor)
{
    if(GetCanvas() != 0)
        ((CMemCanvas*)GetCanvas())->FullColor(BackColor);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void VPage::Flip(void)
{
    StretchBlt(m_hdcPaintGC, 0, 0, m_PaintGCSize.cx, m_PaintGCSize.cy,
                m_hdcMemGC, 0, 0, m_MemGCSize.cx, m_MemGCSize.cy,
                SRCCOPY);
}



//=============================================================================
// NAME:
// DESC:
//=============================================================================
void VPage::Flip(void* ScreenDC)
{
    StretchBlt((HDC)ScreenDC, 0, 0, m_PaintGCSize.cx, m_PaintGCSize.cy,
                m_hdcMemGC, 0, 0, m_MemGCSize.cx, m_MemGCSize.cy,
                SRCCOPY);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void VPage::Flip(void* ScreenDC, LG::Point point)
{
    StretchBlt((HDC)ScreenDC, point.x, point.y, m_PaintGCSize.cx, m_PaintGCSize.cy,
                m_hdcMemGC, 0, 0, m_MemGCSize.cx, m_MemGCSize.cy,
                SRCCOPY);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void VPage::Flip(void* hDC, LG::Rect dRect, HDC m_hMemDC, LG::Rect sRect)
{

    //if (m_hBitmap == NULL)
    //   return;

    //BitBlt((HDC) hDC, x, y, nWidth, nHeight, m_hMemDC, 0, 0, SRCCOPY);

    StretchBlt((HDC) hDC, dRect.left, dRect.top, dRect.right, dRect.bottom,
                m_hMemDC, sRect.left, sRect.top, sRect.right, sRect.bottom,
                SRCCOPY);


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



        SetStretchBltMode((HDC) hDC, COLORONCOLOR);

        StretchBlt((HDC) hDC, x, y, nWidth, nHeight,
                   m_hMemDC, xSrc, ySrc, nSrcWidth, nSrcHeight,
                   SRCCOPY);

        StretchDIBits((HDC) hDC,            // hDC
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




//=============================================================================
// NAME: TransparentShow
// DESC: Transparent draw image
//=============================================================================
void VPage::TransparentShow(HDC hdcDest, LG::Rect nDestRect, HDC hdcSrc, LG::Rect nSrcRect, UINT crTransparent)
{
    /*
     TransparentBlt(hdcDest,
                    nDestRect.left, nDestRect.top,
                    nDestRect.right, nDestRect.bottom,
                    hdcSrc,
                    nSrcRect.left, nSrcRect.top,
                    nSrcRect.right, nSrcRect.bottom,
                    crTransparent);
                    */
}




//=============================================================================
// NAME: Set_Font()
// DESC: set font type and width, height
//=============================================================================
void VPage::Create_Font(const char* font_type, int s_w, int s_h)
{
    m_hFont = CreateFont(s_w, s_h, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                         OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH, font_type);
}

//=============================================================================
// NAME: Text()
// DESC: draw text in screen
//=============================================================================
void VPage::SetBackColor(COLORREF color)
{
    if(m_hdcMemGC != NULL)
        SetBkColor(m_hdcMemGC, color);
}

//=============================================================================
// NAME: Text()
// DESC: draw text in screen
//=============================================================================
void VPage::Text(int x, int y, COLORREF col, LPCTSTR string, int iBkMode)
{
    if(string == 0) 
        return;

    HFONT hFont = (HFONT)SelectObject(m_hdcMemGC, m_hFont);
    SetBkMode(m_hdcMemGC, iBkMode);
    SetTextColor(m_hdcMemGC, col);
    TextOut(m_hdcMemGC, x, y, string, strlen(string));
    SelectObject(m_hdcMemGC, hFont);
}

//=============================================================================
// NAME: Destroy_Font()
// DESC: Destroy font object
//=============================================================================
void VPage::Destroy_Font(void)
{
    DeleteObject(m_hFont);
    m_hFont=0;
    return;
}



//=============================================================================
// NAME: PaintDC() 
// DESC: return main paint drive canvas
//=============================================================================
HDC VPage::PaintGC(void)
{
    return m_hdcPaintGC; // screen DC
}

//=============================================================================
// NAME: PainDCRect() 
// DESC: return main paint drive canvas rect
//=============================================================================
LG::Rect VPage::PaintGCRect(void)
{
    LG::Rect rt(0, 0, m_PaintGCSize.cx - 1, m_PaintGCSize.cy - 1);
    return rt;
}


//=============================================================================
// NAME: PainDCSize() 
// DESC: return main paint drive canvas size
//=============================================================================
LG::Size VPage::PaintGCSize(void)
{
    return m_PaintGCSize;
}

//=============================================================================
// NAME: PainDCRect() 
// DESC: return memory drive canvas
//=============================================================================
HDC VPage::MemGC(void)
{
    return m_hdcMemGC;   // memory DC
}

//=============================================================================
// NAME: PainDCRect() 
// DESC: return memory drive canvas rect
//=============================================================================
LG::Rect VPage::MemGCRect(void)
{
    LG::Rect rt(0, 0, m_MemGCSize.cx - 1, m_MemGCSize.cy - 1);
    return rt;
}


//=============================================================================
// NAME: MemDCSize() 
// DESC: return main paint drive canvas size
//=============================================================================
LG::Size VPage::MemGCSize(void)
{
    return m_MemGCSize;
}

};//namespace LGB