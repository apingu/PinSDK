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
// 
// log      紀錄 因為 有paration 所以 只照時間 再分表  (分散式 DB 就都放同一台中 不分散到多台)
// bulletin 公告 因為 有paration 所以 只照時間 再分表  (分散式 DB 就都放同一台中 不分散到多台)  目前因為 系統使用中 未完成之後 式著 bulletin_gid 都合成一張
namespace cs_db_query
{
    // table partation
    //const static int kMOD_CsCommonDBTable   = 100;
    //const static int kMOD_CsMemberDBTable   = 1000; // 
    //const static int kMOD_CsExpondDBTable   = 100;  // paper model
    //const static int kMOD_CsPlayerDBTable   = 1000;
    //const static int kMOD_CsDatalogDBTable  = 100;  // csdatelog_%05d_yyyymm
    const static int kMOD_CsBulletinDBTable = 100;


    // 有幾台DB的實體機器
    const static int kMOD_CsDatabaseMachineSize = 1;


//#define _GET_COMMON_TABLE_IDX( s )        ( abs( ( int )( s % kMOD_CsCommonDBTable ) ) )
//分散表單
#define _GET_MEMBER_TABLE_IDX( s )          ( abs( ( int )( s % kMOD_CsDatabaseMachineSize ) ) ) // member_uid 分
#define _GET_EXPOND_TABLE_IDX( s )          ( abs( ( int )( s % kMOD_CsDatabaseMachineSize ) ) )
#define _GET_PLAYER_TABLE_IDX( s )          ( abs( ( int )( s % kMOD_CsDatabaseMachineSize ) ) ) // member_uid 分
#define _GET_DATALOG_TABLE_IDX( s )         ( abs( ( int )( s % kMOD_CsDatabaseMachineSize ) ) ) // member_uid 分
#define _GET_BULLETIN_TABLE_IDX( s )        ( abs( ( int )( s % kMOD_CsDatabaseMachineSize ) ) )

    // partation
    const static int kPARTATION_CsMemberDBTable   = 1000; //  
    const static int kPARTATION_CsExpondDBTable   = 100;  // paper model
    const static int kPARTATION_CsPlayerDBTable   = 1000;
    const static int kPARTATION_CsDatalogDBTable  = 10;   // csdatelog_%05d_yyyymm
    const static int kPARTATION_CsBulletinDBTable = 10;

#define _DEFINE_CHARACTER_SET               "utf8mb4"
#define _DEFINE_COLLATE                     "utf8mb4_bin"
}
