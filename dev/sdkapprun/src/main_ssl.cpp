#include <stdio.h>
#include <net/unp.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <pf-file.h>

//- lcrypto
#pragma comment(lib, "libssl_static.lib")
#pragma comment(lib, "libcrypto_static.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "crypt32.lib")

int create_socket( int port )
{
    SOCKET s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    addr.sin_addr.s_addr = htonl( INADDR_ANY );

    s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( s < 0 ) {
        perror( "Unable to create socket" );
        exit( EXIT_FAILURE );
    }

    if ( bind( s, ( struct sockaddr* )&addr, sizeof( addr ) ) < 0 ) {
        perror( "Unable to bind" );
        exit( EXIT_FAILURE );
    }

    if ( listen( s, 1 ) < 0 ) {
        perror( "Unable to listen" );
        exit( EXIT_FAILURE );
    }

    return s;
}

void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new( method );
    if ( !ctx ) {
        perror( "Unable to create SSL context" );
        ERR_print_errors_fp( stderr );
        exit( EXIT_FAILURE );
    }

    return ctx;
}

void configure_context( SSL_CTX *ctx )
{
    SSL_CTX_set_ecdh_auto( ctx, 1 );

    int retval;

    // Search for the presence of a file with a true result.

    /* Set the key and cert */
    if ( SSL_CTX_use_certificate_file( ctx, "./ssl/08cacert.pem", SSL_FILETYPE_PEM ) <= 0 ) {
        ERR_print_errors_fp( stderr );
        exit( EXIT_FAILURE );
    }

    if ( SSL_CTX_use_PrivateKey_file( ctx, "./ssl/08serkey.pem", SSL_FILETYPE_PEM ) <= 0 ) {
        ERR_print_errors_fp( stderr );
        exit( EXIT_FAILURE );
    }

    if( SSL_CTX_check_private_key( ctx ) <= 0 ) {
        ERR_print_errors_fp( stderr );
        exit( EXIT_FAILURE );
    }
}

int main( int argc, char **argv )
{
    //WORD wVersionRequested;
    //WSADATA wsaData;
    //int err;
    //wVersionRequested = MAKEWORD( 2, 2 );
    ///*The WSAStartup function must be the first Windows Sockets function called by an application or DLL*/
    //err = WSAStartup( wVersionRequested, &wsaData );


    int sock;
    SSL_CTX *ctx;

    init_openssl();
    ctx = create_context();

    configure_context( ctx );

    sock = create_socket( 4433 );

    /* Handle connections */
    while ( 1 )
    {
        char buff[1024];
        struct sockaddr_in addr;
        int_t len = sizeof( addr );
        SSL *ssl;
        const char reply[] = "test\n";

        printf( _T("accept\n") );
        int client = accept( sock, ( struct sockaddr* )&addr, &len );
        if ( client < 0 ) {
            perror( "Unable to accept" );
            exit( EXIT_FAILURE );
        }

        ssl = SSL_new( ctx );
        SSL_set_fd( ssl, client );

        int ret = recv( client, ( char* )buff, 1024, 0 );
        printf( buff ); printf( _T("\n") );

        int iii = SSL_accept( ssl );
        if ( iii <= 0 ) {
            ERR_print_errors_fp( stderr );
        }
        else {
            SSL_write( ssl, reply, strlen( reply ) );
        }

        SSL_free( ssl );
        closesocket( client );
    }

    closesocket( sock );
    SSL_CTX_free( ctx );
    cleanup_openssl();
}
