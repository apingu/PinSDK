/*******************************************************************
 *                                                                 *
 * 甇勺lass?彗x閮剛?嚗蒂?潔???                                      *
 * 鈭??Edison.Shih.???賢?嚗?頞喳???銝雲                      *
 *                                                                 *
 * ?孰dison.Shih.???撘?頞喉???edisonx????                  *
 *                                                                 *
 * Class??Visual C++ 6 ?                                       *
 * ?拍?嘩icrosoft Excel 2003 ??Microsoft Excel 2003 SP3 皜祈岫?∟炊  *
 * 皜祈岫撟喳 Microsoft Windows XP SP3                               *
 *                           2011/7/11                            *
 *******************************************************************/
#ifndef EXCELFILE_H
#define EXCELFILE_H


 //#include <vector>

enum Boarder_Style
{
    BS_NONE = 0,         //?⊥?蝺?
    BS_SOLIDLINE,        //銝?祉?
    BS_BIGDASH,          //撠???蝺? - - - - -??蝝?
    BS_SMALLDASH,        //憭折???蝺? - - - - -?∠?蝝?
    BS_DOTDASH,          //??-.-.-.-.-.-.
    BS_DASHDOTDOT,       //??.-..-..-..-..-.
    BS_DOUBLSOLID = 9,   //??============嚗???蝝唳霈?
    BS_SLASHDASH = 13    //??-/-/-/-/-/-/嚗???蝝唳霈?
};

enum Boarder_Weight      //嚗?蝝堆?
{
    BA_HAITLINE = 1,     //瘥??砍?嚗?隞亦??銵函內嚗?
    BA_THIN,             //銝??
    BA_MEDIUM,           //蝎?
    BA_THICK             //??
};

enum Horizontal_Alignment
{
    HA_GENERAL = 1,
    HA_H_LEFT,         //edisonx
    HA_H_CENTER,
    HA_H_RIGHT,        //edisonx
    HA_FILL,           //???喳‵皛?   //edisonx
    HA_JUSTIFYPARA,    //畾菔??嚗??????銵?
    HA_CENTERACROSS,   //頝冽?蝵桐葉嚗???脣??潘?
    HA_JUSTIFY,        //?撠?嚗葬??
};

/*
enum Vertical_Alignment
{
    VA_TOP = 1,        //edisonx
    VA_CENTER,         //edisonx
    VA_BOTTOM,         //edisonx
    VA_JUSTIFYPARA,    //畾菔??嚗??????銵?
    VA_JUSTIFY         //?撠?
};
*/

enum Histogram_Chart_Type
{
    CT_AREA = 0,       //???
    CT_COLUMN,         //?寞
    CT_CONE,           //??
    CT_CYLINDER,       //?
    CT_PYRAMID         //??憛?
};

enum Stock_Type
{
    ST_HLC = 0,    //?擃??雿??嗥
    ST_OHLC,       //?-?擃??雿??嗥
    ST_VHLC,       //?漱???擃??雿??嗥
    ST_VOHLC       //?漱???-?擃??雿??嗥
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

    TCHAR          buf0[1024];  //?怠???銝?
    TCHAR          buf1[1024];
    TCHAR          buf2[1024];

    _tstring        m_filename;                                               // open file name
    _tstring        m_sheetname;                                              // open sheet name
    //TCHAR          filename[1024];

public:
    ExcelFile();
    ~ExcelFile();
    //?ExcelFile* //??瑼?銋??臭誑蝜潛??豢?Sheet???
    ExcelFile* New();
    ExcelFile* Open( const TCHAR* );
    void       SaveAs( const TCHAR* );
    void       Save();
    void       FixSave();
    void       ExportAs( const TCHAR*, int pagefrom = 1, int pageto = 1 ); //?Ｗ?df ??蝚砍嗾? 蝚砍嗾??   
    void       SetAlert( BOOL f );                                         //霅血?閬?
    void       SetAlertBeforeOverwriting( BOOL f );

    CString    GetFilename();
    CString    GetSheetname();

    long       GetFileFormat();

    BOOL       GetAutoUpdateSaveChanges();
    void       SetAutoUpdateSaveChanges( BOOL f );

    long       GetAutomationSecurity( void );
    void       SetAutomationSecurity( long v );                            // 隞亙??冽芋撘??? ??撌券? VB銋???
                                                                           // msoAutomationSecurityForceDisable = 3
                                                                           // msoAutomationSecurityByUI = 2
                                                                           // msoAutomationSecurityLow = 1

    void       SetUserControl( BOOL f );                                   // 蝭????銝?隞亙凳????韏瑚?

    //void       Quit();
    void       SetVisible( bool );                                         //閮剖?閬??箇?敺?嚗蒂??嗆?鈭斤策雿輻??
    //----------------------------------------------------                 
    //Sheet??                                                            
    long       SheetTotal();                                               //?? Sheet ?
    short      AddSheet( const TCHAR* );
    void       SetSheetName( short, const TCHAR* );                        //?崆heetNumber    ??SheetName
    CString    GetSheetName( short );                                      //?崆heetNumber    ??SheetName
    short      FindSheetName( const TCHAR* );                              //?崆heetName      ??SheetNumber
                                                                           
    ExcelFile* SelectSheet( const TCHAR* );                                //?崆heetName      ?豢?Sheet
    ExcelFile* SelectSheet( short );                                       //?崆heetNumber    ?豢?Sheet
    void       CopySheet( const TCHAR* );                                  //銴ˊSheetName    ???Sheet??蝵殷?銝行?摰Sheet?迂
    void       CopySheet( short );                                         //銴ˊSheetNumber  ???Sheet??蝵殷?銝行?摰?蝔?
    void       DelSheet( const TCHAR* );                                   //?祿heetName      ?芷Sheet
    void       DelSheet( short );                                          //?祿heetNumber    ?芷Sheet
    void       SetOrientation( short );                                    //閮剖?? 2:璈怠?  1:?游?
    //-----------------------------------------------------
    //
    long       GetHorztlStartCell();                                       // 韏瑕?銵?
    long       GetVrticlStartCell();                                       // 韏瑕???
    long       GetHorztlTotalCell();                                       // 蝮質???(瘝????典?銋?鋡怠???
    long       GetVrticlTotalCell();                                       // 蝮賢???(瘝????典?銋?鋡怠???

    CRect      GetNotEmptyCellRange();

    //-----------------------------------------------------
    //?ExcelFile* ?訾??澆?銋??臭誑蝜潛?銝??神????賣
    //?訾???
    ExcelFile* SelectCell( const TCHAR* );
    ExcelFile* SelectCell( const TCHAR*, int );
    ExcelFile* SelectCell( TCHAR, int );
    ExcelFile* SelectCell( TCHAR, TCHAR, int );
    ExcelFile* SelectCell( int, int );
    //?訾?????
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
    void       ClearCell();                                               //皜?脣???
    ExcelFile* SetMergeCells( short vMerge = TRUE,                        //?蔥?脣??潘??虜??頝冽?蝵桐葉嚗?
                              bool isCenterAcross = true );


    ExcelFile* InsertCol();
    //--------------------------------------------
    //撠?
    ExcelFile* SetHorztlAlgmet( short );                                   //瘞游像撠?
    ExcelFile* SetVrticlAlgmet( short );                                   //?撠?
    ExcelFile* SetTextAngle( short Angle );                                //?孵?-??閫漲
    ExcelFile* AutoNewLine( bool NewLine );                                //?芸???
    //---------------------------------------------
    //?潛?
    ExcelFile* SetCellBorder( long BoarderStyle = 1,
                              int BoarderWeight = 2, long BoarderColor = 1 ); //閮剖?獢?蝎敦????
    //---------------------------------------
    //?脣??澆之撠?
    void       AutoFitHight();                                             //?芸?隤踵??
    void       AutoFitWidth();                                             //?芸?隤踵甈祝
    ExcelFile* SetCellHeight( float );                                     //閮剖???
    ExcelFile* SetCellWidth( float );                                      //閮剖?甈祝
    //---------------------------------------------
    //摮?
    ExcelFile* SetFont( const TCHAR* fontType = _TEXT( "?啁敦??" ) );      //閮剖?摮?嚗?閮剜蝝唳?擃?
    ExcelFile* SetFontBold( bool isBold = true );                          //蝎?
    ExcelFile* SetFontStrkthrgh( bool isBold = true );                     //?芷蝺?
    ExcelFile* SetFontSize( short fontSize = 12 );                         //閮剖?摮?憭批?嚗?閮?2pt嚗?
    ExcelFile* SetFontColor( short colorIndex = 1 );                       //摮?憿嚗?閮剝??莎?
    //ExcelFile* SetFontColor( DWORD color );                              //*****???游摰寡???閬?瑼Ｘ
    //---------------------------------------------
    ExcelFile* SetCellColor( short );                                      //閮剖?摨
    ExcelFile* SetCellColor( DWORD color );                                //閮剖?摨
    //---------------------------------------------
    //嚗?7-32?梯?????脖?嚗?
    //Microsoft Excel ???脫?摨靘?
    //蝝??????????換?嚗嚗? - 8嚗瘛勗瘛綽?y嚗?1 - 7
    //銝?蝯?RGB銝行???敺?摮?”鋆?
    short    SelectColor( short x = 8, short y = 7 );                      //靘xcel隞?漣璅????
    short    SelectColor( const TCHAR ColorChar = 'W' );                   //敹恍?嚗?D?W??R??G??B??Y嚗?
    //---------------------------------------------
    void     DeleteRange();
    //---------------------------------------------
    //閮剖?鞈??脣摮嚗???銝莎?
    //銝?祉?
    void     SetCell( int );
    void     SetCell( double );
    void     SetCell( long );
    void     SetCell( INT64 );
    void     SetCell( UINT64 );
    void     SetCell( const TCHAR* );
    void     SetCell( CString );
    void     SetCellRange( long c, long r, const TCHAR* );
    void     SetCellRange( long c, long r, CString );

    //?芾?蝝圈?澆???
    void     SetCell( const TCHAR*, int );
    void     SetCell( const TCHAR*, double );
    void     SetCell( const TCHAR*, long );    

    int      GetCellCol();                                               // ?喳?雿????
    int      GetCellRow();                                               // ?喳?雿????

    CString  GetFormat();                                                //  "@";  //閮剖?C甈摮?澆??箸?摮? "yyyy/MM/dd";  //閮剖?C甈摮?澆?  #,##
    void     SetFormat( const TCHAR* );                                  //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
    void     SetFormula( const TCHAR* );                                 // 閮剖楊??
    void     SetNumberFormat( const TCHAR* );                            //"$0.00",  #,##0 (??雿?  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
    //--------------------------------------------
    //??
    int      GetCell2Int();                                              //?澆?敹????詨潭??賢???
    CString  GetCell2CStr();                                             //?澆?敹???????
    //CString  GetCellForce2CStr();                                      //
    CString  GetCellRange2CStr( long c, long r );
    double   GetCell2Double();
    //--------------------------------------------
    //??嚗???摨?//edisonx
    void     Sort( CString IndexCell1, long DeCrement1,
                   CString IndexCell2 = _TEXT( "" ), long DeCrement2 = 1,
                   CString IndexCell3 = _TEXT( "" ), long DeCrement3 = 1 );

    //--------------------------------------------

    //?”?edisonx???賣鞈?

    //?脣??”??.bmp嚗?jpg鈭血隞伐?
    void     SaveChart( CString FullBmpPathName );

    //?”嚗?憿???詨瘥活撱箇??質?雿輻嚗?
    //雿輻????票銝hart?摮蝭?

    //?豢?鞈?蝭?
    ExcelFile* SelectChartRange( const TCHAR*, const TCHAR* );
    ExcelFile* SelectChartRange( const TCHAR*, int, const TCHAR*, int );
    ExcelFile* SelectChartRange( TCHAR, int, TCHAR, int );
    ExcelFile* SelectChartRange( TCHAR, TCHAR, int, TCHAR, TCHAR, int );
    //閮剖?Chart?
    ExcelFile* SetChart( short XaxisByToporLeft = 2, bool isLabelVisable = 1,
                         CString = _TEXT( "" ), CString = _TEXT( "" ), CString = _TEXT( "" ) );
    //???嫘?晞??晞???摮?
    void     InsertHistogramChart( int shapeType = CT_COLUMN,
                                   bool is3D = 0,
                                   int isVrticlorHorztlorOther = 0,
                                   int isNone_Stack_Percent = 0 );
    //?嗅?嚗畾?銵剁?   
    void     InsertBubleChart( bool is3D = 0 );                               //瘜⊥部??
    void     InsertDoughnutChart( bool Explode = 0 );                         //???
    void     InsertSurfaceChart( bool is3D = 0, bool isWire = 0 );            //?脤??
    void     InsertRadarChart( bool isWire = 0, bool isDot = 1 );             //?琿???
    void     InsertPieChart( bool Explode = 0, int  type2Dor3DorOf = 0 );     //????
    void     InsertLineChart( bool isDot = 1, bool is3D = 0,
        int  isNone_Stack_Percent = 0 );             //????
    void     InsertXYScatterChart( bool isDot, bool isLine, bool Smooth );    //?Ｘ??
    void     InsertStockChart( int );                                         //?∠巨??
    //--------------------------------------------
    void     InsertImage( const TCHAR*, float, float, float, float );         //??? x y w h
    void     InsertImage( const TCHAR*, float, float );                       //???
    void     InsertImage( const TCHAR* );                                     //???嚗??詨?蝭?嚗?瑼?憭梁?嚗?
protected:
    void     ExcelFile::newChart();  //?沒heet?啣??”
    //    ?脫迫隞颱???
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

    range.SetFormula(COleVariant("=RAND()*100000"));  //憟撘?
    range.setSetValue(COleVariant("Last Name"));      //頛詨??
    range.SetNumberFormat(COleVariant("$0.00"));      //?詨??澆?

    //??
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
                DeCrement2,  // long Order2, [ 1(?) or 2( ?? ]
                key3,        // key3
                DeCrement3,  // long Order3, [ 1(?) or 2( ?? ]
                2,           // Header, [0,1 : 銝 title 2 : title 嚗????銝韏瑟?
       //?脤?
                VOptional,   // OrderCustom [敺???嚗摰儔?????”銝凋?蝝Ｗ????雿輻撣貉?]
                _variant_t((short)TRUE),    // MatchCase [TRUE?之撠神?
                1,           // Orientation : [???孵?, 1:??, 2:??)
                1,           // SortMethod : [1:??蝚行慰隤?喲?摨? 2:??蝚衣??急]
       //?芰?賊?
                1,           // DataOption1 ?舫 0 ??1
                1,           // DataOption2 ?舫 0 ??1
                1            // DataOption3 ?舫 0 ??1
    );
*/