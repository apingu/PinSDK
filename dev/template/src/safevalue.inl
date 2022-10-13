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
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <t_lock.h>

template <class _Type> class safevalue
{
    T_Lock lock_;
    _Type  value_;

public:

    safevalue() {};
    ~safevalue() {};

    inline void SetValue( const _Type v )
    {
        TS_Lock lock( &lock_ );
        value_ = v;
    }

    inline _Type GetValue( void )
    {
        TS_Lock lock( &lock_ );
        return value_;
    }

    inline _Type& operator= ( const _Type v )
    {
        TS_Lock lock( &lock_ );
        return value_ = v;
    }

    inline operator _Type()
    {
        TS_Lock lock( &lock_ );
        return value_;
    }

    //  inline _Type& operator++ ( const _Type v )
    //  {
    //      TS_Lock lock(&lock_);
    //      return value_++;
    //  }
    //
    //  inline _Type& operator-- ( const _Type v )
    //  {
    //      TS_Lock lock(&lock_);
    //      return value_--;
    //  }
};