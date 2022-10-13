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
#include "stdafx.h"

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-system.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
#include <algorithmic/crypt-crc64.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "ExcelFile.h"
#include "xlsdb-sheet.h"


namespace xlsdb
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring GetKeyString( xlsdb::col_idx_t col, const TCHAR* value )
    {
        _tstring keystring;
        _pf_tstrprintf( keystring, _T( "%d.%s" ), col, value );
        return keystring;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    qword_t GetKeyIndex( _tstring key )
    {
        return crc64::Encode( ( const char* )key.c_str(), ( key.size() * sizeof( tchar_t ) ) );
    }

    //_tstring GetKeyStringCol( const TCHAR* value )
    //{
    //    TCHAR colid[_MAX_STRING_LEN];
    //    TCHAR seps[] = _T( "." );
    //    TCHAR *token = NULL;
    //    TCHAR *next_token = NULL;
    //    _tcscpy( colid, value );
    //    token = _tcstok_s( colid, seps, &next_token );
    //    if ( next_token != NULL )
    //        return token;
    //    return _T("");
    //}

    _tstring InsertStringToValueCache( xlsdb::ValueCache* mt, xlsdb::col_idx_t col, xlsdb::col_idx_t row, const TCHAR* value )
    {
        _tstring keystring = GetKeyString( col, value );
        qword_t keyindex = GetKeyIndex( keystring );
        xlsdb::ValueCache::iterator it = mt->find( keyindex );
        //if ( it != mt->end() )
        if( !mt->miss( keyindex, it ) )
        {
            it->second.insert( std::set<QWORD>::value_type( row ) );
        }
        else
        {
            std::set<QWORD> pts;
            pts.insert( std::set<QWORD>::value_type( row ) );
            mt->insert( keyindex, pts );
        }
        return keystring;
    }

    bool RemoveStringToValueCache( xlsdb::ValueCache* mt, xlsdb::col_idx_t col, xlsdb::col_idx_t row, const TCHAR* value )
    {
        _tstring keystring = GetKeyString( col, value );
        qword_t keyindex = GetKeyIndex( keystring );
        xlsdb::ValueCache::iterator it = mt->find( keyindex );
        //if ( it != mt->end() )
        if( !mt->miss( keyindex, it ) )
        {
            it->second.erase( row );
            // 既然都空了 就順道移除 
            if( it->second.empty() )
            {
                mt->erase( keyindex );
            }
        }
        return true;
    }

    // 有沒有存在 這個值
    bool TestStringFrKeymap( _OUT std::set<QWORD>& rv, xlsdb::ValueCache* mt, xlsdb::col_idx_t col, const TCHAR* value )
    {
        _tstring keystring = GetKeyString( col, value );
        qword_t keyindex = GetKeyIndex( keystring );
        xlsdb::ValueCache::iterator it = mt->find( keyindex );
        if( !mt->miss( keyindex, it ) )
        {
            rv = it->second;
            return true;
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //
    series_idx_t Series::getpos( uint_t idx )
    {
        if( idx < SeqPos.size() )
            return SeqPos[ idx ];
        return -1;
    }

    series_val_t& Series::getval( uint_t idx )
    {
        return operator[] ( idx );
    }

    series_val_t& Series::operator[]( uint_t idx )
    {
        if( idx < SeqVal.size() )
            return SeqVal[ idx ];
        return series_val_t();
    }

    bool Series::empty()
    {
        return SeqVal.empty();
    }

    uint_t Series::size()
    {
        return SeqVal.size();
    }

    // 移除 所有 series 的一行
    void Series::remove( uint_t idx )
    {
        for( uint_t t = 0; t < SeqVal.size(); t++ )
        {
            SeqVal[ t ].erase( SeqVal[ t ].begin() + t );
        }
    }

    void Series::push_back( series_idx_t pos, series_val_t &val )
    {
        SeqPos.push_back( pos );
        SeqVal.push_back( val );
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Sheet::Sheet()
    {
        max_ignore_cols_ = 50;
        max_ignore_rows_ = 50;
        m_max_cols_ = 0;
        field_name_row_index_ = -1;
        //m_maxrows = 0;
        is_visible_ = false;
        //m_value_cache.init( 1000 );
        m_LoadingStep = NULL;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Sheet::~Sheet()
    {

    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    struct readxlsargv
    {
        _tstring filename;
        int      sheetidx;
        int      beginrow;
        int      readrowsize;
        Sheet*   pushsheet;
        T_Lock*  utlock;
        T_Lock*  fslock;
        bool     finish;
    };

    void theadfunc_readxlsx( void* argv )
    {
        readxlsargv* xlsargv = ( readxlsargv* )argv;
        TCHAR buff[ 1024 ];

        ExcelFile* xlsf = new ExcelFile;
        xlsf->SetVisible( FALSE );
        if( xlsf->Open( xlsargv->filename.c_str() ) == NULL )
        {
            return;
        }

        if( xlsf->SelectSheet( xlsargv->sheetidx ) == NULL )
        {
            return;
        }

        int bcol = xlsf->GetHorztlStartCell();
        int brow = xlsargv->beginrow;
        int max_col = xlsf->GetHorztlTotalCell();
        int max_row = xlsf->GetVrticlTotalCell();
        int row_s = xlsargv->readrowsize;
        int erow = min( xlsargv->beginrow + row_s - 1, max_row );

        xlsf->SelectCell( bcol, brow, max_col, erow );

        _stprintf_s( buff, _T( "select %d:%d - %d:%d\n" ), bcol, brow, max_col, erow );
        OutputDebugString( buff );

        for( int trow = 1; trow <= row_s; trow++ )
        {
            record_t row_data;
            for( int tcol = 1; tcol <= max_col; tcol++ )
            {
                CString celldata = xlsf->GetCellRange2CStr( tcol, trow );
                row_data.push_back( ( LPCTSTR )celldata );
            }

            {
                TS_Lock lock( xlsargv->utlock );
                xlsargv->pushsheet->InsertRecord( trow, row_data );
            }

            _stprintf_s( buff, _T( "read row: %d/%d\n" ), trow, max_row );
            OutputDebugString( buff );
        }

        SAFE_DELETE( xlsf );

        {
            TS_Lock lock( xlsargv->fslock );
            xlsargv->finish = true;
        }
    }

    void Sheet::Load( ExcelFile* xlsf, const TCHAR* range )
    {
        TCHAR buff[ 1024 ];

        m_max_cols_ = 0;
        //m_maxrows = 0;
        Clear();

        //file_name_ = xlsf->GetFilename();
        //sheet_name_ = xlsf->GetSheetname();

        // 將fiels與對映的col index填入
        if( range != NULL )
        {
            xlsf->SelectCell( range );
        }
        else
        {
            xlsf->SelectAllCell();
        }

        int b_col = xlsf->GetCellCol();
        int b_row = xlsf->GetCellRow();

        int empty_col = 0;

        //int max_col = xlsf->GetRangeCols();
        //int max_row = xlsf->GetRangeRows();

        int max_col = xlsf->GetHorztlTotalCell();
        int max_row = xlsf->GetVrticlTotalCell();

        // 只選取某一範圍
        if( _tcsstr( range, _T( ":" ) ) != 0 )
        {
            max_col = xlsf->GetRangeCols();
            max_row = xlsf->GetRangeRows();
        }


        if( ( max_row == 0 ) || ( max_col == 0 ) )
            return;

        // 開多續讀
        /*
        {
        SAFE_DELETE( xlsf );
        DWORD currt = ::GetTickCount();

        T_Lock utlock;
        T_Lock fslock;
        int threadssize = 4;// ( pfkrl::GetNumberOfProcessors() * 2 ) + 2;
        int beginrowidx = 1;
        int readrowcount = max_row;
        int avgreadcount = max_row / threadssize;
        std::vector<readxlsargv*> argv_vec;
        //while ( readrowcount != 0 )
        for( int ti = 0; ti < threadssize; ti++ )
        {
        readxlsargv* readargv = new readxlsargv;
        readargv->filename = fname;
        readargv->sheetidx = sheetidx;
        readargv->finish = false;
        readargv->utlock = &utlock;
        readargv->fslock = &fslock;
        readargv->pushsheet = this;

        if( ti == (threadssize - 1) )
        readargv->readrowsize = readrowcount;
        else
        readargv->readrowsize = avgreadcount;

        readargv->beginrow = beginrowidx;
        argv_vec.push_back( readargv );
        beginrowidx += readargv->readrowsize;
        readrowcount -= readargv->readrowsize;
        pfkrl::Thread::BeginProcess( theadfunc_readxlsx, readargv, "read xls" );
        }

        for ( int ai = 0; ai < argv_vec.size(); ai++ )
        {
        Sleep( 3000 );
        TS_Lock lock( &fslock );
        if ( argv_vec[ai]->finish != true )
        {
        ai--;
        continue;
        }
        }

        for ( int ai = 0; ai < argv_vec.size(); ai++ )
        {
        SAFE_DELETE( argv_vec[ai] );
        }
        argv_vec.clear();

        DWORD currf = ::GetTickCount() - currt;
        _stprintf_s( buff, _T( "spend time: %d\n" ), currf );
        OutputDebugString( buff );
        }
        */

        {
            DWORD currt = ::GetTickCount();
            xlsf->SelectCell( b_col, b_row, max_col, max_row );

            int empty_row = 0;
            // field的下一列當成是資料的開始
            for( int trow = 1; trow <= ( max_row + 1 - b_row ); trow++ )
            {
                int empty_col = 0;
                CString rowstr;
                record_t row_data;
                for( int tcol = 1; tcol <= ( max_col + 1 - b_col ); tcol++ )
                {
                    //
                    CString celldata = xlsf->GetCellRange2CStr( tcol, trow );
                    //xlsf->SelectCell( tcol, trow );
                    //CString cellformat = xlsf->GetFormat();
                    //CString celldata = xlsf->GetCell2CStr();
                    if( celldata == _T( "" ) )
                    {
                        empty_col++;
                        //  有N格欄位都是空白的 就當成沒有資料了
                        if( empty_col >= max_ignore_cols_ )
                        {
                            break;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    for( int eptc = 0; eptc < empty_col; eptc++ )
                        row_data.push_back( _T( "" ) );
                    row_data.push_back( ( LPCTSTR )celldata );
                    rowstr += celldata;
                    empty_col = 0;
                }

                m_max_cols_ = __max( m_max_cols_, row_data.size() );
                if( rowstr == _T( "" ) )
                {
                    empty_row++;
                    //  有N格欄位都是空白的 就當成沒有資料了
                    if( empty_row >= max_ignore_rows_ )
                    {
                        break;
                    }
                    else
                    {
                        continue;
                    }
                }
                for( int eptr = 0; eptr < empty_row; eptr++ )
                    AddnewRecord( _tcsvector() );
                AddnewRecord( row_data );
                empty_row = 0;

                //for ( int tcol = 1; tcol <= ( max_col + 1 - b_col ); tcol++ )
                //for( uint_t tcol = 1; tcol <= row_data.size(); tcol++ )
                //{
                //    InsertStringToValueCache( &m_value_cache, tcol, trow, GetCell( tcol, trow ) );
                //}

                //m_maxrows = __max( m_maxrows, m_datarow.size() );

                if( m_LoadingStep )
                {
                    m_LoadingStep( trow, max_row );
                }
                _stprintf_s( buff, _T( "read row: %d/%d\n" ), trow, max_row );
                OutputDebugString( buff );
            }


            DWORD currf = ::GetTickCount() - currt;
            _stprintf_s( buff, _T( "spend time: %d\n" ), currf );
            OutputDebugString( buff );
        }


        // 預設第一列為field
        SetRowAsFieldnames( 1 );
    }

    bool Sheet::Load( const TCHAR* fname, short sheetidx, const TCHAR* range )
    {
        //TCHAR buff[1024];

        m_max_cols_ = 0;
        //m_maxrows = 0;
        Clear();

        ExcelFile* xlsf = new ExcelFile;
        xlsf->SetAlert( FALSE );
        if( xlsf->Open( fname ) == NULL )
        {
            //file_name_ = _T( "" );
            return false;
        }

        xlsf->SetVisible( is_visible_ );

        if( xlsf->SelectSheet( sheetidx ) == NULL )
        {
            return false;
        }
        Load( xlsf, range );
        SAFE_DELETE( xlsf );
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    typedef void( *FuncWriteCell )( ExcelFile* xlsf, RawCells* datamap, int sc, int sr );

    void WriteCellP( ExcelFile* xlsf, RawCells* datamap, int sc, int sr )
    {
        // "" 透空
        for( size_t r = 1; r <= datamap->GetMaxRowSize(); r++ )
        {
            for( size_t c = 1; c <= datamap->GetMaxColSize(); c++ )
            {
                if( _tcscmp( datamap->GetCell( c, r ), _T( "" ) ) != 0 )
                {
                    xlsf->SetCellRange( sc + c - 1, sr + r - 1, datamap->GetCell( c, r ) );
                }
            }
        }
    }

    void WriteCellA( ExcelFile* xlsf, RawCells* datamap, int sc, int sr )
    {
        for( size_t r = 1; r <= datamap->GetMaxRowSize(); r++ )
        {
            for( size_t c = 1; c <= datamap->GetMaxColSize(); c++ )
            {
                xlsf->SetCellRange( sc + c - 1, sr + r - 1, datamap->GetCell( c, r ) );
            }
        }
    }

    void Sheet::SaveAs( ExcelFile* xlsf, const TCHAR* range )
    {
        //file_name_ = xlsf->GetFilename();
        //sheet_name_ = xlsf->GetSheetname();

        FuncWriteCell writefunc = NULL;
        int           posx = 1;
        int           posy = 1;
        int           savetype = 0;
        _tstring      poscell;

        if( range != NULL )
        {
            savetype = 1;
            poscell = range;
            if( poscell == _T( "" ) )
            {
                poscell = _T( "A1" );
            }
        }

        switch( savetype )
        {
        case 1:
            {
                writefunc = WriteCellP;
            }
            break;
        default:
            {
                writefunc = WriteCellA;
            }
            break;
        }

        if( savetype != 1 )
        {
            xlsf->SelectAllCell()->ClearCell();
        }
        else
        {
            xlsf->SelectCell( poscell.c_str() );
            posx = xlsf->GetCellCol();
            posy = xlsf->GetCellRow();
        }

        uint_t max_row = max( xlsf->GetVrticlTotalCell(), posy + GetMaxRowSize() );
        uint_t max_col = max( xlsf->GetHorztlTotalCell(), posx + GetMaxColSize() );
        xlsf->SelectCell( 1, 1, max_col, max_row );

        //xlsf->SetNumberFormat( _T( "@" ) );

        writefunc( xlsf, this, posx, posy );

        xlsf->SelectAllCell();
        //xlsf->AutoFitHight();
        //xlsf->AutoFitWidth();

    }

    void Sheet::SaveAs( const TCHAR* fname, const TCHAR* sheet, const TCHAR* range )
    {
        bool op = true;
        ExcelFile* xlsf = new ExcelFile;
        xlsf->SetAlert( FALSE );
        xlsf->SetAlertBeforeOverwriting( FALSE );

        if( xlsf->Open( fname ) != NULL )
        {
            op = true;
        }
        else
        {
            op = false;
            xlsf->New();
        }
        xlsf->SetVisible( is_visible_ );

        short sheetidx = xlsf->FindSheetName( sheet );
        if( sheetidx == -1 )
        {
            // 取代原有的Sheet
            //xlsf->SetSheetName( 1, sheet );
            xlsf->AddSheet( sheet );
        }

        xlsf->SelectSheet( sheet );

        SaveAs( xlsf, range );

        if( op )
            xlsf->FixSave();
        else
            xlsf->SaveAs( fname );
        delete xlsf;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::Close()
    {
        Clear();
        field_name_col_index_.clear();
        //m_value_cache.clear();
        //file_name_.clear();
        //sheet_name_.clear();
        m_max_cols_ = 0;
        field_name_row_index_ = -1;
        //m_maxrows = 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //_tstring Sheet::GetFilename()
    //{
    //    return file_name_;
    //}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //_tstring Sheet::GetSheetname()
    //{
    //    return sheet_name_;
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::SetVisible( bool t )
    {
        is_visible_ = t;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::SetLoadBreakOnEmptyCol( int n )
    {
        max_ignore_rows_ = n;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::SetLoadingStep( func_loading_step func )
    {
        m_LoadingStep = func;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void Sheet::Paste( int c, int r, Sheet* xls )
    //{
    //    for( int ir = 0; ir < xls->GetMaxRowSize(); ir++ )
    //    {
    //        for( int ic = 0; ic < xls->GetMaxColSize(); ic++ )
    //        {
    //            const TCHAR* value = xls->GetCell( ic, ir );
    //            if( _tcscmp( value, _T( "" ) ) != 0 )
    //            {
    //                SetCell( c + ic, r + ic, value );
    //            }
    //        }
    //    }
    //}

    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //size32_t Sheet::GetMaxColSize()
    //{
    //    return m_max_cols_;
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //size32_t Sheet::GetMaxRowSize()
    //{
    //    return m_datarow.size();
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void Sheet::SetMaxEmptyCol( int c )
    //{
    //    max_ignore_cols_ = c;
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::SetMaxEmptyRow( int r )
    {
        max_ignore_cols_ = r;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::SetRowAsFieldnames( xlsdb::row_idx_t row_entry )
    {
        field_name_row_index_ = 0;
        field_name_col_index_.clear();

        //  記住每個欄位名稱所在的idx 加速找尋
        record_t record;
        if( !SelectRecord( record, row_entry ) )
            return;

        for( uint_t ci = 0; ci < record.size(); ci++ )
        {
            _tstring fieldname = record[ci];
            std::map< _tstring, xlsdb::col_idx_vec_t>::iterator it = field_name_col_index_.find( fieldname );
            if( it == field_name_col_index_.end() )
            {
                xlsdb::col_idx_vec_t idxs;
                idxs.push_back( ci + 1 );
                field_name_col_index_.insert( std::map< _tstring, xlsdb::col_idx_vec_t>::value_type( fieldname, idxs ) );
            }
            else
            {
                it->second.push_back( ci + 1 );
            }
        }
        field_name_row_index_ = row_entry;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    row_idx_t Sheet::GetFieldnamesRowIdx()
    {
        return field_name_row_index_ + 1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::DelFieldnames()
    {
        field_name_col_index_.clear();
        field_name_row_index_ = -1;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    _tstring Sheet::GetFieldname( _IN col_idx_t col )
    {
        return GetCell( col, field_name_row_index_ );
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Sheet::ClearAllFieldname()
    {
        field_name_col_index_.clear();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    xlsdb::col_idx_vec_t Sheet::GetFieldnameIdx( const TCHAR* fieldname )
    {
        if( fieldname == NULL )
            return xlsdb::col_idx_vec_t();

        std::map< _tstring, xlsdb::col_idx_vec_t>::iterator it = field_name_col_index_.find( fieldname );
        if( it != field_name_col_index_.end() )
        {
            return it->second;
        }
        return xlsdb::col_idx_vec_t();
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void Sheet::SetFieldAsPrimarykey( _IN col_idx_t col )
    //{
    //    m_primarykeyidx = col;
    //    //m_primarykeyidx.insert( std::map< col_idx_t, _tstring >::value_type( col, GetFieldname( col ) ) );
    //}


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void Sheet::DelFieldNameKey( const TCHAR* fieldname )
    //{
        //m_value_cache.erase( fieldname );
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    /*
    bool Sheet::SelectRowValues( _OUT Series& result, _IN col_idx_t fieldidx, _IN const TCHAR* value )
    {
        _tstring keystr = GetKeyString( fieldidx, value );
        ValueCache::iterator iter = m_value_cache.find( keystr );
        if ( !m_value_cache.miss( keystr, iter ) )
        {
            Values val;
            std::set<QWORD>::iterator posit = iter->second.begin();
            while ( posit != iter->second.end() )
            {
                int col = HIDWORD( *posit );
                int cow = LODWORD( *posit );
                val.push_back( m_datarow[ col - 1 ][ cow - 1 ] );
                posit++;
            }
            result.push_back( fieldidx, val );
        }

        if ( result.empty() )
        {
            return false;
        }
        return true;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool Sheet::SelectRowValues( xlsdb::Series& result, const TCHAR* fieldname, const TCHAR* value )
    {
        col_idx_vec_t colvec = GetFieldnameIdx( fieldname );
        if ( colvec.empty() )
        {
            return false;
        }

        //_TCHAR keybuff[_MAX_STRING_LEN];
        for ( int ci = 0; ci < colvec.size(); ci++ )
        {
            _tstring keystr = GetKeyString( colvec[ ci ], value );
            ValueCache::iterator iter = m_value_cache.find( keystr );
            if ( !m_value_cache.miss( keystr, iter ) )
            {
                Values val;
                std::set<QWORD>::iterator posit = iter->second.begin();
                while ( posit != iter->second.end() )
                {
                    int col = HIDWORD( *posit );
                    int cow = LODWORD( *posit );
                    val.push_back( m_datarow[ col - 1 ][ cow - 1 ] );
                    posit++;
                }
                result.push_back( colvec[ ci ], val );
            }
        }

        if ( result.empty() )
        {
            return false;
        }
        return true;
    }
    */


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    //void Sheet::SelectRowIdx( _OUT std::vector<row_idx_t>& rows, _IN col_idx_t c, _IN const TCHAR* value, bool all )
    //{
    //    _tstring keystring = GetKeyString( c, value );
    //    qword_t keyindex = GetKeyIndex( keystring );
    //    ValueCache::iterator iter = m_value_cache.find( keyindex );
    //    if( !m_value_cache.miss( keyindex, iter ) )
    //    {
    //        Values val;
    //        std::set<QWORD>::iterator posit = iter->second.begin();
    //        if( all )
    //        {
    //            while( posit != iter->second.end() )
    //            {
    //                row_idx_t col = HIDWORD( *posit );
    //                row_idx_t row = LODWORD( *posit );
    //                rows.push_back( row );
    //                posit++;
    //            }
    //        }
    //        else
    //        {
    //            if( posit != iter->second.end() )
    //            {
    //                row_idx_t col = HIDWORD( *posit );
    //                row_idx_t row = LODWORD( *posit );
    //                rows.push_back( row );
    //            }
    //        }
    //    }
    //}

    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //bool Sheet::SetCell( int col, int row, const TCHAR* data )
    //{
    //    if( ( col <= 0 ) || ( row <= 0 ) )
    //    {
    //        return false;
    //    }
    //
    //    col--;
    //    row--;
    //
    //    // 檢查primary key
    //    //{
    //    //    if ( col == m_primarykeyidx )
    //    //    {
    //    //        row_idx_vec_t rv;
    //    //        if ( TestStringFrKeymap( rv, &m_value_cache, m_primarykeyidx, data ) )
    //    //        {
    //    //            return false;
    //    //        }
    //    //    }
    //    //}
    //
    //    if( m_datarow.size() <= row )
    //    {
    //        m_datarow.resize( row + 1 );
    //    }
    //
    //    if( m_datarow[ row ].size() <= col )
    //    {
    //        m_datarow[ row ].resize( col + 1 );
    //    }
    //
    //    RemoveStringToValueCache( &m_value_cache, col + 1, row + 1, m_datarow[ row ][ col ].c_str() );
    //
    //    m_datarow[ row ][ col ] = data;
    //    m_max_cols_ = __max( m_max_cols_, col++ );
    //    //m_maxrows = __max( m_maxrows, row++ );
    //
    //    // 加入到快速索引表
    //    InsertStringToValueCache( &m_value_cache, col + 1, row + 1, data );
    //
    //    // 重寫欄位
    //    if( row == field_name_row_index_ )
    //    {
    //        SetRowAsFieldnames( row + 1 );
    //    }
    //
    //    return true;
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //const TCHAR* Sheet::GetCell( int col, int row )
    //{
    //    row--;
    //    if( ( row < 0 ) || ( m_datarow.size() <= row ) )
    //    {
    //        return _T( "" );
    //    }
    //
    //    col--;
    //    if( ( col < 0 ) || ( m_datarow[ row ].size() <= col ) )
    //    {
    //        return _T( "" );
    //    }
    //
    //    return m_datarow[ row ][ col ].c_str();
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //bool Sheet::SelectRecord( record_t& result, int row )
    //{
    //    row--;
    //    if( ( row < 0 ) || ( row >= m_datarow.size() ) )
    //    {
    //        return false;
    //    }
    //
    //    //result = m_datarow[ row ];
    //    result.insert( result.end(), m_datarow[ row ].begin(), m_datarow[ row ].end() );
    //    return true;
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //void Sheet::DeleteRecord( int row )
    //{
    //    row--;
    //    if( ( row < 0 ) || ( row >= m_datarow.size() ) )
    //    {
    //        return;
    //    }
    //
    //    // 從索引表裡面移除
    //    {
    //        for( int ci = 0; ci < m_datarow[ row ].size(); ci++ )
    //        {
    //            RemoveStringToValueCache( &m_value_cache, ci + 1, row + 1, m_datarow[ row ][ ci ].c_str() );
    //        }
    //    }
    //    m_datarow.erase( m_datarow.begin() + row );
    //    //m_maxrows = m_datarow.size();
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //bool Sheet::InsertRecord( int row, const xlsdb::record_t& record )
    //{
    //    // 檢查primary key
    //    //{
    //    //    if ( m_primarykeyidx >= 1 )
    //    //    {
    //    //        row_idx_vec_t rv;
    //    //        if ( TestStringFrKeymap( rv, &m_value_cache, m_primarykeyidx, record[m_primarykeyidx].c_str() ) )
    //    //        {
    //    //            return false;
    //    //        }
    //    //    }
    //    //}
    //
    //    row--;
    //    if( row < 0 )
    //    {
    //        return false;
    //    }
    //
    //    if( row >= m_datarow.size() )
    //    {
    //        m_datarow.resize( row + 1 );
    //    }
    //
    //    for( int ci = 0; ci < m_datarow[ row ].size(); ci++ )
    //    {
    //        RemoveStringToValueCache( &m_value_cache, ci + 1, row + 1, m_datarow[ row ][ ci ].c_str() );
    //    }
    //
    //    m_datarow[ row ] = record;
    //    m_max_cols_ = __max( m_max_cols_, record.size() );
    //    //m_maxrows = __max( m_maxrows, row );
    //
    //    for( int ci = 0; ci < m_datarow[ row ].size(); ci++ )
    //    {
    //        InsertStringToValueCache( &m_value_cache, ci + 1, row + 1, m_datarow[ row ][ ci ].c_str() );
    //    }
    //
    //    // 重寫欄位
    //    if( row == field_name_row_index_ )
    //    {
    //        SetRowAsFieldnames( row + 1 );
    //    }
    //
    //    return true;
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //bool Sheet::AddnewRecord( const record_t& record )
    //{
    //    return InsertRecord( m_datarow.size() + 1, record );
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //bool Sheet::SelectField( field_t& result, int col )
    //{
    //    if( col <= 0 )
    //        return false;
    //
    //    col--;
    //
    //    for( int ri = 0; ri < m_datarow.size(); ri++ )
    //    {
    //        if( col >= m_datarow[ ri ].size() )
    //        {
    //            result.push_back( _T( "" ) );
    //        }
    //        else
    //        {
    //            result.push_back( m_datarow[ ri ][ col ] );
    //        }
    //    }
    //    return true;
    //}

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool Sheet::SelectField( xlsdb::Series& result, const TCHAR* fieldname )
    {
        xlsdb::col_idx_vec_t colidx = GetFieldnameIdx( fieldname );
        if( colidx.empty() )
        {
            return false;
        }

        for( uint_t ci = 0; ci < colidx.size(); ci++ )
        {
            field_t ret;
            RawCells::SelectField( ret, colidx[ ci ] );
            result.push_back( colidx[ ci ], ret );
        }
        return true;
    }

    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //void Sheet::DeleteField( int col )
    //{
    //    if( col <= 0 )
    //        return;
    //
    //    col--;
    //
    //    //if ( col == m_primarykeyidx )
    //    //{
    //    //    m_primarykeyidx = -1;
    //    //}
    //
    //    m_max_cols_ = 0;
    //    for( int r = 0; r < m_datarow.size(); r++ )
    //    {
    //        for( int ci = 0; ci < m_datarow[ r ].size(); ci++ )
    //        {
    //            RemoveStringToValueCache( &m_value_cache, ci + 1, r + 1, m_datarow[ r ][ ci ].c_str() );
    //        }
    //        m_datarow[ r ].erase( m_datarow[ r ].begin() + col );
    //        m_max_cols_ = __max( m_max_cols_, m_datarow[ r ].size() );
    //    }
    //
    //    return;
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //void Sheet::InsertField( col_idx_t col )
    //{
    //    if( col <= 0 )
    //        return;
    //    col--;
    //    m_max_cols_ = 0;
    //    for( int ri = 0; ri < m_datarow.size(); ri++ )
    //    {
    //        m_datarow[ ri ].insert( m_datarow[ ri ].begin() + col, _T( "" ) );
    //        m_max_cols_ = __max( m_max_cols_, m_datarow[ ri ].size() );
    //    }
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //void Sheet::InsertField( int col, const xlsdb::field_t& record )
    //{
    //    if( col <= 0 )
    //        return;
    //    col--;
    //    if( record.size() > m_datarow.size() )
    //    {
    //        m_datarow.resize( record.size() );
    //    }
    //
    //    m_max_cols_ = 0;
    //    for( int ci = 0; ci < record.size(); ci++ )
    //    {
    //        if( col >= m_datarow[ ci ].size() )
    //        {
    //            m_datarow[ ci ].resize( col + 1 );
    //        }
    //
    //        RemoveStringToValueCache( &m_value_cache, ci + 1, col + 1, m_datarow[ ci ][ col ].c_str() );
    //
    //        m_datarow[ ci ][ col ] = record[ ci ];
    //        m_max_cols_ = __max( m_max_cols_, m_datarow[ ci ].size() );
    //
    //        InsertStringToValueCache( &m_value_cache, ci + 1, col + 1, m_datarow[ ci ][ col ].c_str() );
    //
    //        // 重寫欄位
    //        if( ci == field_name_row_index_ )
    //        {
    //            SetRowAsFieldnames( ci + 1 );
    //        }
    //    }
    //}
    //
    ////=============================================================================
    ////  Name :
    ////  Desc :
    ////=============================================================================
    //void Sheet::AddnewField( const xlsdb::field_t& record )
    //{
    //    InsertField( m_max_cols_ + 1, record );
    //}

}
