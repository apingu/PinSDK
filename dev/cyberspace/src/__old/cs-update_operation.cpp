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
//
//////////////////////////////////////////////////////////////////////////

//01.== C 系統文件/C++ 系統文件
#include <vector>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-update_operation.h"


namespace cyberspace
{
    Scheduler::Scheduler( void* value )
    {
        value_ = value;
    }
    
    Scheduler::~Scheduler()
    {
        value_ = NULL;
// 		TS_Lock lock(&Global::g_lUpdateOperationList);
// 		for(uint_t i=0; i<Global::g_UpdateOperationList.size(); i++)
// 		{
// 			if(Global::g_UpdateOperationList[i]==this)
// 			{
// 				Global::g_UpdateOperationList.erase(Global::g_UpdateOperationList.begin()+i);
// 				break;
// 			}
// 		}
    };
    
    void Scheduler::BeginUpdate( mstimen_t framecycle )
    {
        timer_.SetCycle( ( mstimen_t )( framecycle ) );
        timer_.Reset();
    }
    
    float Scheduler::TestUpdate( void )
    {
        return timer_.Interval();
    }
    
// 	void Scheduler::DoUpdate(void *argv)
// 	{
// 		GameServer* server=(GameServer*)argv;
//
// 		while(server->GetAlive())
// 		{
// 			TS_Lock lock(&Global::g_lUpdateOperationList);
// 			for(uint_t i=0; i<Global::g_UpdateOperationList.size(); i++)
// 			{
// 				float frame=Global::g_UpdateOperationList[i]->update_timer_.Interval();
// 				if(frame >= TIMER_INTERRUPT)
// 				{
// 					Global::g_UpdateOperationList[i]->OnUpdate(frame);
// 				}
//
// 				if(Global::g_UpdateOperationList[i]->OnEndUpdate())
// 				{
// 					OnCloseUpdate(Global::g_UpdateOperationList[i]);
// 					Global::g_UpdateOperationList.erase(Global::g_UpdateOperationList.begin()+i);
// 					i--;
//
// 					continue;
// 				}
// 				pfstd::MsSleep(10);
// 			}
// 			pfstd::MsSleep(10);
// 		}
// 	}
};




