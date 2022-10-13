// grasp.cpp : Grasp font block from vpage and pack it
//             to 1 bit per pixel

#include "stdafx.h"	    // #include <stdio.h>
#include "V.h"
#include "Grasp.h"

extern unsigned long  RBitMask;
extern unsigned long  GBitMask;
extern unsigned long  BBitMask;

int    nUnpackSize;
char  *pUnpack  = NULL;
int    nPackSize;
char  *pPack    = NULL;

static int  nFontWidth  = 0;
static int  nFontHeight = 0;
static int  bSmooth     = -1;

void ReallocFontBuff(int width, int height, int smooth)
{
    if (width == nFontWidth && height == nFontHeight && smooth == bSmooth)
        return;

    FreeFontBuff();

    nFontWidth  = width;
    nFontHeight = height;
    bSmooth = smooth;

    nUnpackSize = width * height;

    pUnpack = new char [width * height * 2 + 4];    // at 16 bits mode

    VERIFY(pUnpack != NULL);

	if (smooth)
		nPackSize = (nUnpackSize + 1) / 2;  // 4 bits per pixel
	else
    	nPackSize = (nUnpackSize + 7) / 8;  // 1 bit per pixel

    pPack = new char [nPackSize];

    VERIFY(pPack != NULL);
}

void FreeFontBuff(void)
{
    if (pUnpack != NULL)
        delete [] pUnpack;
    if (pPack != NULL)
        delete [] pPack;

    pUnpack = pPack = NULL;
    nUnpackSize = nPackSize = 0;

    nFontWidth = nFontHeight = 0;
}

int inline minof(int a, int b, int c)
{
    if (a > b)
        return (b > c) ? c : b;
    else
        return (a > c) ? c : a;
}

int inline maxof(int a, int b, int c)
{
    if (a > b)
        return (a > c) ? a : c;
    else
        return (b > c) ? b : c;
}

void GraspFontToBuff(int offsetX, int offsetY)
{
    ASSERT(pUnpack != NULL);

    // Get font to memory (bg color is RGB(0,0,0), fg color is else)
    vgetimage(offsetX, offsetY, 
              offsetX + nFontWidth - 1, offsetY + nFontHeight - 1, pUnpack);

    static WORD  *upk;
    static BYTE  *pk;
    static int    i;
    static BYTE   r, g, b;
    static int    l;

    upk = (WORD *) (pUnpack + 4);
    pk  = (BYTE *) pPack;

	if (bSmooth)
	{
    	for (i = 0; i < nUnpackSize / 2; i++)		
    	{
            *pk = 0;

            // high 4 bits
/*     		r = ((*upk) & RBitMask) >> 10;
    		g = ((*upk) & GBitMask) >> 5;
    		b = ((*upk) & BBitMask);    
  			l = (minof(r,g,b) + maxof(r,g,b)) / 2; */
            l = (*upk) & BBitMask;      // r = g = b this time              
            l >>= 1;    // make l is 0~15
            upk++;
            
    		*pk |= ((l & 0x0F) << 4); 

            // low 4 bits
/*    		r = ((*upk) & RBitMask) >> 10;
    		g = ((*upk) & GBitMask) >> 5;
    		b = ((*upk) & BBitMask);    
  			l = (minof(r,g,b) + maxof(r,g,b)) / 2; */
            l = (*upk) & BBitMask;      // r = g = b this time              
            l >>= 1;    // make l is 0~15
    		upk++; 
            
    		*pk |= ((l) & 0x0F);  

    		pk++;
   		}
	}
	else
	{
    	// Pack 8 pixels to 1 byte
    	for (i = 0; i < nUnpackSize / 8; i++)
    	{
        	*pk = 0;

        	*pk |= *(upk++) ? 0x80 : 0;     // bit 7
        	*pk |= *(upk++) ? 0x40 : 0;     // bit 6
        	*pk |= *(upk++) ? 0x20 : 0;     // bit 5
        	*pk |= *(upk++) ? 0x10 : 0;     // bit 4
        	*pk |= *(upk++) ? 0x08 : 0;     // bit 3
        	*pk |= *(upk++) ? 0x04 : 0;     // bit 2
        	*pk |= *(upk++) ? 0x02 : 0;     // bit 1
        	*pk |= *(upk++) ? 0x01 : 0;     // bit 0

        	pk++;
    	}

        if (nUnpackSize % 8)    // any remains?
        {
    	    // Pack remain(less 8) pixels to 1 byte
    	    *pk = 0;

    	    for (i = 0; i < nUnpackSize % 8; i++)
    	    {
                *pk |= *(upk++) ? (1 << (7 - i)) : 0;
    	    }
        }
   	}
}
