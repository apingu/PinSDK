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
#ifndef CSLOGINSERVER_H
#define CSLOGINSERVER_H

#pragma warning(disable:4786)

#include <set>
#include <CsErr.h>
#include <Cyberspace_server.h>
#include "CsDef.h"
//#include "UtCriticalSection.h"


namespace Cyberspace
{
    class LoginField;
    
    class LoginServer : public Server
    {
        LoginField*   m_pLoginField;
        
    public:
        LoginServer();
        ~LoginServer();

        virtual DWORD           OnGetVersion() = 0;

        const tchar_t*          ObjectName() const { return "LoginServer"; }
        virtual int             Shutdown();

        // register the user define message channel
        bool                    RegisterUserDefChannel( channelid_t id, ServerMessageChannelFunction func );
        // unregister the user define message channel
        bool                    UnRegisterUserDefChannel( channelid_t id );

        void                    SetVoidConnectCycle( mstimen_t t );
        uint_t                  GetTotalConnectCount( void );
        uint_t                  GetTotalLoginCount( void );
        uint_t                  GetTotalLoginErrorCount( void );
        uint_t                  GetTotalLoginUnknowErrorCount( void );
        uint_t                  GetTimeoutCount( void );

    protected:
        ////////////////////////////////////////////////////////////////////
        //call back function
        // 收到封包後進行查驗 login, 失敗傳回負值(原因碼)
        virtual CSERR_CODE      OnVerifyLogin( const tchar_t* username, const tchar_t* password );

        // after login successful
        // 給使用者自行判斷的部份(收費機制)
        virtual CSERR_CODE      OnLogin( sessionid_t acc, CSERR_CODE retcode ) { return retcode; }

        virtual sessionid_t     OnLink( const tchar_t* linkdata, tchar_t* namebuff, size_t bufflen,
                                        powerid_t& pwd, macn_t& mac, long& flag, tchar_t* retbuf, size_t buflen ) = 0;

    protected:
        //繼承自父類別
        virtual XDataBase*      OnCreateCommonDB( void );
        virtual void            OnDeleteCommonDB( XDataBase* db );
        
        virtual XDataBase*      OnCreateCustomDB( void );
        virtual void            OnDeleteCustomDB( XDataBase* db );


    private:
        ///////////////////////////////////////////////////////////////////////////////
        // Post request
        
        static int              DoLogin( const ServerEvocator* evocator, const void* pData, unsigned int len );
        static int              DoLogout( const ServerEvocator* evocator, const void* pData, unsigned int len );
        static bool             DoLink( const ServerEvocator* evocator, const void* pData, unsigned int len );

    private:

        //virtual void  OnServerInitialing(void);
        //virtual void  OnServerShtudown(void);
        virtual int             StartUp( uint_t allowplayers, unsigned short port );

        // when receive system message
        //bool    DoVerifyJoinConnection(CsSessionDescriptor sdp);
        //void    DoVerifyExitConnection(CsSessionDescriptor sdp);
        void                   OnClientConnect( CsSessionDescriptor sdp );
        void                   OnClientDisconnect( CsSessionDescriptor sdp );
        void                   OnExecute( void );

        //static BOOL   GetHostNodeList(unsigned int order, _cstSERVERSYMBOL* pHost, void* pData);
        
        // register the user define message channel
        //bool    RegisterUserDefChannel(channelid_t id, ServerMessageChannelFunction func);
    };
};



#endif