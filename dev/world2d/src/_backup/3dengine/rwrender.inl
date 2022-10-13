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



namespace LGR
{
    namespace 2D
	{
		//================================================================================
		//
		//
		//================================================================================
		void RenderVertex( LG::Vertex2l vertex, LC::RGBAb color, float size, CPMatrixf* mat )
		{
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

		//================================================================================
		//
		//
		//================================================================================
		void RenderVertex( LG::Vertex2l vertex, LC::RGBAb color, float size, CPMatrixf* mat )
		{



		}

	};//namespce 2D

};//namespace LGR


