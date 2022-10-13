// Body.cpp game element data store class
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
//                                               Copyright    (C)    2000    Pin
//
#include "Sprite3d.h"
//#include <PFile.h>
#include <Debug.h>
//#include <crtdbg.h>



//////////////////////////////////////////////////////////////////////////////////////////////
//  CP3DSprite class


//=============================================================================
//  Name : 
//  Desc : constructor
//=============================================================================
CP3DSprite::CP3DSprite()
{
	memcpy(&CID, "SPRI", sizeof(Pdword));
	m_ReferCount = NULL;
	m_NowDeedStatus = 0;
	m_NowAnim = 0;
	m_Race = 0;
	m_Collision = 0;
	m_Focus = false;

	ID = -1;
	GID = -1;
	Enable = 1;
	Visible = 1;
	Dragable = 1;
	Moveable = 1;
}


//=============================================================================
//  Name : 
//  Desc : constructor
//=============================================================================
CP3DSprite::CP3DSprite(LG::Vertex3f spos, short anim, Pfloat race)
{
	m_Situs = spos;
	m_NowAnim = anim;  			     //設定目前的動作(走, 攻擊, ..........)
	m_Race = race;  				 //****move distance
}


//=============================================================================
//  Name : 
//  Desc : distructor
//=============================================================================
CP3DSprite::~CP3DSprite()
{
	Release();
}




//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void CP3DSprite::Release(void)
{
	m_Route.clear();
	if (m_ReferCount != NULL)
	{
		if ((*m_ReferCount) == 0)
			SAFE_DELETE(m_ReferCount);
	}
}








