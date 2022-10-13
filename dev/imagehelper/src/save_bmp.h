// SaveBMP.h : header of saving .BMP file
//
// Author:     Yann Chou
//

#ifndef _SAVE_BMP_H_
#define _SAVE_BMP_H_

// Pixelformats in LoadBMP(..)
#ifndef PIXELFMT_555
    #define PIXELFMT_8     -1   // 8 bits indexed
    #define PIXELFMT_888    8   // 24 bits with [R8:G8:B8]
    #define PIXELFMT_BGR  128   // not supported yet
#endif

int  SaveBMP(FILE *fh,                  // file handle of .BMP
             unsigned char *lpImage,    // 8/24 bits raw image
             unsigned char *lpPaltte,
             int pixelFmt);             // pixel format of raw image

#endif  // _SAVE_BMP_H_
