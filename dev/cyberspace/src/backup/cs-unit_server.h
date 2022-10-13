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

#pragma warning (disable : 4786)

#define __NUBLOCKLOCK

#include <map>
#include <pf-thread.h>
#include <t_lock.h>
#include "cs-def.h"


namespace cyberspace
{
    class CoreServer;
    class UnitServer;

    typedef int             ( UnitServer::*unit_member_func )( _IN const Unit::Evocator* evocator, _IN const ubyte_t* combin, _IN unsigned int comlen );

    class __SLAPI UnitServer
    {
    private:
        friend class CoreServer;

        _tstring                name_class_;
        _tstring                name_alias_;         // alias name
        _tstring                name_service_;  // function name

        CoreServer*             v_coreserver_;
        //std::vector<XDatabase*>   database_[ 2 ];

        //pfkrl::PfSytem::Thread::Handle   BeginProcess(func_address func, void* data);

    protected:

        virtual int             StartUp( _IN uint_t allowplayers, _IN ushort_t port );
        int                     DoDispatchMessage( _IN const Unit::Evocator* evocator, _IN channel_uid_t chid, _IN const ubyte_t* command, _IN unsigned int len );

    public:

        UnitServer( const tchar_t* name = NULL );
        ~UnitServer();

        //====================================================================
        // User define callback function
        virtual dword_t         OnGetVersion() = 0;

        // on join one server in server group
        virtual void            OnJoinSection( _IN CsSectionDescriptor sid ) {}

        // on update one server in server group
        //virtual void     OnUpdateServer(cs_section_uid_t sid){}

        // on exit one server in server group
        virtual void            OnExitSection( _IN CsSectionDescriptor sid ) {}

        // 自行對連線上來的client 做驗證
        virtual bool            OnClientAuthenication( _IN cs_session_uid_t sid ) { return true; }

        // when clinet successful 用來初始化 client
        virtual void            OnClientConnect( _IN CsSessionDescriptor sdp ) {};

        // when clinet disconnect
        virtual void            OnClientKickout( _IN CsSessionDescriptor sdp ) {};

    protected:
        // when server startup and do any server initialing function in this
        virtual int             OnServerInitialing( void ) { return 1; }

        // when server shutdown and do any server initialing function in this
        virtual void            OnServerShtudown( void ) {};

        // when receive server message
        //virtual void            OnRecvServerRequest( _IN cs_section_uid_t sid, _IN channel_uid_t cid, _IN const ubyte_t* command, _IN cs_size_t len ) {};

        // when receive system message
        virtual int             OnRecvDefineRequest( _IN CsSessionDescriptor sdp, _IN uniqueid_t who, _IN channel_uid_t cid, _IN const ubyte_t* command, _IN cs_size_t len );

        // set server can't be see by other server
        virtual Pflag           OnGetState( void ) { return 0; }


    protected:

        // if return false server will stop
        virtual bool            OnLoadConfigure( _IN std::map<_tstring, _tstring> setting ) { return true; }
        // 主要的Update呼叫
        // 會再Update執行中被呼叫
        virtual void            OnExecute( void ) {};
        //virtual void            OnLoadConfigure( const tchar_t* argument ) {};


    public:

        //====================================================================
        //
        virtual const tchar_t*  ObjectName() const = 0;// { return "CsServer"; }
        //virtual void            SetName( const tchar_t* name ) { name_class_ = name; };
        virtual const tchar_t*  GetName() { return name_class_.c_str(); }

        virtual bool            LoadConfigure( _IN const tchar_t* filename );
        //! close server if no error, return 0
        virtual int             Shutdown( void );
        int                     Update( void );


        //====================================================================
        const tchar_t*          GetLocalHostname( _IN tchar_t* buf, _IN cs_size_t len );  // 取的本機端的IP
        ushort_t                GetLocalServicePort( void );                               // 開啟接收的Port編號
        const tchar_t*          GetLocalAliasName( void );                                 // 取得本機的別名
        const tchar_t*          GetLocalServiceName( void );                               //  設定檔中設定的Service名稱
        uint_t                  GetLocalPollingCount( void );
        cs_section_uid_t        GetLocalServerUid( void );

        uint_t                  GetLocalLimitConnectSize( void );                          // maximum for allow connect client size
        uint_t                  GetLocalConnectedSize( void );                             // size of current connected client
        uint_t                  GetLocalMaxConnectedSize();

        uint_t                  GetSumRecv( void );                                        // sum of receive message
        uint_t                  GetLongProc( void );                                       // sum of process time(over 1 sec)

        void                    SetThreadHistroy( bool t );                                // 取得目前各序的狀態
        cs_size_t               GetThreadHistory( _OUT std::vector<channel_uid_t>& chrd, _OUT std::vector<mstimen_t>& tmrd );

        _tstring                GetSectionName( _IN CsSectionDescriptor ssectiondp );
        //! 是否停止
        bool                    IsActive( void );

        // set server can't be see by other server
        //void                    SetState( _IN Pflag v );
        //Pflag                   GetState( void );


        bool                    RegisterUserDefChannel( _IN channel_uid_t id, _IN unit_member_func func );                  // register the user define message channel
        bool                    UnRegisterUserDefChannel( _IN channel_uid_t id );                                           // unregister the user define message channel

        void                    GetAllChannelId( _OUT std::vector<channel_uid_t>& channellist );


        stimen_t                GetStartSec( void );                                                                      //
        stimen_t                GetSyncSec( void );                                                                       //先改為標準函式中
        void                    SetLogoutSec( _IN  stimen_t t );                                                          //設定登出後要等待的間隔時間
        //mstimen_t     GetSyncTime     (void);                                                                           // 取得跟 所有機器同步的時間

    public:

        bool                    CreateSession( _IN cs_session_uid_t session_uid, _IN stimen_t lifttime = 180 );                     // 建立一個seesion 在各server中
        bool                    DeleteSession( _IN cs_session_uid_t session_uid );
        ////////
        CsSessionDescriptor     GetSessionDescriptor( _IN cs_session_uid_t session_uid );                                           //get socket by Member id
        CsSessionDescriptor     GetSessionDescriptor( _IN const tchar_t* name );

        CsSessionDescriptor     GetLocalSessionDescriptor( _IN socket_t skt );
        void                    GetAllLocalSessionDescriptor( _OUT std::vector<CsSessionDescriptor>& CsPSockConnlist );

    public:

        static cs_session_uid_t GetSessionSessionUid( _IN CsSessionDescriptor sdp );
        static ipn6_t           GetSessionIpn6( _IN CsSessionDescriptor sdp );
        static ipa_t            GetSessionIpa( _IN CsSessionDescriptor sdp );
        static ushort_t         GetSessionPort( _IN CsSessionDescriptor sdp );
        static cs_section_uid_t GetSessionServerUid( _IN CsSessionDescriptor sdp );
        static stimen_t         GetConnectCreateTime( _IN CsSessionDescriptor sdp );
        static socket_t         GetConnectSocketId( _IN CsSessionDescriptor sdp );

        static bool             GetSessionIsMaster( _IN CsSessionDescriptor sdp );
        static bool             GetSessionIsClone( _IN CsSessionDescriptor sdp );
        static bool             IsLive( _IN CsSessionDescriptor sdp );
        static bool             IsKickout( _IN CsSessionDescriptor sdp );

        mstimen_t               GetLastRecvMST( _IN CsSessionDescriptor sdp );
        cs_size_t               GetConnectSize( _IN CseCONNECT_TYPE t );                         // 1代表取得Master connect 2代表取得Clone
        channel_uid_t           GetStageChannel( _IN CsSessionDescriptor sdp );                  // 似乎是除錯用的 
        void                    GetLocalServerReport( _OUT cyberspace::ServerState& sinfo );


    public:

        //kick out a connected client
        virtual void            Kickout( _IN CsSessionDescriptor sdp ) final;

        //send message to some client
        virtual int             SendCsMessage( _IN CsSessionDescriptor sdp, _IN uniqueid_t uid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len );
        void                    BroadcastMessage( _IN uniqueid_t uid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len );



    public: // paper server


        _tstring                PaperSet( _IN _tstring key, _IN _tstring val, _IN int live_sec = 0 );    // set value into paper server if exist return old key
        _tstring                PaperSetnx( _IN _tstring key, _IN _tstring val, _IN int live_sec = 0 );  //  @val: return current value
        _tstring                PaperGet( _IN _tstring key );                                            // return:  1 : success, 0 : no any value, -1 : value over time  
        _tstring                PaperDel( _IN _tstring key );                                            //  @val: return delete value

    protected:

        void                    SetBanClient( _IN CsSessionDescriptor sdp, _IN bool bBlock );            // 請CLIENT 暫停送資料
        int                     GetSCSServerSize();                                                      // 取得叢集伺服器的數量
        void                    PublishServerList( _IN CsSessionDescriptor sdp );

    public:

        void                    SetKeepAliveMstime( _IN mstimen_t mst );                                // 啟動連線逾時的計時  連線上來多久沒有驗證 就踢除
        //void                    SetRiskAddress( _IN int b );                                             // 阻擋高風險IP
        void                    InsertAllowRemotePort( _IN ushort_t port );
        void                    BeginUpdateOperator( _IN mstimen_t framecycle, _IN void* opvalue );

    protected:

        virtual bool            OnQueryUpdateOperator( _IN frame_t frame, _IN void* opvalue ) { return true; }  // return  回傳false時該呼叫元件會被移除

    public:

        static CsLogMode        GetLogMode();
        static void             SetLogMode( _IN CsLogMode m );
        // asset log message in log file and termi
        void                    OutputMessage( _IN CsLogMode m, _IN const tchar_t* format, ... );

    public:

        // register the scs define message channel
        //bool                    SCSRegisterUserDefChannel( _IN channel_uid_t id, _IN cyberspace::scs::ServerAbtpChannelFunction func );
        // unregister the scs define message channel
        //bool                    SCSUnRegisterUserDefChannel( _IN channel_uid_t id );
        // send message to other server machine
        int                     SCSSendMessage( _IN cs_section_uid_t receiver, _IN uniqueid_t uid, _IN  channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len );
        // Broadcast message to other server machine
        int                     SCSBroadcastMessage( _IN uniqueid_t uid, _IN  channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len );
        // Broadcast message to other server machine beside me
        int                     SCSBroadcastMessageToOther( _IN uniqueid_t uid, _IN channel_uid_t cid, _IN  const ubyte_t* bp, _IN cs_size_t len );
        // Broadcast message to other server machine beside me and invisible server
        int                     SCSBroadcastMessageToOtherAndVisible( _IN uniqueid_t uid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN cs_size_t len );
        // Broadcast message to other server machine beside me and same class name server
        int                     SCSBroadcastMessageToOtherWithObject( _IN const tchar_t* objectname, _IN uniqueid_t uid, _IN channel_uid_t cid, _IN  const ubyte_t* bp, _IN cs_size_t len );
        // get sectionid
        cs_section_uid_t        SCSGetServerUid( _IN const tchar_t* servername );

        //void                    SCSGetAllServerInfo( std::vector<cyberspace::ServerInfo>& serverint_list );


        // 建立一個local scs buffer
        //cyberspace::scs::BufferDescriptor CreateShareBuffer(uniqueid_t uid, uint8_t gid, const ubyte_t* buff, cs_size_t len);
        // 刪除一個scs buffer
        //bool        DeleteShareBuffer(const cyberspace::scs::BufferDescriptor* sbmm);

    private:

        int                     DoPing( _IN const Unit::Evocator* evocator, _IN const ubyte_t* combin, _IN unsigned int comlen );
        int                     DoListServer( _IN const Unit::Evocator* evocator, _IN const ubyte_t* combin, _IN unsigned int comlen );
        //static  int             DoPing( const Unit::ServerEvocator* evocator, const void* combin, unsigned int len );
        //static  int             DoListServer( const Unit::ServerEvocator* evocator, const void* combin, unsigned int len );

    };
};