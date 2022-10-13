//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef SAVEPIN_H
#define SAVEPIN_H

#include <pf-def.h>
#include <tImage.h>
#include <stdio.h>
#include <stdlib.h>

// Error code
#define PINERR_OK            1
#define PINERR_SIGNATURE    -1
#define PINERR_PIXELDEPTH   -2
#define PINERR_COMPRESSMODE -3
#define PINERR_MEMORYOUT    -4
#define PINERR_CONVERTING   -5

#define PINERR_EHEADER      -6
#define PINERR_INFOHEADER   -7
#define PINERR_NOSUPPORT    -8


int  SavePIN( FILE *fh,                                 // file handle of .BMP
              unsigned char*  lpImage,                  // 8/24 bits raw image
              unsigned char*  lpAlpha,
              PixelFormat     pixelFmt = PixelFormat::R5G6B5,  // pixel format of raw image
              bool            isTrim = true,
              int             gradation = 256,
              bool            isCompress = true );

#endif



