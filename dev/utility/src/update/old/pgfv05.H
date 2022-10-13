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
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/stat.h>
//#include <windows.h>


#define TRACE(s)         printf(s)  //OutputDebugString(s)
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


#define BYTE unsigned char
#define WORD unsigned short int
#define DWORD unsigned int



#pragma pack (push,1)

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


#ifndef _WINDOWS_H_

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  定義BMP header
//

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
//  PIN header  V0.6
//
typedef struct tagSrcImageInfo
{
    WORD SrcWidth;
	WORD SrcHeight;
    WORD SrcMisregistrationX;  //Image offset x in source image
    WORD SrcMisregistrationY;  //Image offset y in source image
}SrcInfo;

typedef struct tagExternInfo
{
    BYTE  ExtAct;                //1  34  picture action information
    BYTE  ExtDirection;          //1  35  picture direction information
    BYTE  ExtStep;               //1  36  picture step information
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
    WORD    ApRectWidth;        //2  17
    WORD    ApRectHeight;       //2  19
	WORD    APHitPointX;        //2  21
    WORD    APHitPointY;        //2  23
	SrcInfo ApSrcImage;         // Source image infomation  
	ExtInfo ApExtInfo;          // extern information
    BYTE    ApReserved[28];     //28

}PINHEADER;


/*
typedef struct tagPINHEADER{

    char  ApName[4];	        //4 
    WORD  ApWidth;              //2  4   bitmap width
    WORD  ApHeight;             //2  6   bitmap height
    DWORD ApOffset;             //4  10  image array start address
    WORD  ApPixelMode;	        //2  12
    WORD  ApMisregistrationX;   //2  14
    WORD  ApMisregistrationY;   //2  16
    short ApRelativeX;	        //2  18
    short ApRelativeY;	        //2  20
    WORD  ApPinWidth;           //2  22
    WORD  ApPinHeight;          //2  24
    WORD  ApRectWidth;          //2  26
    WORD  ApRectHeight;         //2  28
	WORD  APHitPointX;          //2  30
    WORD  APHitPointY;          //2  32 
    BYTE  ApAct;                //1  34  picture action information
    BYTE  ApDirection;          //1  35  picture direction information
    BYTE  ApStep;               //1  36  picture step information
    BYTE  ApReserved[27];       //27
}PINHEADER;
*/

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
