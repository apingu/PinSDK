//
//
//
//
//  會呼叫到的資料庫存取函式
//  若要改資料庫 使用這個
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
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
#include <pf-time.h>
#include <database/xdatabase.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-err.h"
#include "cs-db_query_game_model.h"
#include "cs-db_function.h"
#include "cs-uid_generator.h"


namespace cyberspace
{
    //static  T_Lock  g_lQuery;
    //static  int count = 0;
    //=====================================================================================
    namespace utility
    {
        // 商品 房子 公會 道具 交易 ....擴充模組
        inline void dbCreateCmTable( _IN XDatabase* mydb )
        {
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cmgoods( 0 ).c_str() );
            if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
            {
                XDBResult result00;
                for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
                {
                    result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cmgoods( ti ).c_str() );
                    MYSQLDB->FreeResult( result00 );
                }
            }
            MYSQLDB->FreeResult( result );

            result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cmguild( 0 ).c_str() );
            if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
            {
                XDBResult result00;
                for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
                {
                    result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cmguild( ti ).c_str() );
                    MYSQLDB->FreeResult( result00 );
                }
            }
            MYSQLDB->FreeResult( result );

            result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cmhouse( 0 ).c_str() );
            if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
            {
                XDBResult result00;
                for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
                {
                    result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cmhouse( ti ).c_str() );
                    MYSQLDB->FreeResult( result00 );
                }
            }
            MYSQLDB->FreeResult( result );

            result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cmitem( 0 ).c_str() );
            if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
            {
                XDBResult result00;
                for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
                {
                    result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cmitem( ti ).c_str() );
                    MYSQLDB->FreeResult( result00 );
                }
            }
            MYSQLDB->FreeResult( result );

            result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cmtransaction( 0 ).c_str() );
            if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
            {
                XDBResult result00;
                for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
                {
                    result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cmtransaction( ti ).c_str() );
                    MYSQLDB->FreeResult( result00 );
                }
            }
            MYSQLDB->FreeResult( result );

            return;
        }
    }
};