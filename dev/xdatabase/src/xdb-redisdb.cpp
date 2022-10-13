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
#include "xdb-redisdb.h"
#include <pf-stdlib.h>
#include <cpp_redis/cpp_redis>


#ifdef _WINDOWS
#include <Winsock2.h>
#endif /* _WIN32 */

#include <t_lock.h>

namespace xdb
{
#define _CPP_REDIS_CLIENT   ((cpp_redis::client*)v_redis_client_)
#define _MAX_REDIS_LOCK     100

    namespace
    {
        //T_Lock    s_redis_lock[ _MAX_REDIS_LOCK ];
        //_mstring  s_redis_reply[ _MAX_REDIS_LOCK ];
    }

    //========================================================================
    //
    //
    //========================================================================
    RedisDB::RedisDB()
    {
#ifdef _WINDOWS
        //! Windows netword DLL init
        WORD version = MAKEWORD( 2, 2 );
        WSADATA data;

        if( WSAStartup( version, &data ) != 0 ) {
            std::cerr << "WSAStartup() failure" << std::endl;
            return;
        }
#endif /* _WIN32 */

        cpp_redis::client* cpp_redis_client = new cpp_redis::client;
        v_redis_client_ = cpp_redis_client;
    }

    //========================================================================
    //
    //
    //========================================================================
    RedisDB::~RedisDB()
    {
        cpp_redis::client* cpp_redis_client = _CPP_REDIS_CLIENT;
        SAFE_DELETE( cpp_redis_client );
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::Connect( const tchar_t* host, unsigned short port )
    {
        _CPP_REDIS_CLIENT->connect( host, port, []( const std::string& host, std::size_t port, cpp_redis::client::connect_state status )
        {
            if( status == cpp_redis::client::connect_state::dropped ) 
            {
                std::cout << ("client disconnected from ") << host << (":") << port << std::endl;
            }
        } );
        return true;
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::Set( const char* key, const char* value )
    {
        //_CPP_REDIS_CLIENT->send( ("SET"), key, value );
        //_CPP_REDIS_CLIENT->send( { "SET", key, value }, []( cpp_redis::reply& reply )
        //cpp_redis::reply reply = _CPP_REDIS_CLIENT->send( { "SET", key, value } );
        //_CPP_REDIS_CLIENT->sync_commit();

        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "SET", key, value } );
        _CPP_REDIS_CLIENT->commit();
        cpp_redis::reply reply = set_reply.get();
        return reply.ok();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mstring RedisDB::Get( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "GET", key } );
        _CPP_REDIS_CLIENT->commit();
        if( set_reply.get().is_null() )
        {
            return ( "" );
        }
        return set_reply.get().as_string();
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::Del( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "DEL", key } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().ok();
    }


    //========================================================================
//
//
//========================================================================
    bool RedisDB::HSet( const char* key, const char* field, const char* value )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HSET", key, field, value } );
        _CPP_REDIS_CLIENT->commit();
        cpp_redis::reply reply = set_reply.get();
        return reply.ok();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mstring RedisDB::HGet( const char* key, const char* field )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HGET", key, field } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().as_string();
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::HDel( const char* key, const char* field )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HDEL", key, field } );
        _CPP_REDIS_CLIENT->commit();
        cpp_redis::reply reply = set_reply.get();
        return reply.ok();
    }

    //========================================================================
    //
    //
    //========================================================================
    int RedisDB::header_len( _IN const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HLEN", key } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().as_integer();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mbsvector RedisDB::HMGet( _IN const char* key, _IN const char* field )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HMGET", key, field } );
        _CPP_REDIS_CLIENT->commit();

        _mbsvector fields;
        std::vector<cpp_redis::reply> replay_arry = set_reply.get().as_array();
        for( uint_t i = 0; i < replay_arry.size(); i++ )
        {
            fields.push_back( replay_arry[ i ].as_string() );
        }
        return fields;
    }

    //========================================================================
    //
    //
    //========================================================================
    _mbsvector RedisDB::HKeys( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HKEYS", key } );
        _CPP_REDIS_CLIENT->commit();

        _mbsvector fields;
        std::vector<cpp_redis::reply> replay_arry = set_reply.get().as_array();
        for( uint_t i = 0; i < replay_arry.size(); i++ )
        {
            fields.push_back( replay_arry[ i ].as_string() );
        }
        return fields;
    }

    //========================================================================
    //
    //
    //========================================================================
    _mbsvector RedisDB::HVals( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "HVALS", key } );
        _CPP_REDIS_CLIENT->commit();

        _mbsvector values;
        std::vector<cpp_redis::reply> replay_arry = set_reply.get().as_array();
        for( uint_t i = 0; i < replay_arry.size(); i++ )
        {
            values.push_back( replay_arry[ i ].as_string() );
        }
        return values;
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::LPush( const char* key, const char* value )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "LPUSH", key, value } );
        _CPP_REDIS_CLIENT->commit();
        cpp_redis::reply reply = set_reply.get();
        return reply.ok();
    }

    //========================================================================
    //
    //
    //========================================================================
    bool RedisDB::RPush( const char* key, const char* value )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "RPUSH", key, value } );
        _CPP_REDIS_CLIENT->commit();
        cpp_redis::reply reply = set_reply.get();
        return reply.ok();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mstring RedisDB::LPop( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "LPOP", key } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().as_string();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mstring RedisDB::RPop( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "RPOP", key } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().as_string();
    }

    //========================================================================
    //
    //
    //========================================================================
    int RedisDB::LLen( const char* key )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "LLEN", key } );
        _CPP_REDIS_CLIENT->commit();
        return set_reply.get().as_integer();
    }

    //========================================================================
    //
    //
    //========================================================================
    _mbsvector RedisDB::LRange( _IN const char* key, _IN int start, _IN int stop )
    {
        std::future<cpp_redis::reply> set_reply = _CPP_REDIS_CLIENT->send( { "LRANGE", key, _pf_itot( start ).c_str(), _pf_itot( stop ).c_str() } );
        _CPP_REDIS_CLIENT->commit();

        _mbsvector values;
        std::vector<cpp_redis::reply> replay_arry = set_reply.get().as_array();
        for( uint_t i = 0; i < replay_arry.size(); i++ )
        {
            values.push_back( replay_arry[ i ].as_string() );
        }
        return values;
    }

}
