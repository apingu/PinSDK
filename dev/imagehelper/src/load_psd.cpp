// LoadPSD.cpp - implementation of loading .PSD file
//               file must be 24 bits RGB color mode,
//               32 layers limited, 24 channels maximum in each layer.
//               (but only read first 4 channels, ie dont support user's channels)
//
//  Developer   Date    Comments
//  yann chou   990105  build from lasse skyum's source
//              990108  add rapping specified layer
//              990111  fix UnpackLayerData error
//              990119  fix a memory leak
//              000117  make LoadLayer fast
//                      fix lot of error in UnpackLayerData (bugs reported by miles)
//              000128  skip layer len is 0 in LoadLayer (bug reported by miles)
//                      return only 4 bytes for w,h(0,0) of image and alpha in this case
//              000222  fix crash in deleting null PsdInfo.Pixels while reenter LoadPSD
//                      fix crash in psd with only background layer but try to load other
//                      layers


#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <memory.h>

#include  "load_psd.h"

#define MAX_LAYERS      512000      // maximum number of layers in a .PSD


// about Photoshop file format plz see the SDK documentation from Adobe
typedef struct _PSDFMT
{
    // File header
    char  Signature[4]; // always equal to `8BPS'.
    short Version;      // always equal to 1.
    char  Reserved[6];  // must be zero.
    short Channels;     // number of channels in this image. range is 1 to 24
    int   Height;       // height of this image in pixels. range is 1 to 30000.
    int   Width;        // width of this image in pixels. range is 1 to 30000.
    short Depth;        // number of bits per channel. values are 1, 8, and 16
    short Mode;         // color mode of this image. values are
                        // 0: Bitmap, 1: Grayscale, 2: Indexed, 3: RGB
                        // 4: CMYK, 7: Multichannel, 8: Duotone, 9: Lab.

    // Color mode data
    unsigned int ColorDataLength;   // length of the following color data.

    // Image resources
    unsigned int ImageResLength;    // length of the following image resource section.

    // Layer and mask information
    unsigned int  MiscInfoLength;   // length of the misc information section.

    // Merged image data
    short Compression;  // compression method. 0 = Raw data, 1 = RLE compressed.
    struct Pixel
    {
        unsigned char Channel[4];   // B,G,R,A channel
    }   *Pixels;        // image data.
}   PSDFMT;

// in .PSD each layer will crop to fit the best image
#pragma pack (push,1)
typedef struct _LAYERMASK
{
    int top;
    int left;
    int bottom;
    int right;
    unsigned char definecolor;
    unsigned char flags;
    unsigned short padding;
}LAYERMASK;

/*
typedef struct _EXTRADATA
{
    _LAYERMASK layermask;
    char       layername[255];
}EXTRADATA;*/
#pragma pack ( pop )


typedef struct _LAYERRECORD
{
        int  top;                   // rectangle of this layer
        int  left;
        int  bottom;
        int  right;
        int  channels;              // number of channels in this layer
                                    // information of each channel
        int  channelID[24];         // 0: red, 1: green, 2: blue, etc.
                                    // -1: transparency mask
                                    // -2: user supplied layer mask
        int  channelDataLength[24]; // length of following channel data
        char blendModeSig[4];       // Signature always be `8BIM'
        char blendModeKey[4];
        unsigned char Opacity;      // 0 = transparebt ... 255 = opaque
        unsigned char Clipping;     // 0 = case, 1 = non-base
        unsigned char Flags;        // bit 0 = transparency protected
                                    // bit 1 = visible
        unsigned char filler;       // (zero)
        int  extraDataLength;       // length of the next extra data field
        //_EXTRADATA    extraData;
        _LAYERMASK    layermask;
        char          layername[255];
        
        
}   LAYERRECORD;



static PSDFMT       PsdInfo;
static LAYERRECORD  LayerInfo[MAX_LAYERS];

static inline unsigned short fgetw(FILE *fh)
{
    int  hi = fgetc(fh);
    int  lo = fgetc(fh);

    return lo + (hi << 8);
}

static inline unsigned int fgetdw(FILE *fh)
{
    unsigned int b3 = fgetc(fh);
    unsigned int b2 = fgetc(fh);
    unsigned int b1 = fgetc(fh);
    unsigned int b0 = fgetc(fh);

    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

static inline void fgetsig(FILE *fh, char *sig)
{
    fread(sig, sizeof(char), 4, fh);
}

static inline bool verifysig(char *sig, char *valid)
{
    if (sig[0] != valid[0] ||
        sig[1] != valid[1] ||
        sig[2] != valid[2] ||
        sig[3] != valid[3])
        return false;

    return true;
}

static int  LoadPSDHeader( FILE* fh );
static int  LoadLayer(FILE *fh, int layer);
static int  UnpackLayerData(FILE *fh, LAYERRECORD *pLayer);
static int  UnpackImageData(FILE *fh);
static bool ConvertToRaw(int pixelFmt, unsigned char **lplpImage,
                         unsigned char **lplpAlpha);

int LoadPSDHeader( FILE* fh )
{
    if(PsdInfo.Pixels!=NULL)
        delete [] PsdInfo.Pixels;
    memset(&PsdInfo, 0, sizeof(PsdInfo));

    // Read signature
    fgetsig(fh, PsdInfo.Signature);
    if (!verifysig(PsdInfo.Signature, "8BPS"))
        return PSDERR_SIGNATURE;

    // Read version
    if ((PsdInfo.Version = fgetw(fh)) != 1)
        return PSDERR_VERSION;

    // Skip reserved 6 bytes
    fgetdw(fh);
    fgetw(fh);

    // Read the number of channels
    if ((PsdInfo.Channels = fgetw(fh)) > 24)
        return PSDERR_CHANNELNUM;

    // Read heigh and width
    PsdInfo.Height = fgetdw(fh);
    PsdInfo.Width  = fgetdw(fh);

    // Read the number of bits per channel
    if ((PsdInfo.Depth = fgetw(fh)) != 8)
        return PSDERR_PIXELDEPTH;

    // Make sure the color mode is RGB
    if ((PsdInfo.Mode = fgetw(fh)) != 3)
        return PSDERR_COLORMODE;

    // Skip the color mode data
    // (It's the palette for indexed color; other info for other modes.)
    PsdInfo.ColorDataLength = fgetdw(fh);

    if (PsdInfo.ColorDataLength)
        fseek(fh, PsdInfo.ColorDataLength, SEEK_CUR);

    // Skip the image resources (resolution, pen tool paths, etc)
    PsdInfo.ImageResLength = fgetdw(fh);

    if (PsdInfo.ImageResLength)
        fseek(fh, PsdInfo.ImageResLength, SEEK_CUR);

    // Layer and mask records
    PsdInfo.MiscInfoLength = fgetdw(fh);
    return PSDERR_OK;
}

/////////////////////////////////////////////////////////////////////////////

int  LoadPSD(FILE *fh, unsigned char **lplpImage, unsigned char **lplpAlpha,
             int layer, int *left, int *top, int pixelFmt )

{
    int  ret;
    if( ( ret = LoadPSDHeader( fh ) )!=PSDERR_OK )
        return ret;

    if (PsdInfo.MiscInfoLength)
    {
        if (layer == 0)     // skip these data
            fseek(fh, PsdInfo.MiscInfoLength, SEEK_CUR);
        else
        {
            // Read layer info
            int LayersInfoLength = fgetdw(fh);

            ret = LoadLayer(fh, layer);

            if (ret != PSDERR_OK)
                return ret;

            if (top)  *top  = LayerInfo[layer - 1].top;
            if (left) *left = LayerInfo[layer - 1].left;

            // Skip the global layer mask info
            int GlobalLayerMaskInfoLength = fgetdw(fh);

            fseek(fh, GlobalLayerMaskInfoLength, SEEK_CUR);
        }
    }
    else if (layer)
    {
        // Only background layer and no other layer records
        return PSDERR_LAYERNUM;
    }

    if (layer == 0)     // LoadMerged(fh)
    {
        // Allocate image data
        PsdInfo.Pixels = (PSDFMT::Pixel *) new char [4 * PsdInfo.Height * PsdInfo.Width];
        if (PsdInfo.Pixels == NULL)
            return PSDERR_MEMORYOUT;

        // Read merged image data
        UnpackImageData(fh);

        if (left) *left = 0;
        if (top)  *top  = 0;
    }

    // Convert to raw and alpha data
    if (ConvertToRaw(pixelFmt, lplpImage, lplpAlpha) == false)
        ret = PSDERR_CONVERTING;
    else
        ret = PSDERR_OK;

    delete [] PsdInfo.Pixels;

    PsdInfo.Pixels = NULL;

    return ret;
}

int  LoadLayer(FILE *fh, int layer)
{
    int layers = (short) fgetw(fh);
    int i, j;

    layers = abs(layers);

    if (layer > layers || layers > MAX_LAYERS)
        return PSDERR_LAYERNUM;

    // Read all layer records
    for (i = 0; i < layers; i++)
    {
        LayerInfo[i].top       = fgetdw(fh);
        LayerInfo[i].left      = fgetdw(fh);
        LayerInfo[i].bottom    = fgetdw(fh);
        LayerInfo[i].right     = fgetdw(fh);
        LayerInfo[i].channels  = fgetw(fh);

        if (LayerInfo[i].channels > 24)
            return PSDERR_CHANNELNUM;

        for (j = 0; j < LayerInfo[i].channels; j++)
        {
            LayerInfo[i].channelID[j] = (short) fgetw(fh);
            LayerInfo[i].channelDataLength[j] = fgetdw(fh);
        }

        fgetsig(fh, LayerInfo[i].blendModeSig);
        if (!verifysig(LayerInfo[i].blendModeSig, "8BIM"))
            return PSDERR_SIGNATURE;

        fgetsig(fh, LayerInfo[i].blendModeKey);

        LayerInfo[i].Opacity  = fgetc(fh);
        LayerInfo[i].Clipping = fgetc(fh);
        LayerInfo[i].Flags    = fgetc(fh);
        LayerInfo[i].filler   = fgetc(fh);

        LayerInfo[i].extraDataLength = fgetdw(fh);

        // Skip extra data field
        //fseek(fh, LayerInfo[i].extraDataLength, SEEK_CUR);
        // Skip extra data field
        //不要用跳過的        fseek(fh, LayerInfo[i].extraDataLength, SEEK_CUR);
        int extralen = LayerInfo[i].extraDataLength;

        // Read Layer mask data
        int len = fgetdw(fh);
        extralen-=4;
        if (len)
        {
            fread( &LayerInfo[i].layermask, 1, sizeof(LayerInfo[i].layermask), fh );
            //fseek(fh, LayerInfo[i].extraDataLength, SEEK_CUR);
            //skip/or read len bytes
        }
        extralen-=len;

        // Read Layer blending ranges
        len = fgetdw(fh);
        extralen-=4;
        if (len)
        {
            fseek(fh, len, SEEK_CUR);
           //skip/or read len bytes
        }
        extralen-=len;

        // Read Layer name
        len = fgetc(fh);
        extralen-=1;
        fread( &LayerInfo[i].layername, len, 1, fh );
        extralen-=len;
        fseek(fh, extralen, SEEK_CUR);
    }

    layer--;

    // p.s. below I borrowed the variables of PsdInfo to unpack the layer data

    // Skip layer image data before
    long len = 0;

    for (i = 0; i < layer; i++)
    {
        for (j = 0; j < LayerInfo[i].channels; j++)
            len += LayerInfo[i].channelDataLength[j];
    }

    fseek(fh, len, SEEK_CUR);

    // Read this layer
    PsdInfo.Height   = LayerInfo[i].bottom - LayerInfo[i].top;
    PsdInfo.Width    = LayerInfo[i].right - LayerInfo[i].left;
    PsdInfo.Channels = LayerInfo[i].channels;

    // Allocate image data
    delete [] PsdInfo.Pixels;
    PsdInfo.Pixels = NULL;

    len = PsdInfo.Height * PsdInfo.Width;

    if (len == 0)   // empty layer!
        return PSDERR_OK;

    len++; len >>= 1; len <<= 1;    // if len is odd then add pad byte

    len <<= 2;      // for 4 channels

    PsdInfo.Pixels = (PSDFMT::Pixel *) new char [len];
    if (PsdInfo.Pixels == NULL)
        return PSDERR_MEMORYOUT;

    memset(PsdInfo.Pixels, 0xFF, len);

    j = UnpackLayerData(fh, &LayerInfo[i]);

    //delete [] PsdInfo.Pixels;
    //PsdInfo.Pixels


    return j;
}

int UnpackLayerData(FILE *fh, LAYERRECORD *pLayer)
{
    int  Default[4] = {0, 0, 0, 255};
    int  *chn       = pLayer->channelID;    // mapping to R,G,B,A
    int  PixelCount = PsdInfo.Width * PsdInfo.Height;
    int  channels   = pLayer->channels;

    if (channels > 4) channels = 4;

    for (int c = 0; c < channels; c++)
    {
        // Find out if the data is compressed,
        // 0: Raw data, 1: RLE compressed
        if ((PsdInfo.Compression = fgetw(fh)) > 1)
        {
            delete [] PsdInfo.Pixels;

            PsdInfo.Pixels = NULL;

            return PSDERR_COMPRESSMODE;
        }

        if (PsdInfo.Compression)
        {
            int  pn = 0;
            int  channel = chn[c];

            if (channel == -1)     channel = 3;     // treat it as alpha channel
            else if (channel < 3)  channel = 2 - channel;

            // Skip the byte counts for all the scan lines in this channel,
            // which each count stored as a 2-bytes value
            fseek(fh, PsdInfo.Height * 2, SEEK_CUR);

            if (channel >= PsdInfo.Channels)
            {
                for (pn = 0; pn < PixelCount; pn++)
                {
                    PsdInfo.Pixels[pn].Channel[channel] = Default[channel];
                }
            }
            else
            {
                int  count = 0;

                while (count < PixelCount)
                {
                    int  len = fgetc(fh);

                    if (len == 128)
                    {
                    }
                    else if (len < 128)
                    {
                        len++;
                        count += len;
                        while (len)
                        {
                            PsdInfo.Pixels[pn].Channel[channel] = fgetc(fh);
                            pn++;
                            len--;
                        }
                    }
                    else if (len > 128)
                    {
                        len ^= 0x0FF;
                        len += 2;
                        unsigned char val = fgetc(fh);

                        count += len;
                        while (len)
                        {
                            PsdInfo.Pixels[pn].Channel[channel] = val;
                            pn++;
                            len--;
                        }
                    }
                }   // end of count while
            }   // end of channel else
        }
        else // !PsdInfo.Compression
        {
            int  channel = chn[c];

            if (channel == -1)     channel = 3;     // treat it as alpha channel
            else if (channel < 3)  channel = 2 - channel;

            if (channel > PsdInfo.Channels)
            {
                for (int pn = 0; pn < PixelCount; pn++)
                {
                    PsdInfo.Pixels[pn].Channel[channel] = Default[channel];
                }
            }
            else
            {
                for (int n = 0; n < PixelCount; n++)
                {
                    PsdInfo.Pixels[n].Channel[channel] = fgetc(fh);
                }
            }
        }
    }   // end of c loop

    return PSDERR_OK;
}

int UnpackImageData(FILE *fh)
{
    int  Default[4] = {0, 0, 0, 255};
    int  chn[4]     = {2, 1, 0, 3};     // mapping to B,G,R,A order
    int  PixelCount = PsdInfo.Width * PsdInfo.Height;

    // Find out if the data is compressed,
    // 0: Raw data, 1: RLE compressed
    if ((PsdInfo.Compression = fgetw(fh)) > 1)
    {
        delete [] PsdInfo.Pixels;

        PsdInfo.Pixels = NULL;

        return PSDERR_COMPRESSMODE;
    }

    if (PsdInfo.Compression)
    {
        // Skip the byte counts for all the scan lines (height * channels),
        // which each count stored as a 2-bytes value
        fseek(fh, PsdInfo.Height * PsdInfo.Channels * 2, SEEK_CUR);

        for (int c = 0; c < 4; c++)
        {
            int  pn = 0;
            int  channel = chn[c];

            if (channel >= PsdInfo.Channels)
            {
                for (pn = 0; pn < PixelCount; pn++)
                {
                    PsdInfo.Pixels[pn].Channel[channel] = Default[channel];
                }
            }
            else
            {
                int  count = 0;

                while (count < PixelCount)
                {
                    int  len = fgetc(fh);

                    if (len == 128)
                    {
                    }
                    else if (len < 128)
                    {
                        len++;
                        count += len;
                        while (len)
                        {
                            PsdInfo.Pixels[pn].Channel[channel] = fgetc(fh);
                            pn++;
                            len--;
                        }
                    }
                    else if (len > 128)
                    {
                        len ^= 0x0FF;
                        len += 2;
                        unsigned char val = fgetc(fh);

                        count += len;
                        while (len)
                        {
                            PsdInfo.Pixels[pn].Channel[channel] = val;
                            pn++;
                            len--;
                        }
                    }
                }   // end of count while
            }   // end of channel else
        }   // end of c loop
    }
    else // !PsdInfo.Compression
    {
        for (int c = 0; c < 4; c++)
        {
            int  channel = chn[c];

            if (channel > PsdInfo.Channels)
            {
                for (int pn = 0; pn < PixelCount; pn++)
                {
                    PsdInfo.Pixels[pn].Channel[channel] = Default[channel];
                }
            }
            else
            {
                for (int n = 0; n < PixelCount; n++)
                {
                    PsdInfo.Pixels[n].Channel[channel] = fgetc(fh);
                }
            }
        }   // end of c loop
    }

    return PSDERR_OK;
}

bool ConvertToRaw(int pixelFmt, unsigned char **lplpImage, unsigned char **lplpAlpha)
{
    unsigned char *lpImage;
    unsigned char *lpAlpha;
    int            nPixelCount = PsdInfo.Width * PsdInfo.Height;
    int            nPixelDepth;

    if ((pixelFmt == PIXELFMT_555) || (pixelFmt == PIXELFMT_565))
        nPixelDepth = 2;
    else if (pixelFmt == PIXELFMT_888)
        nPixelDepth = 3;
    else
        return false;

    // Rap alpha channel
    if (lplpAlpha != NULL)
    {
        *lplpAlpha = new unsigned char [nPixelCount + 4];

        if (*lplpAlpha != NULL)
        {
            lpAlpha = *lplpAlpha;

            *(unsigned short *) (lpAlpha)     = PsdInfo.Width;
            *(unsigned short *) (lpAlpha + 2) = PsdInfo.Height;

            lpAlpha += 4;

            // Copy alpha chanel
            for (int i = 0; i < nPixelCount; i++)
                *(lpAlpha++) = PsdInfo.Pixels[i].Channel[3];
        }
    }

    // Allocate raw image data
    *lplpImage = new unsigned char [nPixelCount * nPixelDepth + 4];
    if (*lplpImage == NULL)
        return false;

    lpImage = *lplpImage;

    *(unsigned short *) (lpImage)     = (PsdInfo.Width * nPixelDepth);
    *(unsigned short *) (lpImage + 2) = PsdInfo.Height;

    lpImage += 4;

    // Convert to 16/24 bits raw data
    unsigned char r, g, b;

    for (int i = 0; i < nPixelCount; i++)
    {
        // Get R,G,B values of pixel from RGB channels
        b = PsdInfo.Pixels[i].Channel[0];
        g = PsdInfo.Pixels[i].Channel[1];
        r = PsdInfo.Pixels[i].Channel[2];

        // Put it to new pixel
        if (pixelFmt == PIXELFMT_555)
        {
            r >>= 3;    // shrink 8 bits to 5 bits
            g >>= 3;
            b >>= 3;

            *(lpImage++) = (g << 5) | b;        // low byte
            *(lpImage++) = (r << 2) | (g >> 3); // hight byte
        }
        else if (pixelFmt == PIXELFMT_565)
        {
            r >>= 3;    // shrink 8 bits to 5 bits
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
    }

    return true;
}

/*
BOOL DrawToDC(HDC hdc, int Xpos, int Ypos)
{
    BITMAPINFO BitmapInfo;

    ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = PsdInfo.Width;
    BitmapInfo.bmiHeader.biHeight = -PsdInfo.Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
//  LockDC();
    StretchDIBits(hdc,
                  Xpos, Ypos, PsdInfo.Width, PsdInfo.Height,
                  0, 0, PsdInfo.Width, PsdInfo.Height,
                  PsdInfo.Pixels, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
//  UnlockDC();
    return TRUE;
}
*/

int GetPSDLayerNum(FILE *fh, int *w, int *h)
{
    int  layers = 0;

    int  ret;
    if( ( ret = LoadPSDHeader( fh ) )!=PSDERR_OK )
        return ret;

    if (PsdInfo.MiscInfoLength)
    {
        // Read layer info
        int LayersInfoLength = fgetdw(fh);

        layers = (short) fgetw(fh);
    }

    return abs(layers);
}

int GetPSDLayerName(FILE *fh, int layer, char* layername, int len, PSDLAYERTYPE& lt )
{
    lt=PSDLAYER_UNKNOW;
    int  ret;
    if( ( ret = LoadPSDHeader( fh ) )!=PSDERR_OK )
        return ret;

    if (PsdInfo.MiscInfoLength)
    {
        if (layer == 0)     // skip these data
            fseek(fh, PsdInfo.MiscInfoLength, SEEK_CUR);
        else
        {
            // Read layer info
            int LayersInfoLength = fgetdw(fh);

            ret = LoadLayer(fh, layer);

            if (ret != PSDERR_OK)
                return ret;

            if( layername != NULL)
            {
                strcpy( layername, LayerInfo[layer - 1].layername );
            }

            // TODO: Pin
            // 我也知道這樣應該不對，但先頂著
                    
            if( ( LayerInfo[layer - 1].top==0 ) &&
                ( LayerInfo[layer - 1].left==0 ) &&
                ( LayerInfo[layer - 1].right==0 ) &&
                ( LayerInfo[layer - 1].bottom==0 ) )
            {
                if(((LayerInfo[layer - 1].Flags)&(1<<4)) != 0)
                    lt = PSDLAYER_FOLDER;
                else
                    lt = PSDLAYER_IMAGE;                    
            }
            else
                lt = PSDLAYER_IMAGE;
        }
    }
    return PSDERR_OK;
}



