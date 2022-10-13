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
#pragma once

#include <pf-def.h>
#include <vector>
#include <string>

namespace xdb
{
    class RedisDB
    {
        void*      v_redis_client_;

    public:

        RedisDB();
        ~RedisDB();

        bool             Connect( _IN const char* host, _IN unsigned short port );

        bool             Set( _IN const char* key, _IN const char* value );
        _mstring         Get( _IN const char* key );
        bool             Del( _IN const char* key );

        bool             HSet( _IN const char* key, _IN const char* field, _IN const char* value );
        _mstring         HGet( _IN const char* key, _IN const char* field );
        bool             HDel( _IN const char* key, _IN const char* field );
        int              header_len( _IN const char* key );
        _mbsvector       HMGet( _IN const char* key, _IN const char* field );
        _mbsvector       HKeys( _IN const char* key );
        _mbsvector       HVals( _IN const char* key );

        //  放入第一個value
        bool             LPush( _IN const char* key, _IN const char* value );
        //  放入最後一個value
        bool             RPush( _IN const char* key, _IN const char* value );
        //  移除第一個value
        _mstring         LPop( _IN const char* key );
        //  移除最後一個value
        _mstring         RPop( _IN const char* key );
        int              LLen( _IN const char* key );
        _mbsvector       LRange( _IN const char* key, _IN int start, _IN int stop );
    };
}