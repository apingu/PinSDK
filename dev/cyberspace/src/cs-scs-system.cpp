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
//////////////////////////////////////////////////////////////////////////

//01.== C 系統文件/C++ 系統文件
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
#include <pf-thread.h>
#include <pf-system.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-protocol.h"
#include "cs-scs_command.h"
#include "cs-packet_factory.h"
#include "cs-uid_generator.h"
#include "cs-scs-system.h"
#include "cs-log_file_service.h"


namespace cyberspace
{
    extern int g_log_index;

    namespace scs
    {

#define _MAX_RECYCEL_CAN_USE_TIME 1800  //沒有使用超過 30分鐘的才給用

        //////////////////////////////////////////////////////////////////////////
        //
        //class SBuffer
        //{
        //    friend class SectionServer;
        //
        //    uniqueid_t        uid; // 編號
        //    int8_t            gid;
        //    cs_section_uid_t        sectionid;
        //
        //    T_Lock            utlock;
        //    _csbinbuf            buffer; // buffer的本體
        //    net::TCPSocket*   pHostLocation;
        //    CseSCSBUFFER_MODE mode;
        //
        //public:
        //    SBuffer() {};
        //    ~SBuffer() {};
        //};

        //////////////////////////////////////////////////////////////////////////
        //

        /*
        PeerSection* FindHoseConnect( scs::SectionServer* p_server, cs_section_uid_t sectionid )
        {
            std::vector<net::TCPSocket*> CsPSockConnlist;
            p_server->GetAllConnectedSocket( CsPSockConnlist );

            for ( unsigned int i = 0; i < CsPSockConnlist.size(); i++ )
            {
                PeerSection* ss = ( PeerSection* ) CsPSockConnlist[ i ];

                if ( ss->sectionid == sectionid )
                {
                    return ss;
                }
            }

            return NULL;
        }
        */

        //////////////////////////////////////////////////////////////////////////
        //
        class CsPriSectionEntity : public CsSectionEntity
        {
            T_Lock ut_lock;

        public:
            CsPriSectionEntity() { m_pSectionSocket = new CsAbtpPrSocket; }
            ~CsPriSectionEntity() { SAFE_DELETE( m_pSectionSocket ) }
            CsAbtpPrSocket*  m_pSectionSocket;

            void SetEnable( bool t ) { TS_Lock lock( &ut_lock ); if ( t ) { ___SET_SECTION_ENABLE( this ); } else { ___SET_SECTION_DISABLE( this ); } }
            bool GetEnable()         { TS_Lock lock( &ut_lock ); ___IF_SECTION_ENABLEL( this ); }
        };

        //////////////////////////////////////////////////////////////////////////
        //
        class CsPriSessionEntity : public CsSessionEntity
        {
            T_Lock ut_lock;

        public:
            CsPriSessionEntity() { m_pSectionEntity = NULL; m_pSessionSocket = NULL; }
            ~CsPriSessionEntity() {}
            CsAbtpCnSocket*      m_pSessionSocket;
            CsPriSectionEntity*  m_pSectionEntity;

            void SetEnable( bool t ) { TS_Lock lock( &ut_lock ); if ( t ) { ___SET_SESSION_ENABLE( this ); } else { ___SET_SESSION_DISABLE( this ); } }
            bool GetEnable()         { TS_Lock lock( &ut_lock ); return ___IF_SESSION_ENABLEL( this ); }
        };


#define Create_Section(s)            s = new CsPriSectionEntity
#define Delete_Section(s)           SAFE_DELETE( (CsPriSectionEntity*)(s) )

#define Create_Session(s)            s = new CsPriSessionEntity
#define Delete_Session(s)            SAFE_DELETE( (CsPriSectionEntity*)(s) )

#define Section_To_Descriptor( s )   ((CsSectionDescriptor)s)
#define Session_To_Descriptor( s )   ((CsSessionDescriptor)s)

#define Descriptor_To_Section( s )   ((CsPriSectionEntity*)s)
#define Descriptor_To_Session( s )   ((CsPriSessionEntity*)s)

#define Add_Counter( c )             MACRO_START\
                                     {\
                                         TS_Lock sslock( &num_type_lock_ );\
                                         c++;\
                                     }MACRO_STOP

#define Less_Counter( c )            MACRO_START\
                                     {\
                                         TS_Lock sslock( &num_type_lock_ );\
                                         c--;\
                                         if ( c < 0 )\
                                         {\
                                             c = 0;\
                                         }\
                                     }MACRO_STOP


        pfstl::collectpool<CsPriSectionEntity>               s_SectionRecycle;    //
        pfstl::collectpool<CsPriSessionEntity>               s_SessionRecycle;    //


        namespace Utility
        {
            //=============================================================================
            //  
            //    取得所有local建立的連線
            //=============================================================================
            void GetAllMasterSesseionEntity( _IN pfstl::vmapl<CsSessionEntity*>* pSessionContainer, _OUT std::vector<CsSessionEntity>& vec )
            {
                for ( uint_t i = 0; i < pSessionContainer->vsize(); i++ )
                {
                    TS_Lock lock( &pSessionContainer->lock( i ) );
                    _vmapl_iterator( CsSessionEntity* ) miter = pSessionContainer->begin( i );
                    while ( miter != pSessionContainer->end( i ) )
                    {
                        CsSessionEntity* cnode = ( CsSessionEntity* )miter->second;
                        if ( System::GetSessionIsMaster( miter->second ) )
                        {
                            CsSessionEntity snode;
                            memcpy( &snode, cnode, sizeof( CsSessionEntity ) );
                            vec.push_back( snode );
                        }
                        miter++;
                    }
                }
            }

            //=============================================================================
            //
            //  送訊息到 section
            //=============================================================================
            inline int SendMessageToSection( _IN CsSectionEntity* sentity, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len )
            {
                CsPriSectionEntity* section = ( CsPriSectionEntity* )sentity;
                _cs_char_t buff[ 12 ];
                addrs_t addrs;
                net::Socket::SetAddress( net::Socket::IPN4ToA4( net::Socket::IPN6ToN4( section->sym_addr[ section->connect_ip ] ) ).c_str(), itoa( section->scs_port, buff, 10 ), ( sockaddr* )&addrs, 0, SOCK_DGRAM );
                return section->m_pSectionSocket->SendPacket( ( sockaddr* )&addrs, from, cid, bp, len );
            }

            //=============================================================================
            //
            //  送訊息到 section
            //=============================================================================
            inline int SendMessageToSession( _IN CsSessionEntity* sentity, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len )
            {
                CsPriSessionEntity* session = ( CsPriSessionEntity* )sentity;
                return SendMessageToSection( session->m_pSectionEntity, from, cid, bp, len );
            }

            //=============================================================================
            //  
            //  blocal = 1  告知所有的section   我身上有的session    ___IF_SECTION_LOCAL  除了我自己
            //  blocal = 0  通知新加入的section 我的有哪些session   !___IF_SECTION_LOCAL  只有我自己
            //  method = 1   加入 2 移除
            //=============================================================================
            inline void PublishMySession( _IN pfstl::vmapl<CsSectionEntity*>* section_container, cs_section_uid_t sectionid, _IN const CsSessionEntity* buff, _IN _cs_size_t len, int method )
            {
                _csbinbuf buf;
                CsSCSCommand::InsertRemoveMaster::Client::SetRequest( buf, buff, len );
                channel_uid_t channel = ( method == 1 )?CsC_S_INSERT_CLONE_REQUEST:CsC_S_REMOVE_CLONE_REQUEST;

                if ( sectionid == CsINVALID_ID )
                {
                    for ( uint_t i = 0; i < section_container->vsize(); i++ )
                    {
                        TS_Lock lock( &section_container->lock( i ) );
                        _vmapl_iterator( CsSectionEntity* ) it = section_container->begin( i );
                        while ( !section_container->miss( i, it ) )
                        {
                            if ( !___IF_SECTION_LOCAL( it->second ) )
                            {
                                // 2011 02 08 如果阻擋的話 像demon 這種特化 server 將會無法收到
                                //if(___IF_HOST_VISIBLE(it->second->GetHost()))
                                //it->second->SendPacket( from, sessionid, bp, len );
                                SendMessageToSection( it->second, CsINVALID_ID, channel, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                            }
                            it++;
                        }
                    }
                }
                else
                {
                    TS_Lock lock( &section_container->lock( sectionid ) );
                    _vmapl_iterator( CsSectionEntity* ) it = section_container->find( sectionid );
                    if ( !section_container->miss( sectionid, it ) )
                    {
                        SendMessageToSection( it->second, CsINVALID_ID, channel, _CsGetResBufPtr( buf ), _CsGetResBufLen( buf ) );
                    }
                }

                return;
            }

        }


        //////////////////////////////////////////////////////////////////////////
        //
        //======================================================================//
        //
        //
        //======================================================================//
        System::System( SystemDelegate* pDelegate )
        {
            delegate_ = pDelegate;
            server_id_ = CsINVALID_ID;
            //m_pSCSServer = new cyberspace::scs::SectionServer( this );
            timeout_polling_ = 5000000;
            log_mode_ = 0;
            SET_BIT_FLAG( log_mode_, LOGMODE_OUTDEBUG );
            SET_BIT_FLAG( log_mode_, LOGMODE_SAVEFILE );
        }

        //======================================================================//
        //
        //
        //======================================================================//
        System::~System()
        {
            delegate_ = NULL;
            Shutdown();
            //m_CollectPool.destroy();
            //SAFE_DELETE( m_pSCSServer );
        }

        //======================================================================//
        //
        //
        //======================================================================//
        int System::StartUp( ushort_t port )
        {
            Shutdown();
            is_alive_ = true;
            sections_.init( _CsMAX_DIVISION );
            sections_.init( _CsMAX_DIVISION );
            section_agent_ = new CsAbtpPrSocket;
            section_agent_->Create();
            int ret = section_agent_->Bind( port );
            if ( ret > 0 )
            {
                thread_Id_ = pfkrl::Thread::BeginProcess( PollingProcess, this, "SCSSystemPolling" );
            }
            return ret;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        void System::Shutdown()
        {
            bool bc = false;
            is_alive_ = false;
            num_master_ = 0;
            num_clone_ = 0;
            server_id_ = CsINVALID_ID;

            //delegate_ = NULL;

            // 關閉過了
            if ( section_agent_ == NULL )
            {
                return;
            }
            section_agent_->Close();
            SAFE_DELETE( section_agent_ );

            // 清除所有的section  資料
            for ( unsigned i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) section_iter = sections_.begin( i );
                while ( !sections_.miss( i, section_iter ) )
                {
                    CsPriSectionEntity* prisection = ( CsPriSectionEntity* )section_iter->second;
                    prisection->m_pSectionSocket->Close();
                    scs::s_SectionRecycle.release( ( CsPriSectionEntity* )section_iter->second );
                    //Delete_Section( section_iter->second );
                    section_iter++;
                }
                sections_.clear( i );
            }
            sections_.destroy();


            for ( unsigned i = 0; i < sessions_.vsize(); i++ )
            {
                TS_Lock lock( &sessions_.lock( i ) );
                _vmapl_iterator( CsSessionEntity* ) session_iter = sessions_.begin( i );
                while ( !sessions_.miss( i, session_iter ) )
                {
                    scs::s_SectionRecycle.release( ( CsPriSectionEntity* )session_iter->second );
                    session_iter++;
                }
                sessions_.clear( i );
            }
            sessions_.destroy();

            if ( bc )
            {
                Pf_INFO( "scs shutdown" );
            }
        }

        //======================================================================//
        //
        //
        //======================================================================//
        unsigned short System::GetServicePort()
        {
            return section_agent_->GetPort();
        }

        //======================================================================//
        //
        //
        //======================================================================//
        _cs_size_t System::GetSectionSize()
        {
            return sections_.size();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        _cs_size_t System::GetConnectSize( CseCONNECT_TYPE t )
        {
            TS_Lock sslock( &num_type_lock_ );
            if ( t == CONNECTTYPE_MASTER )
            {
                return num_master_;
            }
            if ( t == CONNECTTYPE_CLONE )
            {
                return num_clone_;
            }
            return num_master_ + num_clone_;;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        cs_section_uid_t System::GetSectionId()
        {
            return server_id_;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        cs_section_uid_t System::GetSectionId( const _cs_char_t* servername )
        {
            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    CsSectionEntity* section = it->second;
                    if ( strcmp( section->functionname, servername ) == 0 )
                    {
                        return section->sectionid;
                    }
                    it++;
                }
            }
            return 0;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        CsSectionDescriptor System::GetSectionDescriptor( _IN cs_section_uid_t sectionid )
        {
            TS_Lock lock( &sections_.lock( sectionid ) );
            _vmapl_iterator( CsSectionEntity* ) it = sections_.find( sectionid );
            if ( !sections_.miss( sectionid, it ) )
            {
                Section_To_Descriptor( it->second );
            }
            return NULL;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        cs_section_uid_t System::GetSectionSectionId( const CsSectionDescriptor section )
        {
            return Descriptor_To_Section( section )->sectionid;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        _cs_string System::GetSectionSectionName( CsSectionDescriptor section )
        {
            return Descriptor_To_Section( section )->hostname;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        //bool System::GetSectionInfo( cs_section_uid_t sectionid, CsSectionEntity& info )
        //{
        //    TS_Lock lock( &sections_.lock( sectionid ) );
        //    _vmapl_iterator( CsSectionEntity* ) it = sections_.find( sectionid );
        //    if ( !sections_.miss( sectionid, it ) )
        //    {
        //        CsSectionEntity* node = ( CsSectionEntity* )( it->second );
        //        memcpy( &info, node, _cssizeof( CsSectionEntity ) );
        //        return true;
        //    }
        //    return false;
        //}

        //=============================================================================
        //
        //
        //=============================================================================
        void System::GetAllSectionInfo( std::vector<ServerInfo>& section_list )
        {
            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    CsSectionEntity* section = it->second;
                    // 2011 02 08 這邊不擋 才能取到 設定為隱藏的 server 資料
                    //if(___IF_HOST_VISIBLE(it->second->GetHost()))
                    //{
                    ServerInfo snode;

                    snode._sid = section->sectionid;
                    snode._port =  section->pub_port;
                    snode._maxconnect =  section->limitconnect;
                    snode._usedconnect = section->usedconnect;
                    snode._limitconnect = section->limitconnect;
                    snode._state = section->_state;
                    
                    strcpy( snode._hostname, section->hostname );
                    strcpy( snode._aliasname, section->aliasname );

                    snode._hostlen = section->sym_addrlen;
                    for ( uint_t h = 0; h < section->sym_addrlen; h++ )
                    {
                        //printf( "List server information [%s] to client!!\n", hostnode->hostname );
                        ipa_t ipah = net::Socket::IPN6ToA6( section->sym_addr[ h ] );
                        strcpy( snode.GetIp(h), ipah.c_str() );
                    }
                    section_list.push_back( snode );
                    //}
                    it++;
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////
        //        
        //======================================================================//
        //
        // DESC : 通知大家 加入一台 伺服器
        //======================================================================//
        int System::InsertSection( const CsSectionEntity* section )
        {
            if ( section == NULL )
            {
                return -1;
            }

            if ( IsCsInvalidId( section->sectionid ) )
            {
                return -1;
            }

            CsPriSectionEntity* insert_section = s_SectionRecycle.create( _MAX_RECYCEL_CAN_USE_TIME );
            memcpy( insert_section, section, sizeof( CsSectionEntity ) );
            insert_section->SetEnable( true );

            int ret = 0;
            //_cs_char_t msgstr[ _MAX_MSG ]; msgstr[0] = 0;
            //_cs_char_t buff[ 80 ];
            _cs_string outputstr;

            //cs_section_uid_t fsid = cyberspace::uid_generator::GetSocketAddrUID( net::Socket::I section->sym_addr ) section->sectionid;
            //cs_section_uid_t lsid = delegate_->OnSCSGetLocalServerUID();
            // 把我機器上面的master告訴新加入的那台

            // 先是丟訊息
            for ( uint_t h = 0; h < insert_section->sym_addrlen; h++ )
            {
                ipa_t connecnt_ipa = net::Socket::IPN4ToA4( net::Socket::IPN6ToN4( insert_section->sym_addr[ h ] ) );
                int ret = section_agent_->Connect( connecnt_ipa.c_str(), insert_section->scs_port );
                // 有回應
                if ( ret >= 0 )
                {
                    _pf_tstrprintfa( outputstr, "|| Ip   : %s\n", connecnt_ipa.c_str() );
                    // 由這邊設定server id
                    insert_section->connect_ip = h;
                    {
                        TS_Lock lock( &sections_.lock( insert_section->sectionid ) );
                        bool bconnected = false;
                        // 先找找看 server之前有沒有加入過
                        // 有的話當他是更新資料
                        _vmapl_iterator( CsSectionEntity* ) it = sections_.find( insert_section->sectionid );
                        if ( !sections_.miss( insert_section->sectionid, it ) )
                        {
                            //_stprintf(msgstr, "\n===========================================================================\n");  outputstr+=msgstr;
                            CsSectionEntity* old_section = it->second;
                            sections_.erase( it );
                            s_SectionRecycle.recycle( ( CsPriSectionEntity* )old_section );
                            _pf_tstrprintfa( outputstr, "||  Update server[ Id : %llu ] ==>> update NULL exist server.....successful\n", section->sectionid );
                        }
                        else
                        {
                            // 沒有就加入 並且建立一個agnet連結過去
                            _pf_tstrprintfa( outputstr, "||  Join server[ Id : %llu ] \n", section->sectionid );
                            _pf_tstrprintfa( outputstr, "||  name   : %s\n", section->hostname );
                            _pf_tstrprintfa( outputstr, "||  service: %s\n", section->functionname );
                        }
                        sections_.insert( insert_section->sectionid, ( CsSectionEntity* )insert_section );

                        if ( ___IF_SECTION_LOCAL( insert_section ) )
                        {
                            _pf_tstrprintfa( outputstr, "||  ==>> Join local server....successful\n" );

                            server_id_ = insert_section->sectionid;
                        }
                        else
                        {
                            _pf_tstrprintfa( outputstr, "||  ==>> Join nonlocal server....successful\n" );
                        }
                    }

                    // 如果這台是本地 把我機器上面的master告訴所有其他的機器
                    // 如果這台是他地 把我機器上面的master告訴新加入的那台
                    {
                        std::vector<CsSessionEntity> cnodevec;
                        Utility::GetAllMasterSesseionEntity( &sessions_, cnodevec );
                        if ( !cnodevec.empty() )
                        {
                            const CsSessionEntity* pConnecBuff = NULL;
                            pConnecBuff = &( cnodevec[ 0 ] );
                            Utility::PublishMySession( &sections_, ___IF_SECTION_LOCAL( insert_section )?CsINVALID_ID:insert_section->sectionid, pConnecBuff, cnodevec.size(), 1 );
                        }
                    }

                    ret = 1;
                    break;
                }
            }
            _pf_tstrprintfa( outputstr, "\n||===========================================================================\n||===========================================================================\n" );
            cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, outputstr.c_str() );

            if ( delegate_ )
            {
                delegate_->OnSCSJoinSection( Section_To_Descriptor( insert_section ) );
            }

            return ret;
        }

        //======================================================================//
        //
        // 通知大家 移除一台 伺服器
        //======================================================================//
        int System::RemoveSection( cs_section_uid_t sectionid )
        {
            if ( IsCsInvalidId( sectionid ) )
            {
                return -1;
            }

            //_cs_char_t msgstr[ _MAX_MSG ];
            _cs_string outputstr;
            CsPriSectionEntity* remove_section = NULL;
            // 把這台機器的 section
            {
                TS_Lock lock( &sections_.lock( sectionid ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.find( sectionid );
                if ( !sections_.miss( sectionid, it ) )
                {
                    remove_section = ( CsPriSectionEntity* )it->second;
                    sections_.erase( it );
                }
            }

            // 把這台機器的 session都移除
            {
                for ( unsigned i = 0; i < sessions_.vsize(); i++ )
                {
                    std::vector<cs_session_uid_t> session_id;
                    TS_Lock lock( &sessions_.lock( i ) );
                    _vmapl_iterator( CsSessionEntity* ) session_iter = sessions_.begin( i );
                    while ( !sessions_.miss( i, session_iter ) )
                    {
                        if ( session_iter->second->sectionid == sectionid )
                        {
                            session_id.push_back( session_iter->first );
                            scs::s_SessionRecycle.recycle( ( CsPriSessionEntity* )session_iter->second );
                        }
                        session_iter++;
                    }
                    for ( int i = 0; i < session_id.size(); i++ )
                    {
                        sessions_.erase( session_id[ i ] );
                    }
                }
            }

            if ( remove_section != NULL )
            {
                _pf_tstrprintfa( outputstr, "===========================================================================\n" );
                _pf_tstrprintfa( outputstr, "||  Exit server [ Id : %llu ]\n", remove_section->sectionid );
                _pf_tstrprintfa( outputstr, "||  name: %s\n", remove_section->hostname );
                _pf_tstrprintfa( outputstr, "||  Service: %s\n", remove_section->functionname );
                _pf_tstrprintfa( outputstr, "||  ==>> exit server....successful\n" );
                _pf_tstrprintfa( outputstr, "===========================================================================\n" );
                remove_section->SetEnable( false );
                s_SectionRecycle.recycle( ( CsPriSectionEntity* )remove_section );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, outputstr.c_str() );
            }             

            if ( delegate_ )
            {
                delegate_->OnSCSExitSection( Section_To_Descriptor( remove_section ) );
            }

            return 1;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        int System::UpdateSection( CsSectionEntity& section )
        {
            if ( delegate_ == NULL )
            {
                return -1;
            }

            _csbinbuf buf;
            memset( &section, 0, _cssizeof( CsSectionEntity ) );
            section.createtime = delegate_->DoGetCreateTime();
            delegate_->DoGetLocalHostname( section.hostname, _MAX_HOST_NAME );
            strcpy( section.objectname, delegate_->DoGetObjectName() );
            strcpy( section.functionname, delegate_->DoGetLocalFunctionname() );
            strcpy( section.aliasname, delegate_->DoGetLocalAliasname() );
            //section.sectionid = 0;
            section.maxprocess = ( ushort_t )pfkrl::GetNumberOfProcessors();
            section.limitconnect = delegate_->DoGetLocalLimitConnectSize();
            section.usedconnect = delegate_->DoGetLocalConnectedSize();
            section.maxmemorysize = pfkrl::GetPhysMemorySize();//memstat.dwTotalPhys;
            section.usedmemorysize = pfkrl::GetAvailPhysMemorySize();
            section._state = delegate_->DoGetState();

            std::vector<EthIF> ethif_list;
            net::Socket::GetEthIFInfo( ethif_list );
            //while ( 1 )
            section.sym_addrlen = __min( ethif_list.size(), _MAX_IPNSIZE );
            for ( int i = 0; i < section.sym_addrlen; i++ )
            {
                section.sym_addr[ i ] = net::Socket::GetSockAddrIPN6( ( sockaddr* )&ethif_list[ i ].unicast_addrs[ 0 ] );
            }
            section.pub_port = delegate_->DoGetLocalServicePort();
            section.scs_port = GetServicePort();
            return 1;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        bool System::CreateSession( cs_session_uid_t sessionid, CsAbtpCnSocket* scn )
        {
            if ( IsCsInvalidId( sessionid ) )
            {
                char buff[ 256 ];
                _stprintf( buff, " Create invalid sessionid\n" );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
                return false;
            }

            // 不管3721先刪再說
            if ( DeleteSession( sessionid ) )
            {
                // 表示有重複建立
                // 數不數 要出個警告?
            }

            CsPriSessionEntity* create_session = s_SessionRecycle.create( _MAX_RECYCEL_CAN_USE_TIME );
            {
                create_session->sessionid = sessionid;
                create_session->sectionid  = server_id_;
                create_session->createsec = pftml::Time::GetSTime();
                sockaddr_storage addr_storage;
                if ( scn != NULL )
                {
                    sockaddr* addr = scn->GetPeerAddr( addr_storage );
                    create_session->ipn6 = net::Socket::GetSockAddrIPN6( addr );
                    create_session->port = net::Socket::GetSockAddrPort( addr );
                }
                else
                {
                    memset( &create_session->ipn6, sizeof(ipn6_t), 0 );
                    create_session->port = 0;
                }
                create_session->m_pSessionSocket = scn;
                //create_session->flag = sdp->flag;
                TS_Lock lock( &sections_.lock( create_session->sectionid ) );
                _vmapl_iterator( CsSectionEntity* ) section_iter = sections_.begin( create_session->sectionid );
                if ( !sections_.miss( create_session->sectionid, section_iter ) )
                {
                    create_session->m_pSectionEntity = ( ( CsPriSectionEntity* )section_iter->second );
                }
                scn->SetDescriptor( Session_To_Descriptor( create_session ) );
            }
            
            // 通知除了我自己以外的其他server 有一個session 加入
            {                
                Utility::PublishMySession( &sections_, CsINVALID_ID, create_session, 1, 1 );
            }

            {
                TS_Lock lock( &sessions_.lock( create_session->sessionid ) );
                create_session->SetEnable( true );
                sessions_.insert( create_session->sessionid, create_session );
            }

            {
                Add_Counter( num_master_ );
            }

            if ( delegate_ )
            {
                delegate_->OnSCSJoinSession( Session_To_Descriptor( create_session ) );
            }

            {
                // 新加入的
                // 這裡保證 加入master的一定會是 sectionid = localsid
                char buff[ 256 ];
                _stprintf( buff, " Create sessionid=%llu on local\n", sessionid );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
            }

            return true;                
        }

        //======================================================================//
        //
        //
        //======================================================================//
        bool System::DeleteSession( cs_session_uid_t sessionid )
        {
            if ( IsCsInvalidId( sessionid ) )
            {
                char buff[ 256 ];
                _stprintf( buff, " Delete invalid sessionid\n" );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
                return false;
            }

            //sdp->SetPrivateFlagBit( _CONNECTNODE_INVALID, true );
            // 因為經過 m_pSCSServer->RemoveClient s就會被消滅 所以 在這邊先複製
            //memcpy(&conn, sd, _cssizeof(CsSessionEntity));
            //CsSessionEntity conn;
            //sdp->Copy( conn );
            CsPriSessionEntity* delete_session = NULL;
          
            {
                TS_Lock lock( &sessions_.lock( sessionid ) );
                _vmapl_iterator( CsSessionEntity* ) miter = sessions_.find( sessionid );
                if ( !sessions_.miss( sessionid, miter ) )
                {
                    // 取出原本已經存在的資料
                    delete_session = ( CsPriSessionEntity*)miter->second;
                    sessions_.erase( miter );
                    // 正常的狀態 原本就是本地的socket 應該是主動踢除Master
                }
                else
                {
                    // 原本不存在? 被動喚起的 被 arbit 拿掉了
                    // 如果 server當掉的狀況下 有可能 不能通知別人 但起碼要清一下DB 所以還是 傳ture
                    // Master + Clone都沒有的狀況下 就代表 這個FUNCTON是被別的SERVER剔除的時候 喚起的
                    // 1. 被別台機器要求剔除?
                    // 2. 上次登入錯誤
                    return false;
                }
            }

            // 丟到回收桶裡的
            if ( !delete_session->GetEnable() )
            {
                return false;
            }

            // 通知其他人刪除 該session
            {
                Utility::PublishMySession( &sections_, CsINVALID_ID, delete_session, 1, 2 );
            }

            if ( GetSessionIsMaster( Session_To_Descriptor( delete_session ) ) )
            {
                Less_Counter( num_master_ );
            }
            else
            {
                Less_Counter( num_clone_ );
            }

            if ( delegate_ )
            {
                delegate_->OnSCSExitSession( Session_To_Descriptor( delete_session ) );
            }

            delete_session->SetEnable( false );
            scs::s_SectionRecycle.recycle( ( CsPriSectionEntity* )delete_session );

            {
                char buff[ 256 ];
                _stprintf( buff, " Delete master sessionid=%llu on local\n", delete_session->sessionid );
                cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
            }
           
            return false;
        }

        //=============================================================================
        //
        // 收到別人加入一個連線的處理
        //=============================================================================
        void System::OnInsertSession( const CsSessionEntity* buff, _cs_size_t len )
        {
            for ( uint_t i = 0; i < len; i++ )
            {
                // 先刪再說
                OnRemoveSession( buff, len );
                CsPriSessionEntity* insert_session = s_SessionRecycle.create( _MAX_RECYCEL_CAN_USE_TIME );
                {
                    memcpy( &insert_session, &( buff[ i ] ), _cssizeof( CsSessionEntity ) );
                    insert_session->m_pSessionSocket = NULL;
                    //insert_session->flag = sdp->flag;
                    TS_Lock lock( &sections_.lock( insert_session->sectionid ) );
                    _vmapl_iterator( CsSectionEntity* ) section_iter = sections_.begin( insert_session->sectionid );
                    if ( !sections_.miss( insert_session->sectionid, section_iter ) )
                    {
                        insert_session->m_pSectionEntity = ( ( CsPriSectionEntity* )section_iter->second );
                    }
                }

                {
                    TS_Lock lock( &sessions_.lock( insert_session->sessionid ) );
                    insert_session->SetEnable( true );
                    sessions_.insert( insert_session->sessionid, insert_session );
                }

                {
                    Add_Counter( num_clone_ );
                }

                if ( delegate_ )
                {
                    delegate_->OnSCSJoinSession( Session_To_Descriptor( insert_session ) );
                }

                {
                    // 新加入的
                    // 這裡保證 加入master的一定會是 sectionid = localsid
                    char buff[ 256 ];
                    _stprintf( buff, " Insert sessionid=%llu sectionid=%llu\n", insert_session->sessionid, insert_session->sectionid );
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
                }
            }
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void System::OnRemoveSession( const CsSessionEntity* buff, _cs_size_t len )
        {
            for ( uint_t i = 0; i < len; i++ )
            {
                cs_session_uid_t delete_session_id = (&( buff[ i ] ))->sessionid;
                if ( IsCsInvalidId( delete_session_id ) )
                {
                    char buff[ 256 ];
                    _stprintf( buff, " Remove invalid sessionid\n" );
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
                    continue;
                }
                
                CsPriSessionEntity* remove_session = NULL;
                {
                    TS_Lock lock( &sessions_.lock( delete_session_id ) );
                    _vmapl_iterator( CsSessionEntity* ) miter = sessions_.find( delete_session_id );
                    if ( !sessions_.miss( delete_session_id, miter ) )
                    {
                        // 取出原本已經存在的資料
                        remove_session = ( CsPriSessionEntity* )miter->second;
                        sessions_.erase( miter );
                    }
                    else
                    {
                        continue;
                    }
                }

                // 丟到回收桶裡的
                if ( !remove_session->GetEnable() )
                {
                    return;
                }

                if ( GetSessionIsMaster( Session_To_Descriptor( remove_session ) ) )
                {
                    Less_Counter( num_master_ );
                }
                else
                {
                    Less_Counter( num_clone_ );
                }

                remove_session->SetEnable( false );
                scs::s_SectionRecycle.recycle( ( CsPriSectionEntity* )remove_session );

                {
                    char buff[ 256 ];
                    _stprintf( buff, " Remove sessionid=%llu sectionid=%llu\n", remove_session->sessionid, remove_session->sectionid );
                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
                }
            }
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        CsSessionDescriptor System::GetSessionDescriptor( cs_session_uid_t sessionid )
        {
            //_vecmap_sector(CsSessionEntity*) sect = sessions_.findsector(sessionid);
            //if(!sect.empty())
            {
                TS_Lock lock( &sessions_.lock( sessionid ) );
                _vmapl_iterator( CsSessionEntity* ) miter = sessions_.find( sessionid );
                if ( !sessions_.miss( sessionid, miter ) )
                {
                    if ( miter->second->sessionid != sessionid )
                    {
                        assert( NULL && "System::GetCsPSockConnDescriptor sessionid not equal" );
                    }

                    return Session_To_Descriptor( miter->second );
                }
            }
            return NULL;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        bool System::GetSessionIsMaster( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return ( ___IF_SESSION_MASTER( Descriptor_To_Session(sdp) ) );
            }

            assert( 0 && "SCSSystem GetSessionIsMaster descriptor can't be NULL" );
            return false;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        bool System::GetSessionIsClone( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return ( !___IF_SESSION_MASTER( Descriptor_To_Session( sdp ) ) );
            }

            assert( 0 && "SCSSystem GetSessionIsClone descriptor can't be NULL" );
            return false;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        bool System::GetSessionIsInvalid( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                CsPriSessionEntity* session_entity = Descriptor_To_Session( sdp );
                return session_entity->GetEnable();
            }

            assert( 0 && "SCSSystem GetSessionIsClone descriptor can't be NULL" );
            return false;
        }
        //=============================================================================
        //
        //
        //=============================================================================
        cs_session_uid_t System::GetSessionSessionUID( CsSessionDescriptor sdp )
        {
            if ( sdp == NULL )
            {
                return CsINVALID_ID;
            }

            return Descriptor_To_Session( sdp )->sessionid;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        ipn6_t System::GetSessionIpn6( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return Descriptor_To_Session( sdp )->ipn6;
            }
            ipn6_t ipn;
            memset( &ipn, 0, _cssizeof( ipn6_t ) );
            return ipn;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        ushort_t System::GetSessionPort( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return Descriptor_To_Session( sdp )->port;
            }
            return 0;           
        }

        //=============================================================================
        //
        //
        //=============================================================================
        cs_section_uid_t System::GetSessionSectionId( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return Descriptor_To_Session( sdp )->sectionid;
                
            }
            return CsINVALID_ID;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        CsAbtpCnSocket* System::GetSessionSocket( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return Descriptor_To_Session( sdp )->m_pSessionSocket;
            }
            return NULL;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        stimen_t System::GetSessionCreateSec( CsSessionDescriptor sdp )
        {
            if ( sdp != NULL )
            {
                return Descriptor_To_Session( sdp )->createsec;
            }
            return NULL;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        //CsSessionNote* System::SetSessionNote( CsSessionEntity* sdp, CsSessionNote* note )
        //{
        //    if ( sdp != NULL )
        //    {
        //        return NULL;
        //    }
        //
        //    return NULL;
        //}

        //=============================================================================
        //
        //
        //=============================================================================
        //void System::DelSessionNote( CsSessionEntity* sdp, CsSessionNote* note )
        //{
        //    if ( sdp == NULL )
        //    {
        //        return;
        //    }
        //
        //    return NULL;
        //}

        //=============================================================================
        //
        //
        //=============================================================================
        //CsSessionNote* System::GetSessionNote( CsSessionEntity* sdp, notetid_t notetid )
        //{
        //    if ( sdp == NULL )
        //    {
        //        return NULL;
        //    }
        //
        //    return sdp->FindNote( notetid );
        //}

        int System::SendMessage( _IN CsSessionDescriptor dest, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len )
        {
            TS_Lock lock( &sections_.lock( GetSessionSessionUID( dest ) ) );
            return Utility::SendMessageToSession( dest, from, cid, bp, len );
        }

        //=============================================================================
        //
        //
        //=============================================================================
        int System::SendMessage( cs_section_uid_t dest, uniqueid_t from, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
        {
            TS_Lock lock( &sections_.lock( dest ) );
            _vmapl_iterator( CsSectionEntity* ) it = sections_.find( dest );
            if ( !sections_.miss( dest, it ) )
            {
                // 2011 02 08 如果阻擋的話 像demon 這種特化 server 將會無法收到
                //if(___IF_HOST_VISIBLE(it->second->GetHost()))
                return Utility::SendMessageToSection( it->second, from, cid, bp, len );
                //return 0;
            }

            return -1;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        int System::BroadcastMessage( uniqueid_t from, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
        {
            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    // 2011 02 08 如果阻擋的話 像demon 這種特化 server 將會無法收到
                    //if(___IF_HOST_VISIBLE(it->second->GetHost()))
                    Utility::SendMessageToSection( it->second, from, cid, bp, len );
                    it++;
                }
            }
            return len;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        int System::BroadcastMessageToOther( uniqueid_t from, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
        {
            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    if ( !___IF_SECTION_LOCAL( it->second ) )
                    {
                        // 2011 02 08 如果阻擋的話 像demon 這種特化 server 將會無法收到
                        //if(___IF_HOST_VISIBLE(it->second->GetHost()))
                        //it->second->SendPacket( from, sessionid, bp, len );
                        Utility::SendMessageToSection( it->second, from, cid, bp, len );
                    }
                    it++;
                }
            }
            return len;
        }
        
        //=============================================================================
        //
        //
        //=============================================================================
        int System::BroadcastMessageToOtherAndVisible( uniqueid_t from, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
        {
            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    if ( !___IF_SECTION_LOCAL( it->second ) )
                    {
                        // 2011 02 08 如果阻擋的話 像demon 這種特化 server 將會無法收到
                        // 因為 有些資料不能傳給 隱藏的 所以 多設立 該種廣播
                        if ( ___IF_HOST_VISIBLE( it->second->_state ) )
                        {
                            Utility::SendMessageToSection( it->second, from, cid, bp, len );
                        }
                    }
                    it++;
                }
            }
            return len;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        int System::BroadcastMessageToOtherWithObject( const _cs_char_t* objectname, uniqueid_t from, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len )
        {
            CsSectionEntity* sentity = NULL;

            {
                TS_Lock lock( &sections_.lock( server_id_ ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.find( server_id_ );
                if ( !sections_.miss( server_id_, it ) )
                {
                    sentity = ( CsSectionEntity* )( it->second );
                }
            }

            if( sentity == NULL )
                return -1;

            for ( uint_t i = 0; i < sections_.vsize(); i++ )
            {
                TS_Lock lock( &sections_.lock( i ) );
                _vmapl_iterator( CsSectionEntity* ) it = sections_.begin( i );
                while ( !sections_.miss( i, it ) )
                {
                    if ( !___IF_SECTION_LOCAL( it->second ) )
                    {
                        if ( strcmp( sentity->objectname, objectname ) == 0 )
                        {
                            //it->second->SendPacket( from, sessionid, bp, len );
                            Utility::SendMessageToSection( it->second, from, cid, bp, len );
                        }
                    }
                    it++;
                }
            }
            return len;
        }

        //======================================================================//
        //
        //
        //======================================================================//
        //void System::OnSyncSession( net::TCPSocket* skt, const CsSessionEntity* buff, _cs_size_t len )
        //{
        //    if ( delegate_ == NULL )
        //    {
        //        return;
        //    }
        //
        //    for ( uint_t i = 0; i < len; i++ )
        //    {
        //        CsSessionEntity conn;
        //        memcpy( &conn, &( buff[ i ] ), _cssizeof( CsSessionEntity ) );
        //        PeerSection* sson = ( PeerSection* )skt;
        //
        //        // id不同 來亂的壓
        //        if ( conn.sectionid != sson->sectionid )
        //        {
        //            assert( 0 && "OnInsertConneciton sectionid failure" );
        //            continue;
        //        }
        //
        //        {
        //            TS_Lock lock( &sessions_.lock( conn.sessionid ) );
        //            // 先找原來存不存在
        //            _vmapl_iterator( CsSessionEntity* ) miter = sessions_.find( conn.sessionid );
        //
        //            // 原來存在
        //            //if(miter!=sessions_.end())
        //            if ( !sessions_.miss( conn.sessionid, miter ) )
        //            {
        //                CsSessionEntity* oldced = miter->second;
        //
        //                // 原本是在自己這台機器上 表示是被別台機器要求轉把連線轉移過去
        //                // 自己－－＞別台
        //                if ( GetSessionIsClone( oldced ) )
        //                {
        //                    // 表示是更新連線狀態　單存就換新資料
        //                    //　別台－－＞別台
        //                    oldced->Initial( &conn, NULL );
        //                    oldced->SetConnectFrom( ( PeerSection* )skt );
        //                    char buff[ 256 ];
        //                    _stprintf( buff, " update CsAbtpCnSocket %lld on server [ Id : %lld ] successful\n", conn.sessionid, conn.sectionid );
        //                    cyberspace::LogFileSerivce::OutputMessage( cyberspace::g_log_index, log_mode_, buff );
        //                }
        //            }
        //        }
        //    }
        //}

        //======================================================================//
        //
        //
        //======================================================================//
        void System::OnRecvDefineRequest( uniqueid_t eid, channel_uid_t cid, ubyte_t* command, _cs_size_t len )
        {
            if ( delegate_ == NULL )
            {
                return;
            }

            switch ( cid )
            {
            case CsS_C_SCS_SEND_MESSAGE:
                {
                    cs_session_uid_t cnid = CsINVALID_ID;
                    channel_uid_t    chid = CsINVALID_ID;
                    ubyte_t*         commbuf = NULL;
                    _cs_size_t        commlen = 0;
                    CsSCSCommand::SCSSendRecvMessage::Server::GetRequest( command, cnid, chid, commbuf, commlen );
                    CsSessionDescriptor sdp = GetSessionDescriptor( cnid );
                    CsAbtpCnSocket* scn = GetSessionSocket( sdp );
                    if ( scn != NULL )
                    {
                        scn->SendPacket( eid, chid, commbuf, commlen );
                    }
                }
                break;
            case CsS_C_SCS_RECV_MESSAGE:
                {
                    cs_session_uid_t cnid = CsINVALID_ID;
                    channel_uid_t     chid = CsINVALID_ID;
                    ubyte_t*        commbuf = NULL;
                    _cs_size_t      commlen = 0;
                    CsSCSCommand::SCSSendRecvMessage::Server::GetRequest( command, cnid, chid, commbuf, commlen );
                    CsSessionDescriptor sdp = GetSessionDescriptor( cnid );
                    if ( GetSessionIsClone( sdp ) )
                    {
                        delegate_->OnSCSPassRecvMessage( sdp, eid, chid, commbuf, commlen );
                    }
                }
                break;
            case CsC_S_INSERT_CLONE_REQUEST:
                {
                    //cs_section_uid_t   sectionid;
                    CsSessionEntity* ConnVec;
                    _cs_size_t       ConnLen = 0;
                    CsSCSCommand::InsertRemoveMaster::Server::GetRequest( command, ConnVec, ConnLen );
                    //PeerSection* sson = ( PeerSection* ) skt;
                    //sson->sectionid = sectionid;
                    OnInsertSession( ConnVec, ConnLen );
                }
                break;

                // 移除一個 clinet
            case CsC_S_REMOVE_CLONE_REQUEST:
                {
                    //cs_section_uid_t   sectionid;
                    CsSessionEntity* ConnVec;
                    _cs_size_t     ConnLen = 0;
                    CsSCSCommand::InsertRemoveMaster::Server::GetRequest( command, ConnVec, ConnLen );
                    //PeerSection* sson = ( PeerSection* ) skt;
                    //sson->sectionid = sectionid;
                    OnRemoveSession( ConnVec, ConnLen );
                }
                break;
            default:
                {
                    //delegate_->OnSCSRecvDefineRequest( eid, cid, command, len );
                }
                break;
            }
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void System::PollingProcess( void* data )
        {
            System* pScsSystem = ( System* )data;
            bool brun = true;
            while ( pScsSystem->is_alive_ )
            {
                int nread = pScsSystem->section_agent_->Waiting( pScsSystem->timeout_polling_ );
                if ( nread > 0 )
                {
                    uniqueid_t    uid = CsINVALID_ID;
                    channel_uid_t command_uid = 0;
                    ubyte_t*      recv_buff = NULL;
                    _cs_size_t     command_len = 0;

                    addrs_t addrs;
                    if ( pScsSystem->section_agent_->RecvPacket( (sockaddr*)&addrs, uid, command_uid, &recv_buff, command_len ) >= 0 )
                    {
                        pScsSystem->OnRecvDefineRequest( uid, command_uid, recv_buff, command_len );
                    }
                }

            }
        }

    }
};

