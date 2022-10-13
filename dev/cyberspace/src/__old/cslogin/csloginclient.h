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
#ifndef CSLOGINCLIENT_H
#define CSLOGINCLIENT_H

#include "CsDef.h"
#include "CsErr.h"
#include "Cyberspace_client.h"


namespace Cyberspace
{
    class LoginClientDelegate;
    
    class LoginClient : public Client
    {
        CSERR_CODE             m_Loginstate;
        stimen_t               m_TimeoutBegin;
        stimen_t               m_TimeoutSec;
        
        char                   m_retbuf[512];
        
        LoginClientDelegate*   m_gc;
        
    public:
    
        LoginClient();
        ~LoginClient();
        
        
        void           Create( LoginClientDelegate* gc );
        
        DWORD          OnGetVersion();
        
        //void           ClearState();
        
        virtual bool   Connect( ipa_t host, unsigned short port ); //
        virtual int    Shutdown( void );
        
        //! step 1: login and wait for OnLogin get TRUE...
        CSERR_CODE     Login( const char_t* username, const char_t* password );
        
        // login but not wait, get result form GetLoginState
        void           AsynLogin( const char_t* username, const char_t* password );
        CSERR_CODE     GetLoginState( void );
        
        
        CSERR_CODE     Link( const tchar_t* linkdata );
        void           AsynLink( const tchar_t* linkdata );
        const char*    GetRetBuff();
        
        
        //void           Logout(void);
        
        
        //! 通知SERVER 角色選擇完畢
        //void           Ready(void);
        
        
        /*
        void           ClearHost(void){ m_HostList.clear();}
        
        unsigned int	       GetHostCount();
        
        
        //! 取得註冊的伺服器資訊 @return 是否有資料
        bool	       GetHostInfo(const char* hostname,
        							int &n,
        							ipa_t ipa, ushort_t &port,
        							char* servername,
        							char* functionname,
        							unsigned int &maxconnect, unsigned int &usedconnect, int &load);
        
        bool	       GetHostInfo(int n,
        							char* hostname,
        							ipa_t ipa, ushort_t &port,
        							char* servername,
        							char* functionname,
        							unsigned int &maxconnect, unsigned int &usedconnect, int &load);
        						*/
        
        void           SetTimeoutTime( stimen_t t );
        
    protected:
    
    
        // 		// server指派要加入的
        // 		virtual void   OnAssingHost (ipa_t host, ushort_t port,
        // 									  const char* hostname,
        // 									  const char* servername,
        // 									  const char* functionname,
        // 									  unsigned int maxconnect, unsigned int usedconnect){};
        
        
        
        virtual void   OnRecvDefineRequest( sessionid_t cid, uniqueid_t eid, channelid_t chid, const byte_t* command, size32_t len );
        
        virtual void   OnListServer( const ServerRecord* pServerNode, int mt );
        
        
        //! 取代上層功能
        //virtual void OnConnect(void){};
        
        //! 取代上層功能
        //virtual void OnKnockout(void);
        //! 取代上層功能
        //! 被斷線以後會被喚起的 callback
        //virtual void OnDisconnection(void);
        
        
    };
};



#endif //#ifndef CSLobbyClient_H