// SaveBMP.cpp - implementation of saving .BMP file
//               file saved must be uncompressed 8 bits indexed w/ palette
//               or 24 bits true color
//
//  Developer   Date    Comments
//  yann chou   990929  built
//              991128  added 24 bits image support


#include  <stdio.h>
#include  <stdlib.h>
#include  <memory.h>

#include  "save_bmp.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  dword_t;
typedef long           LONG;

#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

// about Windows bitmap file format plz see technical Manual from Micro$oft
#pragma pack ( push, 1 )
typedef struct tagBITMAPFILEHEADER  // bmfh
{
    WORD    bfType;
    dword_t   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    dword_t   bfOffBits;
}   BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER  // bmih
{
    dword_t   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    dword_t   biCompression;
    dword_t   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    dword_t   biClrUsed;
    dword_t   biClrImportant;
}   BITMAPINFOHEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

typedef struct tagRGBQUAD           // rgbq
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
}   RGBQUAD;
#pragma pack ( pop )


/////////////////////////////////////////////////////////////////////////////

int  SaveBMP(FILE *fh, unsigned char *lpImage, unsigned char *lpPalette, int pixelFmt)
{
    BITMAPFILEHEADER bmfileheader;
    BITMAPINFOHEADER bmheader;
    RGBQUAD          bmiColors[256];
    int              widthBytes, widthPixels, height;
    int              bitcount, colors = 0;
    long             imgsize, headsize, headpos;

    widthBytes = *(short *) lpImage;
    height     = *(short *) (lpImage + 2);

    if (pixelFmt == PIXELFMT_8)
    {
        bitcount = 8;
        colors   = 256;
        widthPixels = widthBytes;
    }
    else if (pixelFmt == PIXELFMT_888)
    {
        bitcount = 24;
        colors   = 0;
        widthPixels = widthBytes / 3;
    }
    else
        return 0;

    imgsize  = WIDTHBYTES((dword_t) widthPixels * bitcount) * height;

    headsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
               sizeof(RGBQUAD) * colors;

    headpos = ftell(fh);

    // Make BMP file header
    bmfileheader.bfType     = 0x4d42;       // 'BM'
    bmfileheader.bfSize     = headsize + imgsize;
    bmfileheader.bfReserved1= 0;
    bmfileheader.bfReserved2= 0;
    bmfileheader.bfOffBits  = headsize;

    // Make BMP info header
    bmheader.biSize         = sizeof(BITMAPINFOHEADER);
    bmheader.biWidth        = widthPixels;
    bmheader.biHeight       = height;       // bottom-up DIB
    bmheader.biPlanes       = 1;
    bmheader.biBitCount     = bitcount;     // bit count of pixel
    bmheader.biCompression  = BI_RGB;
    bmheader.biSizeImage    = imgsize;
    bmheader.biXPelsPerMeter= 0;
    bmheader.biYPelsPerMeter= 0;
    bmheader.biClrUsed      = 0;
    bmheader.biClrImportant = 0;

    // Make BMP color table
    if (colors == 256)
    {
        if (lpPalette == NULL)
            return 0;

        for (int i = 0, n = 0; i < 256; i++)
        {
            bmiColors[i].rgbRed      = lpPalette[n++];
            bmiColors[i].rgbGreen    = lpPalette[n++];
            bmiColors[i].rgbBlue     = lpPalette[n++];
            bmiColors[i].rgbReserved = 0;
        }
    }

    // Make BMP image
    int  bytes = WIDTHBYTES((dword_t) widthPixels * bitcount);

    char *image = new char [imgsize];
    char *from = (char *) (lpImage + 4);
    char *to   = image + bytes * (height - 1);

    if (image == NULL)
        return 0;

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < widthBytes; w++)
            *(to+w) = *(from++);

        to -= bytes;
    }


    // Write BMP file header
    fwrite(&bmfileheader, sizeof(BITMAPFILEHEADER), 1, fh);
    // Write BMP info header
    fwrite(&bmheader, sizeof(BITMAPINFOHEADER), 1, fh);
    // Write BMP color table
    if (colors == 256)
        fwrite(bmiColors, sizeof(RGBQUAD) * colors, 1, fh);
    // Write BMP image data
    fwrite(image, imgsize, 1, fh);

    delete [] image;

    return bmfileheader.bfSize;
}
