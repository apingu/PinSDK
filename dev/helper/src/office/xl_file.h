//
//
//
//
//
//
//
//    Linux    /usr/lib64/libxl.so
//
//
#pragma once

#include <pf-def.h>


namespace office
{
    typedef void*    workbook;
    typedef void*    worksheet;
    typedef void*    range;

    enum book_type
    {
        BOOK_TYPE_NONE,
        BOOK_TYPE_XLSX,
        BOOK_TYPE_XLSM,
        BOOK_TYPE_XLS,
    };

    enum cell_type
    {
        /// no value
        CELL_TYPE_EMPTY,
        /// value is a number
        CELL_TYPE_NUMBER,
        /// value is TRUE or FALSE
        CELL_TYPE_STRING,
        /// value is a string shared with other cells to save space
        CELL_TYPE_BOOLEAN,
        ///
        CELL_TYPE_BLANK,
        ///
        CELL_TYPE_ERROR,
        /// value is an ISO 8601 formatted date
        CELL_TYPE_DATA,
        /// value is a string stored in the cell
        //inline_string,
        /// value is a string shared with other cells to save space
        //shared_string,
        /// value is the string result of a formula
        CELL_TYPE_FORMUL
    };

    enum cell_align_h
    {
        CELL_ALIGN_H_GENERAL, 
        CELL_ALIGN_H_LEFT,
        CELL_ALIGN_H_CENTER,
        CELL_ALIGN_H_RIGHT,
        CELL_ALIGN_H_FILL,
        CELL_ALIGN_H_JUSTIFY,
        CELL_ALIGN_H_MERGE,
        CELL_ALIGN_H_DISTRIBUTED,
    };

    enum cell_align_v
    {
        CELL_ALIGN_V_TOP, 
        CELL_ALIGN_V_CENTER, 
        CELL_ALIGN_V_BOTTOM, 
        CELL_ALIGN_V_JUSTIFY,
        CELL_ALIGN_V_DISTRIBUTED,
    };

    //typedef int color_no;
    enum class xlcolor
    {
        COLOR_BLACK = 8, COLOR_WHITE, COLOR_RED, COLOR_BRIGHTGREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_PINK, COLOR_TURQUOISE, COLOR_DARKRED,
        COLOR_GREEN, COLOR_DARKBLUE, COLOR_DARKYELLOW, COLOR_VIOLET, COLOR_TEAL, COLOR_GRAY25, COLOR_GRAY50, COLOR_PERIWINKLE_CF,
        COLOR_PLUM_CF, COLOR_IVORY_CF, COLOR_LIGHTTURQUOISE_CF, COLOR_DARKPURPLE_CF, COLOR_CORAL_CF, COLOR_OCEANBLUE_CF, COLOR_ICEBLUE_CF,
        COLOR_DARKBLUE_CL, COLOR_PINK_CL, COLOR_YELLOW_CL, COLOR_TURQUOISE_CL, COLOR_VIOLET_CL, COLOR_DARKRED_CL, COLOR_TEAL_CL,
        COLOR_BLUE_CL, COLOR_SKYBLUE, COLOR_LIGHTTURQUOISE, COLOR_LIGHTGREEN, COLOR_LIGHTYELLOW, COLOR_PALEBLUE, COLOR_ROSE, COLOR_LAVENDER,
        COLOR_TAN, COLOR_LIGHTBLUE, COLOR_AQUA, COLOR_LIME, COLOR_GOLD, COLOR_LIGHTORANGE, COLOR_ORANGE, COLOR_BLUEGRAY, COLOR_GRAY40,
        COLOR_DARKTEAL, COLOR_SEAGREEN, COLOR_DARKGREEN, COLOR_OLIVEGREEN, COLOR_BROWN, COLOR_PLUM, COLOR_INDIGO, COLOR_GRAY80,
        COLOR_DEFAULT_FOREGROUND = 0x0040, COLOR_DEFAULT_BACKGROUND = 0x0041, COLOR_TOOLTIP = 0x0051, COLOR_NONE = 0x7F, COLOR_AUTO = 0x7FFF
    };

    // sheet 名稱 最多31個字元
#define _MAX_SHEET_NAME 31

#define _INVALID_VALUE  -1

    // 設為這個的 都是從1開始
    typedef int    xlidx_t;


    class XlFile
    {
        workbook             v_workbook_;
        worksheet            v_worksheet_;     // active_sheet
        range                selected_range_;  // selected range or cell
        book_type            book_type_;       // 
        
        _tstring             file_name_;
        _tstring             password_;

        std::set<_tstring>   format_;

    public:

        XlFile();
        ~XlFile();

        //----------------------------------------------------                 
        // Workbook 操作
        bool             New( _IN book_type type = BOOK_TYPE_XLSX );
        bool             Load( _IN const tchar_t* file_name, _IN const tchar_t* password = NULL );                                      // csv 讀入後會轉成xlsx
        bool             Save();
        bool             Save( _IN const tchar_t* file_name, _IN const tchar_t* password = NULL );

        // 暫不支援
        //bool             SaveAs( _IN const tchar_t* filename );                                    // 轉格式
        //void             ExportAs( _IN const tchar_t*, _IN int pagefrom = 1, _IN int pageto = 1 ); //產出的pdf 會從第幾頁到 第幾頁

        book_type        GetBookType() const;
        XlFile&          operator= ( _IN const XlFile& xlf );

        const tchar_t*   GetErrorMessage();

        int              GetSheetTotal();                                                          // 取得 Sheet 個數
        _tcsvector       GetAllSheetNames();                                                       // 取得所有的sheet_name
        xlidx_t          FindSheetIndex( _IN const tchar_t* sheet_name );                          // 由SheetName    取得SheetIndex  1開始  -1不存在

        XlFile*          SelectSheet( _IN xlidx_t sheet_index );                                   // 由SheetIndex   選擇Sheet 失敗() 回傳 NULL
        XlFile*          SelectSheet( _IN const tchar_t* sheet_name );                             // 由SheetName    選擇Sheet 失敗(傳入 空表名 NULL 未建立或讀入) 回傳 NULL

        _tstring         GetSheetName();                                                           // 由SheetIndex   取得SheetName
        xlidx_t          GetSheetIndex();                                                          // 由SheetName    取得SheetIndex

        void             SetSheetName( _IN const tchar_t* sheet_name );                            // 指定SheetName

        XlFile*          SetPassword( _IN const tchar_t* password );                               // 由將sheet 設密碼

        XlFile*          SetHidden( _IN bool t );                                                  // 將sheet 隱藏

        bool             AddSheet( _IN const tchar_t* sheet_name );                                // 新增SheetName  傳回新Sheet位置 在最後面加入一個sheet
        bool             AddSheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter );        // 新增SheetName  傳回新Sheet位置 在指定位置最後面加入一個sheet
        bool             CopySheet( _IN const tchar_t* sheet_name );                               // 複製SheetName  傳回新Sheet位置 指定要複製Sheet名稱
        bool             CopySheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter );       // 複製SheetName  傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet名稱
        bool             DelSheet( _IN xlidx_t sheet_index );                                      // 選SheetIndex   刪除Sheet
        bool             DelSheet( _IN const tchar_t* sheet_name );                                // 選SheetName    刪除Sheet

        void             SetOrientation( _IN short );                                              // 設定版面 2:橫向  1:直向


        //----------------------------------------------------               
        // WorkSheet操作
        xlidx_t          GetColFirstCell();                                                        // 第一個non-empty 起始行
        xlidx_t          GetRowFirstCell();                                                        // 第一個non-empty 起始列
        int              GetColTotalCell();                                                        // 總行數 (沒資料的部分也會被取出) 最後一個non-empty 結束行
        int              GetRowTotalCell();                                                        // 總列數 (沒資料的部分也會被取出) 最後一個non-empty 結束列

        //---------------------------------------------
        //static std::vector<xlidx_t> GetCellNumber( _IN const tchar_t* address );                 // [ col, row ] 
        //static _tstring               GetCellAddress( _IN xlidx_t col, _IN xlidx_t row );
        // If updateNamedRanges is false it doesn't update existing named ranges. Returns false if error occurs.
        bool             InsertRow( _IN int row_first, _IN int row_last, _IN bool update_name_dranges = true );
        bool             InsertCol( _IN int colfirst, _IN int col_last, _IN bool update_name_dranges = true );
        bool             RemoveRow( _IN int row_first, _IN int row_last, _IN bool update_name_dranges = true );
        bool             RemoveCol( _IN int colfirst, _IN int col_last, _IN bool update_name_dranges = true );

        
        double           GetColWidth( _IN int col );
        double           GetRowHeight( _IN int row );

        // -1 for auto
        bool             SetColWidth( _IN int col, _IN double width );
        bool             SetRowHeight( _IN int row, _IN double height );

        bool             SetColHidden( _IN int col, _IN bool hidden );
        bool             SetRowHidden( _IN int row, _IN bool hidden );

 
        //---------------------------------------------
        XlFile*          SelectAllCells();
        // e.g. A1:D5
        XlFile*          SelectRange( _IN const tchar_t* address );
        XlFile*          SelectRange( _IN xlidx_t colfirst, _IN xlidx_t row_first, _IN xlidx_t col_last, _IN xlidx_t row_last );

        XlFile*          SelectCell( _IN const tchar_t* address );
        XlFile*          SelectCell( _IN xlidx_t col, _IN xlidx_t row );
        
        //---------------------------------------------
        int              GetRangeWidth();
        int              GetRangeHeight();

        xlidx_t          GetRangeLeft();
        xlidx_t          GetRangeTop();
        xlidx_t          GetRangeRight();
        xlidx_t          GetRangeBottom();
        _tstring         GetRangeTopLeftAddress();                                                  // 傳回 左上 位置 like "a1"
        _tstring         GetRangeTopRightAddress();
        _tstring         GetRangeBottomLeftAddress();
        _tstring         GetRangeBottomRightAddress();

        void             SetAutoFit();                                                              // range or cell 自動調整寬高

        //---------------------------------------------
        //設定資料進儲存格（存成字串）
        // 搭配 select range 或 select cell
        // e.g.  SelectCell( "C3" )->GetCellAddress();
        //一般版
                         
        _tstring         GetCellAddress();                                                          // 傳回當前欄位名稱
        static _tstring  GetCellAddress( _IN xlidx_t col, _IN xlidx_t row );                        // 傳回欄位名稱
        xlidx_t          GetCellCol();                                                              // 傳回位在的行
        xlidx_t          GetCellRow();                                                              // 傳回位在的列

        bool             ClearCells();                                                              // clear cell by SelectCell or SelectRange
        bool             SetMerge();
        bool             DelMerge();                                                                // 左上

        XlFile*          SetCellColor( _IN xlcolor color );
        XlFile*          SetCellFont( _IN const tchar_t* font_type = _T( "新細明體" ) );             // 設定字型（預設新細明體）
        XlFile*          SetCellFontBold( _IN bool is_bold = true );                                // 粗體
        XlFile*          SetCellFontStrkthrgh( _IN bool is_strkthrgh = true );                      // 刪除線
        XlFile*          SetCellFontSize( _IN short font_size = 12 );                               // 設定字體大小（預設12pt）
        XlFile*          SetCellFontColor( _IN xlcolor color = xlcolor::COLOR_BLACK );              // 字型顏色（預設黑色）

        cell_type        GetCellType();

        _tstring         GetCellFormat();                                                           //  "@";  //設定C欄儲存格格式為文字; "yyyy/MM/dd";  //設定C欄儲存格格式  #,##
        void             SetCellFormat( _IN const tchar_t* fmt_str );                               //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
                                                                                                    // "$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
        void             SetCellFormula( _IN const tchar_t* fmt_str );                              // 設巨集
        _tstring         GetCellFormula();                                                          //

        void             SetCellAlignH( _IN cell_align_h align );                                   // 水平對齊
        cell_align_h     GetCellAlignH();

        void             SetCellAlignV( _IN cell_align_v align );                                   // 垂直對齊
        cell_align_v     GetCellAlignV();

                        
        void             SetCell( _IN bool );
        void             SetCell( _IN int int_value );
        void             SetCell( _IN uint_t uint_value );
        void             SetCell( _IN int64_t lint_value );
        void             SetCell( _IN uint64_t ulint_value );
        void             SetCell( _IN float float_value );
        void             SetCell( _IN double double_value );
        void             SetCell( _IN const tchar_t* string_value );
                         
        int              GetCell2Int();                                                             // 格式必須是 數值才能取出
        _tstring         GetCell2CStr();                                                            // 格式必須是 文字才能取出
        double           GetCell2Double();
                                                                                                    // CString  GetCellForce2CStr();   

        // 註解
        void             SetComment( _IN const tchar_t* string_value, _IN const tchar_t* author, _IN int width = 20, _IN int height = 20 );
        _tstring         GetComment();
        void             DelComment();
                                                                                    //
        //
        //_tstring        GetCellRange2CStr( _IN long c, _IN long r );
    };
}