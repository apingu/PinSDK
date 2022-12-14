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

    // sheet ?????? ??????31?????????
#define _MAX_SHEET_NAME 31

#define _INVALID_VALUE  -1

    // ??????????????? ?????????1??????
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
        // Workbook ??????
        bool             New( _IN book_type type = BOOK_TYPE_XLSX );
        bool             Load( _IN const tchar_t* file_name, _IN const tchar_t* password = NULL );                                      // csv ??????????????????xlsx
        bool             Save();
        bool             Save( _IN const tchar_t* file_name, _IN const tchar_t* password = NULL );

        // ????????????
        //bool             SaveAs( _IN const tchar_t* filename );                                    // ?????????
        //void             ExportAs( _IN const tchar_t*, _IN int pagefrom = 1, _IN int pageto = 1 ); //?????????pdf ?????????????????? ?????????

        book_type        GetBookType() const;
        XlFile&          operator= ( _IN const XlFile& xlf );

        const tchar_t*   GetErrorMessage();

        int              GetSheetTotal();                                                          // ?????? Sheet ??????
        _tcsvector       GetAllSheetNames();                                                       // ???????????????sheet_name
        xlidx_t          FindSheetIndex( _IN const tchar_t* sheet_name );                          // ???SheetName    ??????SheetIndex  1??????  -1?????????

        XlFile*          SelectSheet( _IN xlidx_t sheet_index );                                   // ???SheetIndex   ??????Sheet ??????() ?????? NULL
        XlFile*          SelectSheet( _IN const tchar_t* sheet_name );                             // ???SheetName    ??????Sheet ??????(?????? ????????? NULL ??????????????????) ?????? NULL

        _tstring         GetSheetName();                                                           // ???SheetIndex   ??????SheetName
        xlidx_t          GetSheetIndex();                                                          // ???SheetName    ??????SheetIndex

        void             SetSheetName( _IN const tchar_t* sheet_name );                            // ??????SheetName

        XlFile*          SetPassword( _IN const tchar_t* password );                               // ??????sheet ?????????

        XlFile*          SetHidden( _IN bool t );                                                  // ???sheet ??????

        bool             AddSheet( _IN const tchar_t* sheet_name );                                // ??????SheetName  ?????????Sheet?????? ????????????????????????sheet
        bool             AddSheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter );        // ??????SheetName  ?????????Sheet?????? ????????????????????????????????????sheet
        bool             CopySheet( _IN const tchar_t* sheet_name );                               // ??????SheetName  ?????????Sheet?????? ???????????????Sheet??????
        bool             CopySheet( _IN const tchar_t* sheet_name, _IN xlidx_t indexafter );       // ??????SheetName  ?????????Sheet?????? ????????????Sheet??????????????????????????????Sheet??????
        bool             DelSheet( _IN xlidx_t sheet_index );                                      // ???SheetIndex   ??????Sheet
        bool             DelSheet( _IN const tchar_t* sheet_name );                                // ???SheetName    ??????Sheet

        void             SetOrientation( _IN short );                                              // ???????????? 2:??????  1:??????


        //----------------------------------------------------               
        // WorkSheet??????
        xlidx_t          GetColFirstCell();                                                        // ?????????non-empty ?????????
        xlidx_t          GetRowFirstCell();                                                        // ?????????non-empty ?????????
        int              GetColTotalCell();                                                        // ????????? (?????????????????????????????????) ????????????non-empty ?????????
        int              GetRowTotalCell();                                                        // ????????? (?????????????????????????????????) ????????????non-empty ?????????

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
        _tstring         GetRangeTopLeftAddress();                                                  // ?????? ?????? ?????? like "a1"
        _tstring         GetRangeTopRightAddress();
        _tstring         GetRangeBottomLeftAddress();
        _tstring         GetRangeBottomRightAddress();

        void             SetAutoFit();                                                              // range or cell ??????????????????

        //---------------------------------------------
        //??????????????????????????????????????????
        // ?????? select range ??? select cell
        // e.g.  SelectCell( "C3" )->GetCellAddress();
        //?????????
                         
        _tstring         GetCellAddress();                                                          // ????????????????????????
        static _tstring  GetCellAddress( _IN xlidx_t col, _IN xlidx_t row );                        // ??????????????????
        xlidx_t          GetCellCol();                                                              // ??????????????????
        xlidx_t          GetCellRow();                                                              // ??????????????????

        bool             ClearCells();                                                              // clear cell by SelectCell or SelectRange
        bool             SetMerge();
        bool             DelMerge();                                                                // ??????

        XlFile*          SetCellColor( _IN xlcolor color );
        XlFile*          SetCellFont( _IN const tchar_t* font_type = _T( "????????????" ) );             // ????????????????????????????????????
        XlFile*          SetCellFontBold( _IN bool is_bold = true );                                // ??????
        XlFile*          SetCellFontStrkthrgh( _IN bool is_strkthrgh = true );                      // ?????????
        XlFile*          SetCellFontSize( _IN short font_size = 12 );                               // ???????????????????????????12pt???
        XlFile*          SetCellFontColor( _IN xlcolor color = xlcolor::COLOR_BLACK );              // ??????????????????????????????

        cell_type        GetCellType();

        _tstring         GetCellFormat();                                                           //  "@";  //??????C???????????????????????????; "yyyy/MM/dd";  //??????C??????????????????  #,##
        void             SetCellFormat( _IN const tchar_t* fmt_str );                               //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
                                                                                                    // "$0.00",  #,##0 (?????????)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
        void             SetCellFormula( _IN const tchar_t* fmt_str );                              // ?????????
        _tstring         GetCellFormula();                                                          //

        void             SetCellAlignH( _IN cell_align_h align );                                   // ????????????
        cell_align_h     GetCellAlignH();

        void             SetCellAlignV( _IN cell_align_v align );                                   // ????????????
        cell_align_v     GetCellAlignV();

                        
        void             SetCell( _IN bool );
        void             SetCell( _IN int int_value );
        void             SetCell( _IN uint_t uint_value );
        void             SetCell( _IN int64_t lint_value );
        void             SetCell( _IN uint64_t ulint_value );
        void             SetCell( _IN float float_value );
        void             SetCell( _IN double double_value );
        void             SetCell( _IN const tchar_t* string_value );
                         
        int              GetCell2Int();                                                             // ??????????????? ??????????????????
        _tstring         GetCell2CStr();                                                            // ??????????????? ??????????????????
        double           GetCell2Double();
                                                                                                    // CString  GetCellForce2CStr();   

        // ??????
        void             SetComment( _IN const tchar_t* string_value, _IN const tchar_t* author, _IN int width = 20, _IN int height = 20 );
        _tstring         GetComment();
        void             DelComment();
                                                                                    //
        //
        //_tstring        GetCellRange2CStr( _IN long c, _IN long r );
    };
}