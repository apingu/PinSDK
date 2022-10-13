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
#include <database/xdb-query.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-err.h"
#include "cs-db_query_base.h"


namespace cs_db_function
{
#define MYSQLDB                           ( ( XDatabase* )mydb )
#define GET_DB_RESULT( r, col, row )      ( MYSQLDB->GetQueryResultM( r, col, row ) )

    //=====================================================================================
    namespace utility
    {
        // member 所有遊戲 系統共用
        inline void dbCreateCsCommonTable( _IN XDatabase* mydb )
        {
            {
                XDBResult result00;
                result00 = MYSQLDB->ExecQuery( cs_db_query::member::CreateIndex().c_str() );
                MYSQLDB->FreeResult( result00 );
            }

            //if( !xdb::query::inquire::IfTableExist( MYSQLDB, cs_db_query::member::GetTableName( 0 ).c_str() ) )
            //{
            //    XDBResult result00;
            //    for( int ti = 0; ti < MAX_CsCommonDBTable; ti++ )
            //    {
            //        result00 = MYSQLDB->ExecQuery( cs_db_query::member::CreateTable( ti ).c_str() );
            //        MYSQLDB->FreeResult( result00 );
            //    }
            //}
            return;
        }

        // player 個遊戲系統分開
        //inline void dbCreateCsExpondTable( _IN XDatabase* mydb )
        //{
        //    //XDBResult result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_cshistory( 0 ) );
        //    //if ( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
        //    //{
        //    //    XDBResult result00;
        //    //    for ( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
        //    //    {
        //    //        result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_cshistory( ti ) );
        //    //        MYSQLDB->FreeResult( result00 );
        //    //    }
        //    //}
        //    //MYSQLDB->FreeResult( result );
        //
        //    XDBResult result = MYSQLDB->ExecQuery( cs_db_query::init::IfTableExist_csplayer( 0 ).c_str() );
        //    if( MYSQLDB->GetQueryResultT( result, 0, 0 ) == ( "0" ) )
        //    {
        //        XDBResult result00;
        //        for( int ti = 0; ti < kMOD_CsExpondDBTable; ti++ )
        //        {
        //            result00 = MYSQLDB->ExecQuery( cs_db_query::init::CreateTable_csplayer( ti ).c_str() );
        //            MYSQLDB->FreeResult( result00 );
        //        }
        //    }
        //    MYSQLDB->FreeResult( result );
        //    return;
        //}
    }

    //=====================================================================================
    /*
    namespace DBUnique
    {
        inline uniqueid_t dbCreateUniqueId( XDatabase* mydb, int gid )
        {
            //TS_Lock lock(&g_lQuery);
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::uniqueid::CreateUniqueId( gid ).c_str() );
            MYSQLDB->FreeResult( result );
            result = MYSQLDB->ExecQuery( cs_db_query::uniqueid::GetLastInsertId().c_str() );
            uniqueid_t uid = atoi( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            //          result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //             MYSQLDB->FreeResult(result);
            return uid;
        }


        inline void dbDeleteUniqueId( XDatabase* mydb, uniqueid_t uid )
        {
            //TS_Lock lock(&g_lQuery);
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::uniqueid::DeleteUniqueId( uid ).c_str() );
            MYSQLDB->FreeResult( result );
            //             result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //             MYSQLDB->FreeResult(result);
            return;
        }

        inline uniqueid_t dbGetUniqueIdGroup( XDatabase* mydb, uniqueid_t uid )
        {
            //TS_Lock lock(&g_lQuery);
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::uniqueid::GetIdGroup( uid ).c_str() );
            int gid = atoi( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            //             result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //             MYSQLDB->FreeResult(result);
            return gid;
        }
    };
    */
    /*
    //=====================================================================================
    namespace DBUtility
    {
        inline void dbCharSet( XDatabase* mydb, const _cs_char_t* charset )
        {
            //TS_Lock lock(&g_lQuery);
            XDBResult result0 = MYSQLDB->ExecQuery( cs_db_query::utility::SetCharSet( charset ).c_str() );
            MYSQLDB->FreeResult( result0 );
        }
    };
    */

    //=====================================================================================
    namespace member
    {
        //=================================================================================
        //
        //
        //=================================================================================
        inline bool dbCreateTable( _IN XDatabase* mydb, cs_session_uid_t member_uid )
        {
            bool ret = false;
            //if( !xdb::query::inquire::IfTableExist( MYSQLDB, cs_db_query::member::GetTableName( 0 ).c_str() ) )
            {
                XDBResult result00;
                result00 = MYSQLDB->ExecQuery( cs_db_query::member::CreateTable( member_uid ).c_str() );
                if( MYSQLDB->GetQuerySuccessful( result00 ) )
                {
                    ret = true;
                }
                MYSQLDB->FreeResult( result00 );
            }
            return ret;
        }

        //=================================================================================
        //
        //
        //=================================================================================
        inline bool dbCheckMember( XDatabase* mydb, const _cs_char_t* username, const _cs_char_t* password )
        {
            //TS_Lock lock(&g_lQuery);
            cs_session_uid_t member_uid = cyberspace::uid_generator::GetSessionUID( username );
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::CheckLogin( member_uid, username, password ).c_str() );
            //result = MYSQLDB->ExecQuery(cs_db_query::LockTable("member", 0));
            //MYSQLDB->FreeResult(result);

            if( MYSQLDB->GetResultEmpty( result ) )
            {
                MYSQLDB->FreeResult( result );
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return false;
            }
            cs_session_uid_t db_member_uid = atoull( GET_DB_RESULT( result, 0, 0 ).c_str() );

            MYSQLDB->FreeResult( result );

            if( db_member_uid != member_uid )
            {
                return false;
            }
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        // DESC: 傳入帳號name 取得 Id
        //=================================================================================
        inline bool dbSetSessionUID( _IN XDatabase* mydb, _IN cs_session_uid_t member_uid, _IN const _cs_char_t* name )
        {
            bool ret = true;
            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetIndexSessionUID( member_uid, name ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    ret = false;
                }
                //XDBResult result = MYSQLDB->ExecQuery(cs_db_query::LockTable("member", 0));
                //MYSQLDB->FreeResult(result);
                MYSQLDB->FreeResult( result );
            }

            {
                //TS_Lock lock(&g_lQuery);
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetSessionUID( member_uid, name ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    ret = false;
                }
                //XDBResult result = MYSQLDB->ExecQuery(cs_db_query::LockTable("member", 0));
                //MYSQLDB->FreeResult(result);
                MYSQLDB->FreeResult( result );
            }
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return ret;
        }

        //=================================================================================
        //
        // DESC: 傳入帳號name 取得 Id
        //=================================================================================
        inline cs_session_uid_t dbGetSessionUID( XDatabase* mydb, const _cs_char_t* name )
        {
            //TS_Lock lock(&g_lQuery);
            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetSessionUID( cyberspace::uid_generator::GetSessionUID( name ), name ).c_str() );
            if( MYSQLDB->GetResultEmpty( result ) )
            {
                //g_lQuery.UnLock();
                MYSQLDB->FreeResult( result );
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return CsINVALID_ID;
            }

            cs_session_uid_t member_uid = atoi( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return member_uid;
        }

        //=================================================================================
        //
        // DESC: 傳入帳號name 取得 Id
        //=================================================================================
        //inline _cs_string dbGetUsername( XDatabase* mydb, cs_session_uid_t member_uid )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return "";
        //    }
        //
        //    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetMemberData( _GET_COMMON_TABLE_IDX( member_uid ), member_uid ) );
        //    if( MYSQLDB->GetResultEmpty( result ) )
        //    {
        //        MYSQLDB->FreeResult( result );
        //        return "";
        //    }
        //    _cs_string        username = GET_DB_RESULT( result, 0, 0 );
        //    cs_group_uid_t     group_uid = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
        //    cs_authority_t authorityid = GET_DB_RESULT( result, 2, 0 );
        //    CseMEMBER_STATE state = ( CseMEMBER_STATE )atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
        //    longlong_t      point = atoll( GET_DB_RESULT( result, 4, 0 ).c_str() );
        //    int             flag = atoi( GET_DB_RESULT( result, 5, 0 ).c_str() );
        //    stimen_t        verify_sec = atoll( GET_DB_RESULT( result, 6, 0 ).c_str() );
        //    stimen_t        login_tm = atoll( GET_DB_RESULT( result, 7, 0 ).c_str() );
        //    stimen_t        logout_tm = atoll( GET_DB_RESULT( result, 8, 0 ).c_str() );
        //    stimen_t        curr_tm = atoll( GET_DB_RESULT( result, 9, 0 ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //
        //    //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //    //MYSQLDB->FreeResult(result);
        //    return username;
        //}

        //=================================================================================
        //
        // DESC: 取出登入檢查用的條件
        //     stime =>CurrLoginSec 跟 NOW()的差距時間
        //=================================================================================
        inline bool dbGetCheckQualification( XDatabase* mydb, const char* username, int& state, macn_t& mac, int& mtype, stimen_t& stime )
        {
            //TS_Lock lock(&g_lQuery);
            if( username == NULL )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetCheckQualification( cyberspace::uid_generator::GetSessionUID( username ), username ).c_str() );
            if( MYSQLDB->GetResultEmpty( result ) )
            {
                MYSQLDB->FreeResult( result );
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return false;
            }

            //maca_t maca = net::Socket::MACNToA(mac);
            //_csscanf(GET_DB_RESULT(result, 1, 0).c_str(), "%02X%02X%02X%02X%02X%02X", &(buff[0]),&(buff[1]),&(buff[2]),&(buff[3]),&(buff[4]),&(buff[5]));
            stime = atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
            mtype = atoi( GET_DB_RESULT( result, 2, 0 ).c_str() );
            mac = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
            state = atoi( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        //
        //=================================================================================
        inline bool dbGetCheckQualification( XDatabase* mydb, cs_session_uid_t member_uid, int& state, macn_t& mac, int& mtype, stimen_t& stime )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetCheckQualification( member_uid ).c_str() );
            if( MYSQLDB->GetResultEmpty( result ) )
            {
                //g_lQuery.UnLock();
                MYSQLDB->FreeResult( result );
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return false;
            }

            //maca_t maca = net::Socket::MACNToA(mac);
            //_csscanf(GET_DB_RESULT(result, 1, 0).c_str(), "%02X%02X%02X%02X%02X%02X", &(buff[0]),&(buff[1]),&(buff[2]),&(buff[3]),&(buff[4]),&(buff[5]));
            stime = atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
            mtype = atoi( GET_DB_RESULT( result, 2, 0 ).c_str() );
            mac = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
            state = atoi( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        //
        //=================================================================================
        //inline bool dbGetIdentity( XDatabase* mydb, cs_session_uid_t member_uid, _cs_string& name, cs_authority_t& authority, long32_t& flag )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return false;
        //    }
        //
        //    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetIdentity( _GET_COMMON_TABLE_IDX( member_uid ), member_uid ) );
        //    if( MYSQLDB->GetResultEmpty( result ) )
        //    {
        //        //g_lQuery.UnLock();
        //        MYSQLDB->FreeResult( result );
        //        //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //        //MYSQLDB->FreeResult(result);
        //        return false;
        //    }
        //
        //    name = GET_DB_RESULT( result, 0, 0 ).c_str();
        //    authority = GET_DB_RESULT( result, 1, 0 ).c_str();
        //    flag = atoi( GET_DB_RESULT( result, 2, 0 ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //    //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //    //MYSQLDB->FreeResult(result);
        //    return true;
        //}

        //=================================================================================
        //
        // DESC: 傳入帳號name 取得 Id
        //=================================================================================
        inline bool dbSetPassword( XDatabase* mydb, cs_session_uid_t member_uid, const _cs_char_t* pwd )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetPassword( member_uid, pwd ).c_str() );
            if( !MYSQLDB->GetQuerySuccessful( result ) )
            {
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return false;
            }

            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        // DESC: 傳入 Id 設定 group_uid
        //=================================================================================
        inline bool dbSetGroupUID( XDatabase* mydb, cs_session_uid_t member_uid, const _cs_char_t* group_uid )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetGroupUID( member_uid, group_uid ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    return false;
                }
                MYSQLDB->FreeResult( result );
            }

            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetIndexGroupUID( member_uid, group_uid ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    return false;
                }
                MYSQLDB->FreeResult( result );
            }

            return true;
        }

        //=================================================================================
        // 0 未登入  1 登入中  2禁止登入 3登出中
        //
        //=================================================================================
        inline bool dbSetState( XDatabase* mydb, cs_session_uid_t member_uid, CseMEMBER_STATE flg )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetState( member_uid, flg ).c_str() );
            if( !MYSQLDB->GetQuerySuccessful( result ) )
            {
                //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
                //MYSQLDB->FreeResult(result);
                return false;
            }

            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        // 使用者資訊
        //
        //=================================================================================
        //struct Data
        //{
        //    _cs_string           username;
        //    CseMEMBER_STATE    state;        //0 未登入  1 登入中  2禁止登入 3登出中
        //    _cs_string           information;
        //    longlong_t         point00;
        //    longlong_t         point01;
        //    longlong_t         point02;
        //    cs_group_uid_t     group_uid;        // permission
        //    int                flag;             // permission
        //    cs_permissionll_t  pri_permissionl;  // permission
        //    cs_permissionss_t  pri_permissions;  // permission
        //    cs_permissionll_t  pub_permissionl;  // permission
        //    cs_permissionss_t  pub_permissions;  // permission
        //    stimen_t           login_tm;
        //    stimen_t           logout_tm;
        //    stimen_t           curr_tm;
        //};
        typedef _mbsmap Data;

        const _cs_string _member_data_username = "username";
        const _cs_string _member_data_state = "state";
        const _cs_string _member_data_acc_quota = "acc_quota";
        const _cs_string _member_data_acc_point00 = "acc_point00";
        const _cs_string _member_data_acc_point01 = "acc_point01";
        const _cs_string _member_data_acc_point02 = "acc_point02";
        const _cs_string _member_data_acc_point03 = "acc_point03";
        const _cs_string _member_data_acc_point04 = "acc_point00";
        const _cs_string _member_data_acc_point05 = "acc_point01";
        const _cs_string _member_data_acc_point06 = "acc_point02";
        const _cs_string _member_data_acc_point07 = "acc_point03";

        const _cs_string _member_data_flag = "flag";
        const _cs_string _member_data_group_uid = "group_uid";
        const _cs_string _member_data_pri_permissionl = "pri_permissionl";
        const _cs_string _member_data_pri_permissions = "pri_permissions";
        const _cs_string _member_data_pub_permissionl = "pub_permissionl";
        const _cs_string _member_data_pub_permissions = "pub_permissions";

        inline bool dbGetData( _OUT Data& info, _IN XDatabase* mydb, _IN cs_session_uid_t member_uid )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetMemberData( member_uid ).c_str() );
            if( MYSQLDB->GetResultEmpty( result ) )
            {
                MYSQLDB->FreeResult( result );
                return false;
            }

            info = MYSQLDB->GetResultRecordM( result, 0 );

            //info = MYSQLDB->GetResultMapM( result )[0];            

            //info.username = GET_DB_RESULT( result, 0, 0 );
            //info.state = ( CseMEMBER_STATE )atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
            //info.information = GET_DB_RESULT( result, 2, 0 );
            //info.group_uid = GET_DB_RESULT( result, 3, 0 ).c_str();
            //info.point00 = atoll( GET_DB_RESULT( result, 4, 0 ).c_str() );
            //info.point01 = atoll( GET_DB_RESULT( result, 5, 0 ).c_str() );
            //info.point02 = atoll( GET_DB_RESULT( result, 6, 0 ).c_str() );
            //info.flag = atoi( GET_DB_RESULT( result, 7, 0 ).c_str() );
            //info.pri_permissionl = atoll( GET_DB_RESULT( result, 8, 0 ).c_str() );
            //info.pub_permissionl = atoll( GET_DB_RESULT( result, 9, 0 ).c_str() );
            //info.pri_permissions = GET_DB_RESULT( result, 10, 0 ).c_str();
            //info.pub_permissions = GET_DB_RESULT( result, 11, 0 );
            //info.login_tm = atoll( GET_DB_RESULT( result, 12, 0 ).c_str() );
            //info.logout_tm = atoll( GET_DB_RESULT( result, 13, 0 ).c_str() );
            //info.curr_tm = atoll( GET_DB_RESULT( result, 14, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            return true;
        }

        // 只會寫入權限的部分
        inline bool dbSetPermission( _IN XDatabase* mydb, _IN cs_session_uid_t member_uid, _IN Data& info )
        {
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::SetMemberPermission(
                member_uid,
                pfstd::strtoi( info[ _member_data_flag ] ),
                info[ _member_data_group_uid ],
                pfstd::strtoll( info[ _member_data_pri_permissionl ] ),
                pfstd::strtoll( info[ _member_data_pub_permissionl ] ),
                info[ _member_data_pri_permissions ],
                info[ _member_data_pub_permissions ] ).c_str() );
            if( !MYSQLDB->GetQuerySuccessful( result ) )
            {
                MYSQLDB->FreeResult( result );
                return false;
            }

            MYSQLDB->FreeResult( result );
            return true;
        }


        //=================================================================================
        //
        //
        //=================================================================================
        inline bool dbSetAllowLogin( XDatabase* mydb, cs_session_uid_t member_uid, macn_t mac )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            //_cs_char_t buff[50];
            //maca_t maca = net::Socket::MACNToA(mac);
            //_csprintf(buff, "%02X%02X%02X%02X%02X%02X%", maca[0],maca[1],maca[2],maca[3],maca[4],maca[5]);
            //printf("MID = %s\n", buff);
            int state = ACCSTATE_OFFLINE;
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::SetAllowLogin( member_uid, mac, state ).c_str() );
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        //
        //=================================================================================
        inline bool dbSetLogin( XDatabase* mydb, cs_session_uid_t member_uid, cs_section_uid_t sid )
        {
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            int state = ACCSTATE_ONLINE;
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::SetLogin( member_uid, sid, state ).c_str() );
            MYSQLDB->FreeResult( result );
            return true;
        }

        //=================================================================================
        //
        // 要記錄登出時間 -1 不記錄登出時間
        //=================================================================================
        inline bool dbSetLogout( XDatabase* mydb, cs_session_uid_t member_uid/*, stimen_t currtime*/ )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::SetLogout( member_uid, ACCSTATE_OFFLINE ).c_str() );
            //else
            // result = MYSQLDB->ExecQuery(cs_db_query::member::SetLogout(member_uid, ACCSTATE_OFFLINE, currtime));
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        // 要記錄登出時間 -1 不記錄登出時間 30 秒內 驗證仍然有效
        //=================================================================================
        inline bool dbSetRetentionLogout( XDatabase* mydb, cs_session_uid_t member_uid/*, stimen_t currtime*/ )
        {
            //TS_Lock lock(&g_lQuery);
            if( member_uid == CsINVALID_ID )
            {
                return false;
            }

            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::SetRetentionLogout( member_uid, ACCSTATE_OFFLINE ).c_str() );
            //else
            // result = MYSQLDB->ExecQuery(cs_db_query::member::SetRetentionLogout(member_uid, ACCSTATE_OFFLINE, currtime));
            MYSQLDB->FreeResult( result );
            //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
            //MYSQLDB->FreeResult(result);
            return true;
        }

        //=================================================================================
        //
        //
        //=================================================================================
        //inline void dbGetState( XDatabase* mydb, cs_session_uid_t member_uid, CseMEMBER_STATE& memstate, stimen_t& lastlogout, stimen_t& currlogin, stimen_t& currtime )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return;
        //    }
        //
        //    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetMemberData( _GET_COMMON_TABLE_IDX( member_uid ), member_uid ) );
        //    if( MYSQLDB->GetResultEmpty( result ) )
        //    {
        //        MYSQLDB->FreeResult( result );
        //        return;
        //    }
        //    _cs_string        username = GET_DB_RESULT( result, 0, 0 );
        //    cs_group_uid_t     group_uid = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
        //    cs_authority_t authorityid = GET_DB_RESULT( result, 2, 0 );
        //    CseMEMBER_STATE state = ( CseMEMBER_STATE )atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
        //    longlong_t      point = atoll( GET_DB_RESULT( result, 4, 0 ).c_str() );
        //    int             flag = atoi( GET_DB_RESULT( result, 5, 0 ).c_str() );
        //    stimen_t        verify_sec = atoll( GET_DB_RESULT( result, 6, 0 ).c_str() );
        //    stimen_t        login_tm = atoll( GET_DB_RESULT( result, 7, 0 ).c_str() );
        //    stimen_t        logout_tm = atoll( GET_DB_RESULT( result, 8, 0 ).c_str() );
        //    stimen_t        curr_tm = atoll( GET_DB_RESULT( result, 9, 0 ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //
        //    memstate = state;
        //    currlogin = login_tm;
        //    lastlogout = logout_tm;
        //    currtime = curr_tm;
        //    //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //    //MYSQLDB->FreeResult(result);
        //    return;
        //}

        //=================================================================================
        //
        //
        //=================================================================================
        //inline longlong_t dbGetPoint( XDatabase* mydb, cs_session_uid_t member_uid )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return 0;
        //    }
        //    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::member::GetMemberData( _GET_COMMON_TABLE_IDX( member_uid ), member_uid ) );
        //    if( MYSQLDB->GetResultEmpty( result ) )
        //    {
        //        MYSQLDB->FreeResult( result );
        //        return 0;
        //    }
        //    _cs_string        username = GET_DB_RESULT( result, 0, 0 );
        //    cs_group_uid_t     group_uid = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
        //    cs_authority_t authorityid = GET_DB_RESULT( result, 2, 0 );
        //    CseMEMBER_STATE state = ( CseMEMBER_STATE )atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
        //    longlong_t      point = atoll( GET_DB_RESULT( result, 4, 0 ).c_str() );
        //    int             flag = atoi( GET_DB_RESULT( result, 5, 0 ).c_str() );
        //    stimen_t        verify_sec = atoll( GET_DB_RESULT( result, 6, 0 ).c_str() );
        //    stimen_t        login_tm = atoll( GET_DB_RESULT( result, 7, 0 ).c_str() );
        //    stimen_t        logout_tm = atoll( GET_DB_RESULT( result, 8, 0 ).c_str() );
        //    stimen_t        curr_tm = atoll( GET_DB_RESULT( result, 9, 0 ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //    //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //    //MYSQLDB->FreeResult(result);
        //    return point;
        //}

        //=================================================================================
        //
        //
        //=================================================================================
        //inline int dbSetPoint( XDatabase* mydb, cs_session_uid_t member_uid, longlong_t point )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return false;
        //    }
        //
        //    XDBResult result = MYSQLDB->ExecQuery( cs_db_query::member::SetPoint( _GET_MEMBER_TABLE_IDX( member_uid ), member_uid, point ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //    //result=MYSQLDB->ExecQuery(cs_db_query::UnLockTable());
        //    //MYSQLDB->FreeResult(result);
        //    return true;
        //}

        //=================================================================================
        //
        //
        //=================================================================================
        //inline longlong_t dbAddPoint( XDatabase* mydb, cs_session_uid_t member_uid, longlong_t addpoint )
        //{
        //    //TS_Lock lock(&g_lQuery);
        //    if( member_uid == CsINVALID_ID )
        //    {
        //        return 0;
        //    }
        //
        //    XDBResult result;
        //    //             result = MYSQLDB->ExecQuery(cs_db_query::LockTable("member"));
        //    //             MYSQLDB->FreeResult(result);
        //    result = MYSQLDB->ExecQuery( cs_db_query::member::AddPoint( _GET_MEMBER_TABLE_IDX( member_uid ), member_uid, addpoint ).c_str() );
        //    MYSQLDB->FreeResult( result );
        //
        //    Data data;
        //    dbGetData( data, mydb, member_uid );
        //    return data.point00;
        //}
    };

    //=====================================================================================
    /*
    namespace DBHistory
    {

        inline uniqueid_t dbBeginMemberHistory( XDatabase* mydb, cs_session_uid_t member_uid, ipa_t ip, macn_t mac, cs_section_uid_t sectionid )
        {
            XDBResult result;
            result = MYSQLDB->ExecQuery( cs_db_query::cshistory::BeginHistory( _GET_COMMON_TABLE_IDX( member_uid ), member_uid, ip, mac, sectionid ) );
            MYSQLDB->FreeResult( result );
            result = MYSQLDB->ExecQuery( cs_db_query::uniqueid::GetLastInsertId() );
            uniqueid_t uid = atoll( GET_DB_RESULT( result, 0, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            return uid;
        }


        //inline void dbEndMemberHistory( XDatabase* mydb, uniqueid_t hid, stimen_t duration_sec )
        inline void dbEndMemberHistory( XDatabase* mydb, uniqueid_t hid )
        {
            if ( hid == CsINVALID_ID )
            {
                return;
            }

            //XDBResult result = MYSQLDB->ExecQuery( cs_db_query::cshistory::EndHistory( _GET_COMMON_TABLE_IDX( hid ), hid, duration_sec ) );
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::cshistory::EndHistory( _GET_COMMON_TABLE_IDX( hid ), hid ) );
            MYSQLDB->FreeResult( result );
            return;
        }
    };
    */
    //=====================================================================================
    namespace paper
    {
        inline void dbSetRecord( XDatabase* mydb, _IN const _cs_char_t* key, _IN const _cs_char_t* val, _IN int create_time, _IN int live_sec )
        {
            // 先建立table
            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::CreateTable( key ).c_str() );
                MYSQLDB->FreeResult( result );
            }

            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::SetRecord( key, val, create_time, live_sec ).c_str() );
                MYSQLDB->FreeResult( result );
            }
            return;
        }

        inline void dbSetnxRecord( XDatabase* mydb, _IN const _cs_char_t* key, _IN const _cs_char_t* val, _IN int create_time, _IN int live_sec )
        {
            // 先建立table
            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::CreateTable( key ).c_str() );
                MYSQLDB->FreeResult( result );
            }

            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::SetnxRecord( key, val, create_time, live_sec ).c_str() );
                MYSQLDB->FreeResult( result );
            }
            return;
        }

        inline int dbGetRecord( XDatabase* mydb, _IN const _cs_char_t* key, _OUT XDBResBuf& value, _OUT int& create_time, _OUT int& update_time, _OUT int& live_sec )
        {
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::GetRecord( key ).c_str() );
            if( MYSQLDB->GetResultEmpty( result ) )
            {
                MYSQLDB->FreeResult( result );
                create_time = -1;
                update_time = -1;
                live_sec = -1;
                return -1;
            }

            value = MYSQLDB->GetQueryResource( result, 0, 0 );
            create_time = atoi( GET_DB_RESULT( result, 1, 0 ).c_str() );
            update_time = atoi( GET_DB_RESULT( result, 2, 0 ).c_str() );
            live_sec = atoi( GET_DB_RESULT( result, 3, 0 ).c_str() );
            MYSQLDB->FreeResult( result );
            return 1;
        }

        inline void dbDelRecord( XDatabase* mydb, _IN const _cs_char_t* key )
        {
            XDBResult result = MYSQLDB->ExecQuery( cs_db_query::paper::DelRecord( key ).c_str() );
            MYSQLDB->FreeResult( result );
            return;
        }
    }

    //=====================================================================================
    namespace log
    {
        inline bool dbSetRecord( _IN XDatabase* mydb, _IN int classify, _IN pftml::Time tbday, _IN int level, _IN cs_session_uid_t member_uid, _IN const _cs_char_t* event, _IN const _cs_char_t* data, _IN const _cs_char_t* content )
        {
            bool ret = true;
            // 先建立table
            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::log::CreateTable( classify, member_uid, tbday ).c_str() );
                MYSQLDB->FreeResult( result );
            }

            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::log::SetRecord( classify, tbday, level, member_uid, event, data, content ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    ret = false;
                }
                MYSQLDB->FreeResult( result );
            }
            return ret;
        }

        inline uint_t dbGetCount( _IN XDatabase* mydb, _IN int classify, _IN cs_session_uid_t member_uid, _IN pftml::Time begin_date, _IN pftml::Time end_date )
        {
            int count = 0;
            pftml::Time curr_date = begin_date;
            pftml::Time last_date = end_date;
            while( curr_date < last_date )
            {
                if( xdb::query::inquire::IfTableExist( MYSQLDB, cs_db_query::log::GetTableName( classify, member_uid, curr_date ).c_str() ) )
                {
                    //TS_Lock lock(&g_lQuery);
                    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::log::GetCount( classify, curr_date, member_uid, begin_date.GetDateTimeTCS().c_str(), end_date.GetDateTimeTCS().c_str() ).c_str() );
                    if( !MYSQLDB->GetQuerySuccessful( result ) )
                    {
                        count += atoi( MYSQLDB->GetQueryResultT( result, 0, 0 ).c_str() );
                    }
                    MYSQLDB->FreeResult( result );
                }
                curr_date.PlusMonth( 1 );
            }
            return count;
        }

        inline bool dbGetRecord( _OUT _cs_string_tableii& result_table, _IN XDatabase* mydb, _IN int classify, _IN cs_session_uid_t member_uid, _IN pftml::Time begin_date, _IN pftml::Time end_date, _IN int limit_offset, _IN int limit_rowcount )
        {
            bool ret = true;
            pftml::Time curr_date = begin_date;
            pftml::Time last_date = end_date;
            while( curr_date < end_date )
            {
                _cs_string tbname = cs_db_query::log::GetTableName( classify, member_uid, curr_date );
                if( xdb::query::inquire::IfTableExist( MYSQLDB, tbname.c_str() ) )
                {
                    XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::log::GetRecord( classify, curr_date, member_uid, begin_date.GetDateTimeTCS().c_str(), end_date.GetDateTimeTCS().c_str(), limit_offset, limit_rowcount ).c_str() );
                    if( MYSQLDB->GetQuerySuccessful( result ) )
                    {
                        for( uint_t ri = 0; ri < MYSQLDB->GetResultSize( result ); ri++ )
                        {

                            // log_uid, create_time, insert_time, member_uid, level, event, data, content
                            _cs_string_vector row_string;
                            {
                                _cs_string    value;
                                // 把log_uid 轉換為 log_add ( address )
                                row_string.push_back( cs_db_query::utility::MakeRecordAddress( tbname.c_str(), MYSQLDB->GetQueryResultT( result, 0, ri ).c_str() ) );
                                row_string.push_back( MYSQLDB->GetQueryResultT( result, 2, ri ).c_str() );  // insert_time
                                row_string.push_back( _pf_ulltot( member_uid ) );
                                row_string.push_back( MYSQLDB->GetQueryResultT( result, 4, ri ).c_str() );  // level
                                row_string.push_back( MYSQLDB->GetQueryResultT( result, 5, ri ).c_str() );
                                row_string.push_back( MYSQLDB->GetQueryResultT( result, 6, ri ).c_str() );
                                row_string.push_back( MYSQLDB->GetQueryResultT( result, 7, ri ).c_str() );
                            }
                            result_table.push_back( row_string );
                        }
                    }
                    else
                    {
                        ret = false;
                    }
                    MYSQLDB->FreeResult( result );
                }
                curr_date.PlusMonth( 1 );
            }

            return false;
        }
    }

    //=====================================================================================
    //  選擇區段     找出 該區段內的 標題留言
    //  選擇標題留言 找出 該留言的回覆
    namespace bulletin
    {
        inline bool dbSetRecord( _IN XDatabase* mydb, _IN int classify, _IN pftml::Time tbday, _IN uniqueid_t bulletin_gid, _IN cs_session_uid_t member_uid, _IN uint64_t reply_bulletin_uid, _IN const _cs_char_t* title, _IN const _cs_char_t* content )
        {
            bool ret = true;
            // 先建立table
            {
                XDBResult result = MYSQLDB->ExecQuery( cs_db_query::bulletin::CreateTable( classify, bulletin_gid, tbday ).c_str() );
                MYSQLDB->FreeResult( result );
            }

            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::bulletin::SetRecord( classify, tbday, member_uid, bulletin_gid, reply_bulletin_uid, title, content ).c_str() );
                if( !MYSQLDB->GetQuerySuccessful( result ) )
                {
                    ret = false;
                }
                MYSQLDB->FreeResult( result );
            }
            return ret;
        }

        inline uint_t dbGetCount( _IN XDatabase* mydb, _IN int classify, _IN uniqueid_t bulletin_gid )
        {
            int count = 0;
            _mbsvector all_table_names;
            xdb::query::inquire::SelectAllTableNames( all_table_names, mydb, cs_db_query::bulletin::GetTableName( classify, bulletin_gid ).c_str() );

            for( uint_t ti = 0; ti < all_table_names.size(); ti++ )
            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::bulletin::GetCount( all_table_names[ ti ].c_str(), bulletin_gid ).c_str() );
                if( MYSQLDB->GetQuerySuccessful( result ) )
                {
                    count += atoi( MYSQLDB->GetQueryResultT( result, 0, 0 ).c_str() );
                }
                MYSQLDB->FreeResult( result );
            }
            return count;
        }

        // bulletin_adr
        inline bool dbGetRecord( _OUT _cs_string_tableii& result_table, _IN XDatabase* mydb, _IN int classify, _IN uniqueid_t bulletin_gid, _IN int limit_offset, _IN int limit_rowcount )
        {
            bool ret = true;
            _mbsvector all_table_names;
            xdb::query::inquire::SelectAllTableNames( all_table_names, mydb, cs_db_query::bulletin::GetTableName( classify, bulletin_gid ).c_str() );

            for( uint_t ti = 0; ti < all_table_names.size(); ti++ )
            {
                XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::bulletin::GetRecord( all_table_names[ ti ].c_str(), bulletin_gid, limit_offset, limit_rowcount ).c_str() );
                if( MYSQLDB->GetQuerySuccessful( result ) )
                {
                    // bulletin_uid, bulletin_gid, create_time, insert_time, update_time, update_count, member_uid, reply_bulletin_uid, title, content
                    for( uint_t ri = 0; ri < MYSQLDB->GetResultSize( result ); ri++ )
                    {
                        // bulletin_uid, bulletin_gid, create_time, insert_time, update_time, update_count, member_uid, reply_bulletin_uid, title, content
                        _cs_string_vector row_string;
                        {
                            _cs_string value;

                            // 把log_uid 轉換為 log_add ( address )
                            row_string.push_back( cs_db_query::utility::MakeRecordAddress( all_table_names[ ti ].c_str(), MYSQLDB->GetQueryResultT( result, 0, ri ).c_str() ) );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 1, ri ).c_str() );  // bulletin_gid
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 3, ri ).c_str() );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 4, ri ).c_str() );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 5, ri ).c_str() );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 6, ri ).c_str() );
                            row_string.push_back( cs_db_query::utility::MakeRecordAddress( all_table_names[ ti ].c_str(), MYSQLDB->GetQueryResultT( result, 7, ri ).c_str() ) );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 8, ri ).c_str() );
                            row_string.push_back( MYSQLDB->GetQueryResultT( result, 9, ri ).c_str() );
                        }
                        result_table.push_back( row_string );
                    }
                }
                else
                {
                    ret = false;
                }
                MYSQLDB->FreeResult( result );
            }
            return ret;
        }

        inline bool dbDelRecord( _IN XDatabase* mydb, _IN const _cs_char_t* bulletin_adr )
        {
            _cs_string tbname;
            _cs_string uid;
            if( !cs_db_query::utility::SplitRecordAddress( tbname, uid, bulletin_adr ) )
            {
                return false;
            }

            XDBResult result = ( XDBResult )MYSQLDB->ExecQuery( cs_db_query::bulletin::DelRecord( tbname.c_str(), _pf_ttoull( uid ) ).c_str() );
            MYSQLDB->FreeResult( result );
            return true;
        }
    }
};