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

//01. == C 系統文件/C++ 系統文件
#if defined(_WINDOWS)  /* Windows platforms */
    #include <windows.h>
#endif
#if defined(_LINUX)
    #include <unistd.h>
    #include <pthread.h>
    static const pthread_mutex_t gc_pmutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#include <assert.h>
#include <sys/timeb.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "t_lock.h"


T_Lock::T_Lock()
{
#if defined(_WINDOWS)  /* Windows platforms */
    CS_ = new CRITICAL_SECTION;
#endif
#if defined(_LINUX)
    CS_ = new pthread_mutex_t;
#endif
#if defined(_WINDOWS) /* Windows platforms */
    InitializeCriticalSection( ( CRITICAL_SECTION* )CS_ );
#endif
#if defined(_LINUX)
    *( ( pthread_mutex_t* )CS_ ) = gc_pmutex;
#endif
    is_init_ = true;
    lock_local_ = 0;
    lock_time_  = 0;
    longest_lock_local_ = 0;
    longest_lock_time_ = 0;
    //s_T_LockPool.insert(std::map<T_Lock*, int>::value_type(this, 0))
}


T_Lock::~T_Lock()
{
    if( CS_ != NULL )
    {
#if defined(_WINDOWS)  /* Windows platforms */
        DeleteCriticalSection( ( CRITICAL_SECTION* )CS_ );
        CRITICAL_SECTION* dc = ( CRITICAL_SECTION* )CS_;
        SAFE_DELETE( dc );
#endif
#if defined(_LINUX)
        pthread_mutex_t* dc = ( pthread_mutex_t* )CS_;
        SAFE_DELETE( dc );
#endif
    }

    //s_T_LockPool.erase(this);
    is_init_ = false;
}

void T_Lock::Lock( unsigned id )
{
    if( is_init_ )
    {
#if defined(_WINDOWS)  /* Windows platforms */
        EnterCriticalSection( ( CRITICAL_SECTION* )CS_ );
        lock_time_ = ::GetTickCount();
#endif
#if defined(_LINUX)
        pthread_mutex_lock( ( pthread_mutex_t* )CS_ );
        struct timeb tb;
        ftime( &tb );
        lock_time_ = ( ( ( tb.time - 1000000000 ) * 1000 ) + ( tb.millitm ) );
#endif
        if( id <= 0 )
            id = 1;
        lock_local_ = id;
    }
}

void T_Lock::UnLock()
{
    if( is_init_ )
    {
#if defined(_WINDOWS) /* Windows platforms */
        dword_t dt = ::GetTickCount() - lock_time_;
#endif
#if defined(_LINUX)
        struct timeb tb;
        ftime( &tb );
        unsigned int dt = ( ( ( tb.time - 1000000000 ) * 1000 ) + ( tb.millitm ) ) - lock_time_;
#endif

        if( dt > longest_lock_time_ )
        {
            longest_lock_local_ = lock_local_;
            longest_lock_time_ = dt;
        }

        lock_time_  = 0;
        lock_local_ = 0;
#if defined(_WINDOWS)  /* Windows platforms */
        LeaveCriticalSection( ( CRITICAL_SECTION* )CS_ );
#endif
#if defined(_LINUX)
        pthread_mutex_unlock( ( pthread_mutex_t* )CS_ );
#endif
    }
}

int T_Lock::TryLock()
{
    if( is_init_ )
    {
#if defined(_WINDOWS) /* Windows platforms */
        dword_t dt = ::GetTickCount() - lock_time_;
#endif
#if defined(_LINUX)
        struct timeb tb;
        ftime( &tb );
        unsigned int dt = ( ( ( tb.time - 1000000000 ) * 1000 ) + ( tb.millitm ) ) - lock_time_;
#endif

        if( dt > longest_lock_time_ )
        {
            longest_lock_local_ = lock_local_;
            longest_lock_time_ = dt;
        }

        lock_time_ = 0;
        lock_local_ = 0;
#if defined(_WINDOWS)  /* Windows platforms */
        // If the critical section is successfully entered or the current thread already owns the critical section, the return value is nonzero.
        // If another thread already owns the critical section, the return value is zero.
        return TryEnterCriticalSection( ( CRITICAL_SECTION* )CS_ );
#endif
#if defined(_LINUX)
        //EBUSY   The mutex is already locked.
        //EINVAL  mutex is not an initialized mutex.
        //EFAULT  mutex is an invalid pointer.
        return pthread_mutex_trylock( ( pthread_mutex_t* )CS_ );
#endif
    }

    return -1;
}

unsigned T_Lock::GetMaxLockLocal() const
{
    return longest_lock_local_;
}

unsigned int T_Lock::GetMaxLockMSec() const
{
    return longest_lock_time_;
}

TS_Lock::TS_Lock( T_Lock* lock, unsigned id )
{
    lock_local_ = id;
    lock->Lock( lock_local_ );
    lock_ = lock;
}

TS_Lock::~TS_Lock( void )
{
    lock_->UnLock();
    lock_local_ = 0;
}



