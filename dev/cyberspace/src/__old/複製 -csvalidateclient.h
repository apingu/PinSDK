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
#ifndef CSVALIDATECLIENT_H
#define CSVALIDATECLIENT_H

#include "CsDef.h"
#include "CsErr.h"
//#include "Cyberspace_client.h"
#include <T_Lock.h>
#include "CsGameClient.h"

namespace Cyberspace
{

	struct Host_t
	{
		char	_ip[64];
		u_short _port;
		char	_hostname[128];
		char	_servicename[128];
		u_int	_maxconnect;
		u_int	_usedconnect;

		Host_t()	{	memset(this, 0, sizeof(Host_t));	}
		Host_t(const char* ip, const u_short port,
			const char* hostname, 
			const char* servicename, 
			u_int maxconnect, u_int usedconnect)
		{		strcpy(_ip, ip);	_port = port;	strcpy(_hostname, hostname);
			strcpy(_servicename, servicename);	_maxconnect = maxconnect;	_usedconnect = usedconnect;		}
		Host_t(const Host_t& rhs)
		{		memcpy(this, &rhs, sizeof(Host_t));		}
	};

		
	class ValidateClient : public Client
	{
		T_Lock       m_UtVaLock;
		CSERR_CODE   m_Loginstate;
		GameClient*  m_GameClient;

		std::vector<Host_t> m_HostList;

	public:

		ValidateClient( GameClient*  client );
		~ValidateClient();

		virtual bool   Connect( char* host, unsigned short port ); //
		
		//! step 1: login and wait for OnLogin get TRUE...
		void           Login( const char* username, const char* password );
		CSERR_CODE     GetLoginState( void );

		void           Logout( void );
		
	    //! step 2: get all entity
		bool           GetAllEntity( void );
		//! step 3: you can create entity,delete one,or select one.
		//			once selected, and OnGetSelectEntityId() called, game started.

		//! 創角
		void           CreateEntity( const char* name, void* prop, size_t len );
		//! 消滅角色
		void           DeleteEntity( const char* name );
		//! 通知SERVER 要選擇一個角色使用
		void           SelectEntity( const char* name );
		

		
		void           ClearHost( void ){ m_HostList.clear();}


		u_int	       GetHostCount();
		
	    //! 取得註冊的伺服器資訊 @return 是否有資料
	    bool	       GetHostInfo( const char *hostname,
                                    int &n,
		                            ipa_t host, u_short& port,
							        char* servicename,
							        u_int& maxconnect, u_int& usedconnect, int& load );

	    bool	       GetHostInfo( int n,
		                            ipa_t host, u_short& port,
							        char *hostname, char* servicename,
							        u_int& maxconnect, u_int& usedconnect, int& load );

	    //
	    

	private:

		//! 取代上層功能
		virtual void OnKnockout( void );
		//! 取代上層功能
		//! 被斷線以後會被喚起的 callback
		virtual void OnDisconnection( void );


		virtual void OnGetHostInfo( ipa_t host, u_short port,
			                        const char* hostname, 
									const char* servicename, 
			                        u_int maxconnect, u_int usedconnect );

		virtual void OnRecvDefineRequest( Net::TCPSocket* socket, entityid_t eid, channelid_t cid, const char* command, size_t len );


//		// 為了讓 Validate以取用 Game的
//		virtual void OnGetCreateEntity( const char* name, void* prop, size_t len ){}
//		virtual void OnEntityDeleted( const char* name, int resultcode ){}
//		virtual void OnGetRecordEntity( const char* name, void* prop, size_t len ){}
//		virtual void OnGetSelectEntityId( CharacterId_t eid ){}
//		virtual void OnJoinGame( CharacterId_t entityId, entitybuf_t prop, size_t len ){};
//		virtual void OnExitGame( CharacterId_t entityId, entitybuf_t prop, size_t len ){};
//	    virtual void OnEnterSphere( CharacterId_t entityId, appearbuf_t prop, size_t len ){};
//		virtual void OnDepartSphere( CharacterId_t entityId ){};


	};
};



#endif //#ifndef CSVALIDATECLIENT_H