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
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件
#include <string.h>
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
//
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件
#include <zlib.h>

//04. == 此專案內.h 文件
#include "pf-ziplib.h"


const int _PZ_OK = 1;
const int _ERR_PZ_ERRNO = Z_ERRNO;
const int _ERR_PZ_STREAM_ERROR = Z_STREAM_ERROR;
const int _ERR_PZ_DATA_ERROR = Z_DATA_ERROR;
const int _ERR_PZ_MEM_ERROR = Z_MEM_ERROR;
const int _ERR_PZ_BUF_ERROR = Z_BUF_ERROR;
const int _ERR_PZ_VERSION_ERROR = Z_VERSION_ERROR;
const int _ERR_PZ_OPEN_COMPRESS_DEST_FILE_FAIL = -7;
const int _ERR_PZ_OPEN_UN_COMPRESS_DEST_FILE_FAIL = -8;
const int _ERR_NOT_PZ_FILE = -10;



const word_t PFTITLE = 0X5050;

const int FILE_TITLE_SIZE = 4;
const int FILE_HEALE_SIZE = 8;
const int SWAPBUF_SIZE = _1MB_;

typedef   dword_t OffsetField;

//const char PFile::PFTITLE[2] = 0X5046;



namespace pfziplib
{
    namespace
    {
        unsigned int g_max_compress_size = 0;

        ///////////////////////////////////////////////////////////////////////////////////
        //
        //
        //
#define extern_len  0


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void SetMaxCompressSize( _IN size32_t len )
        {
            g_max_compress_size = len;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t GetMaxCompressSize( void )
        {
            return g_max_compress_size;
        }

#define segment_size 1460

        int ungzip( char* source, int len, char* des )
        {
            int ret, have;
            int offset = 0;
            z_stream d_stream;
            ubyte_t compr[ segment_size ] = { 0 }, uncompr[ segment_size * 4 ] = { 0 };
            memcpy( compr, (ubyte_t*)source, len );
            ulong32_t comprLen, uncomprLen;
            comprLen = len;
            uncomprLen = segment_size * 4;
            strcpy( (char*)uncompr, "garbage" );
            d_stream.zalloc = Z_NULL;
            d_stream.zfree = Z_NULL;
            d_stream.opaque = Z_NULL;
            d_stream.next_in = Z_NULL;
            d_stream.avail_in = 0;
            //ret = inflateInit2(&d_stream,47);
            ret = inflateInit2( &d_stream, MAX_WBITS + 16 );

            if ( ret != Z_OK )
            {
                printf( "inflateInit2 error:%d", ret );
                return ret;
            }

            d_stream.next_in = compr;
            d_stream.avail_in = comprLen;

            do
            {
                d_stream.next_out = uncompr;
                d_stream.avail_out = uncomprLen;
                ret = inflate( &d_stream, Z_NO_FLUSH );

                //assert(ret != Z_STREAM_ERROR);
                switch ( ret )
                {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;

                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd( &d_stream );
                    return ret;
                }

                have = uncomprLen - d_stream.avail_out;
                memcpy( des + offset, uncompr, have );
                offset += have;
            } while ( d_stream.avail_out == 0 );

            inflateEnd( &d_stream );
            memcpy( des + offset, "\0", 1 );
            return ret;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //
    namespace zip
    {
        size32_t GetMaxCompressedLen( _IN int nLenSrc )
        {
            return compressBound( nLenSrc );
            //int n16kBlocks = (nLenSrc+16383) / 16384; // round up any fraction of a block
            //return ( nLenSrc + 6 + ( n16kBlocks * 5 ) );
        }

        size32_t CompressBuffer( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst )
        {
            int level = Z_DEFAULT_COMPRESSION;

            uLong len = nLenDst;
            if ( compress2( abDst, &len, abSrc, nLenSrc, Z_DEFAULT_COMPRESSION ) == Z_OK )
            {
                return len;
            }
            return 0;

            z_stream zInfo = { 0 };
            zInfo.total_in = zInfo.avail_in = nLenSrc;
            zInfo.total_out = zInfo.avail_out = nLenDst;
            zInfo.next_in = (Bytef*)abSrc;
            zInfo.next_out = abDst;
            int nErr, nRet = -1;

            nErr = deflateInit( &zInfo, level ); // zlib function

            if ( nErr == Z_OK )
            {
                nErr = deflate( &zInfo, Z_FINISH );   // zlib function

                if ( nErr == Z_STREAM_END )
                {
                    nRet = zInfo.total_out;
                }
                else
                {
                    nRet = nErr;
                }
            }

            deflateEnd( &zInfo ); // zlib function
            return ( nRet );
        }

        /*
        size32_t GetMaxZipUncompressedLen(int nLenSrc)
        {
         int n16kBlocks = (nLenSrc+16383) / 16384; // round up any fraction of a block
         //return (nLenSrc + 6 + (n16kBlocks*5));
         return ((nLenSrc*45) + 6 + (n16kBlocks*5));
        }
        */

        size32_t UncompressBuffer( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst )
        {
            uLong dest_len = nLenDst;
            uLong src_len = nLenSrc;
            if ( uncompress2( abDst, &dest_len, abSrc, &src_len ) == Z_OK )
            {
                return dest_len;
            }
            return 0;

            z_stream zInfo = { 0 };
            zInfo.total_in = zInfo.avail_in = nLenSrc;
            zInfo.total_out = zInfo.avail_out = nLenDst;
            zInfo.next_in = (ubyte_t*)abSrc;
            zInfo.next_out = abDst;
            int nErr, nRet = -1;
            nErr = inflateInit( &zInfo );    // zlib function

            if ( nErr == Z_OK )
            {
                nErr = inflate( &zInfo, Z_FINISH ); // zlib function

                if ( nErr == Z_STREAM_END )
                {
                    nRet = zInfo.total_out;
                }
                else
                {
                    nRet = nErr;
                }
            }

            inflateEnd( &zInfo ); // zlib function
            return ( nRet ); // -1 or len of output
        }

        const long int CHUNK = { 16384 };

        size32_t CompressFile( _IN FILE* abSrc, _OUT FILE* abDst )
        {
            int ret, flush;
            unsigned have;
            z_stream strm;
            unsigned char in[ CHUNK ];
            unsigned char out[ CHUNK ];

            int level = Z_DEFAULT_COMPRESSION;

            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            ret = deflateInit( &strm, level );
            if ( ret != Z_OK )
                return ret;

            do
            {
                strm.avail_in = fread( in, 1, CHUNK, abSrc );
                if ( ferror( abSrc ) )
                {
                    (void)deflateEnd( &strm );
                    return Z_ERRNO;
                }
                flush = feof( abSrc ) ? Z_FINISH : Z_NO_FLUSH;
                strm.next_in = in;

                do
                {
                    strm.avail_out = CHUNK;
                    strm.next_out = out;
                    ret = deflate( &strm, flush );    /* anyone error value */
                    assert( ret != Z_STREAM_ERROR );
                    have = CHUNK - strm.avail_out;
                    if ( fwrite( out, 1, have, abDst ) != have || ferror( abDst ) )
                    {
                        (void)deflateEnd( &strm );
                        return Z_ERRNO;
                    }
                } while ( strm.avail_out == 0 );
                assert( strm.avail_in == 0 );

            } while ( flush != Z_FINISH );
            assert( ret == Z_STREAM_END );

            /* limpar e retornar */
            (void)deflateEnd( &strm );
            return Z_OK;
        }

        size32_t UncompressFile( _IN  FILE* abSrc, _OUT FILE* abDst )
        {
            int ret;
            unsigned have;
            z_stream strm;
            unsigned char in[ CHUNK ];
            unsigned char out[ CHUNK ];


            strm.zalloc = Z_NULL;
            strm.zfree = Z_NULL;
            strm.opaque = Z_NULL;
            strm.avail_in = 0;
            strm.next_in = Z_NULL;
            ret = inflateInit( &strm );
            if ( ret != Z_OK )
                return ret;

            do
            {
                strm.avail_in = fread( in, 1, CHUNK, abSrc );
                if ( ferror( abSrc ) )
                {
                    (void)inflateEnd( &strm );
                    return Z_ERRNO;
                }
                if ( strm.avail_in == 0 )
                    break;
                strm.next_in = in;

                do
                {
                    strm.avail_out = CHUNK;
                    strm.next_out = out;
                    ret = inflate( &strm, Z_NO_FLUSH );
                    assert( ret != Z_STREAM_ERROR );
                    switch ( ret )
                    {
                    case Z_NEED_DICT:
                        ret = Z_DATA_ERROR;
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        (void)inflateEnd( &strm );
                        return ret;
                    }
                    have = CHUNK - strm.avail_out;
                    if ( fwrite( out, 1, have, abDst ) != have || ferror( abDst ) )
                    {
                        (void)inflateEnd( &strm );
                        return Z_ERRNO;
                    }
                } while ( strm.avail_out == 0 );

            } while ( ret != Z_STREAM_END );

            (void)inflateEnd( &strm );
            return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
        }

    }

    namespace gzip
    {
        size32_t GetMaxCompressedLen( _IN int nLenSrc )
        {
            return zip::GetMaxCompressedLen( nLenSrc ) + 32;
        }

        size32_t CompressBuffer( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst )
        {
            z_stream zInfo = { 0 };
            zInfo.total_in = zInfo.avail_in = nLenSrc;
            zInfo.total_out = zInfo.avail_out = nLenDst;
            zInfo.next_in = (Bytef*)abSrc;
            zInfo.next_out = abDst;
            int nErr, nRet = -1;
            //nErr= deflateInit(&zInfo, Z_DEFAULT_COMPRESSION); // zlib function
            nErr = deflateInit2( &zInfo, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY );

            if ( nErr == Z_OK )
            {
                nErr = deflate( &zInfo, Z_FINISH );   // zlib function

                if ( nErr == Z_STREAM_END )
                {
                    nRet = zInfo.total_out;
                }
                else
                {
                    nRet = nErr;
                }
            }

            deflateEnd( &zInfo ); // zlib function
            //abDst[9] = 0;
            // mean file system on which compression
            // 0 - FAT filesystem (MS-DOS, OS/2, NT/Win32)
            // 1 - Amiga
            // 2 - VMS (or OpenVMS)
            // 3 - Unix
            // 4 - VM/CMS
            // 5 - Atari TOS
            // 6 - HPFS filesystem (OS/2, NT)
            // 7 - Macintosh
            // 8 - Z-System
            // 9 - CP/M
            // 10 - TOPS-20
            // 11 - NTFS filesystem (NT)
            // 12 - QDOS
            // 13 - Acorn RISCOS
            // 255 - unknown
            return ( nRet );
        }

        size32_t GetMaxUncompressedLen( _IN const ubyte_t* abSrc, _IN int nLenSrc )
        {
            size32_t isize = *(int*)( abSrc + nLenSrc - sizeof( ulong32_t ) );

            if ( isize <= 0 )
            {
                return 0;
            }

            if ( isize >= nLenSrc * 20 )
            {
                isize = nLenSrc * 20;
            }

            return isize;
            //int n16kBlocks = (isize+16383) / 16384; // round up any fraction of a block
            //return ((isize) + 6 + (n16kBlocks*5));
        }

        size32_t UncompressBuffer( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst )
        {
            z_stream zInfo = { 0 };
            zInfo.total_in = zInfo.avail_in = nLenSrc;
            zInfo.total_out = zInfo.avail_out = nLenDst;
            zInfo.next_in = (Bytef*)abSrc;
            zInfo.next_out = abDst;
            int nErr, nRet = -1;
            nErr = inflateInit2( &zInfo, MAX_WBITS + 16 );    // zlib function

            if ( nErr == Z_OK )
            {
                nErr = inflate( &zInfo, Z_FINISH ); // zlib function

                if ( nErr == Z_STREAM_END )
                {
                    nRet = zInfo.total_out;
                }
                else
                {
                    nRet = nErr;
                }
            }

            inflateEnd( &zInfo ); // zlib function
            return ( nRet ); // -1 or len of output
        }

        size32_t CompressFile( _IN FILE* abSrc, _OUT FILE* abDst )
        {
            int rlen = 0;

            if ( ( abSrc ) && ( abDst ) )
            {
                int szlen = filelength( abSrc );
                ubyte_t* szbuf = new ubyte_t[ szlen + 128 ];
                fread( szbuf, 1, szlen, abSrc );

                int dzlen = szlen + 128;
                ubyte_t* dzbuf = new ubyte_t[ dzlen ];

                rlen = pfziplib::gzip::CompressBuffer( szbuf, szlen, dzbuf, dzlen );

                fwrite( dzbuf, rlen, 1, abDst );

                SAFE_DELETE_ARRAY( szbuf );
                SAFE_DELETE_ARRAY( dzbuf );
            }

            return rlen;
        }

        size32_t UncompressFile( _IN FILE* abSrc, _OUT FILE* abDst )
        {
            int rlen = 0;

            if ( ( abSrc ) && ( abDst ) )
            {
                int szlen = filelength( abSrc );
                ubyte_t* szbuf = new ubyte_t[ szlen + 128 ];
                int read_size = fread( szbuf, 1, szlen, abSrc );

                int dzlen = GetMaxUncompressedLen( szbuf, read_size );
                ubyte_t* dzbuf = new ubyte_t[ dzlen ];

                rlen = pfziplib::gzip::UncompressBuffer( szbuf, szlen, dzbuf, dzlen );

                fwrite( dzbuf, rlen, 1, abDst );

                SAFE_DELETE_ARRAY( szbuf );
                SAFE_DELETE_ARRAY( dzbuf );
            }
            return rlen;
        }

    }

    namespace pzip
    {
        int pzcheckformat( const void* ppf, int base_offset );
        int pzcheckformat( const ubyte_t* srcb );
#ifdef _WINDOWS
#pragma pack ( push, 1 )
#else
        __attribute__( ( aligned( 1 ) ) )
#endif
            typedef struct
        {
            ubyte_t z_title[ 4 ];
            ulong32_t  uncompressed_size; /* uncompressed size   4 ubyte_ts */
            ulong32_t  compressed_size;   /* compressed size     4 ubyte_ts */
            ubyte_t reserve[ 12 ];
        } PZ_HEADER;
#ifdef _WINDOWS
#pragma pack ( pop )
#else
        __attribute__( ( packed ) )
#endif

            //=============================================================================
            // NAME:
            // DESC:
            //=============================================================================
            int pzcheckformat( void* ppf, int base_offset )
        {
            if ( (void*)ppf == 0 )
            {
                return -1;
            }

            //int tempoffset = base_offset;
            _pfseek( _pffile( ppf ), base_offset, SEEK_SET );
            ubyte_t title[ 4 ];
            _pfread( title, 1, sizeof( title ), _pffile( ppf ) );
            _pfseek( _pffile( ppf ), base_offset, SEEK_SET );

            if ( memcmp( title, "PZ", 2 ) == 0 )
            {
                return _PZ_OK;
            }
            else
            {
                perror( "file title not conform\n" );
                return _ERR_NOT_PZ_FILE;
            }
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int pzcheckformat( const ubyte_t* srcb )
        {
            ubyte_t title[ 4 ];
            __pfmemcpy( title, srcb, 4 );

            if ( memcmp( title, "PZ", 2 ) == 0 )
            {
                return _PZ_OK;
            }
            else
            {
                //perror("file title not conform\n");
                return _ERR_NOT_PZ_FILE;
            }
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Compress_FToF( _IN FILE* srcpath, _IN FILE* dstpath )
        {
            FILE* pz_file = srcpath;

            if ( pz_file == NULL )
            {
                //open source file failure
                return 0;
            }

            if ( pzcheckformat( pz_file, 0 ) == _PZ_OK )
            {
                return 0;// already a pz file
            }

            FILE* destf;

            if ( ( destf = dstpath ) == NULL )
            {
                perror( "open dest file fail :" );
                return 0;
            }

            ulong64_t srcsize = filelength( pz_file ); ///

            if ( srcsize == 0 )
            {
                return 0;
            }

            uLongf destsize = (uLongf)( srcsize * extern_len );
            Bytef* srcbuf = new Bytef[ srcsize ];
            Bytef* destbuf = new Bytef[ destsize ];
            fseek( pz_file, 0, SEEK_SET );
            fread( srcbuf, 1, srcsize, pz_file );
            int ret = compress( (Bytef*)destbuf, &destsize, (Bytef*)srcbuf, srcsize );

            if ( ret != Z_OK )
            {
                //char buff[256];
                //_stprintf( buff, ( "Compress_FToF compress fialure %d" ), ret );
                //DebugOutPutf( buff, ( "Zip" ) );
                Pf_ERROR( "PZip::Compress_FToF: Compress_FToF compress fialure %d", ret );
                return 0;
            }

            PZ_HEADER pzinfo;
            __pfmemcpy( pzinfo.z_title, "PZ", 2 );
            pzinfo.uncompressed_size = srcsize;
            pzinfo.compressed_size = destsize;
            fwrite( &pzinfo, sizeof( PZ_HEADER ), 1, destf );
            fwrite( destbuf, destsize, 1, destf );
            delete[] srcbuf;
            delete[] destbuf;
            //fclose( destf );
            return destsize + sizeof( PZ_HEADER );
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Uncompress_FToF( _IN FILE* srcpath, _IN FILE* dstpath )
        {
            FILE* pz_file = srcpath;// _tfopen( srcpath, _T( "wb" ) );

            if ( pz_file == NULL )
            {
                //open source file failure
                return 0;
            }

            // is source a pz file?
            if ( ( pzcheckformat( pz_file, 0 ) ) != _PZ_OK )
            {
                //char buff[512];
                //_stprintf( buff, ( "source file %s not a PZ file!\n" ), srcpath );
                Pf_ERROR( "PZip::Uncompress_FToF: source file %s not a PZ file!\n", srcpath );
                return _ERR_PZ_DATA_ERROR;
            }

            FILE* destf = dstpath;

            //if( ( destf = _tfopen( dstpath, _T( "wb" ) ) ) == NULL )
            //{
            //    //char buff[255];
            //    //_stprintf( buff, ( "open uncompress dest file %s fail :\n" ), dstpath );
            //    Pf_ERROR( "PZip::Uncompress_FToF: open uncompress dest file %s fail :\n", dstpath );
            //    return 0;
            //}

            PZ_HEADER pzinfo;
            fseek( _pffile( pz_file ), 0, SEEK_SET );
            fread( &pzinfo, 1, sizeof( pzinfo ), pz_file );
            ulong64_t srcsize = filelength( pz_file );
            uLongf    destsize = pzinfo.uncompressed_size;
            ubyte_t*  srcbuf = new ubyte_t[ srcsize ];
            ubyte_t*  destbuf = new ubyte_t[ destsize ];
            fseek( _pffile( pz_file ), 0 + sizeof( pzinfo ), SEEK_SET );
            fread( srcbuf, 1, srcsize, pz_file );
            int ret = uncompress( (Bytef*)destbuf, &destsize, (Bytef*)srcbuf, srcsize );

            if ( ret != Z_OK )
            {
                //char buff[256];
                //_stprintf( buff, ( "Uncompress_FToF uncompress fialure %d" ), ret );
                Pf_ERROR( "PZip::Uncompress_FToF: Uncompress_FToF uncompress fialure %d", ret );
                return 0;
            }

            fwrite( destbuf, 1, destsize, destf );
            delete[] srcbuf;
            delete[] destbuf;
            //fclose( destf );
            return destsize;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Compress_BToF( _IN const ubyte_t* buf, _IN int bufflen, _IN FILE* dstpath )
        {
            if ( ( pzcheckformat( buf ) ) == _PZ_OK )
            {
                return 0;// already a pz file
            }

            FILE* destf;

            if ( ( destf = dstpath ) == NULL )
            {
                perror( "open dest file fail :" );
                return 0;
            }

            uLongf  destlen = bufflen + extern_len;
            Bytef*  destbuf = new Bytef[ destlen ];
            int ret = compress( (Bytef*)destbuf, &destlen, (Bytef*)buf, bufflen );

            if ( ret != Z_OK )
            {
                //char buff[256];
                //_stprintf( buff, ( "Compress_BToF compress fialure %d" ), ret );
                //DebugOutPutf( buff, ( "Zip" ) );
                Pf_ERROR( "PZip::Compress_FToF: Compress_BToF compress fialure %d", ret );
                return 0;
            }

            PZ_HEADER pzinfo;
            __pfmemcpy( pzinfo.z_title, "PZ", 2 );
            pzinfo.uncompressed_size = bufflen;
            pzinfo.compressed_size = destlen;
            fwrite( &pzinfo, 1, sizeof( PZ_HEADER ), destf );
            fwrite( destbuf, 1, destlen, destf );
            delete[] destbuf;
            //fclose( _pffile( destf ) );
            return destlen + sizeof( PZ_HEADER );
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Uncompress_FToB( _IN FILE* srcpath, _OUT ubyte_t** buffer )
        {
            FILE* pz_file = srcpath;// _tfopen( srcpath, _T( "wb" ) );

            if ( pz_file == NULL )
            {
                //open source file failure
                return 0;
            }

            // is soucre is a pz file?
            if ( ( pzcheckformat( pz_file, 0 ) ) != _PZ_OK )
            {
                Pf_ERROR( "PZip::Uncompress_FToB: source file %s not a PZ file!\n", srcpath );
                return _ERR_PZ_DATA_ERROR;
            }

            PZ_HEADER pzinfo;
            fseek( pz_file, 0, SEEK_SET );
            fread( &pzinfo, 1, sizeof( PZ_HEADER ), pz_file );
            ulong32_t srcsize = _pffilelength( _pffile( pz_file ) );
            uLongf    destsize = pzinfo.uncompressed_size;
            ubyte_t* srcbuf = new ubyte_t[ srcsize ];
            ( *buffer ) = new ubyte_t[ destsize ];
            fseek( pz_file, 0 + sizeof( pzinfo ), SEEK_SET );
            fread( srcbuf, 1, srcsize, pz_file );
            int ret = uncompress( (Bytef*)( *buffer ), &destsize, (Bytef*)srcbuf, srcsize );

            if ( ret != Z_OK )
            {
                //char buff[256];
                //_stprintf( buff, ( "Uncompress_FToB uncompress fialure %d" ), ret );
                Pf_ERROR( "PZip::Uncompress_FToB: Uncompress_FToB uncompress fialure %d", ret );
                return 0;
            }

            delete[] srcbuf;
            return destsize;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t GetMaxCompressLen( _IN int srclen )
        {
            return pfziplib::zip::GetMaxCompressedLen( srclen ) + sizeof( PZ_HEADER );
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Compress_BToB( _IN const ubyte_t* srcbuf, _IN int srclen, _OUT ubyte_t* dstbuf, _IN int dstlen )
        {
            if ( ( pzcheckformat( srcbuf ) ) == _PZ_OK )
            {
                return 0;// already a pz file
            }

            if ( g_max_compress_size != 0 )
            {
                if ( srclen > g_max_compress_size )
                {
                    return 0;
                }
            }

            if ( dstlen < GetMaxCompressLen( srclen ) )
            {
                return _ERR_PZ_BUF_ERROR;
            }

            // 分配 跟他一樣大就好了
            // 因為有時候 可能會 有-5 也就是 壓縮後比壓縮前大 結果 buffer不足產生錯誤
            // 但如果 壓縮後比壓縮前大 那還不如不要壓
            uLongf   compresslen = srclen + extern_len;
            Bytef*   compressbuf = dstbuf + sizeof( PZ_HEADER ); //new ubyte_t[destsize];
            int ret = compress( (Bytef*)compressbuf, &compresslen, (Bytef*)srcbuf, srclen );

            if ( ret != Z_OK )
            {
                //delete [] destbuf;
                //char buff[256];
                //_stprintf( buff, ( "Compress_BToB compress fialure %d" ), ret );
                //DebugOutPutf( buff, ( "Zip" ) );
                Pf_ERROR( "Zip::Compress_BToB: Compress_BToB compress fialure %d", ret );
                return ret;
            }

            //*buffer = new ubyte_t[sizeof(PZ_HEADER) + destsize];
            PZ_HEADER pzinfo;
            __pfmemcpy( pzinfo.z_title, "PZ", 2 );
            pzinfo.uncompressed_size = srclen;
            pzinfo.compressed_size = compresslen;
            __pfmemcpy( dstbuf, &pzinfo, sizeof( PZ_HEADER ) );
            //__pfmemcpy(*buffer, &pzinfo, sizeof(PZ_HEADER));
            //__pfmemcpy((*buffer) + sizeof(PZ_HEADER), destbuf, destsize);
            //delete [] destbuf;
            return pzinfo.compressed_size;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t GetMaxUncompressedLen( _IN const ubyte_t* abSrc, _IN int nLenSrc )
        {
            if ( ( pzcheckformat( abSrc ) ) != _PZ_OK )
            {
                perror( "PZip::GetMaxUncompressedLen: Not a PZ buffer!" );
                return _ERR_PZ_DATA_ERROR;
            }

            if ( nLenSrc <= sizeof( PZ_HEADER ) )
            {
                return _ERR_PZ_DATA_ERROR;
            }

            PZ_HEADER* pzinfo = (PZ_HEADER*)abSrc;
            return pzinfo->uncompressed_size;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        size32_t Uncompress_BToB( _IN const ubyte_t* srcbuf, _IN int srclen, _OUT ubyte_t* dstbuf, _IN int dstlen )
            //(const ubyte_t* srcbuf, ubyte_t** buffer)
        {
            if ( ( pzcheckformat( srcbuf ) ) != _PZ_OK )
            {
                perror( "PZip::Uncompress_BToB: Not a PZ buffer!" );
                return _ERR_PZ_DATA_ERROR;
            }

            PZ_HEADER* pzinfo = (PZ_HEADER*)srcbuf;
            //__pfmemcpy(&pzinfo, srcbuf, sizeof(pzinfo));
            uLongf uncompresslen = pzinfo->uncompressed_size;

            if ( g_max_compress_size != 0 )
            {
                if ( uncompresslen > g_max_compress_size )
                {
                    return 0;
                }
            }

            if ( dstlen < uncompresslen )
            {
                return _ERR_PZ_BUF_ERROR;
            }

            //(*buffer) = new ubyte_t[buflen];
            int ret = uncompress( (Bytef*)dstbuf, &uncompresslen, (Bytef*)( srcbuf + sizeof( PZ_HEADER ) ), (uLongf)( pzinfo->compressed_size ) );

            if ( ret != Z_OK )
            {
                //char buff[256];
                //_stprintf( buff, ( "Uncompress_BToB uncompress fialure %d" ), ret );
                Pf_ERROR( "PZip::Uncompress_BToB: Uncompress_BToB uncompress fialure %d", ret );
                return ret;
            }

            return uncompresslen;
        }
    }

    void FreeZipBuffer( _IN ubyte_t* buffer )
    {
        if ( buffer != NULL )
        {
            SAFE_DELETE_ARRAY( buffer );
        }
    }

};
