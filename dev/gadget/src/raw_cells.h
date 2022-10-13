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
#pragma once

#include <string>
#include <vector>
#include <pf-def.h>

//namespace pfget
//{
class RawCells
{
public:

    typedef uint_t        col_idx_t;
    typedef uint_t        row_idx_t;
    typedef _tcsvector    record_t;  // row
    typedef _tcsvector    field_t;   // col
    //typedef _tcsvector    Values;  // row or col

private:

    uint_t                begin_col_;
    uint_t                begin_row_;
    size32_t              max_cols_;    // 最多的行數
    size32_t              max_rows_;    // 最多的列數
    _tcstableii           cells_;       // array of all datas

public:

    RawCells();
    ~RawCells();

    operator _tcstableii& ( ) { return cells_; }
    operator _tcstableii* ( ) { return &cells_; }
    _tcstableii& operator = ( const _tcstableii& tt );
    RawCells&    operator = ( const RawCells& tt );

    // 加到後面
    void         Append( const RawCells& tt );

    // 開始的欄位數 預設從( 1, 1 )
    void         SetBeginCol( _IN uint_t c );
    void         SetBeginRow( _IN uint_t r );

    void         Clear();
    size32_t     GetMaxColSize() const;
    size32_t     GetMaxRowSize() const;

    // 為了要跟excel 同步 所以 從1, 1 開始
    bool         SetCell( _IN col_idx_t col, _IN row_idx_t row, _IN const TCHAR* val );
    const TCHAR* GetCell( _IN col_idx_t col, _IN row_idx_t row ) const;

    // 將傳入的資料 貼到 資料上
    void         Paste( _IN col_idx_t c, _IN row_idx_t r, _IN RawCells* cells );

    // field
    bool         SelectField( _OUT field_t& result, _IN col_idx_t col );
    bool         UpdateField( _IN col_idx_t col, _IN field_t field );
    bool         DeleteField( _IN col_idx_t col );

    // 把第幾行寫入資料
    bool         InsertField( _IN col_idx_t col, _IN field_t field );
    bool         InsertField( _IN col_idx_t col );
    // 加在 max的最後面
    bool         AddnewField( _IN field_t field );
    bool         AddnewField();

    // record
    bool         SelectRecord( _OUT record_t& result, _IN row_idx_t row );
    bool         UpdateRecord( _IN row_idx_t row, _IN record_t record );
    bool         ReplaceRecord( _IN row_idx_t row, _IN record_t record );  // 不存在就建立
    bool         DeleteRecord( _IN row_idx_t row );
    // 把第幾列寫入資料
    bool         InsertRecord( _IN row_idx_t row, _IN record_t record );
    bool         InsertRecord( _IN row_idx_t row );
    bool         AddnewRecord( _IN record_t record );
    bool         AddnewRecord();

    //struct func_sort
    //{
    //    virtual bool operator() ( _tcsvector t1, _tcsvector t2 ) { return false; };
    //};
    //
    //void         Sort( func_sort* rule, int b, int e = -1 ) ;

};
//}