//
//
//
//
//    將excel讀入記憶體中 當成簡單的資料庫使用
//    01) 該物件預設 excel 檔案的第一列為 標題列
//    02) row col 都以 1開始
//
#pragma once

#include <map>
#include <set>
#include <list>
#include <vector>
#include <gadget/raw_cells.h>
#include <template/vmapl.inl>

namespace office
{
    class XlFile;
}


namespace xldb
{
    typedef RawCells::col_idx_t         col_idx_t;
    typedef RawCells::row_idx_t         row_idx_t;
    typedef uint_t                      series_idx_t;    // row or col

    typedef RawCells::record_t          record_t;       // row
    typedef RawCells::field_t           field_t;        // col
    typedef _tcsvector                  series_val_t;   // row or col


    typedef std::vector<col_idx_t>      col_idx_vec_t;
    typedef std::vector<row_idx_t>      row_idx_vec_t;

    //typedef pfstl::vmapl< std::set< QWORD> > ValueCache;  //


    typedef void( _CALLBACK *OnLoadingStep )( int pos_step, _IN int max_step );

    // 因為可以多個 record 或 feild 所以不用 DBrecord 當型態
    //typedef std::vector<Values> Series;  // 取出的結果 可能有多行
    class Series
    {
    private:

        std::vector<series_idx_t> seq_pos_;
        std::vector<series_val_t> seq_val_;

    public:

        series_idx_t  getpos( _IN uint_t idx );
        series_val_t& getval( _IN uint_t idx );
        series_val_t& operator[]( _IN uint_t idx );
        bool          empty();
        uint_t        size();
        void          remove( _IN uint_t idx );  // 移除 所有 series 的一行
        void          push_back( _IN series_idx_t pos, _IN series_val_t& dat );
    };


    class Sheet : public RawCells
    {
    private:
        uint_t                              max_ignore_cols_;              // 多少個欄位都沒有資料 即試為 資料完結 因為 常會有 空白欄為填入在col內
        uint_t                              max_ignore_rows_;              // 多少個欄位都沒有資料 即試為 資料完結 因為 常會有 空白欄為填入在row內
        row_idx_t                           field_row_index_;              // 欄為名稱所在的row
        std::map< _tstring, col_idx_vec_t > field_col_index_;              // 欄位名稱所在的col 如果多個欄位名稱都相同，就會有多個( 欄位名稱, col )

    public:

        Sheet();
        ~Sheet();

        // excel 檔名 以及 catch 第幾個tab
        void            Load( _INOUT office::XlFile* xlsf, _IN const tchar_t* range = NULL );
        bool            Load( _IN const tchar_t* file_name, _IN short sheet_index = 1, _IN const tchar_t* range = NULL );
        //bool          Load( _IN const tchar_t* file_name, _IN const tchar_t* sheet, _IN const tchar_t* cell = NULL );

        // 設定存檔時的相對位移
        // 可以讓資料 貼在另一個檔案的某一個開始位置  傳入""則不做覆蓋 會先將原始資料清除
        //void         SetSavePos( _IN const tchar_t* pos = _T( "A1" ) );

        // 如果 存在的話 可以選擇清除或 直接覆寫在上面 pos 開始的位置 A1
        void            SaveAs( _INOUT office::XlFile* xlsf, _IN const tchar_t* range = NULL );
        void            SaveAs( _IN const tchar_t* file_name, _IN const tchar_t* sheet_name, _IN const tchar_t* range = NULL );
                       
        void            Close();

        //_tstring     GetFilename();
        //_tstring     GetSheetName();

        // load 檔案時  當連續N個欄位是空白的 就不在繼續讀下去
        void            SetLoadBreakOnEmptyCol( uint_t n );

        // 將傳入的資料 貼到 資料上
        //void         Paste( _IN col_idx_t c, _IN row_idx_t r, _IN Sheet* xls );


        // 最大的 行 與 列 數
        //size32_t     GetMaxColSize();
        //size32_t     GetMaxRowSize();

        //void         SetMaxEmptyCol( _IN uint_t c );
        void            SetMaxEmptyRow( _IN uint_t r );


        // 欄位應該只能有一個
        // 將某一列設定為field 可以有多列
        void            SetRowAsField( _IN row_idx_t row_entry = 1 );
        row_idx_t       GetFieldRowIndex();
        void            DelField();

        _tstring        GetFieldName( _IN col_idx_t col );
        // 將field清除
        void            ClearAllField();

        // 取得欄位名稱所在的位置
        col_idx_vec_t   GetFieldNameIndex( _IN const tchar_t* field_name );

        // 更新
        bool            SetCell( _IN col_idx_t col, _IN row_idx_t row, _IN const TCHAR* val );

        //  將某一個欄位設定為key值
        //  若為Primarykey 的欄位 有相同值 只會更新上一筆
        //  因為可以整行寫入所以有primarykey 似乎意義不大 拿掉
        //void SetFieldAsPrimarykey( _IN col_idx_t col );

        //  刪除key值
        //void DelFieldNameKey( const tchar_t* field_name );

        // 將 欄位 名稱 為 field 裡 該 row 的值為 keyname 值的整行資料取出
        // 忘記是要幹嘛用的 所以 先註解起來
        //bool         SelectRowValues( _OUT Series& result, _IN col_idx_t fieldidx, _IN const tchar_t* value );
        //bool         SelectRowValues( _OUT Series& result, _IN const tchar_t* field_name, _IN const tchar_t* value );


        // 取出在某一行中有value 這個值 的所有列的 位置, all 取出全部, 或只取出第一個
        //   0001, AAA, BBB
        //   0003, A1A, BDB
        //   0007, DDD, B22
        //   0009, AA2, BBB
        //  2, DDD  => 3
        //void         SelectRowIdx( _OUT std::vector<row_idx_t>& rows, _IN col_idx_t c, _IN const tchar_t* value, bool _IN all = true );

        // 可能會有多行  可能會有多行是同一個欄位名稱
        //bool SelectColValues( _OUT Series& result, const tchar_t* field_name );

        // 為了要跟excel 同步 所以 從1, 1 開始
        //bool         SetCell( _IN col_idx_t col, _IN row_idx_t row, _IN const tchar_t* data );
        //const tchar_t* GetCell( _IN col_idx_t col, _IN row_idx_t row );
        //
        //// record
        //bool         SelectRecord( _OUT RawCells::record_t& result, _IN row_idx_t row );
        //void         DeleteRecord( _IN row_idx_t row );
        //// 把第幾列寫入資料
        //bool         InsertRecord( _IN row_idx_t row, _IN const RawCells::record_t& record );
        //bool         AddnewRecord( _IN const RawCells::record_t& record );
        //
        //// field
        //bool         SelectField( _OUT field_t& result, _IN col_idx_t col );
        // 可能會有多行  可能會有多行是同一個欄位名稱
        bool         SelectField( _OUT Series& result, _IN const tchar_t* field_name );
        //void         DeleteField( _IN col_idx_t col );
        //// 把第幾行寫入資料
        //void         InsertField( _IN col_idx_t col );
        //void         InsertField( _IN col_idx_t col, _IN const field_t& record );
        //// 加在 max的最後面
        //void         AddnewField( _IN const field_t& record );
    };
}