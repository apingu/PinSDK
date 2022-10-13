//Device.h 
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
#ifndef DEVICE3D_H
#define DEVICE3D_H

#include <math.h>
#include <VfwKernel.h>


//#include "Particle.h"
#include "Light.h"
#include "Camera.h"


namespace LGR
{
    ///////////////////////////////////////////////////////////////////////////////
    //  3D
    class Device3d// : public CPWindow
    {
        void*           m_DisplayID;
        UINT            m_depth;

#if defined(_WGLKERNEL_) || defined(_XGLKERNEL)
        HGLRC           m_hRC;                          // Permanent Rendering Context
#endif
#ifdef _WGLKERNEL_
        HDC             m_hDC;                          // Private memory graphic context
        HFONT           m_hFont;                        // a HFONT object describing the currently selected font
#endif

    protected:

        virtual int     Initialize3D( void* displayID );    // All Setup For OpenGL Goes Here
        virtual BOOL    SetPixelFormat( int bbp );
        virtual BOOL    SetRenderContext( void );

    public:

        Device3d();
        ~Device3d();

        //std::vector< VfCamera >     Camera;           
        //std::vector< VfLighting >   Light;
        //VfParticle      Particle;

        void* GetDisplay( void )
        {
            return m_DisplayID;
        }           
        void*       GetCanvas( void );

        BOOL        Open( void* displayID, int bitsPerPixel = 16 );

        void        Destroy( void );                                            // Properly Kill The Window

        //void       SetRenderFunc     ( int (__cdecl *RenderFunc ) ( void ) ); 


        //OpenGL light function
        //void       Turn_On_Light     ( Pword num = 0XFF );
        //void       Turn_Off_Light    ( Pword num = 0XFF );


        //Setting
        void        SetZTest( bool open = true );//sort

        int         Show( const Camera* camera );

        //OpenGL function
        //void       BeginDraw         ( int shape )                                            { glBegin( shape ); }
        //void       EndDraw           ( void )                                                 { glEnd(); }
        //void       LoadIdentity      ( void )                                                 { glLoadIdentity(); }
        //void       Clear             ( int mode = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) { glClear( mode ); }     // Clear The Screen And The Depth Buffer
        //void       Color             ( float R, float G, float B)                             { glColor3f( R, G, B ); }
        //void       Color             ( float R, float G, float B, float A )                   { glColor4f( R, G, B, A ); }
        //void       Translate         ( float x, float y, float z )                            { glTranslatef(x, y, z); }
        //void       Vertex            ( float x, float y, float z )                            { glVertex3f( x, y, z ); }
        //void       Rotatef           ( float rtri, float x, float y, float z )                { glRotatef(rtri,x , y, z); }

        VfMetrics*  GetMetrics( VfMetrics* ginfo );

        // font function 
        void        Create_Font( Pcstr font_type, int s_w, int s_h );
        void        Destroy_Font( void );
        void        Text( int x, int y, COLORTYPE col, Pcstr string );

    };

};//namespace L3DE


#endif
