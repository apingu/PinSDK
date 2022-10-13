// ===========================================================================================
//
// Stick Picture Kernel Class
//
//
//
//                                               Use attention +ddraw.lib dxguid.lib D3d8.lib
//                                               Copyright    (C)    1999    Pin
// =========================================================================================
#ifndef VMCDX_H
#define VMCDX_H

#if defined(_WINDOWS)

#include <windows.h>
#include <ddraw.h>
#include <d3d9.h>
#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <fcntl.h>
//#include <sys/stat.h>
#include <windowsx.h>

#include "ivmc-draw2d.h"
//#include "D:\My Documents\Programs\C\PinClass\Container\PLIST.h"


#define _VMC_DEVICE_ID_2DDX   1

#define OUTTRACE(s)   OutputDebugStringA(s)

#define M555 0
#define M565 1

//-----------------------------------------------------------------------------
// NAME: class VMCDX
// DESC: 2D draw class use for directX draw mode
//------------------------------------------------------------------------------

#ifndef  LpDraw
    #define LpDraw             LPDIRECTDRAW7           // LP DIRECT    DRAW
    #define LpSurface         LPDIRECTDRAWSURFACE7  // LP DIRECT    DRAW SURFACE
    #define SurfaceDesc         DDSURFACEDESC2           // DIRECT DRAW SURFACE DESC
    #define LpSurfaceDesc    LPDDSURFACEDESC2       // LP DIRECT    DRAW SURFACE DESC
    #define SurfaceCAPS         DDSCAPS2               // DIRECT DRAW CAPS

    #define Lp3D             LPDIRECT3D3
    #define Lp3DDevice         LPDIRECT3DDEVICE3
    #define D3DDeviceDesc    D3DDEVICEDESC
    #define CD3D_Framework   CD3DFramework
    #define D3DMaterial         D3DMATERIAL
    #define D3DLight         D3DLIGHT
    #define Lp3DViewport     LPDIRECT3DVIEWPORT3
#endif

namespace LGB
{
    class VMCDX : public IVMCDraw2d
    {

    private:

        LPDIRECTDRAW         pDD;
        LpDraw               pDD2;

        LpSurface            pPri;
        LpSurface            pBack;

        SurfaceDesc          Desc;
        LPDIRECTDRAWCLIPPER     pClip;
        
        //LG::Rect             m_WndRect;
        LG::Rect             m_rcViewport;           // Pos.  &size to blt from
        LG::Rect             m_rcScreen;             // Screen pos. for blt

        DWORD                m_dwStyle;


                    
        //LG::Size             m_ScreenSize;
   
        PixelFormat    JudgeVideoMask         (void);                         //判別顯示卡的模式
        //bool        CreateWindowsPrimary   (void);                       //建立Windows的surface
        //bool        CreateFullScreenPrimary(int BackBufferCount);        //建立Fullscreen的surface
    

    public :

        VMCDX();
        VMCDX(HWND hWnd, int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2, bool screenflag=false);
        ~VMCDX();

        DWORD                DeviceType(){ return _VMC_DEVICE_ID_2DDX; }
    

        //------------------------------------------------------------------------------------
        LPDIRECTDRAW         GetDirectDraw (void){ return (LPDIRECTDRAW)pDD2; }
        LPDIRECTDRAWSURFACE  GetPrimSurface(void){ return (LPDIRECTDRAWSURFACE)pPri; }
        LPDIRECTDRAWSURFACE  GetBackSurface(void){ return (LPDIRECTDRAWSURFACE)pBack; }


        void*       GetCanvas              (void)  { return p_canvas_; };


        

        bool        Create                 (void* hWnd,
                                            int x=0, int y=0,
                                            uint_t width = 800, uint_t height = 600,
                                            uint_t depth = 2,
                                            bool screenflag = false
                                          );                         //DX Fullscreen or Windows


        bool        ChangeScreenSize       (int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2, bool fullscreen = true);
        void        Destroy                (void);                                 //消滅DX物件


        long        ChangeCoopLevel        (bool bfullscreen);


        void        ReleaseSurface         (void);
        
        bool        BeginFrame             (DWORD argv=0);
        void        EndFrame               (void);

        void        Flip                   (void);                                    //將一記憶體貼至螢幕
        void        ClippedFlip            (CMemCanvas *src, POINT point, RECT rect); //從來源圖圈選一塊貼至螢幕位置 x,y
        void        Flip                   (CMemCanvas *src, POINT point);            //將一記憶體貼至螢幕位置 x,y
        bool        ClearScreen            (DWORD color);                             //清除螢幕
        

        //----------Font----------------------------------------------------------------------
        bool        Show_Text(LPCSTR str,int x, int y, COLORREF color);
        //------------------------------------------------------------------------------------
    
    };

};//namespace LGB

#endif

#endif
