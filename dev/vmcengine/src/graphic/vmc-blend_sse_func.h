#ifndef _VMCBLENDSSEFUNC_H
#define _VMCBLENDSSEFUNC_H
#if defined(_X86_)

#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <math.h>
#include <pf-def.h>


#include <xmmintrin.h>
#include <emmintrin.h>

#include "vmc-blend_mmx_func.h"

namespace LGB
{

    //////////////////////////////////////////////////////////////////////////
    // SSE

    static __m128i  _SSE_16B_EMPTY_ALPHA  = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};
    static __m128i  _SSE_16B_FULL_ALPHA   = {0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00};
    static __m128i  _SSE_16B_FULL_MASK    = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};

    static __m128i  _SSE_16B_FULL_COPY    = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};


    //=============================================================================
    // NAME:  VMC_SSE_Alpha_565_Blend
    // DESC:  alpha memcpy for 16 bits 565 pixel use MMX
    //=============================================================================
    static __m128i _SSE_565_R_MASK       = {0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8, 0X00, 0XF8};
    static __m128i _SSE_565_G_MASK       = {0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07, 0XE0, 0X07};
    static __m128i _SSE_565_B_MASK       = {0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00};

    //static qword_t _MMX_565_R_MASK = 0XF800F800F800F800LL;
    //static qword_t _MMX_565_G_MASK = 0X07E007E007E007E0LL;
    //static qword_t _MMX_565_B_MASK = 0X001F001F001F001FLL;

    inline void VMC_SSE_Alpha_565_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        /*
        __m128i test1281    = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
        ubyte_t*  ptest1281    = (ubyte_t*)(&test1281);

        __m128i test1282    = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};
        ubyte_t*  ptest1282    = (ubyte_t*)(&test1282);

        _asm
        {
            MOV          esi, ptest1281
            MOV          edi, ptest1282
            PXOR         xxmm7, xxmm7                     //clear xxmm7 
            MOVDQU       xxmm0, [esi]
            MOVDQU       xxmm6, _SSE_565_G_MASK
            //PSUBW        xxmm6, xxmm7
            MOVDQU       [edi],xxmm6
        }
        */

        div_t c_mod_16 = div(count, 16);

        // 一次做8個點
        for(int i = 0; i < c_mod_16.quot; i++)
        {
            // 沒有Alpha直接複製
            if((*(qword_t*)alpha) == 0XFFFFFFFFFFFFFFFF)
            {
                //__pfmemcpy(dest, src, 16);
                _asm
                {
                    MOV     esi, src
                    MOV     edi, dest
                    MOVDQU  xmm0, [esi]                         
                    MOVDQU  [edi],xmm0                          
                }
            }
            // 不是全透明
            else if((*(qword_t*)alpha) != 0X0000000000000000)
            {
                // 這邊其實已經也會順道混和 alpha 所以不用另外算
                _asm
                {
                    MOV          esi, src
                    MOV          edi, dest
                    MOV          eax, alpha
                    // 一次搬8個點
                    MOVLPS       xmm6, [eax]                   //alpha => xxmm6     | 00 00 00 00 | 00 00 00 00 | a7 a6 a5 a4 | a3 a2 a1 a0 |
                    PXOR         xmm7, xmm7                    //clear xmm7
                    PUNPCKLBW    xmm6, xmm7                     
                    MOVDQU       xmm7, _SSE_16B_FULL_ALPHA
                    PSUBW        xmm7, xmm6                    //xmm7 255-alpha     | 00 a7 00 a6 | 00 a5 00 a4 | 00 a3 00 a2 | 00 a1 00 a0 |

                    MOVDQU       xmm0, [esi]                   
                    MOVDQU       xmm1, xmm0
                    MOVDQU       xmm2, xmm0                     

                    MOVDQU       xmm3, [edi]                   
                    MOVDQU       xmm4, xmm3                     
                    MOVDQU       xmm5, xmm3                     

                    ///////////////////////////////////////////////////////////////////
                    //source  &r * alpha
                    PAND         xmm0, _SSE_565_R_MASK    
                    PSRLW        xmm0, 11                 
                    PAND         xmm1, _SSE_565_G_MASK    
                    PSRLW        xmm1, 5                  
                    PAND         xmm2, _SSE_565_B_MASK    

                    PMULLW       xmm0, xmm6                
                    PMULLW       xmm1, xmm6                
                    PMULLW       xmm2, xmm6                

                    // DourceColor * (MaxAlpha - Alpha)
                    PAND         xmm3, _SSE_565_R_MASK    
                    PSRLW        xmm3, 11                 
                    PAND         xmm4, _SSE_565_G_MASK    
                    PSRLW        xmm4, 5                  
                    PAND         xmm5, _SSE_565_B_MASK    

                    PMULLW       xmm3, xmm7                
                    PMULLW       xmm4, xmm7                
                    PMULLW       xmm5, xmm7                

                    // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))
                    PADDW        xmm0, xmm3                
                    PADDW        xmm1, xmm4                
                    PADDW        xmm2, xmm5                

                    // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))/ MaxAlpha
                    PSRLW        xmm0, 8                  
                    PSRLW        xmm1, 8                  
                    PSRLW        xmm2, 8                  

                    PSLLW        xmm0, 11                 
                    PSLLW        xmm1, 5                  

                    POR             xmm0, xmm1                
                    POR             xmm0, xmm2                
                    ///////////////////////////////////////////////////////////////////////////////////      

                    MOVDQU       [edi],xmm0
                };
            }

            src+=16;
            dest+=16;
            alpha+=8;
        }

        // 一次處理 16 byte
        if(c_mod_16.rem > 0)
        {
            VMC_MMX_Alpha_565_Blend(dest, alpha, src, c_mod_16.rem);
        }

        /*
        int C_MOD_7 = count  &7 ;
        if(C_MOD_7 != 0)
        {
            int doready = count- C_MOD_7;
            VMC_Alpha_565_Blend(dest + doready, alpha + (doready >> 1), src + doready, C_MOD_7);
        }
        */
    }

    //=============================================================================
    // NAME:  VMC_MMX_Alpha_555_Blend
    // DESC:  alpha memcpy for 16 bits 555 pixel use MMX
    //=============================================================================
    static __m128i _SSE_555_R_MASK = {0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C, 0X00, 0X7C};
    static __m128i _SSE_555_G_MASK = {0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03, 0XE0, 0X03};
    static __m128i _SSE_555_B_MASK = {0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00, 0X1F, 0X00};

    //static qword_t _MMX_555_R_MASK = 0X7C007C007C007C00LL;
    //static qword_t _MMX_555_G_MASK = 0X03E003E003E003E0LL;
    //static qword_t _MMX_555_B_MASK = 0X001F001F001F001FLL;

    inline void VMC_SSE_Alpha_555_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        div_t c_mod_16 = div(count, 16);

        // 一次做8個點
        for(int i = 0; i < c_mod_16.quot; i++)
        {
            // 沒有Alpha直接複製
            if((*(qword_t*)alpha) == 0XFFFFFFFFFFFFFFFF)
            {
                //__pfmemcpy(dest, src, 16);
                _asm
                {
                    MOV     esi, src
                    MOV     edi, dest
                    MOVDQU  xmm0, [esi]                         
                    MOVDQU  [edi],xmm0                          
                }
            }
            // 不是全透明
            else if((*(qword_t*)alpha) != 0X0000000000000000)
            {
                // 這邊其實已經也會順道混和 alpha 所以不用另外算
                _asm
                {
                    MOV          esi, src                           
                    MOV          edi, dest                          
                    MOV          eax, alpha  


                    MOVLPS       xmm6, [eax]                  //alpha => xxmm6     | 00 00 00 00 | 00 00 00 00 | a7 a6 a5 a4 | a3 a2 a1 a0 |
                    PXOR         xmm7, xmm7                     
                    PUNPCKLBW    xmm6, xmm7
                    MOVDQU       xmm7, _SSE_16B_FULL_ALPHA          
                    PSUBW        xmm7, xmm6                   //xmm7 255-alpha     | 00 a7 00 a6 | 00 a5 00 a4 | 00 a3 00 a2 | 00 a1 00 a0 |

                    MOVDQU       xmm0, [esi]                   
                    MOVDQU       xmm1, xmm0                     
                    MOVDQU       xmm2, xmm0                     

                    MOVDQU       xmm3, [edi]                   
                    MOVDQU       xmm4, xmm3                     
                    MOVDQU       xmm5, xmm3                     

                    ///////////////////////////////////////////////////////////////////
                    //source  &r * alpha
                    PAND         xmm0, _SSE_555_R_MASK    
                    PSRLW        xmm0, 10                 
                    PAND         xmm1, _SSE_555_G_MASK    
                    PSRLW        xmm1, 5                  
                    PAND         xmm2, _SSE_555_B_MASK    

                    PMULLW       xmm0, xmm6                
                    PMULLW       xmm1, xmm6                
                    PMULLW       xmm2, xmm6                

                    // DourceColor * (MaxAlpha - Alpha)
                    PAND         xmm3, _SSE_555_R_MASK    
                    PSRLW        xmm3, 10                 
                    PAND         xmm4, _SSE_555_G_MASK    
                    PSRLW        xmm4, 5                  
                    PAND         xmm5, _SSE_555_B_MASK    

                    PMULLW       xmm3, xmm7                
                    PMULLW       xmm4, xmm7                
                    PMULLW       xmm5, xmm7                

                    // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))
                    PADDW        xmm0, xmm3                
                    PADDW        xmm1, xmm4                
                    PADDW        xmm2, xmm5                

                    // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))/ MaxAlpha
                    PSRLW        xmm0, 8                  
                    PSRLW        xmm1, 8                  
                    PSRLW        xmm2, 8                  

                    PSLLW        xmm0, 10                 
                    PSLLW        xmm1, 5                  

                    POR             xmm0, xmm1                
                    POR             xmm0, xmm2                
                    ///////////////////////////////////////////////////////////////////////////////////      

                    MOVDQU       [edi],xmm0  
                };
            }

            src+=16;
            dest+=16;
            alpha+=8;
        }

        // 一次處理 16 byte
        if(c_mod_16.rem > 0)
        {
            VMC_MMX_Alpha_555_Blend(dest, alpha, src, c_mod_16.rem);
        }
    }


    //=============================================================================
    // NAME:  VMC_MMX_Alpha_556_Blend
    // DESC:  alpha memcpy for 16 bits 556 pixel use MMX
    //=============================================================================
    static __m128 _SSE_556_R_MASK = {0X7C007C00, 0X7C007C00, 0X7C007C00, 0X7C007C00};
    static __m128 _SSE_556_G_MASK = {0X03E003E0, 0X03E003E0, 0X03E003E0, 0X03E003E0};
    static __m128 _SSE_556_B_MASK = {0X001F001F, 0X001F001F, 0X001F001F, 0X001F001F};

    inline void VMC_SSE_Alpha_556_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {


    }

    //=============================================================================
    // NAME:  VMC_MMX_Alpha_655_Blend
    // DESC:  alpha memcpy for 16 bits 655 pixel use MMX
    //=============================================================================
    static __m128 _SSE_655_R_MASK = {0XF800F800, 0XF800F800, 0XF800F800, 0XF800F800};
    static __m128 _SSE_655_G_MASK = {0X03E003E0, 0X03E003E0, 0X03E003E0, 0X03E003E0};
    static __m128 _SSE_655_B_MASK = {0X003F003F, 0X003F003F, 0X003F003F, 0X003F003F}; 

    inline void VMC_SSE_Alpha_655_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {


    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    inline void VMC_SSE_ColorKey_16b_Blend(ubyte_t* dest, int colorkey, const ubyte_t* src, int count)
    {
        div_t c_mod_16 = div(count, 16);
        __m128 keymask;
        //DWORD keymask[4] = {(word_t)colorkey, (word_t)colorkey, (word_t)colorkey, (word_t)colorkey};
        keymask.m128_u16[0] = (word_t)colorkey;
        keymask.m128_u16[1] = (word_t)colorkey;
        keymask.m128_u16[2] = (word_t)colorkey;
        keymask.m128_u16[3] = (word_t)colorkey;
        keymask.m128_u16[4] = (word_t)colorkey;
        keymask.m128_u16[5] = (word_t)colorkey;
        keymask.m128_u16[6] = (word_t)colorkey;
        keymask.m128_u16[7] = (word_t)colorkey;

        for(int i = 0; i < c_mod_16.quot; i++)
        {
            _asm
            {
                MOV          esi,   src
                MOV          edi,   dest

                MOVDQU       xmm0,   [esi] 
                MOVDQU       xmm1,   [edi]  
                MOVDQU       xmm6,   xmm0                // prepare xmm6 to be mask

                PCMPEQW      xmm6,   keymask             // xmm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
                MOVDQU       xmm7,   _SSE_16B_FULL_MASK  // if xmm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF

                PAND         xmm1,   xmm6                //       | K  | S1 |  &| 0xFFFF | K |  =>  | 0  | S1 |
                PANDN        xmm6,   xmm7                // if xmm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF
                PAND         xmm0,   xmm6                //       | D1 | D2 |  &| 0xFFFF | K |  =>  | D1 | 0  |

                POR          xmm0,   xmm1

                MOVDQU       [edi], xmm0
            }

            src+=16;
            dest+=16;
        }


        // 不滿16的 就直接copy
        if(c_mod_16.rem > 0)
        {
            VMC_MMX_ColorKey_16b_Blend(dest, colorkey, src, c_mod_16.rem);
        }
    }

    static __m128i _SSE_32B_FULL_ALPHA  = {0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00};
    static __m128i _SSE_32B_FULL_CALPHA = {0XFF, 0XFF, 0XFF, 0X00, 0XFF, 0XFF, 0XFF, 0X00, 0XFF, 0XFF, 0XFF, 0X00, 0XFF, 0XFF, 0XFF, 0X00};
    static __m128i _SSE_RGBA_GR_MASK    = {0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF}; // 低到高 所以要反過來
    static __m128i _SSE_RGBA_BA_MASK    = {0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00, 0XFF, 0X00};
    static __m128i _SSE_32B_FULL_MASK   = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
    static __m128i _SSE_32B_EMPTY_MASK  = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};
    //static __m128i _SSE_COMP32BPHA      = {0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

    //=============================================================================
    // NAME:  VMC_SSE_Alpha_32b_Blend
    // DESC:  alpha memcpy for alpha map and 32 bits BGRA pixel (not yet)
    //=============================================================================

    inline void VMC_SSE_Alpha_32b_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        /*
        //宣告SSE變數
        __m128i pixel1, pixel2;
        //將混合因子值設到一個128-bit變數裡的32-bit區段
        __m128i blend_mu1 = _xmm_set_ps1(B);
        __m128i blend_m1 = _xmm_set_ps1(1.0-B);
        // 將pixel1設成包含四個浮點變數。首先載入整數值，然後轉成單精度浮點數值。Pixel2也這麼做
        for (int i = 0; i <count; i++)
        {
            pixel1 = _xmm_cvtepi32_ps(_xmm_set_epi32(0.0, r1[i], g1[i], b1[i]));
            pixel2 = _xmm_cvtepi32_ps(_xmm_set_epi32(0.0, r2[i], g2[i], b2[i]));
            //使用intrinsics來執行乘法和加法，如同在實例2-1中看到的
            pixel1 = _xmm_add_ps(_xmm_mul_ps(pixel1, blend_mu1), _xmm_mul_ps(pixel2, blend_m1));
        }
        */
        /*
        __m128i test1281    = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
        ubyte_t*  ptest1281    = (ubyte_t*)(&test1281);

        __m128i test1282    = {0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00};
        ubyte_t*  ptest1282    = (ubyte_t*)(&test1282);

        _asm
        {
            MOV          esi, ptest1281
            MOV          edi, ptest1282
            PXOR         xmm7, xmm7                     //clear xmm7 
            MOVDQU       xmm0, [esi]
            MOVDQU       xmm6, _SSE_32B_FULL_ALPHA
            PSUBW        xmm6, xmm7
            MOVDQU       [edi],xmm6
        }
        */

       
        div_t c_mod_16 = div(count, 16);

        // 一次做四個點
        for(int i = 0; i < c_mod_16.quot; i++)
        {
            // 沒有Alpha直接複製
            if((*(DWORD*)alpha) == 0XFFFFFFFF)
            {
                //__pfmemcpy(dest, src, 16);
                _asm
                {
                    MOV          esi, src
                    MOV          edi, dest
                    MOVDQU       xmm0, [esi]
                    MOVDQU       [edi],xmm0
                }
            }
            // 不是全透明
            else if((*(DWORD*)alpha) != 0X00000000)
            {
                // 這邊其實已經也會順道混和 alpha 所以不用另外算
                _asm
                {
                    MOV          esi, src
                    MOV          eax, alpha
                    MOV          edi, dest
                                        
                    PXOR         xmm7, xmm7                     //clear xmm7 

                    MOVSS        xmm6, [eax]                    //alpha => xmm6     | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | a3 a2 a1 a0 |
                    //PAND         xmm6, _SSE_COMP32BPHA        //xmm6 as alpha     | 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | a3 a2 a1 a0 |
                    //MOVDQU       ret0, xmm6

                    PUNPCKLBW    xmm6, xmm7                     //xmm6              | 00 00 00 00 | 00 00 00 00 | 00 a3 00 a2 | 00 a1 00 a0 |
                    MOVDQU       xmm5, xmm6                     //xmm5              | 00 00 00 00 | 00 00 00 00 | 00 a3 00 a2 | 00 a1 00 a0 |

                    PSLLQ        xmm6, 8                        //xmm6              | 00 00 00 00 | 00 00 00 00 | a3 00 a2 00 | a1 00 a0 00 |
                    PADDB        xmm6, xmm5                     //xmm6              | 00 00 00 00 | 00 00 00 00 | a3 a3 a2 a2 | a1 a1 a0 a0 |
                    PUNPCKLBW    xmm6, xmm7                     //xmm6              | 00 a3 00 a3 | 00 a2 00 a2 | 00 a1 00 a1 | 00 a0 00 a0 |
                    MOVDQU       xmm7, _SSE_32B_FULL_ALPHA
                    PSUBW        xmm7, xmm6                     //xmm7 255-alpha    | 00 A3 00 A3 | 00 A2 00 A2 | 00 A1 00 A1 | 00 A0 00 A0 |
                    //MOVDQU       ret2, xmm7

                    //MOVDQU       xmm6, _SSE_32B_FULL_ALPHA
                    //PSUBW        xmm6, xmm7
                    
                    MOVDQU       xmm0, [esi]                   //xmm0 soruce       | a0 r0 g0 b0 | a1 r1 g1 b1 | a2 r2 g2 b2 | a3 r3 g3 b3 |
                    MOVDQU       xmm1, xmm0

                    MOVDQU       xmm3, [edi]                   //xmm0 destination  | a0 r0 g0 b0 | a1 r1 g1 b1 | a2 r2 g2 b2 | a3 r3 g3 b3 |
                    MOVDQU       xmm4, xmm3       
                    

                    PAND         xmm0, _SSE_RGBA_GR_MASK        //xmm0             | a0 00 g0 00 | a1 00 g1 00 | a2 00 g2 00 | a3 00 g3 00 |
                    PSRLW        xmm0, 8                        //xmm0             | 00 a0 00 g0 | 00 a1 00 g1 | 00 a2 00 g2 | 00 a3 00 g3 |
                    PAND         xmm1, _SSE_RGBA_BA_MASK        //xmm1             | 00 r0 00 b0 | 00 r1 00 b1 | 00 r2 00 b2 | 00 r3 00 b3 |      
                    PMULLW       xmm0, xmm6
                    PMULLW       xmm1, xmm6

                    //MOVDQU       ret6, xmm0
                    //MOVDQU       ret7, xmm1

                    PAND         xmm3, _SSE_RGBA_GR_MASK
                    PSRLW        xmm3, 8
                    PAND         xmm4, _SSE_RGBA_BA_MASK
                    PMULLW       xmm3, xmm7
                    PMULLW       xmm4, xmm7

                    PADDD        xmm0, xmm3
                    PADDD        xmm1, xmm4

                    PSRLW        xmm0, 8
                    PSRLW        xmm1, 8

                    PSLLW        xmm0, 8                       //xmm0              | a0 00 g0 00 | a1 00 g1 00 | a2 00 g2 00 | a3 00 g3 00 |

                    POR          xmm0, xmm1
                    //
                    //PUNPCKLBW    xmm6, xmm7
                    // 準備最後要使用的alpha 因為傳進來的是已經比較過的 所以 直接複製即可
                    //PXOR         xmm7, xmm7                     //clear xmm7 
                    //PUNPCKLBW    xmm5, xmm7                     //xmm7 最後要存進去的| 00 00 00 a3 | 00 00 00 a2 | 00 00 00 a1 | 00 00 00 a0 |
                    //POR          xmm0, xmm5

                    MOVDQU       [edi],xmm0
                    //EMMS
                };
            }

            src+=16;
            dest+=16;
            alpha+=4;
        }


        // 不滿16的 就直接copy
        if(c_mod_16.rem > 0)
        {
            VMC_MMX_Alpha_32b_Blend(dest, alpha, src, c_mod_16.rem);
        }
    }


    //=============================================================================
    // NAME:  VMC_SSE_ColorKey_32b_Blend
    // DESC:  
    //=============================================================================
    inline void VMC_SSE_ColorKey_32b_Blend(ubyte_t* dest, int colorkey, const ubyte_t* src, int count)
    {
        div_t c_mod_16 = div(count, 16);

        /*
        __m128i   keymask;
        keymask.m128i_u32[0] = colorkey;
        keymask.m128i_u32[1] = colorkey;
        keymask.m128i_u32[2] = colorkey;
        keymask.m128i_u32[3] = colorkey;
        */

        /*
        _asm
        {
            MOV          esi,   src
            MOV          edi,   dest

            MOVDQU       xmm0,   [esi] 
            MOVDQU       xmm1,   [edi]  
            MOVDQU       xmm6,   xmm0           // prepare mm6 to be mask

            
            MOVDQU       xmm3,   keymask
            //MOVDQU       ssi,    xmm6
            PCMPEQW      xmm6,   xmm3       // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
        }
        */

        for(int i = 0; i < c_mod_16.quot; i++)
        {
            //qword_t    mmm0 = 0;
            //qword_t    mmm1 = 0;

            _asm
            {
                MOV          esi,   src
                MOV          edi,   dest

                MOVDQU       xmm0,   [esi] 
                MOVDQU       xmm1,   [edi]  
                MOVDQU       xmm6,   xmm0                // prepare mm6 to be mask


                MOVD         xmm5,   colorkey
                SHUFPS       xmm5,   xmm5, 0x00
                PCMPEQD      xmm6,   xmm5                // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)

                //PCMPEQD      xmm6,   keymask           // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
                MOVDQU       xmm7,   _SSE_32B_FULL_MASK  // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF

                PAND         xmm1,   xmm6                //       | K  | S1 |  &| 0xFFFF | K |  =>  | 0  | S1 |
                PANDN        xmm6,   xmm7                // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF
                PAND         xmm0,   xmm6                //       | D1 | D2 |  &| 0xFFFF | K |  =>  | D1 | 0  |

                POR          xmm0,   xmm1

                MOVDQU       [edi],  xmm0
            }

            src+=16;
            dest+=16;
        }

        // 不滿16的 就直接copy
        if(c_mod_16.rem > 0)
        {
            VMC_MMX_ColorKey_32b_Blend(dest, colorkey, src, c_mod_16.rem);
        }
    }


    inline void VMC_SSE_Alpha_combine(ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len)
    {
        for(int i = 0; i < len; i++)
        {
            dest[i] = __max(srca0[i], srca1[i]);
        }
    }
}

#endif // _X86_
#endif
