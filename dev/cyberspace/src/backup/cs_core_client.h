#ifndef _CSCORECLIENT_H
#define _CSCORECLIENT_H

#include "CsDef.h"
#include "CstSDataDef.h"
//#include "CsCommBuf.h"
#include "MessageDispatcher.h"
#include "CsUnitClient.h"
#include "CsPacketClient.h"


#include <PfThread.h>
#include <PfStdlib.h>

#include <safevalue.inl>
#include <list>

namespace Cyberspace
{
    class commbuf_t;
    ///////////////////////////////////////////////////////////////////////////////////
    //
    class CoreClient: public PacketClient
    {
		Cyberspace::UnitClient*       m_Client;
        
        //_cstSession                 m_ConnectSymbol;
        powerid_t                     m_PowerId;
        groupid_t                     m_GroupId;
        
        
        T_Lock                        m_UtLock;
        T_Lock                        m_UpdateLock;
        
        //
        T_Lock                        m_UtrLock;
        std::list<commbuf_t*>         m_RecvList;
        
        //
        T_Lock                        m_UtsLock;
        std::list<commbuf_t*>         m_SendList;
        
        uint_t                        m_MaxSendSize;
        
        PfSystem::Thread::Handle      m_hSending;
        DWORD                         m_SendInterval;
        bool                          m_bSend;
        
        
        long                          m_new_command;
        int                           m_MessageStamp;
        
    private:
    
        T_Lock                        m_BannedLock;
        bool                          m_bBanned;
        MessageDispatcher             m_UserDefMessageDispatcher;
        
    public:
    
		CoreClient( Cyberspace::UnitClient* client );
		~CoreClient();

        void            SetSessionId( sessionid_t cid );
		sessionid_t     GetSessionId();
        void            ClearSessionId();
		macn_t          GetMACAddress();

		//powerid_t       GetPowerId();
		//groupid_t       GetGroupId();
		//void            BindSession( powerid_t pid, int gid );

		void            ToSession( _cstSession& session );
		//void            PassSession( CoreClient& c );
        
		//bool            Connect( const tchar_t* hname, unsigned short hport, const tchar_t* lname = NULL, unsigned short lport = 0 );
		bool            Connect( const tchar_t* hname, unsigned short hport );
        // 0: 被呼叫  1:主動呼叫
		int             Shutdown( int a = 0 );

		int             Update( void );

		int             SendPacket( uniqueid_t eid, unsigned short cid, const ubyte_t* bp, size32_t len );
		int             RecvPacket( uniqueid_t& eid, unsigned short& cid, ubyte_t** bp, size32_t& len );
		void            PostSendPacket( uniqueid_t who, channelid_t cid, const ubyte_t* bp, size32_t len );
		void            BeginSendTask();
		bool            RegisterChannel( channelid_t id, ClientMessageChannelFunction func );
		bool            UnRegisterChannel( channelid_t id );
		int             DispatchMessage( const Evocator* evocator, channelid_t chid, const ubyte_t* command, size32_t len );
		bool            GetBanned();
		void            SetBanned( bool b );
        // 可調參數:
        //! retry_count: 要嘗試幾次? (default: SHORT_MAX)
        //! retry_time: 每次重試間隔: (default:4000)
        short                         retry_count, retry_time;
        safevalue<bool>               m_bCloseUntilSendFinish;
        
    private:
    
        //
        virtual void           OnReceive();
        //virtual void           OnDisconnection(void);
        
        //! 取代上層功能
        //virtual bool           Connect(tchar_t* hname, unsigned short port);
        virtual int            Close( void );
        
        // this is read connect
        //void                   Clear(void);
        
        static void __cdecl    DoSendTask( void* );
        
        
        // 0 是清空 其他是 要收幾個
        void                   ReflashRecv( int cnt );
        //
        // 0 是清空 其他是 要送幾個
        void                   ReflashSend( int cnt );
        
        void                   ClearRecvList();
        void                   ClearSendList();
        
        
    };
    

}

#endif  //_CSCORECLIENT_H