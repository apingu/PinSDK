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
#include <time.h>
#ifdef _WINDOWS
#include <direct.h>
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
#include <pf-ziplib.h>
#include <buffer_storage.h>
#include <pf-thread.h>
#include <parser/http-parser.h>
#include <parser/html-parser.h>
#include <parser/json.h>
#include <parser/xmlnode.h>
#include <algorithmic/crypt-crc64.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-utility.h"
#include "cs-http_cn_session.h"
#include "cs-http_server.h"


namespace cyberspace
{
#define _MAX_LOCK_POOL 10000
    namespace
    {
        BufferStorage s_buffer_storage;
        _cs_string    cs_header_fields = ( "X-CsSession" );
        T_Lock        s_session_lock[ _MAX_LOCK_POOL ];
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    HttpServer::HttpServer( _IN const _cs_char_t* name ) : CoreServer( name )
    {
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    HttpServer::~HttpServer()
    {
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void  HttpServer::SetSessionUID( CoreConnector* session, cs_session_uid_t sid )
    {
        CsHttpCnSession* http_cn_session = (CsHttpCnSession*)session;

        _cs_string value = "suid=" + pfstd::ulltostr( sid );
        http_cn_session->v_http_writer_->SetHeader( cs_header_fields.c_str(), value.c_str() );


        http_cn_session->v_http_writer_->AddCookie( "cs_session_uid", pfstd::ulltostr( sid ).c_str(), 0, "/" );
        //_cs_string cookie_data;
        //pfstd::strprintf( cookie_data, "%s; cs_session=%llu; path=/", http_cn_session->v_http_writer_->GetHeader( "Set-Cookie" ),  sid );
        //http_cn_session->v_http_writer_->SetHeader( "Set-Cookie", cookie_data.c_str() );
        //_keyvalue[ "Access-Control-Expose-Headers" ] = "CsSessionID";
        //_keyvalue[ "CsSessionID" ] = "100";
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    cs_session_uid_t HttpServer::GetSessionUID( _IN CoreConnector* session )
    {
        CsHttpCnSession* http_cn_session = (CsHttpCnSession*)session;
        const char* field = http_cn_session->v_http_parser_->getField( cs_header_fields.c_str() );
        if ( field )
        {
            _cs_string x_cssession = field;
            _mbsvector params = pfstd::string_split( x_cssession, "; " );
            for ( int pi = 0; pi < params.size(); pi++ )
            {
                if ( strstr( params[ pi ].c_str(), "suid=" ) != NULL )
                {
                    return pfstd::strtoull( params[ pi ].c_str() + 5 );
                }
            }
        }
        return CsINVALID_SESSIONID;
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    CsHttpContentType HttpServer::GetContentTypeType( _IN CoreConnector* session )
    {
        CsHttpCnSession* http_cn_session = (CsHttpCnSession*)session;
        return http_cn_session->content_type_type_;
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    _cs_string HttpServer::GetContentTypeValues( _IN CoreConnector* session )
    {
        CsHttpCnSession* http_cn_session = ( CsHttpCnSession* )session;
        return http_cn_session->content_type_values_;
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    _cs_string HttpServer::GetMethod( _IN CoreConnector* session )
    {
        CsHttpCnSession* http_cn_session = ( CsHttpCnSession* )session;
        return http_cn_session->v_http_parser_->getMethod();
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    bool HttpServer::RegisterFunction( _IN channel_nid_t act_uid, _IN bool is_auth, _IN OnMemberFunction func )
    {
        return CoreServer::RegisterFunction( crc64::Encode( act_uid.c_str(), act_uid.size() ), is_auth, func );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void HttpServer::UnRegisterFunction( _IN channel_nid_t act_uid )
    {
        CoreServer::UnRegisterFunction( crc64::Encode( act_uid.c_str(), act_uid.size() ) );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    CoreConnector* HttpServer::OnCreateConnector()
    {
        CsHttpCnSession* http_cn_session = new CsHttpCnSession;
        return http_cn_session;
    }

     //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    void HttpServer::OnDestoryConnector( _IN CoreConnector* session )
    {
        CsHttpCnSession* http_cn_session = (CsHttpCnSession*)session;
        SAFE_DELETE( http_cn_session );
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    int HttpServer::OnRecvMessage( _IN CoreConnector* session )
    {
        //TS_Lock lock( &debug_lock );

        CsHttpCnSession* http_cn_session = (CsHttpCnSession*)session;

        //printf( "recvice 0 client %d\n", s->GetFD() );
        //Pf_DEBUG( "http begin\n" );
        //HttpCnSocket* https = (HttpCnSocket*)s;

        http_cn_session->Clear();
        //CsHttpContentType  enctype = CsHttpEncType_Application;
        //http_cn_session->enc_type_ = CsHttpEncType_Application;
        _cs_string       actstr;
        const ubyte_t* combuf = NULL;
        uint_t         comlen = 0;
        int            response_ret = 1;

        cs_session_uid_t session_uid = CsINVALID_SESSIONID;

        std::string debug_request;
        std::string debug_reponse;

        // 收訊息解析
        {
            int httpread = 0;
            int needrecvm = 0;
            //{
            //    ulong32_t nread = GetRecvLength( http_cn_session );
            //    if ( nread == 0 )
            //        return 0;
            //
            //    ubyte_t* buff = new ubyte_t[ nread + 1 ];
            //    nread = RecvMessage( http_cn_session, buff, nread );
            //    debug_content.append( (const char*)buff, nread );
            //    httpread = http_cn_session->v_http_parser_->addBytes( (const char*)buff, nread );
            //    SAFE_DELETE_ARRAY( buff );
            //    needrecvm = nread;
            //}

            // 要確定資料流已經收完了
            int rc = 0;
            while ( httpread != 1 )
            {
                //printf( ( "01 - 01 \n" ) );
                ulong32_t nread = 1024;// GetRecvLength( http_cn_session );
                if ( nread == 0 )
                {
                    break;
                }
                //// 要等一下 才讀的道下一筆資料
                //pfstd::MSSleep( 10 );
                // 因為 有可能連 headle 都沒收完 所以取不到 need data length 改回用 tcp 收到資料的方法
                //ulong32_t nread = v_http_parser_->getNeedDataLength();
                int readret = 0;
                ubyte_t* buff = new ubyte_t[ nread * 2 ];
                memset( buff, 0, nread + 1 );
                //printf( "RecvPacket 3-1 client %d; recv %d\n", GetFD(), nread );
                if ( ( readret = RecvMessage( http_cn_session, buff, nread ) ) > 0 )
                {
                    //buff[ readret ] = 0;
                    //printf( "RecvPacket 3-2 client %d recv %d; read %d\n", GetFD(), nread, readret );
                    needrecvm += readret;
                    //printf( "RecvPacket 3-2 client %d recv %d; read %d; length = %d\n", GetFD(), nread, readret, needrecvm );
                    httpread = http_cn_session->v_http_parser_->addBytes( (const char*)buff, readret );
                    debug_request.append( (const char*)buff, nread );
                }
                //printf( "RecvPacket 3-4 client %d recv %d read %d, ret %d\n", GetFD(), nread, readret, httpread );
                SAFE_DELETE_ARRAY( buff );
                // 已關閉
                if ( readret <= 0 )
                {
                    break;
                }
            }

            session_uid = GetSessionUID( session );

            // 讓同一個 session 不會同時 多工
            TS_Lock utlock( &s_session_lock[ session_uid %_MAX_LOCK_POOL ] );

            //printf( "RecvPacket 4 client %d\n", GetFD() );
            //ubyte_t* zip_buffer_ = NULL;
            //int      zip_length_ = 0;
            http_cn_session->context_coding_ = CsHttpEncType_Normal;
            http_cn_session->accept_coding_ = CsHttpEncType_Normal;
            {
                const char* Context_Encoding = http_cn_session->v_http_parser_->getField( "content-encoding" );
                if ( Context_Encoding )
                {
                    if ( strstr( Context_Encoding, "gzip" ) != NULL )
                    {
                        http_cn_session->context_coding_ = CsHttpEncType_GZip;
                    }
                }

                const char* Accept_Encoding = http_cn_session->v_http_parser_->getField( "accept-encoding" );
                if ( Accept_Encoding )
                {
                    if ( strstr( Accept_Encoding, "gzip" ) != NULL )
                    {
                        http_cn_session->accept_coding_ = CsHttpEncType_GZip;
                    }
                }
            }

            std::string response_str = "{\"ACK\":9}";
            {
                const char* uri = http_cn_session->v_http_parser_->getUri();
                //const char* qur = httpparser.getQueryString();
                char* dbuf = http::url_decode( (char*)uri );
                //printf("%s\n", dbuf);
                {
                    char* pdbuf = dbuf;
                    char* actstrbuf = strtok( pdbuf, "?" );
                    char* tmpbuf01 = NULL;
                    //BYTE* tmpbuf02=NULL;
                    const char* comstrbuf = NULL;
                    //int         comlen = 0;
                    CsHttpContentType content_type_type = CsHttpEncType_Unknown;
                    _cs_string          content_type_values;
                    {
                        const char* str = http_cn_session->v_http_parser_->getField( "content-type" );
                        if ( str )
                        {
                            if( strstr( str, "application/vnd." ) != NULL )
                            {
                                content_type_type = CsHttpEncType_VND;
                            }
                            else if ( strstr( str, "application/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Application;
                            }
                            else if( strstr( str, "multipart/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Multipart;
                            }
                            else if( strstr( str, "audio/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Audio;
                            }
                            else if( strstr( str, "image/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Image;
                            }
                            else if( strstr( str, "text/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Text;
                            }
                            else if( strstr( str, "video/" ) != NULL )
                            {
                                content_type_type = CsHttpEncType_Video;
                            }
                            content_type_values = str;
                        }
                    }

                    if ( strncmp( http_cn_session->v_http_parser_->getMethod(), "GET", 3 ) == 0 )
                    {
                        comstrbuf = strtok( NULL, "\0" );

                        if ( comstrbuf )
                        {
                            comlen = _csstrlen( comstrbuf ) + 1;
                        }
                    }
                    else if ( strncmp( http_cn_session->v_http_parser_->getMethod(), "POST", 4 ) == 0 )
                    {
                        const BYTE* httpbody = (BYTE*)http_cn_session->v_http_parser_->getBody();
                        int         httplen = http_cn_session->v_http_parser_->getContentLength();

                        if ( httpbody )
                        {
                            if ( http_cn_session->context_coding_ == CsHttpEncType_GZip )
                            {
                                int maxlen = pfziplib::gzip::GetMaxUncompressedLen( httpbody, httplen );

                                if ( http_cn_session->zip_buffer_ != NULL )
                                {
                                    s_buffer_storage.Return( http_cn_session->zip_buffer_ );
                                }
                                http_cn_session->zip_buffer_ = (BYTE*)s_buffer_storage.Borrow( maxlen );// new BYTE[maxlen];
                                                                                    // zip_buffer_ = new BYTE[maxlen];
                                http_cn_session->zip_length_ = pfziplib::gzip::UncompressBuffer( httpbody, httplen, http_cn_session->zip_buffer_, maxlen );
                                httpbody = http_cn_session->zip_buffer_;
                                httplen = http_cn_session->zip_length_;
                                http_cn_session->zip_buffer_[ http_cn_session->zip_length_ ] = 0;
                            }

                            if ( content_type_type == CsHttpEncType_Application )
                            {
                                // TODO: Pin 2019 06 23 (暫)
                                // 因為會有escape的問題 
                                // 假如 要傳一段字串 但字串中 帶有 & => 發送端 encodeuri
                                // 這邊如果先解開 字元& 就會提前出現  程式內再解碼就會出現問題
                                // 解decodeurl 應交給 處理程式處理
                                //comstrbuf = tmpbuf01 = http::url_decode( ( char* )httpbody );
                                comstrbuf = tmpbuf01 = ( (char*)httpbody );
                            }
                            else
                            {
                                comstrbuf = (const char*)httpbody;
                            }
                        }
                        comlen = httplen;
                    }
                    // post get 以外的方法
                    else
                    {
                        response_ret = 0;
                    }

                    http_cn_session->content_type_type_ = content_type_type;
                    http_cn_session->content_type_values_ = content_type_values;
                    if ( actstrbuf != NULL )
                        actstr = actstrbuf;
                    else
                        actstr = "";
                    ( combuf ) = (const ubyte_t*)comstrbuf;
                    //comlen    = comlen;
                }
            }
            //printf( "RecvPacket 7 client %d\n", GetFD() );
        }

        _csbinbuf response_str = "{\"ACK\":-1}";
        if( response_ret != 0 )
        {
            //printf( "recvice 2 client %d\n", s->GetFD() );
            response_str = DoFunction( http_cn_session, crc64::Encode( actstr.c_str(), actstr.size() ), (const ubyte_t*)combuf, comlen );
        }
        else
        {
            response_str = "";
        }

        {
            // 設定header
            {
                const char* origin = http_cn_session->v_http_parser_->getField( "Origin" );
                if ( origin != NULL )
                {
                    _cs_string cros_host;
                    pfstd::strprintf( cros_host, "%s", origin );
                    //if ( strcmp( origin, "null" ) != 0 )
                    //{
                    //    pfstd::strprintf( cros_host, "%s", origin );                                               
                    //}
                    //else
                    //{
                    //    pfstd::strprintf( cros_host, "http://localhost:%d", GetLocalServicePort() );
                    //}
                    http_cn_session->v_http_writer_->SetHeader( "Access-Control-Allow-Origin", cros_host.c_str() );
                    http_cn_session->v_http_writer_->SetHeader( "Access-Control-Allow-Credentials", "true" );
                }
                //http_cn_session->v_http_writer_->SetHeader( "Access-Control-Max-Age", ( "10" ) );

                //if ( session_uid != CsINVALID_SESSIONID )
                //{
                //    SetSessionUID( http_cn_session, session_uid );
                //}
            
                {
                    _cs_string allowheader = http_cn_session->v_http_writer_->GetHeader( "Access-Control-Allow-Headers" );
                    if ( allowheader == "" )
                    {
                        allowheader += cs_header_fields;
                    }
                    else
                    {
                        allowheader += ", " + cs_header_fields;
                    }
                    http_cn_session->v_http_writer_->SetHeader( "Access-Control-Allow-Headers", allowheader.c_str() );
                }
            }

            //char* response_buf = new char[ (response_str.size() * 4 ) + 1 ];
            //strcpy( response_buf, response_str.c_str() );

            //BYTE* sendbuf = ( BYTE* )url_encode( response_buf );
            //int   sendlen = strlen( response_buf );//response_str.size();

            const BYTE* sendbuf = (const BYTE*)response_str.c_str();
            int   sendlen = response_str.size();
            if ( http_cn_session->accept_coding_ == CsHttpEncType_GZip )
            {
                http_cn_session->v_http_writer_->SetContentCoding( "gzip" );
                http_cn_session->v_http_writer_->SetAcceptCoding( "gzip" );
                int compress_len = pfziplib::gzip::GetMaxCompressedLen( sendlen );

                if ( http_cn_session->zip_length_ < compress_len )
                {
                    s_buffer_storage.Return( http_cn_session->zip_buffer_ );
                    http_cn_session->zip_buffer_ = s_buffer_storage.Borrow( compress_len );
                    http_cn_session->zip_length_ = compress_len;
                }

                sendlen = pfziplib::gzip::CompressBuffer( sendbuf, sendlen, http_cn_session->zip_buffer_, http_cn_session->zip_length_ );
                sendbuf = http_cn_session->zip_buffer_;
            }

            //if( !content_type_.empty() )
            //{
            //    v_http_writer_->SetContentType( content_type_.c_str() );
            //}

            http_cn_session->v_http_writer_->SetContent( (const char*)sendbuf, sendlen );
            size_t len = 0;
            const char* response = http_cn_session->v_http_writer_->GetBuffer( len );
            debug_reponse.append( response );
            debug_reponse.append( "\0" );
            int send_size = SendMessage( http_cn_session, (const ubyte_t*)response, len );

            if ( send_size != len )
            {
                Pf_INFO( response );
                Pf_INFO( "\n" );
            }

            s_buffer_storage.Return( http_cn_session->zip_buffer_ );
        }

//#ifdef _WINDOWS_
//        {
//            OutputDebugString( ( "\n" ) );
//            OutputDebugString( ( "===================================================================================\n" ) );
//            OutputDebugString( debug_request.c_str() );
//            OutputDebugString( ( "\n__________________________________________________________________________________" ) );
//            OutputDebugString( ( "\n" ) );
//            OutputDebugString( debug_reponse.c_str() );
//            OutputDebugString( ( "\n===================================================================================\n" ) );
//            OutputDebugString( ( "\n" ) );
//        }
//#endif

        Kickout( http_cn_session );
        return 0;
    }

    //===============================================================================
    // NAME :
    // DESC :
    //===============================================================================
    _cs_string HttpServer::OnRecvOutputDebug( _IN CoreConnector* session, _IN const char* comstr, _IN int comlen )
    {
        char* degbuff = new char[ comlen + 512 ];
        time_t rawtime;
        struct tm* timeinfo;
        time( &rawtime );
        timeinfo = localtime( &rawtime );
        sprintf( degbuff, "%04d/%02d/%02d %02d:%02d:%02d  %s\n",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec,
            comstr );
        printf( degbuff );
        FILE* dbgf = fopen( "web_debug.txt", "a+b" );

        if ( dbgf )
        {
            fwrite( degbuff, _csstrlen( degbuff ), 1, dbgf );
            fclose( dbgf );
        }

        SAFE_DELETE_ARRAY( degbuff );
        //JSONValue *value = JSON::Parse(comstr);
        /*
        if(comstr)
        {
        //JSONObject root = value->AsObject();
        bool parser_succ=true;
        MyPiece::Login::InParam  inp;
        MyPiece::Login::OutParam outp;
        if(!http::Parser::getField(comstr, "ACC", HTTPVType_String,  &inp.Username))
        parser_succ=false;
        if(!http::Parser::getField(comstr, "PWD", HTTPVType_String,  &inp.Password))
        parser_succ=false;
        if(!http::Parser::getField(comstr, "DID", HTTPVType_LLong,  &inp.DeviceId))
        parser_succ=false;
        if(!http::Parser::getField(comstr, "DTP", HTTPVType_LLong,  &inp.DeviceType))
        parser_succ=false;
        if(!http::Parser::getField(comstr, "VER", HTTPVType_Long,  &inp.Version))
        parser_succ=false;

        if(parser_succ)
        {
        delegate_->OnRequestLogin(inp, outp);
        if(outp.Ack==MyPiece::Login::OutErrorCode_Success)
        {
        MyPiece::Utility::Setting::CreatePictureFolder(outp.MemberId);
        }
        return MyPiece::Login::OutputToJSON(&outp,
        delegate_->OnGetValidateCode(outp.MemberId).c_str());
        }
        }
        if(comstr)
        {
        bool parser_succ=true;
        const char* p=strstr(comstr, "\n");
        std::string boundary;
        boundary.append(comstr, p-comstr);

        MPFD::Parser parser;
        parser.SetTempDirForFileUpload("./tmp");
        std::string content_type;
        parser.SetBoundary(boundary.c_str());
        parser.AcceptSomeData(comstr, comlen);
        std::map <std::string,MPFD::Field *> fields=parser.GetFieldsMap();

        MyPiece::UpdatePiece::InParam  inp;
        MyPiece::UpdatePiece::OutParam outp;

        MPFD::Field * pp;
        if(pp=fields["MID"])
        inp.MemberId=_atoi64(pp->GetTextTypeContent().c_str());
        else
        parser_succ=false;

        std::string vcode;
        if(pp=fields["VCD"])
        vcode=pp->GetTextTypeContent();
        else
        parser_succ=false;
        if(!delegate_->OnValidateMember(inp.MemberId, vcode.c_str()))
        return std::string();

        if(pp=fields["PUI"])
        inp.PieceUId = _atoi64(pp->GetTextTypeContent().c_str());
        else
        parser_succ=false;

        if(pp=fields["CON"])
        inp.Content = pp->GetTextTypeContent();
        else
        parser_succ=false;

        if(pp=fields["AV0"])
        inp.sAttribute.Value00 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value00 = 0;

        if(pp=fields["AV1"])
        inp.sAttribute.Value01 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value01 = 0;

        if(pp=fields["AV2"])
        inp.sAttribute.Value02 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value02 = 0;

        if(pp=fields["AV3"])
        inp.sAttribute.Value03 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value03 = 0;

        if(pp=fields["AV4"])
        inp.sAttribute.Value04 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value04 = 0;

        if(pp=fields["AV5"])
        inp.sAttribute.Value05 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value05 = 0;

        if(pp=fields["AV6"])
        inp.sAttribute.Value06 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value06 = 0;

        if(pp=fields["AV7"])
        inp.sAttribute.Value07 = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Value07 = 0;

        if(pp=fields["AHD"])
        inp.sAttribute.Hide = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.Hide = 0;

        if(pp=fields["AMR"])
        inp.sAttribute.MovingRange = atoi(pp->GetTextTypeContent().c_str());
        else
        inp.sAttribute.MovingRange = 0;

        if(pp=fields["IMA"])
        {
        if(pp->GetFileName()!="")
        {
        std::string newfile;
        {
        char* filename1=url_decode(pp->GetFileName().c_str());
        newfile=MyPiece::Utility::Setting::CreateImageFile(inp.MemberId, filename1);
        delete [] filename1;
        }

        std::string finalfname=CopyFilePassible(pp->GetTempFilename().c_str(), newfile.c_str());
        if(finalfname=="")
        {
        parser_succ=false;
        }
        else
        {
        unlink(pp->GetTempFilename().c_str());
        char tmpbuf[512];
        inp.ImageFname=inp.ImageFname + _i64toa(inp.MemberId, tmpbuf, 10) + "/" + finalfname;
        }
        }
        }
        else
        inp.ImageFname="";

        if(parser_succ)
        {
        delegate_->OnRequestUpdatePiece(inp, outp);
        return MyPiece::UpdatePiece::OutputToJSON(&outp);
        }
        }
        */
        return std::string();
    }


    /*
        // example
        std::string HttpServer::OnRecvLogin(net::TCPSocket* s, CsHttpContentType enctype, const char* comstr, int comlen)
        {
            if(enctype!=CsHttpEncType_Application)
                return std::string();

            //JSONValue *value = JSON::Parse(comstr);
            if(comstr)
            {
                //JSONObject root = value->AsObject();
                bool parser_succ=true;
                MyPiece::Login::InParam  inp;
                MyPiece::Login::OutParam outp;
                if(!http::Parser::getField(comstr, "ACC", HTTPVType_String,  &inp.Username))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "PWD", HTTPVType_String,  &inp.Password))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "DID", HTTPVType_LLong,  &inp.DeviceId))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "DTP", HTTPVType_LLong,  &inp.DeviceType))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "VER", HTTPVType_Long,  &inp.Version))
                    parser_succ=false;

                if(parser_succ)
                {
                    delegate_->OnRequestLogin(inp, outp);
                    if(outp.Ack==MyPiece::Login::OutErrorCode_Success)
                    {
                        MyPiece::Utility::Setting::CreatePictureFolder(outp.MemberId);
                    }
                    return MyPiece::Login::OutputToJSON(&outp,
                                                        delegate_->OnGetValidateCode(outp.MemberId).c_str());
                }
            }

            return std::string();
        }

            //=============================================================================
        // NAME :
        // DESC :
        //=============================================================================
        std::string HttpServer::OnRecvSelectSphere(net::TCPSocket* s, CsHttpContentType enctype, const char* comstr, int comlen)
        {
            if(enctype!=CsHttpEncType_Application)
                return std::string();

            //JSONValue *value = JSON::Parse(comstr);
            if(comstr)
            {
                //JSONObject root = value->AsObject();
                bool parser_succ=true;
                MyPiece::SelectSphere::InParam  inp;
                MyPiece::SelectSphere::OutParam outp;

                if(!http::Parser::getField(comstr, "MID", HTTPVType_LLong,  &inp.MemberId))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "ROL", HTTPVType_Float,  &inp.CurrentCoord.Roll))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "PIT", HTTPVType_Float,  &inp.CurrentCoord.Pitch))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "FAC", HTTPVType_Float,  &inp.CurrentCoord.Face))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "LAT", HTTPVType_Double,  &inp.CurrentCoord.Latitude))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "LON", HTTPVType_Double,  &inp.CurrentCoord.Longitude))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "ALT", HTTPVType_Double,  &inp.CurrentCoord.Altitude))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "MET", HTTPVType_Long,  &inp.Method))
                    parser_succ=false;
                if(!http::Parser::getField(comstr, "RAD", HTTPVType_Long,  &inp.Radius))
                    parser_succ=false;

                if(parser_succ)
                {
                    delegate_->OnRequestSelectSphere(inp, outp);
                    return MyPiece::SelectSphere::OutputToJSON(&outp);
                }
            }
            return std::string();
        }

        //=============================================================================
        // NAME :
        // DESC :
        //=============================================================================
        std::string HttpServer::OnRecvInsertPiece(net::TCPSocket* s, CsHttpContentType enctype, const char* comstr, int comlen)
        {
            if(enctype!=CsHttpEncType_Multipart)
                return std::string();

            if(comstr)
            {
                bool parser_succ=true;
                const char* p=strstr(comstr, "\n");
                std::string boundary;
                boundary.append(comstr, p-comstr);

                MPFD::Parser parser;
                parser.SetTempDirForFileUpload("./tmp");
                std::string content_type;
                parser.SetBoundary(boundary.c_str());
                parser.AcceptSomeData(comstr, comlen);
                std::map <std::string,MPFD::Field *> fields=parser.GetFieldsMap();

                MyPiece::InsertPiece::InParam  inp;
                MyPiece::InsertPiece::OutParam outp;

                MPFD::Field * pp;

                if(pp=fields["MID"])
                    inp.MemberId=_atoi64(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                std::string vcode;
                if(pp=fields["VCD"])
                    vcode=pp->GetTextTypeContent();
                else
                    parser_succ=false;
                if(!delegate_->OnValidateMember(inp.MemberId, vcode.c_str()))
                    return std::string();

                if(pp=fields["CON"])
                    inp.Content = pp->GetTextTypeContent();
                else
                    parser_succ=false;

                if(pp=fields["AV0"])
                    inp.sAttribute.Value00 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value00 = 0;

                if(pp=fields["AV1"])
                    inp.sAttribute.Value01 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value01 = 0;

                if(pp=fields["AV2"])
                    inp.sAttribute.Value02 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value02 = 0;


                if(pp=fields["AV3"])
                    inp.sAttribute.Value03 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value03 = 0;

                if(pp=fields["AV4"])
                    inp.sAttribute.Value04 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value04 = 0;

                if(pp=fields["AV5"])
                    inp.sAttribute.Value05 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value05 = 0;

                if(pp=fields["AV6"])
                    inp.sAttribute.Value06 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value06 = 0;

                if(pp=fields["AV7"])
                    inp.sAttribute.Value07 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value07 = 0;

                if(pp=fields["AHD"])
                    inp.sAttribute.Hide = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Hide = 0;

                if(pp=fields["AMR"])
                    inp.sAttribute.MovingRange = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.MovingRange = 0;

                if(pp=fields["CAG"])
                    inp.Category = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.Category = 0;

                if(pp=fields["ROL"])
                    inp.CurrentCoord.Roll=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                if(pp=fields["PIT"])
                    inp.CurrentCoord.Pitch=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                if(pp=fields["FAC"])
                    inp.CurrentCoord.Face=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                 if(pp=fields["LAT"])
                    inp.CurrentCoord.Latitude=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                if(pp=fields["LON"])
                    inp.CurrentCoord.Longitude=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                if(pp=fields["ALT"])
                    inp.CurrentCoord.Altitude=atof(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;


                if(pp=fields["IMA"])
                {
                    if(pp->GetFileName()!="")
                    {
                        std::string newfile;
                        {
                            char* filename1=url_decode(pp->GetFileName().c_str());
                            newfile= MyPiece::Utility::Setting::CreateImageFile(inp.MemberId, filename1);
                            delete [] filename1;
                        }

                        std::string finalfname=CopyFilePassible(pp->GetTempFilename().c_str(), newfile.c_str());
                        if(finalfname=="")
                        {
                            parser_succ=false;
                        }
                        else
                        {
                            unlink(pp->GetTempFilename().c_str());
                            char tmpbuf[512];
                            inp.ImageFname=inp.ImageFname + _i64toa(inp.MemberId, tmpbuf, 10) + "/" + finalfname;
                        }
                    }
                }
                else
                    parser_succ=false;

                if(parser_succ)
                {
                    delegate_->OnRequestInsertPiece(inp, outp);
                    return MyPiece::InsertPiece::OutputToJSON(&outp);
                }

            }
            return std::string();
        }

        //=============================================================================
        // NAME :
        // DESC :
        //=============================================================================
        std::string HttpServer::OnRecvUpdatePiece(net::TCPSocket* s, CsHttpContentType enctype, const char* comstr, int comlen)
        {
            if(enctype!=CsHttpEncType_Multipart)
                return std::string();

            if(comstr)
            {
                bool parser_succ=true;
                const char* p=strstr(comstr, "\n");
                std::string boundary;
                boundary.append(comstr, p-comstr);

                MPFD::Parser parser;
                parser.SetTempDirForFileUpload("./tmp");
                std::string content_type;
                parser.SetBoundary(boundary.c_str());
                parser.AcceptSomeData(comstr, comlen);
                std::map <std::string,MPFD::Field *> fields=parser.GetFieldsMap();

                MyPiece::UpdatePiece::InParam  inp;
                MyPiece::UpdatePiece::OutParam outp;

                MPFD::Field * pp;
                if(pp=fields["MID"])
                    inp.MemberId=_atoi64(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                std::string vcode;
                if(pp=fields["VCD"])
                    vcode=pp->GetTextTypeContent();
                else
                    parser_succ=false;
                if(!delegate_->OnValidateMember(inp.MemberId, vcode.c_str()))
                    return std::string();

                if(pp=fields["PUI"])
                    inp.PieceUId = _atoi64(pp->GetTextTypeContent().c_str());
                else
                    parser_succ=false;

                if(pp=fields["CON"])
                    inp.Content = pp->GetTextTypeContent();
                else
                    parser_succ=false;

                if(pp=fields["AV0"])
                    inp.sAttribute.Value00 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value00 = 0;

                if(pp=fields["AV1"])
                    inp.sAttribute.Value01 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value01 = 0;

                if(pp=fields["AV2"])
                    inp.sAttribute.Value02 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value02 = 0;

                if(pp=fields["AV3"])
                    inp.sAttribute.Value03 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value03 = 0;

                if(pp=fields["AV4"])
                    inp.sAttribute.Value04 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value04 = 0;

                if(pp=fields["AV5"])
                    inp.sAttribute.Value05 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value05 = 0;

                if(pp=fields["AV6"])
                    inp.sAttribute.Value06 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value06 = 0;

                if(pp=fields["AV7"])
                    inp.sAttribute.Value07 = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Value07 = 0;

                if(pp=fields["AHD"])
                    inp.sAttribute.Hide = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.Hide = 0;

                if(pp=fields["AMR"])
                    inp.sAttribute.MovingRange = atoi(pp->GetTextTypeContent().c_str());
                else
                    inp.sAttribute.MovingRange = 0;

                if(pp=fields["IMA"])
                {
                    if(pp->GetFileName()!="")
                    {
                        std::string newfile;
                        {
                            char* filename1=url_decode(pp->GetFileName().c_str());
                            newfile=MyPiece::Utility::Setting::CreateImageFile(inp.MemberId, filename1);
                            delete [] filename1;
                        }

                        std::string finalfname=CopyFilePassible(pp->GetTempFilename().c_str(), newfile.c_str());
                        if(finalfname=="")
                        {
                            parser_succ=false;
                        }
                        else
                        {
                            unlink(pp->GetTempFilename().c_str());
                            char tmpbuf[512];
                            inp.ImageFname=inp.ImageFname + _i64toa(inp.MemberId, tmpbuf, 10) + "/" + finalfname;
                        }
                    }
                }
                else
                    inp.ImageFname="";

                if(parser_succ)
                {
                    delegate_->OnRequestUpdatePiece(inp, outp);
                    return MyPiece::UpdatePiece::OutputToJSON(&outp);
                }
            }
            return std::string();
        }*/


}



