#ifndef _CSSCSSECTIONAGENT_H
#define _CSSCSSECTIONAGENT_H

#include "CsDef.h"
#include "CsPacketFactory.h"
#include "CsPacketClient.h"

namespace Cyberspace
{
    namespace SCS
    {
        //////////////////////////////////////////////////////////////////////////
        //
        class SectionClient: private PacketClient
        {
            typedef PacketClient BaseClass;
            friend  class Cyberspace::SCS::System;
            
            serverid_t                 m_LocalSId;
            Cyberspace::SectionEntity   m_Node;
            _csstring                  m_HName;
			ushort_t                   m_HPort;
			bool                       m_bTryConnect;

            
        public:
        
            SectionClient( serverid_t localsid, const Cyberspace::SectionEntity& node )
            {
                m_LocalSId = localsid;
                m_bTryConnect = false;
                IOMEMCPY( &m_Node, &node, _cssizeof( Cyberspace::SectionEntity ) );
            }
            
            ~SectionClient()
            {
                Close();
                m_bTryConnect = false;
            }
            
            inline bool Connect( const tchar_t* hname, unsigned short port )
            {
                m_HName = hname;
                m_HPort = port;

                //使用 block 模式
                bool ret = BaseClass::Connect( hname, port );
                CsResBuf buf;
                CsSCSCommand::SyncHost::Client::SetRequest( buf, m_LocalSId );
                SendPacket( CsINVALID_ID, CsC_S_SYNC_HOST_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                return ret;
            }
            
            inline int Close()
            {
                return PacketClient::Close();
            }
            
            inline int  SendPacket( uniqueid_t eid, unsigned short sessionid, const ubyte_t* bp, _cssize_t len )
            {
                return PacketClient::SendPacket( eid, sessionid, bp, len );
            }
            inline int  RecvPacket( uniqueid_t& eid, unsigned short& sessionid, ubyte_t** bp, _cssize_t& len )
            {
                return PacketClient::RecvPacket( eid, sessionid, bp, len );
            }
            
            
        private:
        
            inline const Cyberspace::SectionEntity*  GetHost( void )
            {
                return &m_Node;
            }
            
            inline void SetSectionEntity( const Cyberspace::SectionEntity* node )
            {
                memcpy( &m_Node, node, _cssizeof( Cyberspace::SectionEntity ) );
            }
            
            //inline int SyncSection( const _CsSessionEntity* buff, _cssize_t len )
            //{
            //    CsResBuf buf;
            //    CsSCSCommand::SyncHost::Client::SetRequest( buf, m_LocalSId, buff, len );
            //    return SendPacket( CsINVALID_ID, CsC_S_SYNC_HOST_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            //}
            
            // 直接告訴大家有N個新的CLIENT加入
            inline void InsertConnect( const _CsSessionEntity* buff, _cssize_t len )
            {
                // 沒有連線其他伺服器 所以不用報告
                if ( !IsConnected() )
                {
                    return;
                }
                
                CsResBuf buf;
                CsSCSCommand::InsertRemoveMaster::Client::SetRequest( buf, buff, len );
                
                if ( SendPacket( CsINVALID_ID, CsC_S_INSERT_CLONE_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) ) < 0 )
                {
#ifdef _DEBUG
                    printf( _T( "Insert Connect failure?" ) );
#endif
                }
            }
            
            // 直接告訴大家有N個新的CLIENT加入
            inline void    RemoveConnect( const _CsSessionEntity* buff, _cssize_t len )
            {
                if ( !IsConnected() )
                {
                    return;
                }
                
                CsResBuf buf;
                CsSCSCommand::InsertRemoveMaster::Client::SetRequest( buf, buff, len );
                
                if ( SendPacket( CsINVALID_ID, CsC_S_REMOVE_CLONE_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) ) < 0 )
                {
#ifdef _DEBUG
                    printf( _T( "Remove Connect failure?" ) );
#endif
                }
            }
            

            /*
            inline bool SyncConnect( const _CsSessionEntity* buff, _cssize_t len )
            {
                // 沒有連線其他伺服器 所以不用報告
                if ( !IsConnected() )
                {
                    return false;
                }
                
                CsResBuf buf;
                CsSCSCommand::SyncHost::Client::SetRequest( buf, m_LocalSId, buff, len );
                
                if ( SendPacket( CsINVALID_ID, CsC_S_SYNC_HOST_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) ) < 0 )
                {
#ifdef _DEBUG
                    printf( _T( "Sync Connect failure?" ) );
#endif
                    return false;
                }
                
                return true;
            }
            */
            
            /*
            // 建立一個 share buffer
            inline bool    CreateShareBuffer( const Cyberspace::SCS::BufferDescriptor* sbmm )
            {
            
                // 沒有連線其他伺服器 所以不用報告
                if(!IsConnected())
                	return false;
            
                CsResBuf buf;
                CsSCSCommand::CreateShareBuffer::Client::SetRequest(buf,SectionServer::GetSCSBufferUId(sbmm),SectionServer::GetSCSBufferGId(sbmm));
                if(SendPacket(CsINVALID_ID, CsC_C_CREATE_SHAREBUFFER_REQUEST, _CsGetResBufPtr(buf), _CsGetResBufLen(buf))<0)
                {
                #ifdef _DEBUG
                	printf(_T("Create share buffer failure?"));
                #endif
                }
            
                return true;
            }
            
            // 刪除一個 share buffer
            inline bool    DeleteShareBuffer( const Cyberspace::SCS::BufferDescriptor* sbmm )
            {
            
                // 沒有連線其他伺服器 所以不用報告
                if(!IsConnected())
                	return false;
            
                CsResBuf buf;
                CsSCSCommand::DeleteShareBuffer::Client::SetRequest(buf,SectionServer::GetSCSBufferUId(sbmm),SectionServer::GetSCSBufferGId(sbmm));
                if(SendPacket(CsINVALID_ID, CsC_C_DELETE_SHAREBUFFER_REQUEST, _CsGetResBufPtr(buf), _CsGetResBufLen(buf))<0)
                {
                #ifdef _DEBUG
                	printf(_T("Create delete buffer failure?"));
                #endif
                }
            
                return true;
            }
            
            // 以甚麼模式 取出一個share buffer
            inline CsResBuf SCSReadBuffer( const Cyberspace::SCS::BufferDescriptor* sbmm, CseSCSBUFFER_MODE mode = CsDBF_READ )
            {
                CsResBuf buf;
            
                if(!IsConnected())
                	return buf;
            
            
                CsSCSCommand::ReadShareBuffer::Client::SetRequest(buf,SectionServer::GetSCSBufferUId(sbmm),SectionServer::GetSCSBufferGId(sbmm));
                if(SendPacket(CsINVALID_ID, CsC_C_READ_SHAREBUFFER_REQUEST, _CsGetResBufPtr(buf), _CsGetResBufLen(buf))<0)
                {
                #ifdef _DEBUG
                	printf(_T("Read share buffer failure?"));
                #endif
                	return buf;
                }
            
                uniqueid_t  uid;
                channelid_t commandId;
                ubyte_t*  recvbuff=NULL;
                unsigned int   recvlen=0;
            
                if(RecvPacket(uid, commandId, &recvbuff, recvlen)>0)
                {
                	buf.copy(recvbuff, recvlen);
                	SAFE_DELETE_ARRAY(recvbuff);
                }
            
                return buf;
            }
            
            // 存回一個share buffer
            bool     SCSSaveBuffer( const Cyberspace::SCS::BufferDescriptor* sbmm, const ubyte_t* buff, _cssize_t len )
            {
                return false;
            }
            */
            
        private:
        
            
            //void    OnDisconnection(void){};
            void    OnReceive(){};
        };
    };
};

#endif //_CSSCSSECTIONAGENT_H