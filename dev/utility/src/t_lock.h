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
/// must include on last
// 因為需要include 為避免太多問題 將windows 放入CPP中
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <pf-def.h>

class T_Lock
{

private:

    bool                is_init_;
    unsigned long int   lock_local_;         // 可以設定一個數字當作lock 的編號方便除錯
    unsigned long int   lock_time_;
    
    unsigned long int   longest_lock_local_;  // 目前鎖最久的編號
    unsigned long int   longest_lock_time_;   // 目前鎖最久的時間

    void*               CS_;
    
public:

    T_Lock();
    ~T_Lock();
    
    void                Lock( unsigned id = 1 );
    void                UnLock();
    int                 TryLock();
    unsigned            GetMaxLockLocal() const;
    unsigned int        GetMaxLockMSec() const;
};


class TS_Lock
{
    unsigned long int   lock_local_;
    T_Lock*             lock_;
public:

    TS_Lock( T_Lock* lock, unsigned id = 1 );
    ~TS_Lock( void );
};

