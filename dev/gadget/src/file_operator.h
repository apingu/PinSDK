//
//
//
//
//
//
//
//
//    [? + fname] [| + fno] [* + offset]
//    if cross_platform must define _PSDK
//
//
//                                               Copyright    (C)    1999    Pin
//
#ifndef PfFileOperator_H
#define PfFileOperator_H


#include <vector>
#include <string>
#include <pf-def.h>

const int _PF_OK                             =  1;
const int _ERR_FILE_NOT_FIND                 = -1;
const int _ERR_OPEN_SOURCE_COMBINE_FILE_FAIL = -2;
const int _ERR_OPEN_DEST_COMBINE_FILE_FAIL   = -3;
const int _ERR_OPEN_TEMP_COMBINE_FILE_FAIL   = -4;
const int _ERR_OPEN_TEMP_EXTRACT_FILE_FAIL   = -5;
const int _ERR_NOT_PF_FILE                   = -9;
const int _ERR_TITLE_UN_CONFORM              = -11;
const int _ERR_OPEN_FILE_FAIL                = -12;
const int _ERR_NOT_OPEN_FILE                 = -13;
const int _ERR_NO_BUFS                       = -14;
const int _ERR_PF_PATH                       = -15;
const int _ERR_PF_OVER_EOF                   = -16;
const int _ERR_BUFS_OVERFLOW                 = -17;

const int _DELETE_SORUCE_FILE                =  1;


#ifdef _PSDK
    typedef void*             uPfFile;
#else
    typedef FILE*             uPfFile;
#endif

typedef int       offset_t;
typedef long long pffpos_t;                 // pffpos_t in win32 was a long long but in linux was a struce

typedef bool ( _CALLBACK *OnFileM ) ( const char* file, int depth, void* pData );
typedef bool ( _CALLBACK *OnFileW ) ( const wchar_t* file, int depth, void* pData );

#define PF_NO_MARK   '|'
#define PF_NAME_MARK '?'
#define PF_ARRD_MARK '*'

//using namespace std;

//typedef std::string IOFilepath;

//namespace pfget
//{
    class FileOperator
    {
    protected:
    
        uPfFile                 pp_file_;
        _mstring                fname_; //the file in disk
        _mstring                mode_;        
        int                     errno_;
        
    public:
    
        FileOperator ( _IN const char* filename = NULL, _IN const char* mode = "rb" );
        ~FileOperator();
        
        operator                uPfFile() { return pp_file_; }
        //operator FILE () { return pp_file_; }
        
        /* file control function */
        int                     ferrno          ( void )    { return errno_; };
        const char*             fmode           ( void )    { return mode_.c_str(); }
        uPfFile                 operator()      ( void )    { return pp_file_; }
        std::string             fpath           ( void );
        void                    splitpath       ( _IN int deep, _IN char* ppfname );               //split a file path by "/" and deep
        
        void                    setOpenDevice   ( _IN int device = 1 );                                //PS2 only
        
        virtual uPfFile         open            ( _IN const char* path, _IN const char* mode = "rb" );
        virtual error_t         close           ( void );
        virtual int             fno             ( void );
        virtual int             flength         ( void );
        int                     fcopy_to        ( _IN const char* destfname );
        int                     fdelete         ( void );
        int                     frename         ( _IN const char* newfname );
        uPfFile                 opentmp         ( _IN FileOperator* tmpf, _IN const char* mode = "rb" );

        int                     flush           ( void );
        long                    tell            ( void );                                          // get now file offset base on open pos
        int                     eof             ( void );
        pffpos_t                gotoline        ( _IN unsigned int line );
        pffpos_t                gotonextline    ( void );
        pffpos_t                findword        ( _IN const char* word );
        
        /* file io function*/
        long                    seek            ( _IN pffpos_t offset = 0, _IN int origin = SEEK_SET );
        size32_t                read            ( _OUT void* buffer, _IN size32_t size, _IN size32_t count );
        size32_t                write           ( _IN const void* buffer, _IN size32_t size, _IN size32_t count );

        //size32_t                printf          ( _IN const char* format, ... );
        const char*             readword        ( _OUT _mstring& buffer );
        const char*             readline        ( _OUT _mstring& buffer );                   //end by \n
        const char*             readline        ( _OUT _mstring& buffer, _IN char end );         //end by 'end'
        const char*             readline        ( _OUT _mstring& buffer, _IN const char* end );  //end by 'end'
        size32_t                writeline       ( _IN const char* buffer, _IN size32_t size );
        const char*             readbetween     ( _OUT _mstring& buffer, _IN char begin, _IN char end );
        const char*             readbetween     ( _OUT _mstring& buffer, _IN const char* begin, _IN const char* end );
        pffpos_t                replaceword     ( _IN const char* oldword, _IN const char* newword );
    };
    
    class FileSystem : public FileOperator
    {
        unsigned int            m_base_offset_;
        _mstring                ppfname_;         //the file in pPfFileOperator
        _mstring                ppworkdir_;
        
    public:
    
        FileSystem ( const char* filename = NULL, const char* mode = "rb" );
        ~FileSystem();
        
        uPfFile                 open            ( const char* path, const char* mode = "rb" );
        error_t                 close           ( void );
        
        int                     flength         ( void );
        long                    seek            ( pffpos_t offset = 0, int origin = SEEK_SET );
        long                    tell            ( void );
        int                     eof             ( void );
        pffpos_t                pos             ( void );
        
        /* PP file control */
        int                     workdeep        ( void );                                       //now work path deep
        offset_t                bfind           ( const char* path );
        offset_t                brewind         ( void );
        offset_t                base_offset     ( void );
        
        int                     fsCd            ( const char* path );
        const char*             fsPwd           ( void );
        char*                   ppTransfname    ( char* fname, size32_t size );
        
        int                     checkFormat     ( const word_t title = 0 );
        int                     FileCount       ( void );                                       //get the files count in package file
        int                     FileNo          ( const char* fname );
        size32_t                FileSize        ( int fileno );                                 //get the size of file in package file
        int                     FileName        ( int fileno, char* fname, size32_t size );
        
        size32_t                readfile        ( ubyte_t** buffer, unsigned int index );        //read pp packing file
        size32_t                readfile        ( ubyte_t** buffer, const char* pppath = 0 );    //read pp packing file
        
        size32_t                Extractfile     ( const char* pppath = 0 );
        error_t                 Packagefiles    ( std::vector<const char*> filename, const word_t exttitle = 0, int del_src = 0 );  //combine file

        static size32_t         Scanfiles       ( const char* root, OnFileM fcallback, void* pData = NULL );
        static size32_t         Scanfiles       ( const wchar_t* root, OnFileW fcallback, void* pData = NULL );
    };
//};

#endif
