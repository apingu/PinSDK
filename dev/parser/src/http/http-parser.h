#ifndef HTTPHELPER_H
#define HTTPHELPER_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <map>

// A class to incrementally parse an HTTP header as it comes in. It
// lets you know when it has received all required bytes, as specified
// by the content-length header (if present). If there is no content-length,
// it will stop reading after the final "\n\r".
//
// Example usage:
//
// Parser parser;
// Parser::status_t status;
//
// for( ;; ) {
//  // read bytes from socket into buffer, break on error
//  status = parser.addBytes( buffer, length );
//  if ( status != Parser::Incomplete ) break;
// }
//
// if ( status == Parser::Done ) {
//  // parse fully formed http message.
// }

enum HTTPVType
{
    HTTPVType_String,
    HTTPVType_Bool,
    HTTPVType_Double,
    HTTPVType_Float,
    HTTPVType_LLong,
    HTTPVType_Long,
    HTTPVType_Short,
    HTTPVType_ULLong,
    HTTPVType_ULong,
    HTTPVType_UShort
};

namespace http
{
    class Parser
    {
    public:
        Parser();
        ~Parser();

        enum status_t
        {
            Done,
            Error,
            Incomplete
        };

        // return 1 if success
        int         addBytes( const char* bytes, unsigned len );
        int         getNeedDataLength();
        const char* getRawBuffer();
        int         getRawLength();
        void        release();

        const char* getMethod();
        const char* getUri();
        const char* getQueryString();
        const char* getBody();
        unsigned    getBodyLength();
        // key should be in lower case when looking up.
        const char* getField( const char* key );
        unsigned    getContentLength();

        static bool getField( const char* uri, const char* key, HTTPVType type, void* data );

    private:
        void        parseHeader();
        bool        parseRequestLine();

        std::string _data;
        unsigned    _read_data;
        unsigned    _headerStart;
        unsigned    _bodyStart;
        unsigned    _parsedTo;
        int         _state;
        unsigned    _keyIndex;
        unsigned    _valueIndex;
        unsigned    _contentLength;
        unsigned    _contentStart;
        unsigned    _uriIndex;
        std::map< std::string, std::string > _keyvalue;

        //typedef std::vector<unsigned> IntArray;
        //IntArray _keys;

        enum State
        {
            p_request_line = 0,
            p_request_line_cr = 1,
            p_request_line_crlf = 2,
            p_request_line_crlfcr = 3,
            p_key = 4,
            p_key_colon = 5,
            p_key_colon_sp = 6,
            p_value = 7,
            p_value_cr = 8,
            p_value_crlf = 9,
            p_value_crlfcr = 10,
            p_content = 11, // here we are done parsing the header.
            p_error = 12  // here an error has occurred and the parse failed.
        };

        status_t _status;
    };

    struct ContentField
    {
        struct SubField
        {
            int   Type;  //1 as string
            std::string Data;
        };
        std::string                     Disposition;
        std::map<std::string, SubField> DispositionValue;
        std::string                     Type;
        std::map<std::string, SubField> TypeValue;
        std::string                     Data;
    };

    //void HttpContentData_ParseContent( const char* bytes, unsigned len, const char* boundary, std::vector<ContentField>& fieldvec );

    class Writer
    {

        std::string  m_Header;
        //std::string  m_Server;
        //std::string  m_AccessControlAllowOrigin;
        //std::string  m_ContentType;
        //size_t       m_ContentLength;
        //std::string  m_Connection;
        //std::string  m_Keep_Alive;
        //std::string  m_AcceptCoding;
        //std::string  m_ContentCoding;
        std::map< std::string, std::string > _keyvalue;
        std::vector< std::string >           _cookies;

        char*        m_pContentBuf;
        char*        m_Buffer;
        size_t       m_Length;

    public:

        Writer();
        ~Writer();

        void        SetHeader( const char* field, const char* value );
        const char* GetHeader( const char* field );
        void        AddCookie( const char* field, const char* value, int maxage, const char* path = NULL, const char* domain = NULL, bool secure = false );

        void        SetContent( const char* buf, size_t len );

        void        SetContentType( const char* buf );
        // [gzip] or other
        void        SetContentCoding( const char* buf );
        // cant [gzip] or other
        void        SetAcceptCoding( const char* buf );

        void        GetFileLength();

        const char* GetBuffer( size_t& len );

    };

#define urlcodec_buffer_length(s)   ((strlen(s) * 3) + 1)
    char* url_encode( _INOUT char* inouts );
    char* url_decode( _INOUT char* inouts );
}

#endif