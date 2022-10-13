
#include <string.h>
#include <algorithm>

#include <pf-stdlib.h>

#include "http-parser.h"

namespace http
{
    Parser::Parser():
        _headerStart( 0 ),
        _bodyStart( 0 ),
        _status( Incomplete ),
        _state( 0 ),
        _parsedTo( 0 ),
        _keyIndex( 0 ),
        _valueIndex( 0 ),
        _contentLength( 0 ),
        _contentStart( 0 ),
        _uriIndex( 0 ),
        _read_data( 0 )
    {
    }

    Parser::~Parser()
    {
    }

    void Parser::parseHeader()
    {
        // run the fsm.
        const int  CR = 13;
        const int  LF = 10;
        const int  ANY = 256;
        enum Action
        {
            // make lower case
            LOWER = 0x1,

            // convert current character to null.
            NULLIFY = 0x2,

            // set the header index to the current position
            SET_HEADER_START = 0x4,

            // set the key index to the current position
            SET_KEY = 0x8,

            // set value index to the current position.
            SET_VALUE = 0x10,

            // store current key/value pair.
            STORE_KEY_VALUE = 0x20,

            // sets content start to current position + 1
            SET_CONTENT_START = 0x40
        };
        static const struct FSM
        {
            State curState;
            int c;
            State nextState;
            unsigned actions;
        } fsm[] =
        {
            { p_request_line,   CR, p_request_line_cr,  NULLIFY       },
            { p_request_line,  ANY, p_request_line,  0          },
            { p_request_line_cr,   LF, p_request_line_crlf,   0          },
            { p_request_line_crlf, CR, p_request_line_crlfcr, 0          },
            { p_request_line_crlf,   ANY, p_key,     SET_HEADER_START | SET_KEY | LOWER },
            { p_request_line_crlfcr,  LF, p_content,    SET_CONTENT_START      },
            { p_key,     ':', p_key_colon,     NULLIFY       },
            { p_key,     ANY, p_key,     LOWER         },
            { p_key_colon,     ' ', p_key_colon_sp,  0          },
            { p_key_colon_sp,  ANY, p_value,      SET_VALUE        },
            { p_value,    CR, p_value_cr,   NULLIFY | STORE_KEY_VALUE    },
            { p_value,      ANY, p_value,      0          },
            { p_value_cr,    LF, p_value_crlf,    0          },
            { p_value_crlf,     CR, p_value_crlfcr,  0          },
            { p_value_crlf,    ANY, p_key,     SET_KEY | LOWER     },
            { p_value_crlfcr,   LF, p_content,    SET_CONTENT_START      },
            { p_error,      ANY, p_error,      0          }
        };

        for( unsigned i = _parsedTo; i < getRawLength(); ++i )
        {
            char c = _data[ i ];
            State nextState = p_error;

            for( unsigned d = 0; d < sizeof( fsm ) / sizeof( FSM ); ++d )
            {
                if( fsm[ d ].curState == _state && ( c == fsm[ d ].c || fsm[ d ].c == ANY ) )
                {
                    nextState = fsm[ d ].nextState;

                    if( fsm[ d ].actions & LOWER )
                    {
                        _data[ i ] = tolower( _data[ i ] );
                    }

                    if( fsm[ d ].actions & NULLIFY )
                    {
                        _data[ i ] = 0;
                    }

                    if( fsm[ d ].actions & SET_HEADER_START )
                    {
                        _headerStart = i;
                    }

                    if( fsm[ d ].actions & SET_KEY )
                    {
                        _keyIndex = i;
                    }

                    if( fsm[ d ].actions & SET_VALUE )
                    {
                        _valueIndex = i;
                    }

                    if( fsm[ d ].actions & SET_CONTENT_START )
                    {
                        _contentStart = i + 1;
                    }

                    if( fsm[ d ].actions & STORE_KEY_VALUE )
                    {
                        // store position of first character of key.
                        //_keys.push_back( _keyIndex );
                        const char* key = &_data[ _keyIndex ];

                        std::string keystr = key;
                        std::transform( keystr.begin(), keystr.end(), keystr.begin(), ::tolower );
                        _keyvalue.insert( std::map< std::string, std::string >::value_type( key, &_data[ _keyIndex + strlen( key ) + 2 ] ) );
                    }

                    break;
                }
            }

            _state = nextState;

            if( _state == p_content )
            {
                const char* str = getField( "content-length" );

                if( str )
                {
                    _contentLength = atoi( str );
                }

                break;
            }
        }

        _parsedTo = getRawLength();
    }

    bool  Parser::parseRequestLine()
    {
        size_t sp1;
        size_t sp2;
        sp1 = _data.find( ' ', 0 );

        if( sp1 == std::string::npos )
        {
            return false;
        }

        sp2 = _data.find( ' ', sp1 + 1 );

        if( sp2 == std::string::npos )
        {
            return false;
        }

        _data[ sp1 ] = 0;
        _data[ sp2 ] = 0;
        _uriIndex = sp1 + 1;
        return true;
    }

    int Parser::addBytes( const char* bytes, unsigned len )
    {
        if( _status != Incomplete )
        {
            //printf( "addBytes 0 \n" );
            return -1;
        }

        //if( ( strcmp( bytes, "" ) != 0 ) && ( len > 0 ) )
        // 可能因為 binary 所以 這邊 有機會會是 ""
        if( len > 0 )
        {
            // append the bytes to data.
            _data.append( bytes, len );
            _read_data += len;
            //printf( "addBytes add len %d\n", len );

            if( _state < p_content )
            {
                parseHeader();
            }

            if( _state == p_error )
            {
                _status = Error;
            }
            else if( _state == p_content )
            {
                // 沒有內容 或收完資料嘞
                if( ( _contentLength == 0 ) || ( ( getRawLength() - _contentStart ) >= _contentLength ) )
                {
                    if( parseRequestLine() )
                    {
                        _status = Done;
                        return 1;
                    }
                    else
                    {
                        _status = Error;
                    }
                }
            }
        }
        else
        {
            //printf( "addBytes failure len %d\n", len );
        }

        // because if bzip content length not equal need length
        //int bodylen = getBodyLength();
        //int needlen = ( _contentStart + _contentLength ) - _read_data;
        //printf( "addBytes 1 content %d; datalen %d; start %d; lenght %d\n", _contentLength, getRawLength(), _contentStart, _contentLength );
        //return ( _contentStart + _contentLength ) - _read_data;
        return 0;
    }

    int Parser::getNeedDataLength()
    {
        return ( _contentStart + getContentLength() ) - getRawLength();
    }

    const char* Parser::getRawBuffer()
    {
        return _data.c_str();
    }

    int Parser::getRawLength()
    {
        //return _data.length();
        return _read_data;
    }

    void Parser::release()
    {
        _data.clear();
        _keyvalue.clear();
        _headerStart = 0,
            _bodyStart = 0,
            _status = Incomplete,
            _state = 0,
            _parsedTo = 0,
            _keyIndex = 0,
            _valueIndex = 0,
            _contentLength = 0,
            _contentStart = 0,
            _uriIndex = 0;
    }

    const char* Parser::getMethod()
    {
        return &_data[ 0 ];
    }

    const char* Parser::getUri()
    {
        return &_data[ _uriIndex ];
    }

    const char* Parser::getQueryString()
    {
        const char* pos = getUri();

        while( *pos )
        {
            if( *pos == '?' )
            {
                pos++;
                break;
            }
            pos++;
        }

        return pos;
    }

    const char* Parser::getBody()
    {
        if( ( _contentLength > 0 ) && ( _contentStart < _data.size() ) )
        {
            return &_data[ _contentStart ];
        }
        else
        {
            return NULL;
        }
    }

    unsigned Parser::getBodyLength()
    {
        if( _data.size() > 0 )
        {
            return _data.size() - _contentStart;
        }

        return 0;
    }

    // key should be in lower case.
    const char* Parser::getField( const char* key )
    {
        std::string keystr = key;
        std::transform( keystr.begin(), keystr.end(), keystr.begin(), ::tolower );

        std::map< std::string, std::string >::iterator iter = _keyvalue.find( keystr.c_str() );
        if( iter != _keyvalue.end() )
        {
            return iter->second.c_str();
        }
        return NULL;

        //for ( IntArray::iterator iter = _keys.begin(); iter != _keys.end(); ++iter )
        //{
        //    unsigned index = *iter;
        //    
        //    if ( strcasecmp( (const char*)&_data[ index ], key ) == 0 )
        //    {
        //        return &_data[ index + strlen( key ) + 2 ];
        //    }
        //}
        //
        //return NULL;
    }

    unsigned Parser::getContentLength()
    {
        return _contentLength;
    }

    bool Parser::getField( const char* uri, const char* key, HTTPVType type, void* data )
    {
        bool ret = false;
        char* buf = new char[ strlen( uri ) + 1 ];
        strcpy( buf, uri );
        char* p = NULL;
        char* bp = strstr( buf, key );

        if( bp == NULL )
        {
            return false;
        }

        /*
        if(strstr(buf, "?") !=0)
        {
         p=strtok(buf, "?");
         p=strtok(NULL, "&");
         //p=strtok(buf, "&");
        }
        else
        */
        {
            p = strtok( bp, "&" );
        }

        while( p )
        {
            //char* pkey = p;
            char* pval = p;
            int   len = 0;

            for( size_t i = 0; i < strlen( p ); i++ )
            {
                if( p[ i ] == '=' )
                {
                    len = i;
                    pval = &p[ i + 1 ];
                    break;
                }
            }

            //if(strcmp(tkey, key) == 0)
            if( ( len > 0 ) && ( memcmp( p, key, len ) == 0 ) )
            {
                switch( type )
                {
                    case HTTPVType_String:
                        {
                            ( *( ( std::string* )data ) ) = pval;
                            ret = true;
                        }
                        break;

                    case HTTPVType_Bool:
                        {
                            ( *( bool* )data ) = ( bool )atoi( pval );
                            ret = true;
                        }
                        break;

                    case HTTPVType_Double:
                        {
                            ( *( ( double* )data ) ) = ( double )atof( pval );
                            ret = true;
                        }
                        break;

                    case HTTPVType_Float:
                        {
                            ( *( ( float* )data ) ) = ( float )atof( pval );
                            ret = true;
                        }
                        break;

                    case HTTPVType_LLong:
                        {
                            ( *( ( long64_t* )data ) ) = atoll( pval );
                            ret = true;
                        }
                        break;

                    case HTTPVType_Long:
                        {
                            ( *( ( int* )data ) ) = ( int )atoi( pval );
                            ret = true;
                        }
                        break;

                    case HTTPVType_Short:
                        {
                            ( *( ( short* )data ) ) = ( short )atoi( pval );
                            ret = true;
                        }
                        break;
                    case HTTPVType_ULLong:
                        {
                            char* stopstring;
                            ( *( ( ulong64_t* )data ) ) = ( ulong64_t )strtoull( pval, &stopstring, 10 );
                            ret = true;
                        }
                        break;

                    case HTTPVType_ULong:
                        {
                            char* stopstring;
                            ( *( ( uint_t* )data ) ) = ( uint_t )strtoul( pval, &stopstring, 10 );
                            ret = true;
                        }
                        break;

                    case HTTPVType_UShort:
                        {
                            char* stopstring;
                            ( *( ( ushort_t* )data ) ) = ( ushort_t )strtoul( pval, &stopstring, 10 );
                            ret = true;
                        }
                        break;
                }

                break;
            }

            p = strtok( NULL, "&" );
        }

        delete[] buf;
        return ret;
    }

    bool ParseHttpHeaderLine( const char* str, int len, std::string& vn, std::string& vv, std::map<std::string, ContentField::SubField>& k )
    {
        if( len == 0 )
        {
            return false;
        }

        char* cline = new char[ len + 1 ];
        memcpy( cline, str, len );
        cline[ len ] = 0;
        char* p = cline;
        char* s1 = strstr( p, ": " );

        if( s1 )
        {
            vn.append( p, s1 - p );
        }
        else
        {
            return false;
        }

        p = s1 + strlen( ": " );
        char* s2 = p;

        while( ( ( *s2 ) != 0 ) && ( ( *s2 ) != '\n' ) && ( ( *s2 ) != ';' ) )
        {
            s2++;
        }

        int s2len = s2 - p;

        if( s2len > 0 )
        {
            vv.append( p, s2len );
        }
        else
        {
            return false;
        }

        if( ( ( *s2 ) == 0 ) || ( ( *s2 ) == '\n' ) )
        {
            return true;
        }
        else
        {
            p = s2 + 1;
        }

        while( ( *p ) != 0 )
        {
            std::string k1;
            ContentField::SubField v1;
            char* ss = strstr( p, "=" );

            if( ss )
            {
                int len = ss - p;
                k1.append( p, len );
                p += ( len + strlen( "=" ) );
            }

            char* se = p;

            while( ( ( *se ) != 0 ) && ( ( *se ) != '\n' ) && ( ( *se ) != ';' ) )
            {
                se++;
            }

            int len = se - p;

            if( p[ 0 ] == '\"' )
            {
                v1.Type = 1;
                v1.Data.append( p + 1, len - 2 );
            }
            else
            {
                v1.Type = 0;
                v1.Data.append( p, len );
            }

            p += ( len );
            k.insert( std::map<std::string, ContentField::SubField>::value_type( k1, v1 ) );

            while( ( *p ) == ';' )
            {
                p++;
            }

            while( ( *p ) == ' ' )
            {
                p++;
            }
        }

        delete[] cline;
        return true;
    }

    void HttpContentData_ParseContent( const char* bytes, unsigned len, const char* boundary, std::vector<ContentField>& fieldvec )
    {
        char* pbuff = new char[ len ];
        memcpy( pbuff, bytes, len );
        std::string bound_str;
        char* p = NULL;

        // get boundary
        if( boundary == NULL )
        {
            p = strtok( pbuff, "\n" );
            bound_str = p;
            p = pbuff;
        }
        else
        {
            bound_str = boundary;
            p = pbuff;
        }

        p = strstr( p, bound_str.c_str() );

        while( p )
        {
            p += strlen( bound_str.c_str() ) + 1;
            ContentField field;
            //----------------------------------------
            char* pe = NULL;
            pe = strstr( p, "\n" );
            {
                std::string kk1;
                std::string kk2;
                std::map<std::string, ContentField::SubField> kl;

                // first line
                if( ParseHttpHeaderLine( p, pe - p, kk1, kk2, kl ) )
                {
                    if( kk1 == "Content-Disposition" )
                    {
                        field.Disposition = kk2;
                    }

                    if( kk1 == "Content-Type" )
                    {
                        field.Type = kk2;
                    }

                    p = pe + 1;
                }
            }

            while( ( ( *p ) != '\n' ) )
            {
                p++;
            }

            // second line
            pe = strstr( p, "\n" );
            {
                std::string kk1;
                std::string kk2;
                std::map<std::string, ContentField::SubField> kl;

                if( ParseHttpHeaderLine( p, pe - p, kk1, kk2, kl ) )
                {
                    if( kk1 == "Content-Disposition" )
                    {
                        field.Disposition = kk2;
                    }

                    if( kk1 == "Content-Type" )
                    {
                        field.Type = kk2;
                    }

                    p = pe + 1;
                }
            }
            //

            while( ( ( *p ) != '\n' ) )
            {
                p++;
            }

            pe = strstr( p, "\n" );
            field.Data.append( p, pe - p );
            // find end line
            p = pe + 1;
            pe = strstr( p, bound_str.c_str() );
            //----------------------------------------
            fieldvec.push_back( field );
            p = pe;
        }

        delete[] pbuff;
    }


    //////////////////////////////////////////////////////////////////////////
    //

    Writer::Writer()
    {
        m_Header = "HTTP/1.1 200 OK";
        //m_Server = "CsHttpServer";
        //m_AccessControlAllowOrigin = "*";
        //m_ContentType = "text/html; charset=utf-8";
        //m_ContentLength = 0;
        //m_Connection = "keep-Alive";
        //m_Keep_Alive = "timeout=5, max=100";
        //m_AcceptCoding = "";
        //m_ContentCoding = "";    
        m_pContentBuf = NULL;
        m_Buffer = NULL;
        m_Length = 0;

        _keyvalue[ "Server" ] = "CsHttpServer";

        //_keyvalue[ "Access-Control-Allow-Origin" ] = "*";
        _keyvalue[ "Access-Control-Allow-Credentials" ] = "true";
        //_keyvalue[ "Access-Control-Allow-Headers" ] = "*";

        _keyvalue[ "Content-Type" ] = "text/html; charset=utf-8";
        //_keyvalue[ "Content-Length" ] = "0";
        _keyvalue[ "Accept-Encoding" ] = "";
        _keyvalue[ "Content-Encoding" ] = "";
        //_keyvalue[ "Set-Cookie" ] = "CsSessionUID=0;domain=localhost;path=/" ;
        _keyvalue[ "Connection" ] = "keep-alive";
        //_keyvalue[ "keep-Alive" ] = "timeout=5, max=100";
        //_keyvalue[ "CsSessionID" ] = "100";

        //_keyvalue[ "Server" ] = "nginx/1.12.2";
        //_keyvalue[ "Content-Type" ] = "text//html";
        //_keyvalue[ "Content-Length" ] = "0";
        //_keyvalue[ "Accept-Encoding" ] = "";
        //_keyvalue[ "Content-Encoding" ] = "";
        //_keyvalue[ "Set-Cookie" ] = "TestCookie=something+from+somewhere";
        //_keyvalue[ "Connection" ] = "keep-Alive";
        //_keyvalue[ "Transfer-Encoding" ] = "chunked";
        //_keyvalue[ "X-Powered-By" ] = "PHP/5.4.16";
    };


    Writer::~Writer()
    {
        if( m_pContentBuf )
        {
            delete[] m_pContentBuf;
        }

        m_pContentBuf = NULL;

        if( m_Buffer )
        {
            delete[] m_Buffer;
        }

        m_Buffer = NULL;
    }

    void Writer::SetHeader( const char* field, const char* value )
    {
        if( value != NULL )
        {
            if( strcmp( value, "" ) != 0 )
            {
                _keyvalue.erase( field );
                _keyvalue[ field ] = value;
            }
        }
    }

    const char* Writer::GetHeader( const char* field )
    {
        return _keyvalue[ field ].c_str();
    }

    void Writer::AddCookie( const char* field, const char* value, int maxage, const char* path, const char* domain, bool secure )
    {
        _mstring cookie;

        pfstd::strprintf( cookie, "%s=%s", field, value );

        if( maxage != 0 )
        {
            pfstd::strprintfa( cookie, "; max-age=%d", maxage );
        }

        if( domain != NULL )
        {
            pfstd::strprintfa( cookie, "; domain=%s", domain );
        }

        if( path != NULL )
        {
            pfstd::strprintfa( cookie, "; path=%s", path );
        }

        if( secure )
        {
            pfstd::strprintfa( cookie, "; Secure" );
        }

        _cookies.push_back( cookie );
    }

    void Writer::SetContent( const char* buf, size_t len )
    {
        if( m_pContentBuf )
        {
            delete[] m_pContentBuf;
        }

        m_pContentBuf = NULL;
        _keyvalue[ "Content-Length" ] = "0";
        //m_ContentLength = 0;

        if( buf )
        {
            int buflen = len + 1;
            m_pContentBuf = new char[ buflen ];
            memset( m_pContentBuf, 0, buflen );
            memcpy( m_pContentBuf, buf, buflen );

            char buf[ 512 ];
            _keyvalue[ "Content-Length" ] = itoa( len, buf, 10 );
            //m_ContentLength = buflen;
        }
    }

    void Writer::SetContentType( const char* buf )
    {
        _keyvalue[ "Content-Type" ] = buf;
    }


    void Writer::SetContentCoding( const char* buf )
    {
        if( buf == NULL )
        {
            return;
        }

        if( strcmp( buf, "" ) == 0 )
        {
            _keyvalue[ "Content-Encoding" ] = "";
        }
        else
        {
            _keyvalue[ "Content-Encoding" ] = std::string( buf );
        }
    }


    void Writer::SetAcceptCoding( const char* buf )
    {
        if( buf == NULL )
        {
            return;
        }

        if( strcmp( buf, "" ) == 0 )
        {
            _keyvalue[ "Accept-Encoding" ] = "";
        }
        else
        {
            _keyvalue[ "Accept-Encoding" ] = std::string( buf );
        }
    }

    const char* Writer::GetBuffer( size_t& len )
    {
        if( m_Buffer )
        {
            delete[] m_Buffer;
        }

        //char buf[ 512 ];
        //std::string contentlen = itoa( m_ContentLength, buf, 10 );

        {
            char buf[ 1000 ];
            time_t now = time( 0 );
            struct tm tm = *gmtime( &now );
            strftime( buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm );
            _keyvalue[ "Date" ] = buf;
        }

        int contentlen = atoi( _keyvalue[ "Content-Length" ].c_str() );


        std::string response;
        response = m_Header + "\n";
        for( std::map< std::string, std::string >::iterator iter = _keyvalue.begin(); iter != _keyvalue.end(); iter++ )
        {
            if( iter->second != "" )
            {
                response = response + iter->first + ": " + iter->second + "\n";
            }
        }

        for( int i = 0; i < _cookies.size(); i++ )
        {
            response = response + "Set-Cookie: " + _cookies[ i ] + "\n";
        }

        /*
        response = m_Header + "\n" +
            "Server: " + _keyvalue[ "Server" ] + "\n" +
            "Access-Control-Allow-Origin: " + _keyvalue[ "Access-Control-Allow-Origin" ] + "\n" +
            "Content-Type: " + _keyvalue[ "Content-Type" ] + "\n" +
            "Connection: " + _keyvalue[ "Connection" ] + "\n" +
            "Keep-Alive: " + _keyvalue[ "keep-Alive" ] + "\n" +
            "Content-Length: " + _keyvalue[ "Content-Length" ] + "\n";

        if ( _keyvalue[ "Accept-Encoding" ] != "" )
        {
            response = response + std::string( "Accept-Encoding: " ) + _keyvalue[ "Accept-Encoding" ] + "\n";
        }

        if ( _keyvalue[ "Content-Encoding" ] != "" )
        {
            response = response + std::string( "Content-Encoding: " ) + _keyvalue[ "Content-Encoding" ] + "\n";
        }
        */

        response = response + "\n";
        int headerlen = strlen( response.c_str() );
        //int contentlen = atoi( _keyvalue[ "Content-Length" ].c_str() );

        m_Length = response.size() + contentlen;
        //m_Length = headerlen + m_ContentLength;

        m_Buffer = new char[ m_Length + 1 ];
        memset( m_Buffer, 0, m_Length + 1 );
        memcpy( m_Buffer, response.c_str(), response.size() );

        if( m_pContentBuf )
        {
            //memcpy( m_Buffer + response.size(), m_pContentBuf, contentlen );
            memcpy( m_Buffer + headerlen, m_pContentBuf, contentlen );
        }

        len = m_Length;
        return m_Buffer;
    }



    class CRtUrlConv
    {

    public:
        static char* php_url_encode( const char* s, int len, int* new_length )
        {
            register unsigned char c;
            unsigned char* to, * start;
            unsigned char const* from, * end;
            from = ( unsigned char* )s;
            end = ( unsigned char* )s + len;
            start = to = new unsigned char[ 3 * len + 1 ];

            while( from < end )
            {
                c = *from++;

                if( c == ' ' )
                {
                    *to++ = '+';
#ifndef CHARSET_EBCDIC
                }
                else if( ( c < '0' && c != '-' && c != '.' ) ||
                         ( c < 'A' && c > '9' ) ||
                         ( c > 'Z' && c < 'a' && c != '_' ) ||
                         ( c > 'z' ) )
                {
                    to[ 0 ] = '%';
                    to[ 1 ] = HEXCHARS[ c >> 4 ];
                    to[ 2 ] = HEXCHARS[ c & 15 ];
                    to += 3;
#else /*CHARSET_EBCDIC*/
                }
                else if( !isalnum( c ) && strchr( "_-.", c ) == NULL )
                {
                    /* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
                    to[ 0 ] = '%';
                    to[ 1 ] = HEXCHARS[ os_toascii[ c ] >> 4 ];
                    to[ 2 ] = HEXCHARS[ os_toascii[ c ] & 15 ];
                    to += 3;
#endif /*CHARSET_EBCDIC*/
                }
                else
                {
                    *to++ = c;
                }
            }

            *to = 0;

            if( new_length )
            {
                *new_length = to - start;
            }

            return ( char* )start;
        }

        static int php_url_decode( char* str )
        {
            size_t len;
            char* dest = str;
            char* data = str;
            len = strlen( str );

            while( len-- )
            {
                if( *data == '+' )
                {
                    *dest = ' ';
                }
                else if( *data == '%' && len >= 2 && isxdigit( ( int )*( data + 1 ) )
                         && isxdigit( ( int )*( data + 2 ) ) )
                {
#ifndef CHARSET_EBCDIC
                    * dest = ( char )php_htoi( data + 1 );
#else
                    * dest = os_toebcdic[ ( char )php_htoi( data + 1 ) ];
#endif
                    data += 2;
                    len -= 2;
                }
                else
                {
                    *dest = *data;
                }

                data++;
                dest++;
            }

            *dest = '\0';
            return dest - str;
        }

        static int php_htoi( char* s )
        {
            int value;
            int c;
            c = ( ( unsigned char* )s )[ 0 ];

            if( isupper( c ) )
            {
                c = tolower( c );
            }

            value = ( c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10 ) * 16;
            c = ( ( unsigned char* )s )[ 1 ];

            if( isupper( c ) )
            {
                c = tolower( c );
            }

            value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
            return ( value );
        }
        static const char hexchars[];
        static const char HEXCHARS[];
    };
    const char CRtUrlConv::hexchars[] = "0123456789abcdef";
    const char CRtUrlConv::HEXCHARS[] = "0123456789ABCDEF";


    char* url_encode( char* in_str )
    {
        char* out_str;
        int in_str_len, out_str_len;
        in_str_len = strlen( in_str );
        out_str_len = 0;
        out_str = CRtUrlConv::php_url_encode( in_str, in_str_len, &out_str_len );
        strcpy( in_str, out_str );
        delete[] out_str;
        return ( char* )in_str;
    }

    char* url_decode( char* in_str )
    {
        CRtUrlConv::php_url_decode( in_str );
        return in_str;
    }




    /* Converts a hex character to its integer value */
    //char from_hex(char ch) {
    // return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
    //}
    //
    ///* Converts an integer value to its hex character*/
    //char to_hex(char code) {
    // static char hex[] = "0123456789abcdef";
    // return hex[code  &15];
    //}
    //
    ///* Returns a url-encoded version of str */
    ///* IMPORTANT: be sure to free() the returned string after use */
    //char *http::url_encode(const char *str)
    //{
    // char *pstr = (char*)str, *buf = new char[strlen(str) * 3 + 1], *pbuf = buf;
    // while (*pstr) {
    //  if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
    //   *pbuf++ = *pstr;
    //  else if (*pstr == ' ')
    //   *pbuf++ = '+';
    //  else
    //   *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr  &15);
    //  pstr++;
    // }
    // *pbuf = '\0';
    // return buf;
    //}
    //
    ///* Returns a url-decoded version of str */
    ///* IMPORTANT: be sure to free() the returned string after use */
    //char *url_decode(const char *str)
    //{
    // char *pstr = (char*)str, *buf = new char[strlen(str) + 1], *pbuf = buf;
    // while (*pstr) {
    //  if (*pstr == '%') {
    //   if (pstr[1] &&pstr[2]) {
    //    *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
    //    pstr += 2;
    //   }
    //  } else if (*pstr == '+') {
    //   *pbuf++ = ' ';
    //  } else {
    //   *pbuf++ = *pstr;
    //  }
    //  pstr++;
    // }
    // *pbuf = '\0';
    // return buf;
    //}


}
