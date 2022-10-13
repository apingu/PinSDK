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
//
//
#include "3DEditerTools.h"




///////////////////////////////////////////////////////////////////////////////
//
void RenderOriginAxis( LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float len )
{
	PA::Matrixf mat;
	mat.a[0] = len;
	mat.a[5] = len;
	mat.a[10] = len;
    RenderAxis( mat, u, r, a, 10 );
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderModelUnitAxis( CPModel* model, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a )
{
	if( model == NULL )
		return;


	LG::Spheref bsphere = model->GetBSphere();
	
	RenderUnitAxis( *(model->GetMatrix()), u, r, a, (float)(bsphere.radius*1.2), bsphere.radius );
	//for( int jj = 0; jj < model->GetObjectCount(); jj++ )
	//{
		//model->Object(0)->LocalBoundSphere( &bsphere );
	    //DrawUnitAxis( *(model->Object(0)->GetMatrix()), u, r, a, bsphere.radius*1.5, floor(bsphere.radius/5) );
		//DrawAxis( *(model->GetMatrix()), u, r, a, 3 );
	//}
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderModelBoundingBox( CPModel* model, LC::RGBAb color )
{
    if( model == NULL )
		return;

	//VfBBox    bbox = *model->GetBBox();//Object(jj)->LocalBoundBox( &bbox );
	RenderCube( model->GetBBox(), color, 2, model->GetMatrix() );

}


///////////////////////////////////////////////////////////////////////////////
//
void RenderModelBoundingSphere( CPModel* model, LC::RGBAb color, PA::Matrixf* mat )
{
	if( model == NULL )
		return;

	model->UpdateBoundSphere();
	RenderSphere( model->GetBSphere(), color, 2, mat );

	/*
	VfBSphere   bsphere;
	for( int jj = 0; jj < model->GetObjectCount(); jj++ )
	{
		//model->Object(jj)->LocalBoundSphere( &bsphere );
	    RenderCircle( model->GetLocalSphere(), color, 5, model->Object(jj)->GetMatrix() );
	}
	*/
}

void RenderModelSelectSphere( CPModel* model, LC::RGBAb color, PA::Matrixf* mat )
{
	if( model == NULL )
		return;

	model->UpdateBoundSphere();
	RenderSphere( model->GetBSphere(), color, 5, mat );

}


///////////////////////////////////////////////////////////////////////////////
//
/*
/*
 * Point light properties...
 */ 
//#define AMBIENT_LIGHT (0)
//#define POINT_LIGHT (1)
//#define DIRECT_LIGHT (2)
//#define SPOT_LIGHT (3)
//#define SOFTSPOT_LIGHT (4)
/*
 * Point light properties...
 */ 
#define POINT_LIGHT_RADIUS_FACTOR (0.1f)
#define POINT_LIGHT_NUM_VERTICES (50)

/* 
 * Direct light cylinder properties...
 */ 
#define DIRECT_LIGHT_CYLINDER_LENGTH (160.0f)
#define DIRECT_LIGHT_CYLINDER_DIAMETER (48.0f)     
#define DIRECT_LIGHT_NUM_VERTICES (20)

/* 
 * Direct light cone properties...
 */ 
#define DIRECT_LIGHT_CONE_SIZE (96.0f)     
#define DIRECT_LIGHT_CONE_ANGLE (45.0f)    

/* 
 * Spot light properties...
 */
#define SPOT_LIGHTS_RADIUS_FACTOR (0.2f)//4

/*
 * Effects both spot lights & the direct light... 
 */
#define CONE_NUM_VERTICES (10)


RwRGBA LightSolidColor = {255, 255, 0, 255};
RwReal LightRadius = 500.0f;
//RwInt32 LightTypeIndex = 1;
RwReal LightConeAngle = 45.0f;

 
static void 
DrawDirectLight(LGR::Light* CurrentLight)
{
    /*
     * Draw cylinder behind cone -> to produce 3D arrow...
     */
    RwMatrix *matrix;
    RwV3d *right, *up, *at, *pos;
    RwV3d point;
    RwReal cosValue, sinValue;
    RwV3d dRight, dUp, dAt;
    RwIm3DVertex cone[(DIRECT_LIGHT_NUM_VERTICES*2)+1];
    RwImVertexIndex indices[DIRECT_LIGHT_NUM_VERTICES*3];
    RwInt32 i;

    matrix = RwFrameGetLTM(RpLightGetFrame(CurrentLight->m_pLight));

    right = RwMatrixGetRight(matrix);
    up    = RwMatrixGetUp(matrix);
    at    = RwMatrixGetAt(matrix);
    pos   = RwMatrixGetPos(matrix);
      
    for(i=0; i<(DIRECT_LIGHT_NUM_VERTICES*2); i+=2)
    {
        cosValue =
            (RwReal)(RwCos(i/(DIRECT_LIGHT_NUM_VERTICES/2.0f) * rwPI));
        sinValue =
            (RwReal)(RwSin(i/(DIRECT_LIGHT_NUM_VERTICES/2.0f) * rwPI));

        RwV3dScale(&dUp, up, sinValue * DIRECT_LIGHT_CYLINDER_DIAMETER);
        
        RwV3dScale(&dRight, right, cosValue * DIRECT_LIGHT_CYLINDER_DIAMETER);

        RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CONE_SIZE + 1.0f));

        /*
         * Cylinder base vertices...
         */
        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;

        RwIm3DVertexSetPos(&cone[i], point.x, point.y, point.z);
                       
        /*
         *  Cylinder top vertices 
         */
        RwV3dScale(&dAt, at, 
            -(DIRECT_LIGHT_CYLINDER_LENGTH + DIRECT_LIGHT_CONE_SIZE) );
            
        point.x = pos->x + dAt.x + dUp.x + dRight.x;
        point.y = pos->y + dAt.y + dUp.y + dRight.y;
        point.z = pos->z + dAt.z + dUp.z + dRight.z;
        
        RwIm3DVertexSetPos(&cone[i+1], point.x, point.y, point.z);
    }

    /*
     * Set color & alpha of all points...
     */
    for(i=0; i<(2*DIRECT_LIGHT_NUM_VERTICES)+1; i++)
    {
        RwIm3DVertexSetRGBA(&cone[i],
            LightSolidColor.red, LightSolidColor.green,
            LightSolidColor.blue, 128);
    }
    
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

    if( RwIm3DTransform(cone, 2*DIRECT_LIGHT_NUM_VERTICES, NULL, 0) )
    {
        /*
         * Draw cylinder...
         */
        RwIm3DRenderPrimitive(rwPRIMTYPETRISTRIP);
        
        /*
         * Close cylinder...
         */
        RwIm3DRenderTriangle((2*DIRECT_LIGHT_NUM_VERTICES)-2,
            (2*DIRECT_LIGHT_NUM_VERTICES)-1,0);

        RwIm3DRenderTriangle((2*DIRECT_LIGHT_NUM_VERTICES)-1,1,0);        
        
        RwIm3DEnd();
    }
    
    for(i=0; i<(DIRECT_LIGHT_NUM_VERTICES*2)+1; i++)
    {
        RwIm3DVertexSetRGBA(&cone[i],
            LightSolidColor.red, LightSolidColor.green,
            LightSolidColor.blue, 255); 
    }

    /*
     * Set cylinder base center point...
     */    
    RwV3dScale(&dAt, at, 
        -(DIRECT_LIGHT_CYLINDER_LENGTH + DIRECT_LIGHT_CONE_SIZE));

    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[DIRECT_LIGHT_NUM_VERTICES*2], 
        point.x, point.y, point.z);    
    
    /* 
     * Set up vertex list...
     */
    for(i=0; i<DIRECT_LIGHT_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)DIRECT_LIGHT_NUM_VERTICES*2;
        indices[(i*3)+1] = (RwImVertexIndex)((i+1)*2) +1;    /* 3, 5, 7, 9, etc.*/
        indices[(i*3)+2] = (RwImVertexIndex)(i*2)+1;         /* 1, 3, 5, 7, etc.*/       
    }
    
    indices[(DIRECT_LIGHT_NUM_VERTICES*3)-2] = 1;

    /*
     * Draw base...
     */
    if( RwIm3DTransform(cone, (2*DIRECT_LIGHT_NUM_VERTICES)+1, NULL,
            rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, 
            indices, DIRECT_LIGHT_NUM_VERTICES*3);
        
        RwIm3DEnd();
    }  
     
    /*
     * Set cylinder top center point...
     */    
    RwV3dScale(&dAt, at, -(DIRECT_LIGHT_CONE_SIZE + 1.0f));
    point.x = pos->x + dAt.x;
    point.y = pos->y + dAt.y;
    point.z = pos->z + dAt.z;
    RwIm3DVertexSetPos(&cone[DIRECT_LIGHT_NUM_VERTICES*2], 
        point.x, point.y, point.z);    

    /* 
     * Set up vertex list...
     */
    for(i=0; i<DIRECT_LIGHT_NUM_VERTICES; i++)
    {
        indices[(i*3)]   = (RwImVertexIndex)(DIRECT_LIGHT_NUM_VERTICES*2);
        indices[(i*3)+1] = (RwImVertexIndex)i*2;         /* 0, 2, 4, 6, etc.*/
        indices[(i*3)+2] = (RwImVertexIndex)(i+1)*2;     /* 2, 4, 6, 8, etc.*/               
    }
    indices[(DIRECT_LIGHT_NUM_VERTICES*3)-1] = 0;
    
    /*
     * Draw cylinder top...
     */
    if( RwIm3DTransform(cone, (2*DIRECT_LIGHT_NUM_VERTICES)+1, NULL,
            rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, 
            indices, DIRECT_LIGHT_NUM_VERTICES*3);

        RwIm3DEnd();
    }

    /*
     * Draw inverted cone to act as arrow head...
     */
    //DrawCone( CurrentLight, DIRECT_LIGHT_CONE_ANGLE, DIRECT_LIGHT_CONE_SIZE, -2.0f);
	RenderCone( (PA::Matrixf*)RwFrameGetLTM(RpLightGetFrame(CurrentLight->m_pLight)),
		        DIRECT_LIGHT_CONE_ANGLE, DIRECT_LIGHT_CONE_SIZE, -2.0f);

    return;
}


/*
 *****************************************************************************
 */
static void 
DrawPointLight(LGR::Light* CurrentLight)
{
    RwIm3DVertex shape[POINT_LIGHT_NUM_VERTICES];
    RwV3d point, *pos;
    RwMatrix *matrix;
    RwInt32 i;

    matrix = RwFrameGetLTM(RpLightGetFrame(CurrentLight->m_pLight));
    pos = RwMatrixGetPos(matrix);

    for(i=0; i<POINT_LIGHT_NUM_VERTICES; i++)
    {
        point.x = pos->x +
            (RwReal)RwCos(i/(POINT_LIGHT_NUM_VERTICES/2.0f) * rwPI) * 
            LightRadius * POINT_LIGHT_RADIUS_FACTOR;

        point.y = pos->y +
            (RwReal)RwSin(i/(POINT_LIGHT_NUM_VERTICES/2.0f) * rwPI) * 
            LightRadius * POINT_LIGHT_RADIUS_FACTOR;
        
        point.z = pos->z;

        RwIm3DVertexSetRGBA(&shape[i],
            LightSolidColor.red, LightSolidColor.green,
            LightSolidColor.blue, LightSolidColor.alpha);

        RwIm3DVertexSetPos(&shape[i], point.x, point.y, point.z);
    }

    if( RwIm3DTransform(shape, POINT_LIGHT_NUM_VERTICES, NULL,
            rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DRenderLine(POINT_LIGHT_NUM_VERTICES-1, 0);

        RwIm3DEnd();
    }

    return;
}


/*
 *****************************************************************************
 */
void
DrawCurrentLight(LGR::Light* light)
{
    static RwBool stateSet = FALSE;

    if( !stateSet )
    {
        /*
         * Because the clump's or Im3D rendering is not using any textures
         * this render state only got to be set once...
         */
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

        stateSet = TRUE;
    }

    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);

    switch( light->GetType() )
    {
        case Vf_DIRECTION:
        {
            DrawDirectLight( light );
            break;
        }

        case Vf_SPOT:
        case Vf_SPOT_SOFT:
        {
            //DrawCone( light, LightConeAngle, 
            //    LightRadius * SPOT_LIGHTS_RADIUS_FACTOR, 1.0f);
			//RenderCone( (PA::Matrixf*)RwFrameGetLTM(RpLightGetFrame(light->m_pLight)),
		    //   DIRECT_LIGHT_CONE_ANGLE, DIRECT_LIGHT_CONE_SIZE, 2.0f);
			//float f = (light->GetConeAngle()/3.1415)*180;
			RenderCone( (PA::Matrixf*)RwFrameGetLTM(RpLightGetFrame(light->m_pLight)),
		      ( (light->GetConeAngle()/3.1415)*180 ), DIRECT_LIGHT_CONE_SIZE, 2.0f);

			

			//float f = 42.9731;
			//RenderCone( (PA::Matrixf*)RwFrameGetLTM(RpLightGetFrame(light->m_pLight)),
		        //42.97, DIRECT_LIGHT_CONE_SIZE, -2.0f);

            break;
        }

        case Vf_LIGHTPOINT:
        {
            DrawPointLight( light );
            break;
        }
    }

    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

    return;
}


/*
///////////////////////////////////////////////////////////////////////////////
//
void RenderWorldLights( CP3DWorld* world, LC::RGBAb color )
{
	for( Puint i = 0; i < world->GetLightCount(); i++ )
	{
		if( world->GetLight(i)->getType() != Vf_AMBIENT )
		{
			RenderPyramid( LG::Spheref( *(world->getLight(i)->Position()), 5 ), 10,
				           color, 2, world->getLight(i)->getMatrix() );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
void RenderWorldCameras( CP3DWorld* world, LC::RGBAb color )
{





}
*/
