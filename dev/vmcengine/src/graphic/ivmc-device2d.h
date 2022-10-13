#ifndef _IVMCDEVICE2D_H
#define _IVMCDEVICE2D_H

#include "ivmc-device.h"

namespace LGB
{
    class IVMCDevice2d : public IVMCDevice
    {
    public:

        IVMCDevice2d(){}
        ~IVMCDevice2d(){}

        // foundation device interface
        virtual DWORD  DeviceType()=0; 

        virtual void*  GetWidget             (void)=0;
        virtual bool   GetFullScreen         (void)=0;


        virtual bool   Create                (void* hWnd, int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
                                              bool fullscreen=true)=0;
        virtual bool   ChangeScreenSize      (int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
                                              bool fullscreen=true)=0;
        virtual void   Destroy               (void)=0;

        virtual bool   BeginFrame            (DWORD argv=0)=0;
        virtual void   EndFrame              (void)=0;

        virtual void   Flip                  (void)=0;                                                        //將一記憶體貼至螢幕


        virtual void*  GetCanvas             (void) { return NULL; };


        // Draw interface
        virtual int Blit                  (const Icon* Src, LG::Point point)=0;                               //貼圖
        virtual int ClipperBlit           (const Icon* Src, LG::Rect  rect,  LG::Point point)=0;              //貼圖
        virtual int ScaleBlit             (const Icon* Src, LG::Size  ssize, LG::Point point)=0;              //縮放貼圖 

        //
        virtual int Iconcpy               (const Icon* Src, LG::Point point, Icon* Dest)=0;
        //
        virtual int AlphaBlit             (const Icon* Src, LG::Point point)=0;                               //Alpha貼圖
        virtual int AlphaClipperBlit      (const Icon* Src, LG::Rect  rect, LG::Point point)=0;               //Alpha貼圖 
        virtual int Alpha16DarkenBlit     (const Icon* Src, LG::Point point, int Diaphaneity = 128)=0;        //Alpha圖透明比例貼圖
        virtual int Alpha16LightenBlit    (const Icon* Src, LG::Point point, int Lum)=0;                      //Alpha圖透明比例貼圖
        virtual int AlphaWarpBlit         (const Icon* Src, LG::Point point, int Left, int Right)=0;          //Alpha彎曲貼圖
        virtual int AlphaRippleBlit       (const Icon* Src, LG::Point point)=0;                               //Alpha波紋貼圖 
        virtual int AlphaScaleBlit        (const Icon* Src, LG::Size  ssize, LG::Point point)=0;              //Alpha縮放貼圖 
    };
}

#endif //_IVMCDEVICE2D_H