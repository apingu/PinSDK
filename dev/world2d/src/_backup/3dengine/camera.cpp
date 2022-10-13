//Camera.pp
//
//  3D camera
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
#include "Camera.h"
#include "aMath.h"
#include <Debug.h>

namespace LGR
{

#ifdef _RWKERNEL_
#include "ptrdata.h"
void tImageAssignRwRaster( tImage* image, RwRaster* raster );
#endif


#ifdef WIDE_SCREEN
const float Camera::km_DefaultAspectRatio = ( 16.0f / 9.0f );
#else
const float Camera::m_kDefaultAspectRatio = ( 4.0f / 3.0f );
#endif


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Camera::Camera()
{
    ID = -1;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    memSet( &m_fbuffer, 0, sizeof( m_fbuffer ) );
    memSet( &m_zbuffer, 0, sizeof( m_zbuffer ) );
    m_nearPlane = 0.0f;
    m_farPlane = 0.0f;
#endif
#ifdef _RWKERNEL_
    m_pCamera = NULL;
#endif
    //SetClipPlanes();
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Camera::~Camera()
{
    Destroy();
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
Pint Camera::Create( Puint width,
                       Puint height,
                       float nearplace,
                       float farplace,
                       Pbool zBuffer )
{
#ifdef _RWKERNEL_

	RwCamera* camera;

    if( camera = RwCameraCreate() )
    {
        //CPAxis::m_pMatrix = RwFrameCreate();
        RwCameraSetFrame( camera, ( RwFrame * ) CPAxis::Create() );
        RwCameraSetRaster( camera,
                           RwRasterCreate( width,
                                           height,
                                           0,
                                           rwRASTERTYPECAMERA ) );

        if( zBuffer )
        {
            RwCameraSetZRaster( camera,
                                RwRasterCreate( width,
                                                height,
                                                0,
                                                rwRASTERTYPEZBUFFER ) );
        }
        //if( ReSizeScene()
        if( !( zBuffer && RwCameraGetZRaster( camera ) ) )
        {
            Destroy();
            return NULL;
        }
    }

	operator= ( camera );

    /*
     * Error - so clean up...
     */


#endif

#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    if( zBuffer )
        glEnable( GL_DEPTH_TEST );
    else
        glDisable( GL_DEPTH_TEST );
#endif
    if( !SetViewBox( width, height ) )
    {
        Destroy();
        return NULL;
    }

    if( !SetClipPlanes( nearplace, farplace ) )
    {
        Destroy();
        return NULL;
    }

    //VfVertex3f cpos( 0, 0, 0 );
    ReTranslate( &LG::Vertex3f( 0, 0, 50 ) );
    LookAt( VfVertex3f( 0, 0, 0 ) );
    //CPAxis::Identity();
    return 1;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
void Camera::Destroy( void )
{
#ifdef _RWKERNEL_
    if( m_pCamera )
    {
        //Remove the camera and destroy the frame
        RwRaster* raster= RwCameraGetRaster( m_pCamera );
        if( raster )
        {
            RwCameraSetRaster( m_pCamera, 0x0 );
            RwRasterDestroy( raster );
            RwCameraSetRaster( m_pCamera, NULL );
        }

        RwRaster* zraster   = RwCameraGetZRaster( m_pCamera );
        if( raster )
        {
            RwCameraSetZRaster( m_pCamera, 0x0 );
            RwRasterDestroy( zraster );
            RwCameraSetZRaster( m_pCamera, NULL );
        }

        RwCameraDestroy( m_pCamera );
        m_pCamera = NULL;
    }
#endif
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    memSet( &m_fbuffer, 0, sizeof( m_fbuffer ) );
    memSet( &m_zbuffer, 0, sizeof( m_zbuffer ) );
#endif

    return;
}



//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
void Camera::Size( VfRect* rect, float viewWindow, float aspectRatio )
{
#ifdef _RWKERMNEL_
    if( m_pCamera )
    {
        RwVideoMode videoMode;
        static V3DRect origSize ={0, 0, 0, 0};

        if( origSize.w == 0 || origSize.h == 0 )
        {
            origSize.x = origSize.y = 0;
            origSize.w = RwRasterGetWidth( RwCameraGetRaster( m_pCamera ) );
            origSize.h = RwRasterGetHeight( RwCameraGetRaster( m_pCamera ) );
        }

        RwEngineGetVideoModeInfo( &videoMode, RwEngineGetCurrentVideoMode() );

        if( !rect )
        {
            static V3DRect r;

            rect = &r;

            /* 
             * Rect not specified - reuse current values...
             */
            rect->w = RwRasterGetWidth( RwCameraGetRaster( m_pCamera ) );
            rect->h = RwRasterGetHeight( RwCameraGetRaster( m_pCamera ) );
            rect->x = rect->y = 0;
        }

        /* 
         * For full screen applications, resizing the m_pCamera just doesn't
         * make sense, use the video mode size...
         */
        if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
        {
            rect->x = rect->y = 0;
            rect->w = videoMode.width;
            rect->h = videoMode.height;
        }

        if( ( rect->w > 0 ) && ( rect->h > 0 ) )
        {
            RwV2d vw;
            RwRaster* raster    = NULL;
            RwRaster* zRaster   = NULL;

            /*
             * In OpenGl & D3D8 you don't need to care about the rasters
            * of the cameras
             */
#if (defined(D3D8_DRVMODEL_H)) || (defined(OPENGL_DRVMODEL_H))
            raster = RwCameraGetRaster( m_pCamera );
            zRaster = RwCameraGetZRaster( m_pCamera );

            raster->width = rect->w;
            raster->height = rect->h;

            zRaster->width = rect->w;
            zRaster->height = rect->h;

#else
            /*
             * Destroy rasters...
             */
            raster = RwCameraGetRaster( m_pCamera );
            if( raster )
            {
                RwRasterDestroy( raster );

                raster = NULL;
            }

            zRaster = RwCameraGetZRaster( m_pCamera );
            if( zRaster )
            {
                RwRasterDestroy( zRaster );

                zRaster = NULL;
            }

            /*
             * Create new rasters... 
             */
            raster = RwRasterCreate( rect->w, rect->h, 0, rwRASTERTYPECAMERA );
            zRaster = RwRasterCreate( rect->w,
                                      rect->h,
                                      0,
                                      rwRASTERTYPEZBUFFER );

            if( raster && zRaster )
            {
                RwCameraSetRaster( m_pCamera, raster );
                RwCameraSetZRaster( m_pCamera, zRaster );
            }else
            {
                if( raster )
                {
                    RwRasterDestroy( raster );
                    raster = NULL;
                }

                if( zRaster )
                {
                    RwRasterDestroy( zRaster );
                    zRaster = NULL;
                }

                rect->w = origSize.w;
                rect->h = origSize.h;

                /* 
                 * Use default values... 
                 */
                raster = RwRasterCreate( rect->w,
                                         rect->h,
                                         0,
                                         rwRASTERTYPECAMERA );

                zRaster = RwRasterCreate( rect->w,
                                          rect->h,
                                          0,
                                          rwRASTERTYPEZBUFFER );

                RwCameraSetRaster( m_pCamera, raster );
                RwCameraSetZRaster( m_pCamera, zRaster );

                return;
            }
#endif

            /*
             * Figure out the view window... 
             */
            if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
            {
                /*
                 * Derive ratio from aspect ratio...
                 */
                vw.x = viewWindow;
                vw.y = viewWindow / aspectRatio;
            }else
            {
                /*
                 * In OpenGl & D3D8 you don't need to care about the rasters
                * of the cameras, just change the aspect ratio
                 */
#if (defined(D3D8_DRVMODEL_H)) || (defined(OPENGL_DRVMODEL_H))
#else
                rect->w = RwRasterGetWidth( RwCameraGetRaster( m_pCamera ) );
                rect->h = RwRasterGetHeight( RwCameraGetRaster( m_pCamera ) );
#endif
                /*
                 * Derive from pixel ratios... 
                 */
                if( rect->w > rect->h )
                {
                    vw.x = viewWindow;
                    vw.y = ( rect->h * viewWindow ) / rect->w;
                }else
                {
                    vw.x = ( rect->w * viewWindow ) / rect->h;
                    vw.y = viewWindow;
                }
            }

            RwCameraSetViewWindow( m_pCamera, &vw );
        }
    }
#endif

    return;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
Camera* Camera::NewClone( void )
{
    //RwV2d* vs = RwCameraGetViewWindow( m_pCamera );
    //Pbool zbuffer = (RwCameraGetZRaster( m_pCamera ) == NULL)? FALSE: TRUE;
    //Create( vs->x, vs->y, RwCameraGetNearClipPlane( m_pCamera ), RwCameraGetFarClipPlane( m_pCamera ),  zbuffer 0 
    Camera* camera= new Camera;
    memcpy( camera, this, sizeof( Camera ) );
#ifdef _RWKERNEL_
    camera->m_pCamera = RwCameraClone( m_pCamera );
#endif //_RWKERNEL_

    return camera;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::DelClone( Camera* camera )
{
#ifdef _RWKERNEL_
    RwCameraSetRaster( camera->m_pCamera, NULL );
    RwCameraSetZRaster( camera->m_pCamera, NULL );
    camera->m_pMatrix = NULL;
#endif
    delete camera;
}



//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
LG::Line3df Camera::ScreenPixelRay( LG::Point pos )
{
	LG::Line3df ray;
#ifdef _RWKERNEL_
    RwLine pixelRay;
    RwV2d pixel = { (float)pos.x, (float)pos.y };

    RwCameraCalcPixelRay(m_pCamera, &pixelRay, &pixel);
	ray.vertex[0] = LG::Vertex3f(pixelRay.start.x, pixelRay.start.y, pixelRay.start.z);
	ray.vertex[1] = LG::Vertex3f(pixelRay.end.x, pixelRay.end.y, pixelRay.end.z);
#endif
	return ray;}





//////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
/*
void Camera::Rotate( VfVertex3f* pos, float angle )
{
    if( empty() )
        return;
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)

    CPVector3f vNewView;

    // Get the view vector (The direction we are facing)
    CPVector3f vView= ( CPAxis::m_Matrix->at - CPAxis::m_Matrix->pos );

    // Calculate the sine and cosine of the angle once
    float cosTheta  = ( float ) cos( angle );
    float sinTheta  = ( float ) sin( angle );

    // Find the new x position for the new rotated point
    vNewView.x = ( cosTheta + ( 1 - cosTheta ) * pos->x * pos->x ) * vView.x;
    vNewView.x += ( ( 1 - cosTheta ) * pos->x * pos->y - pos->z * sinTheta ) * vView.y;
    vNewView.x += ( ( 1 - cosTheta ) * pos->x * pos->z + pos->y * sinTheta ) * vView.z;

    // Find the new y position for the new rotated point
    vNewView.y = ( ( 1 - cosTheta ) * pos->x * pos->y + pos->z * sinTheta ) * vView.x;
    vNewView.y += ( cosTheta + ( 1 - cosTheta ) * pos->y * pos->y ) * vView.y;
    vNewView.y += ( ( 1 - cosTheta ) * pos->y * pos->z - pos->x * sinTheta ) * vView.z;

    // Find the new z position for the new rotated point
    vNewView.z = ( ( 1 - cosTheta ) * pos->x * pos->z - pos->y * sinTheta ) * vView.x;
    vNewView.z += ( ( 1 - cosTheta ) * pos->y * pos->z + pos->x * sinTheta ) * vView.y;
    vNewView.z += ( cosTheta + ( 1 - cosTheta ) * pos->z * pos->z ) * vView.z;

    // Now we just add the newly rotated vector to our position to Set
    // our new rotated view of our camera.
    CPAxis::m_Matrix->at = CPAxis::m_Matrix->pos + vNewView;
#endif

#ifdef _RWKERNEL_
    VfVertex3f invCamPos;
    //RwFrame *CPAxis::m_pMatrix;
    RwMatrix* cameraMatrix;
    VfVertex3f camPos;

    //CPAxis::m_pMatrix = RwCameraGetFrame(m_pCamera);
    cameraMatrix = RwFrameGetMatrix( CPAxis::m_pMatrix );

    if( pos )
        camPos = *pos;
    else
        camPos = *RwMatrixGetPos( cameraMatrix );

    //camPos = ( (pos)? *pos: *RwMatrixGetPos(cameraMatrix) );

    RwV3dScale( &invCamPos, &camPos, -1.0f );

    //Translate the m_pCamera back to the rotation origin...
    RwFrameTranslate( CPAxis::m_pMatrix,
                      &( RwV3d ) invCamPos,
                      rwCOMBINEPOSTCONCAT );

    //Get the m_pCamera's AT vector and use this as the axis of rotation...
    RwMatrixRotate( cameraMatrix,
                    RwMatrixGetAt( cameraMatrix ),
                    angle,
                    rwCOMBINEPOSTCONCAT );


    //Translate the m_pCamera back to its original position...
    RwFrameTranslate( CPAxis::m_pMatrix,
                      &( RwV3d ) camPos,
                      rwCOMBINEPOSTCONCAT );
#endif

    return;
}
*/


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Camera::LookAt( const LG::Vertex3f pos )
{
    if( empty() )
        return;
#ifdef _RWKERNEL_

    RwMatrix* cameraLTM     = RwFrameGetLTM( CPAxis::m_pMatrix );
    RwMatrix* cameraMat     = RwFrameGetMatrix( CPAxis::m_pMatrix );
    const RwV3d* cameraPos  = &cameraLTM->pos;

    RwMatrix newMat;  // The matrix to transform the camera. 

    // Set up the at vector of the new matrix.
    RwV3dSub( &newMat.at, (RwV3d*)&pos, cameraPos );
    RwV3dNormalize( &newMat.at, &newMat.at );

    // Set up the right vector of the new matrix.
    RwV3dCrossProduct( &newMat.right, &newMat.at, &cameraLTM->up );

    if( newMat.right.x == 0 && newMat.right.y == 0 && newMat.right.z == 0 )
    {
        RwV3d temp  ={cameraLTM->up.x,
                      cameraLTM->up.y,
                      cameraLTM->up.z +
                      ( float )
                      pow( 0.1,
                           10 )};
        RwV3dCrossProduct( &newMat.right, &newMat.at, &temp );
    }

    RwV3dNormalize( &newMat.right, &newMat.right );

    // Set up the up vector of the new matrix.
    RwV3dCrossProduct( &newMat.up, &newMat.right, &newMat.at );
    RwV3dNormalize( &newMat.up, &newMat.up );

    RwV3dScale( &newMat.right, &newMat.right, -1.f );  //

    RwV3dAssign( &newMat.pos, cameraPos );  // Set up the position of the new matrix.

    RwMatrixOrthoNormalize( &newMat, &newMat );

    RwFrameTransform( CPAxis::m_pMatrix, &newMat, rwCOMBINEREPLACE );
    /*
        RwMatrix* cameraLTM = RwFrameGetLTM( CPAxis::m_pMatrix );  // Get the camera LTM matrix.
        //RwMatrix* cameraLTM = GetLTM();
        RwMatrix* cameraMat = RwFrameGetMatrix( CPAxis::m_pMatrix );
        //RwV3d* cameraPos = &cameraLTM->pos;  // Get the position of the LTM of the camera.
        RwV3d* cameraPos = RwMatrixGetPos( cameraMat );
        RwV3d* tarGetPos = (RwV3d*)&pos;
        //RwV3d  tarGetPos;
        //RwV3dSub( &tarGetPos, (RwV3d*)&pos, cameraPos );
        RwMatrix newMat;  // The matrix to transform the camera. 
        // Set up the at vector of the new matrix.
        RwV3dSub(&newMat.at, tarGetPos, cameraPos);
        RwV3dNormalize(&newMat.at, &newMat.at);
        // Set up the right vector of the new matrix.
        //RwV3dCrossProduct(&newMat.right, &newMat.at, &cameraLTM->up);
        RwV3dCrossProduct(&newMat.right, &newMat.at, &cameraLTM->up);
        if(newMat.right.x == 0 && newMat.right.y == 0 && newMat.right.z == 0)
        {
            RwV3d temp = {cameraLTM->up.x, cameraLTM->up.y, cameraLTM->up.z + (float)pow(0.1, 10)};
            RwV3dCrossProduct(&newMat.right, &newMat.at, &temp);
        }
        RwV3dNormalize(&newMat.right, &newMat.right);
        // Set up the up vector of the new matrix.
        RwV3dCrossProduct(&newMat.up, &newMat.right, &newMat.at);
        RwV3dNormalize(&newMat.up, &newMat.up);
        RwV3dScale(&newMat.right, &newMat.right, -1.f);  //
        RwV3dAssign(&newMat.pos, cameraPos);  // Set up the position of the new matrix.
        //RwMatrixUpdate(&newMat);
        RwMatrixOrthoNormalize(&newMat, &newMat);
        //RwMatrixOrthoNormalize(&newMat, &newMat);
        //RwFrameTransform(fCameraFrame, &newMat, rwCOMBINEREPLACE);
        //RwFrameTransform(fCameraFrame, &tmpM, rwCOMBINEPOSTCONCAT);
        //RwMatrixUpdate(GetMatrix());
        
        //Transform(newMat);
        //RwFrameTransform(CPAxis::m_pMatrix, &newMat, rwCOMBINEPOSTCONCAT );
        RwFrameTransform(CPAxis::m_pMatrix, &newMat, rwCOMBINEREPLACE);
        */
#endif
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::Erect( void )
{
	LG::Vector3f v(0, 1, 0 );
	LG::Vector3f v0;
	v0.x = At()->x;
	v0.y = At()->y;
	v0.z = At()->z;

	double a = VectorIncludedAngle( v0, v );
	PreRotate( &LG::_ZAxisf, (float)a );
}

//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
bool Camera::SnapTo( CPModel* model, float distance )
{
    if( !model )
        return false;

#ifdef _RWKERNEL_

    LG::Spheref sphere = model->GetBSphere();
	LG::Vertex3f ver;
    ver = sphere.center;
    //ver.z = ver.z - sphere.radius - distance;
    //sphere->center
    RwV3d* pos  = ( RwV3d* ) model->Position();

    ver.x = ( pos->x );
    ver.y = ( pos->y ) + sphere.radius;
    ver.z = ( pos->z ) + ( sphere.radius * distance );

    ReTranslate( &ver );
    LookAt( VfVertex3f( *pos ) );

    /*
       RwV3d vNewTarGet = {0,0,0};
       RwBBox* pBBox;
     
       //Get the camera's orientation matrix
       //RwFrame* pCameraFrame   = RwCameraGetFrame( m_pCamera );
       RwMatrix* pCameraMatrix = RwFrameGetMatrix( CPAxis::m_pMatrix );
       //Get the camera position and orientation
       RwV3d* vCameraPos    = RwMatrixGetPos   ( pCameraMatrix );
       RwV3d* vCameraAt     = RwMatrixGetAt    ( pCameraMatrix );
       RwV3d* vCameraUp     = RwMatrixGetUp    ( pCameraMatrix );
       RwV3d* vCameraRight  = RwMatrixGetRight ( pCameraMatrix );
       //Set the default orientation of the camera
       vCameraAt->x = 0;
       vCameraAt->y = 0;
       vCameraAt->z = 1;
       vCameraUp->x = 0;
       vCameraUp->y = 1;
       vCameraUp->z = 0;
       vCameraRight->x=1;
       vCameraRight->y=0;
       vCameraRight->z=0;
       //Set the position of the camera
       if( model->m_pClump )
       {
           RwFrame* pClumpFrame;
           RwMatrix* pClumpMatrix = 0x0;
           RwV3d* pvClumpPos;
           //Retrieve the orientation vectors from the matrix
           RwV3d* vClumpPos = RwMatrixGetPos ( pClumpMatrix );
           //Calculate the size of the object by iterating atomics' bounding boxes
           RwBBox BBox = {{-9999.0f,-9999.0f,-9999.0f},{9999.0f,9999.0f,9999.0}};
           pBBox = &BBox;
           model->BBox( (VfBBox*)pBBox );
           //Calculate the clump's center
           vNewTarGet.x = (BBox.sup.x + BBox.inf.x) * 0.5F;
           vNewTarGet.y = (BBox.sup.y + BBox.inf.y) * 0.5F;
           vNewTarGet.z = (BBox.sup.z + BBox.inf.z) * 0.5F;
           //Get the clump's frame
           pClumpFrame   = RpClumpGetFrame  ( model->m_pClump );
           if ( !pClumpFrame )
        {
               DebugMessage("CameraSnapTo: Failed to Get clump frame", "VFRW");
               return false;
        }
           //Get the frame's matrix
           pClumpMatrix  = RwFrameGetMatrix ( pClumpFrame );
           if ( !pClumpMatrix )
        {
               DebugMessage("CameraSnapTo: Failed to Get clump matrix", "VFRW");
               return false;
        }
           //Retrieve the orientation vectors from the clump
           pvClumpPos    = RwMatrixGetPos   ( pClumpMatrix );
           if ( !pvClumpPos )
        {
               DebugMessage("CameraSnapTo: Failed to Get clump vectors", "VFRW");
               return false;
        }
           //Set the position of the camera to the clump
           vCameraPos->x = vNewTarGet.x + pvClumpPos->x;
           vCameraPos->y = vNewTarGet.y + pvClumpPos->y;
           vCameraPos->z = vNewTarGet.z + pvClumpPos->z - (pBBox->sup.z - pBBox->inf.z) * 10;
    }
       //Make camera 'dirty' - force it to update
       if ( !RwFrameUpdateObjects ( CPAxis::m_pMatrix ) )
       {
           DebugMessage("CameraSnapTo: Translation failed", "VFRW");
           return false;
       }
    */
#endif

    return true;
}


//=============================================================================
//  Name :
//  Desc :  Move camera forward and backward.
//  Ver  :  rw
//=============================================================================
void Camera::Dolly( float distance )
{
    if( empty() )
        return;
#ifdef _RWKERNEL_
    VfVertex3f mov  ( 0, 0, distance );
    PreTranslate( &mov );
#endif
}


//=============================================================================
//  Name :
//  Desc :  Move camera around an specified axis of an object.( rw )
//  Ver  :  rw
//=============================================================================
void Camera::Orbit( const LG::Vertex3f center, float angle, LG::Vector3f axis )
{
    if( empty() )
        return;

#ifdef _RWKERNEL_
    //RotateQuat(angle, axis);
	//double a = VectorIncludedAngle( axis, *At() );
	LG::Vertex3f v;
	v.x = Position()->x - center.x;
	v.y = Position()->y - center.y;
	v.z = Position()->z - center.z;

	RwV3d a = {0.f, 1.f, 0.f};
	RwMatrix m;
	RwMatrixSetIdentity(&m);
	RwMatrixRotate(&m, &a, angle, rwCOMBINEREPLACE);

	RwV3dTransformVectors((RwV3d*)&v, (RwV3d*)&v, 1, &m);
	v.x = center.x + v.x;
	v.y = center.y + v.y;
	v.z = center.z + v.z;
	SetPos(&v);

	//float distance = LengthXYZ( v );
	//Dolly( distance );
	//axis.x = 0;
	//axis.y = 1;
	//axis.z = 0;
	//Rotate( &axis, angle );
	//Dolly( -distance );
	//Rotate( &r, 0 );
	//LookAt( center );
	LookAt( center );
#endif
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::Move( const LG::Vertex3f pos )
{
    if( empty() )
        return;
#ifdef _RWKERNEL_
	CPAxis::PreTranslate( &pos );
#endif
}

/*
//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::MoveTo( VfVertex3f pos )
{
    if( empty() )
        return;
    CPAxis::ReTranslate( &pos );

#ifdef _RWKERNEL_
    /*
       VfVertex3f offSet;
       //RwFrame *CPAxis::m_pMatrix;
       RwMatrix *cameraMatrix;
       RwV3d *at, *up, *right;
       //CPAxis::m_pMatrix = RwCameraGetFrame(m_pCamera);
       cameraMatrix = RwFrameGetMatrix(CPAxis::m_pMatrix);
       
       at = RwMatrixGetAt(cameraMatrix);
       up = RwMatrixGetUp(cameraMatrix);
       right = RwMatrixGetRight(cameraMatrix);
       offSet.x = delta.x * right->x + delta.y * up->x + delta.z * at->x;
       offSet.y = delta.x * right->y + delta.y * up->y + delta.z * at->y;
       offSet.z = delta.x * right->z + delta.y * up->z + delta.z * at->z;
       RwFrameTranslate(CPAxis::m_pMatrix, &offSet, rwCOMBINEPOSTCONCAT);
    */
/*
#endif

    return;
}
*/

//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::Zoom( float power )
{
#ifdef _RWKERNEL_
    if( power != 0 )
    {
        const RwV2d* cameraViewWin  = RwCameraGetViewWindow( m_pCamera );
        RwV2d newCameraViewWin;

        float scalar                = ( float ) fabs( power );

        if( power > 0 )
            scalar = 1 / scalar;

        RwV2dScale( &newCameraViewWin, cameraViewWin, scalar );

        RwCameraSetViewWindow( m_pCamera, &newCameraViewWin );
    }
#endif
}



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Camera::Pan( float angle )
{
    if( empty() )
        return;

#ifdef _RWKERNEL_

    RwMatrix* cameraMatrix  = RwFrameGetMatrix( CPAxis::m_pMatrix );
    ReRotate( ( LG::Vector3f* ) RwMatrixGetPos( cameraMatrix ), angle );

    /*
       VfVertex3f invCamPos;
       
       VfVertex3f camPos;
       //CPAxis::m_pMatrix = RwCameraGetFrame(m_pCamera);
       cameraMatrix = RwFrameGetMatrix(CPAxis::m_pMatrix);
       camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);
       RwV3dScale(&invCamPos, &camPos, -1.0f);
       //Tslate the m_pCamera back to the rotation origin...
       RwFrameTranslate(CPAxis::m_pMatrix, &invCamPos, rwCOMBINEPOSTCONCAT);

       //Get the m_pCamera's UP vector and use this as the axis of rotation...
       RwMatrixRotate(cameraMatrix, 
           RwMatrixGetUp(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);
       //Translate the m_pCamera back to its original position...
       
       RwFrameTranslate(CPAxis::m_pMatrix, &camPos, rwCOMBINEPOSTCONCAT);
    */
#endif

    return;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Camera::Tilt( float angle )
{
    if( empty() )
        return;

#ifdef _RWKERNEL_
    RwV3d invCamPos;
    RwV3d camPos;
    RwMatrix* matrix= RwFrameGetMatrix( CPAxis::m_pMatrix );
    RwV3dAssign( &camPos, RwMatrixGetPos( matrix ) );

    RwV3dScale( &invCamPos, &camPos, -1.0f );

    // Translate the camera back to the rotation origin...
    RwFrameTranslate( CPAxis::m_pMatrix, &invCamPos, rwCOMBINEPOSTCONCAT );
    //Translate(&invCamPos);

    // Get the camera's UP vector and use this as the axis of rotation...
    RwMatrixRotate( matrix, &matrix->right, angle, rwCOMBINEPOSTCONCAT );

    // Translate the camera back to its original position...
    RwFrameTranslate( CPAxis::m_pMatrix, &camPos, rwCOMBINEPOSTCONCAT );
    /*
       VfVertex3f invCamPos;
       RwMatrix *cameraMatrix;
       VfVertex3f camPos;
    //CPAxis::m_pMatrix = RwCameraGetFrame(m_pCamera);
       cameraMatrix = RwFrameGetMatrix(CPAxis::m_pMatrix);
       camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);
       RwV3dScale(&invCamPos, &camPos, -1.0f);
       //Translate the m_pCamera back to the rotation origin...
       RwFrameTranslate(CPAxis::m_pMatrix, &invCamPos, rwCOMBINEPOSTCONCAT);
       //Get the m_pCamera's RIGHT vector and use this as the axis of rotation...
       RwMatrixRotate(cameraMatrix, 
           RwMatrixGetRight(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);
        //Translate the m_pCamera back to its original position...
       RwFrameTranslate(CPAxis::m_pMatrix, &camPos, rwCOMBINEPOSTCONCAT);
    */
#endif

    return;
}


//=============================================================================
//  Name :
//  Desc :  Rotate camera around its own at-axis.Y
//=============================================================================
void Camera::Roll( float angle )
{
    if( empty() )
        return;

#ifdef _RWKERNEL_
    RwV3d invCamPos;
    RwV3d camPos;
    RwMatrix* matrix= RwFrameGetMatrix( CPAxis::m_pMatrix );
    RwV3dAssign( &camPos, RwMatrixGetPos( matrix ) );

    RwV3dScale( &invCamPos, &camPos, -1.0f );

    // Translate the camera back to the rotation origin...
    RwFrameTranslate( CPAxis::m_pMatrix, &invCamPos, rwCOMBINEPOSTCONCAT );
    //Translate(&invCamPos);

    // Get the camera's UP vector and use this as the axis of rotation...
    RwMatrixRotate( matrix, &matrix->up, angle, rwCOMBINEPOSTCONCAT );

    // Translate the camera back to its original position...
    RwFrameTranslate( CPAxis::m_pMatrix, &camPos, rwCOMBINEPOSTCONCAT );
#endif
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Camera::Clear( VfRGBA* colour, Pint clearMode )
{
#ifdef _RWKERNEL_
    if( m_pCamera == 0 )
        return;
    RwCameraClear( m_pCamera, ( RwRGBA * ) colour, clearMode );
#endif
#if defined (_WGLKERNEL_) || defined( _WGLKERNEL_)
    glClear( clearMode );
#endif
    return;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pint Camera::BeginUpdate( void )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)

#endif
#ifdef _RWKERNEL_
    RwCameraBeginUpdate( m_pCamera );
#endif
    return 1;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pint Camera::EndUpdate( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    gluLookAt( CPAxis::m_Matrix->pos.x,
               CPAxis::m_Matrix->pos.y,
               CPAxis::m_Matrix->pos.z,
               CPAxis::m_Matrix->at.x,
               CPAxis::m_Matrix->at.y,
               CPAxis::m_Matrix->at.z,
               CPAxis::m_Matrix->up.x,
               CPAxis::m_Matrix->up.y,
               CPAxis::m_Matrix->up.z );
#endif
#ifdef _RWKERNEL_
    RwCameraEndUpdate( m_pCamera );
#endif
    return 1;
}


//=============================================================================
//  Name : Draw()
//  Desc : 
//=============================================================================
void Camera::Render( const VfImage* image, LG::Vertex2f site, float w, float h )
{
#ifdef _RWKERNEL_

    //////
    RwImage* rwimage= RwImageCreate( image->width,
                                     image->height,
                                     image->depth );
    //RwImageAllocatePixels( rwimage );
    rwimage->stride = rwimage->width * ( rwimage->depth >> 3 );

    //RwImageSetPixels( rwimage, RwImageGetPixels( rwimag ) );
    RwImageSetPixels( rwimage, image->pixels );
    RwRaster* ras   = RwRasterCreate( image->width,
                                      image->height,
                                      image->depth,
                                      rwRASTERTYPETEXTURE );
    //RwRasterSetFromImage( ras, rwimage );
    RwRasterSetFromImage( ras, rwimage );

    float recipCamZ = 1;
    float x2        = site.x + RwRasterGetWidth( ras );
    float y2        = site.y + RwRasterGetHeight( ras );
    float z         = RwIm2DGetNearScreenZ();
    float uvOffSet  = 0;

    RwRenderStateSet( rwRENDERSTATETEXTURERASTER, ( void * ) ras );

    RwIm2DVertex vx[ 4 ];
    // Render an opaque white 2D quad at the given coordinates and spanning a whole texture.
    RwIm2DVertexSetScreenX( &vx[ 0 ], site.x );
    RwIm2DVertexSetScreenY( &vx[ 0 ], site.y );
    RwIm2DVertexSetScreenZ( &vx[ 0 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 0 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 0 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 0 ], uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 0 ], uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 1 ], site.x );
    RwIm2DVertexSetScreenY( &vx[ 1 ], y2 );
    RwIm2DVertexSetScreenZ( &vx[ 1 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 1 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 1 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 1 ], uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 1 ], 1.0f + uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 2 ], x2 );
    RwIm2DVertexSetScreenY( &vx[ 2 ], site.y );
    RwIm2DVertexSetScreenZ( &vx[ 2 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 2 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 2 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 2 ], 1.0f + uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 2 ], uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 3 ], x2 );
    RwIm2DVertexSetScreenY( &vx[ 3 ], y2 );
    RwIm2DVertexSetScreenZ( &vx[ 3 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 3 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 3 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 3 ], 1.0f + uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 3 ], 1.0f + uvOffSet, recipCamZ );

    RwIm2DRenderPrimitive( rwPRIMTYPETRISTRIP, vx, 4 );


    RwImageDestroy( rwimage );
    RwRasterDestroy( ras );

#endif

    return;
}



//=============================================================================
//  Name : Draw()
//  Desc : 
//=============================================================================
void Camera::Render( const CPTexture* tex, LG::Vertex2f site, float w, float h )
{
#ifdef _RWKERNEL_
    RwRaster* ras   = RwTextureGetRaster( tex->m_pTexture );
    RwRenderStateSet( rwRENDERSTATETEXTURERASTER, ( void * ) ras );
    w = w * ( float ) RwRasterGetWidth( ras );
    h = h * ( float ) RwRasterGetHeight( ras );

    float z         = RwIm2DGetNearScreenZ();
    float recipCamZ = 1.0f / RwCameraGetNearClipPlane( m_pCamera );
    float uvOffSet  = 0.0f;

    RwIm2DVertex vx[ 4 ];

    // Render an opaque white 2D quad at the given coordinates and spanning a whole texture.
    RwIm2DVertexSetScreenX( &vx[ 0 ], site.x );
    RwIm2DVertexSetScreenY( &vx[ 0 ], site.y );
    RwIm2DVertexSetScreenZ( &vx[ 0 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 0 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 0 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 0 ], uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 0 ], uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 1 ], site.x );
    RwIm2DVertexSetScreenY( &vx[ 1 ], h );
    RwIm2DVertexSetScreenZ( &vx[ 1 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 1 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 1 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 1 ], uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 1 ], 1.0f + uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 2 ], w );
    RwIm2DVertexSetScreenY( &vx[ 2 ], site.y );
    RwIm2DVertexSetScreenZ( &vx[ 2 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 2 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 2 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 2 ], 1.0f + uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 2 ], uvOffSet, recipCamZ );

    RwIm2DVertexSetScreenX( &vx[ 3 ], w );
    RwIm2DVertexSetScreenY( &vx[ 3 ], h );
    RwIm2DVertexSetScreenZ( &vx[ 3 ], z );
    RwIm2DVertexSetIntRGBA( &vx[ 3 ], 255, 255, 255, 255 );
    RwIm2DVertexSetRecipCameraZ( &vx[ 3 ], recipCamZ );
    RwIm2DVertexSetU( &vx[ 3 ], 1.0f + uvOffSet, recipCamZ );
    RwIm2DVertexSetV( &vx[ 3 ], 1.0f + uvOffSet, recipCamZ );

    RwIm2DRenderPrimitive( rwPRIMTYPETRISTRIP, vx, 4 );
#endif
}

//=============================================================================
//  Name : Draw()
//  Desc : 
//=============================================================================
void Camera::Render( const CPObject* object )
{
#ifdef _RWKERNEL_
    RpAtomicRender( object->m_pAtomic );
#endif
}

//=============================================================================
//  Name : Render()
//  Desc : 
//=============================================================================
void Camera::Render( const CPModel* model )
{
#ifdef _RWKERNEL_
    RpClumpRender( model->m_pClump );
#endif
    /*
    for (int loop_o = 0; loop_o < numObject; loop_o++)
    {
        glBegin(GL_TRIANGLES);
        for( int loop_v = 0; loop_v < m_object[loop_o]->numVertex; loop_v++ )
        {
            glNormal3f( 0.0f, 0.0f, 1.0f);
            glTexCoord2f( m_object[loop_o]->vertex[loop_v].vTextureCoord.x,
                          m_object[loop_o]->vertex[loop_v].vTextureCoord.y );
            glVertex3f( m_object[loop_o]->vertex[loop_v].x, 
                        m_object[loop_o]->vertex[loop_v].y, 
                        m_object[loop_o]->vertex[loop_v].z );
        }
        glEnd();
    }
    */
}





///////////////////////////////////////////////////////////////////////////////
//
//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
void Camera::SetBSphere( VfBSphere vfs )
{
    m_bshpere = vfs;
    return;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw
//=============================================================================
void Camera::SetViewPos( Pint posX, Pint posY )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    m_ViewOffSet.x = posX;
    m_ViewOffSet.x = posY;

    glViewport( m_ViewOffSet.x, m_ViewOffSet.y, m_ViewSize.cx, m_ViewSize.cy );          // ReSet The Current Viewport
    return;
#endif

#ifdef _RWKERNEL_
    if( m_pCamera )
    {
        RwRaster* raster= RwCameraGetRaster( m_pCamera );
        raster->nOffsetX = posX;
        raster->nOffsetY = posY;
		//tImageAssignRwRaster( &m_fbuffer, raster );
    }
#endif
    return;
}


//=============================================================================
//  Name :
//  Desc :  Resize And Initialize The GL Window
//  Ver  :  rw GL
//=============================================================================
int Camera::SetViewBox( Puint width, Puint height, Puint depth )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    if( height <= 0 )                           // Prevent A Divide By Zero By
        height = 1;                             // Making Height Equal One

    if( width <= 0 )                            // Prevent A Divide By Zero By
        width = 1;                              // Making Height Equal One

    m_ViewSize.cx = width;
    m_ViewSize.cy = height;

    glViewport( 0, 0, width, height );          // ReSet The Current Viewport
    return 1;
#endif

#ifdef _RWKERNEL_
    if( m_pCamera )
    {
        //Check everything is valid...
		RwRaster* raster = RwCameraGetRaster( m_pCamera );
        if( RwCameraGetFrame( m_pCamera ) && raster )
        {
           
            //Everything OK...
            raster->width = width;
            raster->height = height;
            if( depth != 0 )
                raster->depth = depth;
			//tImageAssignRwRaster( &m_fbuffer, raster );
            return 1;
        }
    }

#endif

    return 0;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
int Camera::SetClipPlanes( float nearplane, float farplane, Pdouble fov )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    m_nearPlane = nearplane;
    m_farPlane = farplane;

    glMatrixMode( GL_PROJECTION );              // Select The Projection Matrix
    glLoadIdentity();                           // ReSet The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    //glOrtho(  0.0f, ( GLfloat )width, 0.0f, ( GLfloat )height, 0.0f, 1.0f  );
    gluPerspective( fov,
                    ( GLfloat ) m_ViewSize.cx / ( GLfloat ) m_ViewSize.cy,
                    m_nearPlane,
                    m_farPlane );

    //glFrustum(  -width, ( GLfloat )width, -height, ( GLfloat )height, 0.1f, 100.0f  );
    glMatrixMode( GL_MODELVIEW );               // Select The Modelview Matrix
    glLoadIdentity();                           // ReSet The Modelview Matrix
    return 1;
#endif

#ifdef _RWKERNEL_
    if( m_pCamera == NULL )
        return 0;
    RwCameraSetNearClipPlane( m_pCamera, nearplane );
    RwCameraSetFarClipPlane( m_pCamera, farplane );
    return 1;
#endif
    return 0;
}


//=============================================================================
//  Name : SetFogDistance()
//  Desc : 
//=============================================================================
void Camera::SetFogDistance( float distance )
{
#ifdef _RWKERNEL_
    RwCameraSetFogDistance( m_pCamera, distance );
#endif
}

//=============================================================================
//  Name : SetFOV()
//  Desc : 
//=============================================================================
void Camera::SetFOV( float fovH, float fovV )
{
#ifdef _RWKERNEL_
    RwV2d vw;   
    vw.x = ( RwReal ) RwTan( ( fovH * 0.5f ) * _PI_ / 180.f );
    vw.y = ( RwReal ) RwTan( ( fovV * 0.5f ) * _PI_ / 180.f ) /
           m_kDefaultAspectRatio;
    RwCameraSetViewWindow( m_pCamera, &vw ); 
#endif
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
VfBSphere Camera::GetBSphere( void )
{
    VfBSphere s = m_bshpere;
    s.center = s.center + (*Position());
    return s;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
float Camera::GetClipNearPlane( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return m_nearPlane;
#endif

#ifdef _RWKERNEL_
    return RwCameraGetNearClipPlane( m_pCamera );
#endif

    return 0;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
float Camera::GetClipFarPlane( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
    return m_farPlane;
#endif

#ifdef _RWKERNEL_
    return RwCameraGetFarClipPlane( m_pCamera );
#endif

    return 0;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
LG::Boxi Camera::GetViewBox( void )
{
	LG::Boxi box;
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
	box.width = m_fbuffer.width;
	box.height = m_fbuffer.height;
	box.depth = m_fbuffer.depth;
#endif
#ifdef _RWKERNEL_
    if( m_pCamera )
    {
		RwRaster* raster= RwCameraGetRaster( m_pCamera );
        if( RwCameraGetFrame( m_pCamera ) && raster )
        {
            box.width = raster->width;
            box.height = raster->height;
            box.depth = raster->depth;
        }
    }
#endif
    return box;
}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
tImage Camera::GetfBuffer( void )
{ 
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
	return &m_fbuffer;
#endif
#ifdef _RWKERNEL_
	tImage image;
	tImageAssignRwRaster( &image,
		                  RwCameraGetRaster( m_pCamera ) );
	return image;
#endif

}


//=============================================================================
//  Name :
//  Desc :
//  Ver  :  rw GL
//=============================================================================
tImage Camera::GetzBuffer( void )
{
#if defined (_WGLKERNEL_)|| defined (_XGLKERNEL_)
	return &m_zbuffer;
#endif
#ifdef _RWKERNEL_
	tImage image;
	tImageAssignRwRaster( &image,
		                  RwCameraGetZRaster( m_pCamera ) );
	return image;


#endif
}






/////////////////////////////////////////////////////////////////
// RenderWare function
//
//
//
//
//
//
//
//
#ifdef _RWKERNEL_

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
Camera::Camera( RwCamera* Camera )
{
	operator=( Camera );
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void tImageAssignRwRaster( tImage* image, RwRaster* raster )
{
    image->format = raster->cFormat;              // The channels in the image (3 = RGB : 4 = RGBA)
    image->width = raster->width;                 // The width of the image in pixels
    image->height = raster->height;               // The height of the image in pixels
    image->depth = raster->depth;
    image->pixels = raster->cpPixels;             // The image pixel data
    image->palette = ( Puint * ) raster->palette;
	return;

}

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
RwCamera& Camera::operator=( RwCamera* rwcamera )
{
	m_pCamera = rwcamera;
    CPAxis::m_pMatrix = RwCameraGetFrame( m_pCamera );
	return *rwcamera;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================

RwRaster* Camera::CreateCrossHair( void )
{
    RwImage* image;

    image = RwImageCreate( CROSSHAIRWIDTH, CROSSHAIRWIDTH, 32 );
    if( image )
    {
        RwRaster* raster;

        RwImageSetStride( image, CROSSHAIRWIDTH * 4 );
        RwImageSetPixels( image, CrossHairData );

        
        //Create the pointer raster...
        raster = RwRasterCreate( RwImageGetWidth( image ),
                                 RwImageGetHeight( image ),
                                 0,
                                 rwRASTERTYPENORMAL );

        if( raster )
        {
            //...and initialize its pixels from those in the image...
            RwRasterSetFromImage( raster, image );

            RwImageDestroy( image );

            return raster;
        }
    }


    return NULL;
}
//=============================================================================
//  Name : SetSub()
//  Desc : 
//=============================================================================
void Camera::SetSub( Camera* subCamera )
{
    if( ( m_pCamera == NULL ) || ( subCamera == NULL ) )
        return;

    RwRaster* Raster, * ZRaster;
    //RwRect SubRect;

    Raster = RwCameraGetRaster( subCamera->m_pCamera );
    ZRaster = RwCameraGetRaster( subCamera->m_pCamera );

    Raster->parent = RwCameraGetRaster( m_pCamera )->parent;
    ZRaster->parent = RwCameraGetZRaster( m_pCamera )->parent;
}

};
#endif

