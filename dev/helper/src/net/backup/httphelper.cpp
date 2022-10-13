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

//01. == C 系統文件/C++ 系統文件
#include <Urlmon.h>
#include <Wininet.h>
#include <stdio.h>
#include <io.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pffile.h>

//03. == 其他 使用 函式庫的.h 文件
#ifdef CURL_STATICLIB
//#include <cstdio>
#include <curl\curl.h>
#ifdef _WINDOWS
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "wldap32.lib" )
#pragma comment ( lib, "crypt32.lib" )
//#pragma comment ( lib, "libssl.lib" )
//#pragma comment ( lib, "libcrypto.lib" )
#endif
#endif

//04. == 此專案內.h 文件
#include "HttpHelper.h"

//#pragma comment(lib, "Urlmon.lib")

namespace net
{
    namespace
    {
        bool g_is_init_ = false;
    }

    HttpHelper::HttpHelper()
    {
        sec_per_mbyte_ = 0;
        strcpy( cookie_filename_, "/tmp/cookie.txt" );

#ifdef CURL_STATICLIB
        if( !g_is_init_ )
        {
            g_is_init_ = true;
            /*   调用curl_global_init()初始化libcurl  */
            CURLcode res;
            res = curl_global_init( CURL_GLOBAL_ALL );
            if( CURLE_OK != res )
            {
                printf( "init libcurl failed." );
                curl_global_cleanup();
            }
        }
#endif
    }

    HttpHelper::~HttpHelper()
    {
#ifdef CURL_STATICLIB
        if( g_is_init_ )
        {
            curl_global_cleanup();
        }        
#endif
    }

#ifdef CURL_STATICLIB

    size_t write_data( void *ptr, size_t size, size_t nmemb, FILE *stream )
    {
        size_t written = fwrite( ptr, size, nmemb, stream );
        return written;
    }

    int CURL_DownloadFile( const char* url, const char* outfilename )
    {
        CURL *curl;
        FILE *fp;
        CURLcode res;
        /*  调用curl_easy_init()函数得到 easy interface型指针  */
        curl = curl_easy_init();
        if( curl )
        {
            fopen_s( &fp, outfilename, "wb" );
            /*  调用curl_easy_setopt()设置传输选项 */
            res = curl_easy_setopt( curl, CURLOPT_URL, url );
            if( res != CURLE_OK )
            {
                fclose( fp );
                curl_easy_cleanup( curl );
                return -1;
            }
            /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
            res = curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );
            if( res != CURLE_OK )
            {
                fclose( fp );
                curl_easy_cleanup( curl );
                return -1;
            }
            /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
            res = curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
            if( res != CURLE_OK )
            {
                fclose( fp );
                curl_easy_cleanup( curl );
                return -1;
            }

            res = curl_easy_perform( curl );                               // 调用curl_easy_perform()函数完成传输任务
            size_t flen = filelength( fp );
            fclose( fp );
            /* Check for errors */
            if( ( res != CURLE_OK ) || ( flen == 0 ) )
            {
                _unlink( outfilename );
                fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror( res ) );
                curl_easy_cleanup( curl );
                return -1;
            }

            /* always cleanup */
            curl_easy_cleanup( curl );                                     // 调用curl_easy_cleanup()释放内存 
        }
        return S_OK;
    };
#endif

    bool HttpHelper::DownloadFile( const char_t* remoteUrlFile, const char_t* localFile )
    {
        for( int i = 0; i < 10; i++ )
        {
            DWORD btime = ::GetTickCount();
#ifndef CURL_STATICLIB
            if( URLDownloadToFileA( 0, remoteUrlFile, localFile, 0, 0 ) == S_OK )
#else
            if( CURL_DownloadFile( remoteUrlFile, localFile ) == S_OK )
#endif
            {
                DWORD etime = ::GetTickCount();
                FILE* f = fopen( localFile, "rb" );
                if( f )
                {
                    size_t flen = _filelength( fileno( f ) );
                    fclose( f );
                    int dtime = ( etime - btime );
                    if( dtime == 0 )
                    {
                        dtime = 1;
                    }
                    sec_per_mbyte_ = flen / dtime;
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
        }

        return 0;
    }

    bool HttpHelper::GetUrl( const char_t* url, const char_t* local_file )
    {
#ifdef CURL_STATICLIB
        CURL *curl;
        CURLcode res;

        curl = curl_easy_init();
        if( curl ) 
        {
            FILE *fp;
            if( ( fp = fopen( local_file, "wb" ) ) == NULL )  // 返回结果用文件存储
                return false;

            /* First set the URL that is about to receive our POST. This URL can
               just as well be a https:// URL if that is what should receive the
               data. */
            curl_easy_setopt( curl, CURLOPT_URL, url );
            curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L );
            curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L );
            curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp ); //将返回的http头输出到fp指向的文件
            //curl_easy_setopt( curl, CURLOPT_HEADERDATA, fp ); //将返回的html主体数据输出到fp指向的文件

            /* Perform the request, res will get the return code */
            res = curl_easy_perform( curl );
            size_t flen = filelength( fp );
            fclose( fp );
            /* Check for errors */
            if( ( res != CURLE_OK ) || ( flen == 0 ) )
            {
                _unlink( local_file );
                curl_easy_cleanup( curl );
                return false;
            }
            /* always cleanup */
            curl_easy_cleanup( curl );
        }
        return true;
#endif
    }

    bool HttpHelper::PostUrl( const char_t* url, const char_t* content, const char_t* local_file )
    {
#ifdef CURL_STATICLIB
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if( curl )
        {
            FILE *fp;
            if( ( fp = fopen( local_file, "wb" ) ) == NULL )
                return false;

            curl_easy_setopt( curl, CURLOPT_COOKIEFILE, cookie_filename_ ); // 指定cookie文件
            curl_easy_setopt( curl, CURLOPT_POSTFIELDS, content );    // 指定post内容
            //curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");
            curl_easy_setopt( curl, CURLOPT_URL, url );   // 指定url
            curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
            res = curl_easy_perform( curl );
            size_t flen = filelength( fp );
            fclose( fp );
            /* Check for errors */
            if( ( res != CURLE_OK ) || ( flen == 0 ) )
            {
                _unlink( local_file );
                curl_easy_cleanup( curl );
                return false;
            }
            curl_easy_cleanup( curl );
        }
        return true;
#endif
    }
}
