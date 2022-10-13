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

#ifndef WORLD3D_H
#define WORLD3D_H

#include <basedef.h>
#include <Engine3dDef.h>


namespace Rune3d
{
	class World : public BaseSceneManager
	{

	public:

		World();
		~World();

		int Load( tchar_t* filename );

	};
};


#endif


