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
#include "cs-db_query_game.h"
#include "cs-db_function.h"
#include "cs-uid_generator.h"


namespace cyberspace
{

    //static  T_Lock  g_lQuery;
    //static  int count = 0;
    //=====================================================================================
    namespace db_player
    {
        // 如果不存在建立一個player table
        inline void CreateTable( _IN XDatabase* mydb, _IN cs_session_uid_t cid )
        {
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::player::CreateTable( cid ).c_str() );
            MYSQLDB->FreeResult( result );
            return;
        }
    }
};