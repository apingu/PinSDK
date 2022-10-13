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

//01. == C 系統文件/C++ 系統文件
#if defined(_WINDOWS)  /* Windows platforms */
//#include <windows.h>
#include <process.h>
#endif

#if defined(_LINUX)
//#ifdef _GUNC
#include <unistd.h>
#include <pthread.h>
#endif

#include <set>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-def.h"
#include "t_lock.h"
#include "pf-stdlib.h"
#include "pf-debug.h"
#include "pf-thread.h"


namespace pfkrl
{
    namespace Thread
    {
    
#if defined(_WINDOWS)  /* Windows platforms */
    #define NULL_THREAD_ID   -1
#endif
#if defined(_LINUX)
    //#ifdef _GUNC
    #define NULL_THREAD_ID    0
#endif
        namespace _GlobalArea
        {
            struct ProcessInfo
            {
                char           name[_MAX_BUF];
                Thread::Handle pid;
                int            idx;
                void*          argv;
                func_address   func;
                //PfThread*      uthread;
            };
            
            //int      g_PFThreadCnt = 0;
            T_Lock   s_utThreadLock;
            std::vector<ProcessInfo*>* s_GlobalProcessList;
        };
        
#if defined(_WINDOWS)  /* Windows platforms */
    void  Processfunc( void* data )
#endif
#if defined(_LINUX)
    void* Processfunc( void* data )
#endif
        {
            _GlobalArea::ProcessInfo* pd = ( _GlobalArea::ProcessInfo* )data;
            //int ProcessIndex   = 0;
            //Handle   threadid  = pd->pid;
            //func_address  func = pd->func;
            //void*         argv = pd->argv;

            Pf_INFO( "PfThread: enter .... name: %s, index: %d\n", pd->name, pd->idx );
            //PfThread*    tt = pd->uthread;
            //pd->argv = NULL;
            //pd->func = NULL;
            //pd->uthread = NULL;
            //SAFE_DELETE(pd);
            ( *pd->func )( pd->argv );
            {
                TS_Lock lock( &_GlobalArea::s_utThreadLock );
                
                if ( _GlobalArea::s_GlobalProcessList )
                {
                    //char buff[256];
                    for ( unsigned ii = 0; ii < _GlobalArea::s_GlobalProcessList->size(); ii++ )
                    {
                        //_stprintf(buff, "leave thread %d\n", ii);
                        //PfOutputDebugString(buff);
                        if ( ( *_GlobalArea::s_GlobalProcessList )[ii]->pid == pd->pid )
                        {
                            _GlobalArea::ProcessInfo* pinfo = ( *_GlobalArea::s_GlobalProcessList )[ii];
                            _GlobalArea::s_GlobalProcessList->erase( _GlobalArea::s_GlobalProcessList->begin() + ii );
                            Pf_INFO( "PfThread: bye bye...%s / %d\n", pd->name, _GlobalArea::s_GlobalProcessList->size() );
                            SAFE_DELETE( pinfo );
                            //thrsize = _GlobalArea::s_GlobalProcessList.size();
                            break;
                        }
                        
                        //_stprintf(buff, "leave thread %d ok\n", ii);
                        //PfOutputDebugString(buff);
                    }
                   
                    if ( _GlobalArea::s_GlobalProcessList->empty() )
                    {
                        SAFE_DELETE( _GlobalArea::s_GlobalProcessList );
                        //_stprintf(buff, "leave thread empty\n");
                        //PfOutputDebugString(buff);
                    }
                }
            }
            //_EXIT_THREAD_PROC:
//#ifdef _DEBUG
            ///int thrsize = 0;
            //tchar_t buff[255];
            //_stprintf(buff, "thread bye bye...%d\n", thrsize);
            //OutputDebugStringA(buff);
//#endif
#if defined(_WINDOWS)  /* Windows platforms */
            _endthread();
            return;
#else
            return NULL;
#endif
        }
        
        
        //=============================================================================
        // NAME: BeginProcess
        // DESC:
        //=============================================================================
        Handle BeginProcess( func_address func, void* argv, const char* name )
        {
            //unsigned int thread_id;
            //TS_Lock lock(&g_Lock);
            //bool bRetval = false;
            _GlobalArea::ProcessInfo* pd = new _GlobalArea::ProcessInfo;

            //pd->uthread=this;
            //pd->ProcessVec=m_ProcessThread;
            TS_Lock lock( &_GlobalArea::s_utThreadLock );
            
            if ( _GlobalArea::s_GlobalProcessList == NULL )
            {
                _GlobalArea::s_GlobalProcessList = new std::vector<_GlobalArea::ProcessInfo*>;
            }

            pd->pid = NULL_THREAD_ID;
            pd->argv = argv;
            pd->func = func;
            pd->idx  = _GlobalArea::s_GlobalProcessList->size();
            memset( pd->name, 0, _MAX_BUF );
            if( name != NULL )
                strcpy( pd->name, name );

            
#if defined(_WINDOWS) // Windows platforms
            //EnterCriticalSection(&m_MapMutex);
            Handle ProcessThread = _beginthread( Processfunc, 0, pd );
            
            //unsigned int ProcessThread = _beginthread(func, 0, argv);
            if ( ProcessThread != NULL_THREAD_ID )
            {
                pd->pid = ProcessThread;
                _GlobalArea::s_GlobalProcessList->push_back( pd );
            }
            else
            {
                SAFE_DELETE( pd );
                ProcessThread = 0;
            }
            
            //LeaveCriticalSection(&m_MapMutex);
#endif
#if defined(_LINUX)
            //pthread_mutex_lock(&m_MapMutex);
            //pthread_t tid;
            //pthread_attr_t attr;
            //char* vv;
            //pthread_attr_init(&attr);
            //pthread_t ProcessThread;
            //pthread_create(&tid, &attr, runner, vv);
            
            Handle ProcessThread;
            int nRetval = pthread_create( &ProcessThread, NULL, Processfunc, pd );
            
            if ( nRetval == 0 )
            {
                pd->pid = ProcessThread;
                _GlobalArea::s_GlobalProcessList->push_back( pd );
            }
            else
            {
                delete pd;
                ProcessThread = 0;
            }
            
            //pthread_mutex_unlock(&m_MapMutex);
#endif           // End platform specific
            
            if ( _GlobalArea::s_GlobalProcessList )
            {
                Pf_INFO( "PfThread: begin .... name: %s, index: %d\n", pd->name, pd->idx );
            }
            
            return ProcessThread;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //int GetTotalThreadSize (void)
        int GetProcessSize( void )
        {
            TS_Lock lock( &_GlobalArea::s_utThreadLock );
            
            if ( _GlobalArea::s_GlobalProcessList == NULL )
            {
                return 0;
            }
            
            return _GlobalArea::s_GlobalProcessList->size();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool TestProcessRun( Handle threadid )
        {
            TS_Lock lock( &_GlobalArea::s_utThreadLock );
            
            if ( _GlobalArea::s_GlobalProcessList )
            {
                for ( unsigned ii = 0; ii < _GlobalArea::s_GlobalProcessList->size(); ii++ )
                {
                    if ( ( *_GlobalArea::s_GlobalProcessList )[ ii ]->pid == threadid )
                    {
                        return true;
                    }
                }
            }
            
            return false;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool TestProcessExit( std::vector<Handle> threadids )
        {
            TS_Lock lock( &_GlobalArea::s_utThreadLock );

            if ( _GlobalArea::s_GlobalProcessList )
            {
                for ( unsigned ii = 0; ii < _GlobalArea::s_GlobalProcessList->size(); ii++ )
                {
                    for ( unsigned ti = 0; ti < threadids.size(); ti++ )
                    {
                        if ( ( *_GlobalArea::s_GlobalProcessList )[ ii ]->pid == threadids[ti] )
                        {
                            return false;
                        }
                    }
                }
            }

            return true;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool WaitAllExit( mstimen_t waittime, const char* name )
        {
            mstimen_t intime = pftml::Time::GetTickMSTime();
            while ( 1 )
            {
                {
                    TS_Lock lock( &_GlobalArea::s_utThreadLock );

                    if ( _GlobalArea::s_GlobalProcessList == NULL )
                    {
                        return true;
                    }
                    if ( name != NULL )
                    {
                        bool bfind = false;
                        for ( unsigned ii = 0; ii < _GlobalArea::s_GlobalProcessList->size(); ii++ )
                        {
                            if ( strcmp( (*_GlobalArea::s_GlobalProcessList )[ ii ]->name, name ) == 0 )
                            {
                                bfind = true;
                                break;
                            }
                        }
                        if( !bfind )
                            return true;
                    }
                }
                mstimen_t curtime = pftml::Time::GetTickMSTime();
                if ( waittime != 0 )
                {
                    if ( ( curtime - intime ) > waittime )
                    {
                        break;
                    }
                }
                pfstd::MSSleep( 0 );
            }          
            return false;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        /*
        void PfThread::EndProcess(unsigned int tid)
        {
        std::vector<Handle>::iterator ivt;
        for(ivt=m_ProcessThread->begin();ivt!=m_ProcessThread->end();ivt++)
        {
        if(*ivt==tid){
        m_ProcessThread->erase(ivt);
        break;
        }
        }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void PfThread::EndAllProcess(void)
        {
        for(unsigned int i = 0; i< m_ProcessThread->size(); i++)
        {
        EndProcess(m_ProcessThread[i]);
        }
        }
        */
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //   int GetThreadSize(void)
        //   {
        //    //TS_Lock lock(&_GlobalArea::s_utThreadLock);
        //    //return m_ProcessThread.size();
        //
        //    return 0;
        //   }
    }
};

