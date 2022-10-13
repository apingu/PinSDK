// LoadTGA.h : header of loading .TGA file
//
// Author:     Yann Chou
//

#ifndef _LOAD_TGA_H_
#define _LOAD_TGA_H_

// Error code
#define TGAERR_OK            1
#define TGAERR_SIGNATURE    -1
#define TGAERR_PIXELDEPTH   -2
#define TGAERR_COMPRESSMODE -3
#define TGAERR_MEMORYOUT    -4
#define TGAERR_CONVERTING   -5

#define TGAERR_TYPECODE     -6
#define TGAERR_COLORMAP     -7

// Pixelformats in LoadTGA(..)
#ifndef PIXELFMT_555
    #define PIXELFMT_8     -1   // 8 bits indexed
    #define PIXELFMT_555    2   // 16bits with [R5:G5:B5]
    #define PIXELFMT_565    4   // 16bits with [R5:G6:B5]
    #define PIXELFMT_888    8   // 24bits with [R8:G8:B8]
    #define PIXELFMT_RGBA  16   // 32bits with [R8:G8:B8:A8]
    #define PIXELFMT_BGR  128   // not supported yet
#endif

int  LoadTGA(FILE *fh,                   // file handle of .TGA
             unsigned char **lplpImage,  // return 8/16/24 bits raw image
             unsigned char **lplpAlpha,  // return 8 bits alpha data (if available)
             int pixelFmt);              // pixel format of raw image u want

int  LoadTGAPalt(FILE *fh,
             unsigned char **lplpPalette, // return 768 bytes RGB array
             int flag = 0);                  // not used

#endif  // _LOAD_TGA_H_
