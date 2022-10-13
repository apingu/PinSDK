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
#ifndef GWORLD_H
#define GWORLD_H


#include "VfSector.h"
#include <Vfw3D.h>
#include <G3DRole.h>
#include <GTrigger.h>




class CGWorld : public VfSector
{
	CPModel    m_Sky;
	CP3DRole*  m_MainRole;
	
	MEM::String   m_WorkFolder;

	std::vector< VfLight* >  m_pLightVec;
	std::vector< CP3DRole* > m_pEntityVec;

	VfCamera*  m_MainCamera;
	std::vector< VfCamera* > m_pCameraVec;

public:

	CGWorld();
	~CGWorld();

	VfError Create(VfBBox bBox);
	VfError Create(Pcstr fname);
	void    Release( void );
	void    DeleteAllLight( void );

	VfCamera* GetMainCamera( void ){ return m_MainCamera; }
	void      SetMainCamera( VfCamera* c ){ m_MainCamera=c; }

	CP3DRole* GetFocusRole( void ){ return m_MainRole; }
	void      SetFocusRole( CP3DRole* m ){ m_MainRole=m; }
	
	int  LoadSky( Pcstr szPathname );

	int  InsertRole( CP3DRole* role );
	int  InsertLight( VfLight* light );
	int  InsertCamera( VfCamera* light );

	int  RemoveRole( CP3DRole* role );
	int  RemoveLight( VfLight* light );
	int  RemoveCamera( VfCamera* light );

	int  Render( void );
	int  RenderMesh( const LC::RGBAb* color );

	CP3DRole* GetEntity( UINT i )    { return m_pEntityVec[i]; }
	UINT      GetEntityCount( void ) { return m_pEntityVec.size(); }

	VfLight*  GetLight( UINT i )    { return m_pLightVec[i]; }
	UINT      GetLightCount( void ) { return m_pLightVec.size(); }

	void Update( void );

	char* GetWorkFolder( void ){ return m_WorkFolder; }
	void  SetWorkFolder( Pcstr szPathname ){ m_WorkFolder = szPathname; }
	void  Load( Pcstr szPathname );
	void  Save( Pcstr szPathname );

	bool RayHitTest( const LG::Line3df &ray, LG::Vertex3f& cosPos );

public:

	CPGTriggerSys  m_TriggerSys;


};


#endif

