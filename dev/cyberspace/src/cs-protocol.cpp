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
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <memio.inl>
#include <pf-stdlib.h>
#include <pf-ziplib.h>
#include <net/net-socket.h>
#include <algorithmic/crypt-crc32.h>
#include <algorithmic/cryptology.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-protocol.h"


//  || CsPacketHeader || CsPacketData || CsCommandHeader || Buff....................||
//  |
//  ||---------------------  CsHeaderBuffer   -----------||
//  |
//  |                                 ||--------------CsCommandPacket---------------||
//  |
//  |----------------------------------CsCommandMessage------------------------------|



#if  defined(_LINUX)
#include <stdarg.h>
#endif


#ifdef _WINDOWS
#pragma pack ( push, 1 )
#else
__attribute__( ( aligned( 1 ) ) )
#endif

//======================================
//
//  (未加密 壓縮部分)
//======================================
struct CsPacketHeader //20bytes
{
    word_t           MagicNumbers;
    long32_t         Crc;
    ubyte_t          IdentNum : 7,                //package version
                     Compress : 1;
    unsigned int     PacketLen;
    //ulonglong_t      SessionUID;                  //連線的Id值
    ubyte_t          reserver[ 1 ];
};

//======================================
// 以下壓縮加密
//======================================
struct CsPacketData //46bytes
{
    ubyte_t          header_len;                  //指向 command pk_header 的位置
    longlong_t       time_stamp;
    cs_session_uid_t session_uid;                 //連線的Id值
    longlong_t       reserver01;                  //不送 保留 送上來的機器位址
    //longlong_t   MacAddress;
    ubyte_t          key[ _CsMAX_CRYPTKEY_SIZE ]; //16
    ubyte_t          reserver[ 5 ];               //保留
};

//======================================
//  CommandHeader Information
//======================================
struct CsCommandHeader
{
    uniqueid_t   who;
    unsigned int command_len;
    channel_uid_t  command_uid;
};

#ifdef _WINDOWS
#pragma pack ( pop )
#else
__attribute__( ( packed ) )
#endif

const int _PACKET_HLEN    = _cssizeof( CsPacketHeader );
const int _PACKET_DLEN    = _cssizeof( CsPacketData );
const int _COMMAND_HLEN   = _cssizeof( CsCommandHeader );
const int _MAX_PACKET_LEN = _cssizeof( CsPacketHeader ) + _cssizeof( CsPacketData );
const int _MAX_HIDEPK_LEN = _cssizeof( CsPacketData )   + _cssizeof( CsCommandHeader );
const int _MAX_HEADER_LEN = _cssizeof( CsPacketHeader ) + _cssizeof( CsPacketData ) + _cssizeof( CsCommandHeader );

#define COMPRESS_MAX_SIZE   64
#define _EMPTYKEY           0X00000000000000000000000000000000
#define _MAX_COMMAND_BUFF  _MAX_SOCKET_BUFF  //最高處理的大小 80k? 

namespace cyberspace
{
    namespace Abtp
    {
        // check message version
        const int8_t kIdentificationNumber = 168;
        const word_t kMagicNumbersString   = 0X7343;  // "Cs"

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void  Initialize( void )
        {
            pfziplib::SetMaxCompressSize( 0 );
        }

        //=============================================================================
        // NAME:
        // DESC: key 傳入這次要加密的KEY 傳出新建立下次要加密的KEY
        //=============================================================================
        int SendPacket( CsIfSocket* sockif, cs_session_uid_t sessionid, uniqueid_t who, channel_uid_t cid, const ubyte_t* bufptr, _cs_size_t buflen, mstimen_t mt, char* key )
        {
            if( sockif == NULL )
            {
                //Beep(50, 50); Beep(50, 50);
                printf( "Send packet but socket == NULL" );
                //assert(sockif !=NULL  && "Send packet but socket == NULL");
                return -1;
            }

            // 送超過1G?
            if( buflen > _MAX_COMMAND_BUFF )
            {
                return -1;
            }

            if( ( bufptr == NULL ) || ( buflen == 0 ) )
            {
                return -1;
            }

            //unsigned int sendlen = _MAX_HEADER_LEN + buflen;
            ubyte_t*     sendbuf = new ubyte_t[ _MAX_HEADER_LEN + ( buflen * 2 ) ]; //(BYTE*)bufptr;
            CsPacketHeader* pk_header = ( CsPacketHeader* )sendbuf;
            pk_header->IdentNum = kIdentificationNumber;
            pk_header->Compress = 0;
            pk_header->Crc = 0;
            //pk_header->SessionUID      = sessionid;
            pk_header->MagicNumbers = kMagicNumbersString;
            //Databuf msgbuf[ 2 ];
            //msgbuf[ 0 ].bufptr = ( ubyte_t* ) &pk_header;
            //msgbuf[ 0 ].buflen = _cssizeof( CsPacketHeader );
            //////////////////////////////////////////////////////////////////////////
            unsigned int  hidelen = _MAX_HIDEPK_LEN + buflen;
            ubyte_t*      hidebuf = new ubyte_t[ hidelen ];//(BYTE*)bufptr;
            CsPacketData* pk_data = ( CsPacketData* )hidebuf;
            pk_data->header_len = _MAX_PACKET_LEN;
            pk_data->time_stamp = mt;

            pk_data->session_uid = sessionid;//sockif->GetMac();
                                           //pk_data->MacAddress = sockif->GetMac();

                                           //
            char newkey[ _CsMAX_CRYPTKEY_SIZE ];
            if( key != NULL )
            {
                cryptology::CreateKey( ( char* )newkey );
            }
            else
            {
                IOMEMSET( newkey, 0, _CsMAX_CRYPTKEY_SIZE );
            }
            IOMEMCPY( pk_data->key, newkey, _CsMAX_CRYPTKEY_SIZE );

            //IOMEMCPY( pk_data->key, newkey, _CsMAX_CRYPTKEY_SIZE );
            CsCommandHeader* cheader = ( CsCommandHeader* )( hidebuf + _PACKET_DLEN );
            cheader->who = who;
            cheader->command_uid = cid;
            cheader->command_len = buflen;
            IOMEMCPY( ( hidebuf + _MAX_HIDEPK_LEN ), bufptr, buflen );
            //message to long32_t do compress
            //////////////////////////////////////////////////////////////////////////
            ubyte_t* compressbuf = sendbuf + _PACKET_HLEN;

            if( ( buflen <= COMPRESS_MAX_SIZE ) )
            {
                //比BUFF小
                // 壓縮失敗或是不需要壓縮的話 就把sendbuf當成是hidebuf
                pk_header->Compress = 0;
                IOMEMCPY( compressbuf, hidebuf, hidelen );
                SAFE_DELETE( hidebuf );
            }
            else
            {
                // 自訂格式有網路傳輸時有問題
                //int compresslen = pfziplib::pzip::GetMaxCompressLen(hidelen);
                //sendbufa = new ubyte_t[compresslen];
                //unsigned int templen = pfziplib::pzip::Compress_BToB((ubyte_t*)hidebuf, hidelen, sendbufa, compresslen);
                int compresslen = pfziplib::gzip::GetMaxCompressedLen( hidelen );

                if( compresslen >= _MAX_COMMAND_BUFF )
                {
                    SAFE_DELETE_ARRAY( sendbuf );
                    SAFE_DELETE_ARRAY( hidebuf );
                    return -1;
                }

                //compressbuf = sendbuf + _PACKET_HLEN;
                unsigned int templen = pfziplib::gzip::CompressBuffer( ( const BYTE* )hidebuf, hidelen, ( BYTE* )compressbuf, compresslen );

                if( templen <= 0 )
                {
                    pk_header->Compress = 0;
                    // 壓縮失敗或是不需要壓縮的話 就把sendbuf當成是hidebuf
                    IOMEMCPY( compressbuf, hidebuf, hidelen );
                    //SAFE_DELETE( hidebuf );
                    //char buff[256];
                    //_stprintf( buff, "cyberspace::Abtp::SendPacket: Send channel %d Compress size %d failure", cid, hidelen );
                    //DebugOutPutf( buff, "cyberspace::Abtp::SendPacket" );
                    Pf_ERROR( "cyberspace::Abtp::SendPacket: Send channel %d Compress size %d failure", cid, hidelen );
                }
                else
                {
                    pk_header->Compress = 1;
                    hidelen = templen;
                    // 壓縮成功的話 hidebuf上面的資料已經 複製到 sendbuf上面了 所以不需要hidebuf了
                    //SAFE_DELETE_ARRAY( hidebuf );
                }
            }
            pk_header->PacketLen = hidelen;

            //////////////////////////////////////////////////////////////////////////
            // 加密
            if( key != NULL )
            {
                // 經過這邊後 pk_data->key 也會一起被編碼 所以 下面不能直接copy
                cryptology::EncCode( key, ( char* )compressbuf, hidelen );
            }

            IOMEMCPY( key, newkey, _CsMAX_CRYPTKEY_SIZE );
            //else
            //{
            //  //IOMEMSET(pk_header.HPacket.key, 0, _CsMAX_CRYPTKEY_SIZE);
            //}
            // 檢查碼
            pk_header->Crc =   crc32::Encode( ( const char* )compressbuf, hidelen );
            //msgbuf[ 1 ].bufptr = ( ubyte_t* ) sendbufa;
            //msgbuf[ 1 ].buflen = hidelen;
            //int ret = sockif->Sendb( msgbuf, 2 );
            int ret = sockif->Send( sendbuf, _PACKET_HLEN + hidelen );
            //pfziplib::FreeZipBuffer(sendbufa);
            SAFE_DELETE_ARRAY( sendbuf );
            SAFE_DELETE( hidebuf );

            //
            if( ret == SOCKET_ERROR )
            {
                for( int i = 0; i < 5; i++ )
                {
                    int errco = net::TCPSocket::GetLastError();
#ifdef _WINDOWS
                    switch( errco )
                    {
                    case ERROR_SUCCESS:
                    case WSAECONNABORTED:
                    case WSAECONNRESET:
                    case WSAEWOULDBLOCK:
                    case ERROR_IO_PENDING:
                        return buflen;
                    }
#else
                    switch( errco )
                    {
                    case 0:
                        return buflen;
                    }
#endif
                    //char buff[256];
                    //_stprintf( buff, "SendPacket: Send failure %lld cid:%d buflen:%d, resend %d\n", who, cid, buflen, i );
                    //DebugOutPutf( buff, "SendPacket" );
                    Pf_ERROR( "SendPacket: Send failure %llu cid:%d buflen:%d, resend %d\n", who, cid, buflen, i );
                    pfstd::MSSleep( 1 );
                    //if ( ( ret = sockif->Sendb( NULL, 0 ) ) != SOCKET_ERROR )
                    //{
                    //    return buflen;
                    //}
                }
                return SOCKET_ERROR;
            }
            return buflen;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //int cyberspace::Abtp::RecvPacket( CsIfSocket* sockif, cs_session_uid_t& sessionid, macn_t& mac, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bufptr, _cs_size_t& buflen, _cs_size_t& recvlen, mstimen_t& mt, char* key )
        int RecvPacket( CsIfSocket* sockif, cs_session_uid_t& sessionid, uniqueid_t& who, channel_uid_t& cid, ubyte_t** bufptr, _cs_size_t& buflen, _cs_size_t& recvlen, mstimen_t& mt, char* key )
        {
            //(*bufptr)=NULL;
            recvlen = 0;

            if( sockif == NULL )
            {
                //Beep(50, 50); Beep(50, 50);
                //char err[256];
                //_stprintf( err, "Recv packet but socket == NULL\n" );
                //assert(sockif !=NULL  && "Send packet but socket == NULL");
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: Recv packet but socket == NULL\n" );
                return -1;
            }

            CsPacketHeader myheader;
            memset( &myheader, 0, _cssizeof( CsPacketHeader ) );

            if( sockif->Recv( ( ubyte_t* )( &myheader ), _cssizeof( CsPacketHeader ) ) != _cssizeof( CsPacketHeader ) )
            {
                //char err[256];
                //_stprintf( err, "recv size error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv size error\n" );
                return -1;
            }

            // 封包版本不同
            if( ( ( ( int )myheader.IdentNum ) != kIdentificationNumber ) && ( myheader.MagicNumbers != kMagicNumbersString ) )
            {
                //char err[256];
                //_stprintf( err, "recv version error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv version error\n" );
                return -1;
            }

            // 接收超過1G?
            if( myheader.PacketLen > _MAX_COMMAND_BUFF )
            {
                //char err[256];
                //_stprintf( err, "recv pk_header max packet size error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv pk_header max packet size error\n" );
                return -1;
            }

            //sessionid   = myheader.session_uid;
            ubyte_t* buffer = new ubyte_t[ myheader.PacketLen ];
            if( sockif->Recv( buffer, myheader.PacketLen ) < 0 )
            {
                SAFE_DELETE_ARRAY( buffer );
                //char err[256];
                //_stprintf( err, "sockif->Recvn error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: sockif->Recvn error\n" );
                return -1;
            }

            // 檢查碼
            ulong32_t checksumcode = 0;
            checksumcode =   crc32::Encode( ( const char* )buffer, myheader.PacketLen );

            if( checksumcode != myheader.Crc )
            {
                SAFE_DELETE_ARRAY( buffer );
                //char err[256];
                //_stprintf( err, "recv crc error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv crc error\n" );
                return -1;
            }

            // 解密
            if( key != NULL )
            {
                cryptology::DecCode( key, ( char* )buffer, myheader.PacketLen );
                //IOMEMCPY(key, myheader.HPacket.key, _CsMAX_CRYPTKEY_SIZE);
            }

            ubyte_t* recvbuf = NULL;

            // 解壓縮
            if( myheader.Compress )
            {
                // 自訂格式有網路傳輸時有問題
                //int recvlen = pfziplib::pzip::GetMaxUncompressedLen((const ubyte_t*)buffer, buflen);
                //recvbuf = new ubyte_t[recvlen];
                //recvlen = pfziplib::pzip::Uncompress_BToB((const ubyte_t*)buffer, buflen, recvbuf, recvlen);      // compress file from buffer to buffer
                _cs_size_t uncomplen = pfziplib::gzip::GetMaxUncompressedLen( ( const BYTE* )buffer, myheader.PacketLen );

                if( uncomplen >= _MAX_COMMAND_BUFF )
                {
                    SAFE_DELETE_ARRAY( buffer );
                    return -1;
                }

                recvbuf = new ubyte_t[ uncomplen ];
                uncomplen = pfziplib::gzip::UncompressBuffer( ( const BYTE* )buffer, myheader.PacketLen, ( BYTE* )recvbuf, uncomplen ); // compress file from buffer to buffer
                                                                                                                                  // 接下來解壓縮後的資料都複製到recvbuf裡面 所以用不到 buffer了
                SAFE_DELETE_ARRAY( buffer );

                if( uncomplen <= 0 )
                {
                    recvlen = 0;
                    //char err[256];
                    //_stprintf( err, "recv compress error\n" );
                    Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv compress error\n" );
                    return -1;
                }
            }
            else
            {
                //如果沒有解壓縮 就直接把buffer 傳給 recvbuf
                recvbuf = buffer;
            }

            //
            CsPacketData* pkbuff = ( CsPacketData* )recvbuf;
            // 取得下一個封包的Ket
            IOMEMCPY( key, pkbuff->key, _CsMAX_CRYPTKEY_SIZE );
            sessionid = pkbuff->session_uid;
            //mac = pkbuff->MacAddress;
            // 該封包發送的時間
            mt = pkbuff->time_stamp;
            CsCommandHeader* cheader = ( CsCommandHeader* )( recvbuf + _cssizeof( CsPacketData ) );
            who = cheader->who;
            cid = cheader->command_uid;
            recvlen = cheader->command_len;

            // 不可能送超過這個長度
            if( recvlen > _MAX_COMMAND_BUFF )
            {
                //char err[256];
                //_stprintf( err, "recv command max packet size error\n" );
                Pf_ERROR( "cyberspace::Abtp::RecvPacket: recv command max packet size error\n" );
                return -1;
            }

            // 傳進來的buffer 夠用
            if( ( ( *bufptr ) != NULL ) && ( buflen >= recvlen ) )
            {
            }
            else
            {
                SAFE_DELETE_ARRAY( ( *bufptr ) );
                ( *bufptr ) = new ubyte_t[ recvlen ];
                buflen = recvlen;
            }

            IOMEMCPY( ( *bufptr ), recvbuf + _cssizeof( CsPacketData ) + _cssizeof( CsCommandHeader ), recvlen );
            pfziplib::FreeZipBuffer( ( ubyte_t* )recvbuf );
            return recvlen;
        }
    }
}


namespace CsFactory
{
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _cs_size_t MakeArgument( ubyte_t* out, const char* fmt, ... )
    {
        ubyte_t* obuf = out;
        char*    ptr = ( char* )fmt;
        va_list argvlist;
        va_start( argvlist, fmt ); /* Initialize variable arguments. */

        //while(i != 0)
        while( ( *ptr ) != 0 )
        {
            if( ( *ptr ) == '%' )
            {
                ptr++;

                switch( ( *ptr ) )
                {
                case 'v':
                    {
                        void* bin = ( void* )va_arg( argvlist, void* );
                        unsigned short binlen = ( unsigned short )va_arg( argvlist, int );
                        ( *( unsigned short* )obuf ) = binlen;
                        obuf += _cssizeof( unsigned short );

                        if( ( bin != NULL ) && ( binlen > 0 ) )
                        {
                            IOMEMCPY( obuf, bin, binlen );
                            obuf += binlen;
                        }
                    }
                    break;

                case 's':
                    {
                        char* str = ( char* )va_arg( argvlist, char* );
                        unsigned short recvlen = ( unsigned short )_csstrlen( str );
                        ( *( unsigned short* )obuf ) = recvlen;
                        obuf += _cssizeof( unsigned short );
                        strcpy( ( char* )obuf, str );
                        //\0
                        obuf += recvlen + 1;
                    }
                    break;

                case 'w':
                    {
                        wchar_t* str = ( wchar_t* )va_arg( argvlist, wchar_t* );
                        unsigned short recvlen = ( unsigned short )wcslen( str );
                        ( *( unsigned short* )obuf ) = recvlen;
                        obuf += _cssizeof( unsigned short );
                        wcscpy( ( wchar_t* )obuf, str );
                        //\0
                        obuf += ( ( recvlen + 1 ) * 2 );
                    }
                    break;

                case 'c':
                    {
                        char argv = ( char )va_arg( argvlist, int );
                        ( *( ( char* )obuf ) ) = argv;
                        obuf += _cssizeof( char );
                    }
                    break;

                case 'd':
                    {
                        ( *( ( int* )obuf ) ) = ( int )va_arg( argvlist, int );
                        obuf += _cssizeof( int );
                    }
                    break;

                case 'l':
                    {
                        ( *( ( long64_t* )obuf ) ) = ( long64_t )va_arg( argvlist, long64_t );
                        obuf += _cssizeof( long64_t );
                    }
                    break;

                case 'h':
                    {
                        short argv = ( short )va_arg( argvlist, int );
                        ( *( ( short* )obuf ) ) = argv;
                        obuf += _cssizeof( short );
                    }
                    break;

                case 'q':
                    {
                        ( *( ( int64_t* )obuf ) ) = ( int64_t )va_arg( argvlist, int64_t );
                        obuf += _cssizeof( int64_t );
                    }
                    break;

                default:
                    {
                        assert( 0 && "MakeArgument % format error" );
                    }
                    break;
                }

                //sum  +=  1;
                //count++;
            }

            ptr++;
        }

        va_end( argvlist );   /* Reset variable arguments.   */
        return ( obuf - out );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _cs_size_t SplitArgument( const ubyte_t* in, const char* fmt, ... )
    {
        ubyte_t* obuf = ( ubyte_t* )in;
        char* ptr = ( char* )fmt;
        va_list argvlist;
        va_start( argvlist, fmt ); /* Initialize variable arguments. */

        //while(i != 0)
        while( ( *ptr ) != 0 )
        {
            if( ( *ptr ) == '%' )
            {
                ptr++;

                switch( ( *ptr ) )
                {
                case 'v':
                    {
                        int binlen = ( *( unsigned short* )obuf );

                        if( binlen > 0 )
                        {
                            IOMEMCPY( ( void* )va_arg( argvlist, void* ), ( obuf + _cssizeof( unsigned short ) ), binlen );
                        }
                        else
                        {
                            IOMEMSET( ( void* )va_arg( argvlist, void* ), 0, binlen );
                        }

                        ( *va_arg( argvlist, int* ) ) = binlen;
                        obuf += ( _cssizeof( unsigned short ) + binlen );
                    }
                    break;

                case 'p':
                    {
                        unsigned short binlen = ( *( unsigned short* )obuf );

                        if( binlen > 0 )
                        {
                            ( *( va_arg( argvlist, long* ) ) ) = ( long )( obuf + _cssizeof( unsigned short ) );
                        }
                        else
                        {
                            ( *( va_arg( argvlist, long* ) ) ) = NULL;
                        }

                        ( *va_arg( argvlist, int* ) ) = binlen;
                        obuf += ( _cssizeof( unsigned short ) + binlen );
                    }
                    break;

                case 's':
                    {
                        unsigned short recvlen = ( *( unsigned short* )obuf );
                        obuf += _cssizeof( unsigned short );
                        strcpy( ( char* )va_arg( argvlist, char* ), ( char* )obuf );
                        obuf += recvlen + 1;
                        //char* argvbuf = ( char* )va_arg( argvlist, char* );
                        //IOMEMCPY( argvbuf, obuf, recvlen );
                        //argvbuf[recvlen + 1] = '\0';
                        //obuf += recvlen;
                    }
                    break;

                case 'w':
                    {
                        unsigned short recvlen = ( *( unsigned short* )obuf );
                        obuf += _cssizeof( unsigned short );
                        wcscpy( ( wchar_t* )va_arg( argvlist, wchar_t* ), ( wchar_t* )obuf );
                        obuf += ( ( recvlen + 1 ) * 2 );
                        //wchar_t* argvbuf = ( wchar_t* ) va_arg( argvlist, wchar_t* );
                        //IOMEMCPY( argvbuf, obuf, recvlen * _cssizeof(wchar_t) );
                        //argvbuf[ recvlen + 1 ] = '\0';
                        //obuf += ( recvlen * _cssizeof( wchar_t ) );
                    }
                    break;

                case 'c':
                    {
                        ( *( va_arg( argvlist, char* ) ) ) = ( *( char* )obuf );
                        obuf += _cssizeof( char );
                    }
                    break;

                case 'd':
                    {
                        ( *( va_arg( argvlist, int* ) ) ) = ( *( int* )obuf );
                        obuf += _cssizeof( int );
                    }
                    break;

                case 'l':
                    {
                        ( *( va_arg( argvlist, long64_t* ) ) ) = ( *( long64_t* )obuf );
                        obuf += _cssizeof( long64_t );
                    }
                    break;

                case 'h':
                    {
                        ( *( va_arg( argvlist, short* ) ) ) = ( *( short* )obuf );
                        obuf += _cssizeof( short );
                    }
                    break;

                case 'q':
                    {
                        ( *( va_arg( argvlist, int64_t* ) ) ) = ( *( int64_t* )obuf );
                        obuf += _cssizeof( int64_t );
                    }
                    break;

                default:
                    {
                        assert( 0 && "SplitArgument % format error" );
                    }
                    break;
                }

                //sum  +=  1;
                //count++;
            }

            ptr++;
        }

        va_end( argvlist );   /* Reset variable arguments.   */
        return ( obuf - in );
    }
}
;