/*==================================================================================
 *
 *  Graphic headle define
 *
 *
 *
 *
 *===================================================================================*/

#ifndef _PGF_H_
#define _PGF_H_


#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <memory.h>
//#include <fcntl.h>
//#include <malloc.h>
//#include <sys/stat.h>
//#include <iostream.h>
//#include <time.h>
#include <windows.h>


#define TRACE(s)          printf(s);
#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)


#define JUMP       0
#define COPY       255


#define DownRight  0
#define DOWN       1
#define DownLeft   2
#define LEFT       3
#define UpLeft     4
#define UP         5
#define UpRight    6
#define RIGHT      7


#define FLAG       int


#pragma pack (push,1)



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  定義BMP header
//

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short int
#endif

#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef _WINGDI_
typedef struct tagRECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;
#endif


#ifndef _WINGDI_
typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;
    DWORD biWidth;
    DWORD biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    DWORD biXPelsPerMeter;
    DWORD biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportang;
}BITMAPINFOHEADER;
#endif

#ifndef _WINGDI_
typedef struct tagBITMAPFILEHEADER{
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
}BITMAPFILEHEADER;
#endif








//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  定義BMP檔 pixel
//
#define Point256 unsigned char
typedef struct tagPoint24{
    BYTE B;
    BYTE G;
    BYTE R;
}Point24;

typedef struct tagPoint32{
    BYTE B;
    BYTE G;
    BYTE R;
    BYTE Alpha;
}Point32;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PIN Pixel
//

#define PINPOINT16 unsigned short int

typedef struct tagPPixel24{
    BYTE R;
    BYTE G;
    BYTE B;
}PINPOINT24;

typedef struct tagPPixel32{
    BYTE R;
    BYTE G;
    BYTE B;
	BYTE Reserved;
}PINPOINT32;


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PIN header  V0.6
//
typedef struct tagSrcImageInfo
{
    WORD SrcWidth;             //2   29
	WORD SrcHeight;            //2   31 
    WORD SrcMisregistrationX;  //2   33   Image offset x in source image
    WORD SrcMisregistrationY;  //2   35   Image offset y in source image
}SrcInfo;

typedef struct tagExternInfo
{
    BYTE  ExtAct;               //1  37  picture action information
    BYTE  ExtDirection;         //1  38  picture direction information
    BYTE  ExtStep;              //1  19  picture step information
}ExtInfo;

typedef struct tagPINHEADER
{
    char    ApName[4];	        //4 
    WORD    ApPinWidth;         //2  4
    WORD    ApPinHeight;        //2  6
    BYTE    ApPixelMode;	    //1  8
    DWORD   ApOffset;           //4  9  image array start address
    short   ApRelativeX;	    //2  13
    short   ApRelativeY;	    //2  15
    WORD    ApRectleft;         //2  17
    WORD    ApRecttop;          //2  19
    WORD    ApRectright;        //2  21
    WORD    ApRectbottom;       //2  23
	WORD    APHitPointX;        //2  25
    WORD    APHitPointY;        //2  27
	SrcInfo ApSrcImage;         // Source image infomation  
	ExtInfo ApExtInfo;          // extern information
    BYTE    ApReserved[24];     //24

}PINHEADER;


typedef struct tagPACKERHEADER{
    char MPName[4];
    WORD MPNumber;
    WORD MidstCount;
    //WORD DownRightCOUNT;
    //WORD DownCOUNT;
    //WORD DownLeftCOUNT;
    //WORD LeftCOUNT;
    //WORD UpLeftCOUNT;
    //WORD UpCOUNT;
    //WORD UpRightCOUNT;
    //WORD RightCOUNT;
    //DWORD StartOffSet;
}PINMONHEADER;

#pragma pack (pop)

//////////////////////////////////////////////////////////////////////////////////////////////////



#endif
