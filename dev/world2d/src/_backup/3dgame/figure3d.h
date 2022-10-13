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
//  			$(RWGSDK)\include\opengl,$(PINSDK)\Base, $(PINSDK)\3DEngine,$(PINSDK)\3DEngine\RW, $(PINSDK)\Game
//
//
//                                               Copyright    (C)    2000    Pin
//
#ifndef FIGURE3D_H
#define FIGURE3D_H


#include "Sprite3d.h"    //3DGame
#include <vector>
#include <Model3d.h>  	 //3DEngine
#include <Anim.h>   	 //3DEngine



///////////////////////////////////////////////////////////////////////////////
// figure model list
class CP3DFigure : public CP3DSprite
{
	std::vector<CPModel*> m_pModelVec;
	CPModel* m_pModelNow;

	void AddModel(CPModel* model);

public:

	CP3DFigure();
	~CP3DFigure();

	CPModel* Model(Pvoid);
	Pint LoadModel(Pcstr szFilename);
};


///////////////////////////////////////////////////////////////////////////////
// atcion act list
class CP3DAction : public CP3DFigure
{
	CPAnim* m_pHAnimNow;
	std::vector<CPAnim*> m_pHAnimSeqs;
	//Puint 				 m_nHAnimCurSeqs;
	//Pint  				 m_nHAnimBlend;

public:

	CP3DAction();
	~CP3DAction();

	//Pvoid 	  Destory  ( Pvoid );
	operator	CPAnim*();

	        Pint  LoadAnim(Pcstr szFilename);
	virtual Pint  Do(Puint curAnim);
	virtual Pvoid UpdateAnim(Pfloat delta);
};



#endif
