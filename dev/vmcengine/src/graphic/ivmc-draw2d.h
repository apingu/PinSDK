#ifndef _IVMCDRAW2D_H
#define _IVMCDRAW2D_H

#include "ivmc-device2d.h"

namespace LGB
{
    class IVMCDraw2d: public IVMCDevice2d
    {
    protected:

        bool                 fullscreen_;
        void*                vobj_hwidget_;
        CMemCanvas*          p_canvas_;

    public:

        IVMCDraw2d();
        ~IVMCDraw2d();

        // foundation device interface
        virtual DWORD  DeviceType() = 0;

        void*          GetWidget           ( void ) { return vobj_hwidget_; }
        bool           GetFullScreen       ( void ) { return fullscreen_; }


        virtual bool   Create              ( void* hWnd, int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2, bool fullscreen = true ) = 0;
        virtual bool   ChangeScreenSize    ( int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2, bool fullscreen = true ) = 0;
        virtual void   Destroy             ( void ) = 0;

        virtual bool   BeginFrame          ( DWORD argv = 0 ) = 0;
        virtual void   EndFrame            ( void ) = 0;

        virtual void   Flip                ( void ) = 0;                               //將一記憶體貼至螢幕

        //
        virtual void*  GetCanvas           ( void ) { return p_canvas_; };

        //
        int            Blit                ( const Icon* Src, LG::Point point );                                 //貼圖
        int            ClipperBlit         ( const Icon* Src, LG::Rect  rect, LG::Point point );                  //貼圖
        int            ScaleBlit           ( const Icon* Src, LG::Size  ssize, LG::Point point );                //縮放貼圖 
        //                                      
        int            Iconcpy             ( const Icon* Src, LG::Point point, Icon* Dest );
        //                                      
        int            AlphaBlit           ( const Icon* Src, LG::Point point );                                //Alpha貼圖
        int            AlphaClipperBlit    ( const Icon* Src, LG::Rect  rect, LG::Point point );                 //Alpha貼圖 
        int            Alpha16DarkenBlit   ( const Icon* Src, LG::Point point, int Diaphaneity = 128 );          //Alpha圖透明比例貼圖
        int            Alpha16LightenBlit  ( const Icon* Src, LG::Point point, int Lum );                        //Alpha圖透明比例貼圖
        int            AlphaWarpBlit       ( const Icon* Src, LG::Point point, int Left, int Right );            //Alpha彎曲貼圖
        int            AlphaRippleBlit     ( const Icon* Src, LG::Point point );                                 //Alpha波紋貼圖 
        int            AlphaScaleBlit      ( const Icon* Src, LG::Size  ssize, LG::Point point );                //Alpha縮放貼圖 


    };
}

#endif //_IVMCDRAW2D_H