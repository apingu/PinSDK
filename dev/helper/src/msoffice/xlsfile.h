#pragma once

#include <PfDef.h>


namespace xlnt
{
    class  workbook;
    class  worksheet;
    class  range;
    class  cell;
}


namespace MsOffice
{
    enum cell_type
    {
        /// no value
        empty,
        /// value is TRUE or FALSE
        boolean,
        /// value is an ISO 8601 formatted date
        date,
        /// value is a known error code such as \#VALUE!
        error,
        /// value is a string stored in the cell
        inline_string,
        /// value is a number
        number,
        /// value is a string shared with other cells to save space
        shared_string,
        /// value is the string result of a formula
        formula_string
    };

    struct cell_pos
    {
        uint_t col;
        uint_t row;
    };

    class XlsFile
    {
        xlnt::workbook*      m_pWorkBook;
        xlnt::worksheet*     m_pWorkSheet; // active_sheet
        xlnt::range*         m_pSelectRange;
        xlnt::cell*          m_pSelectCell;
        
        _tstring             m_filename;
        _tstring             m_password;

    public:

        XlsFile();
        ~XlsFile();

        //----------------------------------------------------                 
        // Workbook 操作
        bool             New();
        bool             Load( _IN const tchar_t* filename );
        bool             Load( _IN const tchar_t* filename, _IN const tchar_t* password );
        bool             Save();
        bool             Save( _IN const tchar_t* filename );
        bool             Save( _IN const tchar_t* filename, _IN const tchar_t* password );
                         
        void             ExportAs( _IN const tchar_t*, _IN int pagefrom = 1, _IN int pageto = 1 ); //產出的pdf 會從第幾頁到 第幾頁    

        int              SheetTotal();                                                             //取得 Sheet 個數
        void             SetSheetName( const tchar_t* sheetname );                                 //指定SheetName
        void             SetSheetName( _IN short index, const tchar_t* sheetname );                //由SheetNumber    指定SheetName
        _tstring         GetSheetName( _IN short index );                                          //由SheetNumber    取得SheetName 從 1 開始
        _tstring_vector  GetSheetNames();                                                          //取得所有的sheetname
        short            FindSheetIndex( _IN const tchar_t* );                                     //由SheetName      取得SheetNumber

        bool             AddSheet( _IN const tchar_t* sheetname );                                 //新增SheetName    傳回新Sheet位置 在最後面加入一個sheet
        bool             AddSheet( _IN const tchar_t* sheetname, _IN short indexafter );           //新增SheetName    傳回新Sheet位置 在指定位置最後面加入一個sheet
        bool             CopySheet( _IN const tchar_t* sheetname );                                //複製SheetName    傳回新Sheet位置 指定要複製Sheet名稱
        bool             CopySheet( _IN const tchar_t* sheetname, _IN short indexafter );          //複製SheetName    傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet名稱
        bool             CopySheet( _IN short index );                                             //複製SheetNumber  傳回新Sheet位置 並指定名稱
        bool             CopySheet( _IN short index, _IN short indexafter );                       //複製SheetNumber  傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet位置
        bool             DelSheet( _IN const tchar_t* );                                           //選SheetName      刪除Sheet
        bool             DelSheet( _IN short );                                                    //選SheetNumber    刪除Sheet

        XlsFile*         SelectSheet( _IN const tchar_t* sheetname );                              //由SheetName      選擇Sheet
        XlsFile*         SelectSheet( _IN short index );                                           //由SheetNumber    選擇Sheet
                         
        void             SetOrientation( _IN short );                                              //設定版面 2:橫向  1:直向

        //----------------------------------------------------                 
        // WorkSheet操作
        long             GetColStartCell();                                                        // 第一個non-empty 起始行
        long             GetRowStartCell();                                                        // 第一個non-empty 起始列
        long             GetColTotalCell();                                                        // 總行數 (沒資料的部分也會被取出) 最後一個non-empty 結束行
        long             GetRowTotalCell();                                                        // 總列數 (沒資料的部分也會被取出) 最後一個non-empty 結束列

        //---------------------------------------------
        static cell_pos  GetCellPos( _IN const tchar_t* refstr );
        static _tstring  GetCellRef( _IN const cell_pos pos );

        void             ClearCell();                                                               //clear cell by SelectCell or SelectRange
        void             ClearCell( _IN const tchar_t* refstr );                                    //clear cell by ref
        void             ClearAllCell();                                                            //clear all cell

        //---------------------------------------------
        XlsFile*         SelectRange( _IN const tchar_t* refstr );

        int              GetRangeWidth();
        int              GetRangeHeight();

        _tstring         GetRangeTopLeft();                                                         // 傳回 左上 位置 like "a1"
        _tstring         GetRangeTopRight();
        _tstring         GetRangeBottomLeft();
        _tstring         GetRangeBottomRight();

        //---------------------------------------------
        //設定資料進儲存格（存成字串）
        //一般版
        XlsFile*         SelectCell( _IN const tchar_t* refstr );
        XlsFile*         SelectCell( _IN int col, _IN int row );
                         
        int              GetCellCol();                                                              // 傳回位在的行
        int              GetCellRow();                                                              // 傳回位在的列
                         
        XlsFile*         SetFont( _IN const TCHAR* fontType = _T( "新細明體" ) );                    //設定字型（預設新細明體）
        XlsFile*         SetFontBold( _IN bool isBold = true );                                     //粗體
        XlsFile*         SetFontStrkthrgh( _IN bool isStrkthrgh = true );                           //刪除線
        XlsFile*         SetFontSize( _IN short fontSize = 12 );                                    //設定字體大小（預設12pt）
        XlsFile*         SetFontColor( _IN short colorIndex = 1 );                                  //字型顏色（預設黑色）
        _tstring         GetFormat();                                                               //  "@";  //設定C欄儲存格格式為文字; "yyyy/MM/dd";  //設定C欄儲存格格式  #,##
        void             SetFormat( _IN const TCHAR* );                                             //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
        void             SetFormula( _IN const TCHAR* );                                            // 設巨集
        _tstring         GetFormula();                                                              //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
        void             SetNumberFormat( _IN const TCHAR* );                                       //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
                        
        void             SetCell( _IN bool );
        void             SetCell( _IN int );
        void             SetCell( _IN uint_t );
        void             SetCell( _IN int64_t );
        void             SetCell( _IN uint64_t );
        void             SetCell( _IN float );
        void             SetCell( _IN double );
        void             SetCell( _IN const tchar_t* );
                         
        int              GetCell2Int();                                              //格式必須是 數值才能取出
        _tstring         GetCell2CStr();                                             //格式必須是 文字才能取出
        double           GetCell2Double();
                                                                                    //CString  GetCellForce2CStr();                                      
                                                                                    //
        //
        //_tstring        GetCellRange2CStr( _IN long c, _IN long r );
        


    };
}