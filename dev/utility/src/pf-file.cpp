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

//01. == C 系統文件/C++ 系統文件
#if defined(_WINDOWS)
#include <io.h>
#include <Windows.h>
#include <Shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shellapi.h>
#include <direct.h>   //mkdir
#endif // _WINDOWS

#if defined(_LINUX)
#include <dirent.h>
#include <unistd.h>
#include <wchar.h>
#include <errno.h>

#include <sys/types.h>

#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <string>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "memio.inl"
#include "pf-file.h"
#include "pf-string.h"
#include "pf-debug.h"
#include "Unicode.h"


long64_t filelength( _IN FILE* fp )
{
    long64_t flen = 0;
    //#if defined(_WINDOWS)
    //    flen = _filelength( fileno( fp ) );
    //#endif // _WINDOWS
#if defined(_LINUX) || defined(_WINDOWS)
    long64_t oldfpos = (long64_t)ftell( fp );
    if ( fseek( fp, 0, SEEK_END ) == 0 )
    {
        flen = (long64_t)ftell( fp );
    }
    fseek( fp, oldfpos, SEEK_SET );
#endif //_LINUX
#if defined(_PS2)
    long64_t oldfpos = (long64_t)ftell( fp );
    long64_t flen = 0;
    flen = sceLseek( fp, 0, SCE_SEEK_END );
    sceLseek( fp, oldpos, SCE_SEEK_SET );
#endif //_PS2
    return flen;
}

#if defined(_LINUX)

FILE* _wfopen( _IN const wchar_t* filename, _IN const wchar_t* mode )
{
    size_t len_f = wcslen( filename );
    size_t len_m = wcslen( mode );
    char* utf = new char[ len_f + 1 ];
    char* utm = new char[ len_m + 1 ];;
    UnicodeToUTF8( utf, (UInt16*)filename, (int)len_f );
    UnicodeToUTF8( utm, (UInt16*)mode, (int)len_m );

    FILE* f = fopen( utf, utm );
    delete[] utf;
    delete[] utm;
    return f;
}

int mkdir( _IN const char* dirname )
{
    return mkdir( dirname, S_IRWXU | S_IRGRP | S_IXGRP );
}

int _wmkdir( _IN const wchar_t* dirname )
{
    char_t* dirnameA = NULL;
    size_t   n = wcslen( dirname );
    dirnameA = new char[ n + 1 ];
    wcstombs( dirnameA, dirname, n );
    int r = mkdir( dirnameA, S_IRWXU | S_IRGRP | S_IXGRP );
    SAFE_DELETE_ARRAY( dirnameA );
    return r;
}

wchar_t* _wrealpath( _IN const wchar_t* path, _IN wchar_t* resolved_path )
{
    char* outsA = NULL;
    char* insA = NULL;

    size_t fn = wcslen( path );
    outsA = new char[ fn + 1 ];
    insA = new char[ fn + 1 ];
    wcstombs( outsA, path, fn );
    wchar_t* ret = NULL;

    if ( ( realpath( outsA, insA ) ) )
    {
        mbstowcs( resolved_path, outsA, fn );
        ret = resolved_path;
    }

    SAFE_DELETE_ARRAY( outsA );
    SAFE_DELETE_ARRAY( insA );
    return ret;
}

/*
long64_t _filelength( int fd )
{
    // Validate parameters
    FILE* fp = fdopen( fd, "w" );
}
*/

#endif //_LINUX


namespace pfsfl  // stand file library
{
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int DirectoryCreate( _IN const char* dir )
    {
        if ( strlen( dir ) <= 1 )
            return false;

        int ret = true;
        char* currpath = new char[ ( strlen( dir ) * 5 ) + 1 ];
#if defined(_XBOX) || defined(_WINDOWS) || defined(_XBO)
#if defined(_XBO)

        if ( path[ 1 ] != ':' )
        {
            char tpath[ MAX_PATH ];
            sprintf( tpath, "%s%s", XBOXONE_TEMP_FOLDER, path );
            path = tpath;
        }

#endif
        BOOL result = ::CreateDirectoryA( mcurrentpath( currpath, dir ), NULL );

        if ( result == FALSE )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif //defined(_XBOX) || defined(_WINDOWS) || defined(_XBO)
#if defined(_LINUX)
        int32_t result = mkdir( mcurrentpath( currpath, dir ), S_IRWXU | S_IRGRP | S_IXGRP );

        if ( result != 0 )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif //_LINUX
#if defined(_PS3)
        CellFsErrno err;
        err = cellFsMkdir( mcurrentpath( currpath, dir ), CELL_FS_DEFAULT_CREATE_MODE_1 );
        setLastError( err );

        if ( err != CELL_FS_SUCCEEDED )
        {
            if ( err != CELL_FS_EEXIST )
            {
                Pf_ERROR( "DirectoryCreate err" );
            }

            ret = false;
        }

#endif
#if defined(_VITA)
        int result;
        result = sceFiosDirectoryCreateSync( nullptr, wcurrentpath( currpath, dir ) );

        //setLastError( result );
        if ( result != SCE_FIOS_OK )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif
#if defined(_PS4)
        int32_t ret = sceKernelMkdir( wcurrentpath( currpath, dir ), SCE_KERNEL_S_IRWU );

        //setLastError( ret );
        if ( ret < SCE_OK )
        {
            if ( ret != SCE_KERNEL_ERROR_EEXIST )
            {
                Pf_ERROR( "DirectoryCreate err" );
            }

            ret = false;
        }

#endif
        SAFE_DELETE_ARRAY( currpath );
        return ret;
    }

    int DirectoryCreate( _IN const wchar_t* dir )
    {
        if ( wcslen( dir ) <= 1 )
            return false;

        int ret = true;
        wchar_t* currpath = new wchar_t[ ( wcslen( dir ) * 5 ) + 1 ];
#if defined(_XBOX) || defined(_WINDOWS) || defined(_XBO)
#if defined(_XBO)

        if ( path[ 1 ] != ':' )
        {
            char tpath[ MAX_PATH ];
            sprintf( tpath, "%s%s", XBOXONE_TEMP_FOLDER, path );
            path = tpath;
        }

#endif
        BOOL result = CreateDirectoryW( wcurrentpath( currpath, dir ), NULL );

        if ( result == FALSE )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif //defined(_XBOX) || defined(_WINDOWS) || defined(_XBO)
#if defined(_LINUX)
        int32_t result = _wmkdir( wcurrentpath( currpath, dir ) );

        if ( result != 0 )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif //_LINUX
#if defined(_PS3)
        CellFsErrno err;
        err = cellFsMkdir( wcurrentpath( currpath, dir ), CELL_FS_DEFAULT_CREATE_MODE_1 );
        setLastError( err );

        if ( err != CELL_FS_SUCCEEDED )
        {
            if ( err != CELL_FS_EEXIST )
            {
                Pf_ERROR( "DirectoryCreate err" );
            }

            ret = false;
        }

#endif
#if defined(_VITA)
        int result;
        result = sceFiosDirectoryCreateSync( nullptr, wcurrentpath( currpath, dir ) );

        //setLastError( result );
        if ( result != SCE_FIOS_OK )
        {
            Pf_ERROR( "DirectoryCreate err" );
            ret = false;
        }

#endif
#if defined(_PS4)
        int32_t ret = sceKernelMkdir( wcurrentpath( currpath, dir ), SCE_KERNEL_S_IRWU );

        //setLastError( ret );
        if ( ret < SCE_OK )
        {
            if ( ret != SCE_KERNEL_ERROR_EEXIST )
            {
                Pf_ERROR( "DirectoryCreate err" );
            }

            ret = false;
        }

#endif
        SAFE_DELETE_ARRAY( currpath );
        return ret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int DirectoryCreateEx( _IN const char* dir )
    {
        if ( strlen( dir ) <= 1 )
            return false;

        char path_buffer[ _MAX_LDIR ];
        //char drive[ _MAX_DRIVE ];
        //char dir[ _MAX_LDIR ];
        //char fname[ _MAX_LFNAME ];
        //char ext[ _MAX_LEXT ];
        strcpy( path_buffer, dir );
#ifdef _WINDOWS
        //_splitpath( path_buffer, drive, dir, fname, ext );
#else
        //msplitfpath( path_buffer, drive, dir, fname, ext );
#endif
    /// may be path was c:\aa\bb so bb will set in fname
    //char ndir[ _MAX_LDIR ];
        char mdir[ _MAX_LDIR ];
        //memset( ndir, 0, _MAX_LDIR );
        memset( mdir, 0, _MAX_LDIR );

        char ndir[ _MAX_LDIR ];
        //memset( ndir, 0, _MAX_LDIR * sizeof( wchar_t ) );
        memset( ndir, 0, _MAX_LDIR );

        //sprintf( ndir, "%s%s%s", drive, dir, fname );
        char* next_token = NULL;
        char* fp = strtok_rc( path_buffer, "/\\", &next_token );
        while ( fp != NULL )
        {
            sprintf( mdir, "%s/%s", ndir, fp );
            DirectoryCreate( mdir );
            fp = strtok_rc( NULL, "/\\", &next_token );
            strcpy( ndir, mdir );
        }
        return 1;
    }

    int DirectoryCreateEx( _IN const wchar_t* dir )
    {
        if ( wcslen( dir ) <= 1 )
            return false;

        wchar_t path_buffer[ _MAX_LDIR ];
        //wchar_t drive[ _MAX_DRIVE ];
        //wchar_t dir[ _MAX_LDIR ];
        //wchar_t fname[ _MAX_LFNAME ];
        //wchar_t ext[ _MAX_LEXT ];
        wcscpy( path_buffer, dir );
#ifdef _WINDOWS
        //_wsplitpath( path_buffer, drive, dir, fname, ext );
#else
        //wsplitfpath( path_buffer, drive, dir, fname, ext );
#endif
    /// may be path was c:\aa\bb so bb will set in fname
    //wchar_t ndir[ _MAX_LDIR ];
        wchar_t mdir[ _MAX_LDIR ];
        //memset( ndir, 0, _MAX_LDIR * sizeof( wchar_t ) );
        memset( mdir, 0, _MAX_LDIR * sizeof( wchar_t ) );

        wchar_t ndir[ _MAX_LDIR ];
        //memset( ndir, 0, _MAX_LDIR * sizeof( wchar_t ) );
        memset( ndir, 0, _MAX_LDIR * sizeof( wchar_t ) );

        //swprintf( ndir, _MAX_LDIR, L"%s%s", drive, dir );
        wchar_t* next_token = NULL;
        wchar_t* fp = wcstok_rc( path_buffer, L"/\\", &next_token );
        while ( fp != NULL )
        {
            swprintf( mdir, _MAX_LDIR, L"%s/%s", ndir, fp );
            DirectoryCreate( mdir );
            fp = wcstok_rc( NULL, L"/\\", &next_token );
            wcscpy( ndir, mdir );
        }
        return 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool DirectoryCopy( _IN const char* source, _IN const char* target )
    {
#if defined(_WINDOWS)
        char sf[ _MAX_PATH + 1 ];
        char tf[ _MAX_PATH + 1 ];

        memset( sf, 0, _MAX_PATH + 1 );
        memset( tf, 0, _MAX_PATH + 1 );

        sprintf( sf, "%s\\*", source );
        strcpy( tf, target );

        sf[ strlen( sf ) + 1 ] = 0;
        tf[ strlen( tf ) + 1 ] = 0;

        //_tstring wtf = UTF8ToWString(tf);
        //_tstring wsf = UTF8ToWString(sf);

        SHFILEOPSTRUCT s = { 0 };
        s.wFunc = FO_COPY;
        s.pTo = tf;//wtf.c_str();
        s.pFrom = sf;//wsf.c_str();
        s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
        int res = SHFileOperation( &s );
        return res == 0;
#endif
        return false;
    }

    bool DirectoryCopy( _IN const wchar_t* source, _IN const wchar_t* target )
    {
#if defined(_WINDOWS)
        wchar_t sf[ _MAX_PATH + 1 ];
        wchar_t tf[ _MAX_PATH + 1 ];

        memset( sf, 0, _MAX_PATH + 1 );
        memset( tf, 0, _MAX_PATH + 1 );

        swprintf( sf, _MAX_PATH, L"%s\\*", source );
        wcscpy( tf, target );

        sf[ wcslen( sf ) + 1 ] = 0;
        tf[ wcslen( tf ) + 1 ] = 0;

        int len = _MAX_PATH + 1;
        char wtf[ _MAX_PATH + 1 ];
        char wsf[ _MAX_PATH + 1 ];
        UnicodeToUTF8( wtf, (UInt16*)sf, len );
        UnicodeToUTF8( wsf, (UInt16*)tf, len );

        SHFILEOPSTRUCT s = { 0 };
        s.wFunc = FO_COPY;
        s.pTo = wtf;
        s.pFrom = wsf;
        s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
        int res = SHFileOperation( &s );
        return res == 0;
#endif
        return false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int DirectoryDelete( _IN const char* dir )
    {
#ifdef _WINDOWS
        bool            bSubdirectory = false;       // Flag, indicating whether
                                                     // subdirectories have been found
        bool            bDeleteSubdirectories = true;
        HANDLE          hFile;                       // Handle to dir
        _mstring        strFilePath;                 // Filepath
        _mstring        strPattern;                  // Pattern
        WIN32_FIND_DATA FileInformation;             // File information


        strPattern = dir;
        strPattern += "\\*.*";
        hFile = ::FindFirstFileA( strPattern.c_str(), &FileInformation );
        if ( hFile != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( FileInformation.cFileName[ 0 ] != '.' )
                {
                    strFilePath.erase();
                    strFilePath = dir;
                    strFilePath += "\\";
                    strFilePath += FileInformation.cFileName;

                    if ( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    {
                        if ( bDeleteSubdirectories )
                        {
                            // Delete subdirectory
                            int iRC = DirectoryDelete( strFilePath.c_str() );
                            if ( iRC )
                                return iRC;
                        }
                        else
                            bSubdirectory = true;
                    }
                    else
                    {
                        // Set file attributes
                        if ( ::SetFileAttributesA( strFilePath.c_str(),
                            FILE_ATTRIBUTE_NORMAL ) == FALSE )
                            return ::GetLastError();

                        // Delete file
                        if ( ::DeleteFileA( strFilePath.c_str() ) == FALSE )
                            return ::GetLastError();
                    }
                }
            } while ( ::FindNextFile( hFile, &FileInformation ) == TRUE );

            // Close handle
            ::FindClose( hFile );

            DWORD dwError = ::GetLastError();
            if ( dwError != ERROR_NO_MORE_FILES )
                return dwError;
            else
            {
                if ( !bSubdirectory )
                {
                    // Set dir attributes
                    if ( ::SetFileAttributes( dir, FILE_ATTRIBUTE_NORMAL ) == FALSE )
                        return ::GetLastError();

                    // Delete dir
                    if ( ::RemoveDirectoryA( dir ) == FALSE )
                        return ::GetLastError();
                }
            }
        }

#else
        struct dirent *entry = NULL;
        DIR *directory = opendir( dir );
        while ( entry = readdir( directory ) )
        {
            DIR *sub_directory = NULL;
            FILE *file = NULL;
            char abs_path[ 100 ] = { 0 };
            if ( *( entry->d_name ) != '.' )
            {
                sprintf( abs_path, "%s/%s", dir, entry->d_name );
                if ( sub_directory = opendir( abs_path ) )
                {
                    closedir( sub_directory );
                    rmdir( abs_path );
                }
                else
                {
                    if ( file = fopen( abs_path, "r" ) )
                    {
                        fclose( file );
                        remove( abs_path );
                    }
                }
            }
        }
        closedir( directory );
        remove( dir );
#endif
        return 0;
    }

    int DirectoryDelete( _IN const wchar_t* dir )
    {
#ifdef _WINDOWS
        bool             bSubdirectory = false;       // Flag, indicating whether
                                                 // subdirectories have been found
        bool             bDeleteSubdirectories = true;
        HANDLE           hFile;                       // Handle to dir
        _wstring         strFilePath;                 // Filepath
        _wstring         strPattern;                  // Pattern
        WIN32_FIND_DATAW FileInformation;             // File information


        strPattern = dir;
        strPattern += L"\\*.*";
        hFile = ::FindFirstFileW( strPattern.c_str(), &FileInformation );
        if ( hFile != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( FileInformation.cFileName[ 0 ] != L'.' )
                {
                    strFilePath.erase();
                    strFilePath = dir;
                    strFilePath += L"\\";
                    strFilePath += FileInformation.cFileName;

                    if ( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                    {
                        if ( bDeleteSubdirectories )
                        {
                            // Delete subdirectory
                            int iRC = DirectoryDelete( strFilePath.c_str() );
                            if ( iRC )
                                return iRC;
                        }
                        else
                            bSubdirectory = true;
                    }
                    else
                    {
                        // Set file attributes
                        if ( ::SetFileAttributesW( strFilePath.c_str(),
                            FILE_ATTRIBUTE_NORMAL ) == FALSE )
                            return ::GetLastError();

                        // Delete file
                        if ( ::DeleteFileW( strFilePath.c_str() ) == FALSE )
                            return ::GetLastError();
                    }
                }
            } while ( ::FindNextFileW( hFile, &FileInformation ) == TRUE );

            // Close handle
            ::FindClose( hFile );

            DWORD dwError = ::GetLastError();
            if ( dwError != ERROR_NO_MORE_FILES )
                return dwError;
            else
            {
                if ( !bSubdirectory )
                {
                    // Set dir attributes
                    if ( ::SetFileAttributesW( dir, FILE_ATTRIBUTE_NORMAL ) == FALSE )
                        return ::GetLastError();

                    // Delete dir
                    if ( ::RemoveDirectoryW( dir ) == FALSE )
                        return ::GetLastError();
                }
            }
        }
#else
        char path_buff[ 1024 ];
        memset( path_buff, 0, 1024 );
        UnicodeToUTF8( path_buff, (UInt16*)dir, 1024 );
        DirectoryDelete( path_buff );
#endif
        return 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void DirectoryDel( _IN const char* dir )
    {
#if defined(_WINDOWS)
        char path_buff[ 1024 ];
        memset( path_buff, 0, 1024 );
        strcpy( path_buff, dir );

        SHFILEOPSTRUCT file_op =
        {
            NULL,
            FO_DELETE,
            path_buff,
            "",
            FOF_NOCONFIRMATION |
            FOF_NOERRORUI |
            FOF_SILENT,
            false,
            0,
            ""
        };
        SHFileOperation( &file_op );
#endif
        return;
    }

    void DirectoryDel( _IN const wchar_t* dir )
    {
#if defined(_WINDOWS)
        char path_buff[ 1024 ];
        memset( path_buff, 0, 1024 );
        UnicodeToUTF8( path_buff, (UInt16*)dir, 1024 );

        SHFILEOPSTRUCT file_op =
        {
            NULL,
            FO_DELETE,
            path_buff,
            "",
            FOF_NOCONFIRMATION |
            FOF_NOERRORUI |
            FOF_SILENT,
            false,
            0,
            ""
        };
        SHFileOperation( &file_op );
#endif
        return;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t DirectoryGetFileCount( _IN const char* dir )
    {
        int file_count = 0;
        char szDir[ _MAX_PATH ];
#if defined(_WINDOWS)
        WIN32_FIND_DATA ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        sprintf( szDir, ( "%s\\*.*" ), dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFile( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    //char inner_dir[ MAX_PATH ] = { 0 };
                    //sprintf( inner_dir, ( "%s\\%s" ), dir, ffd.cFileName );
                    //file_count += DirectoryGetAllFileCount( inner_dir );
                    //recursive( InnerPath, nCount );
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFile( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
#endif
#if defined(_LINUX)
        DIR * dirp;
        struct dirent * entry;
        dirp = opendir( "path" ); /* There should be error handling after this */
        while ( ( entry = readdir( dirp ) ) != NULL )
        {
            //if ( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 )
            //{
            int nLen = snprintf( szDir, sizeof( szDir ) - 1, "%s/%s", dir , entry->d_name );
            szDir[ nLen ] = '\0';
            // Query file status
            struct stat st = {};
            lstat( szDir, &st );

            // 這是因為 d_type 並不是所有的檔案系統都會支援，
            // 像我看 CentOS 上的 xfs filesystem，目錄和檔案都會是相同的值…
            // 因此只好自己用 lstat() 來查詢了：

            // d_type 
            // DT_BLK      This is a block device.
            // DT_CHR      This is a character device.
            // DT_DIR      This is a dir.
            // DT_FIFO     This is a named pipe( FIFO ).
            // DT_LNK      This is a symbolic link.
            // DT_REG      This is a regular file.
            // DT_SOCK     This is a UNIX domain socket.
            // DT_UNKNOWN  The file type could not be determined.
            if ( S_ISDIR( st.st_mode ) )
            {
                //file_count += DirectoryGetAllFileCount( szPath );
            }
            else if ( S_ISREG( st.st_mode ) )
            {
                file_count++;
            }
            //}
        }
        closedir( dirp );
#endif
        return file_count;
    }

    uint_t DirectoryGetFileCount( _IN const wchar_t* dir )
    {
        int file_count = 0;
#if defined(_WINDOWS)
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        wchar_t szDir[ _MAX_PATH ];
        swprintf( szDir, L"%s\\*.*", dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFileW( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    //char inner_dir[ MAX_PATH ] = { 0 };
                    //sprintf( inner_dir, ( "%s\\%s" ), dir, ffd.cFileName );
                    //file_count += DirectoryGetAllFileCount( inner_dir );
                    //recursive( InnerPath, nCount );
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFileW( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
#endif
#if defined(_LINUX)
        char szDir[ _MAX_PATH ];
        wcstombs( szDir, dir, wcslen( dir ) );
        file_count += DirectoryGetAllFileCount( szDir );
#endif
        return file_count;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t DirectoryGetAllFileCount( _IN const char* dir )
    {
        int file_count = 0;
        char szDir[ _MAX_PATH ];
#if defined(_WINDOWS)
        WIN32_FIND_DATA ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        sprintf( szDir, ( "%s\\*.*" ), dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFile( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    if ( ffd.cFileName[ 0 ] != '.' && ffd.cFileName[ 0 ] != '..' )
                    {
                        char inner_dir[ MAX_PATH ] = { 0 };
                        sprintf( inner_dir, ( "%s\\%s" ), dir, ffd.cFileName );
                        file_count += DirectoryGetAllFileCount( inner_dir );
                        //recursive( InnerPath, nCount );
                    }
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFile( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
#endif
#if defined(_LINUX)
        DIR * dirp;
        struct dirent * entry;
        dirp = opendir( "path" ); /* There should be error handling after this */
        while ( ( entry = readdir( dirp ) ) != NULL )
        {
            int nLen = snprintf( szDir, sizeof( szDir ) - 1, "%s/%s", dir, entry->d_name );
            szDir[ nLen ] = '\0';
            // Query file status
            struct stat st = {};
            lstat( szDir, &st );

            // 這是因為 d_type 並不是所有的檔案系統都會支援，
            // 像我看 CentOS 上的 xfs filesystem，目錄和檔案都會是相同的值…
            // 因此只好自己用 lstat() 來查詢了：

            // d_type 
            // DT_BLK      This is a block device.
            // DT_CHR      This is a character device.
            // DT_DIR      This is a dir.
            // DT_FIFO     This is a named pipe( FIFO ).
            // DT_LNK      This is a symbolic link.
            // DT_REG      This is a regular file.
            // DT_SOCK     This is a UNIX domain socket.
            // DT_UNKNOWN  The file type could not be determined.
            if ( S_ISDIR( st.st_mode ) )
            {
                if ( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 )
                {
                    file_count += DirectoryGetAllFileCount( szDir );
                }
            }
            else if ( S_ISREG( st.st_mode ) )
            {
                file_count++;
            }
        }
        closedir( dirp );
#endif
        return file_count;
    }

    uint_t DirectoryGetAllFileCount( _IN const wchar_t* dir )
    {
        int file_count = 0;
#if defined(_WINDOWS)
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        wchar_t szDir[ MAX_PATH ];
        swprintf( szDir, L"%s\\*.*", dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFileW( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    if ( ffd.cFileName[ 0 ] != '.' && ffd.cFileName[ 0 ] != '..' )
                    {
                        wchar_t inner_dir[ MAX_PATH ] = { 0 };
                        swprintf( inner_dir, L"%s\\%s", dir, ffd.cFileName );
                        file_count += DirectoryGetAllFileCount( inner_dir );
                        //recursive( InnerPath, nCount );
                    }
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFileW( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
#endif
#if defined(_LINUX)
        char szDir[ _MAX_PATH ];
        wcstombs( szDir, dir, wcslen( dir ) );
        file_count += DirectoryGetAllFileCount( szDir );
#endif
        return file_count;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t DirectoryDeleteEmpty( _IN const char* dir )
    {
        int file_count = 0;
        char szDir[ _MAX_PATH ];
#if defined(_WINDOWS)
        WIN32_FIND_DATA ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        sprintf( szDir, ( "%s\\*.*" ), dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFile( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    if ( ffd.cFileName[ 0 ] != '.' && ffd.cFileName[ 0 ] != '..' )
                    {
                        char inner_dir[ MAX_PATH ] = { 0 };
                        sprintf( inner_dir, ( "%s\\%s" ), dir, ffd.cFileName );
                        file_count += DirectoryDeleteEmpty( inner_dir );
                        //recursive( InnerPath, nCount );
                    }
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFile( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
#endif
#if defined(_LINUX)
        DIR * dirp;
        struct dirent * entry;
        dirp = opendir( "path" ); /* There should be error handling after this */
        while ( ( entry = readdir( dirp ) ) != NULL )
        {
            int nLen = snprintf( szDir, sizeof( szDir ) - 1, "%s/%s", dir, entry->d_name );
            szDir[ nLen ] = '\0';
            // Query file status
            struct stat st = {};
            lstat( szDir, &st );

            // 這是因為 d_type 並不是所有的檔案系統都會支援，
            // 像我看 CentOS 上的 xfs filesystem，目錄和檔案都會是相同的值…
            // 因此只好自己用 lstat() 來查詢了：

            // d_type 
            // DT_BLK      This is a block device.
            // DT_CHR      This is a character device.
            // DT_DIR      This is a dir.
            // DT_FIFO     This is a named pipe( FIFO ).
            // DT_LNK      This is a symbolic link.
            // DT_REG      This is a regular file.
            // DT_SOCK     This is a UNIX domain socket.
            // DT_UNKNOWN  The file type could not be determined.
            if ( S_ISDIR( st.st_mode ) )
            {
                if ( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, ".." ) != 0 )
                {
                    file_count += DirectoryDeleteEmpty( szDir );
                }
            }
            else if ( S_ISREG( st.st_mode ) )
            {
                file_count++;
            }
        }
        closedir( dirp );
#endif
        if ( file_count == 0 )
        {
            DirectoryDelete( dir );
        }
        return file_count;
    }

    uint_t DirectoryDeleteEmpty( _IN const wchar_t* dir )
    {
        int file_count = 0;
#if defined(_WINDOWS)
        WIN32_FIND_DATAW ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        wchar_t szDir[ _MAX_PATH ];
        swprintf( szDir, L"%s\\*.*", dir );
        /// Find the first file in the dir. 
        hFind = FindFirstFileW( szDir, &ffd );
        if ( hFind )
        {
            /// List all the files in the dir with some info about them.
            do
            {
                // this a folder
                if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                {
                    if ( ffd.cFileName[ 0 ] != '.' && ffd.cFileName[ 0 ] != '..' )
                    {
                        wchar_t inner_dir[ MAX_PATH ] = { 0 };
                        swprintf( inner_dir, L"%s\\%s", dir, ffd.cFileName );
                        file_count += DirectoryDeleteEmpty( inner_dir );
                        //recursive( InnerPath, nCount );
                    }
                }
                else
                {
                    file_count++;
                }
            } while ( FindNextFileW( hFind, &ffd ) != 0 );
        }
        FindClose( hFind );
        if ( file_count == 0 )
        {
            DirectoryDelete( dir );
        }
#endif
#if defined(_LINUX)
        char szDir[ _MAX_PATH ];
        wcstombs( szDir, dir, wcslen( dir ) );
        file_count += DirectoryDeleteEmpty( szDir );
#endif
        return file_count;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    size32_t DirectoryScanM( _IN const char* root, _IN OnFileM fcallback, _IN void* pData, _IN int depth = 0 );
    size32_t DirectoryScanW( _IN const wchar_t* root, _IN OnFileW fcallback, _IN void* pData, _IN int depth = 0 );

    size32_t DirectoryScanM( _IN const char* root, _IN OnFileM fcallback, _IN  void* pData, _IN int depth )
    {
        //char drive[_MAX_DRIVE];
        //char dir[_MAX_LDIR];
        //char fname[_MAX_LFNAME];
        //char ext[_MAX_LEXT];
        //_splitpath(srcfname, drive, dir, fname, ext);
        //XXX 會有一個最後面的 /   c:\AAA\BBB\CCC\  =>最後一個不行
        char  scanfolder[ _MAX_LPATH ];
        strcpy( scanfolder, root );

        if ( ( scanfolder[ strlen( scanfolder ) - 1 ] == '/' ) ||
            ( scanfolder[ strlen( scanfolder ) - 1 ] == '\\' ) )
        {
            scanfolder[ strlen( scanfolder ) - 1 ] = 0;
        }

        int count_ = 0;
#ifdef _WINDOWS
        struct _finddata_t c_file;
        intptr_t hFile;
        char scanpath[ _MAX_LPATH ];

        if ( ( hFile = _findfirst( scanfolder, &c_file ) ) == -1L )
        {
            printf( "No %s files in current dir!\n", root );
            goto EXIT;
        }
        else
        {
            if ( c_file.attrib  & _A_SUBDIR )
            {
                _findclose( hFile );
                sprintf( scanpath, "%s/%s", scanfolder, "*.*" );

                if ( ( hFile = _findfirst( scanpath, &c_file ) ) == -1L )
                {
                    printf( "No *.* files in current dir!\n" );
                    goto EXIT;
                }

                depth++;
            }
            else
            {
                fcallback( scanfolder, depth, pData );
                count_++;
                goto EXIT;
            }

            /* Find the rest of the .c files */
            while ( _findnext( hFile, &c_file ) == 0 )
            {
                if ( ( strcmp( c_file.name, ".." ) == 0 ) ||
                    ( strcmp( c_file.name, "." ) == 0 ) )
                {
                    continue;
                }

                //it is a folder
                if ( c_file.attrib  & _A_SUBDIR )
                {
                    char IOFilepathS[ _MAX_LPATH ];
                    sprintf( IOFilepathS, "%s/%s", scanfolder, c_file.name );
                    char IOFilepath[ _MAX_LPATH ];
                    mcurrentpath( IOFilepath, IOFilepathS );
                    count_ = count_ + DirectoryScanM( IOFilepath, fcallback, pData, depth );
                    continue;
                }
                else//it is a file
                {
                    count_++;
                    char IOFilepathS[ _MAX_LPATH ];
                    sprintf( IOFilepathS, "%s/%s", scanfolder, c_file.name );
                    char IOFilepath[ _MAX_LPATH ];
                    mcurrentpath( IOFilepath, IOFilepathS );

                    if ( !fcallback( IOFilepath, depth, pData ) )
                    {
                        goto EXIT;
                    }
                }
            }
        }

EXIT:
        _findclose( hFile );
#else
        struct dirent* stFiles;
        DIR* stDirIn;

        if ( ( stDirIn = opendir( scanfolder ) ) == NULL )
        {
            printf( "open dir failure!\n", root );
            goto EXIT;
        }

        while ( ( stFiles = readdir( stDirIn ) ) != NULL )
        {
            char szFullName[ 512 ];
            sprintf( szFullName, "%s/%s", scanfolder, stFiles->d_name );
            struct stat stFileInfo;

            if ( lstat( szFullName, &stFileInfo ) < 0 )
            {
                perror( szFullName );
            }

            /* is the file a dir? */
            if ( S_ISDIR( stFileInfo.st_mode ) )
            {
                //printf("Directory: %s\n", szFullName);
                count_ += DirectoryScanM( szFullName, fcallback, pData, depth + 1 );
            }
            else
            {
                //printf("Filename: %s\n", szFullName);
                count_++;

                if ( !fcallback( szFullName, depth, pData ) )
                {
                    goto EXIT;
                }
            }
        }  // end while

EXIT:
        closedir( stDirIn );
#endif
        return count_;
    }

    size32_t DirectoryScanW( _IN const wchar_t* root, _IN OnFileW fcallback, _IN void* pData, _IN int depth )
    {
        //XXX 會有一個最後面的 /   c:\AAA\BBB\CCC\  =>最後一個不行
        wchar_t  scanfolder[ _MAX_LPATH ];
        wcscpy( scanfolder, root );

        if ( ( scanfolder[ wcslen( scanfolder ) - 1 ] == L'/' ) || ( scanfolder[ wcslen( scanfolder ) - 1 ] == L'\\' ) )
        {
            scanfolder[ wcslen( scanfolder ) - 1 ] = 0;
        }

        int count_ = 0;
#ifdef _WINDOWS
        struct _wfinddata_t c_file;
        intptr_t hFile;
        wchar_t scanpath[ _MAX_LPATH ];

        if ( ( hFile = _wfindfirst( scanfolder, &c_file ) ) == -1L )
        {
            wprintf( L"No %s files in current dir!\n", root );
            goto EXIT;
        }
        else
        {
            if ( c_file.attrib  & _A_SUBDIR )
            {
                _findclose( hFile );
                swprintf( scanpath, _MAX_LPATH, L"%s/%s", scanfolder, L"*.*" );

                if ( ( hFile = _wfindfirst( scanpath, &c_file ) ) == -1L )
                {
                    wprintf( L"No *.* files in current dir!\n" );
                    goto EXIT;
                }

                depth++;
            }
            else
            {
                fcallback( scanfolder, depth, pData );
                count_++;
                goto EXIT;
            }

            /* Find the rest of the .c files */
            while ( _wfindnext( hFile, &c_file ) == 0 )
            {
                if ( ( wcscmp( c_file.name, L".." ) == 0 ) || ( wcscmp( c_file.name, L"." ) == 0 ) )
                {
                    continue;
                }

                //it is a folder
                if ( c_file.attrib  & _A_SUBDIR )
                {
                    wchar_t IOFilepathS[ _MAX_LPATH ];
                    swprintf( IOFilepathS, _MAX_LPATH, L"%s/%s", scanfolder, c_file.name );
                    wchar_t IOFilepath[ _MAX_LPATH ];
                    wcurrentpath( IOFilepath, IOFilepathS );
                    count_ = count_ + DirectoryScanW( IOFilepath, fcallback, pData, depth );
                    continue;
                }
                else//it is a file
                {
                    count_++;
                    wchar_t IOFilepathS[ _MAX_LPATH ];
                    swprintf( IOFilepathS, _MAX_LPATH, L"%s/%s", scanfolder, c_file.name );
                    wchar_t IOFilepath[ _MAX_LPATH ];
                    wcurrentpath( IOFilepath, IOFilepathS );

                    if ( !fcallback( IOFilepath, depth, pData ) )
                    {
                        goto EXIT;
                    }
                }
            }
        }

EXIT:
        _findclose( hFile );
#else
        char_t* scanfolderA = NULL;
        size_t   n = wcslen( scanfolder );
        scanfolderA = new char[ n + 1 ];
        wcstombs( scanfolderA, scanfolder, n );
        struct dirent* stFiles;
        DIR* stDirIn;

        if ( ( stDirIn = opendir( scanfolderA ) ) == NULL )
        {
            printf( "open dir failure!\n", root );
            goto EXIT;
        }

        while ( ( stFiles = readdir( stDirIn ) ) != NULL )
        {
            char szFullName[ 512 ];
            sprintf( szFullName, "%s/%s", scanfolder, stFiles->d_name );
            wchar_t* szFullNameW = NULL;
            size_t  fn = strlen( szFullName );
            szFullNameW = new wchar_t[ fn + 1 ];
            mbstowcs( szFullNameW, szFullName, fn );
            struct stat stFileInfo;

            if ( lstat( szFullName, &stFileInfo ) < 0 )
            {
                perror( szFullName );
            }

            /* is the file a dir? */
            if ( S_ISDIR( stFileInfo.st_mode ) )
            {
                //printf("Directory: %s\n", szFullName);
                count_ += DirectoryScanW( szFullNameW, fcallback, pData, depth + 1 );
            }
            else
            {
                //printf("Filename: %s\n", szFullName);
                count_++;

                if ( !fcallback( szFullNameW, depth, pData ) )
                {
                    goto EXIT;
                }
            }

            SAFE_DELETE_ARRAY( szFullNameW );
        }  // end while

EXIT:
        closedir( stDirIn );
        SAFE_DELETE_ARRAY( scanfolderA );
#endif
        return count_;
    }

    size32_t DirectoryScan( _IN const char* root, _IN OnFileM fcallback, _IN void* pData )
    {
        return DirectoryScanM( root, fcallback, pData );
    }

    size32_t DirectoryScan( _IN const wchar_t* root, _IN OnFileW fcallback, _IN void* pData )
    {
        return DirectoryScanW( root, fcallback, pData );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int CopyFileF2F( _IN FILE* nf, _IN FILE* of )
    {
        if ( ( nf == NULL ) || ( of == NULL ) )
            return -1;

        size_t nread = (size_t)filelength( of );

        if ( nread > 0 )
        {
            char* block = new char[ nread + 1 ];

            if ( fread( block, nread, 1, of ) > 0 )
            {
                fwrite( block, nread, 1, nf );
            }

            delete[] block;
        }
        return 1;
    }

    const char* FileCopy( _IN const char* exist_fname, _INOUT char* new_fname, _IN int bufflen, _IN copy_method method )
    {
        char real_fname[ _MAX_LPATH ];
        strcpy( real_fname, new_fname );

        char drive[ _MAX_DRIVE ];
        char dir[ _MAX_DIR ];
        char fname[ _MAX_FNAME ];
        char ext[ _MAX_EXT ];
        _splitpath( new_fname, drive, dir, fname, ext ); // C4996    

        if ( FileExist( new_fname ) )
        {
            switch ( method )
            {
            case copy_method_fail:
                {
                    return new_fname;
                }
                break;
            case copy_method_rename:
                {
                    int ii = 1;
                    do
                    {
                        sprintf( real_fname, "%s%s%s(%d)%s", drive, dir, fname, ii, ext );
                        ii++;
                    } while ( FileExist( real_fname ) );
                }
                break;
            case copy_method_force:
            default:
                break;
            }
        }

        // 強制建立 新檔案的資料夾
        {
            char ndir[ _MAX_LDIR ];
            sprintf( ndir, "%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

        int fret = -1;
        {
            FILE* nf = fopen( real_fname, "w+b" );
            FILE* of = fopen( exist_fname, "rb" );

            fret = CopyFileF2F( nf, of );

            if ( nf != NULL )
            {
                fclose( nf );
            }
            if ( of != NULL )
            {
                fclose( of );
            }
        }

        if ( fret != 1 )
        {
            return NULL;
        }

        strncpy( new_fname, real_fname, __min( bufflen - 1, strlen( real_fname ) ) );
        return new_fname;
    }

    const wchar_t* FileCopy( _IN const wchar_t* exist_fname, _INOUT wchar_t* new_fname, _IN int bufflen, _IN copy_method method )
    {
        wchar_t real_fname[ _MAX_LPATH ];
        wcscpy( real_fname, new_fname );

        wchar_t drive[ _MAX_DRIVE ];
        wchar_t dir[ _MAX_DIR ];
        wchar_t fname[ _MAX_FNAME ];
        wchar_t ext[ _MAX_EXT ];
        _wsplitpath( new_fname, drive, dir, fname, ext ); // C4996    

        if ( FileExist( new_fname ) )
        {
            switch ( method )
            {
            case copy_method_fail:
                {
                    return new_fname;
                }
                break;
            case copy_method_rename:
                {
                    int ii = 1;
                    do
                    {
                        swprintf( real_fname, _MAX_LPATH, L"%s%s%s(%d)%s", drive, dir, fname, ii, ext );
                        ii++;
                    } while ( FileExist( real_fname ) );
                }
                break;
            default:
                {
                    return NULL;                
                }
                break;                    
            }
        }

        // 強制建立 新檔案的資料夾
        {
            wchar_t ndir[ _MAX_LDIR ];
            swprintf( ndir, _MAX_LDIR, L"%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

        int fret = -1;
        {
            FILE* nf = _wfopen( real_fname, L"w+b" );
            FILE* of = _wfopen( exist_fname, L"rb" );

            fret = CopyFileF2F( nf, of );

            if ( nf != NULL )
            {
                fclose( nf );
            }
            if ( of != NULL )
            {
                fclose( of );
            }
        }

        if ( fret != 1 )
        {
            return NULL;
        }

        wcsncpy( new_fname, real_fname, __min( bufflen - 1, wcslen( real_fname ) ) );
        return new_fname;
    }

    int FileCopy( _IN const char* exist_fname, _IN const char* new_fname, _IN bool fail_if_exist, _IN bool force_create_dir )
    {
        //int in, out;
        if ( fail_if_exist )
        {
            if ( FileExist( new_fname ) )
            {
                return -2;
            }
        }

        if ( force_create_dir )
        {
            char path_buffer[ _MAX_LDIR ];
            char drive[ _MAX_DRIVE ];
            char dir[ _MAX_LDIR ];
            char fname[ _MAX_LFNAME ];
            char ext[ _MAX_LEXT ];
            strcpy( path_buffer, new_fname );
#ifdef _WINDOWS
            _splitpath( path_buffer, drive, dir, fname, ext );
#else
            msplitfpath( path_buffer, drive, dir, fname, ext );
#endif
            char ndir[ _MAX_LDIR ];
            sprintf( ndir, "%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

        int fret = -1;
        {
            FILE* nf = fopen( new_fname, "w+b" );
            FILE* of = fopen( exist_fname, "rb" );

            fret = CopyFileF2F( nf, of );

            if ( nf != NULL )
            {
                fclose( nf );
            }
            if ( of != NULL )
            {
                fclose( of );
            }
        }

        return fret;
    }

    int FileCopy( _IN const wchar_t* exist_fname, _IN const wchar_t* new_fname, _IN bool fail_if_exist, _IN bool force_create_dir )
    {
        //int in, out;
        if ( fail_if_exist )
        {
            if ( FileExist( new_fname ) )
            {
                return -2;
            }
        }

        if ( force_create_dir )
        {
            wchar_t path_buffer[ _MAX_LDIR ];
            wchar_t drive[ _MAX_DRIVE ];
            wchar_t dir[ _MAX_LDIR ];
            wchar_t fname[ _MAX_LFNAME ];
            wchar_t ext[ _MAX_LEXT ];
            wcscpy( path_buffer, new_fname );
#ifdef _WINDOWS
            _wsplitpath( path_buffer, drive, dir, fname, ext );
#else
            wsplitfpath( path_buffer, drive, dir, fname, ext );
#endif
            wchar_t ndir[ _MAX_LDIR ];
            swprintf( ndir, _MAX_LDIR, L"%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

        int fret = -1;
        {
            FILE* nf = _wfopen( new_fname, L"w+b" );
            FILE* of = _wfopen( exist_fname, L"rb" );

            fret = CopyFileF2F( nf, of );

            if ( nf != NULL )
            {
                fclose( nf );
            }
            if ( of != NULL )
            {
                fclose( of );
            }
        }

        return fret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int SymLink( _IN const char* lpszPathObj, _IN const char* lpszPathLink, _IN const char* lpszDesc, _IN bool force )
    {
        if ( force )
        {
            char path_buffer[ _MAX_LDIR ];
            char drive[ _MAX_DRIVE ];
            char dir[ _MAX_LDIR ];
            char fname[ _MAX_LFNAME ];
            char ext[ _MAX_LEXT ];
            strcpy( path_buffer, lpszPathLink );
#ifdef _WINDOWS
            _splitpath( path_buffer, drive, dir, fname, ext );
#else
            msplitfpath( path_buffer, drive, dir, fname, ext );
#endif
            char ndir[ _MAX_LDIR ];
            sprintf( ndir, "%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

#if defined(_WINDOWS)
        //char commandA[256];
        //sprintf(commandA, "%s %s", lpszPathLink, lpszPathObj);
        //::ShellExecute(NULL, "mklink", commandA, NULL, NULL, SW_HIDE);
        //return 1;
        CoInitialize( NULL );
        HRESULT hres;
        IShellLink* psl;
        // Get a pointer to the IShellLink interface.
        hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl );

        if ( SUCCEEDED( hres ) )
        {
            IPersistFile* ppf;
            // Set the path to the shortcut target, and add the
            // description.
            char dirbuffA[ _MAX_LDIR ];
            char dirbuffB[ _MAX_LDIR ];
            GetCurrentDirectory( _MAX_LDIR, dirbuffA );
            sprintf( dirbuffB, "%s\\%s", dirbuffA, lpszPathObj );
            //mcurrentpath(dirbuffA, dirbuffB);
            HRESULT ret = psl->SetPath( dirbuffB );
            psl->SetDescription( lpszDesc );
            psl->SetShowCmd( SW_SHOW );
            // Query IShellLink for the IPersistFile interface for saving the
            // shortcut in persistent storage.
            hres = psl->QueryInterface( IID_IPersistFile, (void**)&ppf );

            if ( SUCCEEDED( hres ) )
            {
                WCHAR wsz[ _MAX_LPATH ];
                // Ensure that the string is ANSI.
                MultiByteToWideChar( CP_ACP, 0, lpszPathLink, -1, wsz, _MAX_LPATH );
                // Save the link by calling IPersistFile::Save.
                hres = ppf->Save( wsz, TRUE );
                ppf->Release();
            }

            psl->Release();
        }

        CoUninitialize();
        return hres;
#endif
#if defined(_LINUX)
        symlink( lpszPathObj, lpszPathLink );
#endif
        return -1;
    }

    int SymLink( _IN const wchar_t* lpszPathObj, _IN const wchar_t* lpszPathLink, _IN const wchar_t* lpszDesc, _IN bool force )
    {
        if ( force )
        {
            wchar_t path_buffer[ _MAX_LDIR ];
            wchar_t drive[ _MAX_DRIVE ];
            wchar_t dir[ _MAX_LDIR ];
            wchar_t fname[ _MAX_LFNAME ];
            wchar_t ext[ _MAX_LEXT ];
            wcscpy( path_buffer, lpszPathLink );
#ifdef _WINDOWS
            _wsplitpath( path_buffer, drive, dir, fname, ext );
#else
            wsplitfpath( path_buffer, drive, dir, fname, ext );
#endif
            wchar_t ndir[ _MAX_LDIR ];
            swprintf( ndir, _MAX_LDIR, L"%s%s", drive, dir );
            DirectoryCreateEx( ndir );
        }

#if defined(_WINDOWS)
        //char commandA[256];
        //sprintf(commandA, "%s %s", lpszPathLink, lpszPathObj);
        //::ShellExecute(NULL, "mklink", commandA, NULL, NULL, SW_HIDE);
        //return 1;
        CoInitialize( NULL );
        HRESULT hres;
        IShellLinkW* psl;
        // Get a pointer to the IShellLink interface.
        hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl );

        if ( SUCCEEDED( hres ) )
        {
            IPersistFile* ppf;
            // Set the path to the shortcut target, and add the
            // description.
            wchar_t dirbuffA[ _MAX_LDIR ];
            wchar_t dirbuffB[ _MAX_LDIR ];
            GetCurrentDirectoryW( _MAX_LDIR, dirbuffA );
            swprintf( dirbuffB, L"%s\\%s", dirbuffA, lpszPathObj );
            //mcurrentpath(dirbuffA, dirbuffB);
            HRESULT ret = psl->SetPath( dirbuffB );
            psl->SetDescription( lpszDesc );
            psl->SetShowCmd( SW_SHOW );
            // Query IShellLink for the IPersistFile interface for saving the
            // shortcut in persistent storage.
            hres = psl->QueryInterface( IID_IPersistFile, (void**)&ppf );

            if ( SUCCEEDED( hres ) )
            {
                WCHAR wsz[ _MAX_LPATH ];
                // Ensure that the string is ANSI.
                //MultiByteToWideChar( CP_ACP, 0, lpszPathLink, -1, wsz, _MAX_LPATH );
                // Save the link by calling IPersistFile::Save.
                hres = ppf->Save( wsz, TRUE );
                ppf->Release();
            }

            psl->Release();
        }

        CoUninitialize();
        return hres;
#endif
#if defined(_LINUX)

        size_t len_f = wcslen( lpszPathObj );
        size_t len_m = wcslen( lpszPathLink );
        char* utf = new char[ len_f + 1 ];
        char* utm = new char[ len_m + 1 ];;
        UnicodeToUTF8( utf, (UInt16*)lpszPathObj, (int)len_f );
        UnicodeToUTF8( utm, (UInt16*)lpszPathLink, (int)len_m );

        symlink( utf, utm );

        delete[] utf;
        delete[] utm;

#endif
        return -1;
    }

    //=============================================================================
    // NAME:
    // DESC: Determine if a file exists
    //=============================================================================
    int FileExist  // returns NZ if file exists, 0 if it does not
    (
        _IN const char* name // ASCII name of file
    )
    {
        // Validate parameters
        assert( name != NULL );
        assert( *name != 0 );

        if ( strcmp( name, "" ) == 0 )
        {
            return 0;
        }

        // Check for file existence by opening and closing the file
        FILE* stream = fopen( name, "r" );

        if ( stream )
        {
            fclose( stream );
        }

        // Return NZ if file exists, 0 if it doesn't
        return ( stream != 0 );
    }

    int FileExist  // returns NZ if file exists, 0 if it does not
    (
        _IN const wchar_t* name // ASCII name of file
    )
    {
        // Validate parameters
        assert( name != NULL );
        assert( *name != 0 );
        // Check for file existence by opening and closing th1e file
        FILE* stream = _wfopen( name, L"r" );

        if ( stream )
        {
            fclose( stream );
        }

        // Return NZ if file exists, 0 if it doesn't
        return ( stream != 0 );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool IsDirectory( _IN const char* path )
    {
#if defined(_WINDOWS) || defined(_XBOX)
        struct _stat s;

        if ( _stat( path, &s ) == 0 )
        {
            if ( s.st_mode & S_IFDIR )
            {
                //it's a dir
                return true;
            }

            //else if( s.st_mode & S_IFREG )
            //{
            // //it's a file
            //}
            //else
            //{
            // //something else
            //}
        }

#endif
#if defined(_LINUX)
        struct stat s;

        if ( stat( path, &s ) == 0 )
        {
            if ( s.st_mode & S_IFDIR )
            {
                //it's a dir
                return true;
            }
        }

#endif
        return false;
    }

    bool IsDirectory( _IN const wchar_t* path )
    {
#if defined(_WINDOWS) || defined(_XBOX)
        struct _stat s;

        if ( _wstat( path, &s ) == 0 )
        {
            if ( s.st_mode & S_IFDIR )
            {
                //it's a dir
                return true;
            }

            //else if( s.st_mode & S_IFREG )
            //{
            // //it's a file
            //}
            //else
            //{
            // //something else
            //}
        }

#endif
#if defined(_LINUX)

        size_t len_f = wcslen( path );
        char* utf = new char[ len_f + 1 ];
        UnicodeToUTF8( utf, (UInt16*)path, (int)len_f );

        struct stat s;
        if ( stat( utf, &s ) == 0 )
        {
            if ( s.st_mode & S_IFDIR )
            {

                //it's a dir
                delete[] utf;
                return true;
            }
        }

        delete[] utf;
#endif
        return false;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int FileCmp( _IN const char* srcfname, _IN const char* new_fname )
    {
        FILE* sf = fopen( srcfname, "rb" );

        if ( sf == NULL )
        {
            return -1;
        }

        FILE* df = fopen( new_fname, "rb" );

        if ( df == NULL )
        {
            fclose( sf );
            return 1;
        }

        size_t sl = (size_t)filelength( sf );
        size_t dl = (size_t)filelength( df );

        if ( ( sl == 0 ) || ( dl == 0 ) )
        {
            return -3;
        }

        char* sbuf = new char[ sl + 1 ];
        fread( sbuf, 1, sl, sf );
        char* dbuf = new char[ dl + 1 ];
        fread( dbuf, 1, dl, df );
        int ret = 0;

        if ( sl != dl )
        {
            ret = -2;
        }
        else
        {
            ret = memcmp( sbuf, dbuf, sl );
        }

        fclose( sf );
        fclose( df );
        return ret;
    }

    //=============================================================================
    // NAME: PfGetAbsolutePath
    // DESC:
    //=============================================================================
    size32_t GetAbsolutePath( _OUT char* outs, _IN int outl, _IN const char* ins )
    {
#if defined(_WINDOWS)
        return GetFullPathNameA( ins, outl, outs, NULL );
#endif
#if defined(_LINUX)

        if ( realpath( ins, outs ) )
        {
            return (size32_t)strlen( outs );
        }

#endif
        return 0;
    }

    size32_t GetAbsolutePath( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins )
    {
#if defined(_WINDOWS)
        return GetFullPathNameW( ins, outl, outs, NULL );
#endif
#if defined(_LINUX)

        if ( _wrealpath( ins, outs ) )
        {
            return (size32_t)wcslen( outs );
        }

#endif
        return 0;
    }

    //=============================================================================
    // NAME: PfGetRelativePath
    // DESC:
    //=============================================================================
    int GetRelativePath( _OUT char* outs, _IN int outl, _IN const char* ins, _IN const char* cps )
    {
        memset( outs, 0, outl );
        size32_t len01 = (size32_t)strlen( ins );
        size32_t len02 = (size32_t)strlen( cps );
        int sidx = 0;

        for ( size32_t i = 0; i < __min( len01, len02 ); i++ )
        {
            if ( ins[ i ] != cps[ i ] )
            {
                sidx = i;
                break;
            }
        }

        int uplevcnt = strchrcount( cps + sidx, '/' ) + strchrcount( cps + sidx, '\\' );

        if ( uplevcnt <= 0 )
        {
            strncpy( outs, ins + sidx, outl );
        }
        else
        {
            for ( int i = 0; i <= uplevcnt; i++ )
            {
                strncat( outs, "../", outl - ( (size32_t)strlen( outs ) ) );
            }
            strncat( outs, ins + sidx, outl - ( (size32_t)strlen( outs ) ) );
        }
        return 1;
    }

    int GetRelativePath( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* cps )
    {
        memset( outs, 0, outl * 2 );
        int len01 = (int)wcslen( ins );
        int len02 = (int)wcslen( cps );
        int sidx = 0;

        for ( int i = 0; i < __min( len01, len02 ); i++ )
        {
            if ( ins[ i ] != cps[ i ] )
            {
                sidx = i;
                break;
            }
        }

        int uplevcnt = wcschrcount( cps + sidx, L'/' ) + wcschrcount( cps + sidx, L'\\' );

        if ( uplevcnt <= 0 )
        {
            wcsncpy( outs, ins + sidx, outl );
        }
        else
        {
            for ( int i = 0; i <= uplevcnt; i++ )
            {
                wcsncat( outs, L"../", outl - wcslen( outs ) );
            }
            wcsncat( outs, ins + sidx, outl - wcslen( outs ) );
        }
        return 1;
    }
}
