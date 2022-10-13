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
//                                               Copyright    (C)    2002    Pin
//
#ifndef RWFILE_H
#define RWFILE_H

#include <stdio.h>


namespace RWFile
{
    void* _PakFOpen( const char* name, const char* access );
    int _PakFClose( void* file );
    size_t _PakFRead( void* addr, size_t size, size_t count, void* file );
    size_t _PakFWrite( const void* addr, size_t size, size_t count, void* file );
    char* _PakFGets( char* buffer, int maxLen, void* file );
    int _PakFPuts( const char* buffer, void* file );
    int _PakFEof( void* file );
    int _PakFSeek( void* file, long offset, int origin );
    int _PakFTell( void* file );
    int _PakFFlush( void* file );
};

#endif
