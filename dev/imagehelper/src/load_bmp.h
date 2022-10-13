// LoadBMP.h : header of loading .BMP file
//
// Author:     Apingu
//

#ifndef _LOAD_BMP_H_
#define _LOAD_BMP_H_

// Error code
#define BMPERR_OK            1
#define BMPERR_SIGNATURE    -1
#define BMPERR_PIXELDEPTH   -2
#define BMPERR_COMPRESSMODE -3
#define BMPERR_MEMORYOUT    -4
#define BMPERR_CONVERTING   -5

#define BMPERR_EHEADER      -6
#define BMPERR_INFOHEADER   -7
#define BMPERR_NOSUPPORT    -8


// Pixelformats in LoadBMP(..)
#ifndef PIXELFMT_555
    #define PIXELFMT_8     -1   // 8 bits indexed
    #define PIXELFMT_555    2   // 16bits with [R5:G5:B5]
    #define PIXELFMT_565    4   // 16bits with [R5:G6:B5]
    #define PIXELFMT_RGB    8   // 24bits with [R8:G8:B8]
    #define PIXELFMT_RGBA  16   // 32bits with [R8:G8:B8:A8]
    #define PIXELFMT_ARGB  32   // 32bits with [B8:G8:R8:A8]
    #define PIXELFMT_BGR  128   // not supported yet
#endif

int  LoadBMP(FILE *fh,                   // file handle of .BMP
             unsigned char **lplpImage,  // return 8/16/24 bits raw image
             int pixelFmt);              // pixel format of raw image u want

int  LoadBMPPalt(FILE *fh,
             unsigned char **lplpPalette, // return 768 bytes RGB array
             int flag = 0);                  // not used

#endif  // _LOAD_BMP_H_