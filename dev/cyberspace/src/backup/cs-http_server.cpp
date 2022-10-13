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

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-utility.h"
#include "cs-http_cn_socket.h"
#include "cs-http_server.h"


namespace cyberspace
{
    namespace
    {
        BufferStorage s_buffer_storage;
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
#define _CsHTTPSERVER_SOCKETSUPERVISE(s)  ((cyberspace::SocketSupervise*)(s->v_object_[ 0 ]))

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    HttpServer::HttpServer()
    {
        //mkdir("tmp");
        content_type_;
        v_object_[ 0 ] = new cyberspace::SocketSupervise;
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    HttpServer::~HttpServer()
    {
        cyberspace::SocketSupervise* pModule00 = _CsHTTPSERVER_SOCKETSUPERVISE( this );
        SAFE_DELETE( pModule00 );
    }


    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    int HttpServer::StartUp( unsigned int allowplayers, unsigned short port )
    {
        int ret = net::TCPServer::StartUp( allowplayers, port );
        if( ret == 1 )
        {
            s_buffer_storage.Create( GetPollingCount(), 10 * _1MB_ );
        }
        _CsHTTPSERVER_SOCKETSUPERVISE( this )->DoStartUp( ObjectName(), pftml::Time::GetSTime() );
        return ret;
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    int HttpServer::Shutdown( void )
    {
        _CsHTTPSERVER_SOCKETSUPERVISE( this )->DoClose();
        s_buffer_storage.Destroy();
        return net::TCPServer::Shutdown();
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::SetContentType( const char* buf )
    {
        content_type_ = buf;
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    //void HttpServer::OnInitate()
    //{
    //    //m_ReceiveDispatcher.insert(std::map<std::string, OnReceiveFunction>::value_type("setcoordinate", &cyberspace::HttpServer::OnRecvSetCooordinate));
    //    time_t tp = time( NULL );
    //    printf( "||===========================================================================\n" );
    //    printf( "||  cyberspace server 【 HttpServer 】 initiating........\n" );
    //    printf( "||                         build:  %s %s\n||\n", __DATE__, __TIME__ );
    //    printf( "||    Start on %s", ctime( &tp ) );
    //    printf( "||===========================================================================\n" );
    //    s_buffer_storage.Create( GetPollingCount(), 10 * _1MB_ );
    //}

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::SetKeepAliveMstime( mstimen_t mst )
    {
        _CsHTTPSERVER_SOCKETSUPERVISE( this )->SetKeepAliveMstime( mst );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    int HttpServer::GetInspectedSize()
    {
        return _CsHTTPSERVER_SOCKETSUPERVISE( this )->GeInsecureSize();
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::Update()
    {
        _CsHTTPSERVER_SOCKETSUPERVISE( this )->Update( this );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    net::TCPSocket* HttpServer::OnNewSocket( void )
    {
        CsHttpCnSocket* htsockconn = new CsHttpCnSocket;
        return htsockconn;
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::OnDelSocket( net::TCPSocket* skt )
    {
        CsHttpCnSocket* scn = ( CsHttpCnSocket* )( skt );
        SAFE_DELETE( scn );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::OnAccept( net::TCPSocket* s )
    {
        if( !_CsHTTPSERVER_SOCKETSUPERVISE( this )->DoAccept( s ) )
        {
            net::TCPServer::Kickout( s );
            return;
        }
        OnAcceptSocket( s );
        return;
    }

    //===============================================================================// 
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::OnKickout( net::TCPSocket* s )
    {
        _CsHTTPSERVER_SOCKETSUPERVISE( this )->DoKickout( s );
        OnKickoutSocket( s );
    }

    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    void HttpServer::OnReceive( net::TCPSocket* s )
    {
        //printf( "recvice 0 client %d\n", s->GetFD() );
        //Pf_DEBUG( "http begin\n" );
        HttpCnSocket* https = ( HttpCnSocket* )s;

        https->Clear();
        CsHttpEncType  content_type = CsHttpEncType_Application;
        _mstring       actstr;
        const ubyte_t* combuf = NULL;
        uint_t         comlen = 0;

        https->RecvPacket( content_type, actstr, &combuf, comlen );

        //printf( "recvice 1 client %d\n", s->GetFD() );
        _csbinbuf response_str = "{\"ACK\":9}";
        if( comlen > 0 )
        {
            //printf( "recvice 2 client %d\n", s->GetFD() );
            response_str = OnReceiveSocket( s, content_type, actstr.c_str(), ( const ubyte_t* )combuf, comlen );
        }
        //printf( "recvice 3 client %d\n", s->GetFD() );

        //_binbuf send_buf = response_str; // linux 這邊一定得這樣做 不然傳進去時資料會出事 至於為何會這樣 不知 再想想
        https->SendPacket( response_str );

        /*
        http::Parser* httpparser = https->v_http_parser_;
        http::Writer* httpwriter = https->v_http_writer_;

        ulong32_t nread = s->GetReadLength();
        //int recvret = s->Ioctl( FIONREAD, ( ulong32_t* )&nread );
        ubyte_t* buff = new ubyte_t[ nread + 1 ];
        //char buff[1024];
        nread = s->Recv( buff, nread );
        int needread = httpparser->addBytes( ( const char* )buff, nread );
        SAFE_DELETE_ARRAY( buff );

        //printf( httcombin.c_str() );
        int needrecvm = nread;
        // 要確定資料流已經收完了
        while( needread > 0 )
        {
            ubyte_t* needbuff = new ubyte_t[ needread + 1 ];
            memset( needbuff, 0, needread + 1 );
            if( ( nread = s->Recvn( needbuff, needread ) ) > 0 )
            {
                needbuff[ needread ] = 0;
                needrecvm += needread;
                needread = httpparser->addBytes( ( const char* )needbuff, nread );
                //printf( httcombin.c_str() );
            }
            SAFE_DELETE_ARRAY( needbuff );
        }

        ubyte_t* zip_buffer_ = NULL;
        int      zip_length_ = 0;
        CsHttpCodingType context_coding_ = CsHttpEncType_Normal;
        CsHttpCodingType accept_coding_ = CsHttpEncType_Normal;
        {
            const char* Context_Encoding = httpparser->getField( "content-encoding" );
            if( Context_Encoding )
            {
                if( strstr( Context_Encoding, "gzip" ) != NULL )
                {
                    context_coding_ = CsHttpEncType_GZip;
                }
            }

            const char* Accept_Encoding = httpparser->getField( "accept-encoding" );
            if( Accept_Encoding )
            {
                if( strstr( Accept_Encoding, "gzip" ) != NULL )
                {
                    accept_coding_ = CsHttpEncType_GZip;
                }
            }
        }
        std::string response_str = "{\"ACK\":9}";
        {
            const char* uri = httpparser->getUri();
            //const char* qur = httpparser->getQueryString();
            char* dbuf = url_decode( ( char* )uri );
            //printf("%s\n", dbuf);
            // 分析ㄈ
            {
                //char logbuf[512];
                //sprintf( logbuf, "%s\n", dbuf );
                //Pf_DEBUG( logbuf );
            }
            //if ( delegate_ )
            {
                char* pdbuf = dbuf;
                char* actstr = strtok( pdbuf, "?" );
                char* tmpbuf01 = NULL;
                //BYTE* tmpbuf02=NULL;
                const char* comstr = NULL;
                int comlen = 0;
                CsHttpEncType    content_type = CsHttpEncType_Application;
                {
                    const char* str = httpparser->getField( "content-type" );

                    if( str )
                    {
                        if( strstr( str, "multipart/form-data" ) != NULL )
                        {
                            content_type = CsHttpEncType_Multipart;
                        }
                    }
                }

                if( strncmp( httpparser->getMethod(), "GET", 3 ) == 0 )
                {
                    comstr = strtok( NULL, "\0" );

                    if( comstr )
                    {
                        comlen = _csstrlen( comstr ) + 1;
                    }
                }
                else if( strncmp( httpparser->getMethod(), "POST", 4 ) == 0 )
                {
                    const BYTE* httpbody = ( BYTE* )httpparser->getBody();
                    int         httplen = httpparser->getContentLength();

                    if( httpbody )
                    {
                        if( context_coding_ == CsHttpEncType_GZip )
                        {
                            int maxlen = pfzip::GZip::GetMaxUncompressedLen( httpbody, httplen );
                            zip_buffer_ = ( BYTE* )s_buffer_storage.Borrow( maxlen );// new BYTE[maxlen];
                                                                                     //zip_buffer_ = new BYTE[maxlen];
                            zip_length_ = pfzip::GZip::UncompressBuffer( httpbody, httplen, zip_buffer_, maxlen );
                            httpbody = zip_buffer_;
                            httplen = zip_length_;
                            zip_buffer_[ zip_length_ ] = 0;
                        }

                        if( content_type == CsHttpEncType_Application )
                        {
                            comstr = tmpbuf01 = url_decode( ( char* )httpbody );
                        }
                        else
                        {
                            comstr = ( const char* )httpbody;
                        }
                    }
                    comlen = httplen;
                }

                if( ( comstr ) && ( comlen > 0 ) )
                {
                    response_str = OnReceiveSocket( s, content_type, actstr, comstr, comlen );
                }
            }
        }

        if( response_str == "" )
        {
            response_str = "{\"ACK\":9}";
        }

        {
            httpwriter->SetHeader( "Access-Control-Allow-Origin", httpparser->getField( "origin" ) );

            BYTE* sendbuf = ( BYTE* )response_str.c_str();
            int   sendlen = response_str.size();
            if( accept_coding_ == CsHttpEncType_GZip )
            {
                httpwriter->SetContentCoding( "gzip" );
                httpwriter->SetAcceptCoding( "gzip" );
                int compress_len = pfzip::GZip::GetMaxCompressedLen( sendlen );

                if( zip_length_ < compress_len )
                {
                    s_buffer_storage.Return( zip_buffer_ );
                    zip_buffer_ = s_buffer_storage.Borrow( compress_len );
                    zip_length_ = compress_len;
                }

                sendlen = pfzip::GZip::CompressBuffer( sendbuf, sendlen, zip_buffer_, zip_length_ );
                sendbuf = zip_buffer_;
            }

            if( !content_type_.empty() )
            {
                httpwriter->SetContentType( content_type_.c_str() );
            }

            httpwriter->SetRecord( ( const char* )sendbuf, sendlen );
            size_t len = 0;
            const char* response = httpwriter->GetBuffer( len );
            int send_size = s->Sendn( ( const ubyte_t* )response, len );

            if( send_size != len )
            {
                Pf_DEBUG( response );
                Pf_DEBUG( "\n" );
            }

            s_buffer_storage.Return( zip_buffer_ );
            //SAFE_DELETE_ARRAY( response_buf );
            //SAFE_DELETE_ARRAY(compress_send_buf);
        }
        */

        Kickout( s );
        //Pf_DEBUG( "http end" );

    }


    //===============================================================================//
    // NAME :
    // DESC :
    //===============================================================================//
    std::string HttpServer::OnRecvOutputDebug( net::TCPSocket* s, CsHttpEncType enctype, const char* comstr, int comlen )
    {
        if( enctype != CsHttpEncType_Application )
        {
            return std::string();
        }

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

        if( dbgf )
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
        std::string HttpServer::OnRecvLogin(net::TCPSocket* s, CsHttpEncType enctype, const char* comstr, int comlen)
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
        std::string HttpServer::OnRecvSelectSphere(net::TCPSocket* s, CsHttpEncType enctype, const char* comstr, int comlen)
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
        std::string HttpServer::OnRecvInsertPiece(net::TCPSocket* s, CsHttpEncType enctype, const char* comstr, int comlen)
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
        std::string HttpServer::OnRecvUpdatePiece(net::TCPSocket* s, CsHttpEncType enctype, const char* comstr, int comlen)
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



