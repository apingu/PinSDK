/*******************************************************************
 *                                                                 *
 * 此class由kx設計，並發佈初版                                      *
 * 二版則由Edison.Shih.提供函式，補足初版之不足                      *
 *                                                                 *
 * 由Edison.Shih.提供的函式補足，會做edisonx的記號                   *
 *                                                                 *
 * Class由 Visual C++ 6 開發                                       *
 * 適用於Microsoft Excel 2003 於 Microsoft Excel 2003 SP3 測試無誤  *
 * 測試平台 Microsoft Windows XP SP3                               *
 *                           2011/7/11                            *
 *******************************************************************/
#ifndef EXCELFILE_H
#define EXCELFILE_H


 //#include <vector>

enum Boarder_Style
{
    BS_NONE = 0,         //無框線
    BS_SOLIDLINE,        //一般線
    BS_BIGDASH,          //小間隔虛線- - - - - -有粗細
    BS_SMALLDASH,        //大間隔虛線- - - - - -無粗細
    BS_DOTDASH,          //虛線-.-.-.-.-.-.
    BS_DASHDOTDOT,       //虛線.-..-..-..-..-.
    BS_DOUBLSOLID = 9,   //雙線============（不受粗細改變）
    BS_SLASHDASH = 13    //雙線-/-/-/-/-/-/（不受粗細改變）
};

enum Boarder_Weight      //（粗細）
{
    BA_HAITLINE = 1,     //比一般小（所以用虛線表示）
    BA_THIN,             //一般
    BA_MEDIUM,           //粗
    BA_THICK             //厚
};

enum Horizontal_Alignment
{
    HA_GENERAL = 1,
    HA_H_LEFT,         //edisonx
    HA_H_CENTER,
    HA_H_RIGHT,        //edisonx
    HA_FILL,           //重複至填滿    //edisonx
    HA_JUSTIFYPARA,    //段落重排（有留白邊，有自動斷行）
    HA_CENTERACROSS,   //跨欄置中（不合拼儲存格）
    HA_JUSTIFY,        //分散對齊（縮排）
};

/*
enum Vertical_Alignment
{
    VA_TOP = 1,        //edisonx
    VA_CENTER,         //edisonx
    VA_BOTTOM,         //edisonx
    VA_JUSTIFYPARA,    //段落重排（有留白邊，有自動斷行）
    VA_JUSTIFY         //分散對齊
};
*/

enum Histogram_Chart_Type
{
    CT_AREA = 0,       //區域
    CT_COLUMN,         //方柱
    CT_CONE,           //圓錐
    CT_CYLINDER,       //圓柱
    CT_PYRAMID         //金字塔
};

enum Stock_Type
{
    ST_HLC = 0,    //最高-最低-收盤
    ST_OHLC,       //開盤-最高-最低-收盤
    ST_VHLC,       //成交量-最高-最低-收盤
    ST_VOHLC       //成交量-開盤-最高-最低-收盤
};

class CApplication;
class CWorkbooks;
class CWorkbook;
class CSheets;
class CWorksheet;
class CRange;
class CInterior;
class CExcelFont;
class CChartObjects;
class CChartObject;
class CChart;
class CShapes;

class ExcelFile
{
protected:

    COleVariant    VOptional, VTRUE, VFALSE;
    CApplication*  objApp;
    CWorkbooks*    objBooks;
    CWorkbook*     objBook;
    CSheets*       objSheets;
    CWorksheet*    objSheet;
    CWorksheet*    objSheetT;
    CRange*        range;
    CRange*        col;
    CRange*        row;
    CInterior*     cell;
    CExcelFont*    font;
    COleException  e;

    LPDISPATCH     lpDisp;
    CChartObjects* chartobjects;
    CChartObject*  chartobject;
    CChart*        xlsChart;
    VARIANT        var;

    CShapes*       shapes;

    TCHAR          buf0[1024];  //暫存的字串
    TCHAR          buf1[1024];
    TCHAR          buf2[1024];

    _tstring        file_name_;                                               // open file name
    _tstring        sheet_name_;                                              // open sheet name
    //TCHAR          filename[1024];

public:
    ExcelFile();
    ~ExcelFile();
    //回傳ExcelFile* //開了檔案之後可以繼續選擇Sheet和命名
    ExcelFile* New();
    ExcelFile* Open( const TCHAR* );
    void       SaveAs( const TCHAR* );
    void       Save();
    void       FixSave();
    void       ExportAs( const TCHAR*, int pagefrom = 1, int pageto = 1 ); //產出的pdf 會從第幾頁到 第幾頁    
    void       SetAlert( BOOL f );                                         //警告視窗
    void       SetAlertBeforeOverwriting( BOOL f );

    CString    GetFilename();
    CString    GetSheetname();

    long       GetFileFormat();

    BOOL       GetAutoUpdateSaveChanges();
    void       SetAutoUpdateSaveChanges( BOOL f );

    long       GetAutomationSecurity( void );
    void       SetAutomationSecurity( long v );                            // 以安全模式開啟( 關閉巨集 VB之類的)
                                                                           // msoAutomationSecurityForceDisable = 3
                                                                           // msoAutomationSecurityByUI = 2
                                                                           // msoAutomationSecurityLow = 1

    void       SetUserControl( BOOL f );                                   // 範例看到的 不知道可以幹嘛 先抄起來

    //void       Quit();
    void       SetVisible( bool );                                         //設定視窗為看得見，並把控制權交給使用者

    long       SheetTotal();                                               //取得 Sheet 個數
    short      AddSheet( const TCHAR* );
    void       SetSheetName( short, const TCHAR* );                        //由SheetNumber    指定SheetName
    CString    GetSheetName( short );                                      //由SheetNumber    取得SheetName 從 1 開始
    short      FindSheetName( const TCHAR* );                              //由SheetName      取得SheetNumber
                                                                           
    ExcelFile* SelectSheet( const TCHAR* );                                //由SheetName      選擇Sheet
    ExcelFile* SelectSheet( short );                                       //由SheetNumber    選擇Sheet
    void       CopySheet( const TCHAR* );                                  //複製SheetName    指定插入Sheet的位置，並指定新Sheet名稱
    void       CopySheet( short );                                         //複製SheetNumber  指定插入Sheet的位置，並指定名稱
    void       DelSheet( const TCHAR* );                                   //選SheetName      刪除Sheet
    void       DelSheet( short );                                          //選SheetNumber    刪除Sheet
    void       SetOrientation( short );                                    //設定版面 2:橫向  1:直向


    //----------------------------------------------------                 
    //Sheet操作                                                            
    long       GetHorztlStartCell();                                       // 起始行
    long       GetVrticlStartCell();                                       // 起始列
    long       GetHorztlTotalCell();                                       // 總行數 (沒資料的部分也會被取出)
    long       GetVrticlTotalCell();                                       // 總列數 (沒資料的部分也會被取出)

    CRect      GetNotEmptyCellRange();

    //-----------------------------------------------------
    //回傳ExcelFile* 選了格子之後可以繼續下「讀」「寫」的成員函數
    //選一格
    ExcelFile* SelectCell( const TCHAR* );
    ExcelFile* SelectCell( const TCHAR*, int );
    ExcelFile* SelectCell( TCHAR, int );
    ExcelFile* SelectCell( TCHAR, TCHAR, int );
    ExcelFile* SelectCell( int, int );
    //選一個範圍
    ExcelFile* SelectCell( const TCHAR*, const TCHAR* );
    ExcelFile* SelectCell( const TCHAR*, int, const TCHAR*, int );
    ExcelFile* SelectCell( TCHAR, int, TCHAR, int );
    ExcelFile* SelectCell( TCHAR, TCHAR, int, TCHAR, TCHAR, int );
    ExcelFile* SelectCell( int, int, int, int );
    ExcelFile* SelectAllCell();

    int        GetRangeCols();
    int        GetRangeRows();

    static CString ColFieldNumberToName( int num );                        // 1 -> a    2->b      26->z    27->aa  28->ab
    //--------------------------------------------
    void       ClearCell();                                               //清除儲存格
    ExcelFile* SetMergeCells( short vMerge = TRUE,                        //合併儲存格（通常會配跨欄置中）
                              bool isCenterAcross = true );


    ExcelFile* InsertCol();
    //--------------------------------------------
    //對齊
    ExcelFile* SetHorztlAlgmet( short );                                   //水平對齊
    ExcelFile* SetVrticlAlgmet( short );                                   //垂直對齊
    ExcelFile* SetTextAngle( short Angle );                                //方向-文字角度
    ExcelFile* AutoNewLine( bool NewLine );                                //自動換行
    //---------------------------------------------
    //格線
    ExcelFile* SetCellBorder( long BoarderStyle = 1,
                              int BoarderWeight = 2, long BoarderColor = 1 ); //設定框線粗細和顏色
    //---------------------------------------
    //儲存格大小
    void       AutoFitHight();                                             //自動調整列高
    void       AutoFitWidth();                                             //自動調整欄寬
    ExcelFile* SetCellHeight( float );                                     //設定列高
    ExcelFile* SetCellWidth( float );                                      //設定欄寬
    //---------------------------------------------
    //字
    ExcelFile* SetFont( const TCHAR* fontType = _T( "新細明體" ) );      //設定字型（預設新細明體）
    ExcelFile* SetFontBold( bool isBold = true );                          //粗體
    ExcelFile* SetFontStrkthrgh( bool isBold = true );                     //刪除線
    ExcelFile* SetFontSize( short fontSize = 12 );                         //設定字體大小（預設12pt）
    ExcelFile* SetFontColor( short colorIndex = 1 );                       //字型顏色（預設黑色）
    //ExcelFile* SetFontColor( DWORD color );                              //*****會導致內容跑掉 要再檢查
    //---------------------------------------------
    ExcelFile* SetCellColor( short );                                      //設定底色
    ExcelFile* SetCellColor( DWORD color );                                //設定底色
    //---------------------------------------------
    //（17-32隱藏版也有收進來）
    //Microsoft Excel 的顏色排序是依
    //紅、橙、黃、綠、藍、靛、紫、灰（x）1 - 8，由深到淺（y） 1 - 7
    //不過絕對RGB並沒有規律的存在這個表裡
    short    SelectColor( short x = 8, short y = 7 );                      //依excel介面的座標選擇顏色
    short    SelectColor( const TCHAR ColorChar = 'W' );                   //快速版（黑D、白W、紅R、綠G、藍B、黃Y）
    //---------------------------------------------
    void     DeleteRange();
    //---------------------------------------------
    //設定資料進儲存格（存成字串）
    //一般版
    void     SetCell( int );
    void     SetCell( double );
    void     SetCell( long );
    void     SetCell( INT64 );
    void     SetCell( UINT64 );
    void     SetCell( const TCHAR* );
    void     SetCell( CString );
    void     SetCellRange( long c, long r, const TCHAR* );
    void     SetCellRange( long c, long r, CString );

    //自訂細部格式版
    void     SetCell( const TCHAR*, int );
    void     SetCell( const TCHAR*, double );
    void     SetCell( const TCHAR*, long );    

    int      GetCellCol();                                               // 傳回位在的行數
    int      GetCellRow();                                               // 傳回位在的列數

    CString  GetFormat();                                                //  "@";  //設定C欄儲存格格式為文字; "yyyy/MM/dd";  //設定C欄儲存格格式  #,##
    void     SetFormat( const TCHAR* );                                  //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
    void     SetFormula( const TCHAR* );                                 // 設巨集
    void     SetNumberFormat( const TCHAR* );                            //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_T("#,##0"));
    //--------------------------------------------
    //取值
    int      GetCell2Int();                                              //格式必須是 數值才能取出
    CString  GetCell2CStr();                                             //格式必須是 文字才能取出
    //CString  GetCellForce2CStr();                                      //
    CString  GetCellRange2CStr( long c, long r );
    double   GetCell2Double();
    //--------------------------------------------
    //排序（依列排序）//edisonx
    void     Sort( CString IndexCell1, long DeCrement1,
                   CString IndexCell2 = _T( "" ), long DeCrement2 = 1,
                   CString IndexCell3 = _T( "" ), long DeCrement3 = 1 );

    //--------------------------------------------

    //圖表皆由edisonx提供函數資料

    //儲存圖表圖片.bmp（.jpg亦可以）
    void     SaveChart( CString FullBmpPathName );

    //圖表（三類型的函數在每次建立都要使用）
    //使用前必須選擇貼上Chart的儲存格範圍

    //選擇資料範圍
    ExcelFile* SelectChartRange( const TCHAR*, const TCHAR* );
    ExcelFile* SelectChartRange( const TCHAR*, int, const TCHAR*, int );
    ExcelFile* SelectChartRange( TCHAR, int, TCHAR, int );
    ExcelFile* SelectChartRange( TCHAR, TCHAR, int, TCHAR, TCHAR, int );
    //設定Chart參數
    ExcelFile* SetChart( short XaxisByToporLeft = 2, bool isLabelVisable = 1,
                         CString = _T( "" ), CString = _T( "" ), CString = _T( "" ) );
    //區域、直方、方柱、圓柱、圓錐、金字塔
    void     InsertHistogramChart( int shapeType = CT_COLUMN,
                                   bool is3D = 0,
                                   int isVrticlorHorztlorOther = 0,
                                   int isNone_Stack_Percent = 0 );
    //其它（特殊圖表）   
    void     InsertBubleChart( bool is3D = 0 );                               //泡泡圖
    void     InsertDoughnutChart( bool Explode = 0 );                         //圓環圖
    void     InsertSurfaceChart( bool is3D = 0, bool isWire = 0 );            //曲面圖
    void     InsertRadarChart( bool isWire = 0, bool isDot = 1 );             //雷達圖
    void     InsertPieChart( bool Explode = 0, int  type2Dor3DorOf = 0 );     //圓餅圖
    void     InsertLineChart( bool isDot = 1, bool is3D = 0,
        int  isNone_Stack_Percent = 0 );             //折線圖
    void     InsertXYScatterChart( bool isDot, bool isLine, bool Smooth );    //離散圖
    void     InsertStockChart( int );                                         //股票圖
    //--------------------------------------------
    void     InsertImage( const TCHAR*, float, float, float, float );         //插入圖片 x y w h
    void     InsertImage( const TCHAR*, float, float );                       //插入圖片
    void     InsertImage( const TCHAR* );                                     //插入圖片（先選取範圍，圖檔必失真）
protected:
    void     ExcelFile::newChart();  //在Sheet新增圖表
    //    防止任何運算
private:
    void     operator+( const ExcelFile& );
    void     operator-( const ExcelFile& );
    void     operator*( const ExcelFile& );
    void     operator/( const ExcelFile& );
    void     operator%( const ExcelFile& );
    void     operator=( const ExcelFile& );

    bool     operator<( const ExcelFile& );
    bool     operator>( const ExcelFile& );
    bool     operator>=( const ExcelFile& );
    bool     operator<=( const ExcelFile& );
    bool     operator==( const ExcelFile& );
    bool     operator!=( const ExcelFile& );
    bool     operator&&( const ExcelFile& );
    bool     operator&( const ExcelFile& );
    bool     operator||( const ExcelFile& );
    bool     operator|( const ExcelFile& );

    bool     operator>>( const ExcelFile& );
    bool     operator<<( const ExcelFile& );
};
#endif

/* example

    range.SetFormula(COleVariant("=RAND()*100000"));  //套公式
    range.setSetValue(COleVariant("Last Name"));      //輸入值
    range.SetNumberFormat(COleVariant("$0.00"));      //數字格式

    //插圖
    Shapes shapes = objSheet.GetShapes();
    range = objSheet.GetRange(COleVariant("J7"),COleVariant("R21"));

    //range.AttachDispatch(pRange);
    shapes.AddPicture(
                    "c:\\CHILIN.bmp",                //LPCTSTR Filename
                    false,                           //long    LinkToFile
                    true,                            //long    SaveWithDocument
                    (float)range.GetLeft(  ).dblVal, //float   Left
                    (float)range.GetTop(   ).dblVal, //float   Top
                    (float)range.GetWidth( ).dblVal, //float   Width
                    (float)range.GetHeight().dblVal  //float   Height
    );
    range.Sort(
                key1,        //  key1
                DeCrement1,  // long Order1, [ 1(ascending order) or 2(descending order) ]
                key2,        // key2,
                VOptional,   // type, [xlSortLabels, xlSortValues]
                DeCrement2,  // long Order2, [ 1(升冪) or 2( 降) ]
                key3,        // key3
                DeCrement3,  // long Order3, [ 1(升冪) or 2( 降) ]
                2,           // Header, [0,1 : 不含 title 2 : title （選取範圍）一起排
       //進階
                VOptional,   // OrderCustom [從1開始，自定義排序順序列表中之索引號，省略使用常規]
                _variant_t((short)TRUE),    // MatchCase [TRUE分大小寫排]
                1,           // Orientation : [排序方向, 1:按列, 2:按行)
                1,           // SortMethod : [1:按字符漢語拼音順序, 2:按字符筆畫數]
       //未知選項
                1,           // DataOption1 可選 0 與 1
                1,           // DataOption2 可選 0 與 1
                1            // DataOption3 可選 0 與 1
    );
*/