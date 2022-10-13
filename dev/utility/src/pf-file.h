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
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


namespace pfsfl  // stand file library
{
    //////////////////////////////////////////////////////////////////////////
    //
    // Non-ANSI functions
    // 建立資料夾 (必須是在有上層的資料夾狀況下)
    int             DirectoryCreate( _IN const char*    dir );
    int             DirectoryCreate( _IN const wchar_t* dir );

    // create folder in path if upper folder not exist it will create first
    int             DirectoryCreateEx( _IN const char*    dir );
    int             DirectoryCreateEx( _IN const wchar_t* dir );

    // 複製資料夾
    bool            DirectoryCopy( _IN const char*    source, _IN const char*    target );
    bool            DirectoryCopy( _IN const wchar_t* source, _IN const wchar_t* target );

    // 刪除資料夾
    int             DirectoryDelete( _IN const char*    dir );
    int             DirectoryDelete( _IN const wchar_t* dir );

    void            DirectoryDel( _IN const char* dir );
    void            DirectoryDel( _IN const wchar_t* dir );


    // 取得資料夾底下的檔案數 (資料夾內 不包含內部的資料夾)
    uint_t          DirectoryGetFileCount( _IN const char*    dir );
    uint_t          DirectoryGetFileCount( _IN const wchar_t* dir );

    // 取得資料夾底下的檔案數 (加上底下資料夾嫩的所有檔案數)
    uint_t          DirectoryGetAllFileCount( _IN const char*    dir );
    uint_t          DirectoryGetAllFileCount( _IN const wchar_t* dir );

    // 將資料夾內 空的資料夾全部刪除
    uint_t          DirectoryDeleteEmpty( _IN const char*    dir );
    uint_t          DirectoryDeleteEmpty( _IN const wchar_t* dir );


    // 掃描資料夾
    typedef bool    ( _CALLBACK *OnFileM )  ( const char*    file, int depth, void* pData );
    typedef bool    ( _CALLBACK *OnFileW )  ( const wchar_t* file, int depth, void* pData );
    size32_t        DirectoryScan( _IN const char*    root, _IN OnFileM fcallback, _IN void* pData = NULL );
    size32_t        DirectoryScan( _IN const wchar_t* root, _IN OnFileW fcallback, _IN void* pData = NULL );

    //copyfile
    enum copy_method
    {
        copy_method_force,   // 如果檔案存在 就強制覆蓋
        copy_method_fail,    // 如果檔案存在 就失敗
        copy_method_rename   // 如果檔案存在 就改名 (0) (1) (2)......
    };
    // 如果有重就會改名 然後將新檔名放到new_fname並回傳
    // bufflen => length of new_fname
    const char*     FileCopy( _IN const char*    exist_fname, _INOUT char*    new_fname, _IN int bufflen, _IN copy_method method = copy_method_force );
    const wchar_t*  FileCopy( _IN const wchar_t* exist_fname, _INOUT wchar_t* new_fname, _IN int bufflen, _IN copy_method method = copy_method_force );

    //createforce => if folder not exist will create first
    // 1 存在就失敗
    // 2 不存在 也不強制建立資料夾 ( 資料夾不存在 也會失敗 )
    // 3 不存在 制建立資料夾
    int             FileCopy( _IN const char*    exist_fname, _IN const char*    new_fname, _IN bool fail_if_exist = true, _IN bool force_create_dir = false );
    int             FileCopy( _IN const wchar_t* exist_fname, _IN const wchar_t* new_fname, _IN bool fail_if_exist = true, _IN bool force_create_dir = false );

    //int           PfCopyFileF         (const char* exist_fname, const char* new_fname, bool fail_if_exist);
    int             SymLink( _IN const char*    lpszPathObj, _IN const char*    lpszPathLink, _IN const char*    lpszDesc, _IN bool force = false );
    int             SymLink( _IN const wchar_t* lpszPathObj, _IN const wchar_t* lpszPathLink, _IN const wchar_t* lpszDesc, _IN bool force = false );

    //int           PfSymLinkF          (const char* lpszPathObj, const char* lpszPathLink, const char* lpszDesc);
    // returns NZ if file exists, 0 if it does not
    int             FileExist( _IN const char*    name );
    int             FileExist( _IN const wchar_t* name );

    bool            IsDirectory( _IN const char*    path );
    bool            IsDirectory( _IN const wchar_t* path );

    // compare two file if all content was the same, return 0 else return non-zero
    int             FileCmp( _IN const char* srcfname, _IN const char* new_fname );

   //
    //    getabsolutepath  ==> PfGetabsolutepath
    size32_t        GetAbsolutePath( _OUT char*    outs, _IN int outl, _IN const char*    ins );
    size32_t        GetAbsolutePath( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins );

    int             GetRelativePath( _OUT char*    outs, _IN int outl, _IN const char*    ins, _IN const char* cps );
    int             GetRelativePath( _OUT wchar_t* outs, _IN int outl, _IN const wchar_t* ins, _IN const wchar_t* cps );
}



#if defined(_WINDOWS)
    #include <io.h>
    #include <TCHAR.h>
#endif
#if defined(_LINUX)
    #include <unistd.h>
    #include <wchar.h>
    #ifdef  _UNICODE
        #ifndef _tfopen
            #define _tfopen            _wfopen
        #endif
        //#ifndef _tunlink
        //    #define   _tunlink         _wunlink
        //#endif
    #else
        #ifndef _tfopen
            #define _tfopen            fopen
        #endif
        //#ifndef _tunlink
        //    #define  _tunlink          unlink   // win 標準
        //#endif
    #endif
    #define     _fileno                fileno
    #define     _unlink                unlink
#endif


long64_t        filelength( _IN FILE* fp );

#if defined(_LINUX)
FILE*           _wfopen( _IN const wchar_t *filename, _IN const wchar_t *mode );
int             mkdir( _IN const char* dirname );
int             _wmkdir( _IN const wchar_t* dirname );
wchar_t*        _wrealpath( _IN const wchar_t* path, _IN wchar_t* resolved_path );
long64_t        _filelength( _IN int fd );
#endif //_LINUX

