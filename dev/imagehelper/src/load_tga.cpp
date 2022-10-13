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

#include  "load_tga.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  dword_t;


// about Targa file format plz see technical Manual from Truevision
#pragma pack ( push, 1 )
typedef struct _TGAHEADER
{
    BYTE  IDFieldSize;       // Number of Characters in Identification Field.
    BYTE  ColorMapType;      // Color Map Type.
    BYTE  ImageTypeCode;     // Image Type Code.
                             // 0  - No image data included.
                             // 1  - Uncompressed, color-mapped images.
                             // 2  - Uncompressed, RGB images.
                             // 3  - Uncompressed, black and white images.
                             // 9  - Runlength encoded color-mapped images.
                             // 10 - Runlength encoded RGB images.
                             // 11 - Compressed, black and white images.
    // Color Map Specification:
    WORD  ColorMapOrigin;    // Color Map Origin.
    WORD  ColorMapLength;    // Color Map Length.
    BYTE  ColorMapEntrySize; // Number of Bits in Color Map Entry.
    // Image Specification:
    WORD  XOrigin;           // X Origin of Image.
    WORD  YOrigin;           // Y Origin of Image.
    WORD  Width;             // Width of Image.
    WORD  Height;            // Height of Image.
    BYTE  PixelSize;         // Number of Bits in a Pixel.
    BYTE  ImageDescriptor;   // Image Descriptor Byte.
                             // Bits 3-0 - number of attribute bits associated with each
                             //            pixel.  For the Targa 16, this would be 0 or
                             //            1.  For the Targa 24, it should be 0.  For
                             //            Targa 32, it should be 8.
                             // Bit 4    - reserved.  Must be set to 0.
                             // Bit 5    - screen origin bit.
                             //            0 = Origin in lower left-hand corner.
                             //            1 = Origin in upper left-hand corner.
                             //            Must be 0 for Truevision images.
                             // Bits 7-6 - Data storage interleaving flag.
                             //            00 = non-interleaved.
                             //            01 = two-way (even/odd) interleaving.
                             //            10 = four way interleaving.
                             //            11 = reserved.
}   TGAHEADER;
#pragma pack ( pop )
// varies   Image Identification Field.
// varies   Color map data.
// varies   Image Data Field. (width) x (height) Pixels.


typedef struct _TGAFMT
{
    TGAHEADER  Header;
    BYTE       *IDFieldData;
    BYTE       *ColorMapData;
    BYTE       *Pixels;
}   TGAFMT;


static TGAFMT  TgaInfo;

static int  UnpackImageData(FILE *fh);
static int  UnpackRLEData(FILE *fh);
static bool ConvertToRaw(int pixelFmt, unsigned char **lplpImage,
                         unsigned char **lplpAlpha);

/////////////////////////////////////////////////////////////////////////////
int  LoadTGA(FILE *fh, unsigned char **lplpImage, unsigned char **lplpAlpha,
             int pixelFmt)
{
    long  size;

    memset( &TgaInfo, 0, sizeof(TGAFMT) );

    // Read header
    fread(&TgaInfo.Header, sizeof(TGAHEADER), 1, fh);

    // Check Image Type
    if (TgaInfo.Header.ImageTypeCode != 1  &&
        TgaInfo.Header.ImageTypeCode != 2  &&
        TgaInfo.Header.ImageTypeCode != 3  &&
        TgaInfo.Header.ImageTypeCode != 9  &&
        TgaInfo.Header.ImageTypeCode != 10 &&
        TgaInfo.Header.ImageTypeCode != 11)
        return TGAERR_TYPECODE;

    // Convert number of bits in a pixel to bytes
    TgaInfo.Header.PixelSize >>= 3;

    if (TgaInfo.Header.PixelSize < 1 ||
        TgaInfo.Header.PixelSize > 4)
        return TGAERR_PIXELDEPTH;

    // Skip image identification field
    fseek(fh, TgaInfo.Header.IDFieldSize, SEEK_CUR);

    // Read color map data
    if (TgaInfo.Header.ColorMapType)
    {
        size = TgaInfo.Header.ColorMapLength * (TgaInfo.Header.ColorMapEntrySize >> 3);

        if (TgaInfo.Header.PixelSize == 1)
        {
            if (TgaInfo.Header.ColorMapOrigin != 0 ||
                TgaInfo.Header.ColorMapLength != 256 ||
                TgaInfo.Header.ColorMapEntrySize != 24)
                return TGAERR_COLORMAP;

            TgaInfo.ColorMapData = new BYTE [size];

            if (TgaInfo.ColorMapData == NULL)
                return TGAERR_MEMORYOUT;

            // Read palette (size = 768 bytes)
            fread(TgaInfo.ColorMapData, 1, size, fh);
        }
        else
        {
            TgaInfo.ColorMapData = NULL;

            // Skip color map data
            fseek(fh, size, SEEK_CUR);
        }
    }

    // Allocate image data
    size = TgaInfo.Header.Height * TgaInfo.Header.Width * TgaInfo.Header.PixelSize;
    TgaInfo.Pixels = new BYTE [size];
    if (TgaInfo.Pixels == NULL)
    {
        delete [] TgaInfo.ColorMapData;
        return TGAERR_MEMORYOUT;
    }

    // Read merged image data
    if (TgaInfo.Header.ImageTypeCode == 1 ||
          TgaInfo.Header.ImageTypeCode == 2 ||
          TgaInfo.Header.ImageTypeCode == 3)
        UnpackImageData(fh);
    else // == 9 || == 10 || == 11
        UnpackRLEData(fh);

    // Convert to raw data
    if (ConvertToRaw(pixelFmt, lplpImage, lplpAlpha) == false)
        size = TGAERR_CONVERTING;
    else
        size = TGAERR_OK;

    delete [] TgaInfo.ColorMapData;
    delete [] TgaInfo.Pixels;

    return (int) size;  // size is now TGAERR_XX here!
}

int UnpackImageData(FILE *fh)
{
    int  lineCount = TgaInfo.Header.Width * TgaInfo.Header.PixelSize;

    // Read uncompressed data
    if (TgaInfo.Header.ImageDescriptor & 0x20)
    {
        // Origin in upper left-hand corner
        fread(TgaInfo.Pixels, sizeof(BYTE), lineCount * TgaInfo.Header.Height, fh);
    }
    else
    {
        // Origin in lower left-hand corner
        BYTE *ptr = TgaInfo.Pixels + lineCount * (TgaInfo.Header.Height - 1);

        for (int h = 0; h < TgaInfo.Header.Height; h++)
        {
            fread(ptr, sizeof(BYTE), lineCount, fh);

            ptr -= lineCount;
        }
    }

    return TGAERR_OK;
}

int UnpackRLEData(FILE *fh)
{
    BYTE *ptr;
    int  length;
    int  count;
    BYTE c;
    BYTE pixel[4];
    int  bytes;

    // Read uncompressed data
    if (TgaInfo.Header.ImageDescriptor & 0x20)
    {
        // Origin in upper left-hand corner
        ptr = TgaInfo.Pixels;
        length = TgaInfo.Header.Width * TgaInfo.Header.Height;

        count = 0;

        while (count < length)
        {
            // Origin in upper left-hand corner
            fread(&c, sizeof(BYTE), 1, fh);

            if (c & 0x80)   // the run-length packet
            {
                c = (c & 0x7F) + 1;  // packet length in pixels
                count += c;

                fread(pixel, sizeof(BYTE), TgaInfo.Header.PixelSize, fh);

                while (c--)
                {
                    memcpy(ptr, pixel, TgaInfo.Header.PixelSize);

                    ptr += TgaInfo.Header.PixelSize;
                }
            }
            else            // the raw packet
            {
                c = (c & 0x7F) + 1;  // packet length in pixels
                count += c;

                bytes = c * TgaInfo.Header.PixelSize;
                fread(ptr, sizeof(BYTE), bytes, fh);

                ptr += bytes;
            }
        }
    }
    else
    {
        // Origin in lower left-hand corner
        length = TgaInfo.Header.Width * TgaInfo.Header.PixelSize;
        ptr = TgaInfo.Pixels + length * (TgaInfo.Header.Height - 1);

        for (int h = 0; h < TgaInfo.Header.Height; h++)
        {
            BYTE *p = ptr;

            count = 0;

            // assume the packet doesn't cross the scan line (see doc.)
            while (count < TgaInfo.Header.Width)
            {
                fread(&c, sizeof(BYTE), 1, fh);

                if (c & 0x80)   // the run-length packet
                {
                    c = (c & 0x7F) + 1;  // packet length in pixels
                    count += c;

                    fread(pixel, sizeof(BYTE), TgaInfo.Header.PixelSize, fh);

                    while (c--)
                    {
                        memcpy(p, pixel, TgaInfo.Header.PixelSize);

                        p += TgaInfo.Header.PixelSize;
                    }
                }
                else            // the raw packet
                {
                    c = (c & 0x7F) + 1;  // packet length in pixels
                    count += c;

                    bytes = c * TgaInfo.Header.PixelSize;
                    fread(p, sizeof(BYTE), bytes, fh);

                    p += bytes;
                }
            }

            ptr -= length;
        }
    }

    return TGAERR_OK;
}

bool ConvertToRaw(int pixelFmt, unsigned char **lplpImage, unsigned char **lplpAlpha)
{
    unsigned char *lpImage;
    unsigned char *lpAlpha;
    int            nPixelCount = TgaInfo.Header.Width * TgaInfo.Header.Height;
    int            nPixelDepth;

//  if (TgaInfo.Pixels == NULL)
//      return false;

    if (pixelFmt == PIXELFMT_8)
        nPixelDepth = 1;
    else if ((pixelFmt == PIXELFMT_555) || (pixelFmt == PIXELFMT_565))
        nPixelDepth = 2;
    else if (pixelFmt == PIXELFMT_888)
        nPixelDepth = 3;
    else if (pixelFmt == PIXELFMT_RGBA)///////by Pin
        nPixelDepth = 4;
    else
        return false;    // unsupported pixel format

    // Rap alpha channel
    if (lplpAlpha != NULL)
    {
        if (TgaInfo.Header.PixelSize == 4)
        {        
            *lplpAlpha = new unsigned char [nPixelCount + 4];
    
            if (*lplpAlpha != NULL)
            {
                lpAlpha = *lplpAlpha;

                *(unsigned short *) (lpAlpha)     = TgaInfo.Header.Width;
                *(unsigned short *) (lpAlpha + 2) = TgaInfo.Header.Height;

                lpAlpha += 4;

                // Copy alpha bits
                for (int i = 0; i < nPixelCount; i++)
                    *(lpAlpha++) = TgaInfo.Pixels[(i << 2) + 3];
            }
            // else return false for TGAERR_MEMORYOUT
        }
        else
            *lplpAlpha = NULL;
    }

    // Allocate raw image data
    *lplpImage = new unsigned char [nPixelCount * nPixelDepth + 4];
    if (*lplpImage == NULL)
        return false;

    lpImage = *lplpImage;

    *(unsigned short *) (lpImage)     = (TgaInfo.Header.Width * nPixelDepth);
    *(unsigned short *) (lpImage + 2) = TgaInfo.Header.Height;

    lpImage += 4;

    // Copy pixels from RGB image data
    if ((TgaInfo.Header.PixelSize == 1 && pixelFmt == PIXELFMT_8)   ||
           (TgaInfo.Header.PixelSize == 2 && pixelFmt == PIXELFMT_555) ||
        (TgaInfo.Header.PixelSize == 3 && pixelFmt == PIXELFMT_888))
    {
        // Lucky! just copy on the fly
        memcpy(lpImage, TgaInfo.Pixels, nPixelCount * nPixelDepth);

        return true;
    }
    
    if (pixelFmt == PIXELFMT_8)
    {
        delete [] *lplpImage;

        *lplpImage = NULL;

        return false;   // convert 16/24/32 bits image to 8 bits is not supported yet
    }

    // Convert to 16/24 bits raw data
    unsigned char   r, g, b;
    unsigned short  color;

    for (int i = 0; i < nPixelCount; i++)
    {
        // Get R,G,B values of pixel
        if (TgaInfo.Header.PixelSize == 1)
        {
            color = *(TgaInfo.Pixels + i) * 3;

            b = TgaInfo.ColorMapData[color];
            g = TgaInfo.ColorMapData[color + 1];
            r = TgaInfo.ColorMapData[color + 2];
        }
        else if (TgaInfo.Header.PixelSize == 2)
        {
            color = *(unsigned short *) (TgaInfo.Pixels + (i << 1));

            // expand to 8 bits
            r = (color & 0x7C00) >> 7;
            g = (color & 0x03E0) >> 2;
            b = (color & 0x001F) << 3;
        }
        else if (TgaInfo.Header.PixelSize == 3)
        {
            b = TgaInfo.Pixels[i * 3];
            g = TgaInfo.Pixels[i * 3 + 1];
            r = TgaInfo.Pixels[i * 3 + 2];
        }
        else if (TgaInfo.Header.PixelSize == 4)
        {
            b = TgaInfo.Pixels[(i << 2)];
            g = TgaInfo.Pixels[(i << 2) + 1];
            r = TgaInfo.Pixels[(i << 2) + 2];
        }

        // Put it to new pixel
        if (pixelFmt == PIXELFMT_555)
        {
            r >>= 3;            // shrink 8 bits to 5 bits
            g >>= 3;
            b >>= 3;

            *(lpImage++) = (g << 5) | b;        // low byte
            *(lpImage++) = (r << 2) | (g >> 3); // hight byte
        }
        else if (pixelFmt == PIXELFMT_565)
        {
            r >>= 3;            // shrink 8 bits to 5 bits
            g >>= 2;
            b >>= 3;

            *(lpImage++) = (g << 5) | b;        // low byte
            *(lpImage++) = (r << 3) | (g >> 3); // hight byte
        }
        else if (pixelFmt == PIXELFMT_888)
        {
            *(lpImage++) = b;
            *(lpImage++) = g;
            *(lpImage++) = r;
        }
        else if (pixelFmt == PIXELFMT_RGBA)
        {
            *(lpImage++) = 0XFF;///////by Pin
            *(lpImage++) = b;
            *(lpImage++) = g;
            *(lpImage++) = r;
        }

    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////

int  LoadTGAPalt(FILE *fh, unsigned char **lplpPalette, int flag)
{
    unsigned char *lpPalt;

    // Read header
    fread(&TgaInfo.Header, sizeof(TGAHEADER), 1, fh);

    // Check Image Type
    if (TgaInfo.Header.ImageTypeCode != 1 && TgaInfo.Header.ImageTypeCode != 9)
        return TGAERR_TYPECODE;

    // Skip image identification field
    fseek(fh, TgaInfo.Header.IDFieldSize, SEEK_CUR);

    // Read color map data
    if (TgaInfo.Header.ColorMapType != 1 ||
        TgaInfo.Header.ColorMapLength != 256 ||
        TgaInfo.Header.ColorMapEntrySize != 24)
        return TGAERR_PIXELDEPTH;

    // Allocate palette array
    lpPalt = new unsigned char [768];
    if (lpPalt == NULL)
        return TGAERR_MEMORYOUT;

    // Read color map data
    fread(lpPalt, 1, 768, fh);

#ifdef  _USE_SVGA32
    // Since the VGA device expects a palette value to be 0-63
    // instead of 0-255, you need to divide the values read in
    // the palette by 4 here.
    for (int i = 0; i < 768; i++)
        lpPalt[i] = lpPalt[i] >> 2;
#endif

    *lplpPalette = lpPalt;

    return TGAERR_OK;
}
