#ifndef _CSSCSSECTIONSERVER_H
#define _CSSCSSECTIONSERVER_H

#include <NetServer.h>
#include "CsSCSSystem.h"
#include <Debug.h>

namespace Cyberspace
{
	namespace SCS
	{

		///////////////////////////////////////////////////////////////////////////////////
		// 其他server連到我的socket
		class PeerSection: public PacketConnection
		{
		public:
			PeerSection() {/*m_RecvBuf=NULL; m_RecvLen=0;*/ };
			~PeerSection() {/*SAFE_DELETE_ARRAY(m_RecvBuf); m_RecvLen=0;*/ };

			serverid_t    serverid;
		};

		///////////////////////////////////////////////////////////////////////////////////
		//
		class SectionServer: public Net::TCPServer
		{
			Cyberspace::SCS::System*    m_pDelegate;
			T_Lock                      m_utLock;

		public:

			SectionServer( Cyberspace::SCS::System* pDelegate ) { m_pDelegate = pDelegate; }
			~SectionServer() {}

		private:

			void        OnAccept( Net::TCPSocket* skt ) 
            {
                sockaddr_storage addr_storage;
                sockaddr* addr = skt->GetPeerAddr( addr_storage );
                ipa_t     ipa = Net::Socket::GetSockaddrIPA( addr );
                short     port = Net::Socket::GetSockaddrPort( addr );
                Pf_DEBUG( "SCS connent node on %s:%d\n", ipa.c_str(), port );
                return; 
            }

			void        OnReceive( Net::TCPSocket* skt )
			{
				channelid_t  CommandId = 0;
				unsigned int CommandLen = 0;
				uniqueid_t   eid = CsINVALID_ID;
				ubyte_t*     recvbuff = NULL;
				int          ret = 0;
				PeerSection* cas = ( PeerSection* ) skt; // 這裡進來的 一定是 CsSSession

				if ( ( ret = cas->RecvPacket( eid, CommandId, &recvbuff, CommandLen ) ) > 0 )
				{
					m_pDelegate->OnRecvDefineRequest( skt, eid, CommandId, recvbuff, CommandLen );
				}
			}

			void       OnKickout( Net::TCPSocket* skt )
			{
                sockaddr_storage addr_storage;
                sockaddr* addr = skt->GetPeerAddr( addr_storage );
                ipa_t     ipa = Net::Socket::GetSockaddrIPA( addr );
                short     port = Net::Socket::GetSockaddrPort( addr );
                Pf_DEBUG( "SCS close node on %s:%d\n", ipa.c_str(), port );
                if ( m_pDelegate != NULL )
                {
                    m_pDelegate->OnSectionFree( skt );
                }				
			}

			virtual Net::TCPSocket* OnNewConnection( void ) { return new PeerSection; }
			virtual void            OnDelConnection( Net::TCPSocket* skt ) { PeerSection* cas=(PeerSection*)skt; SAFE_DELETE( cas ); }
		};

	};
};


#endif //_CSSCSSECTIONSERVER_H