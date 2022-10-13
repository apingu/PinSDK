/* ===========================================================================================
 *
 * Base Picture Class
 *
 *
 *                  this class use for read Pin Map 
 *
 *
 * =========================================================================================*/

#ifndef _DXPicture_H_
#define _DXPicture_H_

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "..\Timer\PTimer.h"
#include "PGDEF.h"

const int General = 0;
const int Alpha   = 1;


typedef class tagHPINMAP
{

public:

    tagHPINMAP();
    ~tagHPINMAP();

	BYTE       MapType;	
    WORD       Width;				//load picture width
    WORD       RealWidthByte;       //load picture width bytes
    WORD       Height;			    //load picture height
    WORD       Pixel;				//load pixelmode
	DWORD      Size;                //file size 

    short      RelativeX;			//picture offset x
    short      RelativeY;           //picture offset y

    PCPOINT    Norm;	            //圖形基準點

    BYTE       *PicHeaderPtr;       //File start
	BYTE       *PicAlphaPtr;
    BYTE       *PicImagePtr;        //File picture start
	BYTE       *PicPalette;

	PCLINE     DatumLine;           //順序的基準線
	WORD       RectWidth;
	WORD       RectHeight;
    PTIMER     timer;

	tagHPINMAP& operator=(const tagHPINMAP &src);
	int  Load(char *pathname);      //讀入Pin圖形檔
	void Load(BYTE *pic)     ;      //讀入Pin圖形檔 
	void ReLoad(void)        ;      //重新載入
	void P565ToP555(void)    ;      //將565模式轉變為555
	void P565ToP556(void)    ;      //將565模式轉變為556
	void P565ToP655(void)    ;      //將565模式轉變為655
	void P565ToP888(void){}  ;      //將565模式轉變為888
	void P888ToP565(void){}  ;      //將888模式轉變為565
	void Clear(int Color)    ;
	void Release(void)       ;
    int  ZOrder(tagHPINMAP &Hpinmap);
	
} HPINMAP, *LPHPINMAP;
#endif