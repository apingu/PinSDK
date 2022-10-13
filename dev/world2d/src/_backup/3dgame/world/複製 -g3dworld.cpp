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
#include "G3DWorld.h"
#include <PFile.h>
#include <Debug.h>


#ifdef _RWKERNEL_
#include <RwFile.h>  //3DEngine
RpAtomic* ClumpGetAtomics(RpAtomic* atomic, void* data);
int RwStreamingInsertCamera(CPG3Sector* sector, VfCamera* camera);
int RwStreamingRemoveCamera(CPG3Sector* sector, Puint index);
int RwStreamingInsertLight(CPG3Sector* sector, VfLight* camera);
int RwStreamingRemoveLight(CPG3Sector* sector, Puint index);
#endif

#define _G3ROLE( e )	 ( (CPG3Role*)e )

namespace P3DWorld
{
    Pbool getRoleCallback(int id, void* pData)
	{
	    CPG3Role* r = (CPG3Role*) pData;
	    return (id == r->ID) ? TRUE : FALSE;
	}
	
	struct sector_t
	{
		void*       pData;
		void*       pUserData;
		CPString    Name;
		Puint       cx;
		Puint       cy;
	};
	#define DELETE_SECTOR_T( s )    pData=NULL; pUserData=NULL; delete s


	struct role_t
	{
		//CPG3Sector* pSector;
		void*       pData;
		void*       pUserData;
		sector_t*   pSector;
	};
	#define DELETE_ROLE_T( r )    r->pData=NULL; r->pUserData=NULL; r->pSector=NULL; delete r 

	
	struct camera_t
	{
		void*       pData;
		void*       pUserData;
		sector_t*   pSector;
		DataIter    Iter;
		
	};
	#define DELETE_CAMERA_T( c )    c->pData=NULL; c->pUserData=NULL; c->pSector=NULL; c->Iter=NULL; delete c	

	
	struct light_t
	{
		void*       pData;
		void*       pUserData;
		DataIter    Iter;
	};
	
	#define DELETE_LIGHT_T( l )    l->pData=NULL; l->pUserData=NULL; l->Iter=NULL; delete l


}



#define SECTORTABLEID  0

//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CP3DWorld::CP3DWorld()
{
	m_pSectorVec.clear();
	m_pCameraVec.clear();
	m_pLightVec.clear();
	m_Sectorfname.clear();
	m_pSky = NULL;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
CP3DWorld::~CP3DWorld()
{
	SAFE_DELETE(m_pSky);
	Destroy();
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CP3DWorld::Destroy(void)
{
	RemoveAllRole();
	RemoveAllLight();
	RemoveAllCamera();
	DeleteAllSector();
	
	for (Puint s = 0; s < m_Sectorfname.size(); s++)
		delete [] m_Sectorfname[s];
	m_Sectorfname.clear();
}




//=============================================================================
//  Name :
//  Desc :
//=============================================================================
VfError CP3DWorld::Create( CPBBox bBox, Puint cx, Puint cy, Puint cz )
{
	m_fname.clear();
	DeleteAllSector();
	
	InsertBoard( SECTORTABLEID, cx, cy, );

	Pfloat gx = ((bBox.vertex[1].x - bBox.vertex[0].x)) / (float) cx;
	Pfloat gy = ((bBox.vertex[1].y - bBox.vertex[0].y)) / (float) cy;
	Pfloat gz = ((bBox.vertex[1].z - bBox.vertex[0].z)) / (float) cz;

	//add new sector
	for (Puint z = 0; z < cz; z++)
	{
		for (Puint y = 0; y < cy; y++)
		{
			for (Puint x = 0; x < cx; x++)
			{
				VfBBox box;
				box.vertex[0].x = bBox.vertex[0].x + (x * gx);
				box.vertex[0].y = bBox.vertex[0].y + (y * gy);
				box.vertex[0].z = bBox.vertex[0].z + (z * gz);
				box.vertex[1].x = bBox.vertex[0].x + ((x + 1) * gx);
				box.vertex[1].y = bBox.vertex[0].y + ((y + 1) * gy);
				box.vertex[1].z = bBox.vertex[0].z + ((z + 1) * gz);

				CPG3Sector* sector = new CPG3Sector;
				if (sector->Create(box) != 1)
					return VFE_CREATEWORLDFAIL;
				m_pSectorVec.push_back(sector);
			}
		}
	}


	// add new chessboard
	InsertBoard( "SECTORTABLE", cx, cy );

	return VFE_SUCCESS;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
VfError CP3DWorld::Create(Pcstr szPathname)
{
	m_fname = szPathname;
	DeleteAllSector();
	/*
	CPFile wf;
	wf.open( szPathname );
	for( int sp=0; sp< wf.FileCount(); sp++ )
	{
		char fname[_MAX_FNAME];
		wf.FileName( sp, fname, _MAX_FNAME );
		Pstr name = new char[strlen(fname)];
		strcpy( name, fname );
		m_Sectorfname.push_back( name );
	}
	*/

	return VFE_SUCCESS;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::SectorInsertAllObject( GSECTOR sector )
{
	//for (Puint u = 0; u < EntityCount(); u++)
	//	InsertRole( _G3ROLE( operator[](u) ) );


/*
#ifdef _RWKERNEL_

	//Add camera if already add camera
	for (Puint c = 0; c < m_pCameraVec.size(); c++)
	{
		RwStreamingInsertCamera(sector, m_pCameraVec[c]);
	}

	//Add light if already add light
	for (Puint l = 0; l < m_pLightVec.size(); l++)
	{
		RwStreamingInsertLight(sector, m_pLightVec[l]);
	}
#endif
*/
}


////////////////////////////////////////////////////////////////////////////////////////////
// Sky member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::LoadSky(Pcstr szPathname)
{
	m_pSky = new CPModel;
	return m_pSky->Load(szPathname);
}






////////////////////////////////////////////////////////////////////////////////////////////
// Role member

//==========================================================================================
//  Name : RoleInsert()
//  Desc : Plant a role into this world
//==========================================================================================
GROLE CP3DWorld::InsertRole( void* role, VfVertex3f pos )
{
	//set map pointer
	//CPVertex G_Site = Map_to_Chessboard_Site( role->Map_Situs() );
	//m_MapObject.Insert( role, G_Site.x, G_Site.y );
	if (role == NULL)
		return 0;

	P3DWorld::role_t* r = new P3DWorld::role_t;
	r->pData = role;
	r->pSector = findSector( pos );

	//role->setWorld(this);
	//setFocusEntity( role );
	InsertEntity( CPBuffer(r, sizeof(P3DWorld::role_t) ) );
	//role->Model()->Identity();
	return r;
}


//==========================================================================================
//  Name : RoleRemove()
//  Desc : Eradicate a role form this world
//==========================================================================================
#ifdef _RWKERNEL_
RpClump* RpWroldReleaseClumpCallBack(RpClump* clump, void* data)
{
	RpWorld* world = (RpWorld*) data;
	RpWorldRemoveClump(world, clump);
	return clump;
}
#endif

void* CP3DWorld::RemoveRole( GROLE role )
{
	P3DWorld::role_t* r = (P3DWorld::role_t*)role;

	if( RemoveEntity( r ) == NULL )
		return NULL;
		
	OnRemoveRole( this, r->pData, r->pUserData );
	
#ifdef _RWKERNEL_
	//for (Puint s = 0; s < m_pSectorVec.size(); s++)
	RpWorldForAllClumps( r->pSector->m_pWorld,
			RpWroldReleaseClumpCallBack, r->pSector->m_pWorld );
#endif

	void* Data = r->pData;
	memset( r, 0, sizeof( P3DWorld::role_t ) );
	SAFE_DELETE( r );
	return Data;
}


//==========================================================================================
//  Name : setFocusRole()
//  Desc : get a role form this world
//==========================================================================================
CPG3Role* CP3DWorld::setFocusRole(CPG3Role* frole)
{
	return _G3ROLE(setFocusEntity(frole));
}


//==========================================================================================
//  Name : getRole()
//  Desc : get a role form this world
//==========================================================================================
CPG3Role* CP3DWorld::getRole(int id)
{
	return _G3ROLE(Entity(id, P3DWorld::getRoleCallback));
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPG3Role* CP3DWorld::getFocusRole(void)
{
	return _G3ROLE(getFocusEntity());
}








////////////////////////////////////////////////////////////////////////////////////////////
// Camera member


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::InsertCamera(VfCamera* camera)
{
	m_pCameraVec.push_back(camera);

#ifdef _RWKERNEL_
	for (Puint i = 0; i < m_pSectorVec.size(); i++)
	{
		if (m_pSectorVec[i] != NULL)
		{
			RwStreamingInsertCamera(m_pSectorVec[i], camera);
		}
	}
#endif //_RWKERNEL_
	return m_pCameraVec.size();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfCamera* CP3DWorld::RemoveCamera(VfCamera* camera)
{
	for (Puint ci = 0; ci < m_pCameraVec.size(); ci++)
	{
		if (m_pCameraVec[ci] == camera)
		{
			m_pCameraVec.erase(m_pCameraVec.begin() + ci);

#ifdef _RWKERNEL_
			for (Puint si = 0; si < m_pSectorVec.size(); si++)
			{
				RwStreamingRemoveCamera(m_pSectorVec[si], ci);
			}
#endif
			return camera;
		}
	}



	return NULL;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::setMainCamera(VfCamera* camera)
{
	m_iMainCamera = -1;
	m_pMainCamera = camera;
	for (Puint ci = 0; ci < m_pCameraVec.size(); ci++)
	{
		if (camera == m_pCameraVec[ci])
		{
			m_iMainCamera = ci;
			break;
		}
	}
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfCamera* CP3DWorld::getMainCamera(void)
{
	return m_pMainCamera;
}








////////////////////////////////////////////////////////////////////////////////////////////
// Light member


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::InsertLight( VfLight* light )
{
	m_pLightVec.push_back(light);

#ifdef _RWKERNEL_
	for (Puint si = 0; si < m_pSectorVec.size(); si++)
	{
		RwStreamingInsertLight(m_pSectorVec[si], light);
	}
#endif //_RWKERNEL_
	return m_pLightVec.size();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfLight* CP3DWorld::RemoveLight( VfLight* light )
{
	for (Puint li = 0; li < m_pLightVec.size(); li++)
	{
		if (m_pLightVec[li] == light)
		{
			m_pLightVec.erase(m_pLightVec.begin() + li);
#ifdef _RWKERNEL_
			for (Puint si = 0; si < m_pSectorVec.size(); si++)
			{
				RwStreamingRemoveLight(m_pSectorVec[si], li);
			}
#endif
			return light;
		}
	}
	return NULL;
}





////////////////////////////////////////////////////////////////////////////////////////////
// Sector member


//==========================================================================================
//  Name : SectorInsert()
//  Desc :
//==========================================================================================
Pint CP3DWorld::InsertSector( Puint cx, Puint cy, CPG3Sector* sector )
{
	if( getData( SECTORTABLEID, cx, cy ) != NULL )
		return -1;

	setData( SECTORTABLEID, cx, cy, sector );
	
	//m_pSectorVec.push_back(sector);
	//SectorInsertAllObject(sector);
	OnInsertSector( this, sector, 
	return m_pSectorVec.size();
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPG3Sector* CP3DWorld::RemoveSector( Puint cx, Puint cy )
{
	CPG3Sector* sector = (CPG3Sector*)getData( SECTORTABLEID, cx, cy );
	setData( SECTORTABLEID, cx, cy, NULL );
	return sector;
}


////////////////////////////////////////////////////////////////////////////////////////////
// Update member


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::Update(void)
{
	for (Puint i = 0; i < this->EntityCount(); i++)
	{
		_G3ROLE(Entity(i))->UpdateAnim(0.01f);
	}
	return 1;
}






////////////////////////////////////////////////////////////////////////////////////////////
// Render member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::Render(void)
{
#ifdef _RWKERNEL_
	for (Puint i = 0; i < m_pSectorVec.size(); i++)
	{
		if (m_pSectorVec[i]->m_pWorld != NULL)
		{
			if (RwCameraBeginUpdate(m_pSectorVec[i]->m_pCameraVec[m_iMainCamera]))
			{
				RpWorldRender(m_pSectorVec[i]->m_pWorld);
				RwCameraEndUpdate(m_pSectorVec[i]->m_pCameraVec[m_iMainCamera]);
			}
		}
	}
#endif
	return;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::RenderRole(void)
{
	//for( int i=0; i<EntityCount(); i++ )
	//	m_pCameraVec[0]->Draw( getRole(i)->Model() );
	return;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpWorldSector* SectorPolygonsRender(RpWorldSector* worldSector, void* data)
{
	CPRGBA* color = (CPRGBA*) data;

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
void CP3DWorld::RenderMesh(VfRGBA color)
{
#ifdef _RWKERNEL_
	for (Puint i = 0; i < m_pSectorVec.size(); i++)
	{
		if (m_pSectorVec[i]->m_pWorld != NULL)
			RpWorldForAllWorldSectors(m_pSectorVec[i]->m_pWorld,
				SectorPolygonsRender, &color);
	}
#endif//_RWKERNEL_
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::RenderSky(VfVertex3f* pos)
{
	if (m_pSky == NULL)
		return;
	m_pSky->Translate(pos);
#ifdef _RWKERNEL_
	RpClumpRender(m_pSky->m_pClump);
#endif

	return;
}






////////////////////////////////////////////////////////////////////////////////////////////
// protected member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::DeleteAllSector(void)
{
	for (Puint s = 0; s < m_pSectorVec.size(); s++)
		delete m_pSectorVec[s];
	m_pSectorVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllCamera(void)
{
	for (Puint c = 0; c < m_pCameraVec.size(); c++)
		RemoveCamera( m_pCameraVec[c] );

	m_pCameraVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllLight(void)
{
	for (Puint l = 0; l < m_pLightVec.size(); l++)
		RemoveLight( m_pLightVec[l] );

	m_pLightVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllRole(void)
{
	for (Puint r = 0; r < EntityCount(); r++)
		RemoveRole( _G3ROLE(operator[](r)) );

	EntityClear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPG3Sector* CP3DWorld::findSector( VfVertex3f pos )
{
	return NULL;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::getNeighborhood(const CPAxis* centerpos,
	std::vector<CPG3Sector*>* neighborhood)
{
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////
// streaming member

//==========================================================================================
//  Name :
//  Desc :  set streaming follow position
//==========================================================================================
void CP3DWorld::setCenterAxis(CPAxis* axis)
{
	m_pCenterAxis = axis;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::StreamingUpdate(void)
{
	getNeighborhood(m_pCenterAxis, &m_pSectorVec);
	return 1;
}




////////////////////////////////////////////////////////////////////////////////////////////
// renderware streaming control member

#ifdef _RWKERNEL_
int RwStreamingInsertCamera(CPG3Sector* sector, VfCamera* camera)
{
	if (camera != NULL)
	{
		//RwCamera* cam = (RwCamera*)( camera );
		if ((camera->m_pCamera != NULL) && (sector->m_pWorld != NULL))
		{
			RwCamera* Camera = RwCameraClone(camera->m_pCamera);
			RpWorldAddCamera(sector->m_pWorld, Camera);
			sector->m_pCameraVec.push_back(Camera);
			//RpWorldAddCamera( m_pSectorVec[i]->m_pWorld, camera->m_pCamera );
			return 1;
		}
	}
	return 0;
}


int RwStreamingRemoveCamera(CPG3Sector* sector, Puint index)
{
	RwCamera* camera = sector->m_pCameraVec[index];
	sector->m_pCameraVec.erase(sector->m_pCameraVec.begin() + index);
	RpWorldRemoveCamera(sector->m_pWorld, camera);
	RwCameraDestroy(camera);
	return 1;
}


int RwStreamingInsertLight(CPG3Sector* sector, VfLight* light)
{
	if (light != NULL)
	{
		if ((light->m_pLight != NULL) && (sector->m_pWorld != NULL))
		{
			RpLight* Light = RpLightCreate(light->getType());
			RpLightSetColor(Light, &(RwRGBAReal) light->getColor());
			if (light->m_pMatrix != NULL)
				RpLightSetFrame(Light, light->m_pMatrix);
			RpWorldAddLight(sector->m_pWorld, Light);
			sector->m_pLightVec.push_back(Light);
			return 1;
		}
	}
	return 0;
}


int RwStreamingRemoveLight(CPG3Sector* sector, Puint index)
{
	RpLight* Light = sector->m_pLightVec[index];
	sector->m_pLightVec.erase(sector->m_pLightVec.begin() + index);
	RpWorldRemoveLight(sector->m_pWorld, Light);
	RpLightDestroy(Light);
	return 1;
}



////////////////////////////////////////////////////////////////////////////////////////////
// get member data
//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfBBox CP3DWorld::getBBox( void )
{
	if (m_pSectorVec.empty())
		return VfBBox();
	VfBBox vfbox = m_pSectorVec[0]->getBBox();
	for (Puint si = 1; si < m_pSectorVec.size(); si++)
	{
		VfBBox box = m_pSectorVec[si]->getBBox();
		if (vfbox.vertex[0].x < box.vertex[0].x)
			vfbox.vertex[0].x = box.vertex[0].x;
		if (vfbox.vertex[0].y < box.vertex[0].y)
			vfbox.vertex[0].y = box.vertex[0].y;
		if (vfbox.vertex[0].z < box.vertex[0].z)
			vfbox.vertex[0].z = box.vertex[0].z;
		if (vfbox.vertex[1].x > box.vertex[1].x)
			vfbox.vertex[1].x = box.vertex[1].x;
		if (vfbox.vertex[1].y > box.vertex[1].y)
			vfbox.vertex[1].y = box.vertex[1].y;
		if (vfbox.vertex[1].z > box.vertex[1].z)
			vfbox.vertex[1].z = box.vertex[1].z;
	}
	return vfbox;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfLight* CP3DWorld::getLight( int id, SearchRuleCallBack scb )
{ 
	if (scb == NULL)
		return m_pLightVec[id];

	for (Puint i = 0; i < getLightCount(); i++)
	{
		if (scb(id, m_pLightVec[i]))
			return m_pLightVec[i];
	}
	return NULL;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfCamera* CP3DWorld::getCamera( int id, SearchRuleCallBack scb )
{
	if (scb == NULL)
		return m_pCameraVec[id];

	for (Puint i = 0; i < getCameraCount(); i++)
	{
		if (scb(id, m_pCameraVec[i]))
			return m_pCameraVec[i];
	}
	return NULL;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPG3Sector* CP3DWorld::getSector( int id, SearchRuleCallBack scb )
{
	if (scb == NULL)
		return m_pSectorVec[id];

	for (Puint i = 0; i < m_pSectorVec.size(); i++)
	{
		if (scb(id, m_pSectorVec[i]))
			return m_pSectorVec[i];
	}
	return NULL;
}

#endif
