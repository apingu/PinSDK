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
//                                               Copyright    (C)    2000    Pin
//
#include <Vfw3D.h> //3DEngine



//==========================================================================================
//  Name : RoleRemove()
//  Desc : Eradicate a role form this world
//==========================================================================================
#ifdef _RWKERNEL_
RpClump* RpWroldReleaseClumpCallBack(RpClump* clump, void* data)
{
	RpWorld* world = (RpWorld*) data;
	RpWorldRemoveClump(world, clump);
	return clump;
}
#endif

void        OnPutRole( CPG3World* world, void* role, void* pUserData )
void        OnPutLight( CPG3World* world, void* light, void* pUserData )
void        OnPutCamera( CPG3World* world, void* camera, void* pUserData )
void        OnPutSector( CPG3World* world, void* sector, void* pUserData )

void        OnPopRole( CPG3World* world, void* role, void* pUserData )
void        OnPopLight( CPG3World* world, void* light, void* pUserData )
void        OnPopCamera( CPG3World* world, void* camera, void* pUserData )
void        OnPopSector( CPG3World* world, void* sector, void* pUserData )
