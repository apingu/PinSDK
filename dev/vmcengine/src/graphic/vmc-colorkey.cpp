//VMCColorKey.cpp 2D colorkey Blit function
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


namespace LGB
{

    extern word_t    _PIXEL_WIDTH_SHIFT;  //image width per pixel
    extern void ( * __ColorKey_Blend )( ubyte_t* dest, int colorkey, const ubyte_t* src, int count );
    
    namespace VMCDraw
    {
    
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(以第一點為透空點)
        //=============================================================================
        int ColorKeyBlit( const Icon* Src, LG::Point point, CMemCanvas* Dest )
        {
            DWORD KeyColor  = *( word_t* ) ( ( CPIcon* )Src )->GetPixel();
            
            /*錯誤檢查*/
            if ( Error_Condition( Src ) )
            { return _P_ERROR; }
            
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            
            if ( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            
            int length  = ( _RECT_LENGTH( clipper_rect.right, clipper_rect.left ) <<
                            _PIXEL_WIDTH_SHIFT );
            int tell    = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                       ( clipper_rect.left << _PIXEL_WIDTH_SHIFT );       //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                       ( point.y * Dest->GetWidthSize() ) +
                                       ( point.x << _PIXEL_WIDTH_SHIFT ) ;       //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            
            while ( row < tell )
            {
                __ColorKey_Blend( VMCptr, KeyColor, Srcptr, length );
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                row++;
            }
            
            return true;
        }
        
        
        
        
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(傳入透空值)
        //=============================================================================
        int ColorKeyBlit( const Icon* Src,
                          LG::Point point,
                          CMemCanvas* Dest,
                          COLORTYPE KeyColor )
        {
            /*錯誤檢查*/
            if ( Error_Condition( Src ) )
            { return _P_ERROR; }
            
            /*取出來源檔的檔頭*/
            point.x = point.x - ( ( Icon* )Src )->GetClipOffsetX();
            point.y = point.y - ( ( Icon* )Src )->GetClipOffsetY();
            /*-----------裁圖------------*/
            LG::Rect clipper_rect( 0, 0, ( ( Icon* )Src )->GetClipWidth() - 1, ( ( Icon* )Src )->GetClipHeight() - 1 );
            
            if ( VMC_Clipper( &clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight() ) )
            { return _P_OK; }
            
            int length  = ( _RECT_LENGTH( clipper_rect.right, clipper_rect.left ) <<
                            _PIXEL_WIDTH_SHIFT );
            int tell    = _RECT_LENGTH( clipper_rect.bottom, clipper_rect.top );
            /*---------- 一般 ------------------*/
            register ubyte_t* Srcptr = ( ( CPIcon* )Src )->GetPixel() +
                                       ( clipper_rect.top * ( ( Icon* )Src )->GetWidthSize() ) +
                                       ( clipper_rect.left << _PIXEL_WIDTH_SHIFT );       //將來源記憶體的控制指標移至第一行開始的位置
            register ubyte_t* VMCptr = Dest->GetPixel() +
                                       ( point.y * Dest->GetWidthSize() ) +
                                       ( point.x << _PIXEL_WIDTH_SHIFT ) ;       //將目的記憶體的控制指標移至第一行開始的位置
            int row = 0;
            
            while ( row < tell )
            {
                __ColorKey_Blend( VMCptr, KeyColor, Srcptr, length );
                VMCptr += Dest->GetWidthSize(); // 指向下一列 pixel 起始位址
                Srcptr += ( ( Icon* )Src )->GetWidthSize();
                row++;
            }
            
            return true;
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(以第一點為透空點 ， 50%)
        //=============================================================================
        int ColorKey16DarkenBlit( const Icon* Src,
                                  LG::Point point,
                                  CMemCanvas* Dest )
        {
            return _P_ERROR;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(傳入透空值, 50% 透明)
        //=============================================================================
        int ColorKey16DarkenBlit( const Icon* Src,
                                  LG::Point point,
                                  CMemCanvas* Dest,
                                  COLORTYPE KeyColor )
        {
            return _P_ERROR;
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(以第一點為透空點  ， 傳入透明值)
        //=============================================================================
        int ColorKey16DarkenBlit( const Icon* Src,
                                  LG::Point point,
                                  CMemCanvas* Dest,
                                  int Diaphaneity )
        {
            return _P_ERROR;
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC: KeyColor貼圖(傳入透空值  ， 傳入透明值)
        //=============================================================================
        int ColorKey16DarkenBlit( const Icon* Src,
                                  LG::Point point,
                                  CMemCanvas* Dest,
                                  COLORTYPE KeyColor,
                                  int Diaphaneity )      //KeyColor圖透明比例貼
        {
            return _P_ERROR;
        }
        
    };
};
