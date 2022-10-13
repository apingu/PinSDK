// IVMCDevice.cpp 2D engine super class
//
//
//  DWORD  _R_SEEK;
//  DWORD  _G_SEEK;
//  DWORD  _B_SEEK;
//  DWORD  _R_MASK;
//  DWORD  _G_MASK;
//  DWORD  _B_MASK;
//  qword_t   _MMX_R_MASK;
//  qword_t   _MMX_G_MASK;
//  qword_t   _MMX_B_MASK;
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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-system.h>
//
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件


//04. == 此專案內.h 文件
#include "icon.h"
#include "vmc-matrix.h"
#include "vmc-clipper.h"
#include "vmc-blend_func.h"
#include "vmc-blend_mmx_func.h"
#include "vmc-blend_sse_func.h"
#include "vmc-global.h"
#include "ivmc-device.h"

namespace LGB
{
    // 加速用
    // VMCWidget _Widget;              //設定模式的長寬與點的大小
    extern word_t    _PIXEL_WIDTH_SHIFT;  //image width per pixel
    extern void( *__Alpha_Blend )    ( ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count );
    extern void( *__ColorKey_Blend ) ( ubyte_t* dest, int colorkey, const ubyte_t* src, int count );
    extern void( *__Alpha_combine )  ( ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len );

    //=============================================================================
    // NAME: 
    // DESC: Constructor func
    //=============================================================================
    IVMCDevice::IVMCDevice()
    {
        //m_GCDepth = 0;
        //vobj_hwidget_=NULL;
        //m_fullscreen=false;
        SetPixelMode( 0, 0, 800, 600, CPIcon::GetPixelFmt() );
        //Set_Font("新細明體", 50, 25);
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    IVMCDevice::~IVMCDevice()
    {

    }


    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool IVMCDevice::Create(void* hWnd, int width, int height, int depth, bool fullscreen)
     {
         vobj_hwidget_ = hWnd;
         m_WidgetSize.width = width;
         m_WidgetSize.height = height;
         m_WidgetSize.depth = depth;
         fullscreen_ = fullscreen;
         return true;
     }



    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool IVMCDevice::ChangeScreenSize(int width, int height, int depth, bool fullscreen)
    {
        m_WidgetSize.width = width;
        m_WidgetSize.height = height;
        m_WidgetSize.depth = depth;
        fullscreen_ = fullscreen;
        return true;
    }
    */



    /*
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void IVMCDevice::RegisterRenderScreen(void* hWnd, int width, int height, int depth)
    {
        vobj_hwidget_ = hWnd;
        m_WidgetSize.width = width;
        m_WidgetSize.height = height;
        m_WidgetSize.depth = depth;
        //m_fullscreen   = screenflag;

        //_Widget.hWnd       = hWnd;
        //_Widget.width      = width;
        //_Widget.height     = height;
        //_Widget.depth      = depth;
        //_Widget.fullscreen = screenflag;
        return;
    }
    */

    //=============================================================================
    // NAME: 
    // DESC:  set vedio mask , width shift, copy func, check mmx
    //=============================================================================
    /*
    UINT IVMCDevice::GetDepthb(void)
    {
        return m_WidgetSize.depth;
    }
    */


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    uint16_t IVMCDevice::GetPixelDepthb( PixelFormat VedioMask )
    {
        return LOBYTE( VedioMask );
    }


    //=============================================================================
    // NAME: 
    // DESC:  set vedio mask , width shift, copy func, check mmx
    //=============================================================================
    void IVMCDevice::SetPixelMode( int x, int y, uint_t width, uint_t height, PixelFormat VedioMask )
    {
        //_VGA_PixelFormat   = VedioMask;
        bool is_MMX = pfkrl::CheckCPUInStruction( _CPU_INTRIN_MMX );
        bool is_SSE = pfkrl::CheckCPUInStruction( _CPU_INTRIN_SSSE3 );
        m_WidgetPos( x, y );
        m_WidgetSize( width, height, GetPixelDepthb( VedioMask ) );

        //m_WidgetSize.depth = LOBYTE(VedioMask);
        //m_VedioMask = VedioMask;

        switch( VedioMask )
        {
            case PixelFormat::R5G6B5: //565
            _PIXEL_WIDTH_SHIFT = 1;
            m_WidgetSize.depth = 2;

            //_R_SEEK     = 11;
            //_G_SEEK     = 5;
            //_B_SEEK     = 0;

            //_R_MASK     = 0X0000F800;
            //_G_MASK     = 0X000007E0;
            //_B_MASK     = 0X0000001F;

            //_MMX_R_MASK = 0XF800F800F800F800;
            //_MMX_G_MASK = 0X07E007E007E007E0;
            //_MMX_B_MASK = 0X001F001F001F001F;
#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_SSE_Alpha_565_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_16b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_MMX_Alpha_565_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_16b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_565_Blend;
                __ColorKey_Blend = VMC_ColorKey_16b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        case PixelFormat::R5G5B5: //555

            _PIXEL_WIDTH_SHIFT = 1;
            m_WidgetSize.depth = 2;

            //_R_SEEK     = 5;
            //_G_SEEK     = 5;
            //_B_SEEK     = 0;

            //_R_MASK = 0X00007C00;
            //_G_MASK = 0X000003E0;
            //_B_MASK = 0X0000001F;

            //_MMX_R_MASK = 0X7C007C007C007C00;
            //_MMX_G_MASK = 0X03E003E003E003E0;
            //_MMX_B_MASK = 0X001F001F001F001F;
#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_SSE_Alpha_555_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_16b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_MMX_Alpha_555_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_16b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_555_Blend;
                __ColorKey_Blend = VMC_ColorKey_16b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        case PixelFormat::R6G5B5://p

            _PIXEL_WIDTH_SHIFT = 1;
            m_WidgetSize.depth = 2;

            //_R_SEEK     = 10;
            //_G_SEEK     = 5;
            //_B_SEEK     = 0;

            //_R_MASK = 0X0000FC00;
            //_G_MASK = 0X000003E0;
            //_B_MASK = 0X0000001F;

            //_MMX_R_MASK = 0X7C007C007C007C00;
            //_MMX_G_MASK = 0X03E003E003E003E0;
            //_MMX_B_MASK = 0X001F001F001F001F;
#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_SSE_Alpha_655_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_16b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_MMX_Alpha_655_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_16b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_655_Blend;
                __ColorKey_Blend = VMC_ColorKey_16b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        case PixelFormat::R5G5B6:

            _PIXEL_WIDTH_SHIFT = 1;
            m_WidgetSize.depth = 2;

#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_SSE_Alpha_556_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_16b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_MMX_Alpha_556_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_16b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_556_Blend;
                __ColorKey_Blend = VMC_ColorKey_16b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        case PixelFormat::R8G8B8A8:

            _PIXEL_WIDTH_SHIFT = 2;
            m_WidgetSize.depth = 4;

#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_32b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_32b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_ColorKey_32b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        case PixelFormat::A8R8G8B8:

            _PIXEL_WIDTH_SHIFT = 2;
            m_WidgetSize.depth = 4;

#if defined(_X86_)
            if( is_SSE )
            {
                __Alpha_Blend = VMC_SSE_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_SSE_ColorKey_32b_Blend;
                __Alpha_combine = VMC_SSE_Alpha_combine;
            }
            else if( is_MMX )
            {
                __Alpha_Blend = VMC_MMX_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_MMX_ColorKey_32b_Blend;
                __Alpha_combine = VMC_MMX_Alpha_combine;
            }
            else
#endif
            {
                __Alpha_Blend = VMC_Alpha_32b_Blend;
                __ColorKey_Blend = VMC_ColorKey_32b_Blend;
                __Alpha_combine = VMC_Alpha_combine;
            }
            break;

        default:
            Pf_WARNING( "IVMCDevice::SetPixelMode: No define yet!!!" );
            break;

        }
        CPIcon::SetPixelFmt( VedioMask );
    }



    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================


};//namespace LGB







