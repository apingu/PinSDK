#ifndef IMAGEFILES_H
#define IMAGEFILES_H

#include <pf-def.h>

namespace image_helper
{
    //////////////////////////////////////////////////////////////////////////
    // PNG
    typedef enum PNG_PIXELFMT
    {
        PIXELFMT_GREY       = 0,             /*greyscale: 1,2,4,8,16 bit*/
        PIXELFMT_RGB        = 2,             /*RGB: 8,16 bit*/
        PIXELFMT_PALETTE    = 3,             /*palette: 1,2,4,8 bit*/
        PIXELFMT_GREY_ALPHA = 4,             /*greyscale with alpha: 8,16 bit*/
        PIXELFMT_RGBA       = 6              /*RGB with alpha: 8,16 bit*/
    } ;

    // if success return buffer size else return error no
    int LoadPNG(const char* filename,        // file handle of .TGA
                unsigned char** lplpImage,   // return 8/16/24 bits raw image
                unsigned& w, unsigned& h,    //
                PNG_PIXELFMT pixelFmt,
                unsigned bitdepth = 8);      // pixel format of raw image u want



    //////////////////////////////////////////////////////////////////////////
    // Image processing
    void RGBAToGray(_INOUT dword_t& pixel);
}

#endif //IMAGEFILES_H