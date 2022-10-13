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
//
//                                               Copyright    (C)    2001    Pin
//
#ifndef G3DROLE_H
#define G3DROLE_H

//#include "..\World\World.h"
#include <G3DElement.h>

class CP3DWorld;

class CP3DRole : public CP3DElement
{
	CP3DWorld* m_ResideWorld;

public :

	CP3DRole();
	~CP3DRole();

	virtual Pint Do(Puint curAnim);
	//virtual Pvoid 	  UpdateAnim( Pfloat delta );

	void         SetWorld(CP3DWorld* world)	{ m_ResideWorld = world; }
	CP3DWorld*   GetWorld(void)             { return m_ResideWorld;  }
};


#endif //ROLE_H
