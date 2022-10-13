#pragma once

#include "cs-def.h"
//#include <net-server.h>


namespace cyberspace
{
    enum TPType
    {
        TT_ABTP = 1,
        TT_HTTP = 2
    };

    typedef int                    ( AppServer::*cptp_member_func )( _IN AppSocket* s, _IN CsAbtpEncType enctype, _IN const ubyte_t* combuf, _IN uint_t comlen );
    typedef _csbinbuf              ( AppServer::*http_member_func )( _IN AppSocket* s, _IN CsHttpEncType enctype, _IN const ubyte_t* combuf, _IN uint_t comlen );

    typedef _csbinbuf              ( AppServer::*member_func )( _IN AppSocket* s, _IN CsAbtpEncType enctype, _IN const ubyte_t* combuf, _IN uint_t comlen );

    class AppServer
    {
        _tstring                   name_class_;
        _tstring                   name_alias_;    // alias name
        _tstring                   name_service_;

        stimen_t                   stime_startup_;
        //stimen_t                   sec_session_timeout_;
        //stimen_t                   m_ClearTempSec;  // clear temp file? nohup.out...

        vclass_t                   v_object_[3][ 5 ];  // [0]: 工具類別  [1]: 協定server

    protected:

        void                       RegisterFunction( _IN _tstring actstr, _IN member_func func );
        void                       UnRegisterFunction( _IN _tstring actstr );
        
        void                       RegisterAbtpFunction( _IN channel_uid_t id, _IN cptp_member_func func );
        void                       UnRegisterAbtpFunction( _IN channel_uid_t id );          // unregister the user define message channel

        void                       RegisterHttpFunction( _IN _tstring actstr, _IN http_member_func func );
        void                       UnRegisterHttpFunction( _IN _tstring actstr );


    public:

        AppServer( const tchar_t* name = NULL );
        ~AppServer();

        const tchar_t*             ObjectName() const { return _T( "AppServer" ); }
        const tchar_t*             GroupName()  const { return _T( "ServerGroup" ); }

        virtual int                Shutdown( void );

        bool                       IsAlive();

        void                       SetKeepAliveMstime( stimen_t t );      // 設定最短連線安全檢查時間

        stimen_t                   GetStartSec( void );                    // 取得啟動的時間
        const tchar_t*             GetLocalHostname( _IN tchar_t* buf, _IN cs_size_t len );
        ushort_t                   GetLocalServicePort( _IN int m );       // 開啟接收的Port編號 0 cptp 1 http

        const tchar_t*             GetLocalAliasName( void );              // 取得本機的別名
        const tchar_t*             GetLocalServiceName( void );            // 設定檔中設定的Function名稱

        uint_t                     GetLocalPollingCount( void );
        uint_t                     GetLocalLimitConnectedSize( void );
        uint_t                     GetLocalConnectedSize( _IN int m );     // size of current connected client
        uint_t                     GetLocalMaxConnectedSize( _IN int m );
        long32_t                   GetLivingThreadSize( void );

        //                         
        virtual bool               LoadConfigure( _IN const tchar_t* filename );

        int                        SendMessage( _IN AppSocket* skt, _IN uniqueid_t eid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN uint_t len );

        //  輸出畫面 與寫在本地日誌檔裡
        void                       OutputMessage( _IN CsLogMode m, _IN const tchar_t* format, _IN ... );

        void                       BindSessionUid( _IN AppSocket* skt, _IN cs_session_uid_t sid );
        void                       FreeSessionUid( _IN AppSocket* skt );
        cs_session_uid_t           GetSessionUid( _IN AppSocket* skt );
        //AppSocket*                 GetSessionSocket( _IN cs_session_uid_t sid );


        // 設定seesion的 timeout 秒數超過這時間沒有使用 就會被刪除
        //void                       SetSessionTimoutSec( _IN cs_section_uid_t sessionid, _IN stimen_t sec );

        // 建立一個session
        bool                       CreateSession( _IN cs_session_uid_t sessionid, _IN stimen_t timeoutsec = 900 );
        void                       RemoveSession( _IN cs_session_uid_t sessionid );
        // 設定一個 seesionid 的 key 跟 val,如果原先sessionid 不存在 會建立新的
        bool                       SetSessionKey( _IN cs_session_uid_t sessionid, _IN _tstring key, _IN _tstring val );
        _tstring                   GetSessionKey( _IN cs_session_uid_t sessionid, _IN _tstring key );
        void                       DelSessionKey( _IN cs_session_uid_t sessionid, _IN _tstring key );

        //
        // return content type
        void                       SetHttpContentType( const char* buf );

    protected:

        virtual int                StartUp( _IN unsigned int allowplayers, _IN ushort_t ctptport, _IN ushort_t httpport );


        void                       OnAccept( _IN AppSocket* socket ) {};

        void                       OnKickout( _IN AppSocket* socket ) {};

        // if return false server will stop
        virtual bool               OnLoadConfigure( _IN std::map<_tstring, _tstring> setting ) { return true; }

        virtual int                OnServerInitialing( void ) { return 1; }

        // 無效傳回 CsINVALID_ID
        virtual cs_session_uid_t   OnAuthenication( _IN cyberspace::TPType tptype, _IN const ubyte_t* combin, _IN uint_t comlen ) { return 1; }

        // 放在呼叫function 之前能讓client 送上來的資料 作加密 
        virtual channel_uid_t      OnCallAbtpRequest( _IN AppSocket* s, _IN channel_uid_t channelid );
        virtual const char_t*      OnCallHttpRequest( _IN AppSocket* s, _IN const char_t* actstr );

    private:

        int                        OnRecvAbtpRequest( _IN AppSocket* s, _IN CsAbtpEncType enctype, _IN channel_uid_t channelid, _IN const ubyte_t* combin, _IN uint_t comlen );
        _csbinbuf                  OnRecvHttpRequest( _IN AppSocket* s, _IN CsHttpEncType enctype, _IN const char_t* actstr, _IN const ubyte_t* comstr, _IN uint_t comlen );

        static void                Execute( void* data );
    };
};