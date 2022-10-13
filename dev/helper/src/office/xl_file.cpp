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

//01. == C 系統文件/C++ 系統文件
#include <libxl.h>


//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-stdlib.h>
#include <Unicode.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xlutility.inl"
#include "xl_file.h"
#include <gadget/sentence_pattern.h>

#define __LIBXL__

#ifdef _WINDOWS
//#pragma comment ( lib, "libxl.lib" )
#endif

namespace office
{

#define _Workbook( xl )  ((libxl::Book*)xl->v_workbook_)
#define _Worksheet( xl ) ((libxl::Sheet*)xl->v_worksheet_)
#define _Range( xl )     ((xlutility::Range*)xl->selected_range_)

#ifdef _WINDOWS
    // 3.9.3
    //_tstring libxl_user = _T( "jha334201553355102433ahj" );
    //_tstring libxl_key = _T( "windows-2c25240709c0e60565b36d6ca9z1q5rf" );
    _tstring libxl_user = _T( "Apingu" );
    _tstring libxl_key = _T( "windows-2e2b2c0206c6ec0f6bbc6469a8j8qfb4" );
#endif
#ifdef _LINUX
    // 3.9.4.3
    _tstring libxl_user = _T( "Pin Wang" );
    //_tstring libxl_key = _T( "linux-2f2820010bc7ea0763b1646daeo4pdh8" );
    _tstring libxl_key = _T( "linux-e2d7127e9eaba51e090f2f3247l0jfk5" );
    //_tstring libxl_user = _T( "jha334201553355102433ahj" );
    //_tstring libxl_key = _T( "linux-2c25240709c0e60565b36d6ca9z1q5rf" );
#endif

#define _libxlRelease( xl )     { if (_Workbook( xl ) ){ _Workbook( xl )->release(); v_workbook_ = NULL; v_worksheet_ = NULL; } } 
    // 船進來的是從0開始 所以 要減一
    int _FromIndexValue( _IN xlidx_t si, _IN int maxs = 0 )
    {
        //if( si < 0 )
        //    return 0; 
        //if( si >= maxs )
        //    return maxs - 1;
        //return si;
        si--;
        if( si < 0 )
        {
            si = 0;
        }
        else if( maxs > 0 )
        {
            if( si >= ( int )maxs )
            {
                si = ( int )maxs;
            }
        }
        return si;
    }

    xlidx_t _ToIndexValue( _IN int si, _IN int maxs = 0 )
    {
        //if( si < 0 )
        //    return 0;
        //if( si >= maxs )
        //    return maxs - 1;
        //return si;
        si++;
        if( si <= 0 )
        {
            si = 1;
        }
        else if( maxs > 0 )
        {
            if( si >= ( int )maxs )
            {
                si = ( int )maxs;
            }
        }
        return si;
    }

    _tstring GetBookTypeExt( _IN book_type type )
    {
        switch( type )
        {
            case BOOK_TYPE_XLSX:
                {
                    return _T( ".xlsx" );
                }
                break;
            case BOOK_TYPE_XLSM:
                {
                    return _T( ".xlsm" );
                }
                break;
            case BOOK_TYPE_XLS:
                {
                    return _T( ".xls" );
                }
                break;
        }
        return _T( "" );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    XlFile::XlFile()
    {
        v_workbook_ = NULL;
        v_worksheet_ = NULL;
        selected_range_ = ( office::range )new xlutility::Range();
        book_type_ = BOOK_TYPE_NONE;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    XlFile::~XlFile()
    {
        _libxlRelease( this );
        xlutility::Range* range = _Range( this );
        SAFE_DELETE( range );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool XlFile::New( _IN book_type type )
    {
        file_name_.clear();
        password_.clear();
        book_type_ = type;

#ifdef __LIBXL__
        _libxlRelease( this );
        switch( type )
        {
            case BOOK_TYPE_XLSX:
            case BOOK_TYPE_XLSM:
                {
                    v_workbook_ = ( workbook )xlCreateXMLBook();
                }
                break;
            case BOOK_TYPE_XLS:
                {
                    v_workbook_ = ( workbook )xlCreateBook();
                }
                break;
            default:
                return false;
        }
        _Workbook( this )->setKey( libxl_user.c_str(), libxl_key.c_str() );

#ifndef _UNICODE
#ifdef _LINUX
        _Workbook( this )->setLocale( _T( "en_US.UTF-8" ) );
#endif
#ifdef _WINDOWS
        _Workbook( this )->setLocale( _T( "UTF-8" ) );
#endif
#endif

#endif
        return true;
    }

    _tstring clear_value( _IN const tchar_t* value )
    {
        _tstring cv;

        if( value == NULL )
        {
            return cv;
        }

        int len = _tcslen( value );
        int vb = 0;
        int ve = len - 1;

        for( uint_t vi = 0; vi < len; vi++ )
        {
            if( value[ vi ] == _T( '"' ) )
            {
                vb = vi + 1;
                break;
            }
        }

        for( uint_t vi = ve; vi >= 0; vi-- )
        {
            if( value[ vi ] == _T( '"' ) )
            {
                ve = vi - 1;
                break;
            }
        }

        cv.assign( value, vb, ve - vb );
        return cv;
    }

    bool read_csv( _IN XlFile* xlf, _IN const tchar_t* file_name )
    {
        if( ( xlf == NULL ) || ( file_name == NULL ) )
        {
            return false;
        }

        xlf->New();
        xlf->AddSheet( _T( "sheet01" ) );

        _tcsvector debug_value;
        // 有多少行
        int        max_cols = 0;
        bool       is_error = false;

        FILE* csvf = _tfopen( file_name, _T( "rb" ) );
        if( csvf != NULL )
        {
            size64_t flen = filelength( csvf );
            char* file_buff = new char[ flen + 10 ]; // 要多結束字算
            memset( file_buff, 0, flen + 10 );
            uint_t rc = 1;
            fseek( csvf, 0, SEEK_SET );
            fread( file_buff, flen, 1, csvf );
            fclose( csvf );

            char* utf8_buff = file_buff;

            if( IsUTF8( file_buff ) == 2 )
            {
                // 移除 efbbbf
                utf8_buff += 3;
            }

            char  seps[] = ( "\n\r" );
            char* buff = NULL;
            char* next_token = NULL;

            buff = strtok_rc( utf8_buff, seps, &next_token ); // C4996
            // Note: strtok is deprecated; consider using strtok_s instead

//while ( fgets( buff, flen, csvf ) != NULL )  // 讀一行
            while( buff != NULL )
            {
                uint_t cc = 1;
                _tstring line_string;
#ifdef _UNICODE
                line_string = pfstd::UTF8ToUnicode( buff );
#else
                line_string = buff;
#endif
                // 去除空白
                //if ( !line_string.empty() )
                //{
                //
                //    tchar_t* temp_buff = new tchar_t[ line_string.size() + 1 ];
                //    // 去除前後的空格
                //    _tcscpy( temp_buff, line_string.c_str() );
                //    _tcsextractstring( temp_buff );
                //    line_string = temp_buff;
                //    SAFE_DELETE_ARRAY( temp_buff );
                //}

                if( line_string.empty() )
                {
                    continue;
                }

                if( buff[ strlen( buff ) - 1 ] == '\n' )
                {
                    line_string.erase( line_string.end() - 1, line_string.end() ); // /r/
                }

                if( buff[ strlen( buff ) - 2 ] == '\r' )
                {
                    line_string.erase( line_string.end() - 1, line_string.end() ); // /r/
                }

                // 分割 一行的每一段
                /*
                static const tchar_t parser_escape_char[] = _T( ",\t" );
                _tcsvector           data_splits;  // 整體的 E 操作欄拆解
                std::vector<tchar_t> oper_splits;
                sentence_pattern::SplitString01( data_splits, oper_splits, line_string, parser_escape_char );

                for ( uint_t li = 0; li < data_splits.size(); li++ )
                {
                    _tstring data_section = clear_value( data_splits[ li ] );
                    if ( ( data_section[ 0 ] == _T( "\"" ) ) && ( data_section.end() == _T( "\"" ) ) )
                    {
                        data_section.erase( data_section.begin() );
                        data_section.erase( data_section.begin() + ( data_section.size() - 1 );
                    }
                    xlf->SelectCell( cc, rc )->SetCell( data_splits[li].c_str() );
                }
                */

                line_string.push_back( _T( ',' ) );

                std::vector<tchar_t> cell_data;
                size64_t llen = line_string.size();

                // st 代表進入引號內
                // 但引號內 有引號內的引號 ""的問題 PCHOME  "="""""\t
                bool st = false;

                for( uint_t li = 0; li < llen; li++ )
                {
                    if( line_string[ li ] == _T( '"' ) )
                    {
                        // 看下一個是甚麼
                        //li++;                        

                        // 上一個是 " 
                        if( st )
                        {
                            //if ( line_string[ li ] == _T( ',' ) )
                            //{
                            //    cell_data.push_back( _T( '\0' ) );
                            //    ( cell_data.empty() ) ? xlf->SelectCell( cc, rc )->SetCell( _T( "" ) ) : xlf->SelectCell( cc, rc )->SetCell( &cell_data[ 0 ] );
                            //    cell_data.clear();
                            //    cc++;
                            //    st = false; //回復
                            //    continue;
                            //}
                            // 再再字串中 的 "" 必須要是兩個
                            // 不超過長度
                            if( ( li < llen - 1 ) )
                            {
                                // 字串中的連續分號
                                if( line_string[ li + 1 ] == _T( '"' ) )
                                {
                                    cell_data.push_back( _T( '"' ) );
                                    li++;
                                    continue;
                                }
                            }
                            // 字串做結尾
                            cell_data.push_back( _T( '\0' ) );
                            st = false;
                        }
                        else
                        {
                            st = true;
                            // 如果 連續 ""
                            //if ( line_string[ li ] == _T( '"' ) )
                            //{
                            //    // 連續分號
                            //    cell_data.push_back( _T( '\0' ) );
                            //    ( cell_data.empty() ) ? xlf->SelectCell( cc, rc )->SetCell( _T( "" ) ) : xlf->SelectCell( cc, rc )->SetCell( &cell_data[ 0 ] );
                            //    cell_data.clear();
                            //    cc++;
                            //    st = false; //回復
                            //    continue;
                            //}
                        }
                        //cell_data.push_back( line_string[ li ] );
                        continue;
                    }
                    else if( ( line_string[ li ] == _T( ',' ) ) || ( line_string[ li ] == _T( '\t' ) ) )
                    {
                        if( li > 0 )
                        {
                            // 可能是一個字串的結束
                            // 在格子內的,
                            if( st )
                            {
                                cell_data.push_back( line_string[ li ] );
                                continue;
                            }
                        }

                        // 前面有一個 '?
                        if( cell_data.size() > 1 )
                        {
                            if( ( cell_data[ 0 ] == _T( '\'' ) ) && ( cell_data.back() != _T( '\'' ) ) )
                            {
                                cell_data.erase( cell_data.begin() );
                            }
                        }

                        cell_data.push_back( _T( '\0' ) );
                        ( cell_data.empty() ) ? xlf->SelectCell( cc, rc )->SetCell( _T( "" ) ) : xlf->SelectCell( cc, rc )->SetCell( &cell_data[ 0 ] );
                        debug_value.push_back( &cell_data[ 0 ] );
                        cell_data.clear();
                        cc++;
                        continue;
                    }
                    else
                    {
                        cell_data.push_back( line_string[ li ] );
                    }
                }

                // 檢查列數有沒有都一樣
                // 不一定會一樣 所以作罷
                {
                    if( max_cols == 0 )
                    {
                        max_cols = cc;
                    }

                    //if ( cc != max_cols )
                    //{
                    //    is_error = true;
                    //    break;
                    //}
                }

                rc++;
                buff = strtok_rc( NULL, seps, &next_token ); // C4996
            }
            SAFE_DELETE_ARRAY( file_buff );
        }

        if( !is_error )
        {
            tchar_t drive[ _MAX_DRIVE ];
            tchar_t dir[ _MAX_DIR ];
            tchar_t fname[ _MAX_FNAME ];
            tchar_t ext[ _MAX_EXT ];
            _tsplitfpath( file_name, drive, dir, fname, ext );

            tchar_t savefile_name[ _MAX_PATH ];
            _tmakefpath( savefile_name, drive, dir, fname, _T( ".xlsx" ) );
            xlf->Save( savefile_name );
        }
        return !is_error;
    }

    bool XlFile::Load( _IN const tchar_t* file_name, _IN const tchar_t* password )
    {
        //if( !v_workbook_ )
        //    return false;

        mstimen_t currt = pftml::Time::GetTickMSTime();

        if( !pfsfl::FileExist( file_name ) )
            return false;

        file_name_ = file_name;
        password_.clear();

#ifdef __LIBXL__

        _libxlRelease( this );

        tchar_t ext[ _MAX_EXT ];
        {
            //tchar_t path_buffer[ _MAX_PATH ];
            tchar_t drive[ _MAX_DRIVE ];
            tchar_t dir[ _MAX_DIR ];
            tchar_t fname[ _MAX_FNAME ];
            _tsplitpath( file_name_.c_str(), drive, dir, fname, ext ); // C4996            
        }

        if( _tcscmp( ext, _T( ".xlsx" ) ) == 0 )
        {
            v_workbook_ = ( workbook )xlCreateXMLBook();
            book_type_ = BOOK_TYPE_XLSX;
        }
        else if( _tcscmp( ext, _T( ".xlsm" ) ) == 0 )
        {
            v_workbook_ = ( workbook )xlCreateBook();
            book_type_ = BOOK_TYPE_XLSM;
        }
        else if( _tcscmp( ext, _T( ".xls" ) ) == 0 )
        {
            v_workbook_ = ( workbook )xlCreateBook();
            book_type_ = BOOK_TYPE_XLS;
        }
        else if( _tcscmp( ext, _T( ".csv" ) ) == 0 )
        {
            return read_csv( this, file_name );
        }
        else
        {
            return false;
        }
        _Workbook( this )->setKey( libxl_user.c_str(), libxl_key.c_str() );

#ifndef _UNICODE
#ifdef _LINUX
        _Workbook( this )->setLocale( _T( "en_US.UTF-8" ) );
#endif
#ifdef _WINDOWS
        _Workbook( this )->setLocale( _T( "UTF-8" ) );
#endif
#endif

        bool ret = _Workbook( this )->load( file_name_.c_str() );
        if( !ret )
        {
            const char* err = _Workbook( this )->errorMessage();
            Pf_INFO( err );
        }

        mstimen_t currf = ::pftml::Time::GetTickMSTime() - currt;
#endif
        // 順便選
        SelectSheet( 1 );
#ifdef _UNICODE
        Pf_INFO( _u8( "XlFile::Open load file %s spend time: %d\n" ), pfstd::UnicodeToUTF8( file_name ).c_str(), currf );
#else
        Pf_INFO( ( "XlFile::Open load file %s spend time: %d\n" ), file_name, currf );
#endif
        return ret;
    }

    bool XlFile::Save()
    {
        if( !v_workbook_ )
            return false;

#ifdef __LIBXL__
        return _Workbook( this )->save( file_name_.c_str() );
#endif
        return false;
    }

    bool XlFile::Save( _IN const tchar_t* file_name, _IN const tchar_t* password )
    {
        if( !v_workbook_ )
            return false;

        if( file_name != NULL )
        {
            // 正確的檔案名
            // 建立或讀的是什樣的檔案類型 就要存成甚麼樣的檔名
            tchar_t* curr_file_name = new tchar_t[ _tcslen( file_name ) * 2 ];
            {
                tchar_t drive[ _MAX_DRIVE ];
                tchar_t dir[ _MAX_DIR ];
                tchar_t fname[ _MAX_FNAME ];
                tchar_t ext[ _MAX_EXT ];
                _tsplitpath( file_name, drive, dir, fname, ext ); // C4996
                _tmakefpath( curr_file_name, drive, dir, fname, GetBookTypeExt( book_type_ ).c_str() );
            }
            file_name_ = curr_file_name;
            SAFE_DELETE_ARRAY( curr_file_name );
        }
        else
        {
            if( file_name_.empty() )
            {
                return false;
            }
        }


#ifdef __LIBXL__
        return _Workbook( this )->save( file_name_.c_str() );
#endif
        return false;
    }

    //bool XlFile::SaveAs( _IN const tchar_t* file_name )
    //{
    //    {
    //        //tchar_t path_buffer[_MAX_PATH];
    //        tchar_t drive[ _MAX_DRIVE ];
    //        tchar_t dir[ _MAX_DIR ];
    //        tchar_t file_name[ _MAX_FNAME ];
    //        tchar_t ext[ _MAX_EXT ];
    //        _tsplitpath( file_name, drive, dir, file_name, ext ); // C4996
    //
    //        if ( _tcscmp( ext, _TEXT( ".xlt" ) ) == 0 )
    //        {
    //
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".ods" ) ) == 0 )
    //        {
    //
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xml" ) ) == 0 )
    //        {
    //
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".cvs" ) ) == 0 )
    //        {
    //
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xlsb" ) ) == 0 ) //50
    //        {
    //
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xlsx" ) ) == 0 ) // 51
    //        {
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xlsm" ) ) == 0 ) //52
    //        {
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xltm" ) ) == 0 ) //53
    //        {
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xltx" ) ) == 0 ) //54
    //        {
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xlam" ) ) == 0 ) //55
    //        {
    //        }
    //        else if ( _tcscmp( ext, _TEXT( ".xls" ) ) == 0 )  //56
    //        {
    //        }
    //        else
    //        {
    //        }
    //        return false;
    //    }
    //}
    //
    //void XlFile::ExportAs( _IN const tchar_t*, _IN int pagefrom, _IN int pageto ) //產出的pdf 會從第幾頁到 第幾頁    
    //{
    //
    //}

    book_type XlFile::GetBookType() const
    {
        return book_type_;
    }


    XlFile& XlFile::operator= ( _IN const XlFile& xlf )
    {
        book_type bt = xlf.GetBookType();
        if( bt != BOOK_TYPE_NONE )
        {
            bool succ = false;
            if( !v_workbook_ )
            {
                if( New( bt ) )
                {
                    succ = true;
                }
            }

            if( succ )
            {
#ifdef __LIBXL__
                const char* data_buff;
                unsigned    data_size;
                _Workbook( ( &xlf ) )->saveRaw( &data_buff, &data_size );
                _Workbook( this )->loadRaw( data_buff, data_size );
#endif    
            }

        }
        return *this;
    }

    const tchar_t* XlFile::GetErrorMessage()
    {
        if( !v_workbook_ )
            return NULL;

#ifdef __LIBXL__
#ifdef _UNICODE
        return pfstd::UTF8ToUnicode( _Workbook( this )->errorMessage() ).c_str();
#else
        return pfstd::StringToUTF8( _Workbook( this )->errorMessage() ).c_str();
#endif
#endif
    }

    //取得 Sheet 個數
    int XlFile::GetSheetTotal()
    {
        if( !v_workbook_ )
            return -1;

#ifdef __LIBXL__
        return _Workbook( this )->sheetCount();
#endif
    }

    //取得所有的sheet_name
    _tcsvector XlFile::GetAllSheetNames()
    {
        _tcsvector sheetnames;
        if( !v_workbook_ )
            return sheetnames;

#ifdef __LIBXL__
        for( unsigned short i = 0; i < _Workbook( this )->sheetCount(); ++i )
        {
            sheetnames.push_back( _Workbook( this )->getSheet( i )->name() );
        }
#endif
        return sheetnames;
    }

    xlidx_t XlFile::FindSheetIndex( _IN const tchar_t* sheet_name )
    {
        if( !v_workbook_ )
            return -1;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return -1;
        }

#ifdef __LIBXL__
        for( unsigned short i = 0; i < _Workbook( this )->sheetCount(); ++i )
        {
            libxl::Sheet* sheet = _Workbook( this )->getSheet( i );
            const tchar_t* findname = sheet->name();
            if( findname == NULL )
                continue;

            // excel 表格命名只能有 31個字元
            //if ( _tcsncmp( sheet_name, findname, _MAX_SHEET_NAME ) == 0 )
            if( _tcscmp( sheet_name, findname ) == 0 )
            {
                return _ToIndexValue( i );
            }
        }
#endif
        return -1;
    }

    //由SheetNumber    選擇Sheet
    XlFile* XlFile::SelectSheet( _IN xlidx_t index )
    {
        if( !v_workbook_ )
            return NULL;

#ifdef __LIBXL__
        v_worksheet_ = ( worksheet )_Workbook( this )->getSheet( _FromIndexValue( index ) );
        if( v_worksheet_ == NULL )
        {
            const char* err = _Workbook( this )->errorMessage();
            Pf_ERROR( err );
            //v_worksheet_ = NULL;
        }
        _Workbook( this )->setActiveSheet( _FromIndexValue( index ) );
#endif
        return this;
    }

    //由SheetName      選擇Sheet
    XlFile* XlFile::SelectSheet( _IN const tchar_t* sheet_name )
    {
        if( !v_workbook_ )
            return NULL;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return NULL;
        }

#ifdef __LIBXL__
        xlidx_t idx = FindSheetIndex( sheet_name );
        if( idx < 0 )
        {
            //  20220707 ?? 為什麼 找不到 還要 傳正常? 待確認
            return NULL;
        }
        return SelectSheet( idx );
#endif
        return this;
    }

    //由SheetIndex    取得SheetName 從 1 開始
    _tstring XlFile::GetSheetName()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

#ifdef __LIBXL__
        return _Worksheet( this )->name();
#endif
    }

    //由SheetName     取得SheetIndex 從 1 開始
    xlidx_t XlFile::GetSheetIndex()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

#ifdef __LIBXL__
        return FindSheetIndex( _Worksheet( this )->name() );
#endif
    }

    //指定SheetName
    void XlFile::SetSheetName( _IN const tchar_t* sheet_name )
    {
        if( !v_worksheet_ )
            return;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return;
        }

#ifdef __LIBXL__
        return _Worksheet( this )->setName( sheet_name );
#endif
    }

    //由將sheet 設密碼
    XlFile* XlFile::SetPassword( _IN const tchar_t* password )
    {
        if( !v_worksheet_ )
            return NULL;

#ifdef __LIBXL__
        bool isprotected = _Worksheet( this )->protect();
        // 如果已經設密碼 就解密
        if( isprotected )
            _Worksheet( this )->setProtect( false, password );
        // 如果沒有設密碼 就加密
        else
            _Worksheet( this )->setProtect( true, password );
#endif
        return this;
    }

    //將sheet 隱藏
    XlFile* XlFile::SetHidden( _IN bool t )
    {
        if( !v_worksheet_ )
            return NULL;

#ifdef __LIBXL__
        if( t )
            _Worksheet( this )->setHidden( libxl::SHEETSTATE_HIDDEN );
        else
            _Worksheet( this )->setHidden( libxl::SHEETSTATE_VISIBLE );
#endif
        return this;
    }

    bool XlFile::AddSheet( _IN const tchar_t* sheet_name )
    {
        if( !v_workbook_ )
            return false;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return false;
        }

#ifdef __LIBXL__
        libxl::Sheet* sheet = _Workbook( this )->addSheet( sheet_name, 0 );
        v_worksheet_ = ( worksheet )sheet;
#endif

        return true;
    }

    // 在後面加入一個sheet
    bool XlFile::AddSheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter )
    {
        if( !v_workbook_ )
            return false;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return false;
        }

#ifdef __LIBXL__
        libxl::Sheet* sheet = _Workbook( this )->insertSheet( _FromIndexValue( indexafter ), sheet_name );
        v_worksheet_ = ( worksheet )sheet;
#endif
        return true;
    }

    //複製SheetName    指定插入Sheet的位置，並指定新Sheet名稱
    bool XlFile::CopySheet( _IN const tchar_t* sheet_name )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return false;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return false;
        }

#ifdef __LIBXL__
        libxl::Sheet* sheet = _Workbook( this )->addSheet( sheet_name, _Worksheet( this ) );
        v_worksheet_ = ( worksheet )sheet;
#endif
        if( v_worksheet_ == NULL )
        {
            return false;
        }
        return true;
    }

    //複製SheetName    傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet名稱
    bool XlFile::CopySheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return false;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return false;
        }

        if( indexafter < 0 )
            return false;

#ifdef __LIBXL__
        libxl::Sheet* sheet = _Workbook( this )->insertSheet( _FromIndexValue( indexafter ), sheet_name, _Worksheet( this ) );
        v_worksheet_ = ( worksheet )sheet;
#endif
        return true;
    }

    //選SheetNumber    刪除Sheet
    bool XlFile::DelSheet( _IN xlidx_t index )
    {
        if( !v_workbook_ )
            return false;

        if( index < 0 )
            return false;

#ifdef __LIBXL__
        xlidx_t idx = _FromIndexValue( index );
        // 如果是選到的
        if( _Workbook( this )->getSheet( idx ) == _Worksheet( this ) )
        {
            //v_worksheet_ = ( worksheet )_Worksheet( this )->getSheet( idx - 1 );
            v_worksheet_ = NULL;
        }
        return _Workbook( this )->delSheet( idx );
#endif
    }

    //選SheetName      刪除Sheet
    bool XlFile::DelSheet( _IN const tchar_t* sheet_name )
    {
        if( !v_workbook_ )
            return false;

        if( ( sheet_name == NULL ) || ( _tcscmp( sheet_name, _T( "" ) ) == 0 ) )
        {
            return false;
        }

#ifdef __LIBXL__
        return DelSheet( FindSheetIndex( sheet_name ) );
#endif
    }

    // 第一個non-empty 起始行
    xlidx_t  XlFile::GetColFirstCell()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

#ifdef __LIBXL__
        return _ToIndexValue( _Worksheet( this )->firstCol() );
#endif
    }

    // 第一個non-empty 起始列
    xlidx_t  XlFile::GetRowFirstCell()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

#ifdef __LIBXL__
        return _ToIndexValue( _Worksheet( this )->firstRow() );
#endif
    }

    // 總行數 (沒資料的部分也會被取出)
    // 最後一個non-empty 結束行
    int  XlFile::GetColTotalCell()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

#ifdef __LIBXL__
        return _Worksheet( this )->lastCol();
#endif
    }

    // 總列數 (沒資料的部分也會被取出)
    // 最後一個non-empty 結束列
    int XlFile::GetRowTotalCell()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

#ifdef __LIBXL__
        return _Worksheet( this )->lastRow();
#endif
    }

    //--------------------------------------------------------------------------
    //std::vector<xlidx_t> XlFile::GetCellNumber( const tchar_t* refstr )
    //{
    //    std::vector<xlidx_t> vec;
    //    return vec;
    //}
    //
    //_tstring XlFile::GetCellAddress( xlidx_t row, xlidx_t col )
    //{
    //
    //}

    bool XlFile::InsertRow( _IN int row_first, _IN int row_last, _IN bool update_name_dranges )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->insertRow( _ToIndexValue( row_first ), _ToIndexValue( row_last ), update_name_dranges );
#endif
    }

    bool XlFile::InsertCol( _IN int colfirst, _IN int col_last, _IN bool update_name_dranges )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->insertCol( _ToIndexValue( colfirst ), _ToIndexValue( col_last ), update_name_dranges );
#endif
    }

    bool XlFile::RemoveRow( _IN int row_first, _IN int row_last, _IN bool update_name_dranges )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->removeRow( _ToIndexValue( row_first ), _ToIndexValue( row_last ), update_name_dranges );
#endif
    }

    bool XlFile::RemoveCol( _IN int colfirst, _IN int col_last, _IN bool update_name_dranges )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->removeCol( _ToIndexValue( colfirst ), _ToIndexValue( col_last ), update_name_dranges );
#endif
    }

    double XlFile::GetColWidth( _IN int col )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->colWidth( _ToIndexValue( col ) );
#endif
    }

    double XlFile::GetRowHeight( _IN int row )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->rowHeight( _ToIndexValue( row ) );
#endif
    }

    // -1 for auto
    bool XlFile::SetColWidth( _IN int col, _IN double width )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->setCol( _ToIndexValue( col ), _ToIndexValue( col ), width );
#endif
    }

    bool XlFile::SetRowHeight( _IN int row, _IN double height )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->setRow( _ToIndexValue( row ),height );
#endif
    }

    bool XlFile::SetColHidden( _IN int col, _IN bool hidden )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->setColHidden( _ToIndexValue( col ), hidden );
#endif
    }

    bool XlFile::SetRowHidden( _IN int row, _IN bool hidden )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        return _Worksheet( this )->setRowHidden( _ToIndexValue( row ), hidden );
#endif
    }




    //--------------------------------------------------------------------------
    XlFile* XlFile::SelectAllCells()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

#ifdef __LIBXL__
        _Range( this )->left = _ToIndexValue( _Worksheet( this )->firstCol() );
        _Range( this )->top = _ToIndexValue( _Worksheet( this )->firstRow() );
        _Range( this )->right = _ToIndexValue( _Worksheet( this )->lastCol() );
        _Range( this )->top = _ToIndexValue( _Worksheet( this )->lastRow() );
#endif
        return this;

    }

    XlFile* XlFile::SelectRange( _IN const tchar_t* address )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

        _Range( this )->SetAddress( address );
        return this;
    }

    XlFile* XlFile::SelectRange( _IN xlidx_t colfirst, _IN xlidx_t row_first, _IN xlidx_t col_last, _IN xlidx_t row_last )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

        _Range( this )->left = colfirst;
        _Range( this )->top = row_first;
        _Range( this )->right = col_last;
        _Range( this )->bottom = row_last;
        return this;
    }

    XlFile* XlFile::SelectCell( _IN const tchar_t* address )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

        xlutility::Cell cell( address );
        _Range( this )->left = cell.x;
        _Range( this )->top = cell.y;
        _Range( this )->right = cell.x;
        _Range( this )->bottom = cell.y;
        return this;
    }

    XlFile* XlFile::SelectCell( _IN xlidx_t col, _IN xlidx_t row )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return NULL;

        _Range( this )->left = col;
        _Range( this )->top = row;
        _Range( this )->right = col;
        _Range( this )->bottom = row;
        return this;
    }

    //--------------------------------------------------------------------------
    int XlFile::GetRangeWidth()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

        return _Range( this )->width();
    }

    int XlFile::GetRangeHeight()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

        return _Range( this )->height();
    }

    xlidx_t XlFile::GetRangeLeft()
    {
        return GetCellCol();
    }

    xlidx_t XlFile::GetRangeTop()
    {
        return GetCellRow();
    }

    xlidx_t XlFile::GetRangeRight()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

        return _Range( this )->right;
    }

    xlidx_t XlFile::GetRangeBottom()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

        return _Range( this )->bottom;
    }

    _tstring XlFile::GetRangeTopLeftAddress()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

        xlutility::Cell cell( _Range( this )->left, _Range( this )->top );
        return cell.GetAddress();
    }

    _tstring XlFile::GetRangeTopRightAddress()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

        xlutility::Cell cell( _Range( this )->right, _Range( this )->top );
        return cell.GetAddress();
    }

    _tstring XlFile::GetRangeBottomLeftAddress()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

        xlutility::Cell cell( _Range( this )->left, _Range( this )->bottom );
        return cell.GetAddress();
    }

    _tstring XlFile::GetRangeBottomRightAddress()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

        xlutility::Cell cell( _Range( this )->right, _Range( this )->bottom );
        return cell.GetAddress();
    }

    // range or cell 自動調整寬高
    void  XlFile::SetAutoFit()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        _Worksheet( this )->setAutoFitArea( _FromIndexValue( _Range( this )->top ),
                                            _FromIndexValue( _Range( this )->left ),
                                            _FromIndexValue( _Range( this )->bottom ),
                                            _FromIndexValue( _Range( this )->left ) );
#endif
    }

    //-------------------------------------------------------------------------
    _tstring XlFile::GetCellAddress()
    {
        return GetRangeTopRightAddress();
    }

    //-------------------------------------------------------------------------
    _tstring XlFile::GetCellAddress( _IN xlidx_t col, _IN xlidx_t row )
    {
        xlutility::Cell cell( col, row );
        return cell.GetAddress();
    }

    //-------------------------------------------------------------------------
    // 傳回位在的行數
    xlidx_t XlFile::GetCellCol()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

        return _Range( this )->left;
    }

    // 傳回位在的列數
    xlidx_t XlFile::GetCellRow()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return -1;

        return _Range( this )->top;
    }

    //clear cell by SelectCell or SelectRange
    bool XlFile::ClearCells()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return false;

#ifdef __LIBXL__
        return _Worksheet( this )->clear( _FromIndexValue( _Range( this )->top ),
                                          _FromIndexValue( _Range( this )->bottom ),
                                          _FromIndexValue( _Range( this )->left ),
                                          _FromIndexValue( _Range( this )->right ) );
#endif
    }

    bool XlFile::SetMerge()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return false;

#ifdef __LIBXL__
        return _Worksheet( this )->setMerge( _FromIndexValue( _Range( this )->top ),
                                             _FromIndexValue( _Range( this )->bottom ),
                                             _FromIndexValue( _Range( this )->left ),
                                             _FromIndexValue( _Range( this )->right ) );
#endif
    }

    bool XlFile::DelMerge()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return false;

#ifdef __LIBXL__
        return _Worksheet( this )->delMerge( _FromIndexValue( _Range( this )->top ),
                                             _FromIndexValue( _Range( this )->bottom ) );
#endif
    }

    XlFile* XlFile::SetCellColor( _IN xlcolor color )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

        _mstring fontname;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );

        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //}

        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
            }
        }
        format->setPatternBackgroundColor( ( libxl::Color )color );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    //設定字型（預設新細明體）
    XlFile* XlFile::SetCellFont( _IN const tchar_t* font_type )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

        _mstring fontname;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //}
        //libxl::Font* font = format->font();
        //if ( font == NULL )
        //{
        //    font = _Workbook( this )->addFont();
        //}
        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
                font = format->font();
            }
        }
        font->setName( font_type );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    //粗體
    XlFile* XlFile::SetCellFontBold( _IN bool is_bold )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //}
        //libxl::Font* font = format->font();
        //if ( font == NULL )
        //{
        //    font = _Workbook( this )->addFont();
        //}
        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
                font = format->font();
            }
        }
        font->setBold( is_bold );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    //刪除線
    XlFile* XlFile::SetCellFontStrkthrgh( _IN bool is_strkthrgh )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //}
        //libxl::Font* font = format->font();
        //if ( font == NULL )
        //{
        //    font = _Workbook( this )->addFont();
        //}
        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
                font = format->font();
            }
        }
        font->setStrikeOut( is_strkthrgh );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    //設定字體大小（預設12pt）
    XlFile* XlFile::SetCellFontSize( _IN short font_size )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //}
        //libxl::Font* font = format->font();
        //if ( font == NULL )
        //{
        //    font = _Workbook( this )->addFont();
        //}
        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
                font = format->font();
            }
        }
        font->setSize( font_size );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    //字型顏色（預設黑色）
    XlFile* XlFile::SetCellFontColor( _IN xlcolor color )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return this;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        //libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        //if ( format == NULL )
        //{
        //    format = _Workbook( this )->addFormat();
        //    _Worksheet( this )->setCellFormat( iy, ix, format );
        //}
        //libxl::Font* font = format->font();
        //if ( font == NULL )
        //{
        //    font = _Workbook( this )->addFont();
        //    format->setFont( font );
        //}
        libxl::Format* format = NULL;
        libxl::Font* font = NULL;
        {
            _tstring cell_pos;
            _pf_tstrprintf( cell_pos, _T( "%d:%d" ), ix, iy );
            std::set<_tstring>::iterator iter = format_.find( cell_pos );
            if( iter == format_.end() )
            {
                libxl::Format* src_format = _Worksheet( this )->cellFormat( iy, ix );
                format = _Workbook( this )->addFormat( src_format );
                font = _Workbook( this )->addFont( format->font() );
                format->setFont( font );
                format_.insert( std::set<_tstring>::value_type( cell_pos ) );
            }
            else
            {
                format = _Worksheet( this )->cellFormat( iy, ix );
                font = format->font();
            }
        }
        font->setColor( ( libxl::Color )color );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
        return this;
    }

    cell_type XlFile::GetCellType()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return cell_type::CELL_TYPE_EMPTY;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        if( _Worksheet( this )->isFormula( iy, ix ) )
            return cell_type::CELL_TYPE_FORMUL;

        libxl::CellType ct = _Worksheet( this )->cellType( iy, ix );
        return ( cell_type )ct;
#endif
    }

    //  "@";  //設定C欄儲存格格式為文字; "yyyy/MM/dd";  //設定C欄儲存格格式  #,##
    _tstring XlFile::GetCellFormat()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        if( format == NULL )
        {
            return _T( "" );
        }
        return  _Workbook( this )->customNumFormat( format->numFormat() );
#endif
    }

    //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
    //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
    void XlFile::SetCellFormat( _IN const tchar_t* fmt_str )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        int customnumformat = _Workbook( this )->addCustomNumFormat( fmt_str );

        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        if( format == NULL )
        {
            format = _Workbook( this )->addFormat();
        }
        format->setNumFormat( customnumformat );

        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->setCellFormat( _FromIndexValue( yi ), _FromIndexValue( xi ), format );
            }
        }
#endif
    }

    // 設巨集
    void XlFile::SetCellFormula( _IN const tchar_t* formula )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeFormula( _FromIndexValue( yi ), _FromIndexValue( xi ), formula );
            }
        }
#endif
    }

    _tstring XlFile::GetCellFormula()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        return _Worksheet( this )->readFormula( iy, ix );
#endif
    }

    void XlFile::SetCellAlignH( _IN cell_align_h align )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Workbook( this )->addFormat();
        if( format == NULL )
        {
            return;
        }
        format->setAlignH( ( libxl::AlignH )align );
        _Worksheet( this )->setCellFormat( iy, ix, format );
#endif
    }

    cell_align_h XlFile::GetCellAlignH()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return ( cell_align_h )_INVALID_VALUE;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        if( format == NULL )
        {
            return ( cell_align_h )_INVALID_VALUE;
        }
        return ( cell_align_h )format->alignH();
#endif
    }

    void XlFile::SetCellAlignV( _IN cell_align_v align )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Workbook( this )->addFormat();
        if( format == NULL )
        {
            return;
        }
        format->setAlignV( ( libxl::AlignV )align );
        _Worksheet( this )->setCellFormat( iy, ix, format );
#endif
    }

    cell_align_v XlFile::GetCellAlignV()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return ( cell_align_v )_INVALID_VALUE;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );
        libxl::Format* format = _Worksheet( this )->cellFormat( iy, ix );
        if( format == NULL )
        {
            return ( cell_align_v )_INVALID_VALUE;
        }
        return ( cell_align_v )format->alignV();
#endif
    }


    void XlFile::SetCell( _IN bool boolean_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeBool( _FromIndexValue( yi ), _FromIndexValue( xi ), boolean_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN int int_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )int_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN uint_t uint_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )uint_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN int64_t lint_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )lint_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN uint64_t ulint_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )ulint_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN float float_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )float_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN double double_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__
        for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
        {
            for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
            {
                _Worksheet( this )->writeNum( _FromIndexValue( yi ), _FromIndexValue( xi ), ( double )double_value );
            }
        }
#endif
    }

    void XlFile::SetCell( _IN const tchar_t* string_value )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__

        if( string_value[ 0 ] == _T( '=' ) )
        {
            for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
            {
                for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
                {
                    _Worksheet( this )->writeFormula( _FromIndexValue( yi ), _FromIndexValue( xi ), string_value );
                }
            }
            return;
        }

        libxl::CellType cell_type = libxl::CELLTYPE_STRING;

        //  沒寫數字 會發生 寫入後 該格的 數字被當的文字 導致 公式無法運算
        if( _istnumber( string_value ) != 0 )
        {
            cell_type = libxl::CELLTYPE_NUMBER;
        }

        {
            for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
            {
                for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
                {
                    _Worksheet( this )->writeStr( _FromIndexValue( yi ), _FromIndexValue( xi ), string_value, 0, cell_type );
                }
            }
        }
#endif
    }

    //格式必須是 數值才能取出
    int XlFile::GetCell2Int()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );

        // 這邊 因為 excel 的日期 會存成一個5位數的數字 因此 先判斷是不試日期 再做轉換
        // 你媽的excel
        if( _Worksheet( this )->isDate( iy, ix ) )
        {
            _tstring datetime;
            int year, month, day, hour, min, sec;
            _Workbook( this )->dateUnpack( _Worksheet( this )->readNum( iy, ix ), &year, &month, &day, &hour, &min, &sec );
            _pf_tstrprintf( datetime, _T( "%04d%02d%02d%02d%02d%02d" ), year, month, day, hour, min, sec );
            return _pf_ttoi( datetime );
        }

        return ( int )_Worksheet( this )->readNum( iy, ix );
#endif
    }

    //格式必須是 文字才能取出
    _tstring XlFile::GetCell2CStr()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return _T( "" );

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );

        // 這邊 因為 excel 的日期 會存成一個5位數的數字 因此 先判斷是不試日期 再做轉換
        // 你媽的excel
        if( _Worksheet( this )->isDate( iy, ix ) )
        {
            _tstring datetime;
            int year, month, day, hour, min, sec;
            _Workbook( this )->dateUnpack( _Worksheet( this )->readNum( iy, ix ), &year, &month, &day, &hour, &min, &sec );
            _pf_tstrprintf( datetime, _T( "%04d-%02d-%02d %02d:%02d:%02d" ), year, month, day, hour, min, sec );
            return datetime;
        }

        // 有可能是 數值 或 bool
        switch( _Worksheet( this )->cellType( iy, ix ) )
        {
            case libxl::CELLTYPE_STRING:
                {
                    const tchar_t* s = _Worksheet( this )->readStr( iy, ix );
                    if( s != NULL )
                    {
                        return s;
                    }
                    return _T( "" );
                }
                break;
            case libxl::CELLTYPE_NUMBER:
                {
                    libxl::Format* format = NULL;
                    double num = _Worksheet( this )->readNum( iy, ix, &format );
                    if( format == NULL )
                        return  _T( "" );
                    // is integer ?
                    // is double ?
                    if( _ISINT( num ) )
                    {
                        return _pf_lltot( num );
                    }
                    return _pf_ftot( num );
                }
                break;
            case libxl::CELLTYPE_BOOLEAN:
                {
                    libxl::Format* format = NULL;
                    bool bo = _Worksheet( this )->readBool( iy, ix, &format );
                    if( format == NULL )
                        return  _T( "" );
                    return ( bo ) ? _T( "1" ) : _T( "0" );
                }
                break;
            case libxl::CELLTYPE_BLANK:
                {
                    return _T( "" );
                }
                break;
            case libxl::CELLTYPE_ERROR:
            default:
                {
                    const char* err = _Workbook( this )->errorMessage();
                    // 單純空字串?
                    if( strcmp( err, "ok" ) != 0 )
                        Pf_ERROR( err );
                    return _T( "" );
                }
                break;
        }
        return _T( "" );

#endif
    }

    double XlFile::GetCell2Double()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0.0;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );

        // 這邊 因為 excel 的日期 會存成一個5位數的數字 因此 先判斷是不試日期 再做轉換
// 你媽的excel
        if( _Worksheet( this )->isDate( iy, ix ) )
        {
            _tstring datetime;
            int year, month, day, hour, min, sec;
            _Workbook( this )->dateUnpack( _Worksheet( this )->readNum( iy, ix ), &year, &month, &day, &hour, &min, &sec );
            _pf_tstrprintf( datetime, _T( "%04d%02d%02d%02d%02d%02d" ), year, month, day, hour, min, sec );
            return _pf_ttof( datetime );
        }

        return _Worksheet( this )->readNum( iy, ix );
#endif
    }

    void XlFile::SetComment( _IN const tchar_t* string_value, _IN const tchar_t* author, _IN int width, _IN int height )
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__

        {
            for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
            {
                for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
                {
                    _Worksheet( this )->writeComment( _FromIndexValue( yi ), _FromIndexValue( xi ), string_value, author, width, height );
                }
            }
        }
#endif
    }

    _tstring XlFile::GetComment()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return 0;

#ifdef __LIBXL__
        int ix = _FromIndexValue( _Range( this )->left );
        int iy = _FromIndexValue( _Range( this )->top );

        return _Worksheet( this )->readComment( iy, ix );
#endif
    }

    void XlFile::DelComment()
    {
        if( ( !v_workbook_ ) || ( !v_worksheet_ ) )
            return;

#ifdef __LIBXL__

        {
            for( int xi = _Range( this )->left; xi <= _Range( this )->right; xi++ )
            {
                for( int yi = _Range( this )->top; yi <= _Range( this )->bottom; yi++ )
                {
                    _Worksheet( this )->removeComment( _FromIndexValue( yi ), _FromIndexValue( xi ) );
                }
            }
        }
#endif
    }
}
