//
//
//
//
//回收機制
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
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <sys/timeb.h>
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


namespace pfstl
{
    template <class _Type> class collectpool
    {
        T_Lock                ut_lock_;
        std::map<_Type*, int> recyclebin_;  // 回收區
        std::map<_Type*, int> container_;   // 建立的收集區
        
        typedef typename std::map<_Type*, int>::iterator   _collectpool_iterator;
        typedef typename std::map<_Type*, int>::value_type _collectpool_value_type;
        
        static dword_t TimeClock()
        {
#ifdef _WINDOWS
            return ::GetTickCount();
#endif
#ifdef _LINUX
            struct timeb tb;
            ftime( &tb );
            return ( ( ( tb.time - 1000000000 ) * 1000 ) + ( tb.millitm ) );
#endif
        }
        
    public:
    
        collectpool() {}
        ~collectpool() {}
        
        typedef bool create_condition_func( _Type* t, const void* c );
        
        // 從回收區中 取出一個 物件使用
        // 如果有 建立條件 則依照 建立條件來取出
        // 如果沒有 收閒置多久以上的 物件
        // 若不存在 則建立新的
        _Type* create( int overtm, create_condition_func* func = NULL, const void* cd = NULL )
        {
            dword_t currtime = TimeClock();
            
            TS_Lock lock( &ut_lock_ );
            _collectpool_iterator iter = recyclebin_.begin();
            while( iter != recyclebin_.end() )
            {
                // 沒有建立條件
                if( func == NULL )
                {
                    // 這個已經放在回收桶 超過 n 秒了 拿來用
                    if( ( currtime - iter->second ) > ( uint_t )overtm )
                    {
                        _Type* tt = iter->first;
                        recyclebin_.erase( iter );
                        return tt;
                    }
                }
                else
                {
                    _Type* tt = iter->first;
                    if( func( tt, cd ) )
                    {
                        recyclebin_.erase( iter );
                        return tt;
                    }
                }
                iter++;
            }
            
            // 找不到 建立新的吧
            _Type* tt = new _Type;
            _collectpool_value_type data( tt, currtime );
            container_.insert( data );
            return tt;
        }
        
        void recycle( _Type* t )
        {
            dword_t currtime = TimeClock();
            
            TS_Lock lock( &ut_lock_ );
            // 原本就已經在回收區 --> 重設時間
            _collectpool_iterator iter = recyclebin_.find( t );
            if( iter != recyclebin_.end() )
            {
                iter->second = currtime;
                return;
            }

            container_.erase( t );
            _collectpool_value_type data( t, currtime );
            recyclebin_.insert( data );
            return;
        }

        void release( _Type* t )
        {
            dword_t currtime = TimeClock();

            TS_Lock lock( &ut_lock_ );
            _collectpool_iterator iter = recyclebin_.find( t );
            if ( iter != recyclebin_.end() )
            {
                iter->second = currtime;
                return;
            }

            container_.erase( t );
            recyclebin_.erase( t );
            if ( t )
            {
                delete t;
            }
            t = NULL;
            return;
        }
        
        // 要清出 被回收多久的 要清除幾個 為了 避免lock 住太久的時間 0 就是清到完
        // 全部都找過了 傳回 1 否則傳回0
        int clearup( int overtm, int n )
        {
            dword_t currtime = TimeClock();
            
            TS_Lock lock( &ut_lock_ );
            std::vector<_Type*> delvec;
            int cnt = 0;
            int ret = 1;
            _collectpool_iterator iter = recyclebin_.begin();
            while( iter != recyclebin_.end() )
            {
                cnt++;
                // 這個已經放在回收桶 超過 n 秒了 砍掉他
                if( ( currtime - iter->second ) > ( uint_t )overtm )
                {
                    _Type* tt = iter->first;
                    delvec.push_back( tt );
                }
                
                if( ( n > 0 ) && ( ( int )delvec.size() >= n ) )
                {
                    ret = 0;
                    break;
                }
                iter++;
            }
            
            for( int i = 0; i < delvec.size(); i++ )
            {
                recyclebin_.erase( delvec[i] );
                container_.erase( delvec[i] );
                if( delvec[i] )
                { 
                    delete delvec[i]; 
                }
                delvec[i] = NULL;
            }            
            delvec.clear();
            return ret;
        }
        
        void destroy()
        {
            TS_Lock lock( &ut_lock_ );
            recyclebin_.clear();
            _collectpool_iterator iter = container_.begin();
            while( iter != container_.end() )
            {
                _Type* tt = iter->first;
                if( tt )
                { delete tt; }
                tt = NULL;
                iter++;
            }
            container_.clear();
        }
    };
}