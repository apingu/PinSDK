/*==================================================================================
 *
 *  Graphic headle define
 *
 *
 *
 *
 *===================================================================================*/
/*
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
//  PIN header  V0.7
//
typedef struct tagSrcImageInfo //8
{
    WORD SrcWidth;             //2   31
	WORD SrcHeight;            //2   33 
    WORD SrcMisregistrationX;  //2   35   Image offset x in source image
    WORD SrcMisregistrationY;  //2   37   Image offset y in source image
}SrcInfo;

typedef struct tagExternInfo    //3
{
    BYTE  ExtAct;               //1  39  picture action information
    BYTE  ExtDirection;         //1  40  picture direction information
    BYTE  ExtStep;              //1  41  picture step information
}ExtInfo;
*/

#pragma pack (push,1)

typedef struct tagPINHEADERV0_7
{
    char    ApName[4];	        //4
    WORD    ApAlphaOffset;      //2  4  read headle length and alpha start
    DWORD   ApImageOffset;      //4  6  image array start address
    WORD    ApPinWidth;         //2  10
    WORD    ApPinHeight;        //2  12
    BYTE    ApPixelMode;	    //1  14
    short   ApRelativeX;	    //2  15
    short   ApRelativeY;	    //2  17
    WORD    ApRectleft;         //2  19
    WORD    ApRecttop;          //2  21
    WORD    ApRectright;        //2  23
    WORD    ApRectbottom;       //2  25
    WORD    APHitPointX;        //2  27
    WORD    APHitPointY;        //2  29
    SrcInfo ApSrcImage;         //8  31 Source image infomation
    ExtInfo ApExtInfo;          //3  39 extern information
    BYTE    ApReserved[22];     //22 42 64

}PINHEADERV0_7;

#pragma pack (pop)

/*

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



//////////////////////////////////////////////////////////////////////////////////////////////////

int BMPToPin(const char*    alphafilepath, 
			 const char*    srcfilepath, 
			 const char*    destfilepath,
			 bool           trimRange = true,
			 unsigned char  mode = 16, 
			 unsigned short AdtumMarkX = 0, 
			 unsigned short AdtumMarkY = 0, 
			 unsigned char  Act = 0, unsigned char Direction = 0, unsigned char Step = 0
			 );

int TGAToPin(const char*    srcfilepath , 
			 const char*    destfilepath, 
			 bool           trimRange = true,
			 unsigned char  mode = 16   ,
			 unsigned short pAdtumMarkX = 0, 
			 unsigned short pAdtumMarkY = 0, 
			 unsigned char  pAct = 0, unsigned char pDirection = 0, unsigned char pStep = 0
			);


void WritePinHeader(FILE *fh,
					unsigned short SrcWidth, unsigned short SrcHeight,
					unsigned int  AlphaSize, 
					RECT ImageRect,
					RECT ObjectRect,
					int mode = 16,
					unsigned short AdtumMarkX = 0, unsigned short AdtumMarkY = 0,
					BYTE Act = 0, BYTE Direction = 0, BYTE Step = 0);

RECT  MinScope (BYTE **SrcImage, 
				int Width, int Height,
				int Pixel
				);                   //compute this min scope

void  TrimScope(BYTE **SrcImage,
				int Width, int Height, 
				int Pixel,
				RECT TrimRect
				);   //cut to min scope

unsigned int  AddScope (BYTE **SrcImage,
				        int Width, int Height,
				        int Pixel,
				        int ImagePixel
				        );


#endif
*/
