//texture.h header file for texture.cpp
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
//                                               Copyright    (C)    2001    Pin
//
#ifndef TEXTURE_H
#define TEXTURE_H

//#include <Kernel3D.h>
#include "VfwGraphic.h"  //3DEngine
#include <tString.h>
#include <tImage.h>


//#define VF_TEXTURELOADFAIL    -1

///////////////////////////////////////////////////////////////////////////////

class CPTexture : public tImage
{
private:

	MEM::String         m_fname;
	static MEM::String  m_folder;

#ifdef _GLKERNEL_
    GLuint      m_Using_filter;
    GLuint      m_1Dtexture[ _VMC_MAX_TEXTURE ];  // Storage For One Texture
    GLuint      m_2Dtexture[ _VMC_MAX_TEXTURE ];  // Storage For One Texture    
    GLuint      m_3Dtexture[ _VMC_MAX_TEXTURE ];  // Storage For One Texture
#endif

public:

#ifdef _RWKERNEL_
    RwTexture*  m_pTexture;
    operator  RwTexture*()
    {
        return m_pTexture;
    }
#endif

    CPTexture();
    ~CPTexture();

	char*   filename( void ){ return m_fname; }
	char*   folder( void )  { return m_folder; }

    void    Destroy( void );
    void    Load( Pcstr fname );

    void    SetImage( tImage image );
    Puint   GetWidth( void );
    Puint   GetHeight( void );

    Pint    SetLocation( Pcstr folder );
    Pbool   RegImageFunc( Pcstr ext,
                          VfImageCallBackRead imageRead,
                          VfImageCallBackWrite imageWrite );
};

#endif

