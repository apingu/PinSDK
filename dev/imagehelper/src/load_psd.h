// LoadPSD.h : header of loading .PSD file
//
// Author:     Yann Chou
//

#ifndef _LOAD_PSD_H_
#define _LOAD_PSD_H_

// Error code
#define PSDERR_OK            0
#define PSDERR_SIGNATURE    -1
#define PSDERR_PIXELDEPTH   -2
#define PSDERR_COMPRESSMODE -3
#define PSDERR_MEMORYOUT    -4
#define PSDERR_CONVERTING   -5

#define PSDERR_VERSION      -6
#define PSDERR_CHANNELNUM   -7
#define PSDERR_COLORMODE    -8
#define PSDERR_LAYERNUM     -9

// layer type
enum PSDLAYERTYPE
{
    PSDLAYER_UNKNOW,
    PSDLAYER_IMAGE,
    PSDLAYER_FOLDER,
};


// Pixelformats in LoadPSD(..)
#ifndef PIXELFMT_555
    #define PIXELFMT_555    2   // 16bits with [R5:G5:B5]
    #define PIXELFMT_565    4   // 16bits with [R5:G6:B5]
    #define PIXELFMT_888    8   // 24bits with [R8:G8:B8]
    #define PIXELFMT_BGR  128   // not supported yet
#endif  // PIXELFMT_555

int   LoadPSD(FILE *fh,                  // file handle of .PSD
              unsigned char **lplpImage, // return 16/24 bits raw image
              unsigned char **lplpAlpha, // return 8 bits alpha data
              int layer,                 // rap which layer (0 for merged all layers)
              int *left,                 // layer left
              int *top,                  // layer top
              int pixelFmt               // pixel format of raw image u want
             );

int   GetPSDLayerNum(FILE *fh, int *w, int *h);
int   GetPSDLayerName(FILE *fh, int layer, char* layername, int len, PSDLAYERTYPE& ly );


#endif  // _LOAD_PSD_H_
