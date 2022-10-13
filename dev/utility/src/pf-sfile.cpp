/*
 pf File module

 This module presents a multi-platform interface for file I/O. The interface is
 ANSI compliant with some extensions for asynchronous operations.

 The PS2 implementation serves files from the device passed at initialization, that
 is, the Host PC, or the CD/DVD-ROM drive.

 The initialization function registers the file functions with Renderware. For PS2
 this is a sturdier implementation than skyfs, and runs at approximately the same
 speed. On PC, there is a speed increase of 50% over the default file functions.

 (c) 2002 Zona Inc. All rights reserved.
 Support available from http://www.zona.net and mailto:support@zona.net
 Confidential and Proprietary. Unauthorized use of this software is prohibited.
*/

// PS2 APIs
#if defined(_PS2)
#include <sifdev.h>
#include <libcdvd.h>
#endif

// Standard C APIs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#if defined(_WINDOWS) || defined(_XBOX)
#include <io.h>
#include <Windows.h>
#include <Shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shellapi.h>
#include <direct.h>
#endif
#if defined(_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <wchar.h>
#endif


// Module API
#include "pf-string.h"
#include "pf-stdlib.h"
#include "pf-sfile.h"

#if defined(_IPHONE)
#include "fFile_MacOS.h"
#endif



/******************************************************************************
 Data and Declarations
******************************************************************************/

// Configuration
#define MAXHANDLES  (32)                // Maximum number of files that can be open simultaneously
#define CACHESIZE   (8192)              // Size in bytes of read-ahead cache for each open file


// File stream structure
typedef struct
{
    void* stream;                      // Native file stream
    int   pointer;                     // Logical file pointer
    //int   size;                      // Size of file in bytes
    int   eof;                         // Non-zero if EOF reached during read operation
    int   cachestart;                  // Offset into file of start of cache
    int   cacheend;                    // Offset into file of end of cache
    int   cacheseek;                   // Flag to indicate a seek is necessary
    char  cache[ CACHESIZE ];            // Cached data
} _pfstream;


// File variables
static struct
{
    bool       initialize;             // had been initial?
    int        device;                 // Stores default PS2 device, 0 = Host, 1 = CD/DVD
    _pfstream  handles[ MAXHANDLES ];    // Memory for file handles
    _pfstream* handlepool;             // Pointer to next free stream
    char       directory[ _MAX_LPATH ];  // Current directory
} _pffile_variables;

//typedef _pfstream      PfsFILE;

/******************************************************************************
 Initialize file system
******************************************************************************/
void PfsInit( void )
{
    if( _pffile_variables.initialize == true )
    {
        return;
    }

    _pffile_variables.device = 0;
    // Clear the current directory
    _pffile_variables.directory[ 0 ] = 0;

    // Create a null-terminated single-linked list of file stream objects
    _pffile_variables.handlepool = _pffile_variables.handles;
    for( int i = 0; i < MAXHANDLES - 1; i++ )
    {
        *( void** )&_pffile_variables.handlepool[ i ] = &_pffile_variables.handlepool[ i + 1 ];
        *( void** )&_pffile_variables.handlepool[ i + 1 ] = 0;
    }

    _pffile_variables.initialize = true;
}


/******************************************************************************
 set open device
******************************************************************************/
void PfsSetDevice
(
    int device // Device to use for file access, 0 = Host, 1 = CD-DVD (PS2 Only)
)
{
    // Record the PS2 device
    _pffile_variables.device = device;
}



/******************************************************************************
 Open a file

 Be careful to always use the binary specifier when opening binary files. This is
 required by Windows which would otherwise treat an opened file as text. On the PS2
 all files are treated as binary and the text option is ignored. If the binary
 specifier is accidentally omitted, file reads may well work as expected on the PS2,
 but not work as expected under Windows.

******************************************************************************/
PfsFILE* Pfsopen  // returns +ve non-zero value if successful, or 0 if error
(
    const char* name,   // ASCII name of file
    const char* access  // ASCII access codes, r  = open for read only
    //      w  = open for write only and truncate
    //      a  = open for write only and append
    //      r+ = open for read and write, no truncate
    //      w+ = open for read and write, and truncate
    //      a+ = open for read and write, and append
    // the following additional specifiers may also be used,
    //      b = binary mode
    //      t = text mode (ignored on PS2, all files are binary)
    //      n = non-blocking operation
    //      d = no write-back of data cache
)
{
    // Validate parameters
    assert( name );
    assert( access );
    assert( *name != 0 );
    assert( *access != 0 );

    PfsInit();

#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)

    //-----------------------------------------------------
    // Build a device compliant path to the specified file
    //-----------------------------------------------------

    // Combine drive, directory and filename into a single path
    char fullpath[ _MAX_LPATH ];
    fullpath[ 0 ] = 0;
#if defined(_XBOX)
    strcpy( fullpath, "D:\\" );
#endif
    strcat( fullpath, _pffile_variables.directory );
    strcat( fullpath, name );
    assert( strlen( fullpath ) < sizeof( fullpath ) );

#if defined(_WINDOWS) || defined(_XBOX)
    // Apply formatting rules to path
    // only in windows system gunc accept '/' like  /aaaa.text
    char* p = fullpath;
    while( *p ) { *p = ( *p == '/' ) ? '\\' : *p; p++; }
#endif

#if defined(_IPHONE)
    iPhone_GetFullpath( fullpath, ( ( const char* )fullpath ) );
    //iPhone_GetFullpath((char*)fullpath, (const char*)fullpath);
    //NSString* oldpath=[[NSString alloc]initWithCString: fullpath];
    //const char* mypath = [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:oldpath]UTF8String];
    //strcpy(fullpath, mypath);
    //[oldpath release];
#endif
    //------------------------------------------------------
    // Open the file and fill out a stream structure for it
    //------------------------------------------------------

    // Open the file, exit if error
    FILE* stream = fopen( fullpath, access );
    if( !stream ) { return NULL; }

    // Allocate a file stream object from the pool
    _pfstream* h = _pffile_variables.handlepool;
    if( h )   { _pffile_variables.handlepool = ( _pfstream* ) * ( ( void** )_pffile_variables.handlepool ); }

    // Verify that we got a stream from the pool, exit elegantly if we didn't
    assert( h );
    if( !h ) { fclose( stream ); return 0; }

    // Store the native file stream
    h->stream = ( void* )stream;

    // EOF flag is set when an attempt is made to read past end of file
    h->eof = 0;

    // Move the file pointer to the end of the file to determine file size
    //fseek (stream, 0, SEEK_END);
    //h->size = ftell (stream);

    // If we're appending leave the file pointer at the end, otherwise reset it to the start
    if( strrchr( access, 'a' ) ) { h->pointer = Pfsfilelength( h ); }
    else        { h->pointer = 0; fseek( stream, 0, SEEK_SET ); }
#endif

#if defined(_PS2)
    //-----------------------------------------------------
    // Build a device compliant path to the specified file
    //-----------------------------------------------------

    // Combine device name, directory, filename, and version number into a single path
    char fullpath[ _MAX_LDIR ];
    if( _pffile_variables.device == 0 ) { strcpy( fullpath, "host0:" ); }
    else       { strcpy( fullpath, "cdrom0:\\" ); }
    strcat( fullpath, _pffile_variables.directory );
    strcat( fullpath, name );
    if( _pffile_variables.device != 0 ) { strcat( fullpath, ";1" ); }
    assert( strlen( fullpath ) < sizeof( fullpath ) );

    // Apply formatting rules to path
    char* p = fullpath;
    while( *p != ':' ) { p++; } // Skip device name
    if( _pffile_variables.device == 0 ) while( *p ) { *p = ( *p == '\\' ) ? '/' : *p; p++; }
    else      while( *p ) { *p = ( *p == '/' ) ? '\\' : ( ( *p >= 'a' ) && ( *p <= 'z' ) ) ? *p - 0x20 : *p; p++; }

    //--------------------------------------------------------
    // Translate the ANSI access code into a Sony format flag
    //--------------------------------------------------------

    int flags = 0;
    for( ; *access != 0; access++ )
    {
        switch( *access )
        {
        case 'r':
            flags |= SCE_RDONLY;
            break;
        case 'w':
            flags |= SCE_WRONLY + SCE_CREAT + SCE_TRUNC;
            break;
        case 'a':
            flags |= SCE_WRONLY + SCE_CREAT + SCE_APPEND;
            break;
        case '+':
            flags |= SCE_RDWR;
            break;
            //  case 'n': flags |= SCE_NOWAIT;        break; // untested
            //  case 'd': flags |= SCE_NOWBDC;        break; // untested
        case 'b':
            break;  // Ignore 'binary' flag, we always transfer binary data on PS2
        case 't':
            break;  // Ignore 'text'   flag, we always transfer binary data on PS2
        default:
            assert( 1 ); // Stop if access character is unrecognized
        }
    }

    //------------------------------------------------------
    // Open the file and fill out a stream structure for it
    //------------------------------------------------------

    // Open the file, exit if error
    int stream = sceOpen( fullpath, flags, 0 );
    if( stream < 0 ) { return 0; }

    // Allocate a file stream from the pool
    _pfstream* h = _pffile_variables.handlepool;
    if( h != 0 )  { _pffile_variables.handlepool = ( _pfstream* ) * ( ( void** )_pffile_variables.handlepool ); }

    // Verify that we got a stream from the pool, exit elegantly if we didn't
    assert( h );
    if( !h ) { sceClose( stream ); return 0; }

    // Store the native file stream
    h->stream = ( void* )stream;

    // EOF flag is set when an attempt is made to read past end of file
    h->eof = 0;

    // Move the file pointer to the end of the file to determine file size
    //h->size = sceLseek (stream, 0, SCE_SEEK_END);

    // If we're appending leave the file pointer at the end, otherwise reset it to the start
    if( flags  & SCE_APPEND ) { h->pointer = Pffilelength( h ); }
    else     { h->pointer = 0; sceLseek( stream, 0, SCE_SEEK_SET ); }
#endif

    // Initialize cache
    h->cachestart = 0;
    h->cacheend = 0;
    h->cacheseek = 0;

    // Return pointer to stream structure
    return h;
}

/******************************************************************************
 Close a file
******************************************************************************/
int Pfsclose  // returns 0 if successful, -ve if fail
(
    PfsFILE* stream  // stream of file to close
)
{
    // Validate parameters
    assert( stream != NULL );

    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Close the file
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    int result = fclose( ( FILE* )h->stream );
#endif
#if defined(_PS2)
    int result = sceClose( ( int )h->stream );
#endif

    // Return the file stream object to the pool
    *( void** )h = ( void** )_pffile_variables.handlepool;
    _pffile_variables.handlepool = h;

    // Return result of the close operation
    return result;
}

/******************************************************************************
 Read data from a file
******************************************************************************/
unsigned int Pfsread // returns number of full items read, or 0 if error
(
    void*  buffer,       // ptr to buffer to receive data
    unsigned int size,   // size of items to read
    unsigned int count,  // number of items to read (total bytes = size * count)
    PfsFILE*  stream     // stream of file to read from
)
{
    // Validate parameters
    assert( buffer != NULL );
    assert( stream != NULL );
    assert( ( size * count ) > 0 );


    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    //return fread (buffer, size, count, (FILE*) h->stream);

    // Calculate number of bytes to read
    int totalbytes = size * count;
    //((char*)buffer)[totalbytes] = '\0';

    // If any of the requested data is in the cache then copy it from there
    if( ( h->pointer >= h->cachestart ) && ( h->pointer < h->cacheend ) )
    {
        // Calculate how many bytes of the data are in the cache
        int numbytes = totalbytes;
        if( numbytes > h->cacheend - h->pointer ) { numbytes = h->cacheend - h->pointer; }

        // Copy the cached data to the destination
        memcpy( buffer, &h->cache[ h->pointer - h->cachestart ], numbytes );

        // Move logical file pointer and update read parameters
        h->cacheseek = 1;
        h->pointer += numbytes;
        totalbytes -= numbytes;
        buffer = ( void* )( ( char* )buffer + numbytes );
    }

    // If remaining data is larger than the cache then bypass the cache and load directly to destination
    if( totalbytes > CACHESIZE )
    {
        // Seek to data if necessary
        if( h->cacheseek )
        {
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
            int result = fseek( ( FILE* )h->stream, h->pointer, SEEK_SET );
            assert( result == 0 );
            if( result != 0 ) { return 0; }
#endif
#if defined(_PS2)
            int result = sceLseek( ( int )h->stream, h->pointer, SCE_SEEK_SET );
            assert( result == h->pointer );
            if( result != h->pointer ) { return 0; }
#endif
            h->cacheseek = 0;
        }

        // Read data in multiples of cache size
        int numbytes = ( totalbytes / CACHESIZE ) * CACHESIZE;

        // Read data
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
        size_t bytesread = fread( buffer, 1, numbytes, ( FILE* )h->stream );
#endif
#if defined(_PS2)
        size_t bytesread = sceRead( ( int )h->stream, buffer, numbytes );
        if( bytesread < 0 ) { return 0; }
#endif

        // Move logical file pointer and update read parameters
        h->pointer += bytesread;
        totalbytes -= bytesread;
        buffer = ( void* )( ( char* )buffer + bytesread );
    }

    // Read remaining data into cache and copy from there
    if( totalbytes )
    {
        // Seek to data if necessary
        if( h->cacheseek )
        {
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
            int result = fseek( ( FILE* )h->stream, h->pointer, SEEK_SET );
            assert( result == 0 );
            if( result != 0 ) { return 0; }
#endif
#if defined(_PS2)
            int result = sceLseek( ( int )h->stream, h->pointer, SCE_SEEK_SET );
            assert( result == h->pointer );
            if( result != h->pointer ) { return 0; }
#endif
            h->cacheseek = 0;
        }

        // Fill the cache
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
        h->cachestart = ftell( ( FILE* )h->stream );
        size_t bytesread = fread( h->cache, 1, CACHESIZE, ( FILE* )h->stream );
#endif
#if defined(_PS2)
        size_t bytesread = sceRead( ( int )h->stream, h->cache, CACHESIZE );
        if( bytesread < 0 ) { return 0; }
#endif

        // Store location of cached data within file
        h->cachestart = h->pointer;
        h->cacheend = h->cachestart + bytesread;

        // Calculate how many bytes of the requested data are in the cache
        // (in case cache did not fill all the way)
        int numbytes = totalbytes;
        if( numbytes > h->cacheend - h->pointer ) { numbytes = h->cacheend - h->pointer; }

        // Copy data from cache to destination
        memcpy( buffer, &h->cache[ 0 ], numbytes );

        // Move logical file pointer and update read parameters
        h->pointer += numbytes;
        totalbytes -= numbytes;
    }

    // Calculate total number of bytes read
    int bytesread = ( size * count ) - totalbytes;

    // If an attempt was made to read past end of file then set EOF flag
    if( totalbytes ) { h->eof = 1; }

    // Return the number of full items read
    return bytesread / size;
}

/******************************************************************************
 Write data to a file
******************************************************************************/
unsigned int Pfswrite // returns number of full items written, or 0 if error
(
    const void*  buffer, // ptr to buffer holding data
    unsigned int size,   // size of items to write
    unsigned int count,  // number of items to write (total bytes = size * count)
    PfsFILE*  stream     // stream of file to write to
)
{
    // Validate parameters
    assert( buffer != NULL );
    assert( stream != NULL );
    assert( ( size * count ) > 0 );

    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Calculate number of bytes to write
    int numbytes = size * count;

    // Write data to file, exit if error
#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    size_t byteswritten = fwrite( buffer, 1, numbytes, ( FILE* )h->stream );
    if( byteswritten == 0 ) { return 0; }
#endif
#if defined(_PS2)
    size_t byteswritten = sceWrite( ( int )h->stream, buffer, numbytes );
    if( byteswritten < 0 ) { return 0; }
#endif

    // Move logical file pointer and extend file size if necessary
    h->pointer += byteswritten;
    long32_t hsize = Pfsfilelength( h );
    if( h->pointer > hsize ) { hsize = h->pointer; }

    // Return the number of full items written
    return byteswritten / size;
}


/******************************************************************************
 Read ascii char from a file
******************************************************************************/
int Pfsgetc( PfsFILE* stream )
{
    assert( stream != NULL );

    //#ifdef _PS2
    int c = 0;
    // Read maxlength-1 bytes from the file
    int bytes = Pfsread( &c, 1, 1, stream );
    if( !bytes ) { return EOF; }
    return c;
    //#else
    // return fgetc((FILE*)stream);
    //#endif

}

/******************************************************************************
 write ascii char from a file
******************************************************************************/
int Pfsputc( int c, PfsFILE* stream )
{
    // Validate parameters
    assert( stream != NULL );
    //#ifdef _PS2
    // Calculate length of string, exit if nothing to write
    // Write string, not including terminator, to file
    int byteswritten = Pfswrite( &c, 1, 1, stream );

    // Return -1 if error, or 0 if no error
    if( byteswritten == 0 ) { return EOF; }
    else       { return c; }
    //#else
    //return fputc(c, (FILE*)stream);
    //#endif

}

/******************************************************************************
 Read ascii line from a file
******************************************************************************/
char* Pfsgets  // returns buffer pointer if successful, or 0 if error or EOF reached
(
    char* buffer,  // ptr to buffer to receive data
    int   maxlength, // length of buffer
    PfsFILE* stream  // stream of file to read from
)
{
    // Validate parameters
    assert( buffer != NULL );
    assert( stream != NULL );
    assert( maxlength > 0 );

    // Read maxlength-1 bytes from the file
    int bytes = Pfsread( buffer, 1, maxlength - 1, stream );
    if( !bytes ) { return 0; }
    // Parse data and look for the first newline character
    for( char* p = buffer; bytes != 0; p++, bytes-- )
    {
        if( *p == 0x0a )
        {
            // Terminate string and set file pointer to next character in file
            *( p + 1 ) = 0x00;
            Pfsseek( stream, -( bytes - 1 ), PF_SEEK_CUR );
            break;
        }
        else if( *p == 0x0d )
        {
            // Terminate string and set file pointer to next character in file
            *p = 0x0a;
            *( p + 1 ) = 0x00;
            Pfsseek( stream, -( bytes - 2 ), PF_SEEK_CUR );
            break;
        }
    }

    // Return pointer to buffer to indicate success
    return buffer;
}

/******************************************************************************
 Write ascii line to a file
******************************************************************************/
int Pfsputs       // returns 0 if successful, or -ve if error
(
    const char* buffer,   // ptr to buffer holding data to write
    PfsFILE*    stream // stream of file to write to
)
{
    // Validate parameters
    assert( buffer != NULL );
    assert( stream != NULL );

    // Calculate length of string, exit if nothing to write
    size_t length = strlen( buffer );
    if( !length ) 
    {
        return 0; 
    }

    // Write string, not including terminator, to file
    int byteswritten = Pfswrite( buffer, 1, length, stream );

    // Return -1 if error, or 0 if no error
    if( byteswritten == 0 ) 
    {
        return -1; 
    }
    else       
    {
        return  0; 
    }
}

/******************************************************************************
 Determine if file pointer is at end of file
******************************************************************************/
int Pfseof   // returns NZ if end of file, or 0 if not end of file
(
    PfsFILE* stream  // stream of file to query
)
{
    // Validate parameters
    assert( stream != NULL );

    //if(Pftell(stream) == Pffilelength(stream))
    // return 1;
    //return 0;

    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Return EOF flag (set is an attempt is made to read past end of file)
    return h->eof;
}

/******************************************************************************
 Move file pointer within a file
******************************************************************************/
int Pfsseek  // returns 0 if successful, NZ if error
(
    PfsFILE* stream, // stream of file to seek within
    long32_t offset, // offset for file pointer
    int      origin // origin of offset (start of file, current position or end of file)
)
{
    // Validate parameters
    assert( stream != NULL );

    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    long32_t hsize = Pfsfilelength( h );
    // Move the logical file pointer
    switch( origin )
    {
    case PF_SEEK_SET:
        h->pointer = offset;
        break;
    case PF_SEEK_CUR:
        h->pointer += offset;
        break;
    case PF_SEEK_END:
        h->pointer = hsize - offset;
        break;
    default:
        assert( 1 );
        break;
    }

    // Validate new file pointer
    assert( h->pointer >= 0 );
    if( h->pointer < 0 )  { h->pointer = 0; return -1; }
    assert( h->pointer <= hsize );
    if( h->pointer > hsize ) { h->pointer = hsize; return -1; }

    // If the file pointer moved outside of the current cache, then indicate
    // that a physical seek is necessary at the next read
    if( ( h->pointer < h->cachestart ) || ( h->pointer > h->cacheend ) ) { h->cacheseek = 1; }

    // A seek clears the EOF file
    if( h->pointer == hsize ) { h->eof = 1; }
    else     { h->eof = 0; }


#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    fseek( ( FILE* )h->stream, offset, origin );
#endif
#ifdef _PS2
    sceLseek( stream->stream, offset, origin );
#endif

    // Indicate a successful seek
    return 0;
}

/******************************************************************************
 Flush written data to device
******************************************************************************/
int Pfsflush  // returns 0 if successful, -ve if error
(
    PfsFILE* stream  // stream of file to flush
)
{
    // Validate parameters
    assert( stream != NULL );

#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Call native function
    return fflush( ( FILE* )h->stream );
#endif

#if defined(_PS2)
    return 0;   // No flush is necessary on PS2 so always return success
#endif
}

/******************************************************************************
 Get current file pointer
******************************************************************************/
int Pfstell   // returns current file pointer relative to start of file
(
    PfsFILE* stream  // stream of file to query
)
{
    // Validate parameters
    assert( stream != NULL );

    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Return the logical file pointer
    return h->pointer;
}



/******************************************************************************
 Determine if asynchronous file operation is still in progress
******************************************************************************/
int PfsFileIsBusy // returns +ve if busy, 0 if not busy, -ve if error
(
    PfsFILE* stream // stream of file to query
)
{
    // Validate parameters
    assert( stream != NULL );

#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    return 0;
#endif

#if defined(_PS2)
    // Access the file stream object
    _pfstream* h = ( _pfstream* )stream;

    // Check status of file
    int status;
    int result = sceIoctl( ( int )h->stream, SCE_FS_EXECUTING, &status );

    // If error during query then exit with error condition
    if( result < 0 ) { return -1; }

    // Otherwise return the status of file operations (1 = busy, 0 = not busy)
    return status;
#endif
}


/******************************************************************************
 Get size of file
******************************************************************************/
int Pfsfilelength   // returns size of file
(
    PfsFILE* stream  // stream of file to query
)
{
    // Validate parameters
    assert( stream != NULL );

    long32_t oldfpos = Pfstell( stream );
    long32_t flen = 0;
#if defined(_WINDOWS) || defined(_XBOX)
    flen = _filelength( fileno( ( FILE* )( ( _pfstream* )( stream ) )->stream ) );
#endif
#if defined(_LINUX)
    if( fseek( ( FILE* )( ( ( _pfstream* )( stream ) )->stream ), 0, SEEK_END ) == 0 )
    {
        flen = ftell( ( FILE* )( ( ( _pfstream* )( stream ) )->stream ) );
        fseek( ( FILE* )( ( ( _pfstream* )( stream ) )->stream ), oldfpos, SEEK_SET );
    }
#endif
#ifdef _PS2
    flen = sceLseek( stream->stream, 0, SCE_SEEK_END );
    sceLseek( stream->stream, oldpos, SCE_SEEK_SET );
#endif
    return flen;
}


/******************************************************************************
 Set current directory
******************************************************************************/
void PfsSetDirectory
(
    const char* directory
)
{
    // If a null directory is specified then clear the directory and exit
    if( ( !directory ) || ( *directory == 0 ) ) { _pffile_variables.directory[ 0 ] = 0; return; }

    // Ensure directory name will not overflow our buffer
    assert( strlen( directory ) < sizeof( _pffile_variables.directory ) - 1 );

    // Make a copy of the directory
    strcpy( _pffile_variables.directory, directory );

    // Append a '\' if none is present
    char* end = &_pffile_variables.directory[ strlen( _pffile_variables.directory ) - 1 ];
    if( ( *end != '\\' ) && ( *end != '/' ) ) { *++end = '/'; *++end = 0; }
}