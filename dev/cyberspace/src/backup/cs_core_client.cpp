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
#include "CsCoreClient.h"
#include "CsBaseCommand.h"
#include <Debug.h>


namespace Cyberspace
{

#define _MAX_RESEND 5

    class commbuf_t
    {

    public:
        inline commbuf_t()
        {
            m_who = 0;
            m_cid = 0;
            m_buf = 0;
            m_len = 0;
        }

        inline ~commbuf_t()
        {
            SAFE_DELETE_ARRAY( m_buf );
        }

        uniqueid_t  m_who;
        channelid_t m_cid;
        ubyte_t*    m_buf;
        size32_t    m_len;
        //CsResBuf    m_buff;
    };

	//===============================================================================//
	//
	//
	//===============================================================================//
	CoreClient::CoreClient( UnitClient* client )
    {
        //Clear();
        //m_PacketFactory = new Cyberspace::PacketFactory();
		ClearSessionId();
        m_PowerId = -1;
        m_GroupId = -1;
        m_Client = client;
        m_MaxSendSize = 5120;
        m_SendInterval = 50;
        m_bSend = 1;
        m_bCloseUntilSendFinish.SetValue( true );
        m_UserDefMessageDispatcher.SetMaxChannel( _CsMAXCHANNEL );
        retry_count = 20;
        retry_time = 1000;
        m_new_command = 0;
        m_MessageStamp = 0;
        TS_Lock lock( &m_BannedLock );
        m_bBanned = false;
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    CoreClient::~CoreClient()
    {
        //SAFE_DELETE( m_PacketFactory );
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void CoreClient::SetSessionId( sessionid_t cid )
    {
        TS_Lock lock( &m_UtLock );
        PacketClient::SetSessionId( cid );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    sessionid_t CoreClient::GetSessionId()
    {
        TS_Lock lock( &m_UtLock );
        return PacketClient::GetSessionId();
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void CoreClient::ClearSessionId()
    {
        TS_Lock lock( &m_UtLock );
        PacketClient::SetSessionId( CsINVALID_ID );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    macn_t CoreClient::GetMACAddress()
    {
        TS_Lock lock( &m_UtLock );
        std::vector<EthIf> ethif_list;
        Net::Socket::GetEthIfinfo( ethif_list );
        return ethif_list[0].macn;
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    //void CoreClient::BindSession( powerid_t pid, int gid )
    //{
    //    TS_Lock lock( &m_UtLock );
    //    m_PowerId = pid;
    //    m_GroupId = gid;
    //}
    
	//===============================================================================//
	//
	//
	//===============================================================================//
	void CoreClient::ToSession( _cstSession& session )
	{
        std::vector<EthIf> ethif_list;
        Net::Socket::GetEthIfinfo( ethif_list );
        sockaddr* skaddr = ( sockaddr* ) &ethif_list[ 0 ].unicast_addrs[ 0 ];

		session.sessionid = GetSessionId();
		//session.mac = GetMACAddress();
		//session.power = m_PowerId;
		//session.group = m_PowerId;
		session.serverid = CsINVALID_ID;
		session.ipn6 = Net::Socket::GetSockaddrIPN6( skaddr );
		session.port = 0;       // 沒有連線前 無法取得 port 資料
		session.createsec = GetLastCreateTime();
		session.flag = 0;
	}
    
	//===============================================================================//
	//
	//
	//===============================================================================//
	//void CoreClient::PassSession( CoreClient& c )
	//{
	//    TS_Lock lockA( &m_UtLock );
	//    TS_Lock lockB( &c.m_UtLock );
	//    m_PowerId = c.m_PowerId;
	//    m_GroupId = c.m_GroupId;
	//    ( *m_PacketFactory ) = ( *c.m_PacketFactory );
	//}
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::SendPacket( uniqueid_t eid, unsigned short cid, const ubyte_t* bp, size32_t len )
    {
         return PacketClient::SendPacket( eid, cid, bp, len );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::RecvPacket( uniqueid_t& eid, unsigned short& cid, ubyte_t** bp, size32_t& len )
    {
        // 每次讓他都new 一個 沒關西 反正是clinet
        return PacketClient::RecvPacket( eid, cid, bp, len );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::PostSendPacket( uniqueid_t who, channelid_t cid, const ubyte_t* bp, size32_t len )
    {
        commbuf_t* data = new commbuf_t;
        data->m_who = who;
        data->m_cid = cid;
        data->m_len = len;
        
        if ( len > 0 )
        {
            data->m_buf = new ubyte_t[ len ];
            IOMEMCPY( data->m_buf, bp, len );
        }
        else
        {
            data->m_buf = NULL;
        }
        
        TS_Lock lock0( &m_UtsLock );
        m_SendList.push_back( data );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::BeginSendTask()
    {
        m_bSend = 1;
        m_hSending = PfSystem::Thread::BeginProcess( CoreClient::DoSendTask, this, _T( "CoreClient::BeginSendTask" ) ); //m_pCoreClient->BeginProcess(CoreClient::DoSendTask, m_pCoreClient);
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    bool CoreClient::RegisterChannel( channelid_t id, ClientMessageChannelFunction func )
    {
        return m_UserDefMessageDispatcher.RegisterChannel( id, ( MessageChannelFunction ) func );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    bool CoreClient::UnRegisterChannel( channelid_t id )
    {
        return m_UserDefMessageDispatcher.UnRegisterChannel( id );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::DispatchMessage( const Evocator* evocator, channelid_t chid, const ubyte_t* command, size32_t len )
    {
        return m_UserDefMessageDispatcher.DispatchMessage( &evocator, chid, command, len );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    bool CoreClient::GetBanned()
    {
        TS_Lock lock( &( m_BannedLock ) );
        return m_bBanned;
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::SetBanned( bool b )
    {
        TS_Lock lock( &( m_BannedLock ) );
        m_bBanned = b;
    }
    
	//===============================================================================//
	//
	// this is real connect for client
	//===============================================================================//
    //bool CoreClient::Connect( const tchar_t* hname, unsigned short hport, const tchar_t* lname, unsigned short lport )
	bool CoreClient::Connect( const tchar_t* hname, unsigned short hport )
    {
        //PacketClient::Close();
        //m_PacketFactory->Initate();
        if ( IsConnected() )
        {
            return false;
        }
        
        //if ( lport != 0 )
        //{
        //    struct sockaddr_in locala;
        //    char fportbuf[ 128 ];
        //    itoa( lport, fportbuf, 10 );
        //    Net::Socket::SetAddress( lname, fportbuf, &locala, _T( "tcp" ) );
        //    bind( GetFD(), ( struct sockaddr* ) &locala, sizeof( locala ) );
        //}
        
        if ( PacketClient::Connect( hname, hport ) )
        {
            Set_tcp_buf_size( SO_RCVBUF, _MAX_SOCKET_BUFF );
            Set_tcp_buf_size( SO_SNDBUF, _MAX_SOCKET_BUFF );
            //CsCommandPacket connmsg;
            //SEND_BASE_MESSAGE(m_BaseClient, &connmsg);
            CsResBuf buf;
            CsBaseCommand::ActiveConnect::Client::SetRequest( buf, GetSessionId(), GetMACAddress(), m_Client->OnGetVersion() );
            SendPacket( CsINVALID_ID, CsC_S_CONNECT_REQUEST, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
            return true;
        }
        
        return false;
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::Close( void )
    {
		// 把還沒送的資料 送完
		if ( GetFD() != 0 )
		{
			ReflashSend( 0 );
		}
		else
		{
			ClearSendList();
		}

		// 這邊一定要等資料送完 才能斷線 很重要
		// 目前一起擺到 TCPServer 跟 Client裏面當預設值
		//struct linger li = { 1, 10 };
		//setsockopt(m_BaseClient->GetFD(), SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li));
		//等資料送完
		m_bSend = 0;

		// 先做一次清空的動作
		// 以免有些訊息 不會被處理 像是 login時候的資料的assign
		if ( GetFD() != 0 )
		{
			ReflashRecv( 0 );
		}
		else
		{
			ClearRecvList();
		}

		// 如果是被Server knockout的話 讓她回主緒一下下 看有沒有要處理的
		//Sleep(1000);
		//Clear();
		m_PowerId = -1;
		m_GroupId = -1;
        return PacketClient::Close();
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::Shutdown( int a )
    {
        return Close();
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::OnReceive()
    {
        uniqueid_t      who = CsINVALID_ID;
        unsigned int    CommandLen;
        channelid_t     CommandId;
        ubyte_t*        recvbuff = NULL;
        
        if ( RecvPacket( who, CommandId, &recvbuff, CommandLen ) >= 0 )
        {
            commbuf_t* commb = new commbuf_t;
            commb->m_who = who;
            commb->m_cid = CommandId;
            commb->m_len = CommandLen;
            commb->m_buf = new ubyte_t[ CommandLen ];
            IOMEMCPY( commb->m_buf, recvbuff, CommandLen );
            TS_Lock lock( &( m_UtrLock ) );
            m_RecvList.push_back( commb );
        }
        else
        {
            // 封包錯誤 只好斷線
            Close();
        }
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    int CoreClient::Update( void )
    {
        TS_Lock lock( &m_UpdateLock );
        // 一次只處理一個
        ReflashRecv( 1 );
        return 1;
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void __cdecl CoreClient::DoSendTask( void* peer )
    {
        Cyberspace::CoreClient* speer = ( Cyberspace::CoreClient* )peer;
        
        while ( speer->m_bSend == 1 ) //
        {
            speer->ReflashSend( 1 );
        }
        
        Pf_ERROR( "CoreClient: DoSendTask Exit..." );
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::ReflashRecv( int cnt )
    {
        if ( cnt < 0 )
        {
            return;
        }
        
        // 先做一次清空的動作
        // 以免有些訊息 不會被處理 像是 login時候的資料的assign
        std::vector<commbuf_t*>  rcvlist;
        {
            TS_Lock lock( &( m_UtrLock ) );
            
            if ( cnt == 0 )
            {
                cnt = m_RecvList.size();
            }
            
            for ( int i = 0; i < cnt; i++ )
            {
                if ( m_RecvList.empty() )
                {
                    break;
                }
                
                rcvlist.push_back( m_RecvList.front() );
                m_RecvList.pop_front();
            }
        }

        for ( uint_t i = 0; i < rcvlist.size(); ++i )
        {
            commbuf_t* commb = rcvlist[ i ];
            
            if ( commb->m_cid < _CsMAXCHANNEL )
            {
                m_Client->OnRecvUserDefRequest( m_Client->GetSessionId(), commb->m_who, commb->m_cid, commb->m_buf, commb->m_len );
            }
            else
            {
                m_Client->OnRecvDefineRequest( m_Client->GetSessionId(), commb->m_who, commb->m_cid, commb->m_buf, commb->m_len );
            }
            
            SAFE_DELETE_ARRAY( commb->m_buf );
            SAFE_DELETE( commb );
        }
        
        rcvlist.clear();
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//    
    void CoreClient::ReflashSend( int cnt )
    {
        //char debugbuff[ 256 ];
        uint_t sendsize = 0;
        
        if ( cnt < 0 )
        {
            return;
        }
        
        //SENDLABLE:
        std::vector<commbuf_t*>  sndlist;
        //commbuf_t* data = NULL;
        {
            TS_Lock lock( &m_UtsLock );
            
            if ( cnt == 0 )
            {
                cnt = m_SendList.size();
            }
            
            for ( int i = 0; i < cnt; i++ )
            {
                if ( m_SendList.empty() )
                {
                    break;
                }
                
                sndlist.push_back( m_SendList.front() );
                m_SendList.pop_front();
            }
        }
        {
            for ( uint_t i = 0; i < sndlist.size(); i++ )
            {
                commbuf_t* data = sndlist[ i ];
                bool bsend = false;
                ubyte_t* buff = NULL;
                unsigned int blen = data->m_len;
                
                if ( blen > 0 )
                {
                    buff = data->m_buf;
                }
                
                int error = 0;
                
                for ( uint_t j = 0; j < _MAX_RESEND; j++ )
                {
                    int sndsize = SendPacket( data->m_who, data->m_cid, buff, blen );
                    
                    if ( sndsize == SOCKET_ERROR )
                    {
                        Sleep( m_SendInterval );
                        continue;
                    }
                    else
                    {
                        sendsize += sndsize;
                        bsend = true;
                        break;
                    }
                }
                
                // 送5次還是失敗 記錄一下吧
                if ( !bsend )
                {
                    //sprintf(debugbuff, "client %lld send %d, size = %d failure %d!", data->m_who, data->m_cid, blen, error);
                    //DebugOutPutf(debugbuff, "CoreClient");
                    Pf_ERROR( "CoreClient: client %lld send %d, size = %d failure %d!", data->m_who, data->m_cid, blen, error );
                }
                
                SAFE_DELETE_ARRAY( data->m_buf );
                SAFE_DELETE( data );
                
                // 送出的資料量 已經超過預設的頻寬
                if ( sendsize > m_MaxSendSize )
                {
                    // 重新記數
                    sendsize = 0;
                    // 休息一下
                    Sleep( m_SendInterval );
                }
                
                //	goto SENDLABLE; //continue;
            }
        }
        sndlist.clear();
        // 已經送超過了 休息一下
        Sleep( 1 );
        return;
    }

	//===============================================================================//
	//
	//
	//===============================================================================//    
    void CoreClient::ClearRecvList()
    {
        TS_Lock lock( &m_UtsLock );
        std::list< commbuf_t* >::iterator iter = m_RecvList.begin();
        
        while ( iter != m_RecvList.end() )
        {
            commbuf_t* buff = ( *iter );
            SAFE_DELETE_ARRAY( buff->m_buf );
            SAFE_DELETE( buff );
            iter++;
        }
        
        m_RecvList.clear();
    }
    
	//===============================================================================//
	//
	//
	//===============================================================================//
    void CoreClient::ClearSendList()
    {
        TS_Lock lock( &m_UtsLock );
        std::list< commbuf_t* >::iterator iter = m_SendList.begin();
        
        while ( iter != m_SendList.end() )
        {
            commbuf_t* buff = ( *iter );
            SAFE_DELETE_ARRAY( buff->m_buf );
            SAFE_DELETE( buff );
            iter++;
        }
        
        m_SendList.clear();
    }
}
