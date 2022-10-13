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
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

#ifdef _WINDOWS
#include <Urlmon.h>
#include <io.h>
#include <wininet.h>
#endif //_WINDOWS


//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-file.h>

//#define CURL_STATICLIB
//03. == 其他 使用 函式庫的.h 文件
#ifdef CURL_STATICLIB
//#include <cstdio>
#include <curl/curl.h>
#ifdef _WINDOWS
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "wldap32.lib" )
#pragma comment ( lib, "crypt32.lib" )
// winIDN
#pragma comment ( lib, "Normaliz.lib" )

//#pragma comment ( lib, "libcurld.lib" )
//#pragma comment ( lib, "libssl.lib" )
//#pragma comment ( lib, "libcrypto.lib" )
//#endif
#endif //_WINDOWS
#endif // CURL_STATICLIB

//04. == 此專案內.h 文件
#include "nh-toolkit.h"

//#pragma comment(lib, "Urlmon.lib")

namespace net_helper
{
    namespace
    {

        static bool g_is_init_ = false;

#ifdef CURL_STATICLIB
        class  ToolkitCommon
        {
        public:

            ToolkitCommon()
            {
                if ( !g_is_init_ )
                {
                    /*   调用curl_global_init()初始化libcurl  */
                    CURLcode res;
                    res = curl_global_init( CURL_GLOBAL_ALL );
                    if ( CURLE_OK != res )
                    {
                        printf( "init libcurl failed." );
                        //curl_global_cleanup();
                    }
                    else
                    {
                        g_is_init_ = true;
                    }
                }
            }

            ~ToolkitCommon()
            {
                if ( g_is_init_ )
                {
                    curl_global_cleanup();
                    g_is_init_ = false;
                }
            }
        };

        ToolkitCommon  s_ToolkitCommon;

        size_t write_func( void *ptr, size_t size, size_t nmemb, FILE *stream )
        {
            size_t written = fwrite( ptr, size, nmemb, stream );
            return written;
        }

        /* read data to upload */
        size_t read_func( char *ptr, size_t size, size_t nmemb, FILE *stream )
        {
            FILE *f = (FILE *)stream;
            size_t n;

            if ( ferror( f ) )
                return CURL_READFUNC_ABORT;

            n = fread( ptr, size, nmemb, f ) * size;

            return n;
        }
#endif
    }

    //////////////////////////////////////////////////////////////////////////
    namespace http
    {
        ToolKit::ToolKit()
        {
            sec_per_mbyte_ = 0;
            strcpy( cookie_filename_, "/tmp/cookie.txt" );
        }

        ToolKit::~ToolKit()
        {
        }

        namespace
        {
#ifdef CURL_STATICLIB
            bool CURL_DownloadFile( const char_t* url, const char_t* outfilename )
            {
                bool ret = false;

                CURL *curl_handle;

                FILE* fd = fopen( outfilename, "wb" );
                if ( !fd)
                {
                    return ret;
                }

                /*  调用curl_easy_init()函数得到 easy interface型指针  */
                curl_handle = curl_easy_init();
                if ( curl_handle )
                {
                    /*  调用curl_easy_setopt()设置传输选项 */
                    CURLcode res00 = curl_easy_setopt( curl_handle, CURLOPT_URL, url );

                    /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
                    CURLcode res01 = curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, write_func );

                    /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
                    CURLcode res02 = curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, fd );

                    if ( ( res00 == CURLE_OK ) && ( res01 == CURLE_OK ) && ( res02 == CURLE_OK ) )
                    {
                        CURLcode res = curl_easy_perform( curl_handle );                               // 调用curl_easy_perform()函数完成传输任务
                        size_t flen = filelength( fd );
                        /* Check for errors */
                        if ( ( res != CURLE_OK ) || ( flen == 0 ) )
                        {
                            unlink( outfilename );
                            fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror( res ) );
                        }
                        else
                        {
                            ret = true;
                        }
                    }
                }
                /* always cleanup */
                curl_easy_cleanup( curl_handle );                                     // 调用curl_easy_cleanup()释放内存 

                if ( fd )
                    fclose( fd );

                return ret;
            };
#endif
        }

        bool ToolKit::DownloadFile( _IN const char_t* remote_file, _IN const char_t* local_File )
        {
            for ( int i = 0; i < 10; i++ )
            {
                DWORD btime = ::GetTickCount();
#ifndef CURL_STATICLIB
                if ( URLDownloadToFileA( 0, remote_file, local_File, 0, 0 ) == S_OK )
#else
                if ( CURL_DownloadFile( remote_file, local_File ) )
#endif
                {
                    DWORD etime = ::GetTickCount();
                    FILE* fd = fopen( local_File, "rb" );
                    if ( !fd )
                    {
                        return -1;
                    }

                    size_t flen = _filelength( fileno( fd ) );
                    fclose( fd );
                    int dtime = ( etime - btime );
                    if ( dtime == 0 )
                    {
                        dtime = 1;
                    }
                    sec_per_mbyte_ = flen / dtime;
                    return 1;
                }
            }
            return false;
        }

        bool ToolKit::Get( _IN const char_t* url, _IN const char_t* local_File )
        {
#ifdef CURL_STATICLIB

            CURL* curl_handle;
            CURLcode res;

            curl_handle = curl_easy_init();
            if ( curl_handle )
            {
                FILE* fd = fopen( local_File, "wb" );
                if ( !fd ) // 返回结果用文件存储
                {
                    return false;
                }

                /* First set the URL that is about to receive our POST. This URL can
                   just as well be a https:// URL if that is what should receive the
                   data. */
                curl_easy_setopt( curl_handle, CURLOPT_URL, url );
                curl_easy_setopt( curl_handle, CURLOPT_SSL_VERIFYPEER, 0L );
                curl_easy_setopt( curl_handle, CURLOPT_SSL_VERIFYHOST, 0L );
                curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, fd ); //将返回的http头输出到fd指向的文件
                //curl_easy_setopt( curl_handle, CURLOPT_HEADERDATA, fd ); //将返回的html主体数据输出到fd指向的文件

                /* Perform the request, res will get the return code */
                res = curl_easy_perform( curl_handle );
                size_t flen = filelength( fd );
                fclose( fd );
                /* Check for errors */
                if ( ( res != CURLE_OK ) || ( flen == 0 ) )
                {
                    unlink( local_File );
                    curl_easy_cleanup( curl_handle );
                    return false;
                }
                /* always cleanup */
                curl_easy_cleanup( curl_handle );
            }
            return true;
#endif
            return false;
        }

        bool ToolKit::Post( _IN const char_t* url, _IN const char_t* content, _IN const char_t* local_File )
        {
#ifdef CURL_STATICLIB

            CURL *curl_handle;
            CURLcode res;

            curl_handle = curl_easy_init();
            if ( curl_handle )
            {
                FILE* fd = fopen( local_File, "wb" );
                if ( !fd ) // 返回结果用文件存储
                {
                    return false;
                }

                curl_easy_setopt( curl_handle, CURLOPT_COOKIEFILE, cookie_filename_ ); // 指定cookie文件
                curl_easy_setopt( curl_handle, CURLOPT_POSTFIELDS, content );    // 指定post内容
                //curl_easy_setopt(curl_handle, CURLOPT_PROXY, "10.99.60.201:8080");
                curl_easy_setopt( curl_handle, CURLOPT_URL, url );   // 指定url
                curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, fd );
                res = curl_easy_perform( curl_handle );
                size_t flen = filelength( fd );
                fclose( fd );
                /* Check for errors */
                if ( ( res != CURLE_OK ) || ( flen == 0 ) )
                {
                    unlink( local_File );
                    curl_easy_cleanup( curl_handle );
                    return false;
                }
                curl_easy_cleanup( curl_handle );
            }
            return true;
#endif
            return false;
        }

    }

    //////////////////////////////////////////////////////////////////////////
    //
    namespace sftp
    {

        ToolKit::ToolKit()
        {
        }

        ToolKit::~ToolKit()
        {
        }

        // 下載一個檔案
        bool ToolKit::DownloadFile( const char_t * username, const char_t * password, _IN const char_t* remote_file, _IN const char_t* local_file )
        {
            bool ret = false;

#ifdef CURL_STATICLIB
            CURL* curl_handle = NULL;

            FILE* fd = fopen( local_file, "wb" );
            if ( !fd )
            {
                return ret; /* failure, can't open file to write */
            }

            curl_handle = curl_easy_init();
            if ( curl_handle )
            {
                /*
                 * You better replace the URL with one that works!
                 */
                curl_easy_setopt( curl_handle, CURLOPT_URL, remote_file );
                /* Define our callback to get called when there's data to be written */
                curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, write_func );
                /* Set a pointer to our struct to pass to the callback */
                curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, fd );

#ifndef DISABLE_SSH_AGENT
                /* We activate ssh agent. For this to work you need
                   to have ssh-agent running (type set | grep SSH_AGENT to check) or
                   pageant on windows (there is an icon in systray if so) */
                curl_easy_setopt( curl_handle, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_AGENT );
#endif

                /* Switch on full protocol/debug output */
                curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L );

                CURLcode res = curl_easy_perform( curl_handle );
                if ( CURLE_OK != res )
                {
                    /* we failed */
                    fprintf( stderr, "curl_handle told us %d\n", res );
                }
                else
                {
                    ret = true;
                }
            }
            /* always cleanup */
            curl_easy_cleanup( curl_handle );

            if ( fd )
                fclose( fd ); /* close the local file */

#endif
            return ret;
        }

        namespace
        {
#ifdef CURL_STATICLIB
            curl_off_t SftpGetRemoteFileSize( const char *i_remoteFile )
            {
                CURLcode result = CURLE_GOT_NOTHING;
                curl_off_t remoteFileSizeByte = -1;
                CURL *curl_handle = curl_easy_init();

                curl_easy_setopt( curl_handle, CURLOPT_SSL_VERIFYPEER, false );//设定为不验证证书和HOST
                curl_easy_setopt( curl_handle, CURLOPT_SSL_VERIFYHOST, false );


                curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L );

                curl_easy_setopt( curl_handle, CURLOPT_URL, i_remoteFile );
                curl_easy_setopt( curl_handle, CURLOPT_NOPROGRESS, 1 );
                curl_easy_setopt( curl_handle, CURLOPT_NOBODY, 1 );
                curl_easy_setopt( curl_handle, CURLOPT_HEADER, 1 );
                curl_easy_setopt( curl_handle, CURLOPT_FILETIME, 1 );

                result = curl_easy_perform( curl_handle );
                if ( CURLE_OK == result )
                {
                    result = curl_easy_getinfo( curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &remoteFileSizeByte );
                    if ( result )
                        return -1;
                    printf( "filesize: %" CURL_FORMAT_CURL_OFF_T "\n", remoteFileSizeByte );
                }
                curl_easy_cleanup( curl_handle );

                return remoteFileSizeByte;
            }
#endif
        }

        // const char *remote = "sftp://user:pass@example.com/path/filename";
        // const char *filename = "filename";
        bool ToolKit::UploadFile( _IN const char_t * username, _IN const char_t* password, _IN const char_t* local_file, _IN const char_t* remote_file )
        {
            bool ret = false;

#ifdef CURL_STATICLIB

            //   调用curl_global_init()初始化libcurl
            CURL* curl_handle;
            CURLcode res;
            _mstring s1( username );
            _mstring s2( password );
            _mstring s3 = s1 + ":" + s2;

            FILE* fd = fopen( local_file, "rb" );
            if ( !fd )
            {
                printf( "open failed\n" );
                return ret;
            }
            printf( "------------\n" );
            fseek( fd, 0L, SEEK_END );
            printf( "------------\n" );
            size_t iFileSize = ftell( fd );
            printf( "------------\n" );
            fseek( fd, 0L, SEEK_SET );
            printf( "begin easy_init\n" );

            curl_handle = curl_easy_init();
            printf( "curl_easy_init success\n" );
            if ( curl_handle )
            {
                curl_easy_setopt( curl_handle, CURLOPT_URL, remote_file );
                curl_easy_setopt( curl_handle, CURLOPT_USERPWD, s3.c_str() );
                curl_easy_setopt( curl_handle, CURLOPT_READFUNCTION, read_func );
                curl_easy_setopt( curl_handle, CURLOPT_READDATA, fd );
                curl_easy_setopt( curl_handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );
                curl_easy_setopt( curl_handle, CURLOPT_UPLOAD, 1 );
                curl_easy_setopt( curl_handle, CURLOPT_INFILESIZE, iFileSize );

                printf( "curl_easy_setopt success" );
                res = curl_easy_perform( curl_handle );
                if ( CURLE_OK != res )
                {
                    fprintf( stdout, "curl told us %d\n", res );
                }
                else
                {
                    ret = true;
                }
            }

            curl_easy_cleanup( curl_handle );

            if( fd )
                fclose( fd );

#endif
            return ret;
        }
    }
}