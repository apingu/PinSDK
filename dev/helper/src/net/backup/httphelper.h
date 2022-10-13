#ifndef NETHTTPHELPER_H
#define NETHTTPHELPER_H

#include <pfdef.h>

namespace net
{
    class HttpHelper
    {

        unsigned int  sec_per_mbyte_;   // 平均每秒可下載的 檔案大小
        char_t        cookie_filename_[ 256 ];

    public:

        HttpHelper();
        ~HttpHelper();

        bool          DownloadFile( _IN const char_t* remote_url_file, _OUT const char_t* local_file );

        unsigned int  GetMSecPerByte() { return sec_per_mbyte_; }

        bool          GetUrl( _IN const char_t* url, _OUT const char_t* local_file );

        bool          PostUrl( _IN const char_t* url, _IN const char_t* content, _OUT const char_t* local_file );
    };
}

#endif //NETHTTPHELPER_H