//
//
//
//
//
//
//
//  database 協助模組 
// 
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "xdatabase.h"

namespace xdb
{
    namespace helper
    {

        /////////////////////////////////////////////////////////////////////////////////
        //  非即時 查詢
        //    將查詢放入 柱列中 再由 執行續 執行 以降低 不需要及時回覆的查詢時間
        //    e.g. log 寫入
        class UnPromptQuery
        {
        public:

            class OnQueryListener
            {
            public:
                virtual void OnResult( const XDBResult result ) = 0;
            };

        public:

            UnPromptQuery();
            ~UnPromptQuery();

            static void Create( int process );
            static void Destroy();

            static void Query( XDatabase* p_query_db, const char* sql_string, UnPromptQuery::OnQueryListener* p_listener = NULL );
            static void Query( XDatabase* p_query_db, const wchar_t* sql_string, UnPromptQuery::OnQueryListener* p_listener = NULL );
        };


        /////////////////////////////////////////////////////////////////////////////////
        //  資料庫物件管理池   
        //
        enum class XDB_Type
        {
            XDB_MYSQL = 3
        };

        class DBObjectPool
        {
    
        public:

            DBObjectPool();
            ~DBObjectPool();

            static XDatabase* CreateDB( _IN XDB_Type xdb_type = XDB_Type::XDB_MYSQL );

            static void       DestoryDB( _IN XDatabase* xdb );

            static bool       InsertPool( _IN const char_t* pool_name, _IN uint_t pool_size, _IN XDB_Type xdb_type,
                                          _IN const char_t* db, _IN const char_t* host, _IN unsigned short port, _IN const char_t* username, _IN const char_t* password, _IN const char_t* char_set );
            static bool       InsertPool( _IN const wchar_t* pool_name, _IN uint_t pool_size, _IN XDB_Type xdb_type,
                                          _IN const wchar_t* db, _IN const wchar_t* host, _IN unsigned short port, _IN const wchar_t* username, _IN const wchar_t* password, _IN const wchar_t* char_set );

            static bool       RemovePool( _IN const char_t* pool_name );
            static bool       RemovePool( _IN const wchar_t* pool_name );

            static void       GetAllPoolName( _OUT std::vector<_mstring>& pool_name );
            static void       GetAllPoolName( _OUT std::vector<_wstring>& pool_name );

            // idx < 0 or > size => get random
            static XDatabase* GetDatabase( _IN const char_t* pool_name, _IN int_t idx = -1 );
            static XDatabase* GetDatabase( _IN const wchar_t* pool_name, _IN int_t idx = -1 );
        };


        /////////////////////////////////////////////////////////////////////////////////
        // 自動刪除 物件
        class DBS_Object
        {
            XDatabase* db_obj_;

        public:

            DBS_Object( _IN XDB_Type xdb_type = XDB_Type::XDB_MYSQL );
            ~DBS_Object();

            XDatabase* operator->( void );
            operator XDatabase* ( );
        };

    }  
}