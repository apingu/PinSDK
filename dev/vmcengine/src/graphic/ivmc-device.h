//
//  2D Blit engine
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
#ifndef IVMCDEVICE_H
#define IVMCDEVICE_H

#include "vmc-def.h"


// BeginFrame frame的參數
enum DRAW_MODE
{
    DRAWMODE_2D  = 0,   // 
    DRAWMODE_3D  = 1,   // 
};



namespace LGB
{
//-----------------------------------------------------------------------------
// NAME: class IVMCDevice
// DESC: base class for 2D draw screen
//------------------------------------------------------------------------------
    class IVMCDevice
    {

    private:

            LG::Point        m_WidgetPos;
            LG::Boxui        m_WidgetSize;
            //bool           fullscreen_;
            //void*          vobj_hwidget_;
            int              m_PixelsDepth;

    protected:

            uint16_t       GetPixelDepthb        (PixelFormat VedioMask);
            void           SetPixelMode          (int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, PixelFormat VedioMask = PixelFormat::R5G6B5);
            
                    
    public:
            IVMCDevice();
            virtual ~IVMCDevice();

            virtual DWORD  DeviceType() = 0; 


            virtual void*  GetWidget             (void) = 0;
            int            GetPosX               (void) { return m_WidgetPos.y; }
            int            GetPosY               (void) { return m_WidgetPos.x; }
            uint_t         GetWidth              (void) { return m_WidgetSize.width; }
            uint_t         GetHeight             (void) { return m_WidgetSize.height; }
            uint_t         GetDepthb             (void) { return m_WidgetSize.depth; }
            //virtual void   SetDepthb             (UINT depth) { m_WidgetSize.depth=depth; }

            virtual bool   GetFullScreen         (void) = 0;

            /*
            void*          GetWidget             (void) { return vobj_hwidget_; }
            uint_t         GetWidth              (void) { return m_WidgetSize.width; }
            uint_t         GetHeight             (void) { return m_WidgetSize.height; }
            uint_t         GetDepthb             (void);//get deepth byte size 
            bool           GetFullScreen         (void) { return fullscreen_; }
            */


            //bool           CheckCPUIntrin_Bit    (CPU_INTRIN dd);



            virtual bool   Create                (void* hWnd, int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2,
                                                  bool fullscreen = true) = 0;
            virtual bool   ChangeScreenSize      (int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2,
                                                  bool fullscreen = true) = 0;
            virtual void   Destroy               (void) = 0;

            virtual bool   BeginFrame            (DWORD argv = 0) = 0;
            virtual void   EndFrame              (void) = 0;

            virtual void   Flip                  (void) = 0;                               //將一記憶體貼至螢幕


    };

};//namespace LGB




#endif

