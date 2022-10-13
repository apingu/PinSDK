#include <pf-ziplib.h>
#include <pf-def.h>
#include "convert_image.h"
#include <io.h>
#include <game\HPIN.h>

int PinToPinz( const char* Pinpath, const char* Pinzpath )
{
    PINHEADER PinHeader;
    FILE* pf;
    
    if ( ( pf = fopen( Pinpath, "rb" ) ) == NULL )
    { 
        return OPEN_FILE_FAIL; 
    }
    
    fread( &PinHeader, 1, sizeof( PINHEADER ), pf );
    int bufsize = _filelength( fileno( pf ) ) - sizeof( PINHEADER );
    ubyte_t* buf = new ubyte_t[ bufsize ];
    fseek( pf, sizeof( PINHEADER ), SEEK_SET );
    fread( buf, 1, bufsize, pf );
    size32_t pzBufLen = pfziplib::pzip::GetMaxCompressLen( bufsize );
    ubyte_t* pzBuf = new ubyte_t[ pzBufLen ];
    int pzBufsize = pfziplib::pzip::Compress_BToB( buf, bufsize, pzBuf, pzBufLen );
    
    if ( pzBufsize == 0 )
    {
        fclose( pf );
        return 0;
    }
    
    PinHeader.ApCompressMode = 'Z';
    FILE* pzf;
    
    if ( ( pzf = fopen( Pinzpath, "wb" ) ) == NULL )
    { 
        return OPEN_FILE_FAIL; 
    }
    
    fwrite( &PinHeader, 1, sizeof( PINHEADER ), pzf );
    fwrite( pzBuf, 1, pzBufsize, pzf );
    SAFE_DELETE_ARRAY( buf );
    pfziplib::FreeZipBuffer( pzBuf );
    //SAFE_DELETE_ARRAY( pzBuf );
    fclose( pf );
    fclose( pzf );
    return 1;
}
