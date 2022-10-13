/* ===========================================================================================
 *
 * Base Picture Class
 *
 *
 *                  this class use for read Pin Map 
 *
 *
 * =========================================================================================*/

#ifndef _CImage_H_
#define _CImage_H_

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "..\Timer\PTimer.h"
#include "PGDEF.h"

const int General = 0;
const int Alpha   = 1;


enum VedioMask {p565, p555, p556, p655, p888};

typedef class tagHPINMAP
{
protected:

    int        m_VedioMask;

public:

    tagHPINMAP();
    ~tagHPINMAP();

	BYTE       MapType;	
    WORD       Width;				//load picture width
    WORD       RealWidthByte;       //load picture width bytes
    WORD       Height;			    //load picture height
    WORD       Pixel;				//load pixelmode 8 16 32 ....
	DWORD      Size;                //file size 

    short      RelativeX;			//picture offset x view for paste
    short      RelativeY;           //picture offset y veiw for paste

    PCPOINT    Offset;		        //picture offset x view for user

    BYTE       *PicHeaderPtr;       //File start
	BYTE       *PicAlphaPtr;        //File Alpha   offset
    BYTE       *PicImagePtr;        //File picture offset
	BYTE       *PicPalette;

    PCPOINT    HitPoint;
    PCRECT     ObjRect;
    PCLINE     DatumLine;           //順序的基準線
    PTIMER     timer;

	tagHPINMAP& operator=(const tagHPINMAP &src);
	PCRECT  Rect            (PCPOINT Situs)   ;
	int     Load            (BYTE *pic, int VedioMask = 0) ;              //讀入Pin圖形檔
	int     Load            (const char *pathname, int VedioMask = 0) ;   //讀入Pin圖形檔
	void    ReLoad          (void)            ;      //重新載入
	void    P565ToP555      (void)            ;      //將565模式轉變為555
	void    P565ToP556      (void)            ;      //將565模式轉變為556
	void    P565ToP655      (void)            ;      //將565模式轉變為655
	void    P565ToP888      (void){}          ;      //將565模式轉變為888
	void    P888ToP565      (void){}          ;      //將888模式轉變為565
	void    P555ToP565      (void)            ;      //將555模式轉變為565
	void    Clear           (int Color)       ;
	void    Release         (void)            ;
	bool    InArea          (PCPOINT situs, PCPOINT point);
    int     ZOrder          (tagHPINMAP &Hpinmap);
	PCPOINT InScope         (PCPOINT point, int NormX = 0, int NormY = 0);
	
} HPINMAP, *LPHPINMAP;

#endif
/////////////////////////////////////////////////////////////////////////////