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
//                                               Copyright    (C)    2002    Pin
//
#include "RenderGraphic3D.h"

#include <VfwKernel.h>
#include <aMath.h>

//#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
#include <gl/gl.h>
//#endif

#define NUMPOINTS (100)
///////////////////////////////////////////////////////////////////////////////
//
void RenderVertex( LG::Vertex3f vertex, LC::RGBAb color, float size, PA::Matrixf* mat )
{
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
	}
	
	glColor4f( color.R, color.G, color.B, color.A );
	glPointSize(size);
	glBegin( GL_POINTS );
	    glVertex3f( vertex.x, vertex.y, vertex.z );
	glEnd();
	glPointSize(1);

	if( mat != NULL )
		glPopMatrix();
	return;
#endif //#ifdef __GL_H__
#ifdef _RWKERNEL_
	RwIm3DVertex imVertex[1];
    RwIm3DVertexSetPos( &imVertex[0], vertex.x, vertex.y, vertex.z);
    RwIm3DVertexSetRGBA( &imVertex[0], color.R, color.G, color.B, color.A );
	RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void *)NULL );
	glPointSize(size);
#ifdef __GL_H__
	glPointSize(size);
#endif
	if( RwIm3DTransform( imVertex, 1, (RwMatrix*)mat, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA ) )
    {                         
        RwIm3DRenderPrimitive( rwPRIMTYPEPOLYLINE );
        RwIm3DEnd();
    }
#ifdef __GL_H__
	glPointSize(1);
#endif

	//RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
#endif
	return;
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderVertices( LG::Vertex3f* vertex, int len, LC::RGBAb color, float size, PA::Matrixf* mat )
{
#ifdef __GL_H__

#endif //#ifdef __GL_H__
#ifdef _RWKERNEL_

#endif
	return;
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderLine( LG::Vertex3f vet1, LG::Vertex3f vet2, LC::RGBAb color, float width, PA::Matrixf* mat )
{
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
    }

	glLineWidth( width );
	glColor4ub( color.R, color.G, color.B, color.A );
	glBegin( GL_LINES );
	    glVertex3f( vet1.x, vet1.y, vet1.z );
	    glVertex3f( vet2.x, vet2.y, vet2.z );
	glEnd();
	glLineWidth( 1 );


	if( mat != NULL )
		glPopMatrix();
	return;
#endif //#ifdef __GL_H__

#ifdef _RWKERNEL_
    RwIm3DVertex imVertex[2];

    RwIm3DVertexSetPos(&imVertex[0], vet1.x, vet1.y, vet1.z);
	RwIm3DVertexSetRGBA(&imVertex[0], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&imVertex[1], vet2.x, vet2.y, vet2.z);
    RwIm3DVertexSetRGBA(&imVertex[1], color.R, color.G, color.B, color.A);

#ifdef __GL_H__
	glLineWidth( width );
#endif

    if( RwIm3DTransform(imVertex, 2, (RwMatrix*)mat, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth( 1 );
#endif

#endif

	return;
}



///////////////////////////////////////////////////////////////////////////////
//
void RenderCube( LG::BBoxf cube, LC::RGBAb color, float width, PA::Matrixf* mat )
{

#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
	}

	glLineWidth(width );
	glColor4ub( color.R, color.G, color.B, color.A );
	glBegin( GL_LINE_LOOP );
	    glVertex3f( cube.vertex[0].x, cube.vertex[0].y, cube.vertex[0].z );
		glVertex3f( cube.vertex[1].x, cube.vertex[0].y, cube.vertex[0].z );
		glVertex3f( cube.vertex[1].x, cube.vertex[0].y, cube.vertex[1].z );
	    glVertex3f( cube.vertex[0].x, cube.vertex[0].y, cube.vertex[1].z );
	glEnd();
	glBegin( GL_LINE_LOOP );
		glVertex3f( cube.vertex[0].x, cube.vertex[1].y, cube.vertex[0].z );
		glVertex3f( cube.vertex[0].x, cube.vertex[1].y, cube.vertex[1].z );
		glVertex3f( cube.vertex[1].x, cube.vertex[1].y, cube.vertex[1].z );
	    glVertex3f( cube.vertex[1].x, cube.vertex[1].y, cube.vertex[0].z );
	glEnd();

	glBegin( GL_LINES );
	    glVertex3f( cube.vertex[0].x, cube.vertex[0].y, cube.vertex[0].z );
		glVertex3f( cube.vertex[0].x, cube.vertex[1].y, cube.vertex[0].z );

		glVertex3f( cube.vertex[1].x, cube.vertex[0].y, cube.vertex[0].z );
		glVertex3f( cube.vertex[1].x, cube.vertex[1].y, cube.vertex[0].z );

		glVertex3f( cube.vertex[0].x, cube.vertex[0].y, cube.vertex[1].z );
		glVertex3f( cube.vertex[0].x, cube.vertex[1].y, cube.vertex[1].z );

		glVertex3f( cube.vertex[1].x, cube.vertex[0].y, cube.vertex[1].z );
		glVertex3f( cube.vertex[1].x, cube.vertex[1].y, cube.vertex[1].z );
	glEnd();

	if( mat != NULL )
		glPopMatrix();
	return;
#endif //#ifdef __GL_H__


#ifdef _RWKERNEL_
	//RwInt32 i;
    RwIm3DVertex imVertex[8];
    //RwMatrix *ltm = NULL;
    //RwBBox *bbox = NULL;
    RwImVertexIndex indices[24] = 
    {
        0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7,
        7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7
    };

    //bbox = &ClumpLocalBoundingBox;

    //ltm = RwFrameGetLTM(RpClumpGetFrame(clump));
       
    for( int i = 0; i < 8; i++ )
    {
        RwIm3DVertexSetPos(&imVertex[i],
            i & 1 ? cube.vertex[0].x : cube.vertex[1].x,
            i & 2 ? cube.vertex[0].y : cube.vertex[1].y,
            i & 4 ? cube.vertex[0].z : cube.vertex[1].z
        );

        RwIm3DVertexSetRGBA( &imVertex[i], color.R, color.G, color.B, color.A );
    }

    //RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    //RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
#ifdef __GL_H__
	glLineWidth( width );
#endif
    if( RwIm3DTransform( imVertex, 8, (RwMatrix*)mat, rwIM3D_ALLOPAQUE ) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 24);

        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth(1);
#endif

    //RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderSphere( LG::Spheref sphere, LC::RGBAb color, float width, PA::Matrixf* mat )
{
#ifdef _RWKERNEL_
	RwIm3DVertex imVertex[NUMPOINTS + 1];
	RwReal constant;
	int i;
    constant = 2.0f * rwPI / NUMPOINTS;
    for(i=0; i<NUMPOINTS+1; i++)
    {
        RwV3d point;

		RwMatrix cRot = *(RwMatrix*)mat;
		cRot.pos.x = cRot.pos.y = cRot.pos.z = 0.f;
        point.x = sphere.radius * (RwReal)RwCos(i * constant);
        point.y = sphere.radius * (RwReal)RwSin(i * constant); 
        point.z = 0.f;
		RwV3dTransformPoints(&point, &point, 1, &cRot);
		
		point.x += sphere.center.x;
		point.y += sphere.center.y;
		point.z += sphere.center.z;
        RwIm3DVertexSetPos(&imVertex[i], point.x, point.y, point.z);
		RwIm3DVertexSetRGBA(&imVertex[i], color.R, color.G, color.B, color.A );
    }

    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);


#ifdef __GL_H__
	glLineWidth( width );
#endif
    if(RwIm3DTransform(imVertex, NUMPOINTS + 1, NULL, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);

        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth( 1 );
#endif

    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

#endif
	return;
}

///////////////////////////////////////////////////////////////////////////////
//
void RenderCircle( LG::Spheref sphere, LC::RGBAb color, float width, PA::Matrixf* mat )
{
#ifdef _RWKERNEL_
    RwIm3DVertex imVertex[NUMPOINTS + 1];
	//RwV3d center;

	if( mat != NULL )
        RwV3dTransformPoints( (RwV3d*)&sphere.center, (RwV3d*)&sphere.center, 1, (RwMatrix*)mat );
	//else
	//	memcpy( &center, (RwV3d*)&sphere.center, sizeof(RwV3d) );

    float constant = 2.0f * rwPI / NUMPOINTS;

	//Puint ci = (( (Puint)(sphere.radius) )<<1)+1;
	    
    for( Puint i = 0; i < NUMPOINTS+1; i++ )
    {
        RwV3d point;

        point.x = sphere.center.x + sphere.radius * (RwReal)sin(i * constant);
        point.y = sphere.center.y + sphere.radius * (RwReal)cos(i * constant); 
        point.z = sphere.center.z;

        RwIm3DVertexSetPos(&imVertex[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&imVertex[i], color.R, color.G, color.B, color.A);
    }

#ifdef __GL_H__
	glLineWidth( width );
#endif
    if( RwIm3DTransform(imVertex, NUMPOINTS+1, NULL, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth( 1 );
#endif

#endif
	return;
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderPyramid( LG::Spheref sphere, float height, LC::RGBAb color, float width, PA::Matrixf* mat )
{
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
	}

#endif
#ifdef _RWKERNEL_
	RwIm3DVertex imVertex[100+1];
    float constant = 2.0f * rwPI / 100;

	LG::Vertex3f top( sphere.center.x, sphere.center.y, sphere.center.z );
	sphere.center.y+=height;
	    
    for( Puint i = 0; i < 101; i++ )
    {
        RwV3d point;

        point.x = sphere.center.x + sphere.radius * (RwReal)sin(i * constant);
        point.y = sphere.center.y;
        point.z = sphere.center.z + sphere.radius * (RwReal)cos(i * constant); ;

        RwIm3DVertexSetPos(&imVertex[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&imVertex[i], color.R, color.G, color.B, color.A);
    }

	RwIm3DVertex pmVertex[6];
    RwImVertexIndex indices[10] = 
    {
        0, 1, 0, 2, 0, 3, 0, 4, 0, 5
    };


    RwIm3DVertexSetPos(&pmVertex[0], top.x, top.y, top.z);
    RwIm3DVertexSetRGBA(&pmVertex[0], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&pmVertex[1], sphere.center.x-sphere.radius, sphere.center.y, sphere.center.z );
    RwIm3DVertexSetRGBA(&pmVertex[1], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&pmVertex[2], sphere.center.x+sphere.radius, sphere.center.y, sphere.center.z );
    RwIm3DVertexSetRGBA(&pmVertex[2], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&pmVertex[3], sphere.center.x, sphere.center.y, sphere.center.z-sphere.radius );
    RwIm3DVertexSetRGBA(&pmVertex[3], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&pmVertex[4], sphere.center.x, sphere.center.y, sphere.center.z+sphere.radius );
    RwIm3DVertexSetRGBA(&pmVertex[4], color.R, color.G, color.B, color.A);
    RwIm3DVertexSetPos(&pmVertex[5], sphere.center.x, sphere.center.y, sphere.center.z );
    RwIm3DVertexSetRGBA(&pmVertex[5], color.R, color.G, color.B, color.A);

	
#ifdef __GL_H__
	glLineWidth( width );
#endif

    if( RwIm3DTransform(imVertex, 101, (RwMatrix*)mat, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }

    if( RwIm3DTransform( pmVertex, 6, (RwMatrix*)mat, rwIM3D_ALLOPAQUE ) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 10);

        RwIm3DEnd();
    }

#ifdef __GL_H__
	glLineWidth( 1 );
#endif

#endif
	return;

}


///////////////////////////////////////////////////////////////////////////////
//
void RenderHexahedron ( LG::Vertex3f vertex[8], LC::RGBAb color, PA::Matrixf* mat )
{
#ifdef _RWKERNEL_

#endif
	return;
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderAxis( PA::Matrixf axis, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float width, PA::Matrixf* mat )
{
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
	}
	glLineWidth(width );
	glBegin( GL_LINES );
	    glColor4ub( u.R, u.G, u.B, u.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14]);
	    glVertex3f( axis.a[0]+axis.a[12], axis.a[1]+axis.a[13], axis.a[2]+axis.a[14] );

	    glColor4ub( a.R, a.G, a.B, a.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14]);
	    glVertex3f( axis.a[8]+axis.a[12], axis.a[9]+axis.a[13], axis.a[10]+axis.a[14]);

	    glColor4ub( r.R, r.G, r.B, r.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14]);
	    glVertex3f( axis.a[4]+axis.a[12], axis.a[5]+axis.a[13], axis.a[6]+axis.a[14]);
	glEnd();
	glLineWidth( 1 );


	if( mat != NULL )
		glPopMatrix();
	return;

#endif //#ifdef __GL_H__

#ifdef _RWKERNEL_

    RwIm3DVertex imVertex[6];
	RwImVertexIndex indices[6] = { 0, 1, 2, 3, 4, 5 };

    RwIm3DVertexSetPos(&imVertex[0], axis.a[12]+axis.a[0], axis.a[13]+axis.a[1], axis.a[14]+axis.a[2]);
	RwIm3DVertexSetRGBA(&imVertex[0], u.R, u.G, u.B, u.A);
    RwIm3DVertexSetPos(&imVertex[1], axis.a[12], axis.a[13], axis.a[14]);
    RwIm3DVertexSetRGBA(&imVertex[1], u.R, u.G, u.B, u.A);

    RwIm3DVertexSetPos(&imVertex[2], axis.a[12]+axis.a[8], axis.a[13]+axis.a[9], axis.a[14]+axis.a[10]);
    RwIm3DVertexSetRGBA(&imVertex[2], a.R, a.G, a.B, a.A);
    RwIm3DVertexSetPos(&imVertex[3], axis.a[12], axis.a[13], axis.a[14]);
    RwIm3DVertexSetRGBA(&imVertex[3], a.R, a.G, a.B, a.A);

    RwIm3DVertexSetPos(&imVertex[4], axis.a[12]+axis.a[4], axis.a[13]+axis.a[5], axis.a[14]+axis.a[6]);
    RwIm3DVertexSetRGBA(&imVertex[4], r.R, r.G, r.B, r.A);
    RwIm3DVertexSetPos(&imVertex[5], axis.a[12], axis.a[13], axis.a[14]);
    RwIm3DVertexSetRGBA(&imVertex[5], r.R, r.G, r.B, r.A);
	
#ifdef __GL_H__
	glLineWidth( width );
#endif
    if( RwIm3DTransform(imVertex, 6, (RwMatrix*)mat, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST , indices, 6);
        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth( 1 );
#endif

#endif
	return;
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderUnitAxis( PA::Matrixf axis, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float mul, float width, PA::Matrixf* mat )
{
//#ifdef __GL_H__
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL_)
	//LG::Vector3f up( axis.a[0], axis.a[1], axis.a[2] );
	//LG::Vector3f right( axis.a[4], axis.a[5], axis.a[6] );
	//LG::Vector3f at( axis.a[8], axis.a[9], axis.a[10] );
    LG::Vector3f verup = LG::Vector3f( axis.a[0], axis.a[1], axis.a[2] ).Normalize() * mul;
	LG::Vector3f verat = LG::Vector3f( axis.a[8], axis.a[9], axis.a[10] ).Normalize() * mul;
	LG::Vector3f verright = LG::Vector3f( axis.a[4], axis.a[5], axis.a[6] ).Normalize() * mul;
	
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
		//RwV3dTransformPoints ( (RwV3d*)&(axis.up),    (RwV3d*)&(axis.up), 1,    (RwMatrix*)mat );
		//RwV3dTransformPoints ( (RwV3d*)&(axis.right), (RwV3d*)&(axis.right), 1, (RwMatrix*)mat );
		//RwV3dTransformPoints ( (RwV3d*)&(axis.at),    (RwV3d*)&(axis.at), 1,    (RwMatrix*)mat );
		//RwV3dTransformPoints ( (RwV3d*)&(axis.pos),   (RwV3d*)&(axis.pos), 1,   (RwMatrix*)mat );
        #endif
	}


	glLineWidth( width );
	glBegin( GL_LINES );
	    glColor4ub( u.R, u.G, u.B, u.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14] );
	    glVertex3f( verup.x+axis.a[12], verup.y+axis.a[13], verup.z+axis.a[14] );

	    glColor4ub( a.R, a.G, a.B, a.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14] );
	    glVertex3f( verat.x+axis.a[12], verat.y+axis.a[13], verat.z+axis.a[14] );

	    glColor4ub( r.R, r.G, r.B, r.A );
	    glVertex3f( axis.a[12], axis.a[13], axis.a[14] );
	    glVertex3f( verright.x+axis.a[12], verright.y+axis.a[13], verright.z+axis.a[14]);
	glEnd();
	glLineWidth( 1 );


	if( mat != NULL )
		glPopMatrix();

	return;

#endif //#ifdef __GL_H__

#ifdef _RWKERNEL_
	
    RwIm3DVertex imVertex[6];
	RwImVertexIndex indices[6] = { 0, 1, 2, 3, 4, 5 };

    LG::Vector3f verup( axis.a[0], axis.a[1], axis.a[2] );
	LG::Vector3f verat( axis.a[8], axis.a[9], axis.a[10]);
	LG::Vector3f verright( axis.a[4], axis.a[5], axis.a[6] );
    Normalize( verup );
	Normalize( verat );
	Normalize( verright );
	verup = verup*mul;
	verat = verat*mul;
	verright = verright*mul;


    RwIm3DVertexSetPos(&imVertex[0], verup.x+axis.a[12], verup.y+axis.a[13], verup.z+axis.a[14] );
	RwIm3DVertexSetRGBA(&imVertex[0], u.R, u.G, u.B, u.A);
	RwIm3DVertexSetPos(&imVertex[1], axis.a[12], axis.a[13], axis.a[14] );
    RwIm3DVertexSetRGBA(&imVertex[1], u.R, u.G, u.B, u.A);

	RwIm3DVertexSetPos(&imVertex[2], verat.x+axis.a[12], verat.y+axis.a[13], verat.z+axis.a[14] );
    RwIm3DVertexSetRGBA(&imVertex[2], a.R, a.G, a.B, a.A);
	RwIm3DVertexSetPos(&imVertex[3], axis.a[12], axis.a[13], axis.a[14] );
    RwIm3DVertexSetRGBA(&imVertex[3], a.R, a.G, a.B, a.A);
	
    RwIm3DVertexSetPos(&imVertex[4], verright.x+axis.a[12], verright.y+axis.a[13], verright.z+axis.a[14]);
    RwIm3DVertexSetRGBA(&imVertex[4], r.R, r.G, r.B, r.A);
	RwIm3DVertexSetPos(&imVertex[5], axis.a[12], axis.a[13], axis.a[14] );
    RwIm3DVertexSetRGBA(&imVertex[5], r.R, r.G, r.B, r.A);

#ifdef __GL_H__
	glLineWidth( width );
#endif
    if( RwIm3DTransform(imVertex, 6, (RwMatrix*)mat, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST , indices, 6);
        RwIm3DEnd();
    }
#ifdef __GL_H__
	glLineWidth( 1 );
#endif


#endif

}


void RenderNet( LG::Rect rect, Puint Width, Puint Height, LC::RGBAb color, float linewidth, PA::Matrixf* mat )
{
#ifdef __GL_H__
	if( mat != NULL )
	{
		glPushMatrix();
	    #ifdef _RWKERNEL_
		_rwOpenGLApplyRwMatrix( (RwMatrix*)mat );
        #endif
	}

	glLineWidth( linewidth );
	glColor4ub( color.R, color.G, color.B, color.A );
	glBegin( GL_LINES );
	    if( rect.left > rect.right )
		{
		    for( int c=rect.left; c>=rect.right; c-=Width )
			{
			    glVertex3i( c, 0, rect.top );
	            glVertex3i( c, 0, rect.bottom );
			}
		}
		else
		{
	        for( int c=rect.left; c<=rect.right; c+=Width )
			{
			    glVertex3i( c, 0, rect.top );
	            glVertex3i( c, 0, rect.bottom );
			}
		}

		if( rect.top > rect.bottom )
		{
	        for( int r=rect.top; r>=rect.bottom; r-=Height )
			{
			    glVertex3i( rect.left, 0, r );
	            glVertex3i( rect.right, 0, r );
			}
		}
		else
		{
	        for( int r=rect.top; r<=rect.bottom; r+=Height )
			{
			    glVertex3i( rect.left, 0, r );
	            glVertex3i( rect.right, 0, r );
			}
		}


	glEnd();
	glLineWidth( 1 );
	if( mat != NULL )
		glPopMatrix();

#endif //#ifdef __GL_H__
}


#define RENDER_CONE_NUM_VERTICES (10)
#define CONE_NUM_VERTICES (10)


void RenderCone( const PA::Matrixf* mat, float coneAngle, float coneSize, float coneRatio)
{
    RwRGBA LightSolidColor = {255, 255, 0, 255};
    /*
     * Function to draw a cone.
     * The Cone Ratio parameter set the ratio between the cone height 
     * and width, if this is set to be negative the cone is drawn 
     * inverted - this means that the cone becomes narrower along the at 
     * vector(the direction of the light). Usually it is set to 1.0f which 
     * draws a cone getting wider along the at vector (the direction of 
     * the light).
     */

    RwMatrix *matrix = (RwMatrix*)mat;
    RwV3d *right, *up, *at, *pos;
    RwV3d point;
    RwReal cosValue, sinValue, coneAngleD;
    RwV3d dRight, dUp, dAt;
    RwIm3DVertex cone[CONE_NUM_VERTICES+1];
    RwImVertexIndex indices[CONE_NUM_VERTICES*3];
    RwInt32 i;

    //matrix = RwFrameGetLTM(RpLightGetFrame(CurrentLight));

    right = RwMatrixGetRight(matrix);
    up    = RwMatrixGetUp(matrix);
    at    = RwMatrixGetAt(matrix);
    pos   = RwMatrixGetPos(matrix);

    for(i=1; i<CONE_NUM_VERTICES+1; i++)
    {
        cosValue = (RwReal)(RwCos(i/(CONE_NUM_VERTICES/2.0f) * rwPI) 
            * RwSin(coneAngle / 180.0f * rwPI));
        
        sinValue = (RwReal)(RwSin(i/(CONE_NUM_VERTICES/2.0f) * rwPI) 
            * RwSin(coneAngle / 180.0f * rwPI));

        RwV3dScale(&dUp, up, sinValue * coneSize);
        
        RwV3dScale(&dRight, right, cosValue * coneSize);

        coneAngleD = (RwReal)RwCos(coneAngle / 180.0f * rwPI);

        RwV3dScale(&dAt, at, coneAngleD * coneSize * coneRatio);

        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;

        RwIm3DVertexSetPos(&cone[i], point.x, point.y, point.z);        
    }

    /* 
     * Set up vertex list...
     */
    for(i=0; i < CONE_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)0;
        indices[(i*3)+1] = (RwImVertexIndex)i+2;
        indices[(i*3)+2] = (RwImVertexIndex)i+1;
    }
    
    indices[(CONE_NUM_VERTICES*3)-2] = 1;

    /*
     * Set color & alpha of all points...
     */
    for(i=0; i<(CONE_NUM_VERTICES+1); i++)
    {
        RwIm3DVertexSetRGBA(&cone[i],
            LightSolidColor.red, LightSolidColor.green,
            LightSolidColor.blue, 128);
    }
    
    /*
     * Set cone apex to light position...
     */
    RwIm3DVertexSetPos(&cone[0],  pos->x, pos->y, pos->z);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    if( RwIm3DTransform(cone, (CONE_NUM_VERTICES+1), NULL, 0) )
    {
        /*
         * Draw inside of cone...
         */
        RwIm3DRenderPrimitive(rwPRIMTYPETRIFAN);
        RwIm3DRenderTriangle(0, CONE_NUM_VERTICES, 1);

        /*
         * Draw outside of cone...
         */
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, 
            indices, CONE_NUM_VERTICES*3);
        
        RwIm3DEnd();
    }

    /*
     * Change alpha of all points...
     */
    for(i=0; i<(CONE_NUM_VERTICES+1); i++)
    {
        RwIm3DVertexSetRGBA(&cone[i],
            LightSolidColor.red, LightSolidColor.green,
            LightSolidColor.blue, 255);
    }

     
    /*
     * Set cone apex to same level as cone base 
     */
    coneAngleD = (RwReal)RwCos(coneAngle / 180.0f * rwPI);

    RwV3dScale(&dAt, at, coneAngleD * coneSize * coneRatio);

    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;

    RwIm3DVertexSetPos(&cone[0], point.x, point.y, point.z);

    /*
     * Draw base...
     */       
    if( RwIm3DTransform(cone, CONE_NUM_VERTICES+1, NULL, rwIM3D_ALLOPAQUE) )
    {
        if( coneRatio > 0 )
        {
            RwIm3DRenderPrimitive(rwPRIMTYPETRIFAN);
            RwIm3DRenderTriangle(0, CONE_NUM_VERTICES, 1);
        }
        else
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRIFAN, 
                indices, CONE_NUM_VERTICES*3);
        }

        RwIm3DEnd();
    }
       
    /*
     * Move cone apex by small offset...
     */
    RwV3dScale(&dAt, at, -0.05f);
    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[0], point.x, point.y, point.z);

    /*
     * Draw Lines...
     */    
    if( RwIm3DTransform(cone, CONE_NUM_VERTICES+1, NULL, rwIM3D_ALLOPAQUE) )
    {        
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, 
            indices, CONE_NUM_VERTICES*3);
        
        RwIm3DEnd();
    }

    return;    
}

