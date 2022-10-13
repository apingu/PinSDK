#include "convert_image.h"
#include "convert_func.h"
#include "load_tga.h"
#include "save_pin.h"
#include <pf-debug.h>



//==========================================================================
//
//
//==========================================================================
int TGAToPin( const char*  srcfilepath ,
              const char*  destfilepath,
              PixelFormat     mode     ,
              bool         isTrim,
              int          gradation,
              bool         isCompress )
{
    int  tgaMode         = Switch_Pixel_Format( mode );
    //CLEAR_MSG();
    FILE* fh;
    FILE* fd;
    BYTE* lpImage = 0;
    BYTE* lpAlpha = 0;
    if( ( fd = fopen( destfilepath, "wb" ) ) == NULL )
    {
        Pf_ERROR( "TGAToPin: Open write file fail ...." );
        return -2;
    }
    if( ( fh = fopen( srcfilepath, "rb" ) ) == NULL )
    {
        Pf_ERROR( "TGAToPin: Open source file fail ...." );
        return -4;
    }
    int Result = LoadTGA( fh, &lpImage, &lpAlpha, tgaMode );
    if( Result != TGAERR_OK ) ///////////////////////
    { return CONV_LOAD_MAP_FAIL; }
    fclose( fh );
    if( lpImage == 0 )
    {
        Pf_ERROR( "TGAToPin: TGA file error ...." );
        return -10;
    }
    if( lpAlpha == NULL )
    {
        Pf_WARNING( "TGAToPin: open alpha file failure, continue will be no alpha image!!" );
    }
    if( SavePIN( fd,
                 lpImage,
                 lpAlpha,
                 mode,
                 isTrim,
                 gradation,
                 isCompress ) != 1 )
    {
        Pf_ERROR( "TGAToPin: BMP file error ...." );
    }
    fclose( fd );
    if( lpAlpha != NULL )
    { delete [] lpAlpha; }
    if( lpImage != NULL )
    { delete [] lpImage; }
    Pf_INFO( "Conversion successful !!!" );
    Pf_INFO( "=========================" );
    return 1;
}
