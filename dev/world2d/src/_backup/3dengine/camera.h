//Camera.h
//
//
//    3D camera
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
#ifndef CAMERA_H
#define CAMERA_H

#include <pdef.h>  // Pin/Base
#include <VfwGraphic.h>

#include <tImage.h>// Pin/Base
#include <Model3d.h>

namespace LGR
{
class Camera : public CPAxis
{
    //CPAxis      m_Axis;
    VfBSphere   m_bshpere;

#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    //VfAxis      m_axis;
    tImage      m_fbuffer;
    tImage      m_zbuffer;
    LG::Point   m_ViewOffset;
    CPSize      m_ViewSize;
    float       m_nearPlane;
    float       m_farPlane;
#endif


#ifdef _RWKERNEL_
public:
	RwCamera*   m_pCamera;
	RwCamera&   operator=( RwCamera* model );
    operator    RwCamera*() { return m_pCamera; }

    Camera( RwCamera* Camera );
    void        SetSub( Camera* subCamera = NULL );
	RwRaster*   CreateCrossHair( void );
#endif


    int                 m_shakeTime;
    bool                m_shaking;
    static const float  m_kDefaultAspectRatio;

public:

    Camera();
    ~Camera();


    Pint        Create( Puint width = 800,
                        Puint height = 600,
                        float nearplace = 1.0f,
                        float farplace = 50000.0f,
                        Pbool  zBuffer = TRUE );


    void        Destroy( void );


    Camera*     NewClone( void );
	void        DelClone( Camera* camera );

    /////////////////////////////////////
    //
    void        SetViewPos( Pint posX = 0, Pint posY = 0 );       //canvas position on windows
    int         SetViewBox( Puint width = 800,                    //canvas size on windows
                            Puint height = 600,
                            Puint depth = 0 );                    // Resize And Initialize The GL Window
	int         SetClipPlanes( float nearplane = 1.0f,
                               float farplane = 50000.0f,
                               double fov = 45.0f );

    void        SetBSphere    ( VfBSphere vfs );
	void        SetFOV        ( float fovH, float fovV );                          // by Karl.
    void        SetFogDistance( float distance = 5000.0f );



	VfBSphere   GetBSphere      ( void );
	LG::Boxi    GetViewBox      ( void );
	LG::Point   GetViewPos      ( void );                              //canvas position on windows
	tImage      GetfBuffer      ( void );
    tImage      GetzBuffer      ( void );
	float       GetClipFarPlane ( void );
	float       GetClipNearPlane( void );

   
    void        Size( VfRect* rect, float viewWindow, float aspectRatio );
    LG::Line3df ScreenPixelRay( LG::Point pos );


    /////////////////////////////////////
    //camera control for karl
    //void        Rotate( VfVertex3f* pos, float angle );
	void        Erect( void );
    bool        SnapTo( CPModel* model, float distance );
    void        Zoom( float power );
    void        LookAt( const LG::Vertex3f pos );

    // Camera Moves
	void        Dolly( float distance );                                    // Move camera forward and backward.
    void        Move( const LG::Vertex3f pos );
    //void        MoveTo( VfVertex3f pos );
    void        Orbit( const LG::Vertex3f center, float angle, LG::Vector3f axis );   // Move camera around an specified axis of an object.


    // Camera Rotations: Yaw(pan), Pitch(tilt), Roll
    void        Pan( float angle );                               // Rotate camera around its own up-axis(yaw). Z
    void        Tilt( float angle );                              // Rotate camera around its own right-axis(pitch).X
    void        Roll( float angle );                              // Rotate camera around its own at-axis.Y


    /////////////////////////////////////
    //
    void        Clear( VfRGBA* colour = 0,
                       Pint clearMode = Vf_CLEAR_ZBUFFER |
                       Vf_CLEAR_IMAGEBUFFER );
    Pint        BeginUpdate( void );
    Pint        EndUpdate( void );

    void        Render( const VfImage* image, LG::Vertex2f site, float w = 1, float h = 1 );
    void        Render( const CPTexture* tex, LG::Vertex2f site, float w = 1, float h = 1 );
    void        Render( const CPModel* model );
    void        Render( const CPObject* object );

	


	/*
    // Camera Velocity and Acceleration
    void        DoCameraAccelMonte( float* delta,
                                    float* pos,
                                    float* camAccl,
                                    float td );
    void        DoCameraAccelPerFrame( float* delta,
                                       float* pos,
                                       float* camAccl,
                                       float accl );
    VfVertex3f  GetVelocity( void );
    void        SetVelocity( VfVertex3f velocity );
    VfVertex3f  GetAcceleration( void );
    void        SetAcceleration( VfVertex3f acceleration );

    // Shake
    void        SetShakeTime( float dt );
    void        Shake( int x, int y, float dt );
    void        StartShake( void );
	*/
};

};

#endif
