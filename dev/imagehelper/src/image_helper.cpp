#include "image_helper.h"
#include "lode_png.h"
#include "image_processer.h"

namespace image_helper
{
    int LoadPNG( const char* filename,                // file handle of .TGA
                 unsigned char** lplpImage,           // return 8/16/24 bits raw image
                 unsigned& w, unsigned& h,            //
                 PNG_PIXELFMT pixelFmt,
                 unsigned bitdepth )                  // pixel format of raw image u want
    {
        std::vector<unsigned char> image; //the raw pixels
        int ret = lodepng::decode( image, w, h, filename, ( LodePNGColorType )pixelFmt, bitdepth );
        if( ret == 0 )
        {
            ( *lplpImage ) = new unsigned char[image.size()];
            memcpy( ( *lplpImage ), ( &image[0] ), image.size() );
            ret = image.size();
        }
        else
        {
            ret = 0 - ret;
        }
        image.clear();
        return ret;
    }
    
    
    void RGBAToGray( _IN dword_t& pixel )
    {
        ImageProcesser::RGBAToGray( pixel );
    }
}