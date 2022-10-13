#ifndef _VMCBLENDMMXFUNC_H
#define _VMCBLENDMMXFUNC_H

#if defined(_X86_)

#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <math.h>
#include <pf-def.h>

#include <xmmintrin.h>
#include <emmintrin.h>


#include "vmc-blend_func.h"

namespace LGB
{

    //////////////////////////////////////////////////////////////////////////
    // MMX

    static long long _MMX_16B_EMPTY_ALPHA  = 0X0000000000000000LL;
    static long long _MMX_16B_FULL_ALPHA   = 0X00FF00FF00FF00FFLL;
    static long long _MMX_16B_FULL_MASK    = 0XFFFFFFFFFFFFFFFFLL;

    //=============================================================================
    // NAME:  VMC_MMX_Alpha_565_Blend
    // DESC:  alpha memcpy for 16 bits 565 pixel use MMX
    //=============================================================================
    static qword_t _MMX_565_R_MASK = 0XF800F800F800F800LL;
    static qword_t _MMX_565_G_MASK = 0X07E007E007E007E0LL;
    static qword_t _MMX_565_B_MASK = 0X001F001F001F001FLL;
    inline void VMC_MMX_Alpha_565_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        _asm
        {
            MOV    esi, src                           
            MOV    edi, dest                          
            MOV    eax, alpha                         
            MOV    edx, count                         
            CMP    edx, 9                             
            JL     END_MMX                            
            MOV    ecx, edx                           
            SHR    ecx, 3                             

          //ALIGN 8        // 在macos上不會過 win32上拿掉也好像沒甚麼影響 那我那時候為何要加他?                                
LP0:                                        
            MOV    ebx, DWORD PTR [eax]

            CMP    ebx, 0X00000000                    
            JE     MEM_JUMP
            
            CMP    ebx, 0XFFFFFFFF                    
            JNE    ALPHA_COPY                         

            MOVQ   mm0, [esi]                         
            MOVQ   [edi],mm0                          
            JMP    MEM_JUMP                           

ALPHA_COPY:

          //ALIGN 8                                     
            MOVD         mm6, [eax]                   //mm6 as alpha  | a  | a  | a  | a  |
            PXOR         mm7, mm7                     
            PUNPCKLBW    mm6, mm7                     
            MOVQ         mm7, _MMX_16B_FULL_ALPHA          
            PSUBW        mm7, mm6                     //mm7 as nalpha | na | na | na | na |

            MOVQ         mm0, [esi]                   
            MOVQ         mm1, mm0                     
            MOVQ         mm2, mm0                     

            MOVQ         mm3, [edi]                   
            MOVQ         mm4, mm3                     
            MOVQ         mm5, mm3                     

        ///////////////////////////////////////////////////////////////////
        //source  &r * alpha
                 PAND         mm0, _MMX_565_R_MASK    
                 PSRLW        mm0, 11                 
                 PAND         mm1, _MMX_565_G_MASK    
                 PSRLW        mm1, 5                  
                 PAND         mm2, _MMX_565_B_MASK    

                 PMULLW       mm0, mm6                
                 PMULLW       mm1, mm6                
                 PMULLW       mm2, mm6                

        // DourceColor * (MaxAlpha - Alpha)
                 PAND         mm3, _MMX_565_R_MASK    
                 PSRLW        mm3, 11                 
                 PAND         mm4, _MMX_565_G_MASK    
                 PSRLW        mm4, 5                  
                 PAND         mm5, _MMX_565_B_MASK    

                 PMULLW       mm3, mm7                
                 PMULLW       mm4, mm7                
                 PMULLW       mm5, mm7                

        // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))
                 PADDW        mm0, mm3                
                 PADDW        mm1, mm4                
                 PADDW        mm2, mm5                

        // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))/ MaxAlpha
                 PSRLW        mm0, 8                  
                 PSRLW        mm1, 8                  
                 PSRLW        mm2, 8                  

                 PSLLW        mm0, 11                 
                 PSLLW        mm1, 5                  

                 POR             mm0, mm1                
                 POR             mm0, mm2                
        ///////////////////////////////////////////////////////////////////////////////////      
                
                  MOVQ   [edi],mm0                     

          MEM_JUMP:                                   
            ADD    esi, 8                             
            ADD    edi, 8                             
            ADD    eax, 4                             
            DEC    ecx                                
            JNZ    LP0                                
            EMMS                                      
          END_MMX:                                    
        };

        int C_MOD_7 = count  &7 ;
        if(C_MOD_7 != 0)
        {
            int doready = count- C_MOD_7;
            VMC_Alpha_565_Blend(dest + doready, alpha + (doready >> 1), src + doready, C_MOD_7);
        }
    }


    //=============================================================================
    // NAME:  VMC_MMX_Alpha_555_Blend
    // DESC:  alpha memcpy for 16 bits 555 pixel use MMX
    //=============================================================================
    static qword_t _MMX_555_R_MASK = 0X7C007C007C007C00LL;
    static qword_t _MMX_555_G_MASK = 0X03E003E003E003E0LL;
    static qword_t _MMX_555_B_MASK = 0X001F001F001F001FLL;

    inline void VMC_MMX_Alpha_555_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        _asm
        {
            MOV    esi, src                           
            MOV    edi, dest                          
            MOV    eax, alpha                         
            MOV    edx, count                         
            CMP    edx, 9                             
            JL     END_MMX                            
            MOV    ecx, edx                           
            SHR    ecx, 3                             

          //ALIGN 8        // 在macos上不會過 win32上拿掉也好像沒甚麼影響 那我那時候為何要加他?                                
LP0:                                        
            MOV    ebx, DWORD PTR [eax]

            CMP    ebx, 0X00000000                    
            JE     MEM_JUMP                           

            CMP    ebx, 0XFFFFFFFF                    
            JNE    ALPHA_COPY                         

            MOVQ   mm0, [esi]                         
            MOVQ   [edi],mm0                          
            JMP    MEM_JUMP                           

ALPHA_COPY:
          //ALIGN 8                                   
            MOVD         mm6, [eax]                    //mm6 as alpha  | a  | a  | a  | a  |
            PXOR         mm7, mm7                     
            PUNPCKLBW    mm6, mm7                     
            MOVQ         mm7, _MMX_16B_FULL_ALPHA          
            PSUBW        mm7, mm6                      //mm7 as nalpha | na | na | na | na |

            MOVQ         mm0, [esi]                   
            MOVQ         mm1, mm0                     
            MOVQ         mm2, mm0                     

            MOVQ         mm3, [edi]                   
            MOVQ         mm4, mm3                     
            MOVQ         mm5, mm3                     

        ///////////////////////////////////////////////////////////////////
        //source  &r * alpha
                    PAND         mm0, _MMX_555_R_MASK    
                    PSRLW        mm0, 10                 
                    PAND         mm1, _MMX_555_G_MASK    
                    PSRLW        mm1, 5                  
                    PAND         mm2, _MMX_555_B_MASK    

                    PMULLW       mm0, mm6                
                    PMULLW       mm1, mm6                
                    PMULLW       mm2, mm6                

        // DourceColor * (MaxAlpha - Alpha)
                    PAND         mm3, _MMX_555_R_MASK    
                    PSRLW        mm3, 10                 
                    PAND         mm4, _MMX_555_G_MASK    
                    PSRLW        mm4, 5                  
                    PAND         mm5, _MMX_555_B_MASK    

                    PMULLW       mm3, mm7                
                    PMULLW       mm4, mm7                
                    PMULLW       mm5, mm7                

        // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))
                    PADDW        mm0, mm3                
                    PADDW        mm1, mm4                
                    PADDW        mm2, mm5                

        // (SourceColor * Alpha) + (DourceColor * (MaxAlpha    - Alpha))/ MaxAlpha
                    PSRLW        mm0, 8                  
                    PSRLW        mm1, 8                  
                    PSRLW        mm2, 8                  

                    PSLLW        mm0, 10                 
                    PSLLW        mm1, 5                  

                    POR             mm0, mm1                
                    POR             mm0, mm2                
        ///////////////////////////////////////////////////////////////////////////////////      
                
            MOVQ   [edi],mm0                          

          MEM_JUMP:                                   
            ADD    esi, 8                             
            ADD    edi, 8                             
            ADD    eax, 4                             
            DEC    ecx                                
            JNZ    LP0                                
            EMMS                                      
          END_MMX:                                    
        };

        int C_MOD_7 = count  &7 ;
        if(C_MOD_7 != 0)
        {
            int doready = count- C_MOD_7;
            VMC_Alpha_555_Blend(dest + doready, alpha + (doready >> 1), src + doready, C_MOD_7);
        }
    }

    //=============================================================================
    // NAME:  VMC_MMX_Alpha_556_Blend
    // DESC:  alpha memcpy for 16 bits 556 pixel use MMX
    //=============================================================================
    static qword_t _MMX_556_R_MASK = 0X7C007C007C007C00LL;
    static qword_t _MMX_556_G_MASK = 0X03E003E003E003E0LL;
    static qword_t _MMX_556_B_MASK = 0X001F001F001F001FLL;

    inline void VMC_MMX_Alpha_556_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {


    }


    //=============================================================================
    // NAME:  VMC_MMX_Alpha_655_Blend
    // DESC:  alpha memcpy for 16 bits 655 pixel use MMX
    //=============================================================================
    static qword_t _MMX_655_R_MASK = 0XF800F800F800F800LL;
    static qword_t _MMX_655_G_MASK = 0X03E003E003E003E0LL;
    static qword_t _MMX_655_B_MASK = 0X003F003F003F003FLL;

    inline void VMC_MMX_Alpha_655_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {


    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    inline void VMC_MMX_ColorKey_16b_Blend(ubyte_t* dest, int colorkey, const ubyte_t* src, int count)
    {
        /*
        int col = 0;
        while(col < count)
        {
            if(colorkey != *(word_t *)src)
                *(word_t*)dest = *(word_t*)src;
            //__pfmemcpy(dest, src, 2);

            src  = src  + 2;
            dest = dest + 2;
            col  = col  + 2;
        }
        */
        /*

        qword_t ss1 = 0XFFFFFFFF00FFFFFFLL;
        qword_t ss2 = 0XFFFFFFFFFFFFFFFFLL;
        qword_t ss3 = 0;

        _asm
        {
            MOVQ         mm6,   ss1                // prepare mm6 to be mask
            PCMPEQW      mm6,   ss2            // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
            MOVQ         ss3,   mm6                // prepare mm6 to be mask

        }
        */

        div_t c_mod_8 = div(count, 8);
        DWORD keymask[4] = {(word_t)colorkey, (word_t)colorkey, (word_t)colorkey, (word_t)colorkey};

        for(int i = 0; i<c_mod_8.quot; i++)
        {
            _asm
            {
                MOV          esi,   src
                MOV          edi,   dest

                MOVQ         mm0,   [esi] 
                MOVQ         mm1,   [edi]  
                MOVQ         mm6,   mm0                // prepare mm6 to be mask

                PCMPEQW      mm6,   keymask            // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
                MOVQ         mm7,   _MMX_16B_FULL_MASK // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF

                PAND         mm1,   mm6                //       | K  | S1 |  &| 0xFFFF | K |  =>  | 0  | S1 |
                PANDN        mm6,   mm7                // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF
                PAND         mm0,   mm6                //       | D1 | D2 |  &| 0xFFFF | K |  =>  | D1 | 0  |

                POR          mm0,   mm1

                MOVQ         [edi], mm0
                EMMS
            }

            src+=8;
            dest+=8;
        }


        // 不滿16的 就直接copy
        if(c_mod_8.rem>0)
        {
            VMC_ColorKey_16b_Blend(dest, colorkey, src, c_mod_8.rem);
        }
    }


    static qword_t _MMX_32B_FULL_ALPHA  = 0X00FF00FF00FF00FFLL;
    //static qword_t _MMX_32B_FULL_CALPHA = 0X00FFFFFF00FFFFFFLL;
    static qword_t _MMX_RGBA_GR_MASK    = 0XFF00FF00FF00FF00LL;
    static qword_t _MMX_RGBA_BA_MASK    = 0X00FF00FF00FF00FFLL;
    static qword_t _MMX_COMP32BPHA      = 0X000000000000FFFFLL;
    static qword_t _MMX_32B_FULL_MASK   = 0XFFFFFFFFFFFFFFFFLL;
    static qword_t _MMX_32B_EMPTY_MASK  = 0X0000000000000000LL;

    //=============================================================================
    // NAME:  VMC_MMX_Alpha_32b_Blend
    // DESC:  alpha memcpy for alpha map 32 bits RGBA pixel use MMX  (not yet)
    //=============================================================================
    inline void VMC_MMX_Alpha_32b_Blend(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count)
    {
        div_t c_mod_8 = div(count, 8);

        for(int i = 0; i<c_mod_8.quot; i++)
        {
            if((*(word_t*)alpha)==0XFFFF)
            {
                //*(qword_t*)dest = (*(qword_t *)src);
                _asm
                {
                    MOV          esi, src
                    MOV          edi, dest
                    MOVQ         mm0, [esi]
                    MOVQ         [edi],mm0
                    EMMS
                }
            }
            else if((*(word_t*)alpha)!=0X0000)
            {
                // 這邊其實已經也會順道混和 alpha 所以不用另外算
                _asm
                {
                    MOV          esi, src
                    MOV          eax, alpha
                    MOV          edi, dest

                    PXOR         mm7, mm7
                    MOVD         mm6, [eax]                    //mm6 as alpha  | 00 | 00 | 00 | ba |
                    PAND         mm6, _MMX_COMP32BPHA          //mm6 as alpha  | 00 | 00 | 00 | ba |
                    PUNPCKLBW    mm6, mm7                      //mm6           | 00 | 00 | 0b | 0a |
                    MOVQ         mm5, mm6                      //mm5           | 00 | 00 | 0b | 0a |
                    PSLLD        mm6, 8                        //mm6           | 00 | 00 | b0 | a0 |
                    PADDB        mm6, mm5                      //mm6           | 00 | 00 | bb | aa |
                    PUNPCKLBW    mm6, mm7                      //mm6           | 0b | 0b | 0a | 0a |
                    MOVQ         mm7, _MMX_32B_FULL_ALPHA
                    PSUBW        mm7, mm6                      //mm7 as nalpha | 0B | 0B | 0A | 0A |

                    MOVQ         mm0, [esi]                    //mm0           |    s0   |    s1   |
                    MOVQ         mm1, mm0

                    MOVQ         mm3, [edi]                    //mm3           |    d0   |    d1   |
                    MOVQ         mm4, mm3

                    PAND         mm0, _MMX_RGBA_GR_MASK        //mm0           | G0 | R0 | G0 | R0 |
                    PSRLW        mm0, 8                        //mm0           | 0G | 0R | 0G | 0R |
                    PAND         mm1, _MMX_RGBA_BA_MASK        //mm1           | 0B | 0A | 0B | 0A |
                    PMULLW       mm0, mm6                      // G and R  * alpha
                    PMULLW       mm1, mm6                      // B and A  * alpha

                    PAND         mm3, _MMX_RGBA_GR_MASK
                    PSRLW        mm3, 8
                    PAND         mm4, _MMX_RGBA_BA_MASK
                    PMULLW       mm3, mm7                      // G and R  * (255-alpha)
                    PMULLW       mm4, mm7                      // B and A  * (255-alpha)

                    PADDD        mm0, mm3
                    PADDD        mm1, mm4

                    PSRLW        mm0, 8                         // shift for get high 8 byte of word    | a0 a1 | =>   | 00 a0 |
                    PSRLW        mm1, 8                         // shift for get high 8 byte of word

                    PSLLW        mm0, 8

                    POR          mm0, mm1

                    MOVQ        [edi],mm0

                    EMMS
                //    END_MMX:
                };

                // 因為傳進來的應該是已經合成過的了
                //(*(ubyte_t*)(dest+3)) =(*alpha);
                //(*(ubyte_t*)(dest+7)) =(*alpha+1);
            }

            src+=8;
            dest+=8;
            alpha+=2;
        }

        if(c_mod_8.rem>0)
        {
            VMC_Alpha_32b_Blend(dest, alpha, src, c_mod_8.rem);
        }
        return;
    }


    //=============================================================================
    // NAME:  VMC_MMX_ColorKey_32b_Blend
    // DESC:  
    //=============================================================================
    inline void VMC_MMX_ColorKey_32b_Blend(ubyte_t* dest, int colorkey, const ubyte_t* src, int count)
    {
        div_t c_mod_8 = div(count, 8);
        int keymask[2] = {colorkey, colorkey};

        for(int i = 0; i<c_mod_8.quot; i++)
        {
            _asm
            {
                MOV          esi,   src
                MOV          edi,   dest

                MOVQ         mm0,   [esi] 
                MOVQ         mm1,   [edi]  
                MOVQ         mm6,   mm0           // prepare mm6 to be mask

                PCMPEQD      mm6,   keymask       // mm6   | K  | S1 |  &| K  | K |      => │0xFFFF │ 0 │(mask)
                MOVQ         mm7,   _MMX_32B_FULL_MASK // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF

                PAND         mm1,   mm6           //       | K  | S1 |  &| 0xFFFF | K |  =>  | 0  | S1 |
                //MOVQ         mmm0,  mm6
                PANDN        mm6,   mm7           // if mm6  0XFF00FF00  pandn 0XFFFFFFFF  => 0X00FF00FF
                //MOVQ         mmm1,  mm6
                PAND         mm0,   mm6           //       | D1 | D2 |  &| 0xFFFF | K |  =>  | D1 | 0  |

                POR          mm0,   mm1

                MOVQ         [edi], mm0
                EMMS
            }

            src+=8;
            dest+=8;
        }

        // 不滿16的 就直接copy
        if(c_mod_8.rem>0)
        {
            VMC_ColorKey_32b_Blend(dest, colorkey, src, c_mod_8.rem);
        }
    }



    inline void VMC_MMX_Alpha_combine(ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len)
    {
        for(int i = 0; i < len; i++)
        {
            dest[i] = __max(srca0[i], srca1[i]);
        }
    }
}

#endif // _X86_
#endif
