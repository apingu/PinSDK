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
#include "CsDemonServer.h"
#include "CsDemonCommand.h"
#include <basedef.h>
#include <XMySqlDB.h>
#include <CstSDataDef.h>
#include <direct.h>

#include <map>
#include <string>


namespace Cyberspace
{
    //===============================================================================//
    //
    //
    //===============================================================================//
    DemonServer::DemonServer()
    {
        //m_StatisticTime=(1000*60)*5;
        //m_StatisticTime=(1000*10);
        _mkdir( ".\\LOG_Statistic" );
        Pflag hostflag = 0;
        SET_BIT_FLAG( hostflag, _HOST_FLAG_HIDE_ );
        SetFlag( hostflag );
    }
    
    //===============================================================================//
    //
    //
    //===============================================================================//
    DemonServer::~DemonServer()
    {
    }
    
    DWORD DemonServer::OnGetVersion()
    {
        return 1;
    }
    
    int DemonServer::StartUp( uint_t allowplayers, unsigned short port )
    //void GameServer::OnServerInitialing(void)
    {
        int ret = 0;
        ret = Cyberspace::Server::StartUp( allowplayers, port );
        RegisterUserDefChannel( CsC_S_REQUEST_SERVERCOMMAND, DoGetCommandRequest );
        SCSRegisterUserDefChannel( CsSCSS_C_FEEDBACK_SERVERCOMMAND, DoSCSGetCommandFeedback );
        SetLogMode( LOGMODE_PRINTOUT );
        return ret;
    }
    
    int DemonServer::ShutDown()
    {
        UnRegisterUserDefChannel( CsC_S_REQUEST_SERVERCOMMAND );
        SCSUnRegisterUserDefChannel( CsSCSS_C_FEEDBACK_SERVERCOMMAND );
        return Cyberspace::Server::Shutdown();
    }
    
    
    // on join one server in server group
    void DemonServer::OnJoinServer( serverid_t sid )
    {
    }
    
    // on update one server in server group
    void DemonServer::OnUpdateServer( serverid_t sid )
    {
    }
    
    // on exit one server in server group
    void DemonServer::OnExitServer( serverid_t sid )
    {
    }
    
    void OutputDBResultAsText( XDataBase* pDB, XDBResult result, std::string& result_string )
    {
        int fields_size = pDB->GetFieldsSize( result );
        
        if ( fields_size <= 0 )
        {
            result_string.clear();
            return;
        }
        
        // 先欄位
        for ( int i = 0; i < fields_size; i++ )
        {
            result_string += pDB->GetFieldsName( result, i );
            result_string += '\t';
        }
        
        result_string += '\n';
        
        for ( int i = 0; i < pDB->GetResultSize( result ); i++ )
        {
            for ( int j = 0; j < fields_size; j++ )
            {
                result_string += pDB->GetQueryResult( result, j, i );
                result_string += '\t';
            }
            
            result_string += '\n';
        }
    }
    
    
    void DemonServer::DoInputCommand( const char* commandstring, uniqueid_t uid )
    {
        tchar_t command[_MAX_MSG];
        strcpy( command, commandstring );
        // XXXXX@N
        char* commandparam = strtok( command, "@" );
        
        if ( commandparam )
        {
            char commandstring[512];
            strcpy( commandstring, commandparam );
            char* pDest = strtok( NULL, "@" );
            
            if ( pDest )
            {
                CsResBuf rquestbuf;
                CsSCSDemonCommand::Client::SetRequestMessage( rquestbuf, ( const tchar_t* )commandstring );
                serverid_t sid = atoi( pDest );
                
                if ( sid != 0 )
                {
                    SCSSendMessage( sid, uid, CsSCSC_S_REQUEST_SERVERCOMMAND, _GetResBufPtr( rquestbuf ), _GetResBufLen( rquestbuf ) );
                }
                else
                {
                    SCSBroadcastMessageToOther( uid, CsSCSC_S_REQUEST_SERVERCOMMAND, _GetResBufPtr( rquestbuf ), _GetResBufLen( rquestbuf ) );
                }
            }
            // 沒有目的 那就本伺服器處理處理
            else
            {
                std::string result_string;
                char* commandname = strtok( command, " " );
                
                if ( strcmp( commandname, "kickout" ) == 0 )
                {
					sessionid_t cid = 0;
                    
                    if ( sscanf( commandstring, "kickout %d", &cid ) == -1 )
                    {
                        char retstring[512];
                        sprintf( retstring, "parameter [connectionid] error\n" );
                        result_string = retstring;
                    }
                    else
                    {
						CsSessionDescriptor cd = GetSessionDescriptor( cid );
                        
                        if ( cd )
                        {
                            Kickout( cd );
                            char retstring[512];
                            sprintf( retstring, "ConnectionId %lld kickout successful\n", cid );
                            result_string = retstring;
                        }
                        else
                        {
                            char retstring[512];
                            sprintf( retstring, "ConnectionId %lld not exist\n", cid );
                            result_string = retstring;
                        }
                    }
                }
                else if ( strcmp( commandname, "CustomDBQuery" ) == 0 )
                {
                    //char dbquery[512];
                    char* pQuerString = strtok( NULL, "\0" );
                    
                    if ( pQuerString == NULL )
                    {
                        char retstring[512];
                        sprintf( retstring, "parameter [SQL query string] error\n" );
                        result_string = retstring;
                    }
                    else
                    {
                        XDBResult result = GetCustomDB( 0 )->ExecQuery( pQuerString );
                        OutputDBResultAsText( GetCustomDB( 0 ), result, result_string );
                        
                        if ( result_string == "" )
                        {
                            char* retstring = new char[strlen( pQuerString ) + 50];
                            sprintf( retstring, "%s successful\n", pQuerString );
                            result_string = retstring;
                            SAFE_DELETE_ARRAY( retstring );
                        }
                        
                        GetCustomDB( 0 )->FreeResult( result );
                        // TODO: Pin
                        //sprintf(retstring, "SQL query successful\n");
                    }
                }
                else if ( strcmp( commandname, "CommonDBQuery" ) == 0 )
                {
                    char* pQuerString = strtok( NULL, "\0" );
                    
                    if ( pQuerString == NULL )
                    {
                        char retstring[512];
                        sprintf( retstring, "parameter [SQL query string] error\n" );
                        result_string = retstring;
                    }
                    else
                    {
                        XDBResult result = GetCommonDB( 0 )->ExecQuery( pQuerString );
                        OutputDBResultAsText( GetCommonDB( 0 ), result, result_string );
                        
                        if ( result_string == "" )
                        {
                            char* retstring = new char[strlen( pQuerString ) + 50];
                            sprintf( retstring, "%s successful\n", pQuerString );
                            result_string = retstring;
                            SAFE_DELETE_ARRAY( retstring );
                        }
                        
                        GetCommonDB( 0 )->FreeResult( result );
                        // TODO: Pin
                        //sprintf(retstring, "SQL query successful\n");
                    }
                }
                else
                {
                    char retstring[_MAX_MSG];
                    sprintf( retstring, "Bad command string %s", commandstring );
                    result_string = retstring;
                }
                
                // 回傳結果
                if ( uid == 0 )
                {
                    printf( result_string.c_str() );
                }
                else
                {
					CsSessionDescriptor cdp = GetSessionDescriptor( uid );
                    
                    if ( cdp )
                    {
                        CsResBuf feedbackbuf;
                        CsDemonCommand::Server::SetFeedbackMessage( feedbackbuf, result_string.c_str() );
                        SendCsMessage( cdp, CsINVALID_ID, CsS_C_FEEDBACK_SERVERCOMMAND,
                                       _GetResBufPtr( feedbackbuf ), _GetResBufLen( feedbackbuf ) );
                    }
                }
            }
        }
        
        return;
    }
    
    
    int DemonServer::DoGetCommandRequest( const ServerEvocator* evocator, const void* pData, unsigned int len )
    {
        // 收到clinet 進行處理 (送到 相對應的server)
        CsResBuf recvstring;
        CsDemonCommand::Server::GetRequestMessage( ( const byte_t* )pData, recvstring );
        evocator->pserver->SetLogMessage( evocator->pserver->GetLogMode(), "request command %s form client %s\n",
                                          recvstring.getstring(),
                                          evocator->pserver->GetSessionIpn( evocator->Sessiondescriptor ) );
        ( ( DemonServer* )evocator->pserver )->DoInputCommand( recvstring.getstring(), GetConnectSocketId( evocator->Sessiondescriptor) );
        return 1;
    }
    
    int DemonServer::DoSCSGetCommandFeedback( const SCS::ServerEvocator* evocator, const void* pData, unsigned int len )
    {
        // 收到server 處理回覆 (送到 要求的client)
        CsResBuf recvstring;
        CsSCSDemonCommand::Client::GetFeedbackMessage( ( const byte_t* )pData, recvstring );
        evocator->pserver->SetLogMessage( evocator->pserver->GetLogMode(), "feedback command %s from server %d\n",
                                          recvstring.getstring(), evocator->serverid );
        char fromserver[256];
        sprintf( fromserver, "@%d", evocator->serverid );
        recvstring.catstring( fromserver );
        
        if ( evocator->uniqueid == 0 )
        {
            printf( "%s\n", recvstring.getstring() );
        }
        else
        {
            CsSessionDescriptor cdp = evocator->pserver->GetLocalSessionDescriptor( ( socket_t )evocator->uniqueid );
            
            if ( cdp )
            {
                CsResBuf feedbackbuf;
                CsDemonCommand::Server::SetFeedbackMessage( feedbackbuf, recvstring.getstring() );
                evocator->pserver->SendCsMessage( cdp, CsINVALID_ID, CsS_C_FEEDBACK_SERVERCOMMAND,
                                                  _GetResBufPtr( feedbackbuf ), _GetResBufLen( feedbackbuf ) );
            }
        }
        
        return 1;
    }
    
    
    
    
};
