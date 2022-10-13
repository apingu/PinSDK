//
//  2D engine for Win32 GDI Class
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
#ifndef VMCGDI_H
#define VMCGDI_H


#include <Platform.h>
#include <stdio.h>
//#include <io.h>
//#include "vmc-render_def.h"
#include "ivmc-draw2d.h"

#define _VMC_DEVICE_ID_2DGDI   2

namespace LGB
{
    //-----------------------------------------------------------------------------
    // NAME: class VPage
    // DESC: 2D draw class use for GDI draw mode
    //------------------------------------------------------------------------------
    class VPage : public IVMCDraw2d
    {
        //HWND        m_hPaintWnd;
        Pmgc        m_hdcPaintGC; // screen DC
        LG::Size    m_PaintGCSize;

        Pmgc        m_hdcMemGC;   // memory DC
        LG::Size    m_MemGCSize;

        ubyte_t*    m_DIBPtr;     // screen memory start address
        HFONT       m_hFont;      // a HFONT object describing the currently selected font

        //CMemCanvas* p_canvas_;

        PixelFormat JudgeVideoMask      (PixelFormat pixel_mode = PixelFormat::R5G5B5);          //設定顯示卡的模式
        Pmgc        CreatMemDC             (int width, int height, int Depth);
        void        DestroyPaintDC         (void);
        void        DestroyMemDC           (void);
        void        Destroy_Font           (void);


    public:


        VPage();
        ~VPage();

        DWORD       DeviceType(){ return _VMC_DEVICE_ID_2DGDI; }

        void*       GetCanvas             (void)  { return p_canvas_; };

        //end compatiblity with IVMCDevice function===================================================
        //virtual bool Create               (void* hWnd)=0{};
        bool        Create                 (void* hWnd,
                                            int x=0, int y=0, u_int width=800, u_int height=600, u_int depth=2,
                                            bool screenflag = false
                                          );                         //DX Fullscreen or Windows

        Pmgc        GetMemGC               (void){ return m_hdcMemGC; }

        void        Destroy                (void);

        bool        ChangeScreenSize       (int x=0, int y=0, u_int width=800, u_int height=600, u_int depth=2,
                                            bool fullscreen = true){ return true; }

        void        FullCanvas             (DWORD BackColor);


        bool        BeginFrame             (DWORD argv=0){ return true; }
        void        EndFrame               (void){};


        bool        ClearDIB               (int BackColor);

        //get value
        Pmgc        PaintGC                (void);
        LG::Rect    PaintGCRect            (void);
        LG::Size    PaintGCSize            (void);
        Pmgc        MemGC                  (void);
        LG::Rect    MemGCRect              (void);
        LG::Size    MemGCSize              (void);

        //renew screen 
        void        Flip                   (void);
        void        Flip                   (void* ScreenDC);
        void        Flip                   (void* ScreenDC, LG::Point point);

        void        Flip                   (void* hDC, LG::Rect dRect, Pmgc m_hMemDC, LG::Rect sRect);
        void        TransparentShow        (Pmgc hdcDest, LG::Rect nDestRect, Pmgc hdcSrc, LG::Rect nSrcRect, UINT crTransparent);

        //Font
        //void      ChangeFont     (const char* szFontname, int nWidth, int nHeight, int nWeight);
        void        SetBackColor           (COLORREF color);
        void        Create_Font            (const char* font_type, int s_w, int s_h);
        
        void        Text                   (int x, int y, COLORREF col, LPCTSTR string, int iBkMode=TRANSPARENT);
        void        DrawText               (LPCSTR pString, COLORREF col, LPRECT pRect);
    };

};//namespaec LGB

#endif
