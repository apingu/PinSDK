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
//  �Ȯɩ�o�H��n��C��server �b�h
//
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C �t�Τ��/C++ �t�Τ��
#include <vector>

//02.== ��L PinSDK �禡�w��.h ��� ���ê���᭱
#include <pf-cstring.h>
#include <pf-stdlib.h>
#include <pf-string.h>
#include <pf-debug.h>
//

//03.== ��L �ϥ� �禡�w��.h ���

//04.== ���M�פ�.h ���
#include "cs-db_query_def.h"



namespace cs_db_query
{

    // partation
    const static int kPARTATION_CMGoods_DBTable       = 1000;
    const static int kPARTATION_CMGuild_DBTable       = 10;   // ����
    const static int kPARTATION_CMHouse_DBTable       = 1000;
    const static int kPARTATION_CMItem_DBTable        = 1000;
    const static int kPARTATION_CMRule_DBTable        = 10;   // ����
    const static int kPARTATION_CMTransaction_DBTable = 100;

    //////////////////////////////////////////////////////////////////////////
    //
    namespace init
    {
        ///////////////////////////////////////////////////////////////////////////////
        // �ӫ~
        namespace goods
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmgoods_%010d" ), _GET_EXPOND_TABLE_IDX( tbidex ) );
                return tbname;
            }

            // �ө��ӫ~
            //inline _cs_string IfTableExist_cmgoods( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, _u8( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = '%s'" ), GetTableName( tbidx ).c_str() );
            //    return out;
            //}

            inline _cs_string CreateTable_cmgoods( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
        `goods_uid` bigint( 20 ) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '�ӫ~�s��',\n\
        `store_uid` int( 11 ) NOT NULL COMMENT '�ө��s��',\n\
        `item_pid` int( 11 ) DEFAULT NULL,\n\
        `pocket` int( 11 ) DEFAULT NULL,\n\
        `durability` int( 11 ) DEFAULT '256',\n\
        `weight` int( 11 ) DEFAULT '256',\n\
        `limitation` int( 11 ) DEFAULT '256',\n\
        `buy_price` decimal( 20, 4 ) DEFAULT '0',\n\
        `sell_price` decimal( 20, 4 ) DEFAULT '0',\n\
        `state` int( 11 ) DEFAULT '1',\n\
        `buy_count` int( 11 ) DEFAULT '0',\n\
        `sell_count` int( 11 ) DEFAULT '0',\n\
        `item_count` int( 11 ) DEFAULT '0',\n\
        PRIMARY KEY( `goods_uid`, `store_uid` )\n\
) ENGINE = InnoDB DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH(store_uid)\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMGoods_DBTable );
                return out;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // ���|
        namespace guild
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmguild_0000000000" ) );
                return tbname;
            }

            //inline _cs_string IfTableExist_cmguild( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, _u8( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'cmguild_%05d'" ), tbidx );
            //    return out;
            //}

            inline _cs_string CreateTable_cmguild( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
    `guild_uid` bigint( 20 ) PRIMARY KEY NOT NULL DEFAULT '0' COMMENT '���|�s��'\n\
    `guild_name` char( 255 ) DEFAULT NULL COMMENT '���|�W��',\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH(guild_uid)\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMGuild_DBTable );
                return out;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        //
        namespace house
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmhouse_%010d" ), _GET_EXPOND_TABLE_IDX( tbidex ) );
                return tbname;
            }

            //inline _cs_string IfTableExist_cmhouse( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, _u8( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'cmhouse_%05d'" ), tbidx );
            //    return out;
            //}

            inline _cs_string CreateTable_cmhouse( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
    `house_uid` bigint( 20 ) UNSIGNED NOT NULL AUTO_INCREMENT,\n\
    `table_uid` int( 11 ) DEFAULT NULL,\n\
    `owner_uid` bigint( 20 ) UNSIGNED,\n\
    `owner_name` char( 255 ) DEFAULT NULL,\n\
    `introduce` char( 255 ) DEFAULT NULL,\n\
    `location` bigint( 20 ) DEFAULT NULL,\n\
    `gender` char( 255 ) DEFAULT NULL,\n\
    `pop` int( 11 ) DEFAULT NULL,\n\
    `visit_count` int( 11 ) DEFAULT NULL,\n\
    `guest_num` int( 11 ) DEFAULT NULL,\n\
    `islock` int( 11 ) DEFAULT NULL,\n\
    `issingle` int( 11 ) DEFAULT NULL,\n\
    `guest_limit` int( 11 ) DEFAULT NULL,\n\
    PRIMARY KEY( `house_uid`, `owner_uid` )\n\
) ENGINE = InnoDB AUTO_INCREMENT = 8 DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( house_uid )\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMHouse_DBTable );
                return out;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // �D��
        namespace item
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmitem_%010d" ), _GET_EXPOND_TABLE_IDX( tbidex ) );
                return tbname;
            }

            //inline _cs_string IfTableExist_cmitem( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, _u8( "SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'cmitem_%05d'" ), tbidx );
            //    return out;
            //}

            inline _cs_string CreateTable_cmitem( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
   `item_uid` bigint(20) UNSIGNED AUTO_INCREMENT,\n\
   `owner_uid` bigint( 20 ) UNSIGNED,\n\
   `table_uid` bigint( 20 ) NOT NULL DEFAULT '0',\n\
   `local_uid` bigint( 20 ) DEFAULT NULL,\n\
   `pocket` int( 11 ) NOT NULL DEFAULT '0' COMMENT '�Ҧb���I�]',\n\
   `durability` int( 11 ) DEFAULT NULL,\n\
   `weight` int( 11 ) DEFAULT NULL COMMENT '���q',\n\
   `limitation` int( 11 ) DEFAULT NULL COMMENT '�ɮ�',\n\
   `present_uid` bigint( 20 ) DEFAULT NULL,\n\
   `user_uid` bigint( 20 ) DEFAULT '0' COMMENT '�ϥΤ����H���s��',\n\
   `property` blob NOT NULL,\n\
      PRIMARY KEY( `item_uid`, `owner_uid` )\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( owner_uid )\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMItem_DBTable );
                return out;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        //
        namespace rule
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmrule_0000000000" ) );
                return tbname;
            }

            //inline _cs_string IfTableExist_cmrule( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, " SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'cmrule_%05d' ", tbidx );
            //    return out;
            //}

            inline _cs_string CreateTable_cmrule( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` (\n\
   `rule_uid` int( 11 ) NOT NULL PRIMARY KEY AUTO_INCREMENT,\n\
   `rule_addtime` int( 11 ) NOT NULL,\n\
   `rule_title` varchar( 255 ) DEFAULT NULL,\n\
   `rule_main` varchar( 1023 ) DEFAULT NULL\n\
) ENGINE = MyISAM DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( rule_uid )\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMRule_DBTable );
                return out;
            }
        }

        ///////////////////////////////////////////////////////////////////////////////
        // ���
        namespace transaction
        {
            inline _cs_string GetTableName( _IN uniqueid_t tbidx )
            {
                _cs_string tbname;
                //_pf_tstrprintf( tbname, _u8( "cmitem_%05d" ), _GET_MEMBER_TABLE_IDX( member_uid ) );
                _pf_tstrprintf( tbname, _u8( "cmtransactio_%010d" ), _GET_EXPOND_TABLE_IDX( tbidex ) );
                return tbname;
            }

            //inline _cs_string IfTableExist_cmtransaction( _IN int tbidx )
            //{
            //    _cs_string out;
            //    pfstd::strprintf( out, " SELECT COUNT(*) FROM information_schema.TABLES WHERE TABLE_NAME = 'cmtransaction_%05d' ", tbidx );
            //    return out;
            //}

            inline _cs_string CreateTable_cmtransaction( _IN int tbidx )
            {
                _cs_string out;
                pfstd::strprintf( out, _u8( "\
CREATE TABLE IF NOT EXISTS `%s` \n\
(\n\
    `transaction_uid` bigint( 20 ) UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,\n\
    `transaction_iid` bigint( 20 ) NOT NULL COMMENT '�y���s��',\n\
    `create_time` datetime DEFAULT current_timestamp() COMMENT '��ƫإߤ��',\n\
    `sidea_uid` bigint( 20 ) NOT NULL,\n\
    `sidea_money` decimal( 20, 4 ) NOT NULL DEFAULT '0',\n\
    `sidea_itemarray` blob,\n\
    `sideb_uid` bigint( 20 ) NOT NULL,\n\
    `sideb_money` decimal( 20, 4 ) NOT NULL DEFAULT '0',\n\
    `sideb_itemarray` blob\n\
) ENGINE = InnoDB DEFAULT CHARSET = %s COLLATE = %s\n\
PARTITION BY HASH( transaction_uid )\n\
PARTITIONS %d" ),
GetTableName( tbidx ).c_str(), _DEFINE_CHARACTER_SET, _DEFINE_COLLATE, kPARTATION_CMTransaction_DBTable );
                return out;
            }
        }
    }
}