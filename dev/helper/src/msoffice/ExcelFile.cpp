#include "stdafx.h"

#include "./msexcel/CApplication.h"
#include "./msexcel/CWorkbooks.h"
#include "./msexcel/CWorksheets.h"
#include "./msexcel/CWorkbook.h"
#include "./msexcel/CWorksheet.h"
#include "./msexcel/CRange.h"
#include "./msexcel/CShapes.h"
#include "./msexcel/CShapeRange.h"
#include "./msexcel/CSheets.h"
#include "./msexcel/CChart.h"
#include "./msexcel/CChartObject.h"
#include "./msexcel/CChartObjects.h"
#include "./msexcel/CInterior.h"
#include "./msexcel/CExcelFont.h"
#include <comdef.h>

#include <assert.h>
#include "ExcelFile.h"


ExcelFile::ExcelFile() :
    VOptional( (long)DISP_E_PARAMNOTFOUND, VT_ERROR ), VFALSE( (short)FALSE ), VTRUE( (short)TRUE )
{
    objApp = new CApplication;
    objBooks = new CWorkbooks;
    objBook = new CWorkbook;
    objSheets = new CSheets;
    objSheet = new CWorksheet;
    objSheetT = new CWorksheet;
    range = new CRange;
    col = new CRange;
    row = new CRange;
    cell = new CInterior;
    font = new CExcelFont;
    chartobjects = new CChartObjects;
    chartobject = new CChartObject;
    xlsChart = new CChart;
    shapes = new CShapes;

    CoInitialize( NULL );

    ZeroMemory( buf0, sizeof( buf0 ) );
    ZeroMemory( buf1, sizeof( buf1 ) );
    ZeroMemory( buf2, sizeof( buf2 ) );
    //Step 1.叫Excel應用程式
    if ( !objApp->CreateDispatch( _TEXT( "Excel.Application" ), &e ) )
    {
        CString str;
        str.Format( _TEXT( "Excel CreateDispatch() failed w/err 0x%08lx" ), e.m_sc );
        AfxMessageBox( str, MB_SETFOREGROUND );
    }
#ifdef _DEBUG
    SetVisible( FALSE );
#endif
};

ExcelFile::~ExcelFile()
{
    COleVariant covOptional( (long)DISP_E_PARAMNOTFOUND, VT_ERROR );
    //COleVariant covTrue((short)FALSE), covFalse((short)FALSE),  covOptional1((long)DISP_E_PARAMNOTFOUND,VT_ERROR);  // 關閉有改變不存
    objBook->Close( covOptional, covOptional, covOptional );
    //objBook->Close(covTrue,covOptional,covOptional);
    objBooks->Close();
    objApp->Quit();

    range->ReleaseDispatch();
    objSheet->ReleaseDispatch();
    objSheets->ReleaseDispatch();
    objBook->ReleaseDispatch();
    objBooks->ReleaseDispatch();
    objApp->ReleaseDispatch();

    CoUninitialize();

    delete objApp;
    delete objBooks;
    delete objBook;
    delete objSheets;
    delete objSheet;
    delete objSheetT;
    delete range;
    delete col;
    delete row;
    delete cell;
    delete font;
    delete chartobjects;
    delete chartobject;
    delete xlsChart;
    delete shapes;
}

//////////////////////////////////////////////////////////////////////////
//維持 開新檔案/開啟舊檔/存檔/另存新檔 的整體概念
//    概念描述

ExcelFile* ExcelFile::New()
{
    ( *objBooks ) = objApp->get_Workbooks();
    ( *objBook ) = objBooks->Add( VOptional );    //開新檔案
    ( *objSheets ) = objBook->get_Worksheets();
    SelectSheet( 1 );
    return this;
}

ExcelFile* ExcelFile::Open( const TCHAR* path )
{
    DWORD currt = ::GetTickCount();
    TCHAR buff[_MAX_PATH];
    if ( GetFullPathName( path, _MAX_PATH, buff, NULL ) == 0 )
        _tcscpy( buff, path );

    FILE* f = _tfopen( buff, _TEXT( "rb" ) );
    if ( f == NULL )
    {
        return NULL;
    }
    fclose( f );
    file_name_ = path;
    ( *objBooks ) = objApp->get_Workbooks();
    objBook->AttachDispatch( objBooks->Add( _variant_t( buff ) ) ); //開啟一個已存在的檔案
    objBook->Activate();
    ( *objSheets ) = objBook->get_Worksheets();
    SelectSheet( 1 );

    DWORD currf = ::GetTickCount() - currt;
    _stprintf_s( buff, _T( "ExcelFile::Open load file %s spend time: %d\n" ), path, currf );
    OutputDebugString( buff );

    return this;
}

void ExcelFile::SaveAs( const TCHAR* strTableName )
{
    TCHAR buff[_MAX_PATH];
    ZeroMemory( buff, sizeof( buff ) );
    //_stprintf(buff, _TEXT("%s"), strTableName);  ???
    _tcscpy( buf0, strTableName );
    if ( GetFullPathName( buf0, _MAX_PATH, buff, NULL ) != 0 )
        _tcscpy( buf0, buff );

    COleVariant var = VOptional;

    {
        //TCHAR path_buffer[_MAX_PATH];
        TCHAR drive[_MAX_DRIVE];
        TCHAR dir[_MAX_DIR];
        TCHAR fname[_MAX_FNAME];
        TCHAR ext[_MAX_EXT];
        _tsplitpath( strTableName, drive, dir, fname, ext ); // C4996

        if ( _tcsicmp( ext, _TEXT( ".xlt" ) ) == 0 )
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlTemplate );
        }
        else if ( _tcsicmp( ext, _TEXT( ".ods" ) ) == 0 )
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlOpenDocumentSpreadsheet );
        }
        else if ( _tcscmp( ext, _TEXT( ".xml" ) ) == 0 )
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlXMLSpreadsheet );
        }
        else if ( _tcscmp( ext, _TEXT( ".cvs" ) ) == 0 )
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlCSVWindows );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xlsb" ) ) == 0 ) //50
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlExcel12 );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xlsx" ) ) == 0 ) // 51
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlWorkbookDefault );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xlsm" ) ) == 0 ) //52
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlOpenXMLWorkbookMacroEnabled );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xltm" ) ) == 0 ) //53
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlOpenXMLTemplateMacroEnabled );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xltx" ) ) == 0 ) //54
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlOpenXMLTemplate );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xlam" ) ) == 0 ) //55
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlOpenXMLAddIn );
        }
        else if ( _tcsicmp( ext, _TEXT( ".xls" ) ) == 0 )  //56
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlExcel8 );
        }
        else
        {
            var = COleVariant( (long)Excel::XlFileFormat::xlWorkbookNormal );
        }
    }

    objBook->SaveAs( COleVariant( buf0 ),
        var, VOptional,
        VOptional, VOptional,
        VOptional, 1,
        VOptional, VFALSE,
        VOptional, VOptional, VOptional );
}


void ExcelFile::Save()
{
    objBook->Save();
}

void ExcelFile::FixSave()
{
    _tunlink( file_name_.c_str() );

    TCHAR buff[_MAX_PATH];
    ZeroMemory( buff, sizeof( buff ) );
    //_stprintf(buff, _TEXT("%s"), strTableName);  ???
    _tcscpy( buf0, file_name_.c_str() );
    if ( GetFullPathName( buf0, _MAX_PATH, buff, NULL ) != 0 )
        _tcscpy( buf0, buff );


    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
    _tsplitpath( file_name_.c_str(), drive, dir, fname, ext ); // C4996

    long excelformat = (long)objBook->get_FileFormat();
    {
        switch ( excelformat )
        {
        case Excel::XlFileFormat::xlTemplate:
            {
                _tcscpy( ext, _TEXT( ".xlt" ) );
            }
            break;
        case Excel::XlFileFormat::xlOpenDocumentSpreadsheet:
            {
                _tcscpy( ext, _TEXT( ".ods" ) );
            }
            break;
        case Excel::XlFileFormat::xlXMLSpreadsheet:
            {
                _tcscpy( ext, _TEXT( ".xml" ) );
            }
            break;
        case Excel::XlFileFormat::xlCSVWindows:
            {
                _tcscpy( ext, _TEXT( ".cvs" ) );
            }
            break;
        case Excel::XlFileFormat::xlExcel12:
            {
                _tcscpy( ext, _TEXT( ".xlsb" ) );
            }
            break;
        case Excel::XlFileFormat::xlWorkbookDefault:
            {
                _tcscpy( ext, _TEXT( ".xlsx" ) );
            }
            break;
        case Excel::XlFileFormat::xlOpenXMLWorkbookMacroEnabled:
            {
                _tcscpy( ext, _TEXT( ".xlsm" ) );
            }
            break;
        case Excel::XlFileFormat::xlOpenXMLTemplateMacroEnabled:
            {
                _tcscpy( ext, _TEXT( ".xltm" ) );
            }
            break;
        case Excel::XlFileFormat::xlOpenXMLTemplate:
            {
                _tcscpy( ext, _TEXT( ".xltx" ) );
            }
            break;
        case Excel::XlFileFormat::xlOpenXMLAddIn:
            {
                _tcscpy( ext, _TEXT( ".xlam" ) );
            }
            break;
        case Excel::XlFileFormat::xlExcel8:
            {
                _tcscpy( ext, _TEXT( ".xls" ) );
            }
            break;
        }
    }

    //COleVariant var = COleVariant( (long)excelformat );
    COleVariant var = COleVariant( (long)Excel::XlFileFormat::xlWorkbookDefault );

    TCHAR save_buff[_MAX_PATH];
    _tmakepath( save_buff, drive, dir, fname, ext ); // C4996

    //xlWorkbookDefault

    objBook->SaveAs( COleVariant( buf0 ),
        var, VOptional,
        VOptional, VOptional,
        VOptional, 1,
        VOptional, VFALSE,
        VOptional, VOptional, VOptional );

    _trename( save_buff, file_name_.c_str() );
}

void ExcelFile::SetAlert( BOOL f )
{
    objApp->put_DisplayAlerts( f );
}

void ExcelFile::SetAlertBeforeOverwriting( BOOL f )
{
    objApp->put_AlertBeforeOverwriting( f );
    objApp->put_DisplayAlerts( f );
}

long ExcelFile::GetFileFormat()
{
    return objBook->get_FileFormat();
}

BOOL ExcelFile::GetAutoUpdateSaveChanges()
{
    return objBook->get_AutoUpdateSaveChanges();
}

void ExcelFile::SetAutoUpdateSaveChanges( BOOL f )
{
    objBook->put_AutoUpdateSaveChanges( f );
}

long ExcelFile::GetAutomationSecurity( void )
{
    return objApp->get_AutomationSecurity();
}

void ExcelFile::SetAutomationSecurity( long v )
{
    objApp->put_AutomationSecurity( v );
}

// 範例看到的 不知道可以幹嘛 先抄起來
void ExcelFile::SetUserControl( BOOL f )
{
    objApp->put_UserControl( f );
}


//////////////////////////////////////////////////////////////////////////
//畫出視窗
//    就是把視窗畫出來，之後把控制權給使用者。

//SetVisible()
void ExcelFile::SetVisible( bool a )
{
    objApp->put_Visible( a );    //顯示Excel檔
    objApp->put_UserControl( a );//使用者控制後，就不可以自動關閉
}

//////////////////////////////////////////////////////////////////////////
//對Sheet的操作
//    其中，在選擇sheet時，常常會出現，程式碼指定選擇一個不存在的sheet，這時出錯了，只有一個空白的MessageBox()，出現了也不知道出了什麼事？哪裡出了問題，所以就在SelectSheet()時做try-catch的設計

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sheet操作

//-------------------------
////取得 Sheet 個數
long ExcelFile::SheetTotal()
{
    return objSheets->get_Count();//edisonx
}

//-------------------------
////新增一個 Sheet
short ExcelFile::AddSheet( const TCHAR* SheetName )
{
    short scn = objSheets->get_Count();
    CWorksheet ws = objSheets->get_Item( COleVariant( (short)scn ) );
    _variant_t p1( ws.m_lpDispatch );
    // 最後一個參數不知道為何要填啥 所以 先填1
    objSheets->Add( vtMissing, p1, COleVariant( (short)1 ), COleVariant( (short)1 ) );
    scn += 1;
    ( *objSheet ) = objSheets->get_Item( COleVariant( scn ) );
    objSheet->put_Name( SheetName );//設定sheet名稱
    return scn;
}

//-------------------------
//由SheetIndex 指定SheetName
void ExcelFile::SetSheetName( short SheetIndex, const TCHAR* SheetName )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
        objSheet->put_Name( SheetName );//設定sheet名稱
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("設定第%d個Sheet的名字為%s出錯了！"), SheetIndex, SheetName);
        str.Format( _TEXT( "set Sheet %d name %s falure!" ), SheetIndex, SheetName );
        AfxMessageBox( str );
    }
}
//-------------------------
//由SheetIndex 取得SheetName
CString ExcelFile::GetSheetName( short SheetIndex )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("取得第%d個Sheet名字出錯了！"), SheetIndex);
        str.Format( _TEXT( "get Sheet %d falure!" ), SheetIndex );
        AfxMessageBox( str );
    }
    return objSheet->get_Name();//edisonx
}
//-------------------------
//由SheetName      取得SheetNumber
short ExcelFile::FindSheetName( const TCHAR* SheetName )
{
    for ( int i = 0; i < SheetTotal(); i++ )
    {
        CString sn = GetSheetName( i + 1 );
        if ( sn == SheetName )
        {
            return i + 1;
        }
    }
    return -1;
}
//-------------------------
//選擇Sheet
//由SheetName
ExcelFile* ExcelFile::SelectSheet( const TCHAR* SheetName )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( _variant_t( SheetName ) );
        objSheet->Activate();//edisonx
        sheet_name_ = SheetName;
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇Sheet: %s出錯了！"), SheetName);
        str.Format( _TEXT( "select Sheet %s failure!" ), SheetName );
        AfxMessageBox( str );
    }
    return this;
}
//由SheetIndex 
ExcelFile* ExcelFile::SelectSheet( short SheetIndex )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
        objSheet->Activate();//edisonx
        sheet_name_ = GetSheetName( SheetIndex );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇第%d個Sheet出錯了！"), SheetIndex);
        str.Format( _TEXT( "select Sheet%d failure!" ), SheetIndex );
        AfxMessageBox( str );
    }
    return this;
}
//-------------------------
//複製SheetName 指定插入Sheet的位置，並指定新Sheet名稱
void ExcelFile::CopySheet( const TCHAR* SheetName )
{
    objSheet->AttachDispatch( objSheets->get_Item( _variant_t( SheetName ) ), true );
    objSheet->Copy( vtMissing, _variant_t( objSheet ) );
}
//複製SheetIndex 指定插入Sheet的位置，並指定名稱
void ExcelFile::CopySheet( short SheetIndex )
{
    objSheet->AttachDispatch( objSheets->get_Item( COleVariant( SheetIndex ) ) );
    objSheet->Copy( vtMissing, _variant_t( objSheet ) );
}
//-------------------------
//刪除Sheet
//選SheetName 
void ExcelFile::DelSheet( const TCHAR* SheetName )
{
    ( *objSheet ) = objSheets->get_Item( _variant_t( SheetName ) );
    objSheet->Delete();//edisonx
}
//選SheetIndex
void ExcelFile::DelSheet( short SheetIndex )
{
    ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
    objSheet->Delete();//edisonx
}

void ExcelFile::ExportAs( const TCHAR* strTableName, int pagefrom, int pageto )
{
    TCHAR buff[_MAX_PATH];
    ZeroMemory( buff, sizeof( buff ) );
    //_stprintf(buff, _TEXT("%s"), strTableName);  ???
    _tcscpy( buf0, strTableName );
    if ( GetFullPathName( buf0, _MAX_PATH, buff, NULL ) != 0 )
        _tcscpy( buf0, buff );
    //if(_tcscmp(buf0, _TEXT("")) == 0)
    //    _tcscpy(buf0, buff);

    COleVariant var = VOptional;

    {
        //TCHAR path_buffer[_MAX_PATH];
        TCHAR drive[_MAX_DRIVE];
        TCHAR dir[_MAX_DIR];
        TCHAR fname[_MAX_FNAME];
        TCHAR ext[_MAX_EXT];
        _tsplitpath( strTableName, drive, dir, fname, ext ); // C4996

        if ( _tcscmp( ext, _TEXT( ".pdf" ) ) == 0 )
        {
            objSheet->ExportAsFixedFormat(
                (long)Excel::XlFixedFormatType::xlTypePDF,
                COleVariant( buf0 ),
                COleVariant( (long)Excel::XlFixedFormatQuality::xlQualityStandard ),
                VTRUE, VFALSE, COleVariant( (long)pagefrom ), COleVariant( (long)pageto ), VFALSE, VOptional );
            return;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
//設定版面 
//    2:橫向  1:直向
void ExcelFile::SetOrientation( short )
{
    //range->OlePropertySet(TEXT("Orientation"), xlLandscape);
    //objSheet->
}
///////////////////////////////////////////////////////////////////////////////////////////
//對儲存格(Cell)的操作
//    一般常見的cell操作，都在上面了

///////////////////////////////////////////////////////////////////////////////////////////
//Cell操作
//Cell計數計算
// 取得起始列
long ExcelFile::GetHorztlStartCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    return usedrange.get_Column();
}
// 取得起始行
long ExcelFile::GetVrticlStartCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    return usedrange.get_Row();
}
// 取得總列數
long ExcelFile::GetHorztlTotalCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    range->AttachDispatch( usedrange.get_Columns() );
    return range->get_Count();
}
// 取得總行數
long ExcelFile::GetVrticlTotalCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    range->AttachDispatch( usedrange.get_Rows() );
    return range->get_Count();
}


CRect ExcelFile::GetNotEmptyCellRange()
{
    CRect data_range;
    data_range.top = GetVrticlStartCell() + GetVrticlTotalCell() - 1;
    data_range.left = GetHorztlStartCell() + GetHorztlTotalCell() - 1;
    data_range.right = GetHorztlStartCell();
    data_range.bottom = GetVrticlStartCell();
    
    for ( int rii = GetVrticlStartCell(); rii <= GetVrticlTotalCell(); rii++ )
    {
        for ( int cii = GetHorztlStartCell(); cii <= GetHorztlTotalCell(); cii++ )
        {
            if ( SelectCell( cii, rii )->GetCell2CStr() != _T( "" ) )
            {
                data_range.top = min( data_range.top, rii );
                data_range.left = min( data_range.left, cii );
                data_range.right = max( data_range.right, cii );
                data_range.bottom = max( data_range.bottom, rii );
            }
        }
    }

    return data_range;
}
//清除儲存格
void ExcelFile::ClearCell()
{
    //先選取一個範圍的儲存格
    range->Clear();//edisonx
}
//合併儲存格
ExcelFile* ExcelFile::SetMergeCells( short vMerge, bool isCenterAcross )
{
    //先選取一個範圍的儲存格
    range->put_MergeCells( _variant_t( vMerge ) );
    if ( isCenterAcross ) SetHorztlAlgmet( HA_CENTERACROSS );
    return this;
}

// 插入行
ExcelFile* ExcelFile::InsertCol()
{
    //range->insert(_variant_t(vMerge));
    //rng.EntireColumn.Insert(XlInsertShiftDirection.xlShiftToRight,
    //	XlInsertFormatOrigin.xlFormatFromRightOrBelow);
    return this;
}


CString ExcelFile::ColFieldNumberToName( int num )                        // 1 -> a    2->b      26->z    27->aa  28->ab
{
    //int quotient  = num / 26;
    ////int remainder = num - (quotient * 26);
    //int remainder = num % 26;
    //return _T("0");

    ASSERT( ((num <= 0) || ( num > 256 ) ) || ("num between be 1 - 256") );

    num--;
    int c1 = num / 26;
    int c2 = num - ( c1 * 26 );//   num % 26;

    TCHAR cellid[512];
    if ( c1 == 0 )
        _stprintf( cellid, _TEXT( "%c" ), _TEXT( 'a' ) + c2 );
    else
        _stprintf( cellid, _TEXT( "%c%c" ), _TEXT( 'a' ) + c1 - 1, _TEXT( 'a' ) + c2 );

    return cellid;
}

///////////////////////////////////////////////////////////////////////////////////////////
//選擇儲存格(Cell)
//    選擇格子的動作，就像是用滑鼠點選儲存格一樣直覺，分成兩種，點選格子和點選範圍
//    其中，和選擇sheet一樣，若出現了選擇了超過Z的格子(用迴圈控制，就可能會超過Z)，就會出問題，也設計了try-catch來顯示出問題的格子在哪

 //-------------------------
 //Cell格式設定
 //-------------------------
 //選格子
 //選一格
ExcelFile* ExcelFile::SelectCell( const TCHAR* x )
{
    try
    {
        ( *range ) = objSheet->get_Range( COleVariant( x ), COleVariant( x ) );
        ASSERT( range );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇儲存格%s出錯了！"), x);
        str.Format( _TEXT( "select cell %s failure!" ), x );
        AfxMessageBox( str );
    }
    return this;
}

ExcelFile* ExcelFile::SelectCell( const TCHAR* x, int y )
{
    try
    {
        //ZeroMemory( buf0, sizeof( buf0 ) );
        buf0[0] = 0;
        _stprintf( buf0, _TEXT( "%s%d" ), x, y );
        ( *range ) = objSheet->get_Range( COleVariant( buf0 ), COleVariant( buf0 ) );
        ASSERT( ( *range ) );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇儲存格%s%d出錯了！"), x, y);
        str.Format( _TEXT( "select cell %s%d failure!" ), x, y );
        AfxMessageBox( str );
    }
    return this;
}

//小於Z
ExcelFile* ExcelFile::SelectCell( TCHAR x, int y )
{
    if ( ( x >= 'A' && x <= 'Z' ) || ( x >= 'a' && x <= 'z' ) )
    {
        //ZeroMemory( buf0, sizeof( buf0 ) );
        buf0[0] = 0;
        _stprintf( buf0, _TEXT( "%c%d" ), x, y );
        ( *range ) = objSheet->get_Range( COleVariant( buf0 ), COleVariant( buf0 ) );
        ASSERT( ( *range ) );
    }
    else
    {
        CString str;
        //str.Format(_TEXT("選擇儲存格%c%d出錯了！"), x, y);
        str.Format( _TEXT( "select cell %c%d failure!" ), x, y );
        AfxMessageBox( str );
        ASSERT( x >= 'A' && x <= 'Z' );
    }
    return this;
}
//大於Z，開始選AA
ExcelFile* ExcelFile::SelectCell( TCHAR x1, TCHAR x2, int y )
{
    if ( ( x1 >= 'A' && x1 <= 'Z' ) && ( x2 >= 'A' && x2 <= 'Z' ) )
    {
        //ZeroMemory( buf0, sizeof( buf0 ) );
        buf0[0] = 0;
        _stprintf( buf0, _TEXT( "%c%c%d" ), x1, x2, y );
        ( *range ) = objSheet->get_Range( COleVariant( buf0 ), COleVariant( buf0 ) );
        ASSERT( ( *range ) );
    }
    else
    {
        CString str;
        //str.Format(_TEXT("選擇儲存格%c%c%d出錯了！"), x1, x2, y);
        str.Format( _TEXT( "select cell %c%c%d failure!" ), x1, x2, y );
        AfxMessageBox( str );
        ASSERT( x1 >= 'A' && x1 <= 'Z' );
        ASSERT( x2 >= 'A' && x2 <= 'Z' );
    }
    Sleep( 1 );
    return this;
}

ExcelFile* ExcelFile::SelectCell( int x, int y )
{
    return SelectCell( ExcelFile::ColFieldNumberToName( x ), y );
}

//-------------------------
//選範圍
ExcelFile* ExcelFile::SelectCell( const TCHAR* x1, const TCHAR* x2 )
{
    try
    {
        ( *range ) = objSheet->get_Range( COleVariant( x1 ), COleVariant( x2 ) );
        ASSERT( ( *range ) );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇範圍，從%s到%s出錯了！"), x1, x2);
        str.Format( _TEXT( "select cell range, from %s to %s failure !" ), x1, x2 );
        AfxMessageBox( str );
    }
    return this;
}

ExcelFile* ExcelFile::SelectCell( const TCHAR* x1, int y1, const TCHAR* x2, int y2 )
{
    try
    {
        //ZeroMemory( buf1, sizeof( buf1 ) );
        //ZeroMemory( buf2, sizeof( buf2 ) );
        buf1[0] = 0;
        buf2[0] = 0;
        _stprintf( buf1, _TEXT( "%s%d" ), x1, y1 );
        _stprintf( buf2, _TEXT( "%s%d" ), x2, y2 );
        ( *range ) = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
        ASSERT( ( *range ) );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("選擇範圍從%s%d到%s%d的地方出錯了"), x1, y1, x2, y2);
        str.Format( _TEXT( "select cell range, from %s%d to %s%d failure !" ), x1, y1, x2, y2 );
        AfxMessageBox( str );
    }
    return this;
}

//小於Z
ExcelFile* ExcelFile::SelectCell( TCHAR x1, int y1, TCHAR x2, int y2 )
{
    if ( x1 >= 'A' && x1 <= 'Z' )
    {
        //ZeroMemory( buf1, sizeof( buf1 ) );
        //ZeroMemory( buf2, sizeof( buf2 ) );
        buf1[0] = 0;
        buf2[0] = 0;
        _stprintf( buf1, _TEXT( "%c%d" ), x1, y1 );
        _stprintf( buf2, _TEXT( "%c%d" ), x2, y2 );
        ( *range ) = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
        ASSERT( ( *range ) );
    }
    else
    {
        CString str;
        //str.Format(_TEXT("選擇範圍，從%c%d到%c%d出錯了！"), x1, y1, x2, y2);
        str.Format( _TEXT( "select cell range, from %c%d to %c%d failure !" ), x1, y1, x2, y2 );
        AfxMessageBox( str );
        ASSERT( x1 >= 'A' && x1 <= 'Z' );
    }
    return this;
}
//大於Z，開始選AA
ExcelFile* ExcelFile::SelectCell( TCHAR xA1, TCHAR xB1, int y1, TCHAR xA2, TCHAR xB2, int y2 )
{
    if ( ( xA1 >= 'A' && xA2 <= 'Z' ) && ( xB1 >= 'A' && xB2 <= 'Z' ) )
    {
        //ZeroMemory( buf1, sizeof( buf1 ) );
        //ZeroMemory( buf2, sizeof( buf2 ) );
        buf1[0] = 0;
        buf2[0] = 0;
        _stprintf( buf1, _TEXT( "%c%c%d" ), xA1, xB1, y1 );
        _stprintf( buf2, _TEXT( "%c%c%d" ), xA2, xB2, y2 );

        ( *range ) = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
        ASSERT( ( *range ) );
    }
    else
    {
        CString str;
        //str.Format(_TEXT("選擇範圍，從%c%c%d到%c%c%d出錯了！"), xA1, xB1, y1, xA2, xB2, y2);
        str.Format( _TEXT( "select cell range, from %c%c%d to %c%c%d failure !" ), xA1, xB1, y1, xA2, xB2, y2 );
        AfxMessageBox( str );
        ASSERT( xA1 >= 'A' && xA2 <= 'Z' );
        ASSERT( xB1 >= 'A' && xB2 <= 'Z' );
    }
    return this;
}

ExcelFile* ExcelFile::SelectCell( int x1, int y1, int x2, int y2 )
{
    return SelectCell( ExcelFile::ColFieldNumberToName( x1 ), y1,
                       ExcelFile::ColFieldNumberToName( x2 ), y2 );
}

ExcelFile* ExcelFile::SelectAllCell()
{
    long bc = GetHorztlStartCell();                                    // 起始行
    long br = GetVrticlStartCell();                                    // 起始列
    long cc = GetHorztlTotalCell();                                    // 總行數
    long cr = GetVrticlTotalCell();                                    // 總列數
    return SelectCell( bc, br, bc + cc, br + cr );
}


int ExcelFile::GetRangeCols()
{
    ( *col ) = range->get_EntireColumn();
    return col->get_Count();
}

int ExcelFile::GetRangeRows()
{
    ( *row ) = range->get_EntireRow();
    return row->get_Count();
}



///////////////////////////////////////////////////////////////////////////////////////////
//靠左/靠右/靠上/靠下/置中 旋轉/文字自動換行
//    保持與Excel操作介面相同的思考方式，其參數在.h檔中，有列舉成文字，不需使用數字做辨識，可提高可讀性。

//-------------------------
//對齊方式
//水平對齊
ExcelFile* ExcelFile::SetHorztlAlgmet( short position )
{
    range->put_HorizontalAlignment( COleVariant( position ) );
    return this;
}
//垂直對齊
ExcelFile* ExcelFile::SetVrticlAlgmet( short position )
{
    range->put_VerticalAlignment( COleVariant( position ) );
    return this;
}
//對齊方式的方向幾度（+90~-90）
ExcelFile* ExcelFile::SetTextAngle( short Angle )
{
    range->put_Orientation( COleVariant( Angle ) );
    return this;
}
//設定文字自動換行
ExcelFile* ExcelFile::AutoNewLine( bool NewLine )
{
    if ( NewLine ) range->put_WrapText( VTRUE );
    else           range->put_WrapText( VFALSE );
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////
//設定框線粗細、框線顏色
//    用同一個介面來設定框線和框線顏色，對於造一個表格來說，是方便的事，若有需要，也可以將它拆開兩個各別處理。但是因為沒有遇到這樣的需求，所以就沒有設計這樣的介面。

//設定框線粗細、框線顏色
ExcelFile* ExcelFile::SetCellBorder( long BoarderStyle, int BoarderWeight, long BoarderColor )
{
    range->BorderAround( _variant_t( BoarderStyle ), BoarderWeight, BoarderColor, _variant_t( (long)RGB( 0, 0, 0 ) ), _variant_t( (long)RGB( 0, 0, 0 ) ) );
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////
//自動欄寬、列高
//    在Excel中，只要對著調整格線的地方點兩下，即可適動調整適當欄寬和列高。
//    Excel預設的介面，原本無法一個函數搞定，要先選取整排，再調整。包起來的設計，比較直覺。

//設定欄寬列高
//自動調整列高
void ExcelFile::AutoFitWidth()
{
    ( *col ) = range->get_EntireColumn();    //選取某個範圍的一整排
    col->AutoFit();                          //自動調整一整排的欄寬
}
//自動調整欄寬
void ExcelFile::AutoFitHight()
{
    ( *row ) = range->get_EntireRow();       //選取某個範圍的一整排
    row->AutoFit();                          //自動調整一整排的列高
}
//設定列高
ExcelFile* ExcelFile::SetCellHeight( float height )
{
    range->put_RowHeight( _variant_t( height ) );
    return this;
}
//設定欄寬
ExcelFile* ExcelFile::SetCellWidth( float height )
{
    range->put_ColumnWidth( _variant_t( height ) );
    return this;
}
///////////////////////////////////////////////////////////////////////////////////////////
//儲存格字型的操作
//    字型的顏色、粗體、字型大小...在工作列上的那一排常用的功能，都建上來了。
//    顏色對照，參考.....參考文件！XDDD

//設定字型
ExcelFile* ExcelFile::SetFont( const TCHAR* fontType )
{
    ( *font ) = range->get_Font();
    font->put_Name( _variant_t( fontType ) );//原本是韓文字型
    return this;
}
//粗體
ExcelFile* ExcelFile::SetFontBold( bool isBold )
{
    ( *font ) = range->get_Font();
    if ( isBold )  font->put_Bold( VTRUE );//粗體
    else           font->put_Bold( VFALSE );
    return this;
}
//刪除線
ExcelFile* ExcelFile::SetFontStrkthrgh( bool isStrike )
{
    ( *font ) = range->get_Font();
    if ( isStrike )  font->put_Strikethrough( VTRUE );    //edisonx
    else             font->put_Strikethrough( VFALSE );   //edisonx
    return this;
}
//字型大小
ExcelFile* ExcelFile::SetFontSize( short fontSize )
{
    ( *font ) = range->get_Font();
    font->put_Size( _variant_t( fontSize ) );//字型大小pfstl
    return this;
}
//字型顏色
ExcelFile* ExcelFile::SetFontColor( short colorIndex )
{
    ( *font ) = range->get_Font();
    font->put_ColorIndex( _variant_t( colorIndex ) ); //字色(預設黑色)
    return this;
}

//ExcelFile* ExcelFile::SetFontColor( DWORD color )
//{
//    ( *font ) = range->get_Font();
//    font->put_Color( _variant_t( color ) ); //字色(預設黑色)
//    return this;
//}

///////////////////////////////////////////////////////////////////////////////////////////
//填滿色彩
//    在填滿色彩時，設計了使用座標的方式做操作，此是使用Excel視窗軟體介面上的調色盤的位置為準。
//    除此之外，還有快速選色介面，不過只有幾個純色支援此介面。

//設定底色
ExcelFile* ExcelFile::SetCellColor( short colorIndex )
{
    ( *cell ) = range->get_Interior();                   //取得選取範圍，設定儲存格的記憶體位址
    cell->put_ColorIndex( _variant_t( colorIndex ) );   //設定底色（查表）
    //cell->SetColor(_variant_t(colorIndex));
    return this;
}

ExcelFile* ExcelFile::SetCellColor( DWORD color )                                //設定底色
{
    ( *cell ) = range->get_Interior();                   //取得選取範圍，設定儲存格的記憶體位址
    cell->put_Color( _variant_t( color ) );   //設定底色（查表）
    //cell->SetColor(_variant_t(colorIndex));
    return this;
}

//選擇顏色（適合字色和底色）依excel介面的座標選擇顏色
short ExcelFile::SelectColor( short x, short y )
{
    //Microsoft Excel 的顏色排序是依
    //紅、橙、黃、綠、藍、靛、紫、灰（x）1 - 8，由深到淺（y） 1 - 7
    switch ( x )
    {
    case 1:
        if ( y == 1 ) return 1;
        else if ( y == 2 ) return 9;
        else if ( y == 3 ) return 3;
        else if ( y == 4 ) return 7;
        else if ( y == 5 ) return 38;

        else if ( y == 6 ) return 17;
        else if ( y == 7 ) return 38;
        break;
    case 2:
        if ( y == 1 ) return 53;
        else if ( y == 2 ) return 46;
        else if ( y == 3 ) return 45;
        else if ( y == 4 ) return 44;
        else if ( y == 5 ) return 40;

        else if ( y == 6 ) return 18;
        else if ( y == 7 ) return 26;
        break;
    case 3:
        if ( y == 1 ) return 52;
        else if ( y == 2 ) return 12;
        else if ( y == 3 ) return 43;
        else if ( y == 4 ) return  6;
        else if ( y == 5 ) return 36;

        else if ( y == 6 ) return 19;
        else if ( y == 7 ) return 27;
        break;
    case 4:
        if ( y == 1 ) return 51;
        else if ( y == 2 ) return 10;
        else if ( y == 3 ) return 50;
        else if ( y == 4 ) return  4;
        else if ( y == 5 ) return 35;

        else if ( y == 6 ) return 20;
        else if ( y == 7 ) return 28;
        break;
    case 5:
        if ( y == 1 ) return 49;
        else if ( y == 2 ) return 14;
        else if ( y == 3 ) return 42;
        else if ( y == 4 ) return  8;
        else if ( y == 5 ) return 34;

        else if ( y == 6 ) return 21;
        else if ( y == 7 ) return 29;
        break;
    case 6:
        if ( y == 1 ) return 11;
        else if ( y == 2 ) return  5;
        else if ( y == 3 ) return 41;
        else if ( y == 4 ) return 33;
        else if ( y == 5 ) return 37;

        else if ( y == 6 ) return 22;
        else if ( y == 7 ) return 30;
        break;
    case 7:
        if ( y == 1 ) return 55;
        else if ( y == 2 ) return 47;
        else if ( y == 3 ) return 13;
        else if ( y == 4 ) return 54;
        else if ( y == 5 ) return 39;

        else if ( y == 6 ) return 23;
        else if ( y == 7 ) return 31;
        break;
    case 8:
        if ( y == 1 ) return 56;
        else if ( y == 2 ) return 16;
        else if ( y == 3 ) return 48;
        else if ( y == 4 ) return 15;
        else if ( y == 5 ) return  2;

        else if ( y == 6 ) return 24;
        else if ( y == 7 ) return 32;
        break;
    }
    return 2;//預設白色
}
short ExcelFile::SelectColor( const TCHAR ColorChar )
{
    switch ( ColorChar )
    {
        //黑色
    case 'D':
    case 'd':
        return 1;
        break;
        //白色
    case 'W':
    case 'w':
        return 2;
        break;
        //紅色
    case 'R':
    case 'r':
        return 3;
        break;
        //綠色
    case 'G':
    case 'g':
        return 4;
        break;
        //藍色
    case 'B':
    case 'b':
        return 5;
        break;
        //黃色
    case 'Y':
    case 'y':
        return 6;
        break;
    }
    return 2;//預設白色
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//
void ExcelFile::DeleteRange()
{
    VARIANT shift;
    range->Delete( shift );
}

///////////////////////////////////////////////////////////////////////////////////////////
//Cell填值
//    支援格種常用的儲存格格式，但是不支援時間格式。使用上要小心

    //SetCell()
void ExcelFile::SetCell( int Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, _TEXT( "%d" ), Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( long Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, _TEXT( "%d" ), Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( INT64 Data)
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, _TEXT( "%lld" ), Data );
    range->put_Item( _variant_t( ( long )1 ), _variant_t( ( long )1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( UINT64 Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, _TEXT( "%llu" ), Data );
    range->put_Item( _variant_t( ( long )1 ), _variant_t( ( long )1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( double Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, _TEXT( "%f" ), Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( const TCHAR* Data )
{
    //ZeroMemory( buf0, sizeof( buf0 ) );
    //_tcscpy( buf0, Data );
    //_stprintf(buf0,"%s",Data);
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( Data ) );
}

void ExcelFile::SetCell( CString Data )
{
    //ZeroMemory( buf0, sizeof( buf0 ) );
    //_stprintf( buf0, _TEXT( "%s" ), Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( (LPCTSTR)Data ) );
}

void ExcelFile::SetCellRange( long c, long r, const TCHAR* Data )
{
    range->put_Item( _variant_t( ( long )r ), _variant_t( ( long )c ), _variant_t( Data ) );
}

void ExcelFile::SetCellRange( long c, long r, CString Data)
{
    //ZeroMemory( buf0, sizeof( buf0 ) );
    //_stprintf( buf0, _TEXT( "%s" ), Data );
    range->put_Item( _variant_t( ( long )1 ), _variant_t( ( long )1 ), _variant_t( ( LPCTSTR )Data ) );
}

void ExcelFile::SetCell( const TCHAR* Format, int Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, Format, Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( const TCHAR* Format, double Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, Format, Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

void ExcelFile::SetCell( const TCHAR* Format, long Data )
{
    ZeroMemory( buf0, sizeof( buf0 ) );
    _stprintf( buf0, Format, Data );
    range->put_Item( _variant_t( (long)1 ), _variant_t( (long)1 ), _variant_t( buf0 ) );
}

int ExcelFile::GetCellCol()
{
    return range->get_Column();
}

int ExcelFile::GetCellRow()
{
    return range->get_Row();
}

CString ExcelFile::GetFormat()
{
    assert( 0 || "not finish" );
    _variant_t v = range->get_NumberFormatLocal();
    CString rs = (TCHAR*)_bstr_t( v );
    return rs;
}

void ExcelFile::SetFormat( const TCHAR* Format )
{
    range->put_NumberFormatLocal( _variant_t( Format ) );
}

void ExcelFile::SetFormula( const TCHAR* Formula )
{
    range->put_Formula( _variant_t( Formula ) );
}

void ExcelFile::SetNumberFormat( const TCHAR* Format )
{
    range->put_NumberFormat( _variant_t( Format ) );
}

//-------------------------
CString ExcelFile::GetCell2CStr()
{
    try
    {
        VARIANT v = range->get_Item( _variant_t( (long)1 ), _variant_t( (long)1 ) );
        CString rs = (TCHAR*)_bstr_t( v );
        //CString rs = _T("test");
        //_bstr_t* str = new _bstr_t( v );
        //CString rs = (TCHAR*)str;
        //SAFE_DELETE( str );
        //v.pdispVal->Release();
        VariantClear( &v );
        return rs;
    }
    catch (...)
    {
        CString str;
        str.Format( _TEXT( "set cell format falure!" ) );
        //AfxMessageBox( str );
    }
    return _T( "" );
    
    //return (TCHAR*)_bstr_t(range->get_Item(_variant_t((long)1), _variant_t((long)1)));  //這種作法 memory飆升的很快
    //COleVariant vResult = range->get_Value2(); //edisonx   有些地方 抓不出來 為何?
    //vResult.ChangeType( VT_LPWSTR );
    //return (TCHAR*)vResult.bstrVal;            //edisonx 
}


CString ExcelFile::GetCellRange2CStr( long c, long r )
{
    try
    {
        VARIANT v = range->get_Item( _variant_t( (long)r ), _variant_t( (long)c ) );
        CString rs = (TCHAR*)_bstr_t( v );
        ////v.pdispVal->Release();
        VariantClear( &v );
        return rs;
    }
    catch ( ... )
    {
        CString str;
        str.Format( _TEXT( "set cell format falure on ( %d, %d )!" ), c, r );
        //AfxMessageBox( str );
    }
    return _T( "" );

    //COleVariant vResult = range->get_Value2(); //edisonx   有些地方 抓不出來 為何?
    //vResult.ChangeType( VT_LPWSTR );
    //VariantClear( &v );
    //return ( TCHAR* )vResult.bstrVal;            //edisonx
}

int ExcelFile::GetCell2Int()
{
    COleVariant vResult = range->get_Value2(); //edisonx 
    vResult.ChangeType( VT_INT );              //edisonx 
    return vResult.intVal;                     //edisonx 
}

double ExcelFile::GetCell2Double()
{
    COleVariant vResult = range->get_Value2(); //edisonx 
    vResult.ChangeType( VT_R8 );               //edisonx 
    return vResult.dblVal;                     //edisonx 
}

//CString ExcelFile::GetCellForce2CStr()
//{
//    CString strs = ExcelFile::GetCell2CStr();
//    if ( strs == _TEXT( "" ) )
//    {
//        int stri = GetCell2Int();
//        strs.Format( _TEXT( "%d" ), stri );
//    }
//    return strs;
//}


///////////////////////////////////////////////////////////////////////////////////////////
//排序演算法
//    排序演算法有分主要順序和次要順序的設定，可以順序填值，也可以填完值再排序。

//排序
void ExcelFile::Sort( CString IndexCell1, long DeCrement1, CString IndexCell2, long DeCrement2, CString IndexCell3, long DeCrement3 )
{
    VARIANT key1, key2, key3;

    V_VT( &key1 ) = VT_DISPATCH;
    V_DISPATCH( &key1 ) = objSheet->get_Range( COleVariant( IndexCell1 ), COleVariant( IndexCell1 ) );

    if ( IndexCell2.IsEmpty() )
    {
        range->Sort( key1, DeCrement1, VOptional, VOptional, 1, VOptional, 1, 2,//一般選項
            VOptional, _variant_t( (short)TRUE ),//進階 
            1, 1, 1, 1, 1 );//未知選項//edisonx
    }
    else
    {
        V_VT( &key2 ) = VT_DISPATCH;
        V_DISPATCH( &key2 ) = objSheet->get_Range( COleVariant( IndexCell2 ), COleVariant( IndexCell2 ) );

        if ( IndexCell3.IsEmpty() )
        {
            range->Sort( key1, DeCrement1, key2, VOptional, DeCrement2, VOptional, 1, 2,
                VOptional, _variant_t( (short)TRUE ),                                //進階 
                1, 1, 1, 1, 1 );                                                    //未知選項//edisonx
        }
        else
        {
            V_VT( &key3 ) = VT_DISPATCH;
            V_DISPATCH( &key3 ) = objSheet->get_Range( COleVariant( IndexCell3 ), COleVariant( IndexCell3 ) );

            range->Sort( key1, DeCrement1, key2, VOptional, DeCrement2, key3, DeCrement3, 2, //一般選項
                VOptional, _variant_t( (short)TRUE ),                                 //進階 
                1, 1, 1, 1, 1 );                                                     //未知選項//edisonx
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//其它
//    在其它，就將不常用的圖表/圖的新增與設定的操作放在這。
//    這裡的設計就比較不費心思，使用上也許會有種種困擾。小心使用！

void ExcelFile::newChart()
{
    //在Sheet新增圖表
    lpDisp = objSheet->ChartObjects( VOptional );
    chartobjects->AttachDispatch( lpDisp );
    ( *chartobject ) = chartobjects->Add(
        (float)range->get_Left().dblVal,
        (float)range->get_Top().dblVal,
        (float)range->get_Width().dblVal,
        (float)range->get_Height().dblVal );    //圖表符合儲存格範圍的大小
    xlsChart->AttachDispatch( chartobject->get_Chart() );    //資料來源（範圍left, top預設為 比較Item和Group）
}
///////////////////////////////////////////////////////////////////////////////////////////
//圖表操作
//儲存圖表
//edisonx
void ExcelFile::SaveChart( CString FullBmpPathName )
{
    xlsChart->Export( LPCTSTR( FullBmpPathName ), VOptional, VOptional );
}
//選擇表格資料的範圍
ExcelFile* ExcelFile::SelectChartRange( const TCHAR* x1, const TCHAR* x2 )
{
    newChart();

    lpDisp = objSheet->get_Range( COleVariant( x1 ), COleVariant( x2 ) );
    range->AttachDispatch( lpDisp );

    return this;
}

ExcelFile* ExcelFile::SelectChartRange( const TCHAR* x1, int y1, const TCHAR* x2, int y2 )
{
    newChart();

    ZeroMemory( buf1, sizeof( buf1 ) );
    ZeroMemory( buf2, sizeof( buf2 ) );
    _stprintf( buf1, _TEXT( "%s%d" ), x1, y1 );
    _stprintf( buf2, TEXT( "%s%d" ), x2, y2 );

    lpDisp = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
    range->AttachDispatch( lpDisp );
    return this;
}
//小於Z
ExcelFile* ExcelFile::SelectChartRange( TCHAR x1, int y1, TCHAR x2, int y2 )
{
    newChart();

    ZeroMemory( buf1, sizeof( buf1 ) );
    ZeroMemory( buf2, sizeof( buf2 ) );
    //發現了這個問題，但是忘記了是不是故意這樣寫的！
    //是x1, y1; x2, y2?
    _stprintf( buf1, _TEXT( "%c%d" ), x1, y2 );
    _stprintf( buf2, _TEXT( "%c%d" ), x1, y2 );

    lpDisp = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
    range->AttachDispatch( lpDisp );
    return this;
}
//大於Z，開始選AA
ExcelFile* ExcelFile::SelectChartRange( TCHAR xA1, TCHAR xB1, int y1, TCHAR xA2, TCHAR xB2, int y2 )
{
    newChart();
    ZeroMemory( buf1, sizeof( buf1 ) );
    ZeroMemory( buf2, sizeof( buf2 ) );
    _stprintf( buf1, _TEXT( "%c%c%d" ), xA1, xB1, y1 );
    _stprintf( buf2, _TEXT( "%c%c%d" ), xA2, xB2, y2 );
    lpDisp = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
    range->AttachDispatch( lpDisp );
    return this;
}
// 設定表格參數（預設會顯示立體直方圖）
ExcelFile* ExcelFile::SetChart( short XaxisByToporLeft, bool isLabelVisable, CString TitleString, CString XaxisTitle, CString YaxisTitle )
{
    var.vt = VT_DISPATCH;
    var.pdispVal = lpDisp;

    short LabelVisable( FALSE );
    LabelVisable = ( isLabelVisable ) ? (short)TRUE : (short)FALSE;

    xlsChart->ChartWizard(
        var,                                   // const VARIANT& Source.
        COleVariant( (short)11 ),                // const VARIANT& fix please, Gallery: 3d Column. 1 or 11 是否轉動3D（3D類適用, 1轉，11不轉）
        COleVariant( (short)1 ),                 // const VARIANT& fix please, Format, use default
        COleVariant( XaxisByToporLeft ),         // const VARIANT& PlotBy: 1.X  2.Y 圖表的x軸要使用 表格的1:X-top還是2:Y-left
        COleVariant( (short)1 ),                 // const VARIANT& Category Labels fix please 不當軸的那個資料，從第幾個格子開始算（比較群組資料數量）
        COleVariant( (short)1 ),                 // const VARIANT& Series Labels. Start X, 不當軸的那個資料，資料名稱要用幾排格子（更改名字）
        COleVariant( LabelVisable ),             // const VARIANT& HasLegend. 是否要顯示群組資料標籤
        //以下可不填
        _variant_t( COleVariant( TitleString ) ),  // const VARITNT& Title
        _variant_t( COleVariant( XaxisTitle ) ),   // const VARIANT& CategoryTitle
        _variant_t( COleVariant( YaxisTitle ) ),   // const VARIANT& ValueTitle
        VOptional                              // const VARIANT& ExtraTitle
    );
    return this;
}
//插入圖表
void ExcelFile::InsertHistogramChart( int shapeType, bool is3D,
    int isVrticl_Horztl_Other,
    int isNone_Stack_Percent )
{
    long ChartType = 51;
    if ( shapeType == 0 )//Area
    {
        if ( !is3D )   //2D
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 1;          //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 77;    //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 76;    //有百分比
        }
        else        //3D
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = -4098;      //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 78;    //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 79;    //百分比
        }
    }
    else if ( shapeType == 1 )//直方圖
    {
        if ( isVrticl_Horztl_Other == 0 )//直的
        {
            if ( !is3D )   //2D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 51;      //無堆疊
                else if ( isNone_Stack_Percent == 1 ) ChartType = 52; //有堆疊
                else if ( isNone_Stack_Percent == 2 ) ChartType = 53; //有百分比
            }
            else        //3D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 54;      //無堆疊
                else if ( isNone_Stack_Percent == 1 ) ChartType = 55; //有堆疊
                else if ( isNone_Stack_Percent == 2 ) ChartType = 56; //百分比
            }
        }
        else if ( isVrticl_Horztl_Other == 1 )//橫的
        {
            if ( !is3D )   //2D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 57;
                else if ( isNone_Stack_Percent == 1 ) ChartType = 58;
                else if ( isNone_Stack_Percent == 2 ) ChartType = 59;
            }
            else        //3D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 60;
                else if ( isNone_Stack_Percent == 1 ) ChartType = 61;
                else if ( isNone_Stack_Percent == 2 ) ChartType = 62;
            }
        }
        else                                        ChartType = -4100;    //平面 必3D
    }
    else if ( shapeType == 2 )//CONE
    {
        if ( isVrticl_Horztl_Other == 0 )//直的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 92;       //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 93;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 94;  //百分比
        }
        else if ( isVrticl_Horztl_Other == 1 )//橫的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 95;       //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 96;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 97;  //百分比
        }
        else                                    ChartType = 98;  //平面 必3D
    }
    else if ( shapeType == 3 )
    {
        if ( isVrticl_Horztl_Other == 0 )//直的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 99;       //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 100;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 101;  //百分比
        }
        else if ( isVrticl_Horztl_Other == 1 )//橫的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 102;        //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 103;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 104;  //百分比
        }
        else                    ChartType = 105;//平面 必3D
    }
    else if ( shapeType == 4 )
    {
        if ( isVrticl_Horztl_Other == 0 )//直的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 106;       //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 107;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 108;  //百分比
        }
        else if ( isVrticl_Horztl_Other == 1 )//橫的
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 109;       //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 110;  //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 111;  //百分比
        }
        else                                    ChartType = 112;  //平面 必3D
    }
    xlsChart->put_ChartType( (long)ChartType );
}
///////////////////////////////////////////////////////////////
//泡泡
void ExcelFile::InsertBubleChart( bool is3D )
{
    long ChartType = 51;

    if ( is3D )    ChartType = 15;
    else        ChartType = 87;

    xlsChart->put_ChartType( (long)ChartType );
}
//圓環
void ExcelFile::InsertDoughnutChart( bool Explode )
{
    long ChartType = 51;

    if ( !Explode )    ChartType = -4120;
    else            ChartType = 80;

    xlsChart->put_ChartType( (long)ChartType );

}
//曲面
void ExcelFile::InsertSurfaceChart( bool is3D, bool isWire )
{
    long ChartType = 51;

    if ( is3D )
    {
        if ( !isWire )    ChartType = 83;
        else            ChartType = 84;
    }
    else
    {
        if ( !isWire )    ChartType = 85;
        else            ChartType = 86;
    }

    xlsChart->put_ChartType( (long)ChartType );
}
//雷達
void ExcelFile::InsertRadarChart( bool isWire, bool isDot )
{
    long ChartType = 51;

    if ( isWire )
    {
        if ( !isDot ) ChartType = -4151;
        else        ChartType = 81;
    }
    else            ChartType = 82;

    xlsChart->put_ChartType( (long)ChartType );
}
//圓餅
void ExcelFile::InsertPieChart( bool Explode, int type2Dor3DorOf )
{
    long ChartType = 51;

    if ( !Explode )
    {
        if ( type2Dor3DorOf == 0 )        ChartType = 5;
        else if ( type2Dor3DorOf == 1 )   ChartType = -1402;
        else if ( type2Dor3DorOf == 2 )   ChartType = 68;
    }
    else
    {
        if ( type2Dor3DorOf == 0 )        ChartType = 69;
        else if ( type2Dor3DorOf == 1 )   ChartType = 70;
        else if ( type2Dor3DorOf == 2 )   ChartType = 71;
    }

    xlsChart->put_ChartType( ChartType );
}

void ExcelFile::InsertLineChart( bool isDot, bool is3D, int isNone_Stack_Percent )
{
    long ChartType = 51;

    if ( !is3D )//3D
    {
        if ( !isDot )
        {
            if ( isNone_Stack_Percent == 0 )      ChartType = 4;     //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 63;     //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 64;     //有百分比
        }
        else
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 65;     //無堆疊
            else if ( isNone_Stack_Percent == 1 ) ChartType = 66;     //有堆疊
            else if ( isNone_Stack_Percent == 2 ) ChartType = 67;     //有百分比
        }
    }
    else                                        ChartType = -4101;  //3D

    xlsChart->put_ChartType( (long)ChartType );
}
//離散圖
void ExcelFile::InsertXYScatterChart( bool isDot, bool isLine, bool Smooth )
{
    long ChartType = 51;
    if ( !isLine )          ChartType = -4169;  //3D
    else
    {
        if ( Smooth )
        {
            if ( isDot )    ChartType = 72;
            else         ChartType = 73;
        }
        else
        {
            if ( isDot )    ChartType = 74;
            else         ChartType = 75;
        }
    }
    xlsChart->put_ChartType( (long)ChartType );
}

//股票圖
void ExcelFile::InsertStockChart( int StockType )
{
    long ChartType = 51;

    if ( StockType == 0 )    ChartType = 88;
    else if ( StockType == 1 )    ChartType = 89;
    else if ( StockType == 2 )    ChartType = 90;
    else if ( StockType == 3 )    ChartType = 91;

    xlsChart->put_ChartType( (long)ChartType );
}
//--------------------------------------------
//--------------------------------------------
//插入圖（從檔案）
void ExcelFile::InsertImage( const TCHAR* FileNamePath, float x, float y, float Width, float Height )
{
    TCHAR buff[_MAX_PATH];
    if ( GetFullPathName( FileNamePath, _MAX_PATH, buff, NULL ) == 0 )
        _tcscpy( buff, FileNamePath );

    ( *shapes ) = objSheet->get_Shapes();
    shapes->AddPicture(
        buff,                             //LPCTSTR Filename
        false,                            //long LinkToFile
        true,                             //long SaveWithDocument
        x,                                //float Left
        y,                                //float Top
        Width,                            //float Width
        Height                            //float Height
    );
}

void ExcelFile::InsertImage( const TCHAR* FileNamePath, float Width, float Height )
{
    TCHAR buff[_MAX_PATH];
    if ( GetFullPathName( FileNamePath, _MAX_PATH, buff, NULL ) == 0 )
        _tcscpy( buff, FileNamePath );

    ( *shapes ) = objSheet->get_Shapes();
    shapes->AddPicture(
        buff,                             //LPCTSTR Filename
        false,                            //long LinkToFile
        true,                             //long SaveWithDocument
        (float)range->get_Left().dblVal,  //float Left
        (float)range->get_Top().dblVal,   //float Top
        Width,                            //float Width
        Height                            //float Height
    );
}

void ExcelFile::InsertImage( const TCHAR* FileNamePath )
{
    TCHAR buff[_MAX_PATH];
    if ( GetFullPathName( FileNamePath, _MAX_PATH, buff, NULL ) == 0 )
        _tcscpy( buff, FileNamePath );

    ( *shapes ) = objSheet->get_Shapes();
    shapes->AddPicture(
        buff,                             //LPCTSTR Filename
        false,                            //long LinkToFile
        true,                             //long SaveWithDocument
        (float)range->get_Left().dblVal,  //float Left
        (float)range->get_Top().dblVal,   //float Top
        (float)range->get_Width().dblVal, //float Width
        (float)range->get_Height().dblVal //float Height
    );
}