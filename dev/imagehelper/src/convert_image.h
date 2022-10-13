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
#ifndef CONVERTIMAGE_H
#define CONVERTIMAGE_H

#include <vector>
#include <graphic2d.h>

#define _MAX_STRING 500

//#define AddResultMessage(s)      DebugMessage( s, "Conversion" )
#define CLEAR_MSG()     DebugDelf()
#define END_PROGRAM(s)  return s  


#ifndef TRACE
#define TRACE(s)          printf(s);
#endif

#define OPEN_FILE_FAIL 0;

#define CONV_LOAD_MAP_FAIL     -9


extern char OutDebugString[_MAX_STRING];


//////////////////////////////////////////////////////////////////////////////////////////////////
int BMPToPin( const char*     alphafilename, 
              const char*     srcfilename, 
              const char*     destfilename,
              PixelFormat     mode       = PixelFormat::R5G5B5, 
              bool            isTrim     = true,
              int             gradation  = 256, 
              bool            isCompress = true );

int TGAToPin( const char*    srcfilename , 
              const char*    destfilename, 
              PixelFormat    mode        = PixelFormat::R5G5B5,
              bool           isTrim      = true,
              int            gradation   = 256,
              bool           isCompress = true );


class ConvertImageFunctor
{
public:
    //ConvertImageFunctor(){}
    //~ConvertImageFunctor(){}
    //virtual void OnNewFile(const char* targetfname) = 0;
    virtual void operator()(const char* targetfname)=0;
};


int  PsdToForm( const char*          srcfilename,
                const char*          destfilename,
                PixelFormat          mode        = PixelFormat::R5G5B5,
                   bool                 isTrim      = true,
                int                  gradation   = 256,
                bool                 isCompress = true,
                bool                 isDelImage = true,
                ConvertImageFunctor* pFunctor   = NULL);



//int  PinToPinz( const char* Pinpath, const char* Pinzpath );
int  PinToElem( const char* Pinpath, const char* Pelmpath );



#endif
