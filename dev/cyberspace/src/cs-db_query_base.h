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
//    member paper log bulletin
//
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
#include <algorithmic/crypt-base32.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-db_query_def.h"
#include "cs-uid_generator.h"

//#include "CsGameDef.h"//暫時定義，整合完拿掉

//const char tbn_uniqueid[] = { "csuniqueid" };
//const char tbn_member[] = { "csmember_%05d" };
//const char tbn_player[] = { "csplayer" };
//const char tbn_history[] = { "cshistory_%05d" };



//extern int db_encode_data(const _cs_char_t *data,int dlen,_cs_char_t *encdata);
//extern int db_decode_data(const _cs_char_t *encdata,_cs_char_t *decbuff);

//--------------------------------------------------------
// Database Command Define
//--------------------------------------------------------
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)       "select member_uid from csmember_%05d where username = '%s' and password = '%s'",usr,pwd
//#define TRY_CsPSockConn    10
namespace cs_db_query
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    /*
    namespace utility
    {
        inline _cs_string LockTable( const _cs_char_t* tbl_name, int rw = 1 )
        {
            _cs_string query;

            if( rw == 1 )
            {
                _pf_tstrprintf( query, "LOCK TABLES %s WRITE", tbl_name );
            }
            else
            {
                _pf_tstrprintf( query, "LOCK TABLES %s READ", tbl_name );
            }

            return query;
        }


        inline _cs_string UnLockTable( void )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UNLOCK TABLES" );
            return query;
        }

        inline _cs_string SetCharSet( const _cs_char_t* charset )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SET NAMES '%s'", charset );
            return query;
        }
    }
    */

    //////////////////////////////////////////////////////////////////////////
    //
    /*
    namespace uniqueid
    {
        inline _cs_string IfTableExist_csuniqueid( _IN int tbidx )
        {
            _cs_string query;
            _pf_tstrprintf( query, " SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'csuniqueid' ", tbidx );
            return query;
        }

        inline _cs_string CreateUniqueId( uint_t group )
        {
            _cs_string query;
            _pf_tstrprintf( query, "INSERT INTO csuniqueid(gid) VALUES('%d')", group );
            return query;
        }

        inline _cs_string DeleteUniqueId( uniqueid_t uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "DELETE FROM csuniqueid WHERE uid = '%%llu'", uid );
            return query;
        }

        inline _cs_string GetLastInsertId( void )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT LAST_INSERT_ID()" );
            return query;
        }

        inline _cs_string GetIdGroup( uniqueid_t uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT gid FROM csuniqueid WHERE uid = '%llu'", uid );
            return query;
        }
    }*/

    namespace utility
    {
        inline _cs_string MakeRecordAddress( _IN const _cs_char_t* tbname, _IN const _cs_char_t* uid )
        {
            _cs_string address;
            _pf_tstrprintf( address, ( "%s@%s" ), uid, tbname );
            return base32::Encode( ( const unsigned char* )address.c_str(), address.size() * sizeof( _cs_char_t ) );
        }

        inline bool SplitRecordAddress( _OUT _cs_string& tbname, _OUT _cs_string& uid, _IN const _cs_char_t* address )
        {
            _cs_string decode_address = base32::Decode( address );
            _cs_string_vector value = _pf_tstring_split( decode_address.c_str(), ( "@" ) );
            if( value.size() != 2 )
                return false;

            uid = value[ 0 ];
            tbname = value[ 1 ];
            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // csmember_%05d table
    namespace member
    {
        inline _cs_string GetIndexName()
        {
            _cs_string tbname;
            _pf_tstrprintf( tbname, _u8( "csmember_index" ) );
            return tbname;
        }

        inline _cs_string GetTableName( _IN uniqueid_t member_uid )
        {
            _cs_string tbname;
            // 20220909 後面要多帶0 否則再 linux 下 會被當成  _IN va_list args
            _pf_tstrprintf( tbname, _u8( "csmember_%010d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
            //_pf_tstrprintf( tbname, _u8( "csmember_0000000000" ) );
            return tbname;
        }

        inline _cs_string CreateIndex()
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( \
"CREATE TABLE IF NOT EXISTS `%s`\n\
 (\n\
    `member_idx` bigint(20) NOT NULL AUTO_INCREMENT,\n\
    `member_uid` bigint(20) UNSIGNED NOT NULL COMMENT '帳號編號',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期',\n\
    `update_time` datetime(0) ON UPDATE CURRENT_TIMESTAMP COMMENT '資料修改的時間',\n\
    `username` char( 255 ) DEFAULT NULL,\n\
    `group_uid` char( 255 ) DEFAULT NULL,\n\
    `note01` char( 255 ) DEFAULT NULL,\n\
    `note02` char( 255 ) DEFAULT NULL,\n\
    `note03` char( 255 ) DEFAULT NULL,\n\
    `note04` char( 255 ) DEFAULT NULL,\n\
    `note05` char( 255 ) DEFAULT NULL,\n\
    `note06` char( 255 ) DEFAULT NULL,\n\
    `note07` char( 255 ) DEFAULT NULL,\n\
    `note08` char( 255 ) DEFAULT NULL,\n\
    PRIMARY KEY (`member_idx`, `member_uid`) USING BTREE,\n\
    UNIQUE INDEX `member_uid`(`member_idx`, `member_uid` ) USING BTREE\n\
) ENGINE = InnoDB DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( member_uid )\n\
PARTITIONS % d" ),
GetIndexName().c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsMemberDBTable );
            return query;
        }

        inline _cs_string CreateTable( _IN uniqueid_t member_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( \
"CREATE TABLE IF NOT EXISTS `%s`\n\
(\n\
    `member_idx` bigint(20) NOT NULL AUTO_INCREMENT,\n\
    `member_uid` bigint(20) UNSIGNED NOT NULL COMMENT '帳號編號',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期',\n\
    `update_time` datetime(0) ON UPDATE CURRENT_TIMESTAMP COMMENT '資料修改的時間',\n\
    `state` char( 255 ) DEFAULT '0' COMMENT '0:未登入, 1:登入中, 2:禁止登入 3:登出中, 4:關閉',\n\
    `username` char( 255 ) DEFAULT NULL,\n\
    `password` char( 255 ) DEFAULT NULL,\n\
    `nickname` char( 255 ) DEFAULT NULL,\n\
    `mac` bigint( 20 ) DEFAULT '0' COMMENT 'mac address(目前沒使用)',\n\
    `information` varchar( 1024 ) NOT NULL DEFAULT '0' COMMENT '使用者的資訊',\n\
    `group_uid` char( 255 ) DEFAULT '0' COMMENT '哪一個群組',\n\
    `type` int( 11 ) DEFAULT '2' COMMENT '0:允許mac為0 2:不允許mac為0(目前沒使用)',\n\
    `server_uid` int( 11 ) DEFAULT 0 COMMENT '連線到哪一台server',\n\
    `acc_quota` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有額度, 信用 可以把額度跟 加扣 都放這, quota + point00~08 才應該是真正的帳戶餘額',\n\
    `acc_point00` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲點數)',\n\
    `acc_point01` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢01?)',\n\
    `acc_point02` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢02?)',\n\
    `acc_point03` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢03?)',\n\
    `acc_point04` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢04?)',\n\
    `acc_point05` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢05?)',\n\
    `acc_point06` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢06?)',\n\
    `acc_point07` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢07?)',\n\
    `flag` int( 11 ) DEFAULT 0 COMMENT '保留',\n\
    `pri_permissionl` bigint( 20 ) NOT NULL DEFAULT 0 COMMENT '私有權限(不該被傳給client)permission 自行定義 像是可用在表明身分(一般玩家 VIP玩家 NPC?)',\n\
    `pub_permissionl` bigint( 20 ) NOT NULL DEFAULT 0 COMMENT '公開權限(應該被傳給client)permission',\n\
    `pri_permissions` varchar( 2048 ) NOT NULL DEFAULT '0' COMMENT '私有權限(不該被傳給client)permission',\n\
    `pub_permissions` varchar( 2048 ) NOT NULL DEFAULT '0' COMMENT '公開權限(應該被傳給client) permission',\n\
    `verify_sec` int( 10 ) UNSIGNED ZEROFILL DEFAULT NULL,\n\
    `lastlogin_time` datetime DEFAULT NULL COMMENT '最後登入時間',\n\
    `lastlogout_time` datetime DEFAULT NULL COMMENT '最後登出時間',\n\
    `note01` char( 255 ) DEFAULT NULL,\n\
    `note02` char( 255 ) DEFAULT NULL,\n\
    `note03` char( 255 ) DEFAULT NULL,\n\
    `note04` char( 255 ) DEFAULT NULL,\n\
    `note05` char( 255 ) DEFAULT NULL,\n\
    `note06` char( 255 ) DEFAULT NULL,\n\
    `note07` char( 255 ) DEFAULT NULL,\n\
    `note08` char( 255 ) DEFAULT NULL,\n\
    PRIMARY KEY (`member_idx`, `member_uid`) USING BTREE,\n\
    UNIQUE INDEX `member_uid`(`member_idx`, `member_uid` ) USING BTREE\n\
) ENGINE = InnoDB DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH(member_uid)\n\
PARTITIONS %d" ),
member::GetTableName( member_uid ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsMemberDBTable );
            //_pf_tstrprintf( query, _u8( "CREATE TABLE IF NOT EXISTS `%s` (\
                     //                `member_uid` bigint(20) UNSIGNED PRIMARY KEY NOT NULL COMMENT '帳號編號',\
            //                         `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期',\
            //                         `update_time` datetime(0) ON UPDATE CURRENT_TIMESTAMP COMMENT '資料修改的時間',\
                     //                `state` char( 255 ) DEFAULT '0' COMMENT '0:未登入, 1:登入中, 2:禁止登入 3:登出中, 4:關閉',\
                     //                `username` char( 255 ) DEFAULT NULL,\
                     //                `password` char( 255 ) DEFAULT NULL,\
            //                         `nickname` char( 255 ) DEFAULT NULL,\
                     //                `mac` bigint( 20 ) DEFAULT '0' COMMENT 'mac address(目前沒使用)',\
            //                         `information` varchar( 1024 ) NOT NULL DEFAULT '0' COMMENT '使用者的資訊',\
                     //                `group_uid` char( 255 ) DEFAULT '0' COMMENT '哪一個群組',\
                     //                `type` int( 11 ) DEFAULT '2' COMMENT '0:允許mac為0 2:不允許mac為0(目前沒使用)',\
                     //                `server_uid` int( 11 ) DEFAULT 0 COMMENT '連線到哪一台server',\
            //                         `acc_quota` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有額度, 信用 可以把額度跟 加扣 都放這, quota + point00~08 才應該是真正的帳戶餘額',\
                     //                `acc_point00` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲點數)',\
            //                         `acc_point01` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢01?)',\
            //                         `acc_point02` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢02?)',\
            //                         `acc_point03` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢03?)',\
            //                         `acc_point04` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢04?)',\
            //                         `acc_point05` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢05?)',\
            //                         `acc_point06` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢06?)',\
            //                         `acc_point07` decimal( 20, 4 ) DEFAULT 0  COMMENT '帳號擁有點數 遊戲內容直接加減在這邊(擺遊戲水錢07?)',\
            //                         `flag` int( 11 ) DEFAULT 0 COMMENT '保留',\
                     //                `pri_permissionl` bigint( 20 ) NOT NULL DEFAULT 0 COMMENT '私有權限(不該被傳給client)permission 自行定義 像是可用在表明身分(一般玩家 VIP玩家 NPC?)',\
                     //                `pub_permissionl` bigint( 20 ) NOT NULL DEFAULT 0 COMMENT '公開權限(應該被傳給client)permission',\
                     //                `pri_permissions` varchar( 2048 ) NOT NULL DEFAULT '0' COMMENT '私有權限(不該被傳給client)permission',\
                     //                `pub_permissions` varchar( 2048 ) NOT NULL DEFAULT '0' COMMENT '公開權限(應該被傳給client) permission',\
                     //                `verify_sec` int( 10 ) unsigned zerofill DEFAULT NULL,\
                     //                `lastlogin_time` datetime DEFAULT NULL COMMENT '最後登入時間',\
                     //                `lastlogout_time` datetime DEFAULT NULL COMMENT '最後登出時間',\
            //                         `note01` char( 255 ) DEFAULT NULL,\
            //                         `note02` char( 255 ) DEFAULT NULL,\
            //                         `note03` char( 255 ) DEFAULT NULL,\
            //                         `note04` char( 255 ) DEFAULT NULL,\
            //                         `note05` char( 255 ) DEFAULT NULL,\
            //                         `note06` char( 255 ) DEFAULT NULL,\
            //                         `note07` char( 255 ) DEFAULT NULL,\
            //                         `note08` char( 255 ) DEFAULT NULL,\
            //                         INDEX `member_uid`(`member_uid`) USING BTREE,\
            //                         CONSTRAINT `%s_ibfk_1` FOREIGN KEY( `member_uid` ) REFERENCES `%s` ( `member_uid` ) ON DELETE CASCADE ON UPDATE CASCADE\
                     //                ) ENGINE = InnoDB DEFAULT CHARSET = utf8" ),
            //    member::GetTableName( member_uid ).c_str(),
            //    member::GetTableName( member_uid ).c_str(), member::GetIndexName().c_str() );
            return query;
        }

        inline _cs_string SetSessionUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* username )
        {
            _cs_string query;
            int escoutlen = _csstrlen( username ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, username, _csstrlen( username ) );
            //_pf_tstrprintf( query, "INSERT INTO csmember_%05d( member_uid, username ) VALUES( %lld, %s ) ON DUPLICATE KEY UPDATE %s", tbidx, member_uid, safestring );
           //_pf_tstrprintf( query, "UPDATE csmember_%05d SET member_uid = '%lld' WHERE username = '%s'", tbidx, member_uid, safestring );
            _pf_tstrprintf( query, "INSERT IGNORE INTO %s( member_uid, username ) VALUES( '%llu', '%s' )", GetTableName( member_uid ).c_str(), member_uid, safestring );
            SAFE_DELETE_ARRAY( safestring );
            return query;
        }

        inline _cs_string SetIndexSessionUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* username )
        {
            _cs_string query;
            int escoutlen = _csstrlen( username ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, username, _csstrlen( username ) );
            //_pf_tstrprintf( query, "INSERT INTO csmember_%05d( member_uid, username ) VALUES( %lld, %s ) ON DUPLICATE KEY UPDATE %s", tbidx, member_uid, safestring );
           //_pf_tstrprintf( query, "UPDATE csmember_%05d SET member_uid = '%lld' WHERE username = '%s'", tbidx, member_uid, safestring );
            _pf_tstrprintf( query, "INSERT IGNORE INTO %s( member_uid, username ) VALUES( '%llu', '%s' )", GetIndexName().c_str(), member_uid, safestring );
            SAFE_DELETE_ARRAY( safestring );
            return query;
        }

        //
        // Set the new password by csmember_%05d id
        //
        inline _cs_string SetPassword( _IN uniqueid_t member_uid, _IN const _cs_char_t* password )
        {
            _cs_string query;
            int escoutlen = _csstrlen( password ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, password, _csstrlen( password ) );
            _pf_tstrprintf( query, "UPDATE %s SET password = '%s' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), safestring, member_uid );
            SAFE_DELETE_ARRAY( safestring );
            return query;
        }

        inline _cs_string SetGroupUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* group_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET group_uid = '%s' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), group_uid, member_uid );
            return query;
        }

        inline _cs_string SetIndexGroupUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* group_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET group_uid = '%s' WHERE member_uid = '%llu'", GetIndexName().c_str(), group_uid, member_uid );
            return query;
        }

        //
        // check the username and password
        //
        inline _cs_string CheckLogin( _IN uniqueid_t member_uid, _IN const _cs_char_t* username, _IN const _cs_char_t* password )
        {
            _cs_string query;
            int escoutlen1 = _csstrlen( username ) * 2 + 1;
            _cs_char_t* safestring1 = new _cs_char_t[ escoutlen1 ];
            strtoescapestr( safestring1, escoutlen1, username, _csstrlen( username ) );
            int escoutlen2 = _csstrlen( password ) * 2 + 1;
            _cs_char_t* safestring2 = new _cs_char_t[ escoutlen2 ];
            strtoescapestr( safestring2, escoutlen2, password, _csstrlen( password ) );
            _pf_tstrprintf( query, "SELECT member_uid FROM %s WHERE username = binary( '%s' ) AND password = binary( '%s' )", GetTableName( member_uid ).c_str(), safestring1, safestring2 );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            SAFE_DELETE_ARRAY( safestring1 );
            SAFE_DELETE_ARRAY( safestring2 );
            return query;
        }

        inline _cs_string GetMemberData( _IN uniqueid_t member_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT username, state, information, group_uid, flag, pri_permissionl, pub_permissionl, pri_permissions, pub_permissions, UNIX_TIMESTAMP(lastlogout_time), UNIX_TIMESTAMP(lastlogin_time), UNIX_TIMESTAMP(NOW()) FROM %s WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            return query;
        }

        inline _cs_string SetMemberPermission( _IN uniqueid_t member_uid,
                                             _IN cs_flag_t flag, _IN cs_group_uid_t group_uid,
                                             _IN cs_permissionll_t pri_ll, _IN cs_permissionll_t pub_ll,
                                             _IN cs_permissionss_t pri_ss, _IN cs_permissionss_t pub_ss )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET flag = '%d', group_uid = '%s', pri_permissionl = '%llu', pub_permissionl = '%llu', pri_permissionl = '%s', pub_permissionl = '%s'\
                            FROM csmember_%05d WHERE member_uid = '%llu'",
                            GetTableName( member_uid ).c_str(),
                            flag, group_uid.c_str(), pri_ll, pub_ll, pri_ss.c_str(), pub_ss.c_str(), member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            return query;
        }

        //inline _cs_string GetSetting( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT csmember_%05d.power, csmember_%05d.group FROM csmember_%05d WHERE member_uid = '%llu'", tbidx, tbidx, member_uid );
        //    //_cs_char_t query[512];
        //    //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
        //    return query;
        //}

        //inline _cs_string GetPower( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT power FROM csmember_%05d WHERE member_uid = '%llu'", tbidx, member_uid );
        //    //_cs_char_t query[512];
        //    //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
        //    return query;
        //}

        inline _cs_string GetSessionUID( _IN uniqueid_t member_uid, _IN const _cs_char_t* name )
        {
            _cs_string query;
            int escoutlen = _csstrlen( name ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, name, _csstrlen( name ) );
            _pf_tstrprintf( query, "SELECT member_uid FROM %s WHERE username = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE( safestring );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            return query;
        }

        //inline _cs_string GetName( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT username FROM csmember_%05d WHERE member_uid = '%lld'", tbidx, member_uid );
        //    //_cs_char_t query[512];
        //    //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
        //    return query;
        //}

        inline _cs_string SetState( _IN uniqueid_t member_uid, _IN int state )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET state = '%d' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), state, member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%lld'", active, member_uid);
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%llu'", active, member_uid);
            return query;
        }

        //inline _cs_string GetState( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    //_pf_tstrprintf( query, "SELECT state FROM csmember_%05d WHERE member_uid = '%lld'", member_uid );
        //    _pf_tstrprintf( query, "SELECT state, UNIX_TIMESTAMP(lastlogout_time), UNIX_TIMESTAMP(lastlogin_time), UNIX_TIMESTAMP(NOW()) FROM csmember_%05d WHERE member_uid = '%lld'", tbidx, member_uid );
        //    return query;
        //}


        //nline _cs_string GetState( uniqueid_t member_uid )
        //
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT state, UNIX_TIMESTAMP(lastlogout_time), UNIX_TIMESTAMP(lastlogin_time), UNIX_TIMESTAMP(NOW()) FROM csmember_%05d WHERE member_uid = '%lld'", member_uid );
        //    return query;
        //


        //inline _cs_string csmember_%05d::SetLogin(uniqueid_t member_uid, const _cs_char_t* mac, ipa_t ip, int state)
        inline _cs_string SetAllowLogin( _IN uniqueid_t member_uid, _IN macn_t mac, _IN int state )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET mac = '%lld', state = '%d', server_uid = '0', verify_sec = UNIX_TIMESTAMP(NOW()), lastlogin_time = NOW() WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), mac, state, member_uid );
            return query;
        }

        inline _cs_string SetLogin( _IN uniqueid_t member_uid, _IN cs_section_uid_t sid, _IN int state )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET server_uid = '%d', state = '%d' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), sid, state, member_uid );
            return query;
        }

        /*
        inline _cs_string SetLogout(uniqueid_t member_uid, int state, longlong_t currtime)
        {
        _cs_string query;
        //query.format("UPDATE csmember_%05d SET state = '%d', LastLogoutSec = '%lld' WHERE member_uid = '%lld'", state, currtime, member_uid);
        query.format("UPDATE csmember_%05d SET state = '%d', server_uid = '0', LastLogoutSec = '%lld' WHERE member_uid = '%lld'", state, currtime, member_uid);
        //_cs_char_t query[512];
        //sprintf(query, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, member_uid);
        //sprintf(query, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, member_uid);
        return query;
        }
        */

        inline _cs_string SetLogout( _IN uniqueid_t member_uid, _IN int state )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET state = '%d', server_uid = '0', lastlogout_time = NOW() WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), state, member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, member_uid);
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, member_uid);
            return query;
        }

        /*
        // save logout use input time
        inline _cs_string csmember_%05d::SetRetentionLogout(uniqueid_t member_uid, int state, longlong_t currtime)
        {
        _cs_string query;
        //query.format("UPDATE csmember_%05d SET state = '%d', LastLogoutSec = '%lld' WHERE member_uid = '%lld'", state, currtime, member_uid);
        query.format("UPDATE csmember_%05d SET state = '%d', server_uid = '0', verify_sec = UNIX_TIMESTAMP(NOW()), LastLogoutSec = '%lld' WHERE member_uid = '%lld'", state, currtime, member_uid);
        //_cs_char_t query[512];
        //sprintf(query, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, member_uid);
        //sprintf(query, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, member_uid);
        return query;
        }
        */

        // save logout use database time
        inline _cs_string SetRetentionLogout( _IN uniqueid_t member_uid, _IN int state )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET state = '%d',server_uid = '0', verify_sec = UNIX_TIMESTAMP(NOW()), lastlogout_time = NOW() WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), state, member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d', LastLogin=NOW(), WHERE member_uid='%d'", active, member_uid);
            //sprintf(query, "UPDATE csmember_%05d SET Actived='%d' WHERE member_uid='%d'", active, member_uid);
            return query;
        }


        //inline _cs_string GetPoint( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT point FROM csmember_%05d WHERE member_uid = '%lld'", tbidx, member_uid );
        //    return query;
        //}


        inline _cs_string SetPoint( _IN uniqueid_t member_uid, _IN longlong_t point )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET acc_point00 = '%lld' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), point, member_uid );
            return query;
        }


        inline _cs_string AddPoint( _IN uniqueid_t member_uid, _IN longlong_t point )
        {
            _cs_string query;
            _pf_tstrprintf( query, "UPDATE %s SET acc_point00 = point + '%lld' WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), point, member_uid );
            return query;
        }


        // 取出檢查用的條件
        inline _cs_string GetCheckQualification( _IN uniqueid_t member_uid, _IN const char* username )
        {
            _cs_string query;
            int escoutlen = _csstrlen( username ) * 2 + 1;
            _cs_char_t* safestring = new _cs_char_t[ escoutlen ];
            strtoescapestr( safestring, escoutlen, username, _csstrlen( username ) );
            _pf_tstrprintf( query, "SELECT state, mac, type, UNIX_TIMESTAMP(NOW())-verify_sec FROM %s WHERE username = '%s'", GetTableName( member_uid ).c_str(), safestring );
            SAFE_DELETE_ARRAY( safestring );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            return query;
        }


        inline _cs_string GetCheckQualification( _IN uniqueid_t member_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT state, mac, type, UNIX_TIMESTAMP(NOW())-verify_sec FROM %s WHERE member_uid = '%llu'", GetTableName( member_uid ).c_str(), member_uid );
            //_cs_char_t query[512];
            //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
            return query;
        }


        //inline _cs_string GetIdentity( _IN int tbidx, _IN uniqueid_t member_uid )
        //{
        //    _cs_string query;
        //    _pf_tstrprintf( query, "SELECT username, power, flag FROM csmember_%05d WHERE member_uid = '%llu'", tbidx, member_uid );
        //    //_cs_char_t query[512];
        //    //sprintf(query, "SELECT member_uid, Actived FROM csmember_%05d WHERE username = '%s' AND password = '%s'", username, password);
        //    return query;
        //}
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Paper server
    namespace paper
    {
        inline _cs_string GetTableName( _IN const _cs_char_t* key_code )
        {
            _IN uniqueid_t key_uid = cyberspace::uid_generator::GetPaperUID( key_code );
            _cs_string tbname;
            //_pf_tstrprintf( tbname, _u8( "cprecord_%08d" ), _GET_EXPOND_TABLE_IDX( key_uid ) );
            _pf_tstrprintf( tbname, _u8( "cprecord_%010d" ), _GET_EXPOND_TABLE_IDX( key_uid ) );
            return tbname;
        }

        inline _cs_string CreateTable( _IN const _cs_char_t* key_code )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( "\
CREATE TABLE IF NOT EXISTS `%s`\n\
(\n\
    `rec_key` varchar( 256 ) NOT NULL PRIMARY KEY COMMENT '資料key值',\n\
    `rec_value` blob NOT NULL COMMENT '資料value值',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期',\n\
    `update_time` datetime DEFAULT NULL COMMENT '資料更新日期',\n\
    `live_sec` int( 11 ) unsigned zerofill DEFAULT NULL\n\
) ENGINE = InnoDB DEFAULT CHARSET = %s COLLATE = %s \n\
PARTITION BY KEY( rec_key )\n\
PARTITIONS %d" ),
GetTableName( key_code ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsExpondDBTable );
            return query;
        }

        inline _cs_string SetRecord( _IN const _cs_char_t* key_code, _IN const _cs_char_t* val, _IN int update_time, _IN int live_sec )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( "\
INSERT  INTO %s (rec_key, rec_value, create_time, update_time, live_sec)\n\
        VALUES ('%s', '%s', FROM_UNIXTIME('%d'), FROM_UNIXTIME('%d'), '%d')\n\
ON DUPLICATE KEY\n\
        UPDATE rec_value = '%s', update_time = FROM_UNIXTIME('%d'), live_sec = '%d'" ),
                            GetTableName( key_code ).c_str(), key_code, val, update_time, update_time, live_sec, val, update_time, live_sec );
            return query;
        }

        inline _cs_string SetnxRecord( _IN const _cs_char_t* key_code, _IN const _cs_char_t* val, _IN int create_time, _IN int live_sec )
        {
            _cs_string query;
            //_pf_tstrprintf( query, "INSERT IGNORE INTO cprecord%05d (rec_key, rec_value, create_time, update_time, live_sec) VALUES ('%s', '%s', FROM_UNIXTIME('%d'), FROM_UNIXTIME('%d'), '%d')", tbidx, key, val, create_time, create_time, live_sec );
            _pf_tstrprintf( query, _u8( "\
INSERT INTO %s (rec_key, rec_value, create_time, update_time, live_sec)\n\
        VALUES ('%s', '%s', FROM_UNIXTIME('%d'), FROM_UNIXTIME('%d'), '%d')\n\
ON DUPLICATE KEY\n\
        UPDATE rec_key = rec_key" ),
                            GetTableName( key_code ).c_str(), key_code, val, create_time, create_time, live_sec );
            return query;
        }

        inline _cs_string GetRecord( _IN const _cs_char_t* key_code )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT rec_key, rec_value, UNIX_TIMESTAMP(create_time), UNIX_TIMESTAMP(update_time), live_sec FROM %s WHERE rec_key = '%s'", GetTableName( key_code ).c_str(), key_code );
            return query;
        }

        inline _cs_string DelRecord( _IN const _cs_char_t* key_code )
        {
            _cs_string query;
            _pf_tstrprintf( query, "DELETE FROM %s WHERE rec_key = '%s'", GetTableName( key_code ).c_str(), key_code );
            return query;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // cslog_%04d%02d_%05d
    namespace log
    {
        inline _cs_string GetTableName( _IN int classify, _IN uniqueid_t member_uid, _IN pftml::Time tbday )
        {
            _cs_string tbname;
            //_pf_tstrprintf( tbname, _u8( "cslog%08d_%010d_%04d%02d" ), classify % 10000, _GET_DATALOG_TABLE_IDX( member_uid ), tbday.GetYear(), tbday.GetMonth() );
            _pf_tstrprintf( tbname, _u8( "cslog%08d_%04d%02d_%010d" ), classify % 10000, tbday.GetYear(), tbday.GetMonth(), _GET_DATALOG_TABLE_IDX( member_uid ) );
            return tbname;
        }

        // data log data
        inline _cs_string CreateTable( _IN int classify, _IN uniqueid_t member_uid, _IN pftml::Time tbday )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( "\
CREATE TABLE IF NOT EXISTS `%s`\n\
(\n\
    `log_uid` bigint( 20 ) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '紀錄編號',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期 (實際建立在資料庫的時間)',\n\
    `insert_time` datetime DEFAULT current_timestamp() COMMENT '資料寫入日期 (要求寫入在資料庫的時間)',\n\
    `member_uid` bigint( 20 ) UNSIGNED NOT NULL COMMENT '帳號編號',\n\
    `level` bigint( 11 ) DEFAULT NULL COMMENT '紀錄重要性等級',\n\
    `event` char( 255 ) DEFAULT NULL COMMENT '紀錄事件(自行定義)',\n\
    `data` char( 255 ) DEFAULT NULL COMMENT '紀錄資料(自行定義)',\n\
    `content` varchar( 2048 ) DEFAULT NULL COMMENT '紀錄內容(自行定義)',\n\
    PRIMARY KEY( `log_uid`, `member_uid` )\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH(member_uid)\n\
PARTITIONS %d" ),
GetTableName( classify, member_uid, tbday ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsDatalogDBTable );
            return query;
        }

        inline _cs_string SetRecord( _IN int classify, _IN pftml::Time tbday, _IN uniqueid_t member_uid, _IN int level, _IN const _cs_char_t* event, _IN const _cs_char_t* data, _IN const _cs_char_t* content )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( "INSERT INTO %s( insert_time, member_uid, level, event, data, content) VALUE( '%s', '%llu', '%d', '%s', '%s', '%s' )" ),
                            GetTableName( classify, member_uid, tbday ).c_str(), tbday.GetDateTimeTCS().c_str(), level, member_uid, event, data, content );
            return query;
        }

        inline _cs_string GetCount( _IN int classify, _IN pftml::Time tbday, _IN uniqueid_t member_uid, _IN const _cs_char_t* begin_date, _IN const _cs_char_t* end_date )
        {
            _cs_string querystring;
            _pf_tstrprintf( querystring, ( "SELECT COUNT( log_uid ) FROM %s WHERE member_uid = '%llu' AND insert_time BETWEEN '%s' AND '%s'" ),
                            GetTableName( classify, member_uid, tbday ).c_str(), member_uid, begin_date, end_date );
            return querystring;
        }

        // 
        inline _cs_string GetRecord( _IN int classify, _IN pftml::Time tbday, _IN uniqueid_t member_uid, _IN const _cs_char_t* begin_date, _IN const _cs_char_t* end_date, _IN int limit_offset, _IN int limit_rowcount )
        {
            _cs_string querystring;
            //for ( int i = 0; i < tables.size(); i++ )
            {
                _pf_tstrprintf( querystring, ( "SELECT log_uid, create_time, insert_time, member_uid, level, event, data, content FROM %s WHERE member_uid = '%llu' AND insert_time BETWEEN '%s' AND '%s' ORDER by log_uid ASC LIMIT %d, %d" ),
                                GetTableName( classify, member_uid, tbday ).c_str(),
                                member_uid, begin_date, end_date, limit_offset, limit_rowcount );
            }
            //querystring.erase( querystring.end() - _tcslen( ( " UNION ALL " ) ), querystring.end() );
            return querystring;
        }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // cslog_%04d%02d_%05d
    namespace bulletin
    {
        // 不同用途的公告 也放在不同表中
        inline _cs_string GetTableName( _IN int classify, _IN uniqueid_t bulletin_gid, _IN pftml::Time tbday )
        {
            _cs_string tbname;
            _pf_tstrprintf( tbname, _u8( "csbln%08d_%04d%02d_%010d" ), classify % 10000, tbday.GetYear(), tbday.GetMonth(), _GET_BULLETIN_TABLE_IDX( bulletin_gid ) );
            return tbname;
        }

        // 取出多張表  eg csbln00005_000016  所有的日期
        inline _cs_string GetTableName( _IN int classify, _IN uniqueid_t bulletin_gid )
        {
            _cs_string tbname;
            _pf_tstrprintf( tbname, _u8( "csbln%08d_%%_%010d" ), classify, _GET_BULLETIN_TABLE_IDX( bulletin_gid ) );
            return tbname;
        }

        inline _cs_string CreateTable( _IN int classify, _IN uniqueid_t bulletin_gid, _IN pftml::Time tbday )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8( "\
CREATE TABLE IF NOT EXISTS `%s`\n\
(\n\
    `bulletin_uid` bigint( 20 ) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '紀錄編號',\n\
    `bulletin_gid` bigint( 20 ) UNSIGNED DEFAULT '0' COMMENT '留言群組編號 (切分表格的依據 0 第一筆發言)',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期 (實際建立在資料庫的時間)',\n\
    `insert_time` datetime DEFAULT current_timestamp() COMMENT '資料建立日期 (實際建立在資料庫的時間)',\n\
    `update_time` datetime DEFAULT current_timestamp() COMMENT '資料更新日期 (要求更新資料庫的時間)',\n\
    `update_count` int( 11 ) DEFAULT '0' COMMENT '資料更新次數 (資料更新的次數)',\n\
    `record_state` varchar( 32 ) DEFAULT '1' COMMENT '0:隱藏 1:開啟',\n\
    `classify` int( 11 ) DEFAULT '0' COMMENT '類別編號',\n\
    `member_uid` bigint( 20 ) UNSIGNED NOT NULL COMMENT '帳號編號 (發表的人)',\n\
    `reply_bulletin_uid` bigint( 20 ) DEFAULT '0' COMMENT '回覆留言的編號 (bulletin_uid) 0:主留言',\n\
    `title` varchar( 255 ) DEFAULT NULL COMMENT '留言標題(自行定義)',\n\
    `content` varchar( 2048 ) DEFAULT NULL COMMENT '留言內容(自行定義)',\n\
    `step`   varchar( 32 ) DEFAULT NULL COMMENT '目前的處理狀態(自行定義)',\n\
    `note01` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note02` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note03` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note04` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note05` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note06` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note07` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    `note08` varchar( 255 ) DEFAULT NULL COMMENT '註解(自行定義)',\n\
    PRIMARY KEY( `bulletin_uid`, `bulletin_gid` )\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( bulletin_gid )\n\
PARTITIONS %d" ),
GetTableName( classify, bulletin_gid, tbday ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CsBulletinDBTable );
            return query;
        }

        inline _cs_string SetRecord( _IN int classify, _IN pftml::Time tbday, _IN uniqueid_t member_uid, _IN uniqueid_t bulletin_gid, _IN uint64_t reply_bulletin_uid, _IN const _cs_char_t* title, _IN const _cs_char_t* content )
        {
            _cs_string query;
            _pf_tstrprintf( query, _u8("\
INSERT  INTO %s (record_state, classify, bulletin_gid, insert_time, member_uid, reply_bulletin_uid, title, content) \n\
        VALUES ('1', '%d', '%llu', '%s', '%llu', '%llu', '%s', '%s') \n\
ON DUPLICATE KEY \n\
        UPDATE update_time = NOW(), update_count = update_count + 1, \n\
               bulletin_gid = '%llu', member_uid = '%llu', reply_bulletin_uid = '%llu', title = '%s', content = '%s'" ),
                GetTableName( classify, bulletin_gid, tbday ).c_str(), classify, bulletin_gid, tbday.GetDateTimeTCS().c_str(), member_uid, reply_bulletin_uid, title, content,
                bulletin_gid, member_uid, reply_bulletin_uid, title, content );
            return query;
        }

        inline _cs_string DelRecord( _IN const _cs_char_t* tbname, _IN uint64_t bulletin_uid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "DELETE FROM %s WHERE bulletin_uid = '%llu'", tbname, bulletin_uid );
            return query;
        }

        inline _cs_string DelGroup( _IN const _cs_char_t* tbname, _IN uniqueid_t bulletin_gid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "DELETE FROM %s WHERE bulletin_gid = '%llu'", bulletin_gid );
            return query;
        }

        // 取出一個留言裡所有的回覆
        inline _cs_string GetCount( _IN const _cs_char_t* tbname, _IN uniqueid_t bulletin_gid )
        {
            _cs_string querystring;
            _pf_tstrprintf( querystring, ( "SELECT COUNT( bulletin_uid ) FROM %s WHERE bulletin_gid = '%llu'" ), tbname, bulletin_gid );
            return querystring;
        }

        inline _cs_string GetRecord( _IN const _cs_char_t* tbname, _IN uniqueid_t bulletin_gid, _IN int limit_offset, _IN int limit_rowcount )
        {
            _cs_string querystring;
            //for ( int i = 0; i < tables.size(); i++ )
            {
                _pf_tstrprintf( querystring, ( "SELECT bulletin_uid, bulletin_gid, create_time, insert_time, update_time, update_count, member_uid, reply_bulletin_uid, title, content FROM %s WHERE bulletin_gid = '%llu' ORDER by create_time ASC LIMIT %d, %d" ),
                                tbname, bulletin_gid, limit_offset, limit_rowcount );
            }
            //querystring.erase( querystring.end() - _tcslen( ( " UNION ALL " ) ), querystring.end() );
            return querystring;
        }

        //時間範圍內只取出重複gid的第一筆
        inline _cs_string GetCount( _IN int classify, _IN uniqueid_t bulletin_gid, _IN uniqueid_t member_uid, _IN pftml::Time tbday, _IN const _cs_char_t* begin_date, _IN const _cs_char_t* end_date )
        {
            _cs_string querystring;
            _pf_tstrprintf( querystring, ( "SELECT COUNT( bulletin_uid ) FROM %s WHERE classify = '%d' AND create_time BETWEEN '%s' AND '%s' GROUP BY bulletin_gid" ),
                            GetTableName( classify, bulletin_gid, tbday ).c_str(), classify, begin_date, end_date );
            return querystring;
        }

        inline _cs_string GetRecord( _IN int classify, _IN uniqueid_t bulletin_gid, _IN uniqueid_t member_uid, _IN pftml::Time tbday, _IN const _cs_char_t* begin_date, _IN const _cs_char_t* end_date, _IN int limit_offset, _IN int limit_rowcount )
        {
            _cs_string querystring;
            _pf_tstrprintf( querystring, ( "SELECT bulletin_uid, bulletin_gid, create_time, create_time, update_time, update_count, member_uid, reply_bulletin_uid, title, content FROM %s WHERE classify = '%d' AND create_time BETWEEN '%s' AND '%s' GROUP BY bulletin_gid ASC LIMIT %d, %d" ),
                            GetTableName( classify, bulletin_gid, tbday ).c_str(),
                            classify,
                            begin_date, end_date, limit_offset, limit_rowcount );
            return querystring;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // cshistory_%05d table
    /*
    namespace cshistory
    {
        inline _cs_string GetHistory( _IN int tbidx, _IN uniqueid_t hid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "SELECT duration_sec, frequency_count FROM cshistory_%05d WHERE history_uid = '%llu'", tbidx, hid );
            return query;
        }

        inline _cs_string BeginHistory( _IN int tbidx, _IN uniqueid_t member_uid, _IN ipa_t ip, _IN macn_t macn, _IN cs_section_uid_t sectionid )
        {
            _cs_string query;
            _pf_tstrprintf( query, "INSERT INTO cshistory_%05d(member_uid, from_ip, from_mac, server_uid, online_time, duration_sec, frequency_count) VALUES('%llu', '%s', '%lld', '%d', NOW(), '0', '0')", tbidx, member_uid, ip.c_str(), macn, sectionid );
            return query;
        }

        //inline _cs_string EndHistory( _IN int tbidx, _IN uniqueid_t hid, stime_t duration_sec )
        inline _cs_string EndHistory( _IN int tbidx, _IN uniqueid_t hid )
        {
            _cs_string query;
            //query.format("UPDATE cshistory_%05d SET LastLogout = '%s', duration_sec = '%d' WHERE member_uid = '%llu'", asctime(localtime(&timep)), duration_sec, member_uid);
            _pf_tstrprintf( query, "UPDATE cshistory_%05d SET offline_time = NOW(), duration_sec = TIMESTAMPDIFF(SECOND, online_time, NOW()) WHERE history_uid = '%llu'", tbidx, hid );
            return query;
        }
    }
    */

}

//#define DB_COMMAND_CHECK_LOGIN(u,p)     "SELECT member_uid FROM csmember_%05d WHERE username = '"u"' AND password = '"p"'"
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)       "select member_uid from csmember_%05d where username = '"usr"' and password = '"pwd"'"