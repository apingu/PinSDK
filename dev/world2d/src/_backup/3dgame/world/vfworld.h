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
#ifndef VFWORLD_H
#define VFWORLD_H


#include "G3DWorld.h"
#include <GTrigger.h>
#include <G3DRole.h>
#include "G3DSector.h"


class CPVfWorld : private G3DWorld
{
	GSKY m_Sky;

public:

	CPVfWorld();
	~CPVfWorld();

	int  LoadSky( Pcstr szPathname );



};


#endif
