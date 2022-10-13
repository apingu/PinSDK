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

#ifndef CSLOGINCOMMAND_H
#define CSLOGINCOMMAND_H

#pragma warning(disable:4786)

#include <vector>
//#include <memio.inl>

#include "CsDef.h"
#include "CsProtocol.h"
#include "CsErr.h"
//#include <Debug.h>

//#include "CsCommandFactory.h"


#define CsID_LOBBY  (_CsMAXCHANNEL+15300)
//#define SEND_LOBBY_MESSAGE(s, m)  (s->SendPacket(CsINVALID_ID, (m)->Header.CommandId, (m)->Buff, (m)->Header.CommandLen)
//
enum CsLOGINCOMMAND { CsC_S_LOGIN_REQUEST  = CsID_LOBBY + 1,
                      CsS_C_LOGIN_FEEDBACK,
                      CsC_S_LOGIN_LOGOUT_REQUEST,
                      CsC_S_LOGIN_LOGOUT_FEEDBACK,
                      CsC_S_READY_REQUEST,         //server -> client
                      CsC_S_LINK_REQUEST,
                      CsS_C_LINK_FEEDBACK,
                    };

//using namespace CsCore;

//======================================
//  Login Header Message
//======================================
namespace CsLobbyCommand
{
    //////////////////////////////////////////////////////////////////////////////
    // Client 向SERVER告知我已經選選好要使用的角色了 準備要進入遊戲中
    //
    //======================================
    //
    //======================================
    namespace CsReadyMessage
    {
        namespace Client
        {
            inline size32_t SetRequestMessage( CsResBuf& buf )
            {
                buf.resize( 1 );
                byte_t* param = _GetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%d", 0 );
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////
    //======================================
    //
    //======================================
    
    
};

namespace CsLoginMessage
{
    namespace Client
    {
        inline size32_t SetRequestMessage( CsResBuf& buf, const char* username, const char* password )
        {
            //msg->Header.Who = CsINVALID_ID;
            //msg->Header.CommandId = CsC_S_LOGIN_REQUEST;
            //msg->Header.CommandLen = CsFactory::MakeArgument(msg->Buffer.Param, "%s%s", username, password);
            //msg->Header.CommandLen+=64+(rand()%64);
            buf.resize( sizeofcsstr( username ) + sizeofcsstr( password ) + ( 64 + ( rand() % 64 ) ) );
            byte_t* param = _GetResBufPtr( buf );
            return CsFactory::MakeArgument( param, "%s%s", username, password );
        }
        
        inline void GetFeedback( const byte_t* buff, short& result )
        {
            CsFactory::SplitArgument( buff, "%h", &result );
            return;
        }
    };
    
    namespace Server
    {
        inline size32_t SetFeedbackMessage( CsResBuf& buf, short result )
        {
            //msg->Header.Who = CsINVALID_ID;
            //msg->Header.CommandId = CsS_C_LOGIN_FEEDBACK;
            buf.resize( sizeof( short ) );
            byte_t* param = _GetResBufPtr( buf );
            return CsFactory::MakeArgument( param, "%h", result );
        }
        
        
        inline bool GetRequest( const byte_t* buff, size32_t bufflen, char* username, char* password )
        {
            //DebugMessage("login get requet 1\n", "login");
            if( ( bufflen <= 0 ) || ( bufflen >= ( ( _CsMAXSTRING + sizeof( short ) ) * 2 ) ) || ( buff == NULL ) )
            {
                //DebugMessage("login get requet 2\n", "login");
                username[0] = '\0';
                password[0] = '\0';
                return false;
            }
            
            unsigned short ulen = ( *( unsigned short* )buff );
            if( ulen >= _CsMAXSTRING )
            { 
				return false; 
			}
            unsigned short plen = ( *( unsigned short* )( buff + sizeof( unsigned short ) + ulen + 1 ) );
            if( plen >= _CsMAXSTRING )
            {
				return false; 
			}
            
            if( bufflen < ( ulen + plen + ( sizeof( short ) * 2 ) ) )
            { 
				return false; 
			}
            
            //DebugMessage("login get requet 3\n", "login");
            //CsC_S_LOGIN_REQUEST
            CsFactory::SplitArgument( buff, "%s%s", username, password );
            //DebugMessage("login get requet 4\n", "login");
            return true;
        }
    };
};

namespace CsBridgeMessage
{
    namespace Client
    {
        inline size_t SetRequestMessage( CsResBuf& buf,  const char* linkdata )
        {
            buf.resize( sizeofcsstr( linkdata ) );
            byte_t* param = _GetResBufPtr( buf );
            return CsFactory::MakeArgument( param, "%s", linkdata );
        }

        inline void GetFeedback( const byte_t* buff, short& result, char* retbuf )
        {
            CsFactory::SplitArgument( buff, "%h%s", &result, retbuf );
            return;
        }
    };
    
    namespace Server
    {
        inline size_t SetFeedbackMessage( CsResBuf& buf, short result, char* retbuf )
        {
            //msg->Header.Who = CsINVALID_ID;
            //msg->Header.CommandId = CsS_C_LOGIN_FEEDBACK;
            buf.resize( sizeof( short ) + sizeofcsstr( retbuf ) );
            byte_t* param = _GetResBufPtr( buf );
            return CsFactory::MakeArgument( param, "%h%s", result, retbuf );
        }
        
        
        inline bool GetRequest( const byte_t* buff, size_t bufflen, char* linkdata )
        {
            //DebugMessage("login get requet 1\n", "login");
            if( ( bufflen <= 0 ) ||
                ( bufflen >= ( ( _CsMAXSTRING + sizeof( short ) ) * 2 ) ) ||
                ( buff == NULL ) )
            {
                //DebugMessage("login get requet 2\n", "login");
                linkdata[0] = '\0';
                return false;
            }
            
            
            //CsC_S_LOGIN_REQUEST
            CsFactory::SplitArgument( buff, "%s", linkdata );
            
            //DebugMessage("login get requet 4\n", "login");
            return true;
        }
    };
};






#endif //CSHMESSAGE_CPP
