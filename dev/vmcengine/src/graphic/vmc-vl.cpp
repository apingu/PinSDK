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
//
//                                               Copyright    (C)    1999    Pin
//
#include "vmc-vl.h"


namespace LGB
{

    extern  word_t  _PIXEL_WIDTH_SHIFT;

    //=============================================================================
    //
    //
    //=============================================================================
    VMCVL::VMCVL()
    {
        p_canvas_ = NULL;
    }


    //=============================================================================
    //
    //
    //=============================================================================
    VMCVL::~VMCVL()
    {
        Destroy();
    }


    //=============================================================================
    // NAME: DXInit()
    // DESC: function to create directX
    //=============================================================================
    bool VMCVL::Create( void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool screenflag )
    {
        if( ( p_canvas_ ) != 0 )
            SAFE_DELETE( p_canvas_ );

        if( ( width <= 0 ) || ( height <= 0 ) || ( depth < 0 ) )
        {
            return false;
        }

        vobj_hwidget_ = hWnd;
        ChangeScreenSize( x, y, width, height, depth, screenflag );

        //width  = GetWidth();
        //height = GetHeight();
        //depth  = GetDepthb();

        p_canvas_ = ::new CMemCanvas;
        p_canvas_->Allot( width, height, depth );
        memset( p_canvas_->GetPixel(), 0, p_canvas_->GetClipHeight() * p_canvas_->GetWidthSize() );

        return true;
    }



    //=============================================================================
    // NAME: SetFullScreen
    // DESC: 
    //=============================================================================
    bool VMCVL::ChangeScreenSize( int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen )
    {
        if( ( width == 0 ) || ( height == 0 ) || ( depth == 0 ) )
        {
            return false;
        }


        fullscreen_ = fullscreen;

        if( depth == 2 )
            SetPixelMode( x, y, width, height );
        else
            SetPixelMode( x, y, width, height, PixelFormat::A8B8G8R8 );

        //IVMCDevice::ChangeScreenSize(width, height, depth, fullscreen);
        ShowCursor( 0 );
        InvalidateRect( ( HWND )vobj_hwidget_, NULL, false );
        return true;
    }

    //=============================================================================
    // NAME: SetFullScreen
    // DESC: 
    //=============================================================================
    long VMCVL::ChangeCoopLevel( bool bfullscreen )
    {
        return ChangeScreenSize( GetPosX(), GetPosY(), GetWidth(), GetHeight(), GetDepthb(), bfullscreen );
    }

    //=============================================================================
    // NAME: Destroy()
    // DESC: Desrtoy all directX object
    //=============================================================================-
    void VMCVL::Destroy()
    {
        SAFE_DELETE( p_canvas_ );

        return;
    }
    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    bool VMCVL::BeginFrame( DWORD argv )
    {
        return true;
    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    void VMCVL::EndFrame( void )
    {

    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    void VMCVL::Flip( void )
    {
        return;
    }


};
