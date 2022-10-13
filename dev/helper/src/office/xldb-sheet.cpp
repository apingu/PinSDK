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


//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-system.h>
#include <pf-thread.h>
#include <pf-stdlib.h>
#include <algorithmic/crypt-crc64.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xl_file.h"
#include "xldb-sheet.h"


namespace xldb
{
    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //_tstring GetKeyString( xldb::col_idx_t col, const tchar_t* value )
    //{
    //    _tstring keystring;
    //    _pf_tstrprintf( keystring, _T( "%d.%s" ), col, value );
    //    return keystring;
    //}
    //
    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //qword_t GetKeyIndex( _tstring key )
    //{
    //    return crc64::Encode( ( const char* )key.c_str(), ( key.size() * sizeof( tchar_t ) ) );
    //}
    //
    //_tstring GetKeyStringCol( const tchar_t* value )
    //{
    //    tchar_t colid[_MAX_STRING_LEN];
    //    tchar_t seps[] = _T( "." );
    //    tchar_t *token = NULL;
    //    tchar_t *next_token = NULL;
    //    _tcscpy( colid, value );
    //    token = _tcstok_s( colid, seps, &next_token );
    //    if ( next_token != NULL )
    //        return token;
    //    return _T("");
    //}
    //
    //_tstring InsertStringToValueCache( xldb::ValueCache* mt, xldb::col_idx_t col, xldb::col_idx_t row, const tchar_t* value )
    //{
    //    _tstring keystring = GetKeyString( col, value );
    //    qword_t keyindex = GetKeyIndex( keystring );
    //    xldb::ValueCache::iterator it = mt->find( keyindex );
    //    //if ( it != mt->end() )
    //    if( !mt->miss( keyindex, it ) )
    //    {
    //        it->second.insert( std::set<QWORD>::value_type( row ) );
    //    }
    //    else
    //    {
    //        std::set<QWORD> pts;
    //        pts.insert( std::set<QWORD>::value_type( row ) );
    //        mt->insert( keyindex, pts );
    //    }
    //    return keystring;
    //}
    //
    //bool RemoveStringToValueCache( xldb::ValueCache* mt, xldb::col_idx_t col, xldb::col_idx_t row, const tchar_t* value )
    //{
    //    _tstring keystring = GetKeyString( col, value );
    //    qword_t keyindex = GetKeyIndex( keystring );
    //    xldb::ValueCache::iterator it = mt->find( keyindex );
    //    //if ( it != mt->end() )
    //    if( !mt->miss( keyindex, it ) )
    //    {
    //        it->second.erase( row );
    //        // 既然都空了 就順道移除 
    //        if( it->second.empty() )
    //        {
    //            mt->erase( keyindex );
    //        }
    //    }
    //    return true;
    //}
    //
    //// 有沒有存在 這個值
    //bool TestStringFrKeymap( _OUT std::set<QWORD>& rv, xldb::ValueCache* mt, xldb::col_idx_t col, const tchar_t* value )
    //{
    //    _tstring keystring = GetKeyString( col, value );
    //    qword_t keyindex = GetKeyIndex( keystring );
    //    xldb::ValueCache::iterator it = mt->find( keyindex );
    //    if( !mt->miss( keyindex, it ) )
    //    {
    //        rv = it->second;
    //        return true;
    //    }
    //    return false;
    //}

    //////////////////////////////////////////////////////////////////////////
    //
    series_idx_t Series::getpos( uint_t idx )
    {
        if( idx < seq_pos_.size() )
            return seq_pos_[ idx ];
        return -1;
    }

    series_val_t& Series::getval( uint_t idx )
    {
        return operator[] ( idx );
    }

    series_val_t& Series::operator[]( uint_t idx )
    {
        if( idx < seq_val_.size() )
            return seq_val_[ idx ];
        series_val_t series;
        return series;
    }

    bool Series::empty()
    {
        return seq_val_.empty();
    }

    uint_t Series::size()
    {
        return seq_val_.size();
    }

    // 移除 所有 series 的一行
    void Series::remove( uint_t idx )
    {
        for( uint_t t = 0; t < seq_val_.size(); t++ )
        {
            seq_val_[ t ].erase( seq_val_[ t ].begin() + t );
        }
    }

    void Series::push_back( series_idx_t pos, series_val_t &val )
    {
        seq_pos_.push_back( pos );
        seq_val_.push_back( val );
    };

    //////////////////////////////////////////////////////////////////////////
    //
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Sheet::Sheet()
    {
        max_ignore_cols_ = 50;
        max_ignore_rows_ = 50;
        //m_max_cols_ = 0;
        field_row_index_ = -1;
        field_row_index_ = 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Sheet::~Sheet()
    {

    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    struct readxlsargv
    {
        _tstring filename;
        int      sheet_index;
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
        tchar_t buff[ 1024 ];

        office::XlFile* xlsf = new office::XlFile;
        //xlsf->SetVisible( FALSE );
        if( !xlsf->Load( xlsargv->filename.c_str() ) )
        {
            return;
        }

        if( xlsf->SelectSheet( xlsargv->sheet_index ) == NULL )
        {
            return;
        }

        int bcol = xlsf->GetColFirstCell();
        int brow = xlsargv->beginrow;
        int max_col = xlsf->GetColTotalCell();
        int max_row = xlsf->GetRowTotalCell();
        int row_s = xlsargv->readrowsize;
        int erow = __min( xlsargv->beginrow + row_s - 1, max_row );

        //xlsf->SelectCell( bcol, brow, max_col, erow );

        _stprintf( buff, _T( "select %d:%d - %d:%d\n" ), bcol, brow, max_col, erow );
#ifdef _WINDOWS
        //OutputDebugString( buff );
#endif

        for( int trow = brow; trow <= row_s; trow++ )
        {
            record_t row_data;
            for( int tcol = bcol; tcol <= max_col; tcol++ )
            {
                _tstring celldata = xlsf->SelectCell( tcol, trow )->GetCell2CStr();
                row_data.push_back( celldata );
            }

            {
                TS_Lock lock( xlsargv->utlock );
                xlsargv->pushsheet->InsertRecord( trow, row_data );
            }

            _stprintf( buff, _T( "read row: %d/%d\n" ), trow, max_row );
#ifdef _WINDOWS
            //OutputDebugString( buff );
#endif
        }

        SAFE_DELETE( xlsf );

        {
            TS_Lock lock( xlsargv->fslock );
            xlsargv->finish = true;
        }
    }

    void Sheet::Load( office::XlFile* xlsf, const tchar_t* range )
    {
        tchar_t buff[ 1024 ];

        //m_max_cols_ = 0;
        //m_maxrows = 0;
        Clear();

        // 將fiels與對映的col index填入
        if( range != NULL )
        {
            xlsf->SelectRange( range );
        }
        else
        {
            xlsf->SelectAllCells();
        }

        //int empty_col = 0;
        int b_col = xlsf->GetCellCol();
        int b_row = xlsf->GetCellRow();

        int max_col = xlsf->GetColTotalCell();
        int max_row = xlsf->GetRowTotalCell();
        // 只選取某一範圍
        if( _tcsstr( range, _T( ":" ) ) != 0 )
        {
            max_col = xlsf->GetRangeWidth();
            max_row = xlsf->GetRangeHeight();
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
        readargv->filename = file_name;
        readargv->sheet_index = sheet_index;
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
            //long64_t currt = ::GetTickCount();
            uint_t empty_row = 0;
            // field的下一列當成是資料的開始
            //for( int trow = b_row; trow <= ( max_row + 1 - b_row ); trow++ )
            for( int trow = b_row; trow <= max_row; trow++ )
            {
                uint_t empty_col = 0;
                _tstring rowstr;
                record_t row_data;
                //for( int tcol = b_col; tcol <= ( max_col + 1 - b_col ); tcol++ )
                for( int tcol = b_col; tcol <= max_col; tcol++ )
                {
                    //
                    _tstring celldata = xlsf->SelectCell( tcol, trow )->GetCell2CStr();
                    if( celldata == _T( "" ) )
                    {
                        empty_col++;
                        //  有N格連續欄位都是空白的 就當成沒有資料了
                        if( empty_col >= max_ignore_cols_ )
                        {
                            break;
                        }
                        //else
                        //{
                        //    continue;
                        //}
                    }
                    else
                    {
                        empty_col = 0;
                    }
                    //for( uint_t eptc = 0; eptc < empty_col; eptc++ )
                    //    row_data.push_back( _T( "" ) );
                    row_data.push_back( celldata );
                    rowstr += celldata;                    
                }

                //m_max_cols_ = __max( m_max_cols_, row_data.size() );
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
                for( uint_t eptr = 0; eptr < empty_row; eptr++ )
                    AddnewRecord( _tcsvector() );
                AddnewRecord( row_data );
                empty_row = 0;

                PfTrace::Footprint::OutputString( __FUNCTION__, PfTrace::TRACE_INFO, ( "" ), max_row, trow );
                _stprintf( buff, _T( "read row: %d/%d\n" ), trow, max_row );
#ifdef _WINDOWS
                //OutputDebugString( buff );
#endif
            }

            //long64_t currf = ::GetTickCount() - currt;
            //_stprintf( buff, _T( "spend time: %lld\n" ), currf );
#ifdef _WINDOWS
            //OutputDebugString( buff );
#endif
        }
        // 預設第一列為field
        SetRowAsField( 1 );
    }

    bool Sheet::Load( const tchar_t* file_name, short sheet_index, const tchar_t* range )
    {
        //tchar_t buff[1024];

        //m_max_cols_ = 0;
        //m_maxrows = 0;
        Clear();

        office::XlFile* xlsf = new office::XlFile;
        if( !xlsf->Load( file_name ) )
        {
            //file_name_ = _T( "" );
            return false;
        }

        if( xlsf->SelectSheet( sheet_index ) == NULL )
        {
            return false;
        }
        Load( xlsf, range );
        SAFE_DELETE( xlsf );
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    typedef void( _CALLBACK *OnFuncWriteCell )( office::XlFile* xlsf, RawCells* datamap, int sc, int sr );

    void WriteCellP( office::XlFile* xlsf, RawCells* datamap, int sc, int sr )
    {
        //uint_t max_row = max( xlsf->GetRowTotalCell(), sc + datamap->GetMaxRowSize() );
        //uint_t max_col = max( xlsf->GetColTotalCell(), sr + datamap->GetMaxColSize() );
        // "" 透空
        for( size32_t r = 1; r <= datamap->GetMaxRowSize(); r++ )
        {
            for( size32_t c = 1; c <= datamap->GetMaxColSize(); c++ )
            {
                if( _tcscmp( datamap->GetCell( c, r ), _T( "" ) ) != 0 )
                {
                    xlsf->SelectCell( sc + ( c - 1 ), sr + ( r - 1 ) )->SetCell( datamap->GetCell( c, r ) );
                }
            }
        }
    }

    void WriteCellA( office::XlFile* xlsf, RawCells* datamap, int sc, int sr )
    {
        //uint_t max_row = max( xlsf->GetRowTotalCell(), posy + datamap->GetMaxRowSize() );
        //uint_t max_col = max( xlsf->GetColTotalCell(), posx + datamap->GetMaxColSize() );
        for( size32_t r = 1; r <= datamap->GetMaxRowSize(); r++ )
        {
            for( size32_t c = 1; c <= datamap->GetMaxColSize(); c++ )
            {
                xlsf->SelectCell( sc + ( c - 1 ), sr + ( r - 1 ) )->SetCell( datamap->GetCell( c, r ) );
            }
        }
    }

    void Sheet::SaveAs( office::XlFile* xlsf, const tchar_t* range )
    {
        OnFuncWriteCell writefunc = NULL;
        int             posx = 1;
        int             posy = 1;
        int             savetype = 0;
        _tstring        poscell;

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
            xlsf->SelectAllCells()->ClearCells();
        }
        else
        {
            xlsf->SelectCell( poscell.c_str() );
            posx = xlsf->GetCellCol();
            posy = xlsf->GetCellRow();
        }

        //uint_t max_row = max( xlsf->GetRowTotalCell(), posy + GetMaxRowSize() );
        //uint_t max_col = max( xlsf->GetColTotalCell(), posx + GetMaxColSize() );
        //xlsf->SelectRange( 1, 1, max_col, max_row );
        writefunc( xlsf, this, posx, posy );

        xlsf->SelectAllCells();
    }

    void Sheet::SaveAs( const tchar_t* file_name, const tchar_t* sheet_name, const tchar_t* range )
    {

        office::XlFile* xlsf = new office::XlFile;
        if( !xlsf->Load( file_name ) )
        {
            xlsf->New();
        }

        office::xlidx_t sheet_index = xlsf->FindSheetIndex( sheet_name );
        if( sheet_index == -1 )
        {
            // 取代原有的Sheet
            //xlsf->SetSheetName( 1, sheet );
            xlsf->AddSheet( sheet_name );
        }

        xlsf->SelectSheet( sheet_name );

        SaveAs( xlsf, range );

        xlsf->Save( file_name );
        delete xlsf;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::Close()
    {
        Clear();
        field_col_index_.clear();
        //m_value_cache.clear();
        //file_name_.clear();
        //sheet_name_.clear();
        //m_max_cols_ = 0;
        field_row_index_ = -1;
        //m_maxrows = 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //_tstring Sheet::GetFilename()
    //{
    //    return file_name_;
    //}


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //_tstring Sheet::GetSheetName()
    //{
    //    return sheet_name_;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::SetLoadBreakOnEmptyCol( uint_t n )
    {
        max_ignore_rows_ = n;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void Sheet::Paste( int c, int r, Sheet* xls )
    //{
    //    for( int ir = 0; ir < xls->GetMaxRowSize(); ir++ )
    //    {
    //        for( int ic = 0; ic < xls->GetMaxColSize(); ic++ )
    //        {
    //            const tchar_t* value = xls->GetCell( ic, ir );
    //            if( _tcscmp( value, _T( "" ) ) != 0 )
    //            {
    //                SetCell( c + ic, r + ic, value );
    //            }
    //        }
    //    }
    //}

    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //size32_t Sheet::GetMaxColSize()
    //{
    //    return m_max_cols_;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //size32_t Sheet::GetMaxRowSize()
    //{
    //    return m_datarow.size();
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void Sheet::SetMaxEmptyCol( int c )
    //{
    //    max_ignore_cols_ = c;
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::SetMaxEmptyRow( uint_t r )
    {
        max_ignore_cols_ = r;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::SetRowAsField( xldb::row_idx_t row_entry )
    {
        field_row_index_ = 0;
        field_col_index_.clear();

        //  記住每個欄位名稱所在的idx 加速找尋
        record_t record;
        if( !SelectRecord( record, row_entry ) )
            return;

        for( uint_t ci = 0; ci < record.size(); ci++ )
        {
            _tstring field_name = record[ ci ];
            std::map< _tstring, xldb::col_idx_vec_t>::iterator it = field_col_index_.find( field_name );
            if( it == field_col_index_.end() )
            {
                xldb::col_idx_vec_t idxs;
                idxs.push_back( ci + 1 );
                field_col_index_.insert( std::map< _tstring, xldb::col_idx_vec_t>::value_type( field_name, idxs ) );
            }
            else
            {
                it->second.push_back( ci + 1 );
            }
        }
        field_row_index_ = row_entry;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    row_idx_t Sheet::GetFieldRowIndex()
    {
        return field_row_index_ + 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::DelField()
    {
        field_col_index_.clear();
        field_row_index_ = -1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _tstring Sheet::GetFieldName( _IN col_idx_t col )
    {
        return GetCell( col, field_row_index_ );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sheet::ClearAllField()
    {
        field_col_index_.clear();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    xldb::col_idx_vec_t Sheet::GetFieldNameIndex( const tchar_t* field_name )
    {
        if( field_name == NULL )
            return xldb::col_idx_vec_t();

        std::map< _tstring, xldb::col_idx_vec_t>::iterator it = field_col_index_.find( field_name );
        if( it != field_col_index_.end() )
        {
            return it->second;
        }
        return xldb::col_idx_vec_t();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Sheet::SetCell( _IN col_idx_t col, _IN row_idx_t row, _IN const TCHAR* val )
    {
        bool ret = RawCells::SetCell( col, row, val );
        if ( row == field_row_index_ )
        {
            SetRowAsField( row );
        }
        return ret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void Sheet::SetFieldAsPrimarykey( _IN col_idx_t col )
    //{
    //    m_primarykeyidx = col;
    //    //m_primarykeyidx.insert( std::map< col_idx_t, _tstring >::value_type( col, GetFieldName( col ) ) );
    //}


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    //void Sheet::DelFieldNameKey( const tchar_t* field_name )
    //{
        //m_value_cache.erase( field_name );
    //}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    bool Sheet::SelectRowValues( _OUT Series& result, _IN col_idx_t fieldidx, _IN const tchar_t* value )
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
    // NAME:
    // DESC:
    //=============================================================================
    bool Sheet::SelectRowValues( xldb::Series& result, const tchar_t* field_name, const tchar_t* value )
    {
        col_idx_vec_t colvec = GetFieldNameIndex( field_name );
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
    // NAME:
    // DESC:
    //=============================================================================
    //void Sheet::SelectRowIdx( _OUT std::vector<row_idx_t>& rows, _IN col_idx_t c, _IN const tchar_t* value, bool all )
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
    //// NAME:
    //// DESC:
    ////=============================================================================
    //bool Sheet::SetCell( int col, int row, const tchar_t* data )
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
    //    if( row == field_row_index_ )
    //    {
    //        SetRowAsField( row + 1 );
    //    }
    //
    //    return true;
    //}
    //
    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //const tchar_t* Sheet::GetCell( int col, int row )
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
    //// NAME:
    //// DESC:
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
    //// NAME:
    //// DESC:
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
    //// NAME:
    //// DESC:
    ////=============================================================================
    //bool Sheet::InsertRecord( int row, const xldb::record_t& record )
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
    //    if( row == field_row_index_ )
    //    {
    //        SetRowAsField( row + 1 );
    //    }
    //
    //    return true;
    //}
    //
    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //bool Sheet::AddnewRecord( const record_t& record )
    //{
    //    return InsertRecord( m_datarow.size() + 1, record );
    //}
    //
    ////=============================================================================
    //// NAME:
    //// DESC:
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
    // NAME:
    // DESC:
    //=============================================================================
    bool Sheet::SelectField( xldb::Series& result, const tchar_t* field_name )
    {
        xldb::col_idx_vec_t colidx = GetFieldNameIndex( field_name );
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
    //// NAME:
    //// DESC:
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
    //// NAME:
    //// DESC:
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
    //// NAME:
    //// DESC:
    ////=============================================================================
    //void Sheet::InsertField( int col, const xldb::field_t& record )
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
    //        if( ci == field_row_index_ )
    //        {
    //            SetRowAsField( ci + 1 );
    //        }
    //    }
    //}
    //
    ////=============================================================================
    //// NAME:
    //// DESC:
    ////=============================================================================
    //void Sheet::AddnewField( const xldb::field_t& record )
    //{
    //    InsertField( m_max_cols_ + 1, record );
    //}

}
