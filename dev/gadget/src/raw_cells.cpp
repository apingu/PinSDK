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
//01. == C 系統文件/C++ 系統文件 系統
#include <assert.h>
#include <tchar.h>
#include <algorithm> 

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面 Pf
#include <pf-string.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "raw_cells.h"


//namespace pfget
//{
namespace
{
    // return max_col_size
    // 檢查空間夠不夠 如果不夠 就自動擴展
    // 所以max 應該 只會變大 不會變小
    //void expand_cells( _INOUT std::vector< RawCells::record_t >* cells, _IN uint_t needcols, _IN uint_t needrows )
    //{
    //    // 舊的 會同步擴展所有的
    //    //uint_t max_cols = needcols;
    //    //// 先檢查原本的 cols 夠不夠        
    //    //if( !cells->empty() )
    //    //{
    //    //    // 不夠就要重新分配
    //    //    if( ( *cells  )[ 0 ].size() < needcols )
    //    //    {
    //    //        // 先分配兩倍的量 預留著
    //    //        for( uint_t ri = 0; ri < cells->size(); ri++ )
    //    //        {
    //    //            ( *cells )[ ri ].resize( needcols + 256 );
    //    //        }
    //    //    }
    //    //    max_cols = __max( ( *cells )[ 0 ].size(), needcols );
    //    //}
    //    //
    //    //// 檢查 rows 夠不夠
    //    //if( cells->size() < needrows )
    //    //{
    //    //    // 建立空的等等給擺
    //    //    RawCells::record_t rd;
    //    //    for( uint_t ri = cells->size(); ri < needrows; ri++ )
    //    //    {
    //    //        cells->push_back(rd);
    //    //        cells->back().resize( max_cols );
    //    //    }
    //    //}
    //    ////max_rows = __max( max_rows, needrow );
    //}
}

#define _RAWCELL_ERROR_ROW( row )  ( ( row < begin_row_ ) || ( ( ( size32_t )row - begin_row_ ) >= GetMaxRowSize() ) )? true: false
#define _RAWCELL_ERROR_COL( col )  ( ( col < begin_col_ ) || ( ( ( size32_t )col - begin_col_ ) >= GetMaxColSize() ) )? true: false


//=============================================================================
// NAME:
// DESC:
//=============================================================================
RawCells::RawCells()
{
    begin_row_ = 1;
    begin_col_ = 1;
    max_rows_ = 0;
    max_cols_ = 0;
    //max_cols_ = 0;
    cells_.clear();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
RawCells::~RawCells()
{
    cells_.clear();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
_tcstableii& RawCells::operator = ( const _tcstableii& tt )
{
    begin_col_ = 0;
    begin_row_ = 0;
    for ( uint_t tti = 0; tti < tt.size(); tti++ )
    {
        max_cols_ = __max( max_cols_, tt[ tti ].size() );
    }
    max_rows_ = tt.size();    // 最多的列數
    return cells_ = tt; 
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
RawCells& RawCells::operator = ( const RawCells& tt )
{
    begin_col_ = tt.begin_col_;
    begin_row_ = tt.begin_row_;
    max_cols_  = tt.max_cols_;    // 最多的行數
    max_rows_  = tt.max_rows_;    // 最多的列數
    //max_cols_ = tt.max_cols_;
    cells_ = tt.cells_;
    return *this;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void RawCells::Append( const RawCells& tt )
{
    //begin_col_ = tt.begin_col_;
    //begin_row_ = tt.begin_row_;

    uint_t max_cols = __max( max_cols_, tt.max_cols_ );
    uint_t max_rows = max_rows_ + tt.max_rows_;

    //expand_cells( &cells_, max_cols, max_rows );
    // 這邊是廢話
    cells_.insert( cells_.end(), tt.cells_.begin(), tt.cells_.end() );
    //cells_.resize( __max( cells_.size(), max_rows ) );
    //for( uint_t ri = 0; ri < tt.GetMaxRowSize(); ri++ )
    //{
    //    cells_[ max_rows_ + ri ] = tt.cells_[ ri ];
    //}

    max_cols_ = max_cols;
    max_rows_ = max_rows;
    return;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void RawCells::SetBeginCol( uint_t c )
{
    begin_col_ = c;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void RawCells::SetBeginRow( uint_t r )
{
    begin_row_ = r;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void RawCells::Clear()
{
    cells_.clear();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t RawCells::GetMaxColSize() const
{
    return max_cols_;
    //if( cells_.empty() )
    //    return 0;
    //return cells_[ 0 ].size();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
size32_t RawCells::GetMaxRowSize() const
{
    return max_rows_;
    //return cells_.size();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::SetCell( col_idx_t col, row_idx_t row, const TCHAR* val )
{
    if( ( row < begin_row_ ) || ( col < begin_col_ ) )
    {
        return false;
    }

    row -= begin_row_;
    col -= begin_col_;    

    uint_t max_rows = __max( max_rows_, row + 1 );
    uint_t max_cols = __max( max_cols_, col + 1 );

    //expand_cells( &cells_, max_cols, max_rows );
    if( max_rows > cells_.size() )
        cells_.resize( max_rows );

    if ( max_cols > cells_[ row ].size() )
        cells_[ row ].resize( max_cols );

    cells_[ row ][ col ] = val;

    max_cols_ = max_cols;
    max_rows_ = max_rows;
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const TCHAR* RawCells::GetCell( col_idx_t col, row_idx_t row ) const
{
    //if( _RAWCELL_ERROR_ROW( row ) )
    //    return _T( "" );

    //if( _RAWCELL_ERROR_COL( col ) )
    //    return _T( "" );

    row -= begin_row_;
    col -= begin_col_;

    if( ( row >= cells_.size() ) || ( col >= cells_[ row ].size() ) )
    {
        return _T( "" );
    }
    return cells_[ row ][ col ].c_str();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void RawCells::Paste( col_idx_t c, row_idx_t r, RawCells* cells )
{
    for( uint_t ri = 0; ri < cells->GetMaxRowSize(); ri++ )
    {
        for( uint_t ci = 0; ci < cells->GetMaxColSize(); ci++ )
        {
            const TCHAR* value = cells->GetCell( ci, ri );
            if( _tcscmp( value, _T( "" ) ) != 0 )
            {
                SetCell( c + ci, r + ci, value );
            }
        }
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::SelectField( field_t& result, col_idx_t col )
{
    if( _RAWCELL_ERROR_COL( col ) )
        return false;

    col -= begin_col_;

    for( uint_t ri = 0; ri < max_rows_; ri++ )
    {
        if ( col >= cells_[ ri ].size() )
        {
            result.push_back( _T("") );
        }
        else
        {
            result.push_back( cells_[ ri ][ col ] );
        }        
    }

    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::UpdateField( col_idx_t col, field_t field )
{
    // 要在範圍內
    if( _RAWCELL_ERROR_COL( col ) )
        return false;

    col -= begin_col_;

    //field.resize( cells_.size() );
    for( uint_t ri = 0; ri < max_rows_; ri++ )
    {
        cells_[ ri ].resize( __max( cells_[ ri ].size(), col + 1 ) );
        cells_[ ri ][ col ] = field[ ri ];
    }

    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::DeleteField( col_idx_t col )
{
    if ( _RAWCELL_ERROR_COL( col ) )
        return false;

    col -= begin_col_;

    for( uint_t ri = 0; ri < max_rows_; ri++ )
    {
        if ( col >= cells_[ ri ].size() )
        {
        }
        else
        {
            cells_[ ri ].erase( cells_[ ri ].begin() + col );
        }
    }

    max_cols_--;
    return true;
}

//=============================================================================
// NAME:
// DESC: 把第幾行寫入資料
//=============================================================================
bool RawCells::InsertField( col_idx_t col, field_t field )
{
    if( col < begin_col_ )
        return false;

    col -= begin_col_; 

    uint_t max_rows = __max( max_rows_, field.size() );
    uint_t max_cols = __max( max_cols_ + 1, ( col + 1 )  );

    //expand_cells( &cells_, max_cols, max_rows );

    cells_.resize( max_rows );
    // 這樣如果是插入一個空的底下val 才不會當掉
    field.resize( max_rows ); //field.resize( GetMaxRowSize() );
    for( uint_t ri = 0; ri < max_rows; ri++ )
    {
        const tchar_t* val = field[ ri ].c_str();
        cells_[ ri ].resize( __max( cells_[ ri ].size(), col ) );
        cells_[ ri ].insert( cells_[ ri ].begin() + col, val );
    }

    max_rows_ = max_rows;
    max_cols_ = max_cols;
    
    return true;
}

//=============================================================================
// NAME:
// DESC: 把第幾行寫入資料
//=============================================================================
bool RawCells::InsertField( col_idx_t col )
{
    field_t field;
    return InsertField( col, field );
}

//=============================================================================
// NAME:
// DESC: 加在 max的最後面
//=============================================================================    
bool RawCells::AddnewField( field_t field )
{
    return InsertField( max_rows_, field );
}

//=============================================================================
// NAME:
// DESC: 加在 max的最後面
//=============================================================================    
bool RawCells::AddnewField()
{
    return InsertField( max_rows_ );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::SelectRecord( record_t& result, row_idx_t row )
{
    if( _RAWCELL_ERROR_ROW( row ) )
        return false;

    row -= begin_row_;

    // 可以往後加 不斷的放入結果
    result.insert( result.end(), cells_[ row ].begin(), cells_[ row ].end() );
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::UpdateRecord( row_idx_t row, record_t record )
{
    if( _RAWCELL_ERROR_ROW( row ) )
        return false;

    row -= begin_row_;

    //int max_col = __max( record.size(), GetMaxColSize() );
    //int max_row = __max( row + 1, GetMaxRowSize() );
    //expand_cells( max_cols_, max_rows_, &cells_, record.size(), row );

    //record.resize( cells_[0].size() );
    cells_[ row ] = record;
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::ReplaceRecord( row_idx_t row, record_t record )
{
    row -= begin_row_;

    int max_rows = __max( max_rows_, row + 1 );
    int max_cols = __max( max_cols_, record.size() );

    cells_.resize( __max( cells_.size(), max_rows ) );
    //expand_cells( &cells_, max_cols, max_rows );
    //record.resize( cells_[0].size() );
    cells_[ row ] = record;

    max_rows_ = max_rows;
    max_cols_ = max_cols;    
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::DeleteRecord( row_idx_t row )
{
    if( _RAWCELL_ERROR_ROW( row ) )
        return false;

    row -= begin_row_;
    cells_.erase( cells_.begin() + row );
    max_rows_--;
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::InsertRecord( row_idx_t row, record_t record )
{
    if( row < begin_row_ )
    {
        return false;
    }

    row -= begin_row_;

    int max_rows = __max( max_rows_ + 1, row + 1 );
    int max_cols = __max( max_cols_, record.size() );

    //int max_col = __max( record.size(), GetMaxColSize() );
    //int max_row = __max( row + 1, GetMaxRowSize() );
    //expand_cells( &cells_, record.size(), row + 1 );
    // insert 0就會發生 maxcolsize = 0; 所以後面還是取一次
    //max_cols_ = __max( record.size(), GetMaxColSize() );
    //record.resize( cells_[ 0 ].size() );
    cells_.resize( __max( cells_.size(), max_rows ) );
    cells_.insert( cells_.begin() + row, record );

    max_rows_++;
    max_cols_ = max_cols;
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::InsertRecord( row_idx_t row )
{
    record_t record;
    return InsertRecord( row, record );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::AddnewRecord( record_t record )
{
    //int max_col = __max( record.size(), GetMaxColSize() );
    //expand_cells( &cells_, record.size(), max_rows_ + 1 );
    //max_cols_ = __max( max_cols_, record.size() );
    //record.resize( cells_[ 0 ].size() );
    int max_rows = max_rows_ + 1;
    int max_cols = __max( max_cols_, record.size() );

    cells_.resize( __max( cells_.size(), max_rows ) );
    cells_[ max_rows_ ] = record;

    max_rows_++;
    max_cols_ = max_cols;    
    return true;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool RawCells::AddnewRecord()
{
    record_t record;
    return AddnewRecord( record );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
void RawCells::Sort( func_sort* rule, int b, int e )
{
    if ( e == -1 )
    {
        std::sort( cells_.begin() + b, cells_.end(), *rule );
    }
    else
    {
        std::sort( cells_.begin() + b, cells_.begin() + e, *rule );
    }
}
*/
//}