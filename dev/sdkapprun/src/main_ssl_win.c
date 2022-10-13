/*****************************************************************
*SSL/TLS服務端程序WIN32版(以demos/server.cpp為基礎)
*需要用到動態連接庫libeay32.dll,ssleay.dll,
*同時在setting中加入ws2_32.lib libeay32.lib ssleay32.lib,
*以上庫文件在編譯openssl後可在out32dll目錄下找到,
*所需證書文件請參照文章自行生成.
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <iostream>
#include <winsock2.h>
#include "openssl/rsa.h"
#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
using namespace std;
#pragma comment( lib, "ws2_32.lib" )
/*所有需要的參數信息都在此處以#define的形式提供*/
#define CERTF "ssl/fullchain.pem"  /*服務端的證書(需經CA簽名)*/
#define KEYF "ssl/privkey.pem" /*服務端的私鑰(建議加密存儲)*/
#define CACERT "ca.crt" /*CA 的證書*/
#define PORT 4433 /*準備綁定的埠*/
#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }
int main()
{
    int err;
    int listen_sd;
    int sd;
    struct sockaddr_in sa_serv;
    struct sockaddr_in sa_cli;
    int client_len;
    SSL_CTX* ctx;
    SSL* ssl;
    X509* client_cert;
    char* str;
    char buf[ 4096 ];
    SSL_METHOD *meth;
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
    {
        printf( "WSAStartup()fail:%d\n", GetLastError() );
        return -1;
    }
    OpenSSL_add_ssl_algorithms(); /*初始化*/
    SSL_load_error_strings(); /*為列印調試信息作準備*/
    SSLeay_add_ssl_algorithms();
    ERR_load_BIO_strings();
    meth = const_cast<SSL_METHOD *>( SSLv23_server_method() );
    //meth = const_cast<SSL_METHOD *>(TLSv1_server_method()); /*採用什麼協議(SSLv2/SSLv3/TLSv1)在此指定*/
    ctx = SSL_CTX_new( meth );
    CHK_NULL( ctx );
    //SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); /*驗證與否*/
    // 伺服器端不需要驗證客戶端的ca，改下就好了，見 https://bbs.csdn.net/topics/390222065
    // 如果選擇SSL_VERIFY_PEER，則SSL_accept處會返回-1，一直報錯
    SSL_CTX_set_verify( ctx, SSL_VERIFY_NONE, NULL ); /*驗證與否*/
    //if ( !SSL_CTX_load_verify_locations( ctx, CACERT, NULL ) ) /*若驗證,則放置CA證書*/
    //{
   //     ERR_print_errors_fp( stderr );
    //    exit( 3 );
    //}
    if ( SSL_CTX_use_certificate_file( ctx, CERTF, SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp( stderr );
        exit( 3 );
    }
    if ( SSL_CTX_use_PrivateKey_file( ctx, KEYF, SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp( stderr );
        exit( 4 );
    }
    if ( !SSL_CTX_check_private_key( ctx ) )
    {
        printf( "Private key does not match the certificate public key\n" );
        exit( 5 );
    }
    SSL_CTX_set_cipher_list( ctx, "RC4-MD5" );
    /*開始正常的TCP socket過程.................................*/
    printf( "Begin TCP socket...\n" );
    listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
    CHK_ERR( listen_sd, "socket" );
    /********設置套接字非阻塞******/
    memset( &sa_serv, '\0', sizeof( sa_serv ) );
    sa_serv.sin_family = AF_INET;
    sa_serv.sin_addr.s_addr = INADDR_ANY;
    sa_serv.sin_port = htons( PORT );
    err = bind( listen_sd, ( struct sockaddr* ) &sa_serv,
        sizeof( sa_serv ) );
    CHK_ERR( err, "bind" );
    /*接受TCP連結*/
    err = listen( listen_sd, 5 );
    CHK_ERR( err, "listen" );
    client_len = sizeof( sa_cli );
    while ( true )
    {
        sd = accept( listen_sd, ( struct sockaddr* ) &sa_cli, &client_len );
        if ( sd == SOCKET_ERROR )
        {
            err = WSAGetLastError();
            if ( ( err != WSAEWOULDBLOCK ) )
            {
                cout << "recvfrom()調用失敗，錯誤碼：" << WSAGetLastError() << endl;
                break;
            }
            else
            {
                continue;
            }
        }
        else
            break;
    }
    cout << "連接成功！" << endl;
    closesocket( listen_sd );
    printf( "Connection from %lx, port %x\n",
        sa_cli.sin_addr.s_addr, sa_cli.sin_port );
    /*TCP連接已建立,進行服務端的SSL過程. */
    printf( "Begin server side SSL\n" );
    ssl = SSL_new( ctx );
    CHK_NULL( ssl );
    SSL_set_fd( ssl, sd );
    SSL_set_accept_state( ssl );
    bool isContinue = true;
    while ( isContinue )
    {
        isContinue = false;
        if ( SSL_accept( ssl ) != 1 )
        {
            int icode = -1;
            int iret = SSL_get_error( ssl, icode );
            if ( ( iret == SSL_ERROR_WANT_WRITE ) || ( iret == SSL_ERROR_WANT_READ ) )
            {
                isContinue = true;
            }
            else
            {
                SSL_CTX_free( ctx );
                SSL_free( ssl );
                ctx = NULL;
                ssl = NULL;
                break;
            }
        }
        else
        {
            cout << "SSL連接成功！" << endl;
            break;
        }
    }
    /*列印所有加密算法的信息(可選)*/
    printf( "SSL connection using %s\n", SSL_get_cipher( ssl ) );
    /*這一段不要採用，因為前面設置的是SSL_VERIFY_NONE，伺服器不讀取客戶端的證書。*/
    client_cert = SSL_get_peer_certificate( ssl );
    if ( client_cert != NULL )
    {
        printf( "Client certificate:\n" );
        str = X509_NAME_oneline( X509_get_subject_name( client_cert ), 0, 0 );
        CHK_NULL( str );
        printf( "\t subject: %s\n", str );
        free( str );
        str = X509_NAME_oneline( X509_get_issuer_name( client_cert ), 0, 0 );
        CHK_NULL( str );
        printf( "\t issuer: %s\n", str );
        free( str );
        X509_free( client_cert );/*如不再需要,需將證書釋放 */
    }
    else
        printf( "Client does not have certificate.\n" );
    /* 數據交換開始,用SSL_write,SSL_read代替write,read */
    int ires = 0, count = 0;;
    bool isCoutinue = true;
    while ( true )
    {
        memset( buf, '\0', sizeof( buf ) );
        count = 0;
        while ( isCoutinue )
        {
            ires = SSL_read( ssl, buf + count, 12 - count );
            int nRes = SSL_get_error( ssl, ires );
            if ( nRes == SSL_ERROR_NONE )
            {
                if ( ires > 0 )
                {
                    count += ires;
                    if ( count >= 12 )
                    {
                        cout << buf << endl;
                        break;
                    }
                    continue;
                }
            }
            else if ( nRes == SSL_ERROR_WANT_READ )
            {
                continue;
            }
            else
            {
                break;
            }
        }
        Sleep( 500 );
    }
    /* 收尾工作*/
    shutdown( sd, 2 );
    SSL_free( ssl );
    SSL_CTX_free( ctx );
    system( "pause" );
    return 0;
}
/*****************************************************************
* EOF - serv.cpp
*****************************************************************/