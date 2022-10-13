//Vfw3D.h 
//
//   Visual framework 3D engine
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
//                                               Copyright    (C)    2000    Pin
//
#ifndef VFW3D_H
#define VFW3D_H

#include <math.h>

#include <VfwKernel.h>
//#include <Graphic3D.h>


//#include "Particle.h"
#include "Light.h"
#include "Camera.h"

/*
#ifdef RWCORE_H
    #include "RWCamera.h"
    typedef V3DCamera V3DCamera
#else
    #ifdef GL_H
        #include "V3DCamera.h"
        typedef V3DCamera V3DCamera
    #endif
#endif
*/


///////////////////////////////////////////////////////////////////////////////
//  3D

class VFW3D// : public CPWindow
{
    void*           m_DisplayID;
    Puint           m_depth;

#if defined(_WGLKERNEL_) || defined(_XGLKERNEL)
    HGLRC           m_hRC;                          // Permanent Rendering Context
#endif
#ifdef _WGLKERNEL_
    HDC             m_hDC;                          // Private memory graphic context
    HFONT           m_hFont;                        // a HFONT object describing the currently selected font
#endif


protected:

    virtual Pint    Initialize3D( void* displayID );    // All Setup For OpenGL Goes Here
    virtual Pbool   SetPixelFormat( Pint bbp );
    virtual Pbool   SetRenderContext( Pvoid );

public:

    VFW3D();
    ~VFW3D();

    //std::vector< VfCamera >     Camera;           
    //std::vector< VfLighting >   Light;
    //VfParticle      Particle;

    void* GetDisplay( Pvoid )
    {
        return m_DisplayID;
    }           
    void*       GetCanvas( Pvoid );

    Pbool       Open( void* displayID, Pint bitsPerPixel = 16 );

    Pvoid       Destroy( Pvoid );                                            // Properly Kill The Window

    //Pvoid       SetRenderFunc     ( int (__cdecl *RenderFunc ) ( Pvoid ) ); 


    //OpenGL light function
    //Pvoid       Turn_On_Light     ( Pword num = 0XFF );
    //Pvoid       Turn_Off_Light    ( Pword num = 0XFF );


    //Setting
    void        SetZTest( bool open = true );//sort

    Pint        Show( const VfCamera* camera );

    //OpenGL function
    //Pvoid       BeginDraw         ( int shape )                                            { glBegin( shape ); }
    //Pvoid       EndDraw           ( Pvoid )                                                { glEnd(); }
    //Pvoid       LoadIdentity      ( Pvoid )                                                { glLoadIdentity(); }
    //Pvoid       Clear             ( int mode = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) { glClear( mode ); }     // Clear The Screen And The Depth Buffer
    //Pvoid       Color             ( float R, float G, float B)                             { glColor3f( R, G, B ); }
    //Pvoid       Color             ( float R, float G, float B, float A )                   { glColor4f( R, G, B, A ); }
    //Pvoid       Translate         ( float x, float y, float z )                            { glTranslatef(x, y, z); }
    //Pvoid       Vertex            ( float x, float y, float z )                            { glVertex3f( x, y, z ); }
    //Pvoid       Rotatef           ( float rtri, float x, float y, float z )                { glRotatef(rtri,x , y, z); }

    VfMetrics*  GetMetrics( VfMetrics* ginfo );

    // font function 
    Pvoid       Create_Font( Pcstr font_type, int s_w, int s_h );
    Pvoid       Destroy_Font( void );
    Pvoid       Text( int x, int y, COLORTYPE col, Pcstr string );
};




#endif
