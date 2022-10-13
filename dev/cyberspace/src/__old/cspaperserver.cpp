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
#include <assert.h>

#include <xmlnode.h>
#include <algorithmic/Crc32.h>

#include "CsPacketFactory.h"
#include "CsPeerSocket.h"
#include "CsLogFileExporter.h"
#include "MessageDispatcher.h"
#include "CsDBFunction.h"
#include "CsPaperCommand.h"

#if  defined(_WINDOWS)
#include <direct.h>
#endif
#if  defined(_LINUX)
#include <stdarg.h>
#endif

#include "CsPaperServer.h"
// 有 T_Lock 放最後
#include <XMySQLDB.h>

#include <Debug.h>
#include <PfSysinfo.h>
#include <PfThread.h>
#include <PfStdlib.h>


namespace Cyberspace
{

	namespace Paper
	{
		int g_MaxTb = 100;
		//  連上來的每個server用的socket
		class PeerClient: public PeerSocket
		{
		public:
			PeerClient(){};
			~PeerClient(){};
		};

		struct Evocator
		{
			Server*     pServer;
			PeerClient* pClient;
		};


		struct CallUpdate
		{
			Server*     pServer;
			int         Idx;
		};


		//===============================================================================//
		// NAME :
		// DESC :
		//===============================================================================//
		Server::Server( const tchar_t* name )
		{
			m_logfile = new CsLog::FileExporter;
			m_MessageDispatcher = new MessageDispatcher;
			m_fLogMode = 0;
			m_StartUpSec = 0;

			if ( name != NULL )
			{
				m_Name = name;
			}
			else
			{
				TCHAR szPath[ _MAX_LPATH ];
				PfSystem::GetCurrentProcessFullName( szPath, _MAX_LPATH );
				TCHAR szFName[ _MAX_LFNAME ];
				getpathfname( szFName, szPath );
				m_Name = szFName;
			}

			m_MessageDispatcher->RegisterChannel( CsC_S_REQUEST_SET_VALUE,   ( MessageChannelFunction ) DoSetValue );
			m_MessageDispatcher->RegisterChannel( CsC_S_REQUEST_SETNX_VALUE, ( MessageChannelFunction ) DoSetnxValue );
			//m_MessageDispatcher->RegisterChannel( CsC_S_REQUEST_RPL_VALUE,   ( MessageChannelFunction ) DoRplValue );
			m_MessageDispatcher->RegisterChannel( CsC_S_REQUEST_GET_VALUE,   ( MessageChannelFunction ) DoGetValue );
			m_MessageDispatcher->RegisterChannel( CsC_S_REQUEST_DEL_VALUE,   ( MessageChannelFunction ) DoDelValue );
		}

		//===============================================================================//
		// NAME :
		// DESC :
		//===============================================================================//
		Server::~Server()
		{
			//ClearAllConnection();
			Shutdown();
			m_MessageDispatcher->UnRegisterChannel( CsC_S_REQUEST_SET_VALUE );
			m_MessageDispatcher->UnRegisterChannel( CsC_S_REQUEST_SETNX_VALUE );
			//m_MessageDispatcher->UnRegisterChannel( CsC_S_REQUEST_RPL_VALUE );
			m_MessageDispatcher->UnRegisterChannel( CsC_S_REQUEST_GET_VALUE );
			m_MessageDispatcher->UnRegisterChannel( CsC_S_REQUEST_DEL_VALUE );
			SAFE_DELETE( m_logfile );
			SAFE_DELETE( m_MessageDispatcher );
		}

		//===============================================================================//
		// NAME :
		// DESC :
		//===============================================================================//
		Net::TCPSocket* Server::OnNewSocket( void )
		{
			return new PeerClient();
		}

		//===============================================================================//
		// NAME :
		// DESC :
		//===============================================================================//
		/*
		void Server::OnDelSocket(Net::TCPSocket* skt)
		{
		CsArbitHostSocket* obj = (CsArbitHostSocket*)skt;
		SAFE_DELETE(obj);
		}
		*/

		//===============================================================================//
		// NAME :
		// DESC :
		//===============================================================================//
		int Server::StartUp( unsigned int allowplayers, unsigned short port )
		{
			time_t startt = PfTime::GetSTime();
			m_StartUpSec = startt;
			srand( startt );
			//time( &startt );
			// open a log history file
			mkdir( ".\\LOG" );
			struct tm* p;
			p = localtime( &startt );
			tchar_t filename[ _MAX_PATH ];
			_stprintf( filename,
					 ".\\LOG\\ArbitServer_History_%d_%d_%d_%d_%d_%d.log",
					 p->tm_year + 1900,
					 p->tm_mon + 1,
					 p->tm_mday,
					 p->tm_hour,
					 p->tm_min,
					 p->tm_sec );
			m_logfile->Open( filename );
			Net::TCPServer::Create();
			int ret = Net::TCPServer::StartUp( allowplayers, port );
			srand( ( int ) PfTime::GetSTime() );
			//srand( ( int ) time( 0 ) );
			return ret;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		int Server::Shutdown( void )
		{
			Net::TCPServer::Shutdown();
			m_StartUpSec = 0;
			SAFE_DELETE_ARRAY( m_DBQueue );

			{
				for ( unsigned i = 0; i < m_RecStorage.vecsize(); i++ )
				{
					TS_Lock lock( &m_RecStorage.lock( i ) );
					_vmaps_iterator( SRecord* ) it = m_RecStorage.begin( i );

					while ( !m_RecStorage.miss( i, it ) )
					{
						SAFE_DELETE( it->second );
						it++;
					}

					m_RecStorage.clear( i );
				}
				m_RecStorage.destroy();
			}

			{
				for ( unsigned i = 0; i < m_TabStorage.vecsize(); i++ )
				{
					TS_Lock lock( &m_TabStorage.lock( i ) );
					_vmaps_iterator( recmap_t* ) it = m_TabStorage.begin( i );

					while ( !m_TabStorage.miss( i, it ) )
					{
						SAFE_DELETE( it->second );
						it++;
					}

					m_TabStorage.clear( i );
				}
				m_TabStorage.destroy();
			}


			//for ( uint_t i = 0; i < m_Database.size(); i++ )
			//{
			//    OnDeleteDataBase( m_Database[ i ] );
			//}
			m_Database.clear();
			return 1;
		}

		//===============================================================================//
		//
		//
		//===============================================================================//
		bool Server::LoadConfigure( const tchar_t* filename )
		{
			bool loadret = true;
			XMLNode xml;

			if ( xml.LoadFromFile( filename ) )
			{
				XMLNode* GroupCfg = xml.FindFirstNode( _T( "PaperGroup" ) );

				if ( GroupCfg != NULL )
				{
					unsigned int AllowedPlayers = 0;
					tchar_t host[ 255 ];
					unsigned short port = 0;
					serverid_t serverid = CsINVALID_ID;
					int max_storage = 0;
					int db_max_connections = m_MaxDB;
					tstring db_name;
					tstring db_hostname;
					tstring db_username;
					tstring db_password;
					tstring db_charset;
					unsigned short db_port = 0;
					//////////////////////////////////////////////////////////////////////////
					// Read configure file
					{
						XMLNode* ServerCfg = GroupCfg->FindFirstNode( _T( ObjectName() ) );

						while ( ServerCfg )
						{
							XMLAttribute* pa = ServerCfg->FindAttribute( _T( "name" ) );

							if ( ( m_Name == "" ) || ( pa == NULL ) )
							{
								break;
							}
							else if ( pa->m_Value == m_Name.c_str() )
							{
								break;
							}

							ServerCfg = xml.FindNextNode( _T( ObjectName() ) );
						}

						//if(ServerCfg.m_Text.c_str()!=NULL)
						if ( ServerCfg )
						{
							for ( int ni = 0; ni < ServerCfg->GetNodeSize(); ni++ )
							{
								XMLNode* SettingNode = ServerCfg->FindNode( ni );

								if ( SettingNode != NULL )
								{
									if ( SettingNode->m_Name == _T( "Id" ) )
									{
										SettingNode->m_Text >> serverid;
									}
									else if ( SettingNode->m_Name == _T( "MaxNodes" ) )
									{
										SettingNode->m_Text >> AllowedPlayers;
									}
									else if ( SettingNode->m_Name == _T( "Port" ) )
									{
										SettingNode->m_Text >> port;
									}
									else if ( SettingNode->m_Name == _T( "MaxStorage" ) )
									{
										SettingNode->m_Text >> max_storage;
									}
									else if ( SettingNode->m_Name == _T( "Database" ) )
									{
										XMLNode* DBCfg = SettingNode;
										//( *DBCfg )[ _T( "MaxConnect" ) ].m_Text >> db_max_connections;
										db_name = ( *DBCfg )[ _T( "DBName" ) ].m_Text;
										db_hostname = ( *DBCfg )[ _T( "Hostname" ) ].m_Text;
										( *DBCfg )[ _T( "Port" ) ].m_Text >> db_port;
										db_username = ( *DBCfg )[ _T( "Username" ) ].m_Text;
										db_password = ( *DBCfg )[ _T( "Password" ) ].m_Text;
										db_charset = ( *DBCfg )[ _T( "CharSet" ) ].m_Text;
									}
								}
							}
						}
						else
						{
							SetLogMessage( LOGMODE_PRINTOUT, ">> Can't find server configure node!!!\n" );
							goto _LOADCONFIGURE_EXIT;
						}
					}
					tstring logstring;
					tchar_t  logbuf[ _MAX_MSG ];
					_stprintf( logbuf, "||===========================================================================\n" );
					logstring += logbuf;
					_stprintf( logbuf, "||  Cyberspace server 【 %s 】 initiating........\n", ObjectName() );
					logstring += logbuf;
					_stprintf( logbuf, "||       build:  %s %s\n||\n", __DATE__, __TIME__ );
					logstring += logbuf;

					if ( StartUp( AllowedPlayers, port ) != 1 )
					{
						SetLogMessage( LOGMODE_PRINTOUT, _T( ">> start up failure close this server....\n" ) );
						SetLogMessage( LOGMODE_PRINTOUT, _T( "||===========================================================================\n\n" ) );
						goto _LOADCONFIGURE_EXIT;
					}

					time_t starttime = 0;
					_stprintf( logbuf, _T( "|| Start on %s" ), ctime( &starttime ) );
					logstring += logbuf;
					SetLogMessage( LOGMODE_PRINTOUT, logstring.c_str() );
					SetLogMessage( LOGMODE_PRINTOUT, _T( "||---------------------------------------------------------------------------\n" ) );
					uint_t  ipcount = 0;
					ipn4_t   ipn;
					memset( &ipn, 0, sizeof( ipn4_t ) );
					m_Id = serverid;

					if ( GetLocalServerId() == 0 )
					{
						SetLogMessage( LOGMODE_PRINTOUT, _T( ">> Server Id can't be 0!!...\n" ) );
						goto _LOADCONFIGURE_EXIT;
					}

					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| server_uid : %d\n" ), GetLocalServerId() );
					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| HostName : %s\n" ), GetLocalHostname( host, 255 ) );
					ipn = GetLocalIPAddress( ipcount );

					while ( ipn.s_addr != 0 )
					{
						SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Ip %d : %s\n" ), ipcount, Net::Socket::IPN4ToA4( GetLocalIPAddress( ipcount ) ).c_str() );
						ipcount++;
						ipn = GetLocalIPAddress( ipcount );
					}

					if ( ipcount == 0 )
					{
						SetLogMessage( LOGMODE_PRINTOUT, _T( ">> Can't find any ip!!...\n" ) );
						goto _LOADCONFIGURE_EXIT;
					}

					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Port : %d\n" ), port );
					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Max allowed connect = %d\n" ), AllowedPlayers );
					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Max strage connect = %d\n" ), max_storage );
					m_RecStorage.init( max_storage );
					m_TabStorage.init( max_storage );
					//
					//tstring remoteportname = (*ServerCfg)[_T("RemotePort")].m_Text.c_str();
					//////////////////////////////////////////////////////////////////////////
					m_MaxDB = GetPollingCount() * 2;
					db_max_connections = m_MaxDB;
					//////////////////////////////////////////////////////////////////////////
					//Open Database
					{
						SetLogMessage( LOGMODE_PRINTOUT, _T( "||  Search for function database...." ) );

						// clear database object first
						for ( uint_t i = 0; i < m_Database.size(); i++ )
						{
							OnDeleteDataBase( m_Database[ i ] );
						}

						m_Database.clear();

						//int max_connections = atoi(DBCfg[_T("MaxConnect")].m_Text.c_str());
						for ( int i = 0; i < db_max_connections; i++ )
						{
							m_Database.push_back( OnCreateDataBase() );
						}

						if ( m_Database.empty() )
						{
							SetLogMessage( LOGMODE_PRINTOUT, _T( ">> No database object on %s!!!\n" ), db_name.c_str() );
						}
						else
						{
							int conncnt = 0;

							if ( !db_charset.empty() )
							{
								//DBUtility::dbCharSet(m_Database[0], ACDBCfg[_T("CharSet")].m_Text.c_str());
								SetLogMessage( LOGMODE_PRINTOUT, _T( "\n|| DBName=%s Hostname=%s Port=%d CharSet=%s " ), db_name.c_str(), db_hostname.c_str(), db_port, db_charset.c_str() );
							}
							else
							{
								SetLogMessage( LOGMODE_PRINTOUT, _T( "\n|| DBName=%s Hostname=%s Port=%d " ), db_name.c_str(), db_hostname.c_str(), db_port );
							}

							for ( uint_t i = 0; i < m_Database.size(); i++ )
							{
								if ( !db_charset.empty() )
								{
									m_Database[ i ]->SetCharSet( db_charset.c_str() );
								}

								if ( m_Database[ i ]->Connect( db_name.c_str(), db_hostname.c_str(), db_port, db_username.c_str(), db_password.c_str() ) )
								{
									conncnt++;
								}
							}

							SetLogMessage( LOGMODE_PRINTOUT, _T( "Connected=%d/%d\n" ), conncnt, m_Database.size() );

							if ( conncnt == m_Database.size() )
							{
								SetLogMessage( LOGMODE_PRINTOUT, _T( "|| ....connect database successful\n" ) );
							}
							else
							{
								//_stprintf(message, "  ..connect database %s failure\n", DBCfg[_T("DBName")].m_Text.c_str());
								SetLogMessage( LOGMODE_PRINTOUT, _T( "|| ....connect database failure\n" ) );
								goto _LOADCONFIGURE_EXIT;
							}
						}
					}
					SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Cyberspace [ %s ] server start success.\n||===========================================================================\n" ), ObjectName() );
				}
			}
			else
			{
				SetLogMessage( LOGMODE_PRINTOUT, _T( ">> Can't load configure file!!\n" ) );
				SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Cyberspace [ %s ] server start failure.\n||===========================================================================\n" ), ObjectName() );
				goto _LOADCONFIGURE_EXIT;
			}

			if ( !m_Database.empty() )
			{
				Cyberspace::DBInit::dbCreateCpTable( m_Database[ 0 ], g_MaxTb );
				// crate update
				m_dbLock.resize( m_MaxDB );
				m_DBQueue = new std::queue<SRecord*>[ m_MaxDB ];

				for ( int i = 0; i < m_MaxDB; i++ )
				{
					CallUpdate* cp = new CallUpdate;
					cp->pServer = this;
					cp->Idx = i;
					PfSystem::Thread::BeginProcess( Execute, cp );
				}
			}

			return true;
_LOADCONFIGURE_EXIT:
			SetLogMessage( LOGMODE_PRINTOUT, _T( "|| Cyberspace [ %s ] server start falure.\n||===========================================================================\n" ), ObjectName() );
			Shutdown();
			return false;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void Server::OnAccept( Net::TCPSocket* skt )
		{
			TS_Lock lock( &m_utLock );
			// it allow connect until server initial finish
			//tchar_t host[ 255 ];
			time_t timep;
			time( &timep );
			sockaddr_in addrin;
			skt->GetAddressIn( addrin );
			ipn4_t ipn = Net::Socket::GetInternetAddr( &addrin );
			char hostname[ NI_MAXHOST ];
			char servinfo[ NI_MAXSERV ];
			Net::Socket::GetAddressNameInfo( ( struct sockaddr* )&addrin, sizeof( addrin ), hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
			SetLogMessage( LOGMODE_PRINTOUT, " \n ||================================================\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||    Join Paper Agnet\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||------------------------------------------------\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  HostName : %s\n", hostname );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Service  : %s\n", servinfo );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Time     : %s", asctime( localtime( &timep ) ) );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Ip       : %s\n", Net::Socket::IPN4ToA4( ipn ).c_str() );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||================================================\n" );
			// 設定SERVER的ID
			//SetCsServerId(skt);
			// 將目前的連線送給這台SERVER
			//SendConnectionListTo(s);
			// 要求資料
			// 第一次由節點主動送
			//SyncHostInfo(s);
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		//int Server::Kickout( Net::TCPSocket* skt )
		void Server::OnKickout( Net::TCPSocket* skt )
		{
			TS_Lock lock( &m_utLock );
			//if ( skt->GetFD() == 0 )
			//{
			//	return 0;
			//}
			//tchar_t host[ 255 ];
			time_t timep = PfTime::GetSTime();
			//time( &timep );
			sockaddr_in addrin;
			skt->GetAddressIn( addrin );
			ipn4_t ipn = Net::Socket::GetInternetAddr( &addrin );
			char hostname[ NI_MAXHOST ];
			char servinfo[ NI_MAXSERV ];
			Net::Socket::GetAddressNameInfo( ( struct sockaddr* )&addrin, sizeof( addrin ), hostname, NI_MAXHOST, servinfo, NI_MAXSERV, 0 );
			SetLogMessage( LOGMODE_PRINTOUT, " \n ||================================================\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||    Exit Paper Agent\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||------------------------------------------------\n" );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  HostName : %s\n", hostname );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Service  : %s\n", servinfo );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Time     : %s", asctime( localtime( &timep ) ) );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||  Ip       : %s\n", Net::Socket::IPN4ToA4( ipn ).c_str() );
			SetLogMessage( LOGMODE_PRINTOUT,    " ||================================================\n" );
			//TCPServer::Kickout( skt );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void Server::OnReceive( Net::TCPSocket* skt )
		{
			channelid_t  commandidx = 0;
			unsigned int recvlne    = 0;
			ubyte_t*     recvbuf    = NULL;
			uniqueid_t   eid        = -1;
			( ( PeerClient* ) skt )->RecvPacket( eid, commandidx, &recvbuf, recvlne );
			Evocator evocator;
			evocator.pServer = this;
			evocator.pClient = ( PeerClient* ) skt;
			m_MessageDispatcher->DispatchMessage( &evocator, commandidx, recvbuf, recvlne );
			//SAFE_DELETE_ARRAY( recvbuf )
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void Server::SetLogMessage( long32_t mode, const tchar_t* format, ... )
		{
			tchar_t string[ _MAX_LONG_MSG ] = { "" };
			//FILE str;
			//register FILE *outfile = &str;
			va_list arglist;
			//register int retval;
			const tchar_t* i = format;
			va_start( arglist, format );
			assert( string != NULL );
			assert( format != NULL );
			vsprintf( string, format, arglist );
			va_end( arglist );

			if ( strcmp( string, "" ) == 0 )
			{
				return;
			}

			if ( mode == LOGMODE_PRINTOUT )
			{
				printf( string );
			}

			m_logfile->SetLogMessage( string );
			return;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void Server::SetLogMode( long32_t mode )
		{
			m_fLogMode = mode;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		long32_t Server::GetLogMode( void )
		{
			return m_fLogMode;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		XDataBase*  Server::OnCreateDataBase( void )
		{
			return ( XDataBase* )new XMySqlDB;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void Server::OnDeleteDataBase( XDataBase* db )
		{
			XMySqlDB* xdb = ( XMySqlDB* ) db;
			SAFE_DELETE( xdb );
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		int Server::DoSetValue( void* evocator, const void* pdata, unsigned int len )
		{
			Server*     pserver = ( ( Evocator* ) evocator )->pServer;
			PeerClient* pclient = ( ( Evocator* ) evocator )->pClient;
			CsRecord    record;
			//tstring     keybuf;
			//ubyte_t*    valbuf = NULL;
			//int         vallen = 0;
			int         live_sec = 0;
			int         ret = PAPERRESULT_UNKNOW;
			ResBuf      oldval;
			Databuf     recvdata;
			recvdata.buf = ( ubyte_t* ) pdata;
			recvdata.len = len;
			CsPaperCommand::SetValue::Server::GetRequest( &recvdata, record, live_sec );
			mstimen_t currmsce = PfTime::GetTickMSTime();
			mstimen_t create_msce = currmsce;
			mstimen_t updaet_msce = currmsce;

			{
				TS_Lock lock( &pserver->m_RecStorage.lock( record.key ) );
				_vmaps_iterator( SRecord* ) it = pserver->m_RecStorage.find( record.key );
				SRecord* vb = NULL;
				if ( !pserver->m_RecStorage.miss( record.key, it ) )
				{
					oldval = it->second->value;
					vb = it->second;
					create_msce = vb->create_time;
					ret = PAPERRESULT_EXIST_SUCCESS;
				}
				else
				{
					vb = new SRecord;
					pserver->m_RecStorage.insert( record.key, vb );
					vb->create_time = currmsce;
					ret = PAPERRESULT_SUCCESS;
				}
				( *vb ) = record;
				vb->update_time = currmsce;
				vb->live_sec = live_sec;
				vb->ctl = 1;
			}
#ifdef _DEBUG
			tchar_t debugstring[ _MAX_MSG ];
			_stprintf( debugstring, _T( "set value : key = [ %s ], table = [ %s ], new value = [ %s ], old value = [ %s ], live_sec =  [ %d ]\n" ), record.key.c_str(), record.table.c_str(), record.value.c_str(), oldval.c_str(), live_sec );
			Pf_DEBUG( debugstring );
#endif
			record.value = oldval;
			ResBuf retbuf;
			CsPaperCommand::SetValue::Server::SetFeedback( retbuf, record, ret );
			pclient->SendPacket( CsINVALID_ID, CsS_C_FEEDBACK_SET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
			// put into db query list
			{
				SRecord* vb = new SRecord;
				( *vb ) = record;
				vb->create_time = create_msce;
				vb->update_time = updaet_msce;
				vb->live_sec = live_sec;
				vb->ctl = 1;
				int idx = rand() % pserver->m_MaxDB;
				TS_Lock lock( &pserver->m_dbLock[ idx ] );
				pserver->m_DBQueue[ idx ].push( vb );
			}
			return 1;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		int Server::DoSetnxValue( void* evocator, const void* pdata, unsigned int len )
		{
			Server*     pserver = ( ( Evocator* ) evocator )->pServer;
			PeerClient* pclient = ( ( Evocator* ) evocator )->pClient;
			CsRecord    record;
			//tstring     keybuf;
			//ubyte_t*    valbuf = NULL;
			//int         vallen = 0;
			int         live_sec = 0;
			int         ret = PAPERRESULT_UNKNOW;
			ResBuf      oldval;
			Databuf     recvdata;
			recvdata.buf = ( ubyte_t* ) pdata;
			recvdata.len = len;
			CsPaperCommand::SetValue::Server::GetRequest( &recvdata, record, live_sec );
			mstimen_t currmsce = PfTime::GetTickMSTime();
			{
				TS_Lock lock( &pserver->m_RecStorage.lock( record.key ) );
				_vmaps_iterator( SRecord* ) it = pserver->m_RecStorage.find( record.key );

				if ( pserver->m_RecStorage.miss( record.key, it ) )
				{
					SRecord* vb = new SRecord;
					pserver->m_RecStorage.insert( record.key, vb );
					( *vb ) = record;
					vb->create_time = currmsce;
					vb->update_time = currmsce;
					vb->live_sec = live_sec;
					vb->ctl = 2;
					ret = PAPERRESULT_SUCCESS;
				}
				else
				{
					oldval = it->second->value;
					ret = PAPERRESULT_EXIST_FAILURE;
				}
			}
#ifdef _DEBUG
			tchar_t debugstring[ _MAX_MSG ];
			_stprintf( debugstring, _T( "set value : key = [ %s ], table = [ %s ], new value = [ %s ], old value = [ %s ], live_sec =  [ %d ]\n" ), record.key.c_str(), record.table.c_str(), record.value.c_str(), oldval.c_str(), live_sec );
			Pf_DEBUG( debugstring );
#endif
			record.value = oldval;
			CsResBuf retbuf;
			CsPaperCommand::SetValue::Server::SetFeedback( retbuf, record, ret );
			pclient->SendPacket( CsINVALID_ID, CsS_C_FEEDBACK_SETNX_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
			// put into db query list
			{
				SRecord* vb = new SRecord;
				( *vb ) = record;
				vb->create_time = currmsce;
				vb->update_time = currmsce;
				vb->live_sec = live_sec;
				vb->ctl = 2;
				int idx = rand() % pserver->m_MaxDB;
				TS_Lock lock( &pserver->m_dbLock[ idx ] );
				pserver->m_DBQueue[ idx ].push( vb );
			}
			return 1;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		int Server::DoGetValue( void* evocator, const void* pdata, unsigned int len )
		{
			Server*     pserver = ( ( Evocator* ) evocator )->pServer;
			PeerClient* pclient = ( ( Evocator* ) evocator )->pClient;
			tstring     keybuf;
			//int         live_sec;
			int         ret = PAPERRESULT_UNKNOW;
			ResBuf      valbuf;
			Databuf     recvdata;
			recvdata.buf = ( ubyte_t* ) pdata;
			recvdata.len = len;
			CsPaperCommand::GetValue::Server::GetRequest( &recvdata, keybuf );
			mstimen_t currmsce = PfTime::GetTickMSTime();
			long32_t idx = 0;
			{
				{
					TS_Lock lock( &pserver->m_RecStorage.lock( keybuf ) );
					_vmaps_iterator( SRecord* ) it = pserver->m_RecStorage.find( keybuf );

					if ( !pserver->m_RecStorage.miss( keybuf, it ) )
					{
						SRecord* vb = it->second;

						// 過期了?
						if ( ( ( vb->create_time + vb->live_sec ) <= currmsce ) && ( vb->live_sec != 0 ) )
						{
							// 過期了
							pserver->m_RecStorage.erase( it );
							SAFE_DELETE( vb );
							ret = PAPERRESULT_OVERDUE;
						}
						else
						{
							valbuf = vb->value;
							ret = PAPERRESULT_SUCCESS;
						}
					}
					else
					{
						ret = PAPERRESULT_NOT_EXIST;
					}
				}

				// find in database
				if ( ret == PAPERRESULT_NOT_EXIST )
				{
					// put into db query list
					tstring val;
					int create_time = 0;
					int update_time = 0;
					int live_sec = 0;
					int idx = rand() % pserver->m_MaxDB;
					int tbidx = Crc32::Encode( keybuf.c_str(), keybuf.size() ) % g_MaxTb;
					DBPaper::dbGetRecord( pserver->m_Database[ idx ], tbidx, keybuf.c_str(), val, create_time, update_time, live_sec );

					if ( val != _T( "" ) )
					{
						// 過期了?
						if ( ( ( create_time + live_sec ) <= currmsce ) && ( live_sec != 0 ) )
						{
							ret = PAPERRESULT_OVERDUE;
						}
						else
						{
							SRecord* vb = new SRecord;
							vb->key = keybuf;
							vb->value = valbuf;
							vb->create_time = create_time;
							vb->update_time = update_time;
							vb->live_sec = live_sec;
							vb->ctl = 3;
							pserver->m_RecStorage.insert( keybuf, vb );
							valbuf.copy( (ubyte_t*)val.c_str(), val.size() );
							ret = PAPERRESULT_SUCCESS;
						}
					}
				}
			}
#ifdef _DEBUG
			tchar_t debugstring[ _MAX_MSG ];
			_stprintf( debugstring, _T( "get value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), keybuf.c_str(), valbuf.c_str(), ret );
			Pf_DEBUG( debugstring );
#endif
			ResBuf retbuf;
			CsPaperCommand::GetValue::Server::SetFeedback( retbuf, keybuf, valbuf, valbuf.size(), ret );
			pclient->SendPacket( CsINVALID_ID, CsS_C_FEEDBACK_GET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
			return 1;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		int Server::DoDelValue( void* evocator, const void* pdata, unsigned int len )
		{
			Server*     pserver = ( ( Evocator* ) evocator )->pServer;
			PeerClient* pclient = ( ( Evocator* ) evocator )->pClient;
			tstring     keybuf;
			int         live_sec;
			int         ret = PAPERRESULT_NOT_EXIST;
			ResBuf      valbuf;
			Databuf     recvdata;
			recvdata.buf = ( ubyte_t* ) pdata;
			recvdata.len = len;
			CsPaperCommand::DelValue::Server::GetRequest( &recvdata, keybuf );
			{
				TS_Lock lock( &pserver->m_RecStorage.lock( keybuf ) );
				_vmaps_iterator( SRecord* ) it = pserver->m_RecStorage.find( keybuf );

				if ( !pserver->m_RecStorage.miss( keybuf, it ) )
				{
					SRecord* vb = it->second;
					valbuf = vb->value;
					live_sec = vb->live_sec;
					SAFE_DELETE( vb );
					pserver->m_RecStorage.erase( keybuf );
					ret = PAPERRESULT_SUCCESS;
				}
				else
				{
					ret = PAPERRESULT_NOT_EXIST_SUCCESS;
				}
			}
#ifdef _DEBUG
			tchar_t debugstring[ _MAX_MSG ];
			_stprintf( debugstring, _T( "del value : key = [ %s ], value = [ %s ], ret = [ %d ]\n" ), keybuf.c_str(), valbuf.c_str(), ret );
			Pf_DEBUG( debugstring );
#endif
			ResBuf retbuf;
			CsPaperCommand::DelValue::Server::SetFeedback( retbuf, keybuf, valbuf, valbuf.size(), ret );
			pclient->SendPacket( CsINVALID_ID, CsS_C_FEEDBACK_GET_VALUE, _CsGetResBufPtr( retbuf ), _CsGetResBufLen( retbuf ) );
			// put into db query list
			{
				SRecord* vb = new SRecord;
				vb->key = keybuf;
				vb->create_time = 0;
				vb->update_time = 0;
				vb->live_sec = 0;
				vb->ctl = 4;
				int idx = rand() % pserver->m_MaxDB;
				TS_Lock lock( &pserver->m_dbLock[ idx ] );
				pserver->m_DBQueue[ idx ].push( vb );
			}
			return 1;
		}


		//===============================================================================//
		//
		//
		//===============================================================================//
		void __cdecl Server::Execute( void* data )
		{
			CallUpdate* cp = ( CallUpdate* ) data;
			Server* pserver = cp->pServer;
			int     cindex = cp->Idx;
			XDataBase* pdb = pserver->m_Database[ cindex ];

			while ( pserver->GetFD() != 0 )
			{
				SRecord* vb = NULL;
				{
					TS_Lock lock( &pserver->m_dbLock[ cindex ] );

					if ( !pserver->m_DBQueue[ cindex ].empty() )
					{
						vb = pserver->m_DBQueue[ cindex ].front();
						pserver->m_DBQueue[ cindex ].pop();
					}
					else
					{
						goto _EXECUTE_EMPTY;
					}
				}

				switch ( vb->ctl )
				{
				case 1:
					{
						int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
						DBPaper::dbSetRecord( pdb, tbidx, vb->key.c_str(), vb->value.c_str(), vb->update_time, vb->live_sec );
					}
					break;

				case 2:
					{
						int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
						DBPaper::dbSetnxRecord( pdb, tbidx, vb->key.c_str(), vb->value.c_str(), vb->create_time, vb->live_sec );

					}
					break;

				case 4:
					{
						int tbidx = Crc32::Encode( vb->key.c_str(), vb->key.size() ) % g_MaxTb;
						DBPaper::dbDelRecord( pdb, tbidx, vb->key.c_str() );
					}

					break;
				}

				SAFE_DELETE( vb );
_EXECUTE_EMPTY:
				PfStd::MsSleep( 100 );
			}

			pserver->OnDeleteDataBase( pdb );
		}

	}
}
;

