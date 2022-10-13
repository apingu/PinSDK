//
//
//  3D game texture
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
//                                               Copyright    (C)    2001    Pin
//
#include "Texture.h"
#include <vector>


MEM::String CPTexture::m_folder;

namespace VfTexture
{
    std::vector<Pstr> fname;
};


#if defined( _XGLKERNEL_)|| defined( _WGLKERNEL_)
//#include "..\..\Template\IMap.h"
#include <TMap.h>
#include "..\LoadImage.h"


struct Imagerwfunc
{
    VfImageCallBackRead     read;
    VfImageCallBackWrite    write;
};


pt::Map<Pcstr, Imagerwfunc*> _gvfimagefunc;
#endif

#ifdef _RWKERNEL_
RwImage* PinImageRead( const RwChar* imageName )
{
    return 0;
}
#endif

//=============================================================================
//  Name :
//  Desc : Constructor
//=============================================================================

CPTexture::CPTexture()
{
#ifdef _GLKERNEL_
    m_pTexture = NULL;
    memset( m_2Dtexture, 0, sizeof( m_2Dtexture ) );
#endif
}


//=============================================================================
//  Name :
//  Desc : Distructor
//=============================================================================
CPTexture::~CPTexture()
{
    Destroy();
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void CPTexture::Load( Pcstr fname )
{
    m_fname.cat( fname );
#ifdef _RWKERNEL_
    m_pTexture = RwTextureRead( fname, NULL );
    RwRaster* ras   = RwTextureGetRaster( m_pTexture );
    width = ras->width;
    height = ras->height;
    depth = ras->depth;
    format = ras->cFormat;
    pixels = ( Pbyte * ) ras->cpPixels;
    palette = ( Puint * ) ras->palette;
#endif
    return;
}

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void CPTexture::Destroy( void )
{
#if defined( _XGLKERNEL_)|| defined( _WGLKERNEL_)
    for( Puint i = 0;i < _gvfimagefunc.size();i++ )
        delete _gvfimagefunc( i );

    if( m_pTexture != NULL )
        RwTextureDestroy( m_pTexture );
#endif
    m_fname.clear();
    return ;
}



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pbool CPTexture::RegImageFunc( Pcstr ext,
                               VfImageCallBackRead imageRead,
                               VfImageCallBackWrite imageWrite )
{
#ifdef _RWKERNEL_
    return RwImageRegisterImageFormat( ext, imageRead, imageWrite );
#endif
#if defined( _XGLKERNEL_)|| defined( _WGLKERNEL_)
    Imagerwfunc* func   = new Imagerwfunc;
    func->read = imageRead;
    func->write = imageWrite;
    _gvfimagefunc.push_back( ext, func );

#endif
    return TRUE;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pint CPTexture::SetLocation( Pcstr folder )
{
#ifdef _RWKERNEL_

	m_folder = folder;
    //path = RsPathnameCreate(RWSTRING("./textures/"));

    //renderwave set texture folder last path char must be //
    char correctfolder[ _MAX_PATH ];
    if( folder[ 0 ] == '\\' )
        strcpy( correctfolder, folder + 1 );
    else
        strcpy( correctfolder, folder );

    int len = strlen( folder );
    if( folder[ len - 1 ] != '\\' )
    {
        if( len == _MAX_PATH )
            return -1;
        correctfolder[ len ] = '\\';
        correctfolder[ len + 1 ] = '\0';
    }
    RwImageSetPath( correctfolder );
    m_fname = folder;
#endif
    return 1;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CPTexture::SetImage( tImage image )
{
#ifdef _RWKERNEL_
    if( m_pTexture != NULL )
        RwTextureDestroy( m_pTexture );

    RwImage* rwimg  = RwImageCreate( image.width, image.height, image.depth );
    //rwimg->cpPixels = (Pbyte*)image.pixels;
    //rwimg->palette  = (RwRGBA*)image.palette;
    RwRaster* ras   = RwRasterCreate( image.width,
                                      image.height,
                                      image.depth,
                                      rwRASTERTYPETEXTURE ) ;
    RwRasterLock( ras, 0, rwRASTERLOCKWRITE );
    ras = RwRasterSetFromImage( ras, rwimg );
    RwRasterUnlock( ras );
    m_pTexture = RwTextureCreate( ras );
#endif

    return;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Puint CPTexture::GetWidth( void )
{
#ifdef _RWKERNEL_
    RwRaster* ras   = RwTextureGetRaster( m_pTexture );
    width = RwRasterGetWidth( ras );
#endif
    return width;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Puint CPTexture::GetHeight( void )
{
#ifdef _RWKERNEL_
    RwRaster* ras   = RwTextureGetRaster( m_pTexture );
    height = RwRasterGetHeight( ras );
#endif
    return  height;
}

//=============================================================================
//  Name :
//  Desc :  arg3, 4 ( GL_LINEAR
//                    GL_NEAREST
//                    GL_NEAREST_MIPMAP_NEAREST  => only for GL_TEXTURE_MIN_FILTER
//                    GL_NEAREST_MIPMAP_LINEAR   => only for GL_TEXTURE_MIN_FILTER
//                    GL_LINEAR_MIPMAP_NEAREST   => only for GL_TEXTURE_MIN_FILTER
//                    GL_NEAREST_MIPMAP_LINEAR   => only for GL_TEXTURE_MIN_FILTER
//
//
//          arg5    ( wrap = GL_CLAMP or GL_REPERT )
//=============================================================================
/*
GLuint CPTexture::Load2D( char* filename, int id, bool Alpha, GLenum minfilter, GLenum magfilter, GLenum wrap)
{
    int Status=FALSE;                                                // Status Indicator
    tImage *TextureImage[1];
    
    memset(TextureImage,0,sizeof(void *)*1);                          // Set The Pointer To NULL
    
    if(strstr(filename, ".jpg"))
    {
        //TextureImage[0] = LoadJPG(strFileName);
    }
    // If the file is a tga, load the tga and store the data in pImage
    else if(strstr(filename, ".tga"))
    {
        //TextureImage[0] = LoadTGA(strFileName);
    }
    // If the file is a bitmap, load the bitmap and store the data in pImage
    else if(strstr(filename, ".bmp"))
    {
        TextureImage[0] = LoadBMPTexture(filename);
    }

    if( TextureImage[0] )
    {
        Status=TRUE;                                                  // Set The Status To TRUE
        glGenTextures  ( _VMC_MAX_TEXTURE, &m_2Dtexture[id]);             // Create The Texture

        // Typical Texture Generation Using Data From The Bitmap
        glBindTexture  ( GL_TEXTURE_2D, m_2Dtexture[id] );
        // Generate The Texture
        if( Alpha )
            glTexImage2D   ( GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        else
            glTexImage2D   ( GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB,  GL_UNSIGNED_BYTE, TextureImage[0]->data);

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
        //glTexEnvi( GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

    if (TextureImage[0])                            // If Texture Exists
    {
        if (TextureImage[0]->data)                  // If Texture Image Exists
            SAFE_DELETE( TextureImage[0]->data ); 
        SAFE_DELETE( TextureImage[0] );
    }
    return Status;                                  // Return The Status
}*/



