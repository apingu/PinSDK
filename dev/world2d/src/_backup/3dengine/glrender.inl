// Render.inl
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
//                                  Pin
//                                     Copyright 2002
//                              
#include <VfwKernel.h>
#include <pmath.h>



//#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
#include <gl/gl.h>
//#endif


///////////////////////////////////////////////////////////////////////////////
//
void RenderVertex( CPVertex3f vertex, CPRGBA color, float size, CPMatrixf* mat )
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
void RenderVertices( CPVertex3f* vertex, int len, CPRGBA color, float size, CPMatrixf* mat )
{
#ifdef __GL_H__

#endif //#ifdef __GL_H__
#ifdef _RWKERNEL_

#endif
	return;
}
