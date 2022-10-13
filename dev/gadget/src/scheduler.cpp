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
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
#include <pf-stdlib.h>
#include <pf-thread.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "scheduler.h"


namespace cyberspace
{
    namespace scheduler
    {
        namespace
        {
            T_Lock                         g_operctrl_lock_;
            std::map<OnTask, pftml::Timer> g_operctrl_cntr_; //Container
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Execute( void* data )
        {
            stimen_t starttime = pftml::Time::GetSTime();
            bool blive = true;
            while( blive )
            {
                pfstd::MSSleep( 100 );

                std::vector<OnTask> proc_opers_vec;
                std::vector<float>       proc_frame_vec;

                {
                    TS_Lock lock( &g_operctrl_lock_ );
                    if( g_operctrl_cntr_.empty() )
                    {
                        blive = false;
                        continue;
                    }

                    for( std::map<OnTask, pftml::Timer>::iterator it = g_operctrl_cntr_.begin(); it != g_operctrl_cntr_.end(); it++ )
                    {
                        // 因為是用Timer所以他會追回時間
                        // 比方說 中間有暫停過久
                        float frame = it->second.Interval();
                        if( frame >= TIMER_INTERRUPT )
                        {
                            proc_opers_vec.push_back( it->first );
                            proc_frame_vec.push_back( frame );
                        }
                    }
                }

                for( uint_t pi = 0; pi < proc_opers_vec.size(); pi++ )
                {
                    proc_opers_vec[ pi ]( proc_frame_vec[ pi ] );
                }
            }
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        bool BeginUpdate( mstimen_t framecycle, OnTask op_func )
        {
            pftml::Timer timer;
            timer.SetCycle( ( mstimen_t )( framecycle ) );
            timer.Reset();

            bool begin_exec = false;
            {
                TS_Lock lock( &g_operctrl_lock_ );
                if( g_operctrl_cntr_.empty() )
                {
                    begin_exec = true;
                }
                g_operctrl_cntr_.insert( std::map<OnTask, pftml::Timer>::value_type( op_func, timer ) );
            }

            if( begin_exec )
            {
                pfkrl::Thread::BeginProcess( Execute, NULL, ( "CsScheduler::Execute" ) );
            }

            return true;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void EndTask( OnTask op_func )
        {
            TS_Lock lock( &g_operctrl_lock_ );
            g_operctrl_cntr_.erase( op_func );
        }
    }    

   
//     void Scheduler::DoUpdate(void *argv)
//     {
//         GameServer* server=(GameServer*)argv;
//
//         while(server->GetAlive())
//         {
//             TS_Lock lock(&Global::g_lUpdateOperationList);
//             for(uint_t i=0; i<Global::g_UpdateOperationList.size(); i++)
//             {
//                 float frame=Global::g_UpdateOperationList[i]->update_timer_.Interval();
//                 if(frame >= TIMER_INTERRUPT)
//                 {
//                     Global::g_UpdateOperationList[i]->OnUpdate(frame);
//                 }
//
//                 if(Global::g_UpdateOperationList[i]->OnEndUpdate())
//                 {
//                     OnCloseUpdate(Global::g_UpdateOperationList[i]);
//                     Global::g_UpdateOperationList.erase(Global::g_UpdateOperationList.begin()+i);
//                     i--;
//
//                     continue;
//                 }
//                 pfstd::MSSleep(10);
//             }
//             pfstd::MSSleep(10);
//         }
//     }
};




