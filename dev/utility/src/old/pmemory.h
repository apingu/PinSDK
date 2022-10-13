//VMCString.h 2D engine paste funtion kernel
//
//
//    1. use MMX to do memcpy is in vain, it will be more solw than REP
//    2. 32 bits mode int VGA card it A R G B
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

#ifndef PMEMORY_H
#define PMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <math.h>
#include "pdef.h"




//=============================================================================
//  Name :
//  Desc :1 byte pixel move
//=============================================================================
inline void pmemcpy( void* dest, const void* src, size_t n )
{
    __ASM_BEGIN__
	_AL    MOV    esi, src      AL_
	_AL    MOV    edi, dest     AL_
	_AL    MOV    eax, n        AL_
	_AL    MOV    ecx, eax      AL_
	_AL    SHR    ecx, 2        AL_
	_AL    REP    MOVSD         AL_
	_AL    MOV    ecx, eax      AL_
	_AL    AND    ecx, 3        AL_
	_AL    REP    MOVSB         AL_
	__ASM_END__
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline void pmemset( void *dest, int c, size_t n )
{
	//memset(dest, c, n);
	int m_c = n;

	__ASM_BEGIN__
	_AL    MOV  eax, c     AL_
	_AL    MOV  ebx, 0     AL_
	_AL    MOV  ecx, m_c   AL_
    _AL  LAB:              AL_
	_AL    MOV  dest, eax  AL_
	_AL    ADD	 ebx, 1    AL_
	_AL    LOOP LAB        AL_
	__ASM_END__

}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline void pmapcpy( Pbyte* dest, int d_w, const Pbyte* src, int s_w, int s_h, size_t n )
{
    int C_DIV_2 = n	>> 2;
    int C_MOD_1 = n	&  3;
    
	for( int row = 0; row < s_h; row ++ )
	{
	    __ASM_BEGIN__
	    _AL    MOV    esi, src      AL_
		_AL    MOV    edi, dest     AL_
		_AL    MOV    ecx, C_DIV_2  AL_
		_AL    REP    MOVSD         AL_
		_AL    MOV    ecx, C_MOD_1  AL_
		_AL    REP    MOVSB         AL_
		__ASM_END__
		dest += d_w; // 指向下一列 pixel 起始位址
		src  += s_w;
	}
}







///////////////////////////////////////////////////////////////////////////////
//  MMX function
//

//=============================================================================
//  Name :
//  Desc :1 byte pixel move
//=============================================================================
inline void MMX_pmemcpy( Pbyte* dest, Pbyte* src, size_t n ) 
{ 
    __ASM_BEGIN__
    _AL    MOV    esi, src    AL_
    _AL    MOV    edi, dest   AL_
    _AL    MOV    eax, n  AL_
	_AL    CMP    edx, 9      AL_
	_AL    JL     END_MMX     AL_
    _AL    MOV    ecx, eax    AL_
    _AL    SHR    ecx, 3      AL_
    _AL  ALIGN 8              AL_
    _AL  LP0:                 AL_
    _AL    MOVQ   mm0, [esi]  AL_
    _AL    ADD    esi, 8      AL_
    _AL    MOVQ   [edi],mm0   AL_
    _AL    ADD    edi, 8      AL_
    _AL    DEC    ecx         AL_
    _AL    JNZ    LP0         AL_
    
	_AL    MOV  ecx, eax      AL_
	_AL    AND  ecx, 7        AL_
    _AL    REP  MOVSB         AL_
    _AL    EMMS               AL_
	_AL  END_MMX:             AL_
	__ASM_END__

	int C_MOD_7 = n & 7 ;
	if( C_MOD_7 != 0 )
	{
		int do_already = n- C_MOD_7;
		pmemcpy( dest + do_already, src + do_already, C_MOD_7 );
	}
} 


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline void	MMX_pmemset( Pbyte *dest, int c, size_t n )
{


}



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline void	MMX_pZeroMemory( void *d_pt, size_t n )
{
	    Pbyte *b_d_pt = (Pbyte *)d_pt;

        __ASM_BEGIN__
		_AL  ALIGN 4
		_AL    mov	edi,b_d_pt     AL_
        _AL    mov	eax,n          AL_
		_AL    mov	ebx,eax        AL_
		_AL    shr	eax,3          AL_
		_AL    and	ebx,7          AL_
		_AL    cmp	eax,0          AL_
		_AL    je	q1             AL_ 

		_AL    pxor	mm1,mm1        AL_
        _AL  L1:                   AL_
		_AL    movq	[edi],mm1      AL_
		_AL    add	edi,8          AL_
		_AL    dec	eax            AL_
		_AL    jne	L1             AL_
        _AL  q1:                   AL_

		_AL    test	ebx,4          AL_
		_AL    jz	q2             AL_
		_AL    stosd               AL_
        _AL  q2:                   AL_
		_AL    test	ebx,2          AL_
		_AL    jz	q3             AL_
		_AL    stosw               AL_
        _AL  q3:                   AL_
		_AL    test	ebx,1          AL_
		_AL    jz	q4             AL_
		_AL    stosw               AL_
        _AL  q4:                   AL_
		_AL    emms                AL_
		__ASM_END__
}


#endif
