// Body.h header file of body.cpp
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
#ifndef SPRITE3D_H
#define SPRITE3D_H

#include <DeList.tl>

#include <Graphic3D.h>
#include <Gene.h>
#include <tString.h>


#define _IMAGE_PLAY_ON_INTERVAL_TIMES  0
#define _IMAGE_STOP 				  -1
#define _IMAGE_PLAY_ONE_TIMES   	  -2
#define _IMAGE_PLAY_N_TIMES 		  -3

#define _GET_FOCUS  1
#define _LOST_FOCUS 0


//-----------------------------------------------------------------------------
// Name: class CP3DSprite
// Desc: to store base value use for game element
//------------------------------------------------------------------------------

//extern LG::Vertex3f _Map_norm;  	//所在地圖的基準點

class CP3DSprite : public CPGene
{
protected:

	Pdword*    m_ReferCount;   		//被複製的數量
	
	MEM::String m_fname;
	short       m_NowDeedStatus;		//****目前動作狀態(0 - 等待, 1 - 準備, 2 - 開始動作, 3 - 動作進行中, 4 - 完成動作);

	float      m_Race;              //****move distance
	short      m_NowAnim;           //設定目前的動作(走, 攻擊, ..........)

	LG::Vertex3f m_Situs;             //地圖位置
	pt::Delist<LG::Vertex3f> m_Route; 				 //****行走路徑  push to protected screen pos


	int      m_Collision;           //碰撞產生Flag
	Pbool    m_Focus;  				//焦點Flag

	//CPCube   m_bbox;
	//CPSphere m_bsphere;


public :

	Pbool Enable;
	Pbool Visible;
	Pbool Dragable;
	Pbool Moveable;

	CP3DSprite();
	CP3DSprite(LG::Vertex3f spos, short anim, float race);
	~CP3DSprite();


    LG::Vertex3f       Situs    (void)           { return m_Situs; }
	LG::Vertex3f       Goal     (void)           { return m_Route.back();}//return where will move to	
	virtual void       Release  (void);
	        Pbool      isFocus  (void)           { return m_Focus; }
	        Pcstr      filename (void)           { return m_fname; }
	        int        GetNowDeedStatus(void)    { return m_NowDeedStatus; }

};


#endif
