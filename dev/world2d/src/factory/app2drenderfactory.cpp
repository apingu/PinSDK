#include "App2dFactory.h"
#include <VMCDraw.h> /*source*/

namespace App2dFactory
{
    Render2d::Render2d()
    {

    }

    Render2d::~Render2d()
    {

    }

    int Render2d::AlphaScaleBlit( const Icon *Src, LG::Point point, LG::Size ssize )
    {
        return LGB::VMCDraw::AlphaScaleBlit( Src, point, GetCanvas(), ssize );
    }


    int Render2d::SpceialEffectBlit( CPIcon* Src, LG::Point point, BlitMode mode, int argument1, int argument2 )
    {
        if ( GetCanvas() == NULL )
        {
            return 0;
        }

        switch ( mode )
        {
        case _BLIT_M:
            return LGB::VMCDraw::Blit( Src, point, GetCanvas() );

        case _DARKEN_M:
            return LGB::VMCDraw::Darken16Blit( Src, point, GetCanvas(), argument1 );

        case _LIGHTEN_M:
            return LGB::VMCDraw::Lighten16Blit( Src, point, GetCanvas(), argument1 );

        case _SEAM_M:
            return LGB::VMCDraw::SeamBlit( Src, point, GetCanvas(), argument1 );

        case _INTERLACTED_M:
            return LGB::VMCDraw::InterlactedBlit( Src, point, GetCanvas(), argument1 );

        case _WARP_M:
            return LGB::VMCDraw::WarpBlit( Src, point, GetCanvas(), argument1, argument2 );

        case _ALPAH_DARKEN_M:
            return LGB::VMCDraw::Alpha16DarkenBlit( Src, point, GetCanvas(), argument1 );

        case _ALPHA_LIGHTEN_M:
            return LGB::VMCDraw::Alpha16LightenBlit( Src, point, GetCanvas(), argument1 );

        case _ALPHA_WARP_M:
            return LGB::VMCDraw::AlphaWarpBlit( Src, point, GetCanvas(), argument1, argument2 );

        case _ALPHA_RIPPLE_M:
            return LGB::VMCDraw::AlphaRippleBlit( Src, point, GetCanvas() );

        default://case _STICK_M:
            return LGB::VMCDraw::AlphaBlit( Src, point, GetCanvas() );
        }
    }

};