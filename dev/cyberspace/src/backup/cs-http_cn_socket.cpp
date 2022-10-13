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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
#include <pf-ziplib.h>
#include <buffer_storage.h>
#include <parser/http-parser.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-http_cn_socket.h"


namespace cyberspace
{
    namespace
    {
        BufferStorage s_buffer_storage;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    HttpCnSocket::HttpCnSocket()
    {
        v_http_parser_ = NULL;
        v_http_writer_ = NULL;
        Clear();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    HttpCnSocket::~HttpCnSocket()
    {
        SAFE_DELETE( v_http_parser_ );
        SAFE_DELETE( v_http_writer_ );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void HttpCnSocket::Clear()
    {
        SAFE_DELETE( v_http_parser_ );
        SAFE_DELETE( v_http_writer_ );
        v_http_parser_ = new http::Parser;
        v_http_writer_ = new http::Writer;
        context_coding_ = CsHttpEncType_Normal;
        accept_coding_ = CsHttpEncType_Normal;
        zip_buffer_ = NULL;
        zip_length_ = 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void HttpCnSocket::SetSessionUid( cs_session_uid_t cid )
    {
        _mstring allowheader = v_http_writer_->GetHeader( "Access-Control-Expose-Headers" );
        if( allowheader == "" )
        {
            allowheader += "X-CsSession";
        }
        else
        {
            allowheader += ", X-CsSession";
        }

        _mstring value = "SId=" + pfstd::ulltostr( cid );
        v_http_writer_->SetHeader( "Access-Control-Expose-Headers", allowheader.c_str() );
        v_http_writer_->SetHeader( "X-CsSession", value.c_str() );
        //_keyvalue[ "Access-Control-Expose-Headers" ] = "CsSessionID";
        //_keyvalue[ "CsSessionID" ] = "100";
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    cs_session_uid_t HttpCnSocket::GetSessionUid()
    {
        _mstring x_cssession = v_http_parser_->getField( "X-CsSession" );
        _mbsvector params = pfstd::string_split( x_cssession, "; " );
        for( int pi = 0; pi < params.size(); pi++ )
        {
            if( strstr( params[ pi ].c_str(), "SId=" ) != NULL )
            {
                return pfstd::strtoull( params[ pi ].c_str() + 4 );
            }
        }
        return CsINVALID_SESSIONID;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int HttpCnSocket::RecvPacket( CsHttpEncType& enctype, _mstring& actstr, const ubyte_t** combuf, cs_size_t& comlen )
    {
        //printf( _T( "01 - 00 \n" ) );
        int httpread = 0;
        int needrecvm = 0;
        {
            ulong32_t nread = GetReadLength();
            if( nread == 0 )
                return 0;

            ubyte_t* buff = new ubyte_t[ nread + 1 ];
            nread = Recv( buff, nread );
            httpread = v_http_parser_->addBytes( ( const char* )buff, nread );
            SAFE_DELETE_ARRAY( buff );
            needrecvm = nread;
        }
        
        // 要確定資料流已經收完了
        int rc = 0;
        while( httpread != 1 )
        {
            //printf( _T( "01 - 01 \n" ) );
            ulong32_t nread = GetReadLength();
            if( nread == 0 )
            {
                // 以免死結
                if( rc > 20 ) return 0;
                rc++;
                // 要等一下 才讀的道下一筆資料 否則就會一直卡在這邊
                pfstd::MsSleep( 100 );
                //if( errno == EINTR )
                //{
                //    break;
                //}
                continue;
            }
            //// 要等一下 才讀的道下一筆資料
            //pfstd::MsSleep( 10 );
            // 因為 有可能連 headle 都沒收完 所以取不到 need data length 改回用 tcp 收到資料的方法
            //ulong32_t nread = v_http_parser_->getNeedDataLength();
            int readret = 0;
            ubyte_t* buff = new ubyte_t[ nread * 2 ];
            memset( buff, 0, nread + 1 );
            //printf( "RecvPacket 3-1 client %d; recv %d\n", GetFD(), nread );
            if( ( readret = Recvn( buff, nread ) ) > 0 )
            {
                //buff[ readret ] = 0;
                //printf( "RecvPacket 3-2 client %d recv %d; read %d\n", GetFD(), nread, readret );
                needrecvm += readret;
                //printf( "RecvPacket 3-2 client %d recv %d; read %d; length = %d\n", GetFD(), nread, readret, needrecvm );
                httpread = v_http_parser_->addBytes( ( const char* )buff, readret );
            }
            //printf( "RecvPacket 3-4 client %d recv %d read %d, ret %d\n", GetFD(), nread, readret, httpread );
            SAFE_DELETE_ARRAY( buff );
            // 已關閉
            if( readret <= 0 )
            {
                return 0;
            }
        }

        //printf( "RecvPacket 4 client %d\n", GetFD() );
        //ubyte_t* zip_buffer_ = NULL;
        //int      zip_length_ = 0;
        context_coding_ = CsHttpEncType_Normal;
        accept_coding_  = CsHttpEncType_Normal;
        {
            const char* Context_Encoding = v_http_parser_->getField( "content-encoding" );
            if( Context_Encoding )
            {
                if( strstr( Context_Encoding, "gzip" ) != NULL )
                {
                    context_coding_ = CsHttpEncType_GZip;
                }
            }

            const char* Accept_Encoding = v_http_parser_->getField( "accept-encoding" );
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
            const char* uri = v_http_parser_->getUri();
            //const char* qur = httpparser.getQueryString();
            char* dbuf = http::url_decode( (char*)uri );
            //printf("%s\n", dbuf);
            {
                char* pdbuf     = dbuf;
                char* actstrbuf = strtok( pdbuf, "?" );
                char* tmpbuf01  = NULL;
                //BYTE* tmpbuf02=NULL;
                const char* comstrbuf = NULL;
                //int         comlen = 0;
                CsHttpEncType content_type = CsHttpEncType_Application;
                {
                    const char* str = v_http_parser_->getField( "content-type" );

                    if( str )
                    {
                        if( strstr( str, "multipart/form-data" ) != NULL )
                        {
                            content_type = CsHttpEncType_Multipart;
                        }
                    }
                }

                if( strncmp( v_http_parser_->getMethod(), "GET", 3 ) == 0 )
                {
                    comstrbuf = strtok( NULL, "\0" );

                    if( comstrbuf )
                    {
                        comlen = _csstrlen( comstrbuf ) + 1;
                    }
                }
                else if( strncmp( v_http_parser_->getMethod(), "POST", 4 ) == 0 )
                {
                    const BYTE* httpbody = ( BYTE* )v_http_parser_->getBody();
                    int         httplen  = v_http_parser_->getContentLength();

                    if( httpbody )
                    {
                        if( context_coding_ == CsHttpEncType_GZip )
                        {
                            int maxlen = pfzip::GZip::GetMaxUncompressedLen( httpbody, httplen );

                            if( zip_buffer_ != NULL )
                            {
                                s_buffer_storage.Return( zip_buffer_ );
                            }
                            zip_buffer_ = ( BYTE* )s_buffer_storage.Borrow( maxlen );// new BYTE[maxlen];
                                                                                // zip_buffer_ = new BYTE[maxlen];
                            zip_length_ = pfzip::GZip::UncompressBuffer( httpbody, httplen, zip_buffer_, maxlen );
                            httpbody = zip_buffer_;
                            httplen = zip_length_;
                            zip_buffer_[ zip_length_ ] = 0;
                        }

                        if( content_type == CsHttpEncType_Application )
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
                            comstrbuf = ( const char* )httpbody;
                        }
                    }
                    comlen = httplen;
                }

                enctype   = content_type;
                if( actstrbuf != NULL )
                    actstr = actstrbuf;
                else
                    actstr = "";
                (*combuf) = (const ubyte_t*)comstrbuf;
                //comlen    = comlen;
            }
        }
        //printf( "RecvPacket 7 client %d\n", GetFD() );
        return 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int HttpCnSocket::SendPacket( const _binbuf& response_str )
    {
        // 20180816
        // 要找一下 為何要把這邊設為 origin 而不是 *
        // 如果設為* browser 會整個 將訊息擋掉 所以不能設為*
        v_http_writer_->SetHeader( "Access-Control-Allow-Origin", v_http_parser_->getField( "origin" ) );

        //char* response_buf = new char[ (response_str.size() * 4 ) + 1 ];
        //strcpy( response_buf, response_str.c_str() );

        //BYTE* sendbuf = ( BYTE* )url_encode( response_buf );
        //int   sendlen = strlen( response_buf );//response_str.size();

        const BYTE* sendbuf = ( const BYTE* )&response_str[0];
        int   sendlen = response_str.size();
        if( accept_coding_ == CsHttpEncType_GZip )
        {
            v_http_writer_->SetContentCoding( "gzip" );
            v_http_writer_->SetAcceptCoding( "gzip" );
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

        //if( !content_type_.empty() )
        //{
        //    v_http_writer_->SetContentType( content_type_.c_str() );
        //}

        v_http_writer_->SetContent( ( const char* )sendbuf, sendlen );
        size_t len = 0;
        const char* response = v_http_writer_->GetBuffer( len );
        int send_size = Sendn( ( const ubyte_t* )response, len );

        if( send_size != len )
        {
            Pf_INFO( response );
            Pf_INFO( "\n" );
        }

        s_buffer_storage.Return( zip_buffer_ );
        //SAFE_DELETE_ARRAY( response_buf );
        //SAFE_DELETE_ARRAY(compress_send_buf);
        return 0;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //int HttpCnSocket::Send( _IN const ubyte_t* bp, _IN cs_size_t len, _IN unsigned int flag )
    //{
    //    return BaseClass::Sendn( bp, len, flag );
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //int HttpCnSocket::Recv( _OUT ubyte_t* bp, _IN cs_size_t len, _IN unsigned int flag )
    //{
    //    return BaseClass::Recvn( bp, len, flag );
    //}
}