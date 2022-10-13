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
#include <pmath.h>
#include <SectorRecord.h>



#define _G3ROLE( e )	 ( (CPG3Role*)e )

namespace P3DWorld
{
	/*
	struct SectorRecord
	{
		void*       pData;
		void*       pUserData;
		CPString    Name;
		Puint       GridX;
		Puint       GridY;
	};
	*/
	#define DELETE_SECTOR_T( s )    pData=NULL; pUserData=NULL; delete s


	struct role_t
	{
		//CPG3Sector* pSector;
		void*           pData;
		void*           pUserData;
		SectorRecord*   pSector;
		GENTITY         Iter;
	};
	#define DELETE_ROLE_T( r )    r->pData=NULL; r->pUserData=NULL; r->pSector=NULL; delete r 

	
	struct camera_t
	{
		void*       pData;
		void*       pUserData;
		SectorRecord*   pSector;
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

	PBOARD SectorTable=NULL;


}


#define SECTORTABLEID  0
const char SectorName[255] = {"SectorBoard" };

//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CP3DWorld::CP3DWorld()
{
	m_pSectorVec.clear();
	m_pCameraVec.clear();
	m_pLightVec.clear();
	m_pMainCamera = NULL;
	m_pCenterAxis = NULL;
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

		
	//for (Puint s = 0; s < m_Sectorfname.size(); s++)
	//	delete [] m_Sectorfname[s];
	//m_Sectorfname.clear();
}




//=============================================================================
//  Name :
//  Desc :
//=============================================================================
VfError CP3DWorld::Create( CPBBox bBox, Puint cx, Puint cy, Puint cz )
{
	m_fname.clear();
	DeleteAllSector();

	m_Bound = bBox;

	m_GridSize.cx = CubeXLen( m_Bound )/cx;
	m_GridSize.cy = CubeZLen( m_Bound )/cy;
	
	P3DWorld::SectorTable = InsertBoard( SectorName, cx, cy );

	/*

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
	*/


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




////////////////////////////////////////////////////////////////////////////////////////////
//sky member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
BOOL  CP3DWorld::InsertSky( void* sky )
{
	if( m_pSky != NULL )
		return FALSE;
	m_pSky = sky;
	return TRUE;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::RemoveSky( void )
{
	void* data = m_pSky;
	m_pSky=NULL;
	return data;   
}



//==========================================================================================
//  Name : RoleInsert()
//  Desc : Plant a role into this world
//==========================================================================================
GROLE CP3DWorld::InsertRole( void* role, CPVertex3f pos )
{
	//set map pointer
	//CPVertex G_Site = Map_to_Chessboard_Site( role->Map_Situs() );
	//m_MapObject.Insert( role, G_Site.x, G_Site.y );
	if (role == NULL)
		return 0;

	P3DWorld::role_t* r = new P3DWorld::role_t;
	r->pData = role;
	r->pUserData=NULL;
	r->pSector = (P3DWorld::SectorRecord*)OnFindSector( pos );
	r->Iter = InsertEntity( r );

	OnInsertRole( this, r->pData, r->pUserData );
	
	return r;
}


//==========================================================================================
//  Name : RoleRemove()
//  Desc : Eradicate a role form this world
//==========================================================================================
void* CP3DWorld::RemoveRole( GROLE role )
{
	P3DWorld::role_t* r = (P3DWorld::role_t*)role;

	if( RemoveEntity( r->Iter ) == NULL )
		return NULL;
		
	OnRemoveRole( this, r->pData, r->pUserData );

	if( r->pUserData!= NULL )
		DebugMessage( "user define data not clear!!!", "Remove role" );
	
	void* data = r->pData;
	memset( r, 0, sizeof( P3DWorld::role_t ) );
	SAFE_DELETE( r );
	return data;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
GCAMERA CP3DWorld::InsertCamera( void* camera, CPVertex3f pos )
{
	P3DWorld::camera_t* c = new P3DWorld::camera_t;
	c->pData = camera;
	c->pUserData=NULL;
	c->pSector = (P3DWorld::SectorRecord*)OnFindSector( pos );
	m_pCameraVec.push_back(c);
	c->Iter = m_pCameraVec.end();

	OnInsertCamera( this, c->pData, c->pUserData );

	return c;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::RemoveCamera( GCAMERA camera )
{
	P3DWorld::camera_t* c = (P3DWorld::camera_t*)camera;
	m_pCameraVec.erase( c->Iter );

	OnRemoveCamera( this, c->pData, c->pUserData );

	if( c->pUserData!= NULL )
		DebugMessage( "user define data not clear!!!", "Remove camera" );

	void* data = c->pData;
	memset( c, 0, sizeof( P3DWorld::camera_t ) );
	SAFE_DELETE( c );
	return data;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
GLIGHT CP3DWorld::InsertLight( void* light )
{
	P3DWorld::light_t* l = new P3DWorld::light_t;
	l->pData = light;
	l->pUserData=NULL;
	m_pLightVec.push_back(l);
	l->Iter = m_pLightVec.end();

	OnInsertLight( this, l->pData, l->pUserData );

	return l;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::RemoveLight( GLIGHT light )
{
	P3DWorld::light_t* l = (P3DWorld::light_t*)light;
	m_pLightVec.erase( l->Iter );

	OnRemoveLight( this, l->pData, l->pUserData );

	if( l->pUserData!= NULL )
		DebugMessage( "user define data not clear!!!", "Remove light" );

	void* data = l->pData;
	memset( l, 0, sizeof( P3DWorld::light_t ) );
	SAFE_DELETE( l );
	return data;
}


//==========================================================================================
//  Name : SectorInsert()
//  Desc :
//==========================================================================================
GSECTOR CP3DWorld::InsertSector( Pcstr filename, Puint cx, Puint cy )
{
	if( Board_Count() == 0 )
		return NULL;

	P3DWorld::SectorRecord* s = new P3DWorld::SectorRecord;
	s->pData=NULL;
	s->pUserData=NULL;
	s->Name = filename;
	s->GridX = cx;
	s->GridY = cy;

	SetData( SECTORTABLEID, cx, cy, s );
	
	OnInsertSector( this, s->pData, s->pUserData );

	return s;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::RemoveSector( GSECTOR sector )
{
	P3DWorld::SectorRecord* s = (P3DWorld::SectorRecord*)sector;
	SetData( SECTORTABLEID, s->GridX, s->GridY, NULL );

	OnRemoveSector( this, s->pData, s->pUserData );

	void* data = s->pData;
	memset( s, 0, sizeof( P3DWorld::SectorRecord ) );
	SAFE_DELETE( s );
	return data;
}




////////////////////////////////////////////////////////////////////////////////////////////
// protected member

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::DeleteAllSector(void)
{
	for (Puint s = 0; s < m_pSectorVec.size(); s=0)
		RemoveSector( (P3DWorld::SectorRecord*)m_pSectorVec[s] );
	//m_pSectorVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllCamera(void)
{
	for (Puint c = 0; c < m_pCameraVec.size(); c=0)
		RemoveCamera( (P3DWorld::camera_t*)m_pCameraVec[c] );
	//m_pCameraVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllLight(void)
{
	for (Puint l = 0; l < m_pLightVec.size(); l=0)
		RemoveLight( (P3DWorld::light_t*)m_pLightVec[l] );

	//m_pLightVec.clear();
	return 1;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pint CP3DWorld::RemoveAllRole(void)
{
	for (Puint r = 0; r < EntityCount(); r=0)
		RemoveRole( (P3DWorld::role_t*)Entity(r) );
	//EntityClear();
	return 1;
}




////////////////////////////////////////////////////////////////////////////////////////////
// streaming member

//==========================================================================================
//  Name :
//  Desc :  set streaming follow position
//==========================================================================================
void CP3DWorld::UpdateStreamCenter( CPAxis* axis )
{
	m_pCenterAxis = axis;
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////
// get member data
//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
CPBBox CP3DWorld::GetBBox( void )
{
	return m_Bound;

	/*
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
	*/
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Puint CP3DWorld::GetCols( void )
{
	return GetBroadCols(SECTORTABLEID);
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Puint CP3DWorld::GetRows( void )
{
	return GetBroadRows(SECTORTABLEID);
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetSky( void )
{
	return m_pSky;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetRole( GROLE role )
{
	P3DWorld::role_t* r = (P3DWorld::role_t*)role;
	return r->pData;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetLight( GLIGHT light )
{
	P3DWorld::light_t* l = (P3DWorld::light_t*)light;
	return l->pData;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetCamera( GCAMERA camera )
{
	P3DWorld::camera_t* c = (P3DWorld::camera_t*)camera;
	return c->pData;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetSector( GSECTOR sector )
{
	P3DWorld::SectorRecord* s = (P3DWorld::SectorRecord*)sector;
	return s->pData;
}

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetFocusRole(void)
{
	return GetFocusEntity();
}

//==========================================================================================
//  Name : setFocusRole()
//  Desc : get a role form this world
//==========================================================================================
void CP3DWorld::SetFocusRole( GROLE frole )
{
	P3DWorld::role_t* r = (P3DWorld::role_t*)frole;
	SetFocusEntity(r->Iter);
	return;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::GetMainCamera(void)
{
	P3DWorld::camera_t* c = (P3DWorld::camera_t*)m_pMainCamera;
	return c->pData;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::SetMainCamera( GCAMERA camera)
{
	m_pMainCamera = camera;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::RoleGetSector( GROLE role )
{
	P3DWorld::role_t* r = (P3DWorld::role_t*)role;
	return r->pSector;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void* CP3DWorld::CameraGetSector( GCAMERA camera )
{
	P3DWorld::camera_t* c = (P3DWorld::camera_t*)camera;
	return c->pSector;
}



///////////////////////////////////////////////////////////////////////////////////////
// update function

//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
int CP3DWorld::Update(void)
{


}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::UpdateRole( GROLE role, CPAxis* martix )
{



}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::UpdateLight( GLIGHT light, CPAxis* martix )
{



}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::UpdateCamera( GCAMERA camera, CPAxis* martix )
{



}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::UpdateStreamCenter( CPAxis* martix )
{


}





//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
GSECTOR CP3DWorld::FindSector( CPVertex3f pos )
{
	return GetData( SECTORTABLEID, (pos.x / m_GridSize.cx), (pos.y / pos.x / m_GridSize.cy) );
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
void CP3DWorld::GetNeighborhood(const CPAxis* centerpos,
	std::vector<void*>& neighborhood)
{
	return;
}

