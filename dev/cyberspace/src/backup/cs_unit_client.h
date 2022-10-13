//
// Commented as doxygen format by Wood Woo, Mar 8 2005
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
#include "CsDef.h"

#define MAX_PING_COUNT	5
#define PING_TIMEOUT	2000

typedef int		ClientId_t;

namespace Cyberspace
{

    class CoreClient;
    //class ServerRecord;
    //class MessageDispatcher;
    
    //typedef void (*CommandHandlingFunc)(Cyberspace::UnitClient* speer,
    //const char* bp, size32_t len);
    
    
    //! 一般的 Cyberspace client, 接收 Cyberspace 形式的命令
    class __SLAPI UnitClient// : private IO::TCPClient
    {
    private:
    
        friend class CoreClient;
        CoreClient*                m_pCoreClient;
        std::vector<ServerInfo*>   m_HostList;
        int                        m_SendMode;  // 0: direction, 1: post
        
    public:
    
        UnitClient();
        virtual ~UnitClient();
        
        // 送出程式版本
        virtual dword_t        OnGetVersion() = 0;

        void                   SetSessionId( sessionid_t cid );
        void                   ClearSessionId();
        
        //! 取代上層連線
        //virtual bool           Connect( ipa_t hname, unsigned short hport, ipa_t lname = NULL, unsigned short lport = 0 ); //
		virtual bool           Connect( ipa_t hname, unsigned short hport ); //
        
        virtual int            Shutdown( void );
        
        virtual	int            Update( void );
        
        bool                   RegisterChannel( channelid_t id, ClientMessageChannelFunction func );
        bool                   UnRegisterChannel( channelid_t id );
        
        //! 清空未發送訊息
        //int                    RefreshMessage(void);
        
        // 會把所有的訊息都送完才結束
        void                   SetCloseUntilSendFinish( bool b );
        int                    SendCsMessage( uniqueid_t who, channelid_t cid, const ubyte_t* bp, size32_t len );
        
        
        CsSessionDescriptor    GetSessionDescriptor( void );
        sessionid_t            GetSessionId( void );
        macn_t                 GetMACAddress( void );
        powerid_t              GetPowerId( void );
        void                   Ping( mstimen_t t );
        
        bool                   IsConnected( void );
        
    protected:
    
        virtual void           OnRecvUserDefRequest( sessionid_t cid, uniqueid_t who, channelid_t chid, const ubyte_t* command, size32_t len );
        virtual void           OnRecvDefineRequest ( sessionid_t cid, uniqueid_t who, channelid_t chid, const ubyte_t* command, size32_t len );
        virtual void           OnPing( mstimen_t t ) {};
        //virtual void           OnListServer( const ServerInfo* pServerNode, int mt ) {};
        virtual void           OnListServer() = 0;

        void                   ListServer();
        int                    GetServerCount();
        const ServerInfo*      GetServerInfo( int i );
        //! 取代上層功能
        //virtual void           OnConnect(void){};
        //! 取代上層功能
        //virtual void           OnKnockout(void){};
        //! 取代上層功能
        //! 被斷線以後會被喚起的 callback
        //virtual void           OnDisconnection(void){}
        
    protected:
    
        //void                   FreeConnect();
        int                    DoAssignMessage( const Evocator* evocator, channelid_t chid, const ubyte_t* command, unsigned int len );
        
        //void           SetConnectionId(sessionid_t cid);
        //void           SetPowerId(powerid_t pid);
        
        
    };
};


