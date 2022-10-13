#ifndef IMAGEPROCESSER_H
#define IMAGEPROCESSER_H

#include <pf-def.h>

namespace ImageProcesser
{
    inline void RGBAToGray(_IN dword_t& pixel)
    {
        unsigned char* pixA = (unsigned char*)&pixel;
        int gray= (0.299 * pixA[0]) + (0.587 * pixA[1]) + (0.114 * pixA[2]);
        if(gray > 255)
            gray = 255;
        pixA[0] = pixA[1] = pixA[2] = gray;
    }
}
#endif //IMAGERY_H