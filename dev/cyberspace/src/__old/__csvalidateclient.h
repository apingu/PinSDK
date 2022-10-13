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
#include "CsLoginClient.h"
//#include "CsGameClient.h"
#include "CsIGameClient.h"
#include <safevalue.inl>


namespace Cyberspace
{
		
	class ValidateClient : public LoginClient
	{

		T_Lock                m_UtVaLock;
		IGameClient*          m_GameClient;
		safevalue<bool>       m_bCbBlock;

	public:

		ValidateClient( IGameClient*  client );
		~ValidateClient();

		
	    //! step 2: get all entity
		bool           GetAllEntity( void );
		//! step 3: you can create entity,delete one,or select one.
		//			once selected, and OnGetSelectEntityId() called, game started.
		//! 創角
		void           CreateEntity( const char_t* name, const byte_t* prop, size_t len );
		//! 消滅角色
		void           DeleteEntity( const char_t* name );
		//! 通知SERVER 要選擇一個角色使用
		void           SelectEntity( const char_t* name );

		virtual	int    Update( void );


		void           SetBlock( bool t );
		bool           GetBlock( void );

	private:


		virtual void   OnAssingHost ( ipa_t host, u_short port,
									  const char* hostname, 
									  const char* servername, 
									  const char* functionname, 
									  u_int maxconnect, u_int usedconnect );


		void           OnGetHostInfo( ipa_t host, u_short port, 					
			                          const char* hostname, 
									  const char* servername, 
			                          const char* functionname, 
			                          u_int maxconnect, u_int usedconnect );

		virtual void   OnRecvDefineRequest( connectionid_t cid, uniqueid_t eid, channelid_t chid, const byte_t* command, size_t len );


		//! 取代上層功能
		virtual void   OnConnect( void );
		
		virtual void   OnKnockout( void );
				
		virtual void   OnDisconnection( void );


	};
};



#endif //#ifndef CSVALIDATECLIENT_H