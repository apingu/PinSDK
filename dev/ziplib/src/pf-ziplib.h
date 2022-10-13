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
#pragma once

#include <stdio.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


extern const int _PZ_OK;
extern const int _ERR_PZ_ERRNO;
extern const int _ERR_PZ_STREAM_ERROR;
extern const int _ERR_PZ_DATA_ERROR;
extern const int _ERR_PZ_MEM_ERROR;
extern const int _ERR_PZ_BUF_ERROR;
extern const int _ERR_PZ_VERSION_ERROR;
extern const int _ERR_PZ_OPEN_COMPRESS_DEST_FILE_FAIL;
extern const int _ERR_PZ_OPEN_UN_COMPRESS_DEST_FILE_FAIL;
extern const int _ERR_NOT_PZ_FILE;


namespace pfziplib
{
    void     SetMaxCompressSize   ( _IN size32_t len );
    size32_t GetMaxCompressSize   ( void );
    
    // 未標準化
    namespace zip
    {
        size32_t GetMaxCompressedLen  ( _IN int nLenSrc );                                                                   // forecast buffer length after compress
        size32_t CompressBuffer       ( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst );
        size32_t UncompressBuffer     ( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst );

        size32_t CompressFile         ( _IN FILE* abSrc, _OUT FILE* abDst);
        size32_t UncompressFile       ( _IN FILE* abSrc, _OUT FILE* abDst );
    }
    
    // .gz
    namespace gzip
    {
        size32_t GetMaxCompressedLen  ( _IN int nLenSrc );                                                                  // forecast buffer length after compress
        size32_t CompressBuffer       ( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst );
        size32_t GetMaxUncompressedLen( _IN const ubyte_t* abSrc, _IN int nLenSrc );                                        // get need buffer length to uncompress gzip buffer
        size32_t UncompressBuffer     ( _IN const ubyte_t* abSrc, _IN int nLenSrc, _OUT ubyte_t* abDst, _IN int nLenDst );

        size32_t CompressFile         ( _IN FILE* abSrc, _OUT FILE* abDst );
        size32_t UncompressFile       ( _IN FILE* abSrc, _OUT FILE* abDst );
    }
    
    namespace pzip
    {
        size32_t Compress_FToF        ( _IN FILE* srcpath, _IN FILE* dstpath );                                            // compress file from disk to disk
        size32_t Uncompress_FToF      ( _IN FILE* srcpath, _IN FILE* dstpath );                                            // uncompress file from disk to disk

        size32_t Compress_BToF        ( _IN const ubyte_t* buf, _IN int bufflen, _IN FILE* dstpath );                      // compress file from buffer to disk
        size32_t Uncompress_FToB      ( _IN FILE* srcpath, _OUT ubyte_t** buffer );                                        // compress file from disk to buffer

        size32_t GetMaxCompressLen    ( _IN int srclen );               // forecast buffer length after compress
        size32_t Compress_BToB        ( _IN const ubyte_t* srcbuf, _IN int srclen, _OUT ubyte_t* dstbuf, _IN int dstlen ); // compress file from buffer to buffer
        
       
        size32_t GetMaxUncompressedLen( _IN const ubyte_t* abSrc, _IN int nLenSrc );                                       // get need buffer length to uncompress pzip buffer
        size32_t Uncompress_BToB      ( _IN const ubyte_t* srcbuf, _IN int srclen, _OUT ubyte_t* dstbuf, _IN int dstlen ); // compress file from buffer to buffer
    }
    
    void   FreeZipBuffer  ( _IN ubyte_t* buffer );
};