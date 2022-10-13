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
//
#pragma once

#include "cs-def.h"
#include <pf-time.h>
#include <database/xdatabase.h>


namespace cyberspace
{
    enum CsDBType
    {
        CsDBType_Common,  // 0:會員等多個產品共用的
        CsDBType_Expand,  // 1:各產品獨立的
        CsDBType_Extern,  // 2:額外的 像log 因為增長 所以讓 db乾淨
        CsDBType_MAX,
    };

    // 因為遊戲中 可以分為
    // quota         額度
    // point00 ~ 03  四種款項 遊戲勝負 水錢01~03?(用來做不同計算用)
    // 各自的欄位名稱
#define _ACC_QUOTA     ( "acc_quota" );
#define _ACC_POINT00   ( "acc_point00" );
#define _ACC_POINT01   ( "acc_point01" );
#define _ACC_POINT02   ( "acc_point02" );
#define _ACC_POINT04   ( "acc_point04" );

    //typedef std::map< _cs_string, longlong_t > cs_baccount_t;
    typedef _cs_string                         cs_db_pool_index_t;  // 資料庫池的名稱
    typedef uniqueid_t                         cs_db_obj_index_t;   // 資料庫池中 資料庫物件的位置

    class DatabaseService// : public UnitServer
    {
        typedef std::vector<XDatabase*>  XDBPool;

        bool                                         is_alive_;
        uint_t                                       service_identity_;  // 唯一識別碼
        //std::vector<XDatabase*> database_[ CsDBType_MAX ];
        std::map< cs_db_pool_index_t, XDBPool >      database_pool_[ CsDBType_MAX ];
        std::map< uint_t, cs_db_pool_index_t >       database_pool_seq_[ CsDBType_MAX ];
             
    public:

        DatabaseService();
        virtual ~DatabaseService();

        const _cs_char_t*       ObjectName() const { return ( "DatabaseService" ); }

        // if no error, return 0
        virtual int             Shutdown( void );

        virtual void            OnOutputMessage( _IN CsLogMode m, _IN const _cs_char_t* logtext ) = 0;

        // 所有的產品共用 config database 中的name
        XDatabase*              GetDB( _IN CsDBType db_type, _IN uniqueid_t db_index );

        //size32_t                GetDBSize( _IN CsDBType db_type, _IN cs_session_uid_t session_uid );

        // db_pool_idx :  "" 分散  @first 第一個
        // db_idx = -1 for get db by random
        XDatabase*              GetDatabase( _IN CsDBType db_type, _IN cs_db_pool_index_t db_pool_idx, _IN cs_db_obj_index_t db_obj_idx = -1 );
        //XDatabase*              GetFirstDatabase( _IN CsDBType db_type, _IN cs_db_obj_index_t db_obj_idx = -1 );

        // if return false server will stop
        virtual bool            OnLoadConfigure( std::map<_cs_string, _cs_string> setting ) = 0;

        // member function ============================================================
        // common db
        cs_session_uid_t        CreateMemberSessionUID( _IN const _cs_char_t* username );
        bool                    SetMemberPassword( _IN cs_session_uid_t session_uid, _IN const _cs_char_t* password );
        bool                    SetMemberGroupUID( _IN cs_session_uid_t session_uid, _IN cs_group_uid_t group_uid );

        cs_session_uid_t        CheckMemberSessionUID( _IN const _cs_char_t* username, _IN const _cs_char_t* password );
        static cs_session_uid_t GetMemberSessionUID( _IN const _cs_char_t* username );
        _cs_string              GetMemberName( _IN cs_session_uid_t session_uid );
        CseMEMBER_STATE         GetMemberState( _IN cs_session_uid_t session_uid );
        void                    SetMemberState( _IN cs_session_uid_t session_uid, _IN CseMEMBER_STATE state );

        // 使用ulongong 儲存的權限值
        bool                    GetMemberPermission( _OUT cs_flag_t& flag, _OUT cs_group_uid_t& group_uid,
                                                     _OUT cs_permissionll_t& pri_permissionl, _OUT cs_permissionll_t& pub_permissionl,
                                                     _OUT cs_permissionss_t& pri_permissions, _OUT cs_permissionss_t& pub_permissions,
                                                     _IN cs_session_uid_t session_uid );

        bool                    SetMemberPermission( _IN cs_flag_t flag, _IN cs_group_uid_t group_uid,
                                                     _IN cs_permissionll_t pri_permissionl, _IN cs_permissionll_t pub_permissionl,
                                                     _IN cs_permissionss_t pri_permissions, _IN cs_permissionss_t pub_permissions,
                                                     _IN cs_session_uid_t session_uid );

        // quota + point01 可用餘額?
        longlong_t              GetMemberAccountBalance( _IN cs_session_uid_t session_uid );
        //cs_baccount_t           GetMemberAccount( _IN cs_session_uid_t session_uid );
        //void                    SetMemberAccount( _IN cs_session_uid_t session_uid, _IN cs_baccount_t account );
        //longlong_t              AddMemberAccount( _IN cs_session_uid_t session_uid, _IN cs_baccount_t account );

        // log function ===============================================================
        // 前為log 的分類 表名 會成為 clogxxxxxx_xxxxx_日期 讓log 可以用在不同的作用中
        // classify: 哪一種類型的log
        // level: log的等級 紀錄 錯誤 警告.... 自定
        // data,255 跟 content,2048 都是自行定義文字格式
        void                    SetLogMessage( _IN int classify, _IN cs_session_uid_t session_uid, _IN int level, _IN const _cs_char_t* event, _IN const _cs_char_t* data, _IN const _cs_char_t* content );
        // 某一個區段時間內
        size32_t                GetLogCount( _IN int classify, _IN cs_session_uid_t session_uid, _IN const pftml::Time& bdate, _IN const pftml::Time& edate );
        // 某一個區段時間內  開始的資料 取的筆數
        // log_adr, insert_time, member_uid, event, event, data, content
        bool                    GetLogMessage( _OUT _cs_string_tableii& result_table, _IN int classify, _IN cs_session_uid_t session_uid, _IN const pftml::Time& bdate, _IN const pftml::Time& edate, _IN int limit_offset, _IN int limit_rowcount );

        // bulletin function ==========================================================
        // 留言的種類 => 自訂 (公告?  錯誤回報?..... )  留言的群組
        // bulletin_gid 留言的編號  會讓同編號的留言都放在同一張表內
        void                    PostBulletinMessage( _IN int classify, _IN uniqueid_t bulletin_gid, _IN cs_session_uid_t author_uid, _IN const _cs_char_t* title, _IN const _cs_char_t* content );
        // reply_bulletin_uid ==> 回覆哪一則留言
        void                    ReplyBulletinMessage( _IN int classify, _IN uniqueid_t bulletin_gid, _IN cs_session_uid_t author_uid, _IN uint64_t reply_bulletin_uid, _IN const _cs_char_t* title, _IN const _cs_char_t* content );

        // 公告有兩種取法
        // 01) 取得一個主留言內   所有的 回覆
        size32_t                GetBulletinCount( _IN int classify, _IN uniqueid_t bulletin_gid );
        // bulletin_adr, bulletin_gid, insert_time, update_time, update_count, member_uid, reply_bulletin_uid, title, content
        bool                    GetBulletinMessage( _OUT _cs_string_tableii& result_table, _IN int classify, _IN uniqueid_t bulletin_gid, _IN int limit_offset, _IN int limit_rowcount );
        bool                    DelBulletinMessage( _IN const _cs_char_t* bulletin_adr, _IN uniqueid_t bulletin_gid );

        // 02) 取得一個時間區段內 所有的 主留言 (發布的人)


    protected:

        //virtual XDatabase*      OnCreateDB( _IN CsDBType db_type );
        //virtual void            OnDeleteDB( _IN XDatabase* db );

    private:

        void                    CreateCommonDBTable();
        //void                    CreateExpondDBTable();

        static void             DoUpdate( void* data );
    };

}