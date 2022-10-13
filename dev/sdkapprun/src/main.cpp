
#include <stdio.h>
#include <net/unp.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <parser/config_reader.h>
#include <gadget/raw_cells.h>
#include <gadget/sentence_pattern.h>

#include <pf-file.h>

#ifdef _LINUX
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#endif

//- lcrypto
//#pragma comment(lib, "Ws2_32.lib")
#ifdef _WINDOWS
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "crypt32.lib")
#ifdef ENV64BIT
#pragma comment(lib, "libcrypto64MTd.lib")
#pragma comment(lib, "libssl64MTd.lib")
#else
#pragma comment(lib, "libcrypto32MTd.lib")
#pragma comment(lib, "libssl32MTd.lib")
#endif
#endif

#ifdef _UNICODE
    #pragma comment(lib, "gadgetU.lib")
#else
    #pragma comment(lib, "gadget.lib")
#endif



// Read more on:
// http://www.bogotobogo.com/cplusplus/multithreading_pthread.php


#define FAIL    -1

int OpenListener( int port )
{
    int sd;
    struct sockaddr_in addr;

    sd = socket( PF_INET, SOCK_STREAM, 0 );
    bzero( &addr, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    addr.sin_addr.s_addr = INADDR_ANY;
    if ( bind( sd, ( struct sockaddr* )&addr, sizeof( addr ) ) != 0 )
    {
        perror( "can't bind port" );
        abort();
    }
    if ( listen( sd, 10 ) != 0 )
    {
        perror( "Can't configure listening port" );
        abort();
    }
    return sd;
}

SSL_CTX* InitServerCTX( void )
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = SSLv23_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new( method );   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp( stderr );
        abort();
    }
    return ctx;
}

void LoadCertificates( SSL_CTX* ctx, char* CertFile, char* KeyFile )
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file( ctx, CertFile, SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp( stderr );
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file( ctx, KeyFile, SSL_FILETYPE_PEM ) <= 0 )
    {
        ERR_print_errors_fp( stderr );
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key( ctx ) )
    {
        fprintf( stderr, "Private key does not match the public certificate\n" );
        abort();
    }
}

void ShowCerts( SSL* ssl )
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate( ssl ); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf( "Server certificates:\n" );
        line = X509_NAME_oneline( X509_get_subject_name( cert ), 0, 0 );
        printf( "Subject: %s\n", line );
        free( line );
        line = X509_NAME_oneline( X509_get_issuer_name( cert ), 0, 0 );
        printf( "Issuer: %s\n", line );
        free( line );
        X509_free( cert );
    }
    else
        printf( "No certificates.\n" );
}

void Servlet( SSL* ssl ) /* Serve the connection -- threadable */
{

    /* Serve the connection - threadable */

    char buf[ 1024 ];
    const char* reply;
    int sd, bytes, rtn;

    if ( SSL_accept( ssl ) == FAIL )
    {                   /* do SSL-protocol accept */
        ERR_print_errors_fp( stderr );
    }
    else
    {
        //crtprint( ssl );                                /* get any certificates   */
        bytes = SSL_read( ssl, buf, sizeof( buf ) );    /* get request            */
        if ( bytes > 0 )
        {
            buf[ bytes ] = 0;
            char tmp[ 1024 ];
            printf( "data: '%s' (%d bytes)", buf, bytes );
            // strcpy(reply,buf); 
            reply = "HTTP/1.1 200 OK\r\nAccept-Encoding: gzip\r\nAccess-Control-Allow-Credentials: true\r\nAccess-Control-Allow-Headers: *\r\nAccess-Control-Allow-Origin: *\r\nConnection: keep-Alive\r\nContent-Type: text/plain; charset=utf-8\r\n\r\nThis is some text in a paragraph.";

            SSL_write( ssl, reply, strlen( reply ) );            /* send reply             */
        }
        else
            ERR_print_errors_fp( stderr );
    }
    sd = SSL_get_fd( ssl );                             /* get socket connection  */
    SSL_free( ssl );                                    /* release SSL state      */
    close( sd );
}




int main( int argc, char *argv[] )
{
    {

        RawCells::field_t rf;
        rf.push_back( _T( "AA" )); rf.push_back( _T( "BB" ) ); rf.push_back( _T( "DD" ) ); rf.push_back( _T( "cc" ) );
        rf.push_back( _T( "AA" ) ); rf.push_back( _T( "BB" ) ); rf.push_back( _T( "DD" ) ); rf.push_back( _T( "cc" ) );
        RawCells rc;
        rc.InsertField( 3, rf );
        rc.InsertField( 6, rf );
        rc.InsertField( 4, rf );

        RawCells::field_t af;
        af.push_back( _T( "1AA" ) ); af.push_back( _T( "1BB" ) ); af.push_back( _T( "1DD" ) ); af.push_back( _T( "1cc" ) );
        af.push_back( _T( "1AA" ) ); af.push_back( _T( "1BB" ) ); af.push_back( _T( "1DD" ) ); af.push_back( _T( "1cc" ) );
        rc.UpdateField( 6, rf );


        std::vector<int>  aaa;
        aaa.resize( 10 );
        aaa.insert( aaa.begin() + 20, 77 );
        _tstring strs = _T( "很好CDD$(   test01 )BBBBB   $(test02   )kk   kk 77  " );
        _tcsmap  rps;
        rps[ _T( "test01" ) ] = _T( "你好" );
        rps[ _T( "test02" ) ] = _T( "good good aaa" );
        _tstring reps = sentence_pattern::ReplaceString( strs, &rps, _T( "$(" ), _T( ")" ) );

        CConfigReader cf;
        cf.OpenConfigFile( "DataRefiner.cfg" );
    }



#ifdef _WINDOWS
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 2, 2 );
    ///*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    err = WSAStartup( wVersionRequested, &wsaData );
#endif

    SSL_CTX *ctx;
    int server;
    //char *portnum;

    //if(!isRoot())
    //{
    //    printf("This program must be run as root/sudo user!!");
    //    exit(0);
    //}
    //if ( count != 2 )
    //{
    //    printf("Usage: %s <portnum>\n", strings[0]);
    //    exit(0);
    //}
    SSL_library_init();

    //portnum = strings[ 1 ];
    ctx = InitServerCTX();        /* initialize SSL */
    //LoadCertificates(ctx, "/etc/letsencrypt/live/absystem.picno.biz/fullchain.pem", "/etc/letsencrypt/live/absystem.picno.biz/privkey.pem"); /* load certs */
    LoadCertificates( ctx, "ssl/fullchain.pem", "ssl/privkey.pem" ); /* load certs */
    server = OpenListener( 4433 );    /* create server socket */

    while ( 1 )
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof( addr );
        SSL *ssl;

        int client = accept( server, ( struct sockaddr* )&addr, &len );  /* accept connection as usual */
        printf( "Connection: %s:%d\n", inet_ntoa( addr.sin_addr ), ntohs( addr.sin_port ) );
        ssl = SSL_new( ctx );              /* get new SSL state with context */
        SSL_set_fd( ssl, client );      /* set connection socket to SSL state */
        Servlet( ssl );         /* service connection */
    }
    close( server );          /* close server socket */
    SSL_CTX_free( ctx );         /* release context */
    return 1; // delete if dont need Qt
}
