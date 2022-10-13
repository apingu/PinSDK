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


namespace P3DWorld
{
    Pbool getRoleCallback(int id, void* pData)
	{
	    CPG3Role* r = (CPG3Role*) pData;
	    return (id == r->ID) ? TRUE : FALSE;
	};
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPVfWorld::CPVfWorld()
{
	CPModel* sky = (CPModel*)RemoveSky( m_Sky );
	SAFE_DELETE( sky );


}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPVfWorld::~CPVfWorld()
{



}


////////////////////////////////////////////////////////////////////////////////////////////
// Sky member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CPVfWorld::LoadSky(Pcstr szPathname)
{
	CPModel* sky = new CPModel;
	int ret = sky->Load(szPathname);
	m_Sky = InsertSky( sky );
	return ret;
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
CPG3Role* CP3DWorld::SetFocusRole(CPG3Role* frole)
{
	return _G3ROLE(SetFocusEntity(frole));
}


//==========================================================================================
//  Name : getRole()
//  Desc : get a role form this world
//==========================================================================================
CPG3Role* CP3DWorld::GetRole(int id)
{
	return _G3ROLE(Entity(id, P3DWorld::GetRoleCallback));
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPG3Role* CP3DWorld::GetFocusRole(void)
{
	return _G3ROLE(GetFocusEntity());
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

