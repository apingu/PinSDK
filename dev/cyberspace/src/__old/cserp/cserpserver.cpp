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
#include "CsERPServer.h"
#include "CsERPDBFunction.h"
#include "CsERPCommand.h"
#include "CsERPRules.h"
//#include <CsArbitPeer.h>
//#include <CstIdentity.h>
//#include <XMySqlDB.h>

namespace Cyberspace
{
    namespace ERP
    {
        namespace Singleton
        {
            Cyberspace::ERP::Rules g_Rules;
        }
        
#define _SEND_EMSERVER_MESSAGE( s, m )  SendCsMessage( s, CsINVALID_ID, (m)->Header.CommandId, (m)->Buff, (m)->Header.CommandLen )
        
        Server::Server()
        {
        
        }
        
        
        Server::~Server()
        {
        
        }
        
        int Server::StartUp( uint_t allowplayers, unsigned short port )
        {
            int ret = Cyberspace::Server::StartUp( allowplayers, port );
            RegisterUserDefChannel( EMC_S_SELECT_BULLETIN_REQUEST, DoSelectBulletin );
            return ret;
        }
        
        
        void Server::OnClientConnect( CsSessionDescriptor sdp )
        {
        
        }
        
        void Server::OnClientDisconnect( CsSessionDescriptor sdp )
        {
        
        }
        
        int Server::DoSelectBulletin( const ServerEvocator* evocator, const void* pData, unsigned int len )
        {
            if( !ERP::Singleton::g_Rules.GetItem( EMC_S_SELECT_BULLETIN_REQUEST, GetSessionGroup( evocator->Sessiondescriptor ) ) )
            { return 0; }
            
            return 1;
        }
    };
};