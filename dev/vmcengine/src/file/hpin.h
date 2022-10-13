//
//
//
//  Graphic headle define V1.01
//
//
//
//                             2002 / 7 /31      V1.01 add alpha compress flag
//                                               Copyright    (C)    1999    Pin
//

#ifndef PGF_H
#define PGF_H


//#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pf-def.h"


#ifdef WIN32
#pragma pack ( push, 1 )
#else
__attribute__ ( ( aligned ( 1 ) ) )
#endif

//#define JUMP       0
//#define COPY       255


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PIN header  V1.0  => 10
//
typedef struct tagSrcImageInfo  //20
{
    word_t   SrcWidth;              //2   20
    word_t   SrcHeight;             //2   22
    word_t   SrcOffsetX;            //2   24   Image offset x in source image
    word_t   SrcOffsetY;            //2   26   Image offset y in source image
} SrcInfo;

typedef struct tagExternInfo     //3
{
    byte_t   ExtAct;               //1  27  picture action information
    byte_t   ExtDirection;         //1  28  picture direction information
    byte_t   ExtStep;              //1  29  picture step information
}ExtInfo;

typedef struct tagPINHEADER
{
    char     ApName[ 4 ];        //4  0
    word_t   ApAlphaOffset;      //2  4   read headle length and alpha start
    dword_t  ApImageOffset;      //4  6   image array start address
    word_t   ApPinWidth;         //2  10
    word_t   ApPinHeight;        //2  12
    word_t   ApPixelMode;        //2  14
    byte_t   ApACompressMode;    //1  16
    byte_t   ApCompressMode;     //1  17
    short    ApRelativeX;        //2  18
    short    ApRelativeY;        //2  20
    SrcInfo  ApSrcImage;         //8  22  Source image infomation
    byte_t   ApVar;              //1  30
    ExtInfo  ApExtInfo;          //3  31
    word_t   ApAmbitShape;       //2  34
    word_t   ApAmbitTall;        //2  36
    uint_t   ApId;               //4  38
    byte_t   ApReserved[ 54 ];   //62 38   extern segment begin
    word_t   APHitPointX3;       //2       
    word_t   APHitPointY3;       //2       128 extern information
    word_t   APHitPointX2;       //2  
    word_t   APHitPointY2;       //2  
    word_t   APHitPointX1;       //2  
    word_t   APHitPointY1;       //2       extern segment end
    word_t   ApZLineX0;          //2  108
    word_t   ApZLineY0;          //2  110
    word_t   ApZLineX1;          //2  112
    word_t   ApZLineY1;          //2  114
    word_t   ApRectleft0;        //2  116
    word_t   ApRecttop0;         //2  118
    word_t   ApRectright0;       //2  120
    word_t   ApRectbottom0;      //2  122
    word_t   APHitPointX0;       //2  124
    word_t   APHitPointY0;       //2  126  128 extern information
}PINHEADER;

#ifdef WIN32
#pragma pack ( pop )
#else
__attribute__ ( ( packed ) )
#endif

#endif
