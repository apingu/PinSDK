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

//#include <GTrigger.h>
//#include <G3DRole.h>
//#include "G3DSector.h"


typedef void*     GSKY;
typedef void*     GROLE;
typedef void*     GLIGHT;
typedef void*     GCAMERA;
typedef void*     GSECTOR;


class CP3DWorld : public CPGSpace, public CPChessboard
{
	CPString m_fname;
	CPAxis*  m_pCenterAxis;

	//std::vector<VfLight*> m_pLightVec;

	GSKY*    m_pSky;
	//int      m_iMainCamera;
	GCAMERA   m_pMainCamera;
	DataList  m_pCameraVec;
	DataList  m_pLightVec;
	DataList  m_pSectorVec;
	
	//VfCamera* m_pMainCamera;
	//std::vector<VfCamera*> m_pCameraVec;

	//std::vector<Pstr> m_Sectorfname;
	//std::vector<CPG3Sector*> m_pSectorVec;

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


	virtual VfError Create( CPBBox bBox, Puint cx = 1, Puint cy = 1, Puint cz = 1 );
	virtual VfError Create( Pcstr szPathname );
	        void    Destroy(void);
	virtual Pbool   empty(void){ return m_pSectorVec.empty();}


	//sky member
	int       LoadSky( Pcstr szPathname );

	//role member
	GROLE     InsertRole( void* role, VfVertex3f pos );
	void*     RemoveRole( GROLE descriptor );

	//light member
	GLIGHT    InsertLight( void* light );
	void*     RemoveLight( GLIGHT light );

	//camera member
	GCAMERA   InsertCamera( void* camera );
	void*     RemoveCamera( GCAMERA camera );

	//sector member
	GSECTOR   InsertSector( Pcstr filename, Puint cx, Puint cy );
	void*     RemoveSector( GSECTOR sector );


	//render member
	virtual void Render(void);
	        void RenderRole(void);
	        void RenderMesh(CPRGBA color);
	        void RenderSky(CPVertex3f* pos);


	//update member
	int     Update(void);
	
	//streaming control
	//void setCenterAxis( CPAxis* pos );
	void    UpdateRole   ( GROLE   role,    CPMatrixf* martix );
	void    UpdateLight  ( GLIGHT  light,   CPMatrixf* martix );
	void    UpdateCamera ( GCAMERA camera,  CPMatrixf* martix );
	int     UpdateStreamCenter( CPMatrixf* martix );


	//get member
	CPBBox        getBBox(void);
	Puint         getLightCount( void )  { return m_pLightVec.size(); }
	Puint         getSectorCount( void ) { return m_pSectorVec.size(); }
	Puint         getCameraCount( void ) { return m_pCameraVec.size(); }
	
	void*         getRole( GROLE role );
	void*         getLight( GLIGHT light );
	void*         getCamera( GCAMERA camera );
	void*         getSector( GSECTOR sector );
	void*         getSky( void );
	
	void*         RoleGetSector( GROLE role );
	void*         CameraGetSector( GCAMERA camera );
	
	
	void*         getFocusRole( void );
	void*         setFocusRole( GROLE role );

	void          setMainCamera(GCAMERA camera);
	void*         getMainCamera(void);


protected:

	virtual void        OnStreamingUpdate( CP3DWorld* world, CPAxis* Center );// streaming loading rule

	virtual void        OnInsertRole( CP3DWorld* world, void* role, void* pUserData );
	virtual void        OnInsertLight( CP3DWorld* world, void* light, void* pUserData );
	virtual void        OnInsertCamera( CP3DWorld* world, void* camera, void* pUserData );
	virtual void        OnInsertSector( CP3DWorld* world, void* sector, void* pUserData );

	virtual void        OnRemoveRole( CP3DWorld* world, void* role, void* pUserData );
	virtual void        OnRemoveLight( CP3DWorld* world, void* light, void* pUserData );
	virtual void        OnRemoveCamera( CP3DWorld* world, void* camera, void* pUserData );
	virtual void        OnRemoveSector( CP3DWorld* world, void* sector, void* pUserData );
	
	virtual void        OnRenderSky( void* sky, void* pUserData );
	virtual void        OnRenderRole( void* role, void* pUserData );
	virtual void        OnRenderSector( void* sector, void* pUserData );
	
	virtual void*       OnLoadSky( Pcstr filename );
	virtual void*       OnLoadSector( Pcstr filename );
	virtual void        OnReleaseSector( Pcstr filename );

};


#endif
