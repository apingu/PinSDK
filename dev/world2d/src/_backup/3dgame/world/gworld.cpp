//G3DWorld.h
//
//    streaming world
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
#include "GWorld.h"
#include <stdlib.h>
#include <PGSPFile.h>
#include <Debug.h>
#include <aIntersect.h>




//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CGWorld::CGWorld()
{
	m_MainRole = NULL;
	m_MainCamera = NULL;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CGWorld::~CGWorld()
{
	Release();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CGWorld::Release( void )
{
    for( UINT l=0; l<GetLightCount(); l++ )
	{
		delete GetLight(l);
	}
	m_pLightVec.clear();

    for( UINT i=0; i<GetEntityCount(); i++ )
	{
		delete GetEntity(i);
	}
	m_pEntityVec.clear();
	m_MainRole = NULL;
	m_Sky.Destroy();
	VfSector::Release();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CGWorld::DeleteAllLight( void )
{
    for( UINT i = 0;i < GetLightCount();i++ )
	{
		delete GetLight(i);
	}
	m_pLightVec.clear();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfError CGWorld::Create(VfBBox bBox)
{
	int ret = VfSector::Create( bBox );
	//Add camera if already add camera
	for (UINT c = 0; c < m_pCameraVec.size(); c++)
	{
		RpWorldAddCamera( m_pWorld, m_pCameraVec[c]->m_pCamera );
	}

	//Add light if already add light
	for (UINT l = 0; l < m_pLightVec.size(); l++)
	{
		RpWorldAddLight( m_pWorld, m_pLightVec[l]->m_pLight );
	}
	return ret;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfError CGWorld::Create(Pcstr fname)
{
	int ret = VfSector::Create( fname );
	//Add camera if already add camera
	for (UINT c = 0; c < m_pCameraVec.size(); c++)
	{
		RpWorldAddCamera( m_pWorld, m_pCameraVec[c]->m_pCamera );
	}

	//Add light if already add light
	for (UINT l = 0; l < m_pLightVec.size(); l++)
	{
		RpWorldAddLight( m_pWorld, m_pLightVec[l]->m_pLight );
	}
	return ret;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CGWorld::LoadSky(Pcstr szPathname)
{
	int ret = m_Sky.Load(szPathname);
	return ret;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CGWorld::InsertRole( CP3DRole* role )
{
	if( RpWorldAddClump( m_pWorld, role->Model()->m_pClump ) == NULL )
		return 0;

	m_pEntityVec.push_back( role );
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CGWorld::InsertLight( VfLight* light )
{
	if( RpWorldAddLight( m_pWorld, light->m_pLight ) == NULL )
		return 0;

	m_pLightVec.push_back( light );
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CGWorld::InsertCamera( VfCamera* camera )
{
	if( RpWorldAddCamera( m_pWorld, camera->m_pCamera ) == NULL )
		return 0;

	m_pCameraVec.push_back( camera );
	return 1;
}


int CGWorld::RemoveRole( CP3DRole* role )
{
	for( UINT i=0; i<m_pEntityVec.size(); i++ )
	{
		if( m_pEntityVec[i] == role )
		{
			m_pEntityVec.erase( m_pEntityVec.begin()+i );
			RpWorldRemoveClump( m_pWorld, role->Model()->m_pClump );
			return 1;
		}
	}
	return 0;
}


int CGWorld::RemoveLight( VfLight* light )
{
	for( UINT i=0; i<m_pLightVec.size(); i++ )
	{
		if( m_pLightVec[i] == light )
		{
			m_pLightVec.erase( m_pLightVec.begin()+i );
			RpWorldRemoveLight( m_pWorld, light->m_pLight );
			return 1;
		}
	}
	return 0;
}


int CGWorld::RemoveCamera( VfCamera* camera )
{
	for( UINT i=0; i<m_pCameraVec.size(); i++ )
	{
		if( m_pCameraVec[i] == camera )
		{
			m_pCameraVec.erase( m_pCameraVec.begin()+i );
			RpWorldRemoveCamera( m_pWorld, camera->m_pCamera );
			return 1;
		}
	}
	return 0;
}


class GWorldCfg : public CPGSPFile
{
public:

	struct lightdata
	{
		UINT    type;
		LC::RGBAf color;
	};

	CGWorld * m_pWorld;

    void* OnCreateObject( const char* name )
	{
		if( strcmp( name, "3DWORLD" ) == 0 )
		{
			return m_pWorld;
		}
		else if( strcmp( name, "3DROLE" ) == 0 )
		{
			CP3DRole* role  = new CP3DRole;
			return role;
		}
		else if( strcmp( name, "LIGHT" ) == 0 )
		{
			lightdata* light  = new lightdata;
			return light;
		}

		return NULL;
	}

    void OnReadAttribute( const char* name,
                                     void* obj,
                                     char* attribute,
                                     char* value )
	{
		if( strcmp( name, "3DWORLD" ) == 0)
		{
			CGWorld* w =  (CGWorld*)obj;

			if( strcmp( attribute, "WORKFOLDER" ) == 0 )
			{
    			w->SetWorkFolder( value );
			}

			if( strcmp( attribute, "LOAD" ) == 0 )
			{
				MEM::String texf = m_pWorld->GetWorkFolder();	texf.cat( strtok( value, "," ) );
				MEM::String fname = m_pWorld->GetWorkFolder();	fname.cat( strtok( NULL, "," ) );

				CPTexture texture;
                texture.SetLocation( texf );
    			w->Create( fname );
			}
			else if( strcmp( attribute, "SKY" ) == 0 )
			{
				MEM::String texf = m_pWorld->GetWorkFolder(); texf.cat( strtok( value, "," ) );
				MEM::String fname = m_pWorld->GetWorkFolder(); fname.cat( strtok( NULL, "," ) );

				CPTexture texture;
                texture.SetLocation( texf );
    			w->LoadSky( fname );
			}

		}

		else if( strcmp( name, "3DROLE" ) == 0 )
		{
			CP3DRole* r =  (CP3DRole*)obj;
			if( strcmp( attribute, "ID" ) == 0 )
			{
				r->ID = atoi( value );
			}
			else if( strcmp( attribute, "LOAD" ) == 0 )
			{
				MEM::String texf = m_pWorld->GetWorkFolder(); texf.cat( strtok( value, "," ) );
				MEM::String fname = m_pWorld->GetWorkFolder(); fname.cat( strtok( NULL, "," ) );

				CPTexture texture;
                texture.SetLocation( texf );
    			r->LoadModel( fname );
			}

			else if( strcmp( attribute, "UP" ) == 0 )
			{
				LG::Vertex3f up;
				up.x = atof( strtok( value, "," ) );
				up.y = atof( strtok( NULL, "," ) );
				up.z = atof( strtok( NULL, "," ) );
				r->Model()->GetMatrix();
			}

			else if( strcmp( attribute, "RIGHT" ) == 0 )
			{
				r->Model()->Right()->x = atof( strtok( value, "," ) );
				r->Model()->Right()->y = atof( strtok( NULL, "," ) );
				r->Model()->Right()->z = atof( strtok( NULL, "," ) );
			}

			else if( strcmp( attribute, "AT" ) == 0 )
			{
				r->Model()->At()->x = atof( strtok( value, "," ) );
				r->Model()->At()->y = atof( strtok( NULL, "," ) );
				r->Model()->At()->z = atof( strtok( NULL, "," ) );
			}

			else if( strcmp( attribute, "POS" ) == 0 )
			{
				LG::Vertex3f pos;
				pos.x = atof( strtok( value, "," ) );
				pos.y = atof( strtok( NULL, "," ) );
				pos.z = atof( strtok( NULL, "," ) );
				r->Model()->Translate( &pos );
			}

		}

		else if( strcmp( name, "LIGHT" ) == 0 )
		{
			lightdata *light = (lightdata*)obj;
			if( strcmp( attribute, "TYPE" ) == 0 )
			{
				light->type = atoi( value );
			}
			else if( strcmp( attribute, "COLOR" ) == 0 )
			{
				light->color.R = atof( strtok( value, "," ) );
    			light->color.G = atof( strtok( NULL, "," ) );
				light->color.B = atof( strtok( NULL, "," ) );
				light->color.A = atof( strtok( NULL, "," ) );
			}
	
		}
	}

	void OnCloseObject( const char* name, void* obj )
	{
		if( strcmp( name, "3DROLE" ) == 0 )
		{
			CP3DRole* r =  (CP3DRole*)obj;
			m_pWorld->InsertRole( r );
		}
		else if( strcmp( name, "LIGHT" ) == 0 )
		{
			lightdata *l = (lightdata*)obj;
		    VfLight* light  = new VfLight;
            light->Create( l->type );
			light->SetColor( l->color );
            m_pWorld->InsertLight( light );
			delete l;
		}
	}

};

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CGWorld::Load( Pcstr szPathname )
{
	GWorldCfg worldcfg;

	worldcfg.m_pWorld = this;

	worldcfg.Load( szPathname );
	if( GetEntityCount() >0 )
	    SetFocusRole( GetEntity(0) );
	return;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CGWorld::Save( Pcstr szPathname )
{
	GWorldCfg writer;

	char buf[255];

	writer.Open( szPathname );

	writer.CreateObject("3DWORLD");

	if( !m_WorkFolder.empty() )
		writer.SetReadAttribute( "WORKFOLDER", m_WorkFolder );

	char *texf= (char*)texfname(),
		 *fname=(char*)filename();
    if( !m_WorkFolder.empty() )
	{
		texf=strstr( texfname(), m_WorkFolder );
		if( texf!=NULL )
		    texf+= m_WorkFolder.len();
		fname=strstr( filename(), m_WorkFolder );
		if( fname!=NULL )
		    fname+= m_WorkFolder.len();
	}
    sprintf( buf, "%s,%s", texf, fname );

	writer.SetReadAttribute( "LOAD", buf );

	if( !m_Sky.empty() )
	{
		texf=m_Sky.texfname(),
		fname=m_Sky.filename();
	    if( !m_WorkFolder.empty() )
		{
		    texf=strstr( m_Sky.texfname(), m_WorkFolder );
		    if( texf!=NULL )
		        texf+= m_WorkFolder.len();
		    fname=strstr( m_Sky.filename(), m_WorkFolder );
		    if( fname!=NULL )
		        fname+= m_WorkFolder.len();
		}
		sprintf( buf, "%s,%s", texf, fname );
	    writer.SetReadAttribute( "SKY", buf );
	}

	for( UINT l = 0; l< m_pLightVec.size(); l++ )
	{
		VfLight* light = m_pLightVec[l];
		writer.CreateObject("LIGHT");

		writer.SetReadAttribute( "TYPE", itoa( light->GetType(), buf, 10 ) );

		LC::RGBAf color = light->GetColor();
	    sprintf( buf, "%f,%f,%f,%f", color.R,
			                         color.G,
								     color.B,
								     color.A );

		writer.SetReadAttribute( "COLOR", buf );

		writer.CloseObject();
	}

	for( UINT i = 0; i< GetEntityCount(); i++ )
	{
		writer.CreateObject("3DROLE");
		itoa( GetEntity(i)->ID, buf, 10 );
		writer.SetReadAttribute( "ID", buf );

		texf=GetEntity(i)->Model()->texfname(),
		fname=GetEntity(i)->Model()->filename();
	    if( !m_WorkFolder.empty() )
		{
		    texf=strstr( GetEntity(i)->Model()->texfname(), m_WorkFolder );
		    if( texf!=NULL )
		        texf+= m_WorkFolder.len();
		    fname=strstr( GetEntity(i)->Model()->filename(), m_WorkFolder );
		    if( fname!=NULL )
		        fname+= m_WorkFolder.len();
		}
		sprintf( buf, "%s,%s", texf, fname );

		writer.SetReadAttribute( "LOAD", buf );


		sprintf( buf, "%f,%f,%f", GetEntity(i)->Model()->Up()->x,
		                          GetEntity(i)->Model()->Up()->y,
								  GetEntity(i)->Model()->Up()->z );
		writer.SetReadAttribute( "UP", buf );

		sprintf( buf, "%f,%f,%f", GetEntity(i)->Model()->Right()->x,
		                          GetEntity(i)->Model()->Right()->y,
								  GetEntity(i)->Model()->Right()->z );
		writer.SetReadAttribute( "RIGHT", buf );

		sprintf( buf, "%f,%f,%f", GetEntity(i)->Model()->At()->x,
		                          GetEntity(i)->Model()->At()->y,
								  GetEntity(i)->Model()->At()->z );
		writer.SetReadAttribute( "AT", buf );

		sprintf( buf, "%f,%f,%f", GetEntity(i)->Model()->Position()->x,
		                          GetEntity(i)->Model()->Position()->y,
								  GetEntity(i)->Model()->Position()->z );
		writer.SetReadAttribute( "POS", buf );
		writer.CloseObject();
	}
	writer.CloseObject();

	return;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CGWorld::Render( void )
{
	if( m_Sky.m_pClump != NULL )
	{
	    RpClumpRender( m_Sky.m_pClump );
		RwCameraClear( m_MainCamera->m_pCamera, 0, rwCAMERACLEARZ);
	}
	if( m_pWorld != NULL )
	{
        RpWorldRender( m_pWorld );
	    return 1;
	}
	return 0;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpWorldSector* SectorPolygonsRender(RpWorldSector* worldSector, void* data)
{
	LC::RGBAb* color = (LC::RGBAb*) data;

	for (int i = 0; i < worldSector->numPolygons; i++)
	{
		RwIm3DVertex imVertex[3];
		RwImVertexIndex indices[6] =
		{
			0, 1, 1, 2, 2, 0
		};

		RwIm3DVertexSetPos(&imVertex[0],
			worldSector->vertices[worldSector->polygons[i].vertIndex[0]].x,
			worldSector->vertices[worldSector->polygons[i].vertIndex[0]].y,
			worldSector->vertices[worldSector->polygons[i].vertIndex[0]].z);
		RwIm3DVertexSetRGBA(&imVertex[0], color->R, color->G, color->B,
			color->A);
		RwIm3DVertexSetPos(&imVertex[1],
			worldSector->vertices[worldSector->polygons[i].vertIndex[1]].x,
			worldSector->vertices[worldSector->polygons[i].vertIndex[1]].y,
			worldSector->vertices[worldSector->polygons[i].vertIndex[1]].z);
		RwIm3DVertexSetRGBA(&imVertex[1], color->R, color->G, color->B,
			color->A);
		RwIm3DVertexSetPos(&imVertex[2],
			worldSector->vertices[worldSector->polygons[i].vertIndex[2]].x,
			worldSector->vertices[worldSector->polygons[i].vertIndex[2]].y,
			worldSector->vertices[worldSector->polygons[i].vertIndex[2]].z);
		RwIm3DVertexSetRGBA(&imVertex[2], color->R, color->G, color->B,
			color->A);

		if (RwIm3DTransform(imVertex, 3, NULL, rwIM3D_ALLOPAQUE))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 6);
			//RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
			RwIm3DEnd();
		}
	}
	return worldSector;
}
#endif

int CGWorld::RenderMesh( const LC::RGBAb* color )
{
	if(m_pWorld != NULL)
		RpWorldForAllWorldSectors( m_pWorld, SectorPolygonsRender, &color);

	return 1;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CGWorld::Update( void )
{
	if( ( m_MainCamera != NULL ) && (!m_Sky.empty()) )
	    m_Sky.ReTranslate( m_MainCamera->Position() ); 

	for (Puint i = 0; i < m_pEntityVec.size(); i++)
	{
		m_pEntityVec[i]->UpdateAnim(0.01f);
	}
	return;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpCollisionTriangle* LineCollisionCB(RpIntersection* intersection, 
									 RpWorldSector* sector, 
									 RpCollisionTriangle* collTriangle, 
									 RwReal dist, void *data)
{
	PA::CollData* cosTri = (PA::CollData*)data; 
	if( cosTri->Dist > dist )
	{
		cosTri->IsCollision=true;
		cosTri->Dist=dist;
		cosTri->Normal.x = collTriangle->normal.x;
		cosTri->Normal.y = collTriangle->normal.y;
		cosTri->Normal.z = collTriangle->normal.z;
		cosTri->Poly.vertex[0].x = collTriangle->vertices[0]->x;
		cosTri->Poly.vertex[0].y = collTriangle->vertices[0]->y;
		cosTri->Poly.vertex[0].z = collTriangle->vertices[0]->z;
		cosTri->Poly.vertex[1].x = collTriangle->vertices[1]->x;
		cosTri->Poly.vertex[1].y = collTriangle->vertices[1]->y;
		cosTri->Poly.vertex[1].z = collTriangle->vertices[1]->z;
		cosTri->Poly.vertex[2].x = collTriangle->vertices[2]->x;
		cosTri->Poly.vertex[2].y = collTriangle->vertices[2]->y;
		cosTri->Poly.vertex[2].z = collTriangle->vertices[2]->z;
	}
    return collTriangle;
}
#endif//_RWKERNEL_

bool CGWorld::RayHitTest( const LG::Line3df &ray, LG::Vertex3f& cosPos )
{
	PA::CollData    cosData;
	LG::Triangle3df cosTri;

#ifdef _RWKERNEL_
	cosData.Poly.Allot(3);

    RpIntersection intersection;

    // test front for ground collision
    // move and rotate the collision points to vehicles position and orientation
    intersection.type = rpINTERSECTLINE;

    // fill in intersection object's data 
    intersection.t.line.start.x = ray.vertex[0].x;
	intersection.t.line.start.y = ray.vertex[0].y;
	intersection.t.line.start.z = ray.vertex[0].z;
    intersection.t.line.end.x   = ray.vertex[1].x;
	intersection.t.line.end.y   = ray.vertex[1].y;
	intersection.t.line.end.z   = ray.vertex[1].z;
	cosData.Dist = 1;

    // test for intersection
    RpCollisionWorldForAllIntersections( m_pWorld, &intersection, 
                                         LineCollisionCB, 
                                         &cosData );
#endif//_RWKERNEL_
	if( cosData.IsCollision == true )
	{
    	LG::Vector3f v;
		float len = VertexToVertex( ray.vertex[1], ray.vertex[0] ) * cosData.Dist;
		cosPos.x = ray.vertex[0].x + ((ray.vertex[1].x - ray.vertex[0].x) * cosData.Dist);
		cosPos.y = ray.vertex[0].y + ((ray.vertex[1].y - ray.vertex[0].y) * cosData.Dist);
		cosPos.z = ray.vertex[0].z + ((ray.vertex[1].z - ray.vertex[0].z) * cosData.Dist);
	}
	return cosData.IsCollision;
}