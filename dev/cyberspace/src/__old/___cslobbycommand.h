// Cyber space common define file
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

#ifndef CSLOBBYCOMMAND_H
#define CSLOBBYCOMMAND_H

#pragma warning(disable:4786)

#include <vector>
#include <memio.inl>

#include "CsDef.h"
#include "CsProtocol.h"
#include "CsErr.h"
//#include <Debug.h>

//#include "CsCommandFactory.h"


#define CsID_LOBBY  ( _CsMAXCHANNEL+1200 )
//#define SEND_LOBBY_MESSAGE( s, m )  ( s->SendPacket( CsINVALID_ID, (m)->Header.CommandId, (m)->Buff, (m)->Header.CommandLen )
//
enum CsLOBBYCOMMAND { CsC_S_LOGIN_REQUEST  = CsID_LOBBY+1,
                      CsS_C_LOGIN_FEEDBACK,
					  CsC_S_LOGOUT_REQUEST,
					  CsS_C_LOGOUT_REQUEST,


  					  CsC_S_READY_REQUEST,                           //server -> client

					  CsC_S_SERVERINFI_POST,
};

//using namespace CsCore;

//======================================
//  Login Header Message
//======================================
namespace CsLobbyCommand
{
	namespace ServerInfo
	{
		namespace Client
        {
            inline void GetRequest( char*    buff,
				                    ipn_t&   host,
				                    u_short& port,
									char*    hostname,
									char*    servicename,
									u_int&   maxconnect,
									u_int&   usedconnect )
			{
				CsFactory::SplitArgument( buff, "%l%h%s%s%d%d", &host,
					                                            &port,
																hostname,
																servicename,
																&maxconnect,
																&usedconnect );
			}
        };


        namespace Server
        {
            inline size_t SetRequest( CsCommandPacket* msg,
				                      ipn_t       host,
				                      u_short     port,
									  const char* hostname,
									  const char* servicename,
									  u_int       maxconnect,
									  u_int       usedconnect )
			{
				msg->Header.Who = CsINVALID_ID;
				msg->Header.CommandId = CsC_S_SERVERINFI_POST;
				return ( msg->Header.CommandLen = CsFactory::MakeArgument( msg->Buff, "%l%h%s%s%d%d", host.S_un.S_addr,
																				                      port,
																									  hostname,
																									  servicename,
																				                      maxconnect,
																				                      usedconnect ) );
			}
		};

	}

	//////////////////////////////////////////////////////////////////////////////
	// Client 向SERVER告知我已經選選好要使用的角色了 準備要進入遊戲中
	// 
	//======================================
	//  Select Entity Header Message
	//======================================
	namespace CsReadyMessage
	{
		namespace Client
		{
			inline size_t SetRequestMessage( CsResBuf& buf )
			{
				buf.resize(1);
				byte_t* param = _GetResBufPtr(buf);
				return return CsFactory::MakeArgument( param, "%d", 0);
			}
		}
	}

};

namespace CsLoginMessage
{
	namespace Client
	{
		inline size_t SetRequestMessage( CsCommandPacket* msg,
									     const char* username,
									     const char* password )
		{
			msg->Header.Sender = CsINVALID_ID;
			msg->Header.CommandId = CsC_S_LOGIN_REQUEST;
			msg->Header.CommandLen = CsFactory::MakeArgument( msg->Buff, "%s%s", username, password );
			msg->Header.CommandLen+=64+(rand()%64);
			return msg->Header.CommandLen;
		}

		inline void GetFeedback( char* buff, short& result )
		{
			CsFactory::SplitArgument( buff, "%h", &result );
			//if( msg->Header.CommandId == CsS_C_LOGIN_FEEDBACK )
			return;
		}
	};

	namespace Server
	{
		inline size_t SetFeedbackMessage( CsCommandPacket* msg, short result )
		{
			msg->Header.Sender = CsINVALID_ID;
			msg->Header.CommandId = CsS_C_LOGIN_FEEDBACK;
			return ( msg->Header.CommandLen = CsFactory::MakeArgument( msg->Buff, "%h", result ) );
		}


		inline bool GetRequest( char* buff, size_t bufflen, char* username, char* password )
		{
			//DebugMessage( "login get requet 1\n", "login" );
			if( (bufflen<=0)||
				(bufflen>=((_CsMAXSTRING+sizeof(short))*2))||
				(buff==NULL) )
			{
				//DebugMessage( "login get requet 2\n", "login" );
				username[0]='\0';
				password[0]='\0';
				return false;
			}

			unsigned short ulen= (*(unsigned short*)buff);
			if( ulen>=_CsMAXSTRING )
				return false;
			unsigned short plen= (*(unsigned short*)(buff+sizeof(unsigned short)+ulen+1));
			if( plen>=_CsMAXSTRING )
				return false;

			if( bufflen < (ulen+plen+(sizeof(short)*2)) )
				return false;
			
			//DebugMessage( "login get requet 3\n", "login" );

			//CsC_S_LOGIN_REQUEST
			CsFactory::SplitArgument( buff,
				                      "%s%s",
 									  username,
									  password );

			//DebugMessage( "login get requet 4\n", "login" );
			return true;
		}
	};
};





#endif //CSHMESSAGE_CPP
