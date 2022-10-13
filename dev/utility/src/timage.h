//tImage.h
//
//
//  base image struct
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    1999    Pin
//////////////////////////////////////////////////////////////////////////
#pragma once


// These defines are used to tell us about the type of TARGA file it is
//#define TGA_RGB    2      // This tells us it's a normal RGB (really BGR) file
//#define TGA_A      3      // This tells us it's a ALPHA file
//#define TGA_RLE   10      // This tells us that the targa is Run-Length Encoded (RLE)

// This is our image structure for our targa data
struct tImage
{
    unsigned int    width;              // The width of the image in pixels
    unsigned int    height;             // The height of the image in pixels
    unsigned int    depth;

    unsigned char*  pixels;             // The image pixel data
    unsigned int*   palette;

    int             format;             // The channels in the image (3 = RGB : 4 = RGBA)
};