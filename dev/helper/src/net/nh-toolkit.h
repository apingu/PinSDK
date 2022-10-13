//
//
//
//
//
//
//
//
// g++ curl_test.cpp -I /usr/local/curl/include/ -I /usr/local/libssh2/include/ -I /usr/local/openssl/include/  -L /usr/local/curl/lib/ -L /usr/local/libssh2/lib/ -L /usr/local/openssl/lib/ -lrt -lcurl -lssh2 -lssl -lcrypto -ldl -lz
//                          linux:  
//                                dnf install curl-devel
//                                curl   再執行檔中link
//
#pragma once


#include <pf-def.h>

namespace net_helper
{
    //////////////////////////////////////////////////////////////////////////
    // 透過 url
    namespace http
    {
        class ToolKit
        {
            unsigned int  sec_per_mbyte_;   // 平均每秒可下載的 檔案大小
            char_t        cookie_filename_[ 256 ];

        public:

            ToolKit();
            ~ToolKit();


            unsigned int  GetMSecPerByte() { return sec_per_mbyte_; }

            bool          DownloadFile( _IN const char_t* remote_file, _IN const char_t* local_file );

            // 發送一個 get Method
            bool          Get( _IN const char_t* url, _OUT const char_t* local_file );

            // 發送一個 post Method
            bool          Post( _IN const char_t* url, _IN const char_t* content, _IN const char_t* local_file );
        };
    }

    //////////////////////////////////////////////////////////////////////////
    // SSH
    namespace sftp
    {
        class ToolKit
        {
        public:

            ToolKit();
            ~ToolKit();

            // 下載一個檔案
            bool DownloadFile( _IN const char_t * username, _IN const char_t * password, _IN const char_t* remote_file, _IN const char_t* local_file );

            // 上傳一個檔案
            // "sftp://ip/path..../filename"
            bool UploadFile( _IN const char_t * username, _IN const char_t * password, const _IN char_t * local_file, _IN const char_t * remote_file );
        };
    }
}