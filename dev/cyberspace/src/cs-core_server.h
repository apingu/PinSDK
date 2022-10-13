//
//
//
//
//  server 素體  以此為基礎衍生出各種不同的 包裝(通訊協定 parser)
//
//
//
//
//
#pragma once

#include "cs-def.h"

//#include <net-server.h>


namespace cyberspace
{
    class CoreServer;

    // 一個 client 端的連線 就會有一個 CoreConnector
    class CoreConnector
    {
        friend CoreServer;
        vclass_t                   v_object_; // client socket
    public:

        const _cs_char_t*          ObjectName() const { return ( "CoreConnector" ); }
    };

    
    class CoreServer
    {
        _cs_string                 name_class_;    // 建構時設定的 唯一名稱 對應到 cfg 的 name ( 對應 cfg 的設定是 屬於該 物件 )
        _cs_string                 name_alias_;    // alias name
        _cs_string                 name_service_;
        //vclass_t                 v_object_[3][ 5 ];  // [0]: 工具類別  [1]: 協定server
        vclass_t                   v_object_[ 5 ]; // 0: CoreJunction
    public:

        CoreServer( _IN const _cs_char_t* name = NULL );
        ~CoreServer();

        //
        typedef _csbinbuf( CoreServer::*OnMemberFunction )( _IN CoreConnector* s, _IN const ubyte_t* combuf, _IN uint_t comlen );


        virtual const _cs_char_t*  GroupName() const { return ( "ServerGroup" ); }     // configure file 裡 XML <XXXXX> 會對應讀該名稱
        virtual const _cs_char_t*  ObjectName() const { return ( "CoreServer" ); }     // configure file 裡 XML <XXXXX> 會對應讀該名稱
         

        virtual int                Shutdown( void );

        bool                       IsAlive();

        void                       SetKeepAliveMStime( _IN mstimen_t mst );       // 設定最短連線安全檢查時間
        void                       SetUpdateIntervalMSTime( _IN mstimen_t mst );  // OnUpdate 觸發的時間間隔

        stimen_t                   GetStartSec( void );                    // 取得啟動的時間
        const _cs_char_t*          GetLocalHostname( _IN _cs_char_t* buf, _IN _cs_size_t len );
        ushort_t                   GetLocalServicePort( void );            // 開啟接收的Port編號 0 cptp 1 http

        const _cs_char_t*          GetLocalAliasName( void );              // 取得本機的別名
        const _cs_char_t*          GetLocalServiceName( void );            // 設定檔中設定的Function名稱

        uint_t                     GetLocalPollingCount( void );
        uint_t                     GetLocalLimitConnectedSize( void );
        uint_t                     GetLocalConnectedSize( void );          // size of current connected client
        uint_t                     GetLocalMaxConnectedSize( void );
        long32_t                   GetLivingThreadSize( void );

        //                         
        virtual bool               LoadConfigure( _IN const _cs_char_t* filename );

        //  輸出畫面 與寫在本地日誌檔裡
        void                       OutputMessage( _IN CsLogMode m, _IN const _cs_char_t* format, _IN ... );

        //CoreConnector*                 GetSessionSocket( _IN cs_session_uid_t sid );

        // 設定seesion的 timeout 秒數超過這時間沒有使用 就會被刪除
        //void                       SetSessionTimoutSec( _IN cs_section_uid_t sessionid, _IN stimen_t sec );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // 建立一個session
        // 這邊名子取的有點爛 跟 core session 不是同一個東西 類似 php的 session
        bool                       InsertSession( _IN cs_session_uid_t sessionid, _IN stimen_t timeoutsec = 900 );
        void                       DeleteSession( _IN cs_session_uid_t sessionid );
        // 設定一個 seesionid 的 key 跟 val,如果原先sessionid 不存在 會建立新的
        bool                       SetSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key, _IN _cs_string val );
        _cs_string                 GetSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key );
        void                       DelSessionKey( _IN cs_session_uid_t sessionid, _IN _cs_string key );

        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //virtual void               SetSessionUID( _IN CoreConnector* session, _IN cs_session_uid_t sid ) {};
        //virtual cs_session_uid_t   GetSessionUID( _IN CoreConnector* session ) { return CsINVALID_SESSIONID; };
        void                       Kickout( _IN CoreConnector* session );
    
    protected:

        // 註冊function
        // is_auth: 需不需要經過 OnAuthenication
        bool                       RegisterFunction( _IN channel_uid_t ch_uid, _IN bool is_auth, _IN OnMemberFunction func );
        void                       UnRegisterFunction( _IN channel_uid_t ch_uid );
        _csbinbuf                  DoFunction( _IN CoreConnector* session, _IN channel_uid_t ch_uid, _IN const ubyte_t* com_buf, _IN uint_t com_len );

        int                        SendMessage( _IN CoreConnector* session, _IN const ubyte_t* bp, _IN uint_t len );
        size32_t                   GetRecvLength( _IN CoreConnector* session );
        int                        RecvMessage( _IN CoreConnector* session, _IN ubyte_t* bp, _IN uint_t len ); 

        virtual const _cs_char_t*  OnVersion() = 0;

        virtual int                OnServerInitialing() { return 1; }

        // 自行建立自己的seesion 物件
        virtual CoreConnector*     OnCreateConnector() { return new CoreConnector; };
        // 自行消滅自己的seesion 物件
        virtual void               OnDestoryConnector( _IN CoreConnector* session ) { SAFE_DELETE( session ); }

        virtual void               OnAccept( _IN CoreConnector* session ) {};

        virtual void               OnKickout( _IN CoreConnector* session ) {};

        // if return false server will stop
        virtual bool               OnLoadConfigure( _IN std::map<_cs_string, _cs_string> setting ) { return true; }

        // 在 call function 作為驗證該連線是否可以成立 
        // @驗證成功: 驗證通過就傳 _csbinbuf() 
        // @驗證失敗: 就 會傳 回傳失敗值
        virtual _csbinbuf          OnAuthenication( _IN CoreConnector* session ) { return _csbinbuf(); }

        virtual int                OnRecvMessage( _IN CoreConnector* session ) = 0;

        // 連續呼叫的update 傳入經過的 毫秒
        virtual void               OnUpdate( mstimen_t update_time ) {};

        // 放在呼叫function 之前能讓client 送上來的channel編號 作加密 
        //virtual const _cs_string     OnCallRequest( _IN CoreConnector* s, _IN const char_t* channel_name ) { return channel_name; }

    private:

        void                       SetConnectorObject( _IN CoreConnector* session, _IN vclass_t* obj );
        static void                Execute( _IN void* data );
    };
};