//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#include "convert_image.h"

char OutDebugString[_MAX_STRING];

UINT Switch_Pixel_Format( PixelFormat mode )
{
    switch(mode)
    {
    case PixelFormat::R5G6B5:   return  4;//PIXELFMT_565;
    case PixelFormat::R5G5B5:   return  2;//PIXELFMT_555;
    //case PixelFormat::556:  return  PIXELFMT_556;
    //case PixelFormat::655:  return  PIXELFMT_655;
    case PixelFormat::R8G8B8:   return  8;// PIXELFMT_RGB;
    case PixelFormat::R8G8B8A8: return 16;// PIXELFMT_RGBA;
    case PixelFormat::A8R8G8B8: return 32;// PIXELFMT_BGRA;
    default: return (UINT)PixelFormat::UNKNOW;
    }
}