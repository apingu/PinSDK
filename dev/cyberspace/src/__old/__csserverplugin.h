#ifndef CSSERVERPLUGIN_H
#define CSSERVERPLUGIN_H

//#include "CsDef.h"
//#include "XDatabase.h"
#include <basedef.h>
#include "CsGameServer.h"


// Win32
//#ifdef _WIN32

//! server 初始化
__SLAPI int         OnCsServerInitial( const char* servername );
//! server 結束
__SLAPI int         OnCsServerShutdown( const char* servername );


//! Database
__SLAPI XDataBase*  OnCsCreateAccountDB( void );
__SLAPI void        OnCsDeleteAccountDB( XDataBase* db );

__SLAPI XDataBase*  OnCsCreateDatabase( void );
__SLAPI void        OnCsDeleteDatabase( XDataBase* db );


__SLAPI u_int       OnCsGetMapCx( void ){return 0;};
__SLAPI u_int       OnCsGetMapCy( void ){return 0;};
__SLAPI u_int       OnCsGetMapCz( void ){return 0;};

//找尋座標 x,y,z 是在哪個 ragion 中 (傳回索引值)
__SLAPI RegionId_t  OnCsGetRegionID(void *prop){ return 0; };

//當有一個新物品要加入地圖中,傳回該物品存在的 ragion 索引值
//取出 ragion 列表中,其中一個 ragion 的鄰居列表
//__SLAPI void        OnCsGetNeighbors( RegionId_t rid, std::vector<RegionId_t> &neighbors ){ neighbors.push_back(rid); }
__SLAPI RegionId_t* OnCsGetNeighbors( RegionId_t rid, size_t& len );
__SLAPI void        OnCsDeleteNeighbors( RegionId_t* neighbor, size_t len );


//
__SLAPI bool        OnCsLogin( connectionid_t acc );


//! Logout 完後
__SLAPI void        OnCsLogout( connectionid_t acc );


//! 產生新的 player (???). 傳回1(成功)或錯誤碼(失敗)
__SLAPI void*       OnCsCreateEntity( entityid_t eid,
			                           const char *name,
							           void *prop, int plen,
									   size_t& len );

__SLAPI void        OnCsDeleteResbuf( void* resbuf, size_t len );


//! 消除一個 player 資料
__SLAPI int         OnCsDeleteEntity( entityid_t eid,
			                           const char *name,
									   void *prop, int plen );

//! 當有一個角色剛剛加入遊戲時的 callback
__SLAPI bool        OnCsEntityJoinGame( CsEntityDescriptor entity );


//! 當有一個角色正要離開遊戲時的 callback
__SLAPI bool        OnCsEntityExitGame( CsEntityDescriptor entity );



//
__SLAPI void        OnCsGetEntityAppearance( const void* buff, size_t bufflen,
			                                 void** appear, size_t& len );


//#endif




#endif //CSSERVERPLUGIN_H