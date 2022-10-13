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
#include <PfStdlib.h>
#include <memio.inl>
#include <string>
#include <PfTimer.h>

#include "NetServer.h"
#include "CsPaperClient.h"
#include "CsPacketFactory.h"
#include "CsPaperCommand.h"



namespace Cyberspace
{
    namespace Paper
    {
        //=============================================================================
        //
        //
        //=============================================================================
        Client::Client()
        {
            m_PacketFactory = NULL;
            //m_pSCSSystem = NULL;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        Client::~Client()
        {
            Close();
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        int Client::StartUp( const tchar_t* hname, unsigned short port )
        {
            TS_Lock lock( &m_utLock );
            
            if ( m_PacketFactory == NULL )
            {
                m_PacketFactory = new PacketFactory;
            }
            
            m_Addr = hname;              // server address
            m_Port = port;               // port number
            m_PacketFactory->Initate();
            
            if ( Ping() )
            {
                return 1;
            }
            
            return 0;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        int Client::Shutdown( void )
        {
            return Close();
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
		tstring Client::Set( tstring key, tstring value, int live_sec )
        {
			CsRecord set_record;
			set_record.key = key;
			set_record.table = _T("");
			set_record.value = value.c_str();
			
            CsResBuf buf;
			CsPaperCommand::SetValue::Client::SetRequest( buf, set_record, live_sec );
            m_PacketFactory->SendPacket( ( Net::TCPSocket* )( *this ), CsINVALID_ID, CsC_S_REQUEST_SET_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            uniqueid_t  who;
            channelid_t commandId;
            ubyte_t* recvbuf = NULL;
            size32_t recvlen = 0;
			
            
            if ( m_PacketFactory->RecvPacket( ( Net::TCPSocket* )( *this ), who, commandId, &recvbuf, recvlen ) > 0 )
            {
                //tstring retkey;
                //ubyte_t* valbuf = NULL;
                //int     vallen = 0;
				CsRecord ret_record;
                int      ret = -1;
                Databuf  recvdata;
                recvdata.buf = recvbuf;
                recvdata.len = recvlen;
                CsPaperCommand::SetValue::Client::GetFeedback( &recvdata, ret_record, ret );
                //SAFE_DELETE_ARRAY( recvbuf );
                
                if ( commandId != CsS_C_FEEDBACK_SET_VALUE )
                {
                    // TODO: Pin
                }
                
                if ( ret_record.key != key )
                {
                    // TODO: Pin
                }
                
				//ResBuf retbuf;
				//retbuf.copy( valbuf, vallen );
				return ret_record.value.c_str();
            }
            
			return _T("");
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
		tstring Client::Setnx( _IN tstring key, _IN tstring value, _IN int live_sec )
        {
			CsRecord set_record;
			set_record.key = key;
			set_record.table = _T( "" );
			set_record.value = value.c_str();
            CsResBuf buf;
            CsPaperCommand::SetValue::Client::SetRequest( buf, set_record, live_sec );
            m_PacketFactory->SendPacket( ( Net::TCPSocket* )( *this ), CsINVALID_ID, CsC_S_REQUEST_SETNX_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            uniqueid_t  who;
            channelid_t commandId;
            ubyte_t* recvbuf = NULL;
            size32_t recvlen = 0;
            
            if ( m_PacketFactory->RecvPacket( ( Net::TCPSocket* )( *this ), who, commandId, &recvbuf, recvlen ) > 0 )
            {
                //tstring retkey;
                //ubyte_t* valbuf = NULL;
                //int     vallen = 0;
				CsRecord ret_record;
				int      ret = -1;
				Databuf  recvdata;
				recvdata.buf = recvbuf;
				recvdata.len = recvlen;
				CsPaperCommand::SetValue::Client::GetFeedback( &recvdata, ret_record, ret );
				//SAFE_DELETE_ARRAY( recvbuf );
                
                if ( commandId != CsS_C_FEEDBACK_SET_VALUE )
                {
                    // TODO: Pin
                }
                
                if ( ret_record.key != key )
                {
                    // TODO: Pin
                }
                
				return ret_record.value.c_str();
            }
            
			return _T( "" );
        }
        
        
        //=============================================================================
        //
        //
        //=============================================================================
        tstring Client::Get( tstring key )
        {
            CsResBuf buf;
            CsPaperCommand::GetValue::Client::SetRequest( buf, key );
            m_PacketFactory->SendPacket( ( Net::TCPSocket* )( *this ), CsINVALID_ID, CsC_S_REQUEST_GET_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            uniqueid_t  who;
            channelid_t commandId;
            ubyte_t* recvbuf = NULL;
            size32_t recvlen = 0;
            
            if ( m_PacketFactory->RecvPacket( ( Net::TCPSocket* )( *this ), who, commandId, &recvbuf, recvlen ) > 0 )
            {
                tstring retkey;
                ubyte_t* valbuf = NULL;
                int     vallen = 0;
                int     ret = 0;
                Databuf recvdata;
                recvdata.buf = recvbuf;
                recvdata.len = recvlen;
                CsPaperCommand::GetValue::Client::GetFeedback( &recvdata, retkey, &valbuf, vallen, ret );
                //SAFE_DELETE_ARRAY( recvbuf );
                
                if ( commandId != CsS_C_FEEDBACK_GET_VALUE )
                {
                    // TODO: Pin
                }
                
                if ( retkey != key )
                {
                    // TODO: Pin
                }
                
				ResBuf retbuf;
				retbuf.copy( valbuf, vallen );
				return retbuf.c_str();
            }
            
            return _T( "" );
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        tstring Client::Del( tstring key )
        {
            CsResBuf buf;
            CsPaperCommand::DelValue::Client::SetRequest( buf, key );
            m_PacketFactory->SendPacket( ( Net::TCPSocket* )( *this ), CsINVALID_ID, CsC_S_REQUEST_DEL_VALUE, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            uniqueid_t  who;
            channelid_t commandId;
            ubyte_t* recvbuf = NULL;
            size32_t recvlen = 0;
            
            if ( m_PacketFactory->RecvPacket( ( Net::TCPSocket* )( *this ), who, commandId, &recvbuf, recvlen ) > 0 )
            {
                tstring retkey;
                ubyte_t* valbuf = NULL;
                int     vallen = 0;
                int     ret = 0;
                Databuf recvdata;
                recvdata.buf = recvbuf;
                recvdata.len = recvlen;
                CsPaperCommand::DelValue::Client::GetFeedback( &recvdata, retkey, &valbuf, vallen, ret );
                //SAFE_DELETE_ARRAY( recvbuf );
                
                if ( commandId != CsS_C_FEEDBACK_DEL_VALUE )
                {
                    // TODO: Pin
                }
                
                if ( retkey != key )
                {
                    // TODO: Pin
                }
                
				ResBuf retbuf;
				retbuf.copy( valbuf, vallen );
				return retbuf.c_str();
            }
            
            return _T( "" );
        }

		//=============================================================================
		//
		//
		//=============================================================================
		void Client::Join( tstring tables, tstringlist key )
		{

		}

		//=============================================================================
		//
		//
		//=============================================================================
		tstringmap Client::GetTb( tstring table )
		{
			tstringmap string_map;
			return string_map;
		}

		//=============================================================================
		//
		//
		//=============================================================================
		void Client::Part( tstring tables, tstringlist key )
		{
			return;
		}
        
        //=============================================================================
        //
        //
        //=============================================================================
        void Client::OnReceive( Net::TCPSocket* skt )
        {
            return;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        int Client::Close( void )
        {
            TS_Lock lock( &m_utLock );
            int ret = Net::TCPClient::Close();
            
            if ( ret != 1 )
            {
                return ret;
            }
            
            if ( m_PacketFactory )
            {
                m_PacketFactory->Initate();
            }
            
            //SAFE_DELETE( m_pSCSSystem );
            SAFE_DELETE( m_PacketFactory );
            return ret;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        bool Client::Ping( void )
        {
            //m_pSCSSystem->Close();
            TCPClient::Close();
            TCPClient::Create();
            
            if ( TCPClient::Connect( ( tchar_t* ) m_Addr.c_str(), m_Port ) )
            {
                TCPClient::SetBlock( false );
                return true;
            }
            
            return false;
        }
    };
    
};
