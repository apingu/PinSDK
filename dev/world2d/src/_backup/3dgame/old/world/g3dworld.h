//
//
//
//    streaming world
//
//
//
//
//
//
//
//  		$(PINSDK)\Base, $(PINSDK)\include, $(PINSDK)\Game
//
//                                               Copyright    (C)    2000    Pin
//
#ifndef G3DWORLD_H
#define G3DWORLD_H

#include <pdef.h>  //Base
#include <datadef.h>

#include <Axis.h>
#include <GSpace.h>
#include <Chessboard.h>


typedef void*     GSKY;
typedef void*     GROLE;
typedef void*     GLIGHT;
typedef void*     GCAMERA;
typedef void*     GSECTOR;


class CP3DWorld : public CPGSpace, public CPChessboard
{
	LG::BBoxf   m_Bound;
	CPSize      m_GridSize;

	CPString m_fname;
	CPAxis*  m_pCenterAxis;


	GSKY      m_pSky;
	GCAMERA   m_pMainCamera;
	DataList  m_pCameraVec;
	DataList  m_pLightVec;
	DataList  m_pSectorVec;

	std::vector<void*> m_pRenderSector;
	
	//void SectorInsertAllObject( GSECTOR sector );

protected:

	int RemoveAllRole(void);
	int RemoveAllLight(void);
	int RemoveAllCamera(void);
	int DeleteAllSector(void);
						
	//virtual CPG3Sector* findSector( VfVertex3f pos );
	//virtual void        RenderCallBack ( std::vector<void*> Renderdata ){};

public:

	//#ifdef _RWKERNEL_
	//	RpWorld*  m_pWorld;
	//#endif

	CP3DWorld();
	~CP3DWorld();


	virtual VfError Create( LG::BBoxf bBox, Puint cx = 1, Puint cy = 1, Puint cz = 1 );
	virtual VfError Create( Pcstr szPathname );
	        void    Destroy(void);
	virtual Pbool   empty(void){ return m_pSectorVec.empty();}


	//sky member
	BOOL      InsertSky( void* sky );
	void*     RemoveSky( void );

	
	//role member
	GROLE     InsertRole( void* role, LG::Vertex3f pos );
	void*     RemoveRole( GROLE role );

	//light member
	GLIGHT    InsertLight( void* light );
	void*     RemoveLight( GLIGHT light );

	//camera member
	GCAMERA   InsertCamera( void* camera, LG::Vertex3f pos );
	void*     RemoveCamera( GCAMERA camera );

	//sector member
	GSECTOR   InsertSector( Pcstr filename, Puint cx, Puint cy );
	void*     RemoveSector( GSECTOR sector );



	//get member
	LG::BBoxf     GetBBox(void);
	Puint         GetCols( void );
	Puint         GetRows( void );
	Puint         GetLightCount( void )  { return m_pLightVec.size(); }
	Puint         GetSectorCount( void ) { return m_pSectorVec.size(); }
	Puint         GetCameraCount( void ) { return m_pCameraVec.size(); }

	void*         GetSky( void );
	void*         GetRole( GROLE role );
	void*         GetLight( GLIGHT light );
	void*         GetCamera( GCAMERA camera );
	void*         GetSector( GSECTOR sector );

	void*         GetFocusRole( void );
	void          SetFocusRole( GROLE role );

	void*         GetMainCamera(void);
	void          SetMainCamera(GCAMERA camera);
	
	void*         RoleGetSector( GROLE role );
	void*         CameraGetSector( GCAMERA camera );


	//update member
	int     Update(void);

	//streaming control
	void    UpdateRole   ( GROLE   role,    CPAxis* martix );
	void    UpdateLight  ( GLIGHT  light,   CPAxis* martix );
	void    UpdateCamera ( GCAMERA camera,  CPAxis* martix );
	void    UpdateStreamCenter( CPAxis* martix );

		

	//render member
	virtual void Render(void){};
	

protected:


	virtual void        OnInsertRole( CP3DWorld* world, void* role, void* pUserData );
	virtual void        OnInsertLight( CP3DWorld* world, void* light, void* pUserData );
	virtual void        OnInsertCamera( CP3DWorld* world, void* camera, void* pUserData );
	virtual void        OnInsertSector( CP3DWorld* world, void* sector, void* pUserData );

	virtual void        OnRemoveRole( CP3DWorld* world, void* role, void* pUserData );
	virtual void        OnRemoveLight( CP3DWorld* world, void* light, void* pUserData );
	virtual void        OnRemoveCamera( CP3DWorld* world, void* camera, void* pUserData );
	virtual void        OnRemoveSector( CP3DWorld* world, void* sector, void* pUserData );


	
	virtual GSECTOR     OnFindSector( LG::Vertex3f pos );
	virtual void        OnGetNeighborhood( const CPAxis* centerpos, 
		                                   std::vector<void*>& neighborhood );

	virtual void        OnStreamingUpdate( CP3DWorld* world, CPAxis* Center );// streaming loading rule

	virtual void        OnRenderSky( void* sky, void* pUserData );
	virtual void        OnRenderRole( void* role, void* pUserData );
	virtual void        OnRenderSector( void* sector, void* pUserData );
	
	virtual void*       OnLoadSky( Pcstr filename );
	virtual void*       OnLoadSector( Pcstr filename );
	virtual void        OnReleaseSector( Pcstr filename );

};


#endif
