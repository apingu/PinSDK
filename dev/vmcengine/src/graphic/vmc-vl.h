// ===========================================================================================
//
// Stick Picture Kernel Class
//
//
//
//                                               Use attention +ddraw.lib dxguid.lib D3d8.lib
//                                               Copyright    (C)    1999    Pin
// =========================================================================================
#ifndef VMCVL_H
#define VMCVL_H


#include "ivmc-draw2d.h"


#define _VMC_DEVICE_ID_2DVL   4

namespace LGB
{
    class VMCVL : public IVMCDraw2d
    {

    private:

        //CMemCanvas*          p_canvas_;

    public :

        VMCVL();
        ~VMCVL();
    
        DWORD         DeviceType(){return _VMC_DEVICE_ID_2DVL;}

        void*         GetCanvas            (void)  { return p_canvas_; };

        bool          Create               (void* hWnd,
                                            int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
                                            bool screenflag = false);

        // depth 沒作用
        bool          ChangeScreenSize     (int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2, bool fullscreen = true);

        void          Destroy              (void);                                 //消滅DX物件

        long          ChangeCoopLevel      (bool bfullscreen);
        
        bool          BeginFrame           (DWORD argv=0);
        void          EndFrame             (void);

        void          Flip                 (void);                                    //將一記憶體貼至螢幕
    };

};//namespace LGB

#endif
