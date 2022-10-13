//ppk.cpp file package and compress function class
//
//       char[4] PP
//       int     counts
//       int     offset(1)
//                  :
//                  :
//                  :
//                  :
//                  :
//       int     offset(counts)
//       int     fname(1)len
//       char    fname(1)[fnamelen]
//                  :
//                  :
//                  :
//                  :
//                  :
//       int     fname(counts)len
//       char    fname(counts)[fnamelen]
//
//
//                                               Copyright    (C)    1999    Pin
//

//01. == C 系統文件/C++ 系統文件
#include <errno.h>    // android need
#include <stdarg.h>
#include <assert.h>
#ifdef _LINUX_
#include <unistd.h>
#endif
//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "file_operator.h"


const word_t PFTITLE = 0X5050;

const int FILE_TITLE_SIZE = 4;
const int FILE_HEALE_SIZE = 16;
const int SWAPBUF_SIZE = _1MB_;

typedef   dword_t OffsetField;

//const char FileOperator::PFTITLE[2] = 0X5046;



//namespace pfget
//{
namespace
{
    int         checkformat( uPfFile ppf, unsigned int base_offset );                              //is a pf file
    int         getfilecount( uPfFile ppf, unsigned int base_offset );                             //how much file in it
    int         getfileoffset( unsigned int fileno, uPfFile ppf, unsigned int base_offset );       //file address include fname
    int         getfileaddr( unsigned int fileno, uPfFile ppf, unsigned int base_offset );         //file address
    int         getfileeof( unsigned int fileno, uPfFile ppf, unsigned int base_offset );
    int         getfilesize( unsigned int fileno, uPfFile ppf, unsigned int base_offset );         //file address
    int         getfileno( unsigned int addr, uPfFile ppf, unsigned int base_offset );             //number of file
    int         getfileno( const char* fname, uPfFile ppf, unsigned int base_offset );             //number of file
    int         getfilename( unsigned int fileno, char* fname, size32_t size, uPfFile ppf, unsigned int base_offset );
    int         pathdeep( const char* fname );
    int         makeppath( const char* fname, char* buffer, size32_t size );
    char        splitppath( const char* fname, unsigned int deep, char* buffer, size32_t size );
    int         dumpppath( const char* fname, unsigned int deep, char* buffer, size32_t size );
    char*       transppath( const char* fname, const char* dir, char* buffer, size32_t size );     //transfor to real path
    int         pfnamegetaddr( const char* fname, uPfFile ppf );
    int         pfnamegetsize( const char* fname, uPfFile ppf );

    uPfFile   gSave_Temp_f = NULL;
    //long FileAttributes(const char* fname);
    bool        Add_File( const char* srcfname, int depth, void* pData );

    /*
    size32_t    ScanFiles       ( const char*      root,
                                  OnFileM    fcallback,
                                  void*            pData,
                                  int              depth = 0 );
    size32_t    ScanFiles       ( const wchar_t*   root,
                                  OnFileW    fcallback,
                                  void*            pData,
                                  int              depth = 0 );
    */
    int         maketmpfname( str_t tmpfname, int size, const char* srcfname );
    int         fshift( uPfFile f, int shift );
};



//=============================================================================
// NAME:
// DESC:
//=============================================================================
FileOperator::FileOperator( const char* filename, const char* mode )
//: PFTITLE(PFTITLE)
{
    _pfInit();
    errno_ = _PF_OK;
    pp_file_ = NULL;
    fname_[ 0 ] = '\0';

    //m_base_offset_ = 0;
    //m_title_ext[0] = '\0';
    //ppfname_[0] = '\0';
    //ppworkdir_[0] = '\0';
    if( filename != NULL )
    {
        open( filename, mode );
    }
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
FileOperator::~FileOperator()
{
    mode_.clear();
    close();
}


///////////////////////////////////////////////////////////////////////////////
//


//=============================================================================
// NAME: Open()
// DESC:
//=============================================================================
void FileOperator::setOpenDevice( int device )
{
#ifdef _PS2
    pfsetDevice( int device );
#endif
}


//=============================================================================
// NAME: Open()
// DESC:
//=============================================================================
uPfFile FileOperator::open( const char* path, const char* mode )
{
    if( path == NULL )
    {
        return NULL;
    }

    char buf[ 256 ];
    //轉為正確路徑
    mcurrentpath( buf, path );

    if( _pffile( pp_file_ ) == 0 )
    {
        // input empty path
        if( strcmp( path, "" ) == 0 )
        {
            goto OPENFAIL;
        }

        // 給你3次的機會
        for( int i = 0; i < 3; i++ )
        {
            if( ( pp_file_ = _pfmopen( buf, mode ) ) != NULL )
            {
                i = 3;
                break;
            }
            else
            {
                if( errno == ENOENT )
                {
                    i = 3;
                    break;
                }
                else
                {
                    pfstd::MSSleep( 10 );
                }
            }
        }

        // 開檔如果-1則失敗，傳回-1
        if( pp_file_ == NULL )
        {
        OPENFAIL:
            errno_ = _ERR_OPEN_FILE_FAIL;
            return NULL;
        }

        mode_ = mode;
        // path name
        fname_ = path;
    }

    //_pfseek(_pffile(pp_file_), 0, SEEK_SET);
    return pp_file_;
}

/*
uPfFile FileOperator::open(const wchar_t* path, const wchar_t* mode)
{
    if(path == NULL)
        return NULL;

    wchar_t buf[256];

    //轉為正確路徑
    wcurrentpath(buf, path);

    if (_pffile(pp_file_) == 0)
    {
        // input empty path
        if (wcscmp(path, L"") == 0)
            goto OPENFAIL;

        // 給你3次的機會
        for(int i = 0; i<3; i++)
        {
            if ((pp_file_ = _wfopen(buf, mode)) != NULL)
            {
                i=3;
                break;
            }
            else
            {
                if(errno == ENOENT)
                {
                    i=3;
                    break;
                }
                else
                {
                    Sleep(10);
                }
            }
        }

        // 開檔如果-1則失敗，傳回-1
        if(pp_file_ == NULL)
        {
OPENFAIL :
            errno_ = _ERR_OPEN_FILE_FAIL;
            return NULL;
        }

        mode_ = mode;

        // path name
        wcscpy(fname_, path);
    }
    //_pfseek(_pffile(pp_file_), 0, SEEK_SET);

    return pp_file_;

}
*/


//=============================================================================
// NAME:
// DESC:
//=============================================================================
error_t FileOperator::close( void )
{
    int result = 0;

    if( _pffile( pp_file_ ) != NULL )
    {
        result = _pfclose( _pffile( pp_file_ ) );
    }

    pp_file_ = NULL;
    fname_[ 0 ] = '\0';
    return result;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::fno( void )
{
    return _pffileno( _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::flength( void )
{
    //open a exist file
    return _pffilelength( _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
uPfFile FileOperator::opentmp( FileOperator* tmpf, const char* mode )
{
    tmpf->close();
    char tmpfname[ _MAX_LPATH ];
    maketmpfname( tmpfname, _MAX_LPATH, fname_.c_str() );
    return tmpf->open( tmpfname, mode );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
_mstring FileOperator::fpath()
{
    return fname_;

}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void FileOperator::splitpath( int deep, char* ppfname )
{
    if( deep == 0 )
    {
        strcpy( ppfname, fname_.c_str() );
        return;
    }
    splitppath( ppfname, deep, ppfname, _MAX_LFNAME );
    return;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
char* FileOperator::_splitpath(char* path, char* ppdir, char* ppfname)
{
    ppfname[0] = '\0'
    int deep = pathdeep(fname_);
    splitppath(fname_, 0, path, _MAX_LPATH);

    for(int i= 1; i < deep; i++)
    {
        char fname[_MAX_LFNAME];
        splitppath(fname_, 0, fname, _MAX_LDIR);
        strcat(ppdir, fname);
    }
    splitppath(fname_, deep, ppfname, _MAX_LFNAME);
    return path;
}
*/
//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::fcopy_to( const char* destpath )
{
    uPfFile n_f;

    if( ( n_f = _pfmopen( destpath, "wb" ) ) == NULL )
    {
        //DebugMessage("copy file fail |||~_~ !!!!", "FileOperator");
        return -1;
    }

    int flen = flength();
    ubyte_t* fbuf = new ubyte_t[ flen ];
    read( fbuf, flen, 1 );
    _pfwrite( fbuf, flen, 1, _pffile( n_f ) );
    _pfclose( _pffile( n_f ) );
    return 1;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::fdelete( void )
{
    _mstring fp = fpath();
    close();
    return remove( fp.c_str() );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::frename( const char* newfname )
{
    _mstring p = fpath();
    //IOFilepath pp = fpath(2);
    _mstring mode = fmode();
    long int currpos = tell();
    close();
    int ret = rename( p.c_str(), newfname );
    //IOFilepath sp = newfname;
    //sp.cat(pp);
    //char* buff=new char[strlen(sp.c_str())+strlen(pp.c_str())+1];
    //sprintf(buff, "%s%s", sp.c_str(), pp.c_str());
    //sp=buff;
    //SAFE_DELETE_ARR//AY(buff);
    //open(sp.c_str(), fmode());
    // 重新打開原來的檔案 並把 位置 指向原來的位置
    open( newfname, mode.c_str() );
    seek( currpos );
    return ret;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::flush( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    return _pfflush( _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
long FileOperator::tell( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return -1;
    }

    return  _pftell( _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
//     pffpos_t FileOperator::pos(void)
//     {
//         if (_pffile(pp_file_) == 0)
//             return -1;
//         return  _pftell(_pffile(pp_file_));
//     }

//=============================================================================
// NAME: eof()
// DESC: is now end of file
//=============================================================================
int FileOperator::eof( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 1;
    }

    return _pfeof( _pffile( pp_file_ ) );
}

//=============================================================================
// NAME: readword()
// DESC: find a word and return it's position
//=============================================================================
pffpos_t FileOperator::findword( const char* word )
{
    pffpos_t oldfpos = tell();
    pffpos_t retpos = 0;

    while( eof() == 0 )
    {
        retpos = tell();
        char fw;
        read( &fw, 1, 1 );
        pffpos_t nextpos = tell();

        if( fw == word[ 0 ] )
        {
            size_t remainlen = strlen( word ) - 1;

            if( remainlen == 0 )
            {
                return retpos;
            }

            ubyte_t* buf = new ubyte_t[ remainlen ];
            read( buf, remainlen, 1 );

            if( memcmp( buf, word + 1, remainlen ) == 0 )
            {
                delete[] buf;
                return retpos;
            }
            else
            {
                delete[] buf;
                seek( nextpos, SEEK_SET );
            }
        }
    }

    seek( oldfpos, SEEK_SET );
    return -1;
}

//=============================================================================
// NAME: gotoline()
// DESC:
//=============================================================================
pffpos_t FileOperator::gotoline( unsigned int line )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    pffpos_t oldpos = tell();
    seek( 0, SEEK_SET );
    unsigned int c, ls = 0;

    while( ls != line )
    {
        if( ( c = _pfgetc( _pffile( pp_file_ ) ) ) == EOF )
        {
            seek( oldpos, SEEK_SET );
            return -1;
        }
        else if( c == '\n' )
        {
            /*
                    if((c = _pfgetc(_pffile(pp_file_))) == EOF)
                    {
                        seek(oldpos, SEEK_SET);
                        return -1;
                    }
                    else if(c == 0X0A)
                    */
            ls++;
        }
    }

    return tell();
}

//=============================================================================
// NAME: gotoline()
// DESC:
//=============================================================================
pffpos_t FileOperator::gotonextline( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    pffpos_t oldpos = tell();
    unsigned int c = 0;

    while( !_pfeof( _pffile( pp_file_ ) ) )
    {
        if( ( c = _pfgetc( _pffile( pp_file_ ) ) ) == EOF )
        {
            seek( oldpos, SEEK_SET );
            return -1;
        }
        else if( c == '\n' )
        {
            /*
                    if((c = _pfgetc(_pffile(pp_file_))) == EOF)
                    {
                        seek(oldpos, SEEK_SET);
                        return -1;
                    }
                    else if(c == 0X0A)
                    */
            break;
        }
    }

    return tell();
}

/////////////////////////////////////////////////////////////
// file r/w

//=============================================================================
// NAME:
// DESC:
//=============================================================================
long FileOperator::seek( pffpos_t offset, int origin )
{
    if( origin == SEEK_SET )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )( offset ), origin );
    }
    else if( origin == SEEK_CUR )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )offset, origin );
    }

    if( origin == SEEK_END )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )( ( _pffilelength( _pffile( pp_file_ ) ) - flength() ) + offset ), origin );
    }

    return 0;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileOperator::read( void* buffer, size32_t size, size32_t count )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    return _pfread( buffer, size, count, _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileOperator::write( const void* buffer, size32_t size, size32_t count )
{
    if( ( _pffile( pp_file_ ) == 0 ) || ( buffer == NULL ) )
    {
        return 0;
    }

    return _pfwrite( buffer, size, count, _pffile( pp_file_ ) );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
//size32_t FileOperator::printf( const char* format, ... )
//{
//    if( _pffile( pp_file_ ) == 0 )
//    {
//        return 0;
//    }
//
//    int ret = 0;
//    va_list argList;
//    va_start( argList, format );
//    //fprintf(_pffile(pp_file_), format, argList);
//    //FormatV(format, argList);
//    va_end( argList );
//    return ret;
//}

//=============================================================================
// NAME: readword()
// DESC:
//=============================================================================
const char* FileOperator::readword( _mstring& buffer )
{
    buffer.clear();
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    int c = 0;

    do
    {
        if( ( c = _pfgetc( _pffile( pp_file_ ) ) ) == EOF )
        {
            buffer.clear();
            return NULL;
        }
    } while( ( c == 0X20 ) || ( c == 0X0D ) || ( c == 0X0A ) || ( c == 0X09 ) );

    //buffer[0] = c;
    //readline( buffer + 1, size - 1, 0X20 );
    seek( -1, SEEK_CUR );
    readline( buffer, 0X20 );
    return buffer.c_str();
}

//=============================================================================
// NAME: readline()
// DESC:
//=============================================================================
const char* FileOperator::readline( _mstring& buffer )
{
    buffer.clear();
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    //fgets(buffer, size, _pffile(pp_file_));
    // 部要用fgets會有ODOA的問題 \0 前面會多一個 13 == 0D
    size32_t i = 0;

    while( 1 )
    {
        char cc = ( char )_pfgetc( _pffile( pp_file_ ) );
        if( ( ( ( int )cc ) == EOF ) || ( cc == '\n' ) )
        {
            break;
        }
        buffer.push_back( cc );
    }

    //buffer[i] = '\0';

    //if ( i > 0 )
    //{
    //    if ( buffer[i - 1] == 0X0D )
    //    {
    //        buffer[i - 1] = '\0';
    //    }
    //}

    if( !buffer.empty() )
    {
        if( buffer[ buffer.size() - 1 ] == 0X0D )
        {
            buffer[ buffer.size() - 1 ] = '\0';
        }
    }
    return buffer.c_str();                              // Jump Back
}

//=============================================================================
// NAME: readline
// DESC:
//=============================================================================
const char* FileOperator::readline( _mstring& buffer, char end )
{
    buffer.clear();
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    size32_t i = 0;

    while( 1 )
    {
        char cc = ( char )_pfgetc( _pffile( pp_file_ ) );

        if( ( cc == end ) || ( ( ( int )cc ) == EOF ) || ( cc == '\n' ) )
        {
            break;
        }

        //buffer[i] = cc;
        buffer.push_back( cc );
        //             if ((buffer[i] = (char) _pfgetc(_pffile(pp_file_))) == EOF)
        //                 break;
        //             if ((buffer[i] == end) ||
        //                  (buffer[i] == 0X0D) ||
        //                  (buffer[i] == 0X0A))
        //                 break;
        //i++;
    }

    //buffer[i] = '\0';
    //
    //if ( i > 0 )
    //{
    //    if ( buffer[i - 1] == 0X0D )
    //    {
    //        buffer[i - 1] = '\0';
    //    }
    //}
    if( !buffer.empty() )
    {
        if( buffer[ buffer.size() - 1 ] == 0X0D )
        {
            buffer[ buffer.size() - 1 ] = '\0';
        }
    }

    return buffer.c_str();
}

//=============================================================================
// NAME: readline
// DESC:
//=============================================================================
const char* FileOperator::readline( _mstring& buffer, const char* end )
{
    buffer.clear();
    if( _pffile( pp_file_ ) == 0 )
    {
        return NULL;
    }

    pffpos_t nowpos = tell();
    pffpos_t endpos = findword( end );

    if( endpos <= 0 )
    {
        return NULL;
    }

    seek( nowpos, SEEK_SET );

    {
        int bufsize = ( int )( endpos - nowpos + 1 );
        buffer.resize( bufsize + 1 );
        //ubyte_t* buff = new ubyte_t[ bufsize + 1 ];
        if( bufsize > 0 )
        {
            read( ( char* )buffer.c_str(), bufsize, 1 );
        }
        //buff[ bufsize - 1 ] = '\0';
        //buffer = ( char* )buff;
        //SAFE_DELETE_ARRAY( buff );
    }

    // 移到下一個位元 並跳過 結束字元
    seek( endpos + strlen( end ), SEEK_SET );
    //pffpos_t curpos = tell();
    return buffer.c_str();
}

//=============================================================================
// NAME: writeline()
// DESC:
//=============================================================================
size32_t FileOperator::writeline( const char* buffer, size32_t size )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    size_t ret = _pfwrite( buffer, size, 1, _pffile( pp_file_ ) );
    _pfputc( '\n', _pffile( pp_file_ ) );
    return ret;
}

//=============================================================================
// NAME: readbetween
// DESC:
//=============================================================================
const char* FileOperator::readbetween( _mstring& buffer, char begin, char end )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    int c = 0;

    while( ( c = _pfgetc( _pffile( pp_file_ ) ) ) != begin )
    {
        //c = fgetc(_pffile(pp_file_));
        if( c == EOF )
        {
            return NULL;
        }
    }
    return readline( buffer, end );
}


//=============================================================================
// NAME: readbetween
// DESC:
//=============================================================================
const char* FileOperator::readbetween( _mstring& buffer, const char* begin, const char* end )
{
    if( findword( begin ) == -1 )
    {
        return NULL;
    }
    return readline( buffer, end );
}


//=============================================================================
// NAME: replaceword
// DESC: rplace old word to new word
//=============================================================================
pffpos_t FileOperator::replaceword( const char* oldword, const char* newword )
{
    pffpos_t pos = findword( oldword );

    if( pos >= 0 )
    {
        //seek(pos, SEEK_SET);
        fshift( _pffile( pp_file_ ), strlen( newword ) - strlen( oldword ) );
        seek( pos, SEEK_SET );
        write( newword, strlen( newword ), 1 );
    }

    return pos;
}











/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::Offset(int fileno)
{
    return getfileaddr(fileno, _pffile(pp_file_), base_offset());
}
//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileOperator::Offset(const char* fname)
{
    return Offset(getfileno(fname, _pffile(pp_file_), base_offset()));
}
*/
//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
void FileOperator::AddFileFront(const char addfile, int frontfileno, bool deletefile)
{
}
*/

/////////////////////////////////////////////////////////////
//  package file function

//=============================================================================
// NAME:
// DESC:
//=============================================================================
FileSystem::FileSystem( const char* filename, const char* mode )
{
    brewind();
    ppworkdir_[ 0 ] = '\0';
    open( filename, mode );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
FileSystem::~FileSystem()
{
}

//=============================================================================
// NAME: Open()
// DESC:
//=============================================================================
uPfFile FileSystem::open( const char* path, const char* mode )
{
    if( path == NULL )
    {
        return NULL;
    }

    char currbuf[ 256 ];
    mcurrentpath( currbuf, path );
    // get the real exist file path
    char dpath[ _MAX_LPATH ];
    splitppath( currbuf, 0, dpath, _MAX_LPATH );

    if( ( FileOperator::open( dpath, mode ) ) == NULL ) //開檔如果-1則失敗，傳回-1
    {
        return NULL;
    }

    if( bfind( currbuf ) < 0 )
    {
        close();
        errno_ = _ERR_FILE_NOT_FIND;
        return NULL;
    }

    return pp_file_;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
error_t FileSystem::close( void )
{
    int result = FileOperator::close();
    ppfname_[ 0 ] = '\0';
    ppworkdir_[ 0 ] = '\0';
    return result;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::flength( void )
{
    //open a exist file
    if( base_offset() == 0 )
    {
        return _pffilelength( _pffile( pp_file_ ) );
    }

    //open a package file
    char path[ _MAX_LPATH ];
    sprintf( path, "%s%s", fname_.c_str(), ppfname_.c_str() );
    return pfnamegetsize( path, _pffile( pp_file_ ) );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long FileSystem::seek( pffpos_t offset, int origin )
{
    if( origin == SEEK_SET )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )( base_offset() + offset ), origin );
    }
    else if( origin == SEEK_CUR )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )offset, origin );
    }

    if( origin == SEEK_END )
    {
        return ( long )_pfseek( _pffile( pp_file_ ), ( long )( ( _pffilelength( _pffile( pp_file_ ) ) - base_offset() - flength() ) + offset ), origin );
    }

    return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long FileSystem::tell( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return -1;
    }

    return  _pftell( _pffile( pp_file_ ) ) - base_offset();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
pffpos_t FileSystem::pos( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return -1;
    }

    return  _pftell( _pffile( pp_file_ ) ) - base_offset();
}

//=============================================================================
// NAME: eof()
// DESC: is now end of file
//=============================================================================
int FileSystem::eof( void )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 1;
    }

    if( base_offset() == 0 )
    {
        return _pfeof( _pffile( pp_file_ ) );
    }

    return ( tell() == flength() ) ? 1 : 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
offset_t FileSystem::base_offset( void )
{
    return m_base_offset_;
}


/////////////////////////////////////////////////////////////
// file tree moving

//=============================================================================
// NAME: ppOpen()
// DESC:
//=============================================================================
offset_t FileSystem::bfind( const char* path )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return 0;
    }

    int offset = 0;
    char tmpdir[ _MAX_LDIR ];
    transppath( path, ppworkdir_.c_str(), tmpdir, _MAX_LDIR );

    if( ( offset = pfnamegetaddr( tmpdir, _pffile( pp_file_ ) ) ) < 0 )
    {
        return errno_ = offset;
    }

    m_base_offset_ = offset;
    char tmpD[ _MAX_LPATH ];
    splitppath( tmpdir, 0, tmpD, _MAX_LPATH );
    ppfname_ = ( tmpdir + strlen( tmpD ) );
    //strcpy( ppfname_, ( tmpdir + strlen( tmpD ) ) );
    _pfseek( _pffile( pp_file_ ), m_base_offset_, SEEK_SET );
    errno_ = _PF_OK;
    return m_base_offset_;
}


//=============================================================================
// NAME:
// DESC:  rewind the base offset to 0
//=============================================================================
offset_t FileSystem::brewind( void )
{
    ppfname_[ 0 ] = '\0';
    return m_base_offset_ = 0;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::workdeep( void )
{
    return pathdeep( ppworkdir_.c_str() );
}



//=============================================================================
// NAME: ppCd()
// DESC: success return _PF_OK
//=============================================================================
int FileSystem::fsCd( const char* path )
{
    char tmpdir[ _MAX_LDIR ];
    transppath( path, ppworkdir_.c_str(), tmpdir, _MAX_LDIR );

    if( ( errno_ = pfnamegetaddr( tmpdir, _pffile( pp_file_ ) ) ) < 0 ) // file exist
    {
        return errno_;
    }

    if( ( errno_ = checkformat( _pffile( pp_file_ ), errno_ ) ) != _PF_OK ) //no a pp folder
    {
        return errno_;
    }

    ppworkdir_ = tmpdir;
    return errno_ = _PF_OK;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const char* FileSystem::fsPwd( void )
{
    return ppworkdir_.c_str();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
char* FileSystem::ppTransfname( char* fname, size32_t size )
{
    sprintf( fname, "%s%s", fname_.c_str(), ppfname_.c_str() );
    makeppath( fname, fname, _MAX_LFNAME );
    return fname;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
void FileOperator::SetTitle(const char* title)
{
memcpy(m_title_ext, title, sizeof(m_title_ext));
}
*/
//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::checkFormat( const word_t title )
{
    if( _pffile( pp_file_ ) == 0 )
    {
        return errno_ = _ERR_NOT_OPEN_FILE;
    }

    if( ( errno_ = checkformat( _pffile( pp_file_ ), base_offset() ) ) != _PF_OK )
    {
        return errno_;
    }

    if( title == 0 )
    {
        return errno_ = _PF_OK;
    }

    byte_t  extTitle[ 2 ];
    byte_t* ftitle = ( byte_t* )( &title );
    _pfseek( _pffile( pp_file_ ), base_offset() + 2, SEEK_SET );
    read( extTitle, 1, sizeof( word_t ) );

    if( ( ftitle[ 0 ] == extTitle[ 0 ] ) && ( ftitle[ 1 ] == extTitle[ 1 ] ) )
    {
        return errno_ = _PF_OK;
    }

    // ext title mark
    return errno_ = _ERR_TITLE_UN_CONFORM;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::FileCount( void )
{
    if( errno_ = checkformat( _pffile( pp_file_ ), base_offset() ) != _PF_OK )
    {
        return 0;
    }

    int fcount = getfilecount( _pffile( pp_file_ ), base_offset() );

    if( fcount < 0 )
    {
        errno_ = fcount;
        return 0;
    }

    //errno_ = _PF_OK;
    return fcount;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileSystem::FileSize( int fileno )
{
    if( errno_ = checkformat( _pffile( pp_file_ ), base_offset() ) != _PF_OK )
    {
        return 0;
    }

    int fsize = getfilesize( fileno, _pffile( pp_file_ ), base_offset() );

    if( fsize < 0 )
    {
        errno_ = fsize;
        return 0;
    }

    //errno_ = _PF_OK;
    return fsize;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::FileName( int fileno, char* fname, size32_t size )
{
    if( ( errno_ = checkformat( _pffile( pp_file_ ), base_offset() ) ) != _PF_OK )
    {
        return errno_;
    }

    strcpy( fname, "no name" );
    return errno_ = getfilename( fileno, fname, size, _pffile( pp_file_ ), base_offset() );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int FileSystem::FileNo( const char* fname )
{
    if( errno_ = checkformat( _pffile( pp_file_ ), base_offset() ) != _PF_OK )
    {
        return errno_;
    }

    int fno = getfileno( fname, _pffile( pp_file_ ), base_offset() );

    if( fno < 0 )
    {
        return errno_ = fno;
    }

    errno_ = _PF_OK;
    return fno;
}


size32_t FileSystem::readfile( ubyte_t** buffer, unsigned int index ) //讀入Pp圖形包裝檔
{
    int faddr = getfileaddr( index,
        _pffile( pp_file_ ),
        base_offset() );

    if( faddr < 0 )
    {
        errno_ = _ERR_FILE_NOT_FIND;
        return 0;
    }

    dword_t size = getfilesize( index, _pffile( pp_file_ ), base_offset() );

    if( size < 0 )
    {
        errno_ = _ERR_FILE_NOT_FIND;
        return 0;
    }

    ( *buffer ) = new ubyte_t[ size ];
    _pfseek( _pffile( pp_file_ ), faddr, SEEK_SET );
    read( ( *buffer ), size, 1 );
    return size;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileSystem::readfile( ubyte_t** buffer, const char* ppfname ) //讀入Pp圖形包裝檔
{
    //if(ppfname== NULL)
    //  return 0;
    char dir[ _MAX_LDIR ];
    sprintf( dir, "%s", ppfname_.c_str() );

    if( ppfname != 0 )
    {
        strcat( dir, ppfname );
    }

    //if(strcmp(dir, "") == 0)
    //  return errno_ = _ERR_NOT_CORRECT_PP_PATH;
    int faddr = pfnamegetaddr( dir, _pffile( pp_file_ ) );

    if( faddr < 0 )
    {
        errno_ = faddr;
        return 0;
    }

    int size = pfnamegetsize( dir, _pffile( pp_file_ ) );

    if( size < 0 )
    {
        errno_ = size;
        return 0;
    }

    ( *buffer ) = new ubyte_t[ size ];
    _pfseek( _pffile( pp_file_ ), faddr, SEEK_SET );
    read( ( *buffer ), size, 1 );
    return size;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileSystem::Extractfile( const char* ppfname )
{
    //make dest path
    char spath[ _MAX_LPATH ];
    char dpath[ _MAX_LPATH ];
    sprintf( spath, "%s%s", fname_.c_str(), ppfname_.c_str() );

    if( ppfname != 0 )
    {
        strcat( spath, ppfname );
    }

    makeppath( spath, dpath, _MAX_LPATH );
    ubyte_t* buffer;
    int fsize = readfile( &buffer, ppfname );

    if( fsize <= 0 )
    {
        errno_ = fsize;
        return 0;
    }

    uPfFile f;

    if( ( f = _pfmopen( dpath, "wb" ) ) == NULL )
    {
        perror( "open extract dest file fail :" );
        errno_ = _ERR_OPEN_TEMP_EXTRACT_FILE_FAIL;
        return 0;
    }

    _pfwrite( buffer, fsize, 1, _pffile( f ) );
    _pfclose( _pffile( f ) );
    delete[] buffer;
    return fsize;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
error_t FileSystem::Packagefiles( std::vector<const char*> filename,
    const word_t exttitle,
    int del_src )
{
    //uPfFile _pffile(gSave_Temp_f);
    char tempfpath[ _MAX_LPATH ];
    // 保存每一個壓縮檔的位移資料
    std::vector<int> offset_contain;
    offset_contain.clear();
    /* open temp file */
    sprintf( tempfpath, "%s%s", fname_.c_str(), ".tmp" );

    if( ( gSave_Temp_f = _pfmopen( tempfpath, "wb" ) ) == NULL )
    {
        perror( "Could not open new temporary file\n" );
        return errno_ = _ERR_OPEN_FILE_FAIL;
    }

    dword_t filescount = 0;

    for( unsigned int i = 0; i < filename.size(); i++ )
    {
        //filescount = ScanFiles( filename[i],
        //                                                      Add_File,
        //                                                      &offset_contain,
        //                                                      0 ) + filescount;
        filescount = pfsfl::DirectoryScan( filename[ i ], Add_File, &offset_contain ) + filescount;
    }

    _pfclose( _pffile( _pffile( gSave_Temp_f ) ) ); //close if first

    if( ( gSave_Temp_f = _pfmopen( tempfpath, "rb" ) ) == NULL )
    {
        perror( "Could not open new temporary file\n" );
        return errno_ = _ERR_OPEN_TEMP_COMBINE_FILE_FAIL;
    }

    int temp_len = _pffilelength( gSave_Temp_f );

    if( temp_len <= 0 )
    {
        close();
        return errno_ = _ERR_BUFS_OVERFLOW;
    }

    byte_t* buf = new byte_t[ temp_len ];
    _pfseek( _pffile( _pffile( gSave_Temp_f ) ), 0, SEEK_SET );
    _pfread( buf, 1, temp_len, _pffile( gSave_Temp_f ) );
    _pfclose( _pffile( gSave_Temp_f ) );
    gSave_Temp_f = NULL;
    //Remove temporary files
    unlink( tempfpath );
    _pfseek( _pffile( pp_file_ ), 0, SEEK_SET );
    dword_t fileoffset = FILE_HEALE_SIZE;
    dword_t headoffset = FILE_HEALE_SIZE + temp_len;
    //write file title
    write( &PFTITLE, sizeof( PFTITLE ), 1 );
    write( &exttitle, sizeof( word_t ), 1 );
    //write file conunt
    write( &filescount, sizeof( dword_t ), 1 );
    //write file offset
    write( &fileoffset, sizeof( dword_t ), 1 );
    //write head offset
    write( &headoffset, sizeof( dword_t ), 1 );

    if( filescount != 0 )
    {
        write( buf, temp_len, 1 );
        int file_offset = 0;

        for( unsigned int j = 0; j < offset_contain.size(); j++ )
        {
            write( &file_offset, sizeof( dword_t ), 1 );
            file_offset = file_offset + offset_contain[ j ];
        }

        write( &file_offset, sizeof( dword_t ), 1 );
    }

    offset_contain.clear();
    SAFE_DELETE_ARRAY( buf );

    //delete combine source file
    if( del_src == 1 )
    {
        for( unsigned int i = 0; i < filename.size(); i++ )
        {
            unlink( filename[ i ] );
        }
    }

    close();
    return errno_ = _PF_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//load folder
//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t FileSystem::Scanfiles( const char* root, OnFileM fcallback, void* pData )
{
    return pfsfl::DirectoryScan( root, fcallback, pData );
}

size32_t FileSystem::Scanfiles( const wchar_t* root, OnFileW  fcallback, void* pData )
{
    return pfsfl::DirectoryScan( root, fcallback, pData );
}


///////////////////////////////////////////////////////////////////////////////
//
namespace
{
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int checkformat( uPfFile ppf, unsigned int base_offset )
    {
        if( ppf == 0 )
        {
            return _ERR_NOT_OPEN_FILE;
        }

        int tempoffset = base_offset;
        _pfseek( _pffile( ppf ), base_offset, SEEK_SET );
        word_t title;
        _pfread( &title, sizeof( title ), 1, _pffile( ppf ) );
        _pfseek( _pffile( ppf ), tempoffset, SEEK_SET );

        if( title == PFTITLE )
        {
            return _PF_OK;
        }

        return  _ERR_NOT_PF_FILE;
    }

    int pfcheckfileno( unsigned int fileno, uPfFile ppf, unsigned int base_offset )
    {
        if( fileno > ( unsigned int )getfilecount( ppf, base_offset ) )
        {
            return  _ERR_PF_OVER_EOF;
        }

        return _PF_OK;
    }

    int getfilecount( uPfFile ppf, unsigned int base_offset )
    {
        int ret = 0;

        if( ( ret = checkformat( ppf, base_offset ) ) != _PF_OK )
        {
            return ret;
        }

        unsigned int fcount;
        _pfseek( _pffile( ppf ), base_offset + sizeof( dword_t ), SEEK_SET );
        _pfread( &fcount, sizeof( dword_t ), 1, _pffile( ppf ) );
        return fcount;
    }

    int getfileoffset( unsigned int fileno, uPfFile ppf, unsigned int base_offset )
    {
        int ret = 0;

        if( ( ret = checkformat( ppf, base_offset ) ) != _PF_OK )
        {
            return ret;
        }

        if( ( ret = pfcheckfileno( fileno, ppf, base_offset ) ) != _PF_OK )
        {
            return ret;
        }

        dword_t fileoffset = 0;
        dword_t headoffset = 0;
        _pfseek( _pffile( ppf ), base_offset + 8, SEEK_SET );
        _pfread( &fileoffset, sizeof( dword_t ), 1, _pffile( ppf ) );
        _pfread( &headoffset, sizeof( dword_t ), 1, _pffile( ppf ) );
        _pfseek( _pffile( ppf ), base_offset + headoffset + ( sizeof( OffsetField ) * fileno ), SEEK_SET );
        dword_t offset = 0;
        _pfread( &offset, sizeof( OffsetField ), 1, _pffile( ppf ) );
        return base_offset + fileoffset + offset;
    }

    int getfileaddr( unsigned int fileno, uPfFile ppf, unsigned int base_offset )
    {
        dword_t offset;

        if( ( offset = getfileoffset( fileno, ppf, base_offset ) ) < 0 )
        {
            return offset;
        }

        _pfseek( _pffile( ppf ), offset, SEEK_SET );
        dword_t fnamelen = 0;
        _pfread( &fnamelen, sizeof( dword_t ), 1, _pffile( ppf ) );
        return offset + sizeof( dword_t ) + fnamelen;
    }

    int getfileeof( unsigned int fileno, uPfFile ppf, unsigned int base_offset )
    {
        int fcount = getfilecount( ppf, base_offset );

        if( fcount < 0 )
        {
            return fcount;
        }

        _pfseek( _pffile( ppf ), base_offset + FILE_HEALE_SIZE + ( sizeof( OffsetField ) * fcount ), SEEK_SET );
        dword_t offset = 0;
        _pfread( &offset, sizeof( OffsetField ), 1, _pffile( ppf ) );
        return base_offset + offset;
    }

    int getfilesize( unsigned int fileno, uPfFile ppf, unsigned int base_offset )
    {
        int begin = getfileaddr( fileno, ppf, base_offset );

        if( begin < 0 )
        {
            return begin;
        }

        int end = getfileoffset( fileno + 1, ppf, base_offset );

        if( end == _ERR_PF_OVER_EOF )
        {
            end = getfileeof( fileno + 1, ppf, base_offset );
        }
        else if( end < 0 )
        {
            return end;
        }

        return end - begin;
    }

    int getfilename( unsigned int fileno, char* fname, size32_t size, uPfFile ppf, unsigned int base_offset )
    {
        fname[ 0 ] = '\0';
        //fileno must in range
        int ret = 0;

        if( ( ret = pfcheckfileno( fileno, ppf, base_offset ) ) != _PF_OK )
        {
            return ret;
        }

        //get file offset
        int offset = getfileoffset( fileno, ppf, base_offset );

        if( offset < 0 )
        {
            return offset;
        }

        //read filename
        //move to file offset
        _pfseek( _pffile( ppf ), offset, SEEK_SET );
        //read file name length
        dword_t fnamelen = 0;
        _pfread( &fnamelen, sizeof( dword_t ), 1, _pffile( ppf ) );

        //check fname buffer
        if( size < fnamelen )
        {
            return _ERR_NO_BUFS;
        }

        //read file name
        _pfread( fname, 1, fnamelen, _pffile( ppf ) );
        fname[ fnamelen ] = '\0';
        return _PF_OK;
    }

    int getfileno( unsigned int addr, uPfFile ppf, unsigned int base_offset )
    {
        int fcount = getfilecount( ppf, base_offset );

        if( fcount < 0 )
        {
            return fcount;
        }

        for( int i = 0; i < fcount; i++ )
        {
            unsigned int iaddr = getfileaddr( i, ppf, base_offset );

            if( addr == iaddr )
            {
                return i;
            }

            //read file name
        }

        return _ERR_FILE_NOT_FIND;
    }

    int getfileno( const char* fname, uPfFile ppf, unsigned int base_offset )
    {
        int fcount = getfilecount( ppf, base_offset );

        if( fcount < 0 )
        {
            return fcount;
        }

        for( int i = 0; i < fcount; i++ )
        {
            char name[ _MAX_LFNAME ];
            int ret = getfilename( i, name, _MAX_LFNAME, ppf, base_offset );

            if( ret < 0 )
            {
                return ret;
            }

            if( strcmp( name, fname ) == 0 )
            {
                return i;
            }
        }

        return _ERR_FILE_NOT_FIND;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    // 要不要檢查下一個
    inline bool isppsep( char c, bool& bcnext )
    {
        if( bcnext )
        {
            if( ( c > 0x19 && c < 0x7F ) )
            {
                bcnext = true;
                return ( ( c == PF_NAME_MARK ) || ( c == PF_NO_MARK ) || ( c == PF_ARRD_MARK ) ) ? 1 : 0;
            }
            else
            {
                bcnext = false;
                return 0;
            }
        }

        bcnext = true;
        return 0;
    }
    //#define isppsep(c)  ((c == PF_NAME_MARK) || (c == PF_NO_MARK) || (c == PF_ARRD_MARK))? 1 : 0

    int pathdeep( const char* path )
    {
        int deep = 0;

        if( path != 0 )
        {
            deep++;
        }
        else
        {
            return -1;
        }

        bool cnext = true;

        for( int i = 0; i < ( int )strlen( path ); i++ )
        {
            if( isppsep( path[ i ], cnext ) )
            {
                deep++;
            }

            /*
            if((path[i] > 0x19 && path[i] < 0x7F))
            {
                if (isppsep(path[i]))
                    deep++;
            }
            else
            {
                // 跳過下一個字
                i++;
            }
            */
        }

        return deep;
    }

    // DESC:
    int makeppath( const char* path, char* buffer, size32_t size )
    {
        size_t len = strlen( path );

        if( size < len )
        {
            return _ERR_NO_BUFS;
        }

        bool cnext = true;

        for( size_t i = 0; i < len; i++ )
        {
            if( isppsep( path[ i ], cnext ) )
            {
                buffer[ i ] = '_';
            }
            else
            {
                buffer[ i ] = path[ i ];
            }

            /*
            if((path[i] > 0x19 && path[i] < 0x7F))
            {
                if (isppsep(path[i], cnext))
                    buffer[i] = '_';
                else
                    buffer[i] = path[i];
            }
            else
            {
                // 跳過下一個字
                buffer[i] = path[i];
                i++;
                buffer[i] = path[i];
            }
            */
        }

        buffer[ len ] = '\0';
        return _PF_OK;
    }

    // DESC: split the exist file name and virtual file name
    char splitppath( const char* path, unsigned int deep, char* buffer, size32_t size )
    {
        unsigned int lpdeep = 0;
        char type = '/';
        int i = 0;
        bool cnext = true;

        while( ( lpdeep < deep ) && ( i < ( int )strlen( path ) ) )
        {
            if( isppsep( path[ i ], cnext ) )
            {
                type = path[ i ];
                lpdeep++;
            }

            i++;
            /*
            if((path[i] > 0x19 && path[i] < 0x7F))
            {
                if (isppsep( path[i]))
                {
                    type = path[i];
                    lpdeep++;
                }
                i++;
            }
            else
            {
                i++;
                i++;
            }
            */
        }

        if( lpdeep != deep )
        {
            buffer[ 0 ] = '\0';
            return NULL;
        }

        int j = 0;
        /*
        strcpy(buffer, path);
        const char* p=strtok(buffer, "?|*");
        strcpy(buffer, p);
        return type;
        */
        cnext = true;

        while( ( j < ( int )( size - 1 ) ) && ( j < ( int )strlen( path ) ) )
        {
            if( isppsep( path[ i ], cnext ) )
            {
                break;
            }

            buffer[ j ] = path[ i ];
            i++;
            j++;
            /*
            if((path[i] > 0x19 &&path[i] < 0x7F))
            {
                if (isppsep(path[i]))
                    break;
                buffer[j] = path[i];
                i++;
                j++;
            }
            else
            {
                buffer[j] = path[i];
                i++;
                j++;

                buffer[j] = path[i];
                i++;
                j++;
            }
            */
        }

        buffer[ j ] = '\0';
        return type;
    }

    // DESC:
    int dumpppath( const char* path, unsigned int deep, char* buffer, size32_t size )
    {
        char tmpbuf[ _MAX_LFNAME ];
        splitppath( path, deep, tmpbuf, size );
        int len = strlen( tmpbuf );

        if( len == 0 )
        {
            return _ERR_FILE_NOT_FIND;
        }

        strcpy( buffer, path );
        char* s = strstr( buffer, tmpbuf );
        strcpy( ( s - 1 ), ( s + len ) );
        return _PF_OK;
    }

    // DESC:
    char* transppath( const char* path, const char* dir, char* buffer, size32_t size )
    {
        strcpy( buffer, path );
        char fpath[ _MAX_LPATH ];
        splitppath( path, 0, fpath, _MAX_LPATH );

        if( strcmp( fpath, "" ) == 0 ) //相對路徑
        {
            strcpy( buffer, dir );

            for( int d = 1; d < pathdeep( path ); d++ ) // 絕對路徑
            {
                char type = splitppath( path, d, fpath, _MAX_LPATH );

                if( strcmp( fpath, ".." ) == 0 )
                {
                    int pwdd = pathdeep( buffer ) - 1;
                    dumpppath( buffer, pwdd, buffer, _MAX_LDIR );
                }
                else
                {
                    sprintf( buffer, "%s%c%s", buffer, type, fpath );
                }
            }
        }

        return buffer;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int pfnamegetaddr( const char* path, uPfFile ppf )
    {
        if( ppf == 0 )
        {
            return _ERR_OPEN_SOURCE_COMBINE_FILE_FAIL;
        }

        char fname[ _MAX_LFNAME ];
        int offset = 0;
        unsigned int deep = pathdeep( path );
        _pfseek( _pffile( ppf ), 0, SEEK_SET );

        if( deep == 0 )
        {
            return _ERR_FILE_NOT_FIND;
        }
        else if( deep == 1 ) //file path
        {
            return offset;
        }

        for( unsigned int i = 1; i < deep; i++ )
        {
            int ret = checkformat( ppf, offset );

            if( ret != _PF_OK ) // ppf = 0 or not a ppf
            {
                return ret;
            }

            int fno = 0;

            switch( splitppath( path, i, fname, _MAX_LFNAME ) )
            {
            case PF_NO_MARK:
                // no
                {
                    fno = atoi( fname );
                    int ret = pfcheckfileno( fno, ppf, offset ); //fileno overflow

                    if( ret != _PF_OK )
                    {
                        return ret;
                    }

                    break;
                }

            case PF_NAME_MARK:
                // name
                {
                    if( ( fno = getfileno( fname, ppf, offset ) ) < 0 )
                    {
                        return fno;
                    }

                    break;
                }

            case PF_ARRD_MARK:
                //offset
                {
                    if( ( fno = getfileno( atoi( fname ), ppf, offset ) ) < 0 )
                    {
                        return fno;
                    }

                    break;
                }

            default:
                return _ERR_PF_PATH;
            }

            if( ( offset = getfileaddr( fno, ppf, offset ) ) < 0 )
            {
                return offset;
            }
        }

        return offset;
    }


    int pfnamegetsize( const char* path, uPfFile ppf )
    {
        char fname[ _MAX_LFNAME ];
        int offset = 0;
        unsigned int deep = pathdeep( path );
        _pfseek( _pffile( ppf ), 0, SEEK_SET );

        if( deep == 0 )
        {
            return _ERR_FILE_NOT_FIND;
        }
        else if( deep == 1 ) //file path
        {
            return _pffilelength( _pffile( ppf ) );
        }

        for( unsigned int i = 1; i < deep; i++ )
        {
            int ret = checkformat( ppf, offset );

            if( ret != _PF_OK ) // ppf = 0 or not a ppf
            {
                return ret;
            }

            int fno = 0;

            switch( splitppath( path, i, fname, _MAX_LFNAME ) )
            {
            case PF_NO_MARK:
                // no
                {
                    fno = atoi( fname );
                    int ret = pfcheckfileno( fno, ppf, offset );

                    if( ret != _PF_OK )
                    {
                        return ret;
                    }

                    break;
                }

            case PF_NAME_MARK:
                // name
                {
                    if( ( fno = getfileno( fname, ppf, offset ) ) < 0 )
                    {
                        return fno;
                    }

                    break;
                }

            case PF_ARRD_MARK:
                //offset
                {
                    if( ( fno = getfileno( atoi( fname ), ppf, offset ) ) < 0 )
                    {
                        return fno;
                    }

                    break;
                }

            default:
                return _ERR_PF_PATH;
            }

            if( i == ( deep - 1 ) )
            {
                return getfilesize( fno, ppf, offset );
            }

            if( ( offset = getfileaddr( fno, ppf, offset ) ) < 0 )
            {
                return offset;
            }
        }

        return offset;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int maketmpfname( str_t tmpfname, int size, const char* srcfname )
    {
        if( size < ( int )( strlen( srcfname ) + strlen( ".tmp" ) ) )
        {
            return -1;
        }

        sprintf( tmpfname, "%s.tmp", srcfname );
        return 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //         long FileAttributes(const char* path)
    //         {
    //             struct _finddata_t c_file;
    //             long hFile;
    //
    //             if ((hFile = _findfirst(path, &c_file)) == -1L)
    //                 return -1;
    //
    //             _findclose(hFile);
    //
    //             return c_file.attrib;
    //         }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================7================================
    bool Add_File( const char* srcfname, int depth, void* pData )
    {
        uPfFile f;
        std::vector<int>* foffset = ( std::vector<int>* ) pData;

        if( ( f = _pfmopen( srcfname, "rb" ) ) == NULL )
        {
            perror( "add file fail !!!" );
            return true;
        }

        char fname[ _MAX_LFNAME ];
        dword_t fnamesize = pathrdivf( fname, _MAX_LFNAME, srcfname, depth );
        _pfseek( _pffile( f ), 0, SEEK_SET );                //move to combine file base_offset()
        _pfwrite( &fnamesize, sizeof( dword_t ), 1, _pffile( gSave_Temp_f ) ); //write file name size;
        _pfwrite( &fname, fnamesize, 1, _pffile( gSave_Temp_f ) ); //write file name;
        //int file_len = _pffilelength(_pffileno(_pffile(f)));
        int file_len = _pffilelength( f );

        if( file_len > 0 )
        {
            byte_t* buf = new byte_t[ file_len ];
            _pfread( buf, 1, file_len, _pffile( f ) );
            _pfwrite( buf, file_len, 1, _pffile( gSave_Temp_f ) ); //write file
            delete[] buf;
        }

        _pfclose( _pffile( f ) );
        //包含檔名檔案的大小
        foffset->push_back( sizeof( dword_t ) + fnamesize + file_len );
        return true;
    }

    ////////////////////////////////////////////////////////////////////
    //  if shift<0 may have some question about end file
    int fshift( uPfFile f, int shift )
    {
        //pffpos_t oldpos, newpos;
        //_pfgetpos(_pffile(f), &oldpos);
        long oldpos = _pftell( _pffile( f ) );
        _pfseek( _pffile( f ), 0, SEEK_END );
        //_pfgetpos(_pffile(f), &newpos);
        long newpos = _pftell( _pffile( f ) );
        unsigned int bufsize = ( unsigned int )__min( _pffilelength( f ) - oldpos, SWAPBUF_SIZE );
        //_pfseek(f, (0-bufsize), SEEK_END);
        byte_t* swapbuf = new byte_t[ bufsize ];

        while( oldpos != newpos )
        {
            _pfseek( _pffile( f ), ( 0 - bufsize ), SEEK_CUR );
            //_pfgetpos(_pffile(f), &newpos);
            newpos = _pftell( _pffile( f ) );
            _pfread( swapbuf, 1, bufsize, _pffile( f ) );
            _pfseek( _pffile( f ), ( 0 - bufsize + shift ), SEEK_CUR );
            _pfwrite( swapbuf, 1, bufsize, _pffile( f ) );
            _pfseek( _pffile( f ), ( long )newpos, SEEK_SET );
            //_pfgetpos(f, &newpos);
        }

        if( shift < 0 )
        {
            _pfseek( _pffile( f ), shift, SEEK_END );
            char* endbuf = new char[ -shift ];
            memset( endbuf, 0X20, -shift );
            _pfwrite( endbuf, -shift, 1, _pffile( f ) );
            delete[] endbuf;
        }

        delete[] swapbuf;
        return 1;
    }
};

/*
private:
int fshift(uPfFile f, int shift)
{
    FILE* tf = tmPfFileOperator();
    pffpos_t oldpos, newpos;
    _pfgetpos(f, &oldpos);
    fseek(tf, 0, SEEK_SET);
    _pfseek(f, 0, SEEK_END);
    _pfgetpos(f, &newpos);
    int bufsize = __min(filelength(fileno((FILE*)f))-oldpos, SWAPBUF_SIZE);


    //_pfseek(f, (0-bufsize), SEEK_END);

    byte_t* swapbuf = new byte_t[bufsize];
    while(oldpos != newpos)
    {
        _pfseek(f, (0-bufsize), SEEK_CUR);
        _pfgetpos(f, &newpos);
        _pfread(swapbuf, bufsize, 1, f);
        _pfseek(f, (0 - bufsize + shift), SEEK_CUR);
        _pfwrite(swapbuf, bufsize, 1, f);
        _pfseek(f, newpos, SEEK_SET);
        //_pfgetpos(f, &newpos);
    }
    if(shift<0)
    {
        _pfseek(f, shift, SEEK_END);
        char* endbuf = new char[-shift];
        memset(endbuf, 0X20,  -shift);
        _pfwrite(endbuf, -shift, 1, f);
        delete [] endbuf;
    }
    delete [] swapbuf;
    return 1;
}
    struct fileFunctions
    {
        void*  (*Pfopen) (const char* name, const char* mode);
        int    (*Pfclose)(void* stream);
        bool   (*pfexist)(const char* name);
        size32_t (*Pfread) (void* addr, size32_t size, size32_t count, void* stream);
        size32_t (*Pfwrite)(const void *addr, size32_t size, size32_t count, void* stream);
        char*  (*pfgets) (char* buffer, int maxLen, void* stream);
        int    (*pfputs) (const char* buffer, uPfFile stream);
        int    (*pfeof)  (void* stream);
        int    (*Pfseek) (void* stream, long offset, int origin);
        int    (*pfflush)(void* stream);
        int    (*Pftell) (void* stream);
    };
    fileFunctions m_fileFs;
*/


///////////////////////////////////////////////////////////////////////////////
//
/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
str_t FileOperator::dirve(str_t drive)
{
    char dir[_MAX_LDIR];
    char fname[_MAX_LFNAME];
    char ext[_MAX_LEXT];
    _splitpath(fname_, drive, dir, fname, ext );
    return drive;
}
//=============================================================================
// NAME:
// DESC:
//=============================================================================
str_t FileOperator::dir(str_t dir)
{
    char drive[_MAX_DRIVE];
    char fname[_MAX_LFNAME];
    char ext[_MAX_LEXT];
    _splitpath(fname_, drive, dir, fname, ext );
    return dir;
}
//=============================================================================
// NAME:
// DESC:
//=============================================================================
str_t FileOperator::fname(str_t name)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_LDIR];
    char ext[_MAX_LEXT];
    _splitpath(fname_, drive, dir, name, ext );
    return name;
}
//=============================================================================
// NAME:
// DESC:
//=============================================================================
str_t FileOperator::ext(str_t ext)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_LDIR];
    char fname[_MAX_LFNAME];
    _splitpath(fname_, drive, dir, fname, ext );
    return ext;
}
//=============================================================================
// NAME:
// DESC:
//=============================================================================
str_t FileOperator::fname_ext(str_t fname)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_LDIR];
    char ext[_MAX_LEXT];
    _splitpath(fname_, drive, dir, fname, ext );
    strcat(fname, ext);
    return fname;
}
*/
//};
