// Arbitration server class
//
// Commented as doxygen format by Wood Woo on Mar 8, 2005
//
//
//
//
//
//
//
//
//
//        Pin
//           2004/5/1
//////////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning(disable:4786)

//01.== C 系統文件/C++ 系統文件
#include <map>
#include <set>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <net/net-server.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"


namespace cyberspace
{
    //! Arbitration server: 整個系統的管理類別
    //! 此伺服器可以監控各種伺服器的狀態, 並提示 client 如何連線最好, 唯一系統的核心伺服器
    class ArbitrationServer: public net::TCPServer//, public MessageDispatcher
    {
        T_Lock                             arbitrationserver_ut_lock_[ _MAX_BUF ];
        stimen_t                           stime_startup_;
        CsLogMode                          def_log_mode_;
        typedef std::set<net::TCPSocket*>  SockeList;
        std::map<_cs_string, SockeList>    host_bank_;
        
    public:
    
        ArbitrationServer();
        ~ArbitrationServer();
        
        const _cs_char_t*          ObjectName() const { return ( "ArbitServer" ); }
        
        virtual int                StartUp( unsigned int allowplayers, unsigned short port );
        
        int                        Shutdown( void );
        
        //! 載入設定檔
        virtual bool               LoadConfigure( const _cs_char_t* filename );
        
        //! get arbit-node machine information
        const CsSectionDescriptor  GetHostInfo( net::TCPSocket* skt );
        
        
        //aset log message in log file and termi
        void                       OutputMessage( CsLogMode m, const _cs_char_t* format, ... );
        //void            SetLogMode( CsLogMode m );   //開關輸出訊息
        //CsLogMode       GetLogMode( void );          //開關輸出訊息
        
        int                        SendMessage( net::TCPSocket* skt, uniqueid_t who, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len );
        int                        RecvMessage( net::TCPSocket* skt, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bp, _cs_size_t& len );
        
    private:
    
        // 取得跟 所有機器同步的時間
        // 以Arbitration為主 啟動到現在的時間
        //mstimen_t   GetSyncTime(void);
        stimen_t                   GetSyncSec( void );
        
        //! 同步 S 的節點資料
        void                       NotifySyncSection( net::TCPSocket* skt );
        
        //! send all exist host information to host s
        void                       NotifyJoinSection( net::TCPSocket* skt );
        
        //! publish host s mount
        //void            PublishMountHost( CsSectionEntity* skt );
        //! publish host s dismount
        void                       NotifyExitSection( net::TCPSocket* skt );
                                
        //! 取代上層的對應         callback
        //virtual void           OnInitiate(void);
        //! 取代上層的對應 callback
        virtual void               OnAccept( net::TCPSocket* skt );
        //! 取代上層的對應 callback
        virtual void               OnReceive( net::TCPSocket* skt );
        //! 取代上層的對應 callback
        virtual void               OnKickout( net::TCPSocket* skt );
        //! 取代上層的對應 callback
        //virtual void           OnShutDown(void);
        //! kickout one client
        //virtual int             Kickout( net::TCPSocket* s );
        
        virtual net::TCPSocket*    OnNewSocket( void );
        
        virtual void               OnDelSocket( net::TCPSocket* sk );
        
    };
};