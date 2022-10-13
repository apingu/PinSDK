//VMCAlpha.cpp 2D alpha Blit function
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
#include <memio.inl>

#include "vmc-draw.h"
#include "vmc-matrix.h"
#include "vmc-clipper.h"
#include "vmc-blend_func.h"


namespace LGB
{
    //extern PixelFormat _VGA_PixelFormat;
    extern word_t    _PIXEL_WIDTH_SHIFT;  //image width per pixel
    extern void ( * __Alpha_Blend )   ( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count );
    extern void ( * __Alpha_combine )  ( ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len );
    
    namespace VMCDraw
    {
    
        int Iconcpy( const Icon* Src, LG::Point point, Icon* Dest )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            {
                return Blit( Src, point, Dest );
            }
            if( Dest->PicAlphaPtr == NULL )
            {
                return AlphaBlit( Src, point, Dest );
            }
            LG::Point orgin = point;
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int Alpha_length = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       //(clipper_rect.top * ((Icon*)Src)->GetClipWidth()) +
                                       clipper_rect.top  +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT ); //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* DestAlpha_Ptr = Dest->PicAlphaPtr +
                                           ( point.y * ( ( Icon* )Dest )->GetClipWidth() ) +
                                           point.x;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ;          //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            while( row < tell )
            {
                __Alpha_combine( DestAlpha_Ptr, Alpha_Ptr, DestAlpha_Ptr,  Alpha_length );
                __Alpha_Blend( VMCptr, Alpha_Ptr, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                DestAlpha_Ptr += ( ( Icon* )Dest )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            return _P_OK;
        }
        
        int AlphaIconcpy( const Icon* Src, LG::Point point, Icon* Dest )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            { return Blit( Src, point, Dest ); }
            if( Dest->PicAlphaPtr == NULL )
            { return AlphaBlit( Src, point, Dest ); }
            LG::Point orgin = point;
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int Alpha_length = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetClipWidth() ) +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT ); //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* DestAlpha_Ptr = Dest->PicAlphaPtr +
                                           //(point.y * ((Icon*)Dest)->GetClipWidth()) +
                                           point.y +
                                           point.x;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ;          //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            while( row < tell )
            {
                //  CombineAlpha(DestAlpha_Ptr, Alpha_length, Alpha_Ptr, DestAlpha_Ptr);
                __Alpha_Blend( VMCptr, DestAlpha_Ptr, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                DestAlpha_Ptr += ( ( Icon* )Dest )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            return _P_OK;
        }
        
        
        //=============================================================================
        // NAME: AlphaBlit()
        // DESC: Alpha貼圖                                     ok /
        //=============================================================================
        int AlphaBlit( const Icon* Src, LG::Point point, CMemCanvas* Dest )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            { return Blit( Src, point, Dest ); }
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int Alpha_length = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetClipWidth() ) +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT );          //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ;          //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            while( row < tell )
            {
                __Alpha_Blend( VMCptr, Alpha_Ptr, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            return _P_OK;
        }
        
        
        //=============================================================================
        // NAME: AlphaRLEBlit()
        // DESC: Alpha透明貼圖(傳入透明比例)                                              ok /
        //=============================================================================
        int AlphaRLEBlit( const Icon* Src, LG::Point point, CMemCanvas* Dest )
        {
            return _P_ERROR;
        }
        
        
        //=============================================================================
        // NAME: Alpha16DarkenBlit()
        // DESC: Alpha透明貼圖(傳入透明比例)                                              ok /
        //=============================================================================
        int Alpha16DarkenBlit( const Icon* Src,
                               LG::Point point,
                               CMemCanvas* Dest,
                               int Diaphaneity )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            { return Blit( Src, point, Dest ); }
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int Alpha_length = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetClipWidth() ) +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT ); //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ; //將目的記憶體的控制指標移至第一行開始的位置
            register ubyte_t* New_Alpha = NULL;
            int row = 0;
            New_Alpha = new ubyte_t[Alpha_length];
            while( row < tell )
            {
                for( int a_c = 0 ; a_c < Alpha_length; a_c++ )
                {
                    //                  if(Alpha_Ptr[a_c] > Diaphaneity)
                    //                  {
                    //                      New_Alpha[a_c] = Alpha_Ptr[a_c] - (256 - Diaphaneity);
                    //                  }
                    //                  else
                    //                      New_Alpha[a_c]=0;
                    int aa = Alpha_Ptr[a_c] - ( 256 - Diaphaneity );
                    if( aa < 0 )
                    { New_Alpha[a_c] = 0; }
                    else
                    { New_Alpha[a_c] = aa; }
                }
                __Alpha_Blend( VMCptr, New_Alpha, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            delete [] New_Alpha;
            return _P_OK;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:  Alpha加亮貼圖
        //=============================================================================
        int Alpha16LightenBlit( const Icon* Src,
                                LG::Point point,
                                CMemCanvas* Dest,
                                int Lum )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            { return Blit( Src, point, Dest ); }
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int Alpha_length = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetClipWidth() ) +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT );          //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ;          //將目的記憶體的控制指標移至第一行開始的位置
            register ubyte_t* New_Alpha = NULL;
            int row = 0;
            New_Alpha = new ubyte_t[Alpha_length];
            while( row < tell )
            {
                for( int a_c = 0 ; a_c < Alpha_length; a_c++ )
                {
                    int a   = ( Alpha_Ptr[a_c] == 0 ) ? 0 : ( Alpha_Ptr[a_c] + ( 256 - Lum ) );
                    if( a > 0XFF )
                    { a = 0XFF; }
                    New_Alpha[a_c] = a;
                }
                __Alpha_Blend( VMCptr, New_Alpha, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            delete [] New_Alpha;
            return _P_OK;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:  Alpha
        //=============================================================================
        int AlphaClipperBlit( const Icon* Src, LG::Point point, CMemCanvas* Dest, LG::Rect rect )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( No_Alpha_Condition( Src ) )
            { return Blit( Src, point, Dest ); }
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            LG::Point srcp = point;
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            //LG::Rect clipper_rect   ={Rect.left, Rect.top, Rect.right, Rect.bottom};
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            if( rect.top < clipper_rect.top )
            { rect.top = clipper_rect.top; }
            if( rect.left < clipper_rect.left )
            { rect.left = clipper_rect.left; }
            if( rect.right > clipper_rect.right )
            { rect.right = clipper_rect.right; }
            if( rect.bottom > clipper_rect.bottom )
            { rect.bottom = clipper_rect.bottom; }
            point.x += ( rect.left - ( point.x - srcp.x ) );
            point.y += ( rect.top - ( point.y - srcp.y ) );
            int Alpha_length = _RECT_LENGTH( rect.right, rect.left );
            int length      = Alpha_length << _PIXEL_WIDTH_SHIFT;
            int tell        = _RECT_LENGTH( rect.bottom, rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetClipWidth() ) +
                                       clipper_rect.left;//將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( clipper_rect.left << _PIXEL_WIDTH_SHIFT );          //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                    ( point.y * Dest->GetWidthSize() ) +
                                    ( point.x << _PIXEL_WIDTH_SHIFT ) ;          //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            while( row < tell )
            {
                __Alpha_Blend( VMCptr, Alpha_Ptr, Srcptr, length );
                Alpha_Ptr += ( ( Icon* )Src )->GetClipWidth();
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                row++;
            }
            return _P_OK;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:  Alpha縮放貼圖
        //=============================================================================
        int AlphaScaleBlit( const Icon* Src,
                            LG::Point point,
                            CMemCanvas* Dest,
                            LG::Size ssize )
        {
            /*錯誤檢查*/
            if( Error_Condition( Src ) )
            { return _P_ERROR; }
            if( ( ssize.cx <= 0 ) || ( ssize.cy <= 0 ) )
            { return _P_OK; }
            if( No_Alpha_Condition( Src ) )
            { return ScaleBlit( Src, point, Dest, ssize ); }
            if( ( Src->GetWidth() == 0 ) || ( Src->GetHeight() == 0 ) )
            { return _P_ERROR; }
            //float cxp = (float)(((float)ssize.cx)/((float)Src->GetWidth()));
            //float cyp = (float)(((float)ssize.cy)/((float)Src->GetHeight()));
            ssize.cx = ( Src->GetClipWidth() * ssize.cx ) / Src->GetWidth();
            ssize.cy = ( Src->GetClipHeight() * ssize.cy ) / Src->GetHeight();
            int    ddepth = Dest->GetDepth();
            double dx = ( double )( ( Icon* )Src )->GetClipWidth() / ( double )ssize.cx;
            double dy = ( double )( ( Icon* )Src )->GetClipHeight() / ( double )ssize.cy;
            //float  Fy= (float) ((Icon*)Src)->GetClipHeight() / (float) ssize.cy;
            /*取出來源檔的檔頭*/
            point.x = point.x - ( int ) ( ( float ) ( ( Icon* )Src )->GetClipOffsetX() / dx );
            point.y = point.y - ( int ) ( ( float ) ( ( Icon* )Src )->GetClipOffsetY() / dy );
            //point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            //point.y = point.y - ((Icon*)Src)->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ssize.cx - 1, ssize.cy - 1 );
            if( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            int length  = _RECT_LENGTH( clipper_rect.right, clipper_rect.left );
            int tell    = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 搬移 ------------------*/
            //將來源記憶體的控制指標移至第一行開始的位置
            int shifttop  = clipper_rect.top * dy;
            int shiftleft = clipper_rect.left * dx;
            register ubyte_t* Alpha_Ptr = Src->PicAlphaPtr +
                                       ( ( ( int )( shifttop ) ) * ( ( Icon* )Src )->GetClipWidth() ) +
                                       ( ( int )( shiftleft ) ); //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                    ( ( ( int )( shifttop ) ) * ( ( Icon* )Src )->GetWidthSize() ) +
                                    ( ( ( int )( shiftleft ) ) << _PIXEL_WIDTH_SHIFT );
            //將目的記憶體的控制指標移至第一行開始的位置
            //register ubyte_t* VMCptr = Dest->GetPixel() + (point.y * Dest->GetWidthSize()) + (point.x << _PIXEL_WIDTH_SHIFT) ;
            //        VMC_Matrix_Scaling(((Icon*)Src)->GetClipWidth(),
            //                            ((Icon*)Src)->GetClipHeight(),
            //                            Srcptr,
            //                            ssize.cx,
            //                            ssize.cy,
            //                            point,
            //                            Dest->GetClipWidth(),
            //                            Dest->GetClipHeight(),
            //                            Dest->GetPixel());
            //double dx = (double)((Icon*)Src)->GetClipWidth() / (double)ssize.cx;
            int s_w = ( ( Icon* )Src )->GetWidthSize();
            int a_w = ( ( Icon* )Src )->GetClipWidth();
            int d_w = ( ( Icon* )Dest )->GetWidthSize();
            int d_lw = ( d_w - ( length * ddepth ) );
            ubyte_t* dest = Dest->GetPixel() + ( point.y * d_w ) + ( point.x * ddepth );
            float diffy = 0;
            float diffx = 0;
            int   dify = 0;
            int   difx = 0;
            int   a_yl = 0;
            int   s_yl = 0;
            for( int py = 0; py < tell; py++ )
            {
                diffx = 0;
                dify = ( int )floor( diffy );
                a_yl = dify * a_w;
                s_yl = dify * s_w;
                for( int px = 0; px < length; px++ )
                {
                    difx = ( int )floor( diffx );
                    ubyte_t* alpha = Alpha_Ptr + a_yl + ( difx );
                    if( ( *alpha ) == 0XFF )
                    { __pfmemcpy( dest, Srcptr + s_yl + ( difx << _PIXEL_WIDTH_SHIFT ), ddepth ); }
                    else
                    {
                        __Alpha_Blend( dest,
                                       alpha,
                                       Srcptr + s_yl + ( difx << _PIXEL_WIDTH_SHIFT ),
                                       ddepth
                                     );
                    }
                    //*/
                    diffx += ( float )dx;
                    dest += ddepth;
                }
                diffy += ( float )dy;
                dest  += d_lw;
            }
            return _P_OK;
        }
        
        //=============================================================================
        // NAME:
        // DESC:  Alpha彎曲貼圖
        //=============================================================================
        int AlphaWarpBlit( const Icon* Src,
                           LG::Point point,
                           CMemCanvas* Dest,
                           int Left,
                           int Right )
        {
            return _P_ERROR;
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC: Alpha波紋貼圖
        //=============================================================================
        int AlphaRippleBlit( const Icon* Src, LG::Point point, CMemCanvas* Dest )
        {
            return _P_ERROR;
        }
    };
};
