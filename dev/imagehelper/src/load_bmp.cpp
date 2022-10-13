// LoadTGA.cpp - implementation of loading .TGA file
//               file must be uncompressed/RLE 8/16/24/32 bits RGB images.
//
//  Developer   Date    Comments
//  yann chou   011899  building
//              081099  add RLE RGB images support
//                      fix 32 bits image converting to 16 bits error
//                      read alpha bits in Targa 32 image
//              021800  fix the wrong screen origin bit checking in ImageDescriptor (bug reported by miles)
//              100200  add 8 bits image support
//              100300  fix a fault in 021800 fixing (starnge! I figure it had been done)
//              112700  add ImageTypeCode 11(0x0B) support
//              011601  add ImageTypeCode 3 support

#include  <stdio.h>
#include  <memory.h>

#include "load_bmp.h"

typedef long           LONG;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  dword_t;

#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)


#pragma pack (push,1)

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  定義BMP header
//

#define Point8 unsigned char
typedef struct tagPoint24{
    BYTE B;
    BYTE G;
    BYTE R;
}Point24;

typedef struct tagPoint32{
    BYTE B;
    BYTE G;
    BYTE R;
    BYTE A;
}Point32;

#ifndef tagBITMAPINFOHEADER
typedef struct tagBITMAPINFOHEADER
{
    dword_t biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    dword_t biCompression;
    dword_t biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    dword_t biClrUsed;
    dword_t biClrImportang;
}BITMAPINFOHEADER;
#endif

#ifndef tagBITMAPFILEHEADER
typedef struct tagBITMAPFILEHEADER
{
    WORD  bfType;
    dword_t bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    dword_t bfOffBits;
}BITMAPFILEHEADER;
#endif



#pragma pack ( pop )


//=============================================================================
//
//
//=============================================================================
int LoadBMP(FILE *fh, unsigned char **lplpImage, int pixelFmt)
{
    int Pixel_size = 0;
    switch( pixelFmt )
    {
    case PIXELFMT_8:    Pixel_size = 1;   break;
    case PIXELFMT_RGB:  Pixel_size = 3;   break;
    case PIXELFMT_RGBA: Pixel_size = 4;   break;
    case PIXELFMT_ARGB: Pixel_size = 4;   break;
    default:            Pixel_size = 2;   break;
    }

    BITMAPINFOHEADER bih;
    BITMAPFILEHEADER bif;
    

    int BitMapWidth = 0; 
    /*read source bmp file header*/
    if(fread(&bif, 1, sizeof(bif), fh) != sizeof(bif))
    {
        fclose( fh );
        return BMPERR_EHEADER;
        
    }
    if(fread(&bih, 1, sizeof(bih), fh) != sizeof(bih))
    {
        fclose( fh );
        return BMPERR_INFOHEADER;
    }

    BitMapWidth = ((((bih.biWidth) * (bih.biBitCount  / 8)) + 3)/4) * 4;
    int size = bih.biHeight * bih.biWidth * Pixel_size;
    ( *lplpImage ) = new BYTE[ size + 4 ];
    BYTE* lpImage = ( *lplpImage );
    *(WORD*)lpImage = (WORD)( bih.biWidth * Pixel_size );
    lpImage = lpImage + 2;
    *(WORD*)lpImage = (WORD)( bih.biHeight );
    lpImage = lpImage + 2;


    /*將資料檔寫入*/
    for( int i =0 ; i < bih.biHeight ; i++)
    {
        fseek(fh, bif.bfOffBits + (((bih.biHeight - i) - 1) * BitMapWidth), SEEK_SET);
        //lseek(srcfptr,bif.bfOffBits + ((rowscount - 1) * (bih.biWidth * (bih.biBitCount / 8))), SEEK_SET);
        for( int j = 0 ; j < bih.biWidth ; j++)
        {
            switch( bih.biBitCount )
            {
                case 8:
                {
                    Point8 BmpPoint; 
                    fread(&BmpPoint,sizeof(BmpPoint),1, fh);
                    switch(pixelFmt)
                    {
                        case PIXELFMT_8:
                            *(BYTE*)lpImage = BmpPoint; lpImage ++;
                            break;
                        default: return BMPERR_NOSUPPORT;

                    }
                    break;
                }

                case 24:
                {
                    Point24 BmpPoint; 
                    fread(&BmpPoint,sizeof(BmpPoint),1, fh);
                    switch(pixelFmt)
                    { 
                        case PIXELFMT_565: //--------------16 bits Pin file---------------------
                        {
                            *(WORD*)lpImage = (WORD)( ((BmpPoint.R >> 3) << 11) |  ((BmpPoint.G >> 2) << 5)  | (BmpPoint.B >> 3) );
                            lpImage = lpImage + 2;
                            break;
                        }
                         case PIXELFMT_RGB:    //--------------24 bits Pin file---------------------
                        {
                            *(BYTE*)lpImage = BmpPoint.R; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.G; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.B; lpImage ++;
                            break;
                        }
                        case PIXELFMT_RGBA:    //--------------RBGA 32 bits Pin file----------------
                        {
                            *(BYTE*)lpImage = BmpPoint.R; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.G; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.B; lpImage ++;
                            *(BYTE*)lpImage = 0XFF;       lpImage ++;
                            break;
                        }
                        case PIXELFMT_ARGB:    //--------------RBGA 32 bits Pin file----------------
                        {
                            *(BYTE*)lpImage = 0XFF;       lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.R; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.G; lpImage ++;
                            *(BYTE*)lpImage = BmpPoint.B; lpImage ++;
                            break;
                        }


                        default: return BMPERR_NOSUPPORT;
                    }
                    break;
                }

            }
        }

    }
    return BMPERR_OK;
}






//=============================================================================
//
//
//=============================================================================
int  LoadBMPPalt(FILE *fh, unsigned char **lplpPalette, int flag)
{




    return BMPERR_OK;
}
