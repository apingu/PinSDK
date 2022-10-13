#include "ivmc-draw2d.h"
#include "vmc-draw.h"

namespace LGB
{
    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    IVMCDraw2d::IVMCDraw2d()
    {
        p_canvas_ = NULL;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    IVMCDraw2d::~IVMCDraw2d()
    {
        SAFE_DELETE( p_canvas_ );
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int IVMCDraw2d::Blit( const Icon* Src, LG::Point point )                                     //貼圖
    {
        return LGB::VMCDraw::Blit( Src, point, p_canvas_ );
    }

    int IVMCDraw2d::ClipperBlit( const Icon* Src, LG::Rect rect, LG::Point point )               //貼圖
    {
        return LGB::VMCDraw::ClipperBlit( Src, point, p_canvas_, rect );
    }

    int IVMCDraw2d::ScaleBlit( const Icon* Src, LG::Size  ssize, LG::Point point )               //縮放貼圖
    {
        return LGB::VMCDraw::ScaleBlit( Src, point, p_canvas_, ssize );
    }

    int IVMCDraw2d::Iconcpy( const Icon* Src, LG::Point point, Icon* Dest )
    {
        return LGB::VMCDraw::Iconcpy( Src, point, Dest );
    }

    int IVMCDraw2d::AlphaBlit( const Icon* Src, LG::Point point )
    {
        return LGB::VMCDraw::AlphaBlit( Src, point, p_canvas_ );
    }

    int IVMCDraw2d::AlphaClipperBlit( const Icon* Src, LG::Rect rect, LG::Point point )
    {
        return LGB::VMCDraw::AlphaClipperBlit( Src, point, p_canvas_, rect );
    }

    int IVMCDraw2d::Alpha16DarkenBlit( const Icon* Src, LG::Point point, int Diaphaneity )
    {
        return LGB::VMCDraw::Alpha16DarkenBlit( Src, point, p_canvas_, Diaphaneity );
    }

    int IVMCDraw2d::Alpha16LightenBlit( const Icon* Src, LG::Point point, int Lum )
    {
        return LGB::VMCDraw::Alpha16LightenBlit( Src, point, p_canvas_, Lum );
    }

    int IVMCDraw2d::AlphaWarpBlit( const Icon* Src, LG::Point point, int Left, int Right )
    {
        return LGB::VMCDraw::AlphaWarpBlit( Src, point, p_canvas_, Left, Right );
    }
    int IVMCDraw2d::AlphaRippleBlit( const Icon* Src, LG::Point point )
    {
        return LGB::VMCDraw::AlphaRippleBlit( Src, point, p_canvas_ );
    }

    int IVMCDraw2d::AlphaScaleBlit( const Icon* Src, LG::Size ssize, LG::Point point )
    {
        return LGB::VMCDraw::AlphaScaleBlit( Src, point, p_canvas_, ssize );
    }
}
