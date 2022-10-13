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
//  暫時放這以後要改遊戲server 在搬
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件
#include <vector>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-cstring.h>
#include <pf-stdlib.h>
#include <pf-string.h>
#include <pf-debug.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-db_query_def.h"

namespace cs_db_query
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // entity
    namespace player
    {
        inline _cs_string GetTableName( _IN uniqueid_t member_uid )
        {
            _cs_string tbname;
            _pf_tstrprintf( tbname, _u8( "csplayer_%010d" ), _GET_PLAYER_TABLE_IDX( member_uid ) );            
            return tbname;
        }

        inline _cs_string IfTableExist( _IN uniqueid_t member_uid )
        {
            _cs_string out;
            pfstd::strprintf( out, _u8( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = '%s'" ), GetTableName( member_uid ).c_str() );
            return out;
        }

        inline _cs_string CreateTable( _IN uniqueid_t member_uid )
        {
            _cs_string out;
            pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
        `player_uid` bigint( 20 ) UNSIGNED NOT NULL COMMENT '角色的唯一編號',\n\
        `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期',\n\
        `update_time` datetime(0) ON UPDATE CURRENT_TIMESTAMP COMMENT '資料修改的時間',\n\
        `state` char( 255 ) NOT NULL DEFAULT '1' COMMENT '角色狀態 0:刪除(不應該立刻刪除, 所以使用狀態保留) 1:使用',\n\
        `member_uid` bigint( 20 ) UNSIGNED NOT NULL COMMENT '擁有角色的會員編號',\n\
        `guild_uid` int( 11 ) NOT NULL DEFAULT '0' COMMENT '角色的公會編號',\n\
        `name` char( 255 ) NOT NULL DEFAULT '' COMMENT '角色姓名',\n\
        `length` int( 11 ) DEFAULT '0' COMMENT '角色資料結構長度',\n\
        `property` blob NOT NULL COMMENT '角色資料結構',\n\
        `create_ip` char( 100 ) DEFAULT '0',\n\
        `create_mac` bigint( 20 ) DEFAULT '0',\n\
        `delete_time` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',\n\
        `delete_ip` char( 100 ) DEFAULT '0',\n\
        `delete_mac` bigint( 20 ) DEFAULT NULL,\n\
        PRIMARY KEY( `player_uid`, `member_uid` )\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH(member_uid)\n\
PARTITIONS %d" ),
GetTableName( member_uid ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsPlayerDBTable );
            return out;
        }

        inline _cs_string InsertEntity( _IN cs_session_uid_t member_uid, _IN const _cs_char_t* name, _IN const ubyte_t* propbuf, _IN int len, _IN const _cs_char_t* fromip, _IN const macn_t frommac )
        {
            if( len <= 0 )
            {
                return NULL;
            }

            if( len >= 10000 )
            {
                return NULL;
            }

            //const _cs_char_t *psource = (const _cs_char_t *)propbuf;
            _cs_size_t buff_len = ( len * 2 );
            _binbuf query;
            query.resize( buff_len );
            ubyte_t* outbuf = &( query[ 0 ] );
            _cs_size_t binlen = bintostr( outbuf, buff_len, propbuf, len );
            std::vector<_cs_char_t> buf;
            buf.resize( 512 + binlen );
            _cs_char_t* bufptr = &( buf[ 0 ] );
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* name_safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( name_safestring, escoutlen, name, _csstrlen( name ) );
            sprintf( bufptr, "INSERT INTO %s( member_uid, name, length, property, create_time, create_ip, create_mac ) VALUES( '%llu', '%s', '%d', '%s', NOW(), '%s', '%lld' )", GetTableName( member_uid ).c_str(), member_uid, name_safestring, len, outbuf, fromip, frommac );
            SAFE_DELETE_ARRAY( name_safestring );
            return bufptr;
            /*
            sprintf(buf, "INSERT INTO csplayer(member_uid, name, property) VALUES('%llu', '%s', '", member_uid, name);
            long32_t index = _csstrlen(buf);

            _cs_size_t buff_len = index + (len*2) + _csstrlen("')") + 256;
            std::vector< _cs_char_t > query;
            query.resize(buff_len);

            _cs_char_t* outbuf=NULL;
            if(buff_len>0)
            {
            outbuf=&(query[0]);
            memcpy(outbuf, buf, index*_cssizeof(_cs_char_t));
            index += _MakeBinaryToString(outbuf+index, buff_len, propbuf, len);

            sprintf(&(outbuf[index]), "')");
            }
            //_cs_string s;
            //s=outbuf;
            //memset(outbuf, 0, buff_len);
            //delete [] outbuf;
            //return s;
            return outbuf;
            */
        }

        inline _cs_string RemoveEntity( _IN uniqueid_t member_uid )
        {
            _cs_char_t buf[ 512 ];
            sprintf( buf, "DELETE FROM %s WHERE player_uid = '%llu'", GetTableName( member_uid ).c_str(), member_uid );
            return buf;
        }

        inline _cs_string RemoveEntity( _IN uniqueid_t member_uid, _IN const _cs_char_t* name )
        {
            _cs_char_t buf[ 512 ];
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            sprintf( buf, "DELETE FROM %s WHERE name = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE_ARRAY( safestring );
            return buf;
        }

        // 標記起來 而不是真正刪除
        inline _cs_string MarkDeleteEntity( _IN uniqueid_t member_uid, _IN const _cs_char_t* name, _IN const _cs_char_t* fromip, _IN const macn_t frommac )
        {
            _cs_char_t buf[ 512 ];
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            sprintf( buf, "UPDATE %s SET delete_time = NOW(), state = '0', delete_ip = '%s', delete_mac = '%lld' WHERE name = '%s'", GetTableName( member_uid ).c_str(), fromip, frommac, safestring );
            SAFE_DELETE_ARRAY( safestring );
            return buf;
        }

        inline _cs_string UpdateEntity( _IN uniqueid_t member_uid, _IN uniqueid_t eid, _IN const ubyte_t* propbuf, _IN int len )
        {
            if( len <= 0 )
            {
                return NULL;
            }

            if( len >= 10000 )
            {
                return NULL;
            }

            _cs_size_t buff_len = ( len * 2 ) + 20;
            _binbuf query;
            query.resize( buff_len );
            ubyte_t* outbuf = &( query[ 0 ] );
            _cs_size_t binlen = bintostr( outbuf, buff_len, propbuf, len );
            std::vector<_cs_char_t> buf;
            buf.resize( 512 + binlen );
            _cs_char_t* bufptr = &( buf[ 0 ] );
            sprintf( bufptr, "UPDATE %s SET length = '%d', property = '%s' WHERE player_uid = '%llu'", GetTableName( member_uid ).c_str(), len, outbuf, eid );
            return bufptr;
            /*
            sprintf(buf, "UPDATE csplayer SET property = '");
            _cs_size_t index = _csstrlen(buf);

            _cs_size_t buff_len = index + (len*2) + _csstrlen("' WHERE player_uid = '%llu'") + 256;
            std::vector< _cs_char_t > query;
            query.resize(buff_len);
            _cs_char_t* outbuf = NULL;
            if(buff_len>0)
            {
            outbuf=&(query[0]);
            memcpy(outbuf, buf, index * _cssizeof(_cs_char_t));
            index += _MakeBinaryToString(outbuf + index, buff_len - index, propbuf, len);
            sprintf(outbuf+index, "' WHERE player_uid = '%llu'", eid);
            }
            return outbuf;
            */
        }

        inline _cs_string GetEntityRecord( _IN uniqueid_t member_uid, _IN const _cs_char_t* name )
        {
            _cs_char_t out[ 1024 ];
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            sprintf( out, "SELECT member_uid, player_uid, property, UNIX_TIMESTAMP(delete_time) FROM %s WHERE name = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE_ARRAY( safestring );
            return out;
        }

        inline _cs_string GetEntityRecord( _IN uniqueid_t member_uid, _IN uniqueid_t eid )
        {
            _cs_string out;
            pfstd::strprintf( out, "SELECT member_uid, name, property, UNIX_TIMESTAMP(delete_time) FROM %s WHERE player_uid = '%llu'", GetTableName( member_uid ).c_str(), eid );
            //sprintf(out,"SELECT member_uid, name, property FROM csplayer WHERE player_uid = '%d'",eid);
            return out;
        }

        inline _cs_string GetEntityProperity( _IN uniqueid_t member_uid, _IN uniqueid_t eid )
        {
            _cs_string out;
            pfstd::strprintf( out, "SELECT property FROM %s WHERE player_uid = %llu", GetTableName( member_uid ).c_str(), eid );
            //sprintf(out,"SELECT member_uid, name, property FROM csplayer WHERE player_uid = '%d'",eid);
            return out;
        }

        inline _cs_string GetEntityUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* name )
        {
            _cs_string out;
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            pfstd::strprintf( out, "SELECT player_uid FROM %s WHERE name = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE_ARRAY( safestring )
                return out;
        }

        inline _cs_string SetEntityState( _IN uniqueid_t member_uid, _IN uniqueid_t eid, _IN int state )
        {
            _cs_string out;
            pfstd::strprintf( out, "UPDATE %s SET state = '%d' WHERE player_uid = '%llu'", GetTableName( member_uid ).c_str(), state, eid );
            //_cs_char_t out[512];
            //sprintf(out, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, aid);
            //sprintf(out, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, aid);
            return out;
        }

        inline _cs_string GetEntityState( _IN uniqueid_t member_uid, _IN uniqueid_t eid )
        {
            _cs_string out;
            pfstd::strprintf( out, "SELECT state FROM %s WHERE player_uid = '%llu'", GetTableName( member_uid ).c_str(), eid );
            return out;
        }

        inline _cs_string SetEntityState( _IN uniqueid_t member_uid, _IN const _cs_char_t* name, _IN int state )
        {
            _cs_string out;
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            pfstd::strprintf( out, "UPDATE %s SET state = '%d' WHERE name = '%s'", GetTableName( member_uid ).c_str(), state, safestring );
            SAFE_DELETE_ARRAY( safestring );
            //_cs_char_t out[512];
            //sprintf(out, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, aid);
            //sprintf(out, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, aid);
            return out;
        }

        inline _cs_string GetEntityState( _IN uniqueid_t member_uid, _IN const _cs_char_t* name )
        {
            _cs_string out;
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            pfstd::strprintf( out, "SELECT state FROM %s WHERE name = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE_ARRAY( safestring );
            return out;
        }

        inline _cs_char_t* QueryEntitiesOfMember( _IN uniqueid_t member_uid, _IN _cs_char_t* out, _IN cs_session_uid_t aid )
        {
            sprintf( out, "SELECT player_uid FROM %s WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), aid );
            return out;
        }
    }
}

