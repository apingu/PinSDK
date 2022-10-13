#ifndef _VMCBLENDFUNC_H
#define _VMCBLENDFUNC_H

#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <math.h>
#include <pf-def.h>

#if defined(_X86_)
    #include <xmmintrin.h>
    #include <emmintrin.h>
#endif

namespace LGB
{
    //////////////////////////////////////////////////////////////////////////
    //
    
#define _565_R_MASK    0X0000F800
#define _565_G_MASK    0X000007E0
#define _565_B_MASK    0X0000001F
    
#define _555_R_MASK    0X00007C00
#define _555_G_MASK    0X000003E0
#define _555_B_MASK    0X0000001F
    
#define _556_R_MASK    0X0000F800
#define _556_G_MASK    0X000003E0
#define _556_B_MASK    0X0000003F
    
#define _655_R_MASK    0X0000FC00
#define _655_G_MASK    0X000003E0
#define _655_B_MASK    0X0000001F
    
    
    //=============================================================================
    // NAME:  VMC_Alpha_565_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK     = 0X0000F800;
    //          _G_MASK     = 0X000007E0;
    //          _B_MASK     = 0X0000001F;
    //
    //=============================================================================
    inline void VMC_Alpha_565_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                __pfmemcpy( dest, src, 2 );
                //*(word_t*)dest = (*(word_t *)src);
            }
            else if( *alpha != 0X00 )
            {
                DWORD n_a = ( _FULL_ALPHA - ( *alpha ) );
                *( word_t* )dest = ( word_t )( ( ( ( ( ( ( *( word_t* )src )  &_565_R_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_565_R_MASK ) * n_a ) ) >> 8 )  &_565_R_MASK )
                                             | ( ( ( ( ( ( *( word_t* )src )  &_565_G_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_565_G_MASK ) * n_a ) ) >> 8 )  &_565_G_MASK )
                                             | ( ( ( ( ( ( *( word_t* )src )  &_565_B_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_565_B_MASK ) * n_a ) ) >> 8 )  &_565_B_MASK )  );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    //=============================================================================
    // NAME:  VMC_Alpha_555_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X00007C00;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000001F;
    //=============================================================================
    inline void VMC_Alpha_555_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                //*(word_t*)dest = (*(word_t *)src);
                __pfmemcpy( dest, src, 2 );
            }
            else if( *alpha != 0X00 )
            {
                DWORD n_a = ( _FULL_ALPHA - ( *alpha ) );
                *( word_t* )dest = ( word_t )( ( ( ( ( ( ( *( word_t* )src )  &_555_R_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_555_R_MASK ) * n_a ) ) >> 8 )  &_555_R_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_555_G_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_555_G_MASK ) * n_a ) ) >> 8 )  &_555_G_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_555_B_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_555_B_MASK ) * n_a ) ) >> 8 )  &_555_B_MASK )  );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    //=============================================================================
    // NAME:  VMC_Alpha_556_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X0000F800;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000003F;
    //=============================================================================
    inline void VMC_Alpha_556_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                __pfmemcpy( dest, src, 2 );
                //*(word_t*)dest = (*(word_t *)src);
            }
            else if( *alpha != 0X00 )
            {
                DWORD n_a = ( _FULL_ALPHA - ( *alpha ) );
                *( word_t* )dest = ( word_t )( ( ( ( ( ( ( *( word_t* )src )  &_556_R_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_556_R_MASK ) * n_a ) ) >> 8 )  &_556_R_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_556_G_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_556_G_MASK ) * n_a ) ) >> 8 )  &_556_G_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_556_B_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_556_B_MASK ) * n_a ) ) >> 8 )  &_556_B_MASK )  );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    
    //=============================================================================
    // NAME:  VMC_Alpha_655_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X0000FC00;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000001F;
    //=============================================================================
    inline void VMC_Alpha_655_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                __pfmemcpy( dest, src, 2 );
                //*(word_t*)dest = (*(word_t *)src);
            }
            else if( *alpha != 0X00 )
            {
                DWORD n_a = ( _FULL_ALPHA - ( *alpha ) );
                *( word_t* )dest = ( word_t )( ( ( ( ( ( ( *( word_t* )src )  &_655_R_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_655_R_MASK ) * n_a ) ) >> 8 )  &_655_R_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_655_G_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_655_G_MASK ) * n_a ) ) >> 8 )  &_655_G_MASK )
                                         | ( ( ( ( ( ( *( word_t* )src )  &_655_B_MASK ) * ( *alpha ) ) + ( ( ( *( word_t* )dest )  &_655_B_MASK ) * n_a ) ) >> 8 )  &_655_B_MASK )  );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    inline void VMC_ColorKey_16b_Blend( ubyte_t* dest, int colorkey, const ubyte_t* src, int count )
    {
        int col = 0;
        while( col < count )
        {
            if( colorkey != *( word_t* )src )
            { *( word_t* )dest = *( word_t* )src; }
            //__pfmemcpy(dest, src, 2);
            src  = src  + 2;
            dest = dest + 2;
            col  = col  + 2;
        }
    }
    
    //=============================================================================
    // NAME:  VMC_Light_565_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK     = 0X0000F800;
    //          _G_MASK     = 0X000007E0;
    //          _B_MASK     = 0X0000001F;
    //
    //=============================================================================
    inline void VMC_Light_565_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        word_t RColor = 0, GColor = 0, BColor = 0;
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                *( word_t* )dest = ( *( word_t* )src );
                //__pfmemcpy(dest, src, 2);
            }
            else if( *alpha != 0X00 )
            {
                RColor = ( ( ( ( ( *( word_t* )src )  &_565_R_MASK ) ) + ( ( ( *( word_t* )dest )  &_565_R_MASK ) ) ) >> 8 );
                GColor = ( ( ( ( ( *( word_t* )src )  &_565_G_MASK ) ) + ( ( ( *( word_t* )dest )  &_565_G_MASK ) ) ) >> 8 );
                BColor = ( ( ( ( ( *( word_t* )src )  &_565_B_MASK ) ) + ( ( ( *( word_t* )dest )  &_565_B_MASK ) ) ) >> 8 );
                if( RColor > 0XFF ) { RColor = 0XFF; }
                if( GColor > 0XFF ) { GColor = 0XFF; }
                if( BColor > 0XFF ) { BColor = 0XFF; }
                *( word_t* )dest = ( word_t )( ( RColor  & _565_R_MASK ) | ( GColor  & _565_G_MASK ) | ( BColor  & _565_B_MASK ) );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    
    //=============================================================================
    // NAME:  VMC_Light_555_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X00007C00;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000001F;
    //=============================================================================
    inline void VMC_Light_555_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        word_t RColor = 0, GColor = 0, BColor = 0;
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                //*(word_t*)dest = (*(word_t *)src);
                __pfmemcpy( dest, src, 2 );
            }
            else if( *alpha != 0X00 )
            {
                RColor = ( ( ( ( ( *( word_t* )src )  &_555_R_MASK ) ) + ( ( ( *( word_t* )dest )  &_555_R_MASK ) ) ) >> 8 );
                GColor = ( ( ( ( ( *( word_t* )src )  &_555_G_MASK ) ) + ( ( ( *( word_t* )dest )  &_555_G_MASK ) ) ) >> 8 );
                BColor = ( ( ( ( ( *( word_t* )src )  &_555_B_MASK ) ) + ( ( ( *( word_t* )dest )  &_555_B_MASK ) ) ) >> 8 );
                if( RColor > 0XFF ) { RColor = 0XFF; }
                if( GColor > 0XFF ) { GColor = 0XFF; }
                if( BColor > 0XFF ) { BColor = 0XFF; }
                *( word_t* )dest = ( word_t )( ( RColor  & _555_R_MASK ) | ( GColor  & _555_G_MASK ) | ( BColor  & _555_B_MASK ) );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    //=============================================================================
    // NAME:  VMC_Light_556_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X0000F800;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000003F;
    //=============================================================================
    inline void VMC_Light_556_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        word_t RColor = 0, GColor = 0, BColor = 0;
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                //*(word_t*)dest = (*(word_t *)src);
                __pfmemcpy( dest, src, 2 );
            }
            else if( *alpha != 0X00 )
            {
                RColor = ( ( ( ( ( *( word_t* )src )  &_556_R_MASK ) ) + ( ( ( *( word_t* )dest )  &_556_R_MASK ) ) ) >> 8 );
                GColor = ( ( ( ( ( *( word_t* )src )  &_556_G_MASK ) ) + ( ( ( *( word_t* )dest )  &_556_G_MASK ) ) ) >> 8 );
                BColor = ( ( ( ( ( *( word_t* )src )  &_556_B_MASK ) ) + ( ( ( *( word_t* )dest )  &_556_B_MASK ) ) ) >> 8 );
                if( RColor > 0XFF ) { RColor = 0XFF; }
                if( GColor > 0XFF ) { GColor = 0XFF; }
                if( BColor > 0XFF ) { BColor = 0XFF; }
                *( word_t* )dest = ( word_t )( ( RColor  & _556_R_MASK ) | ( GColor  & _556_G_MASK ) | ( BColor  & _556_B_MASK ) );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    
    //=============================================================================
    // NAME:  VMC_Light_655_Blend
    // DESC:  alpha memcpy for 16 bits pixel
    //          _R_MASK = 0X0000FC00;
    //          _G_MASK = 0X000003E0;
    //          _B_MASK = 0X0000001F;
    //=============================================================================
    inline void VMC_Light_655_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        word_t RColor = 0, GColor = 0, BColor = 0;
        count = count >> 1;
        for( int i = 0; i < count; i++ )
        {
            if( *alpha == 0XFF )
            {
                //*(word_t*)dest = (*(word_t *)src);
                __pfmemcpy( dest, src, 2 );
            }
            else if( *alpha != 0X00 )
            {
                RColor = ( ( ( ( ( *( word_t* )src )  &_655_R_MASK ) ) + ( ( ( *( word_t* )dest )  &_655_R_MASK ) ) ) >> 8 );
                GColor = ( ( ( ( ( *( word_t* )src )  &_655_G_MASK ) ) + ( ( ( *( word_t* )dest )  &_655_G_MASK ) ) ) >> 8 );
                BColor = ( ( ( ( ( *( word_t* )src )  &_655_B_MASK ) ) + ( ( ( *( word_t* )dest )  &_655_B_MASK ) ) ) >> 8 );
                if( RColor > 0XFF ) { RColor = 0XFF; }
                if( GColor > 0XFF ) { GColor = 0XFF; }
                if( BColor > 0XFF ) { BColor = 0XFF; }
                DWORD n_a = ( _FULL_ALPHA - ( *alpha ) );
                *( word_t* )dest = ( word_t )( ( RColor  & _655_R_MASK ) | ( GColor  & _655_G_MASK ) | ( BColor  & _655_B_MASK ) );
            }
            alpha = alpha + 1;
            dest  = dest  + 2;
            src   = src   + 2;
        }
    }
    
    
    // 32bit
    //=============================================================================
    // NAME:  VMC_Alpha_32b_Blend
    // DESC:  alpha memcpy for alpha map and 32 bits BGRA pixel (not yet)
    //          (s * alpha) + (d * (255-alpha)) >> 8
    //=============================================================================
    inline void VMC_Alpha_32b_Blend( ubyte_t* dest, const ubyte_t* alpha, const ubyte_t* src, int count )
    {
        count = count >> 2;
        for( int i = 0; i < count; i++ )
        {
            if( ( *alpha ) == 0XFF )
            {
                ( *( dword_t* )dest ) = ( *( dword_t* )src );
            }
            else if( ( *alpha ) != 0X00 )
            {
                dword_t n_a = ( _FULL_ALPHA - ( *alpha ) );
                ( *( ubyte_t* )dest )         = ( ( ( *src )         * ( *alpha ) ) + ( ( *dest )         * n_a ) ) >> 8;
                ( *( ubyte_t* )( dest + 1 ) ) = ( ( ( *( src + 1 ) ) * ( *alpha ) ) + ( ( *( dest + 1 ) ) * n_a ) ) >> 8;
                ( *( ubyte_t* )( dest + 2 ) ) = ( ( ( *( src + 2 ) ) * ( *alpha ) ) + ( ( *( dest + 2 ) ) * n_a ) ) >> 8;
                ( *( ubyte_t* )( dest + 3 ) ) = __max( ( *( ubyte_t* )( dest + 3 ) ), ( *alpha ) );
            }
            alpha = alpha + 1;
            dest  = dest  + 4;                              //將目的指標加上要跳過的點數，再指向下一點(拷貝的第一點)
            src   = src   + 4;                              //將目的指標加上要跳過的點數，再指向下一點(下一個訊息點)
        }
    }
    
    //=============================================================================
    // NAME:  VMC_ColorKey_32b_Blend
    // DESC:
    //=============================================================================
    inline void VMC_ColorKey_32b_Blend( ubyte_t* dest, int colorkey, const ubyte_t* src, int count )
    {
        int col = 0;
        while( col < count )
        {
            if( colorkey != *( DWORD* )src )
            { *( DWORD* )dest = *( DWORD* )src; }
            //__pfmemcpy(dest, src, 4);
            src  = src  + 4;
            dest = dest + 4;
            col  = col  + 4;
        }
    }
    
    //=============================================================================
    // NAME:  VMC_Light_32b_Blend
    // DESC:
    //=============================================================================
    inline void VMC_Light_32b_Blend( ubyte_t* dest, const ubyte_t* src, int count )
    {
        count = count >> 2;
        for( int i = 0; i < count; i++ )
        {
            word_t pcolor = 0;
            pcolor = ( ( ( *src )   ) + ( ( *dest )   ) ) >> 8;
            if( pcolor > 0XFF )
            { pcolor = 0XFF; }
            ( *( ubyte_t* )dest ) = ( ubyte_t )LOWORD( pcolor );
            pcolor = ( ( ( *( src + 1 ) ) ) + ( ( *( dest + 1 ) ) ) ) >> 8;
            if( pcolor > 0XFF )
            { pcolor = 0XFF; }
            ( *( ubyte_t* )( dest + 1 ) ) = ( ubyte_t )LOWORD( pcolor );
            pcolor =  ( ( ( *( src + 2 ) ) ) + ( ( *( dest + 2 ) ) ) ) >> 8;
            if( pcolor > 0XFF )
            { pcolor = 0XFF; }
            ( *( ubyte_t* )( dest + 2 ) ) = ( ubyte_t )LOWORD( pcolor );
            ( *( ubyte_t* )( dest + 3 ) ) = __max( ( *( ubyte_t* )( dest + 3 ) ), ( *( ubyte_t* )( src + 3 ) ) );
            dest  = dest  + 4;                              //將目的指標加上要跳過的點數，再指向下一點(拷貝的第一點)
            src   = src   + 4;                              //將目的指標加上要跳過的點數，再指向下一點(下一個訊息點)
        }
    }
    
    
    inline void VMC_Alpha_combine( ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len )
    {
        for( int i = 0; i < len; i++ )
        {
            dest[i] = __max( srca0[i], srca1[i] );
        }
    }
}


#endif
