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
#include <string.h>
#include <stdarg.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <algorithmic/crypt-crc32.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xdb-uid_catalog.h"


namespace xdb
{
    namespace
    {
        _mstring xdb_table_name = _T( "xuid" );

        void CreateXUIDTable( XDatabase* pXDB, const char* dbtbname )
        {
            _mstring querystringa;
            pfstd::strprintf( querystringa,
                ( " CREATE TABLE IF NOT EXISTS `%s`  (\
                              `record_uid` bigint( 20 ) NOT NULL COMMENT '紀錄編號',\
                              `create_time` datetime( 0 ) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '資料建立日期',\
                              `update_time` datetime( 0 ) NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '資料更新日期',\
                              `update_count` int( 11 ) NOT NULL DEFAULT 0 COMMENT '資料更新次數',\
                              `record_value` varchar( 255 ) COMMENT '欄位索引值',\
                              `record_table` varchar( 255 ) COMMENT '表格名稱',\
                              PRIMARY KEY( `record_uid`, `record_value` ) )\
                     PARTITION BY KEY(record_value)\
                     PARTITIONS %d" ),
                dbtbname, 1000 );
            XDBResult ret = pXDB->ExecQueryT( querystringa.c_str() );
            pXDB->FreeResult( ret );
        }

        //========================================================================
        _tstring GetDBTableName( uint64_t unique_id, int tb_category, _mstring table_name )
        {
            _mstring dbtbname;
            pfstd::strprintf( dbtbname, ( "%s%05d_0000000000" ), xdb_table_name.c_str(), tb_category );
            return dbtbname;
        }
    }
    //========================================================================
    //
    //
    //========================================================================
    UIDCatalog::UIDCatalog( XDatabase* pXDB, int category )
    {
        p_xdb_ = pXDB;
        //tb_num_ = 1000;
        tb_category_ = category;
    }

    //========================================================================
    //
    //
    //========================================================================
    UIDCatalog::~UIDCatalog()
    {

    }

    //========================================================================
    //
    //
    //========================================================================
    //void UIDCatalog::Create()
    //{
    //    if( p_xdb_ == NULL )
    //        return;
    //
    //    _mstring querystringa;
    //    // 先取出db裡所有的 talbe 名稱
    //    {
    //        size64_t count = 0;
    //        // 先檢查該table 存不存在
    //        //bool bexist = false;
    //        pfstd::strprintf( querystringa, ( "SELECT COUNT( TABLE_NAME ) FROM information_schema.TABLES WHERE TABLE_NAME LIKE '%%%s%%' AND TABLE_SCHEMA = '%s'" ), xdb_table_name.c_str(), p_xdb_->GetDBName().c_str() );
    //        XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
    //        count = pfstd::strtoi( p_xdb_->GetQueryResultM( ret, 0, 0 ) );
    //        p_xdb_->FreeResult( ret );
    //        if( count != 0 )
    //        {
    //            //  已經建立過了，所以不用再建
    //            tb_num_ = count;
    //            return;
    //        }
    //    }
    //
    //    for( int i = 0; i < tb_num_; i++ )
    //    {
    //        _mstring dbtbname;
    //        pfstd::strprintf( dbtbname, ( "%s_%s_%06d" ), xdb_table_name.c_str(), tb_name_.c_str(), i );
    //
    //        pfstd::strprintf( querystringa,
    //            ( " CREATE TABLE IF NOT EXISTS `%s`  (\
    //                          `unique_id` bigint( 20 ) NOT NULL COMMENT '紀錄編號',\
    //                          `create_time` datetime( 0 ) NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '資料建立日期',\
    //                          `update_time` datetime( 0 ) COMMENT '資料更新日期',\
    //                          `update_count` int( 11 ) NOT NULL DEFAULT 0 COMMENT '資料更新次數',\
    //                          `table_name` varchar( 255 ) COMMENT '表格名稱',\
    //                          PRIMARY KEY( `unique_id` ) )" ),
    //            dbtbname.c_str() );
    //        XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
    //        p_xdb_->FreeResult( ret );
    //    }
    //}

    //========================================================================
    //
    //
    //========================================================================
    void UIDCatalog::SetUniqueId( uint64_t unique_id, _mstring table_name )
    {
        _mstring querystringa;

        _mstring dbtbname = GetDBTableName( unique_id % tb_num_, tb_category_, xdb_table_name.c_str() );
        pfstd::strprintf( dbtbname, ( "%s%05d_0000000000" ), xdb_table_name.c_str(), tb_category_ );
        //pfstd::strprintf( dbtbname, ( "%s%05d_%06d" ), xdb_table_name.c_str(), tb_category_, unique_id % tb_num_ );

        // 用之前在建立就好
        {
            CreateXUIDTable( p_xdb_, dbtbname.c_str() );
        }

        pfstd::strprintf( querystringa, ( "INSERT INTO %s( unique_id, update_time, update_count, table_name  ) \
                                                   VALUES ( '%llu', NOW(), '0', '%s' )\
                                            ON DUPLICATE KEY\
                                                   UPDATE  update_time = NOW(), update_count = update_count + 1, table_name = '%s' " ),
            dbtbname.c_str(), unique_id, table_name.c_str(), table_name.c_str() );

        XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
        p_xdb_->FreeResult( ret );
    }

    //========================================================================
    //
    //
    //========================================================================
    _mstring UIDCatalog::GetUniqueIdTable( uint64_t unique_id )
    {
        _mstring querystringa;

        _mstring dbtbname = GetDBTableName( unique_id % tb_num_, tb_category_, xdb_table_name.c_str() );
        //pfstd::strprintf( dbtbname, ( "%s%05d_%06d" ), xdb_table_name.c_str(), tb_category_, unique_id % tb_num_ );

        pfstd::strprintf( querystringa, ( "SELECT table_name FROM %s WHERE unique_id = '%llu' " ), dbtbname.c_str(), unique_id );
        XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
        _mstring table_name;
        if( p_xdb_->GetQuerySuccessful( ret ) )
        {
            table_name = p_xdb_->GetQueryResultM( ret, 0, 0 );
        }        
        p_xdb_->FreeResult( ret );
        return table_name;
    }

    //========================================================================
    //
    //
    //========================================================================
    bool UIDCatalog::DelUniqueId( uint64_t unique_id )
    {
        _mstring querystringa;

        _mstring dbtbname = GetDBTableName( unique_id % tb_num_, tb_category_, xdb_table_name.c_str() );
        //pfstd::strprintf( dbtbname, ( "%s%05d_%06d" ), xdb_table_name.c_str(), tb_category_, unique_id % tb_num_ );

        pfstd::strprintf( querystringa, ( "DELETE FROM %s WHERE unique_id = '%llu' "), dbtbname.c_str(), unique_id );
        XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
        bool ret_val = p_xdb_->GetQuerySuccessful( ret );
        p_xdb_->FreeResult( ret );
        return ret_val;
    }

    //========================================================================
    //
    //
    //========================================================================
    bool UIDCatalog::MoveRecord( XDatabase* xdb, _mstring id_field_name, uint64_t unique_id,  _mstring new_table_name )
    {
        _mstring querystringa;

        _mstring dbtbname = GetDBTableName( unique_id % tb_num_, tb_category_, xdb_table_name.c_str() );
        //pfstd::strprintf( dbtbname, ( "%s%05d_%06d" ), xdb_table_name.c_str(), tb_category_, unique_id % tb_num_ );

        //  先鎖住存放ID的table
        {
            pfstd::strprintf( querystringa, ( "LOCK TABLES %s LOW_PRIORITY WRITE" ),  dbtbname.c_str() );
            XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
            if( !p_xdb_->GetQuerySuccessful( ret ) )
            {
                // table 不存在 還沒建立過所以代表沒有設定過id
                p_xdb_->FreeResult( ret );
                return false;
            }
            p_xdb_->FreeResult( ret );
        }

        _mstring old_table_name;
        {
            pfstd::strprintf( querystringa, ( "SELECT table_name FROM %s WHERE unique_id = '%llu' " ), dbtbname.c_str(), unique_id  );
            XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
            if( !p_xdb_->GetQuerySuccessful( ret ) )
            {
                // table 不存在 還沒建立過所以代表沒有設定過id 不過上面檢查過 應該不會在發生一次?
                p_xdb_->FreeResult( ret );
                return false;
            }
            old_table_name = p_xdb_->GetQueryResultM( ret, 0, 0 );
            p_xdb_->FreeResult( ret );
        }

        if( old_table_name != new_table_name )
        {
            XDBResult ret = NULL;

            // old new 因為是使用者自己定義的table 所以 必須保證存在
            pfstd::strprintf( querystringa, ( "LOCK TABLES %s, %s LOW_PRIORITY WRITE" ), old_table_name.c_str(), new_table_name.c_str() );
             ret = xdb->ExecQueryT( querystringa.c_str() );
             xdb->FreeResult( ret );

            pfstd::strprintf( querystringa, ( "REPLACE INTO %s SELECT * FROM %s WHERE %s = '%llu' " ), new_table_name.c_str(), old_table_name.c_str(), id_field_name.c_str(), unique_id );
            ret = xdb->ExecQueryT( querystringa.c_str() );
            xdb->FreeResult( ret );

            pfstd::strprintf( querystringa, ( "DELETE FROM %s WHERE %s = '%llu' " ), old_table_name.c_str(), id_field_name.c_str(), unique_id );
            ret = xdb->ExecQueryT( querystringa.c_str() );
            xdb->FreeResult( ret );

            // 更新紀錄
            pfstd::strprintf( querystringa, ( "INSERT INTO %s( unique_id, update_time, update_count, table_name  ) \
                                                   VALUES ( '%llu', NOW(), '0', '%s' )\
                                                ON DUPLICATE KEY\
                                                   UPDATE update_time = NOW(), update_count = update_count + 1, table_name = '%s' " ),
                dbtbname.c_str(), unique_id, new_table_name.c_str(), new_table_name.c_str() );

            ret = p_xdb_->ExecQueryT( querystringa.c_str() );
            p_xdb_->FreeResult( ret );

            pfstd::strprintf( querystringa, ( "UNLOCK TABLES" ) );
            ret = xdb->ExecQueryT( querystringa.c_str() );
            xdb->FreeResult( ret );
        }

        {
            pfstd::strprintf( querystringa, ( "UNLOCK TABLES" ) );
            XDBResult ret = p_xdb_->ExecQueryT( querystringa.c_str() );
            p_xdb_->FreeResult( ret );
        }
        return true;
    }
}





