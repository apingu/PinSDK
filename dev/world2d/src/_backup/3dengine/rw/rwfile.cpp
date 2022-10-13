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
#include "RwFile.h"
#include <PFile.h>


void* RWFile::_PakFOpen( const char* name, const char* access )
{
    CPFile* pkf = new CPFile;
    if( pkf->open( name ) == NULL )
    {
        delete pkf;
        return NULL;
    }
    return pkf;
}


int RWFile::_PakFClose( void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    delete pkf;
    return 0;
}


size_t RWFile::_PakFRead( void* addr, size_t size, size_t count, void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->read( addr, size, count );
}

size_t RWFile::_PakFWrite( const void* addr,
                           size_t size,
                           size_t count,
                           void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->write( addr, size, count );
}


char* RWFile::_PakFGets( char* buffer, int maxLen, void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->readline( buffer, maxLen, '\0' );
}

int RWFile::_PakFPuts( const char* buffer, void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->writeline( buffer, strlen( buffer ) );
}


int RWFile::_PakFEof( void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->eof();
}


int RWFile::_PakFSeek( void* file, long offset, int origin )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->seek( offset, origin );
}

int RWFile::_PakFTell( void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->tell();
}

int RWFile::_PakFFlush( void* file )
{
    CPFile* pkf = ( CPFile* ) file;
    return pkf->flush();
}

