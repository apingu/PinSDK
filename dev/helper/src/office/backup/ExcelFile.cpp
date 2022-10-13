#include "stdafx.h"

#include "CApplication.h"
#include "CWorkbooks.h"
#include "CWorksheets.h"
#include "CWorkbook.h"
#include "CWorksheet.h"
#include "CRange.h"
#include "CShapes.h"
#include "CShapeRange.h"
#include "CSheets.h"
#include "CChart.h"
#include "CChartObject.h"
#include "CChartObjects.h"
#include "CInterior.h"
#include "CExcelFont.h"
#include <comdef.h>

#include <assert.h>
#include "ExcelFile.h"
//#include <windows.h>


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
    //Step 1.?亟xcel?蝔?
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
    //COleVariant covTrue((short)FALSE), covFalse((short)FALSE),  covOptional1((long)DISP_E_PARAMNOTFOUND,VT_ERROR);  // ???霈?摮?
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
//蝬剜? ?瑼?/????/摮?/?血??唳? ?擃?敹?
//    璁艙?膩

ExcelFile* ExcelFile::New()
{
    ( *objBooks ) = objApp->get_Workbooks();
    ( *objBook ) = objBooks->Add( VOptional );    //?瑼?
    ( *objSheets ) = objBook->get_Worksheets();
    SelectSheet( 1 );
    return this;
}

ExcelFile* ExcelFile::Open( const TCHAR* path )
{
    TCHAR buff[_MAX_PATH];
    if ( GetFullPathName( path, _MAX_PATH, buff, NULL ) == 0 )
        _tcscpy( buff, path );

    FILE* f = _tfopen( buff, _TEXT( "rb" ) );
    if ( f == NULL )
    {
        return NULL;
    }
    fclose( f );
    m_filename = path;
    ( *objBooks ) = objApp->get_Workbooks();
    objBook->AttachDispatch( objBooks->Add( _variant_t( buff ) ) ); //??銝?歇摮??獢?
    objBook->Activate();
    ( *objSheets ) = objBook->get_Worksheets();
    SelectSheet( 1 );
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
    _tunlink( m_filename.c_str() );

    TCHAR buff[_MAX_PATH];
    ZeroMemory( buff, sizeof( buff ) );
    //_stprintf(buff, _TEXT("%s"), strTableName);  ???
    _tcscpy( buf0, m_filename.c_str() );
    if ( GetFullPathName( buf0, _MAX_PATH, buff, NULL ) != 0 )
        _tcscpy( buf0, buff );


    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
    _tsplitpath( m_filename.c_str(), drive, dir, fname, ext ); // C4996

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

    _trename( save_buff, m_filename.c_str() );
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

// 蝭????銝?隞亙凳????韏瑚?
void ExcelFile::SetUserControl( BOOL f )
{
    objApp->put_UserControl( f );
}


//////////////////////////////////////////////////////////////////////////
//?怠閬?
//    撠望??蝒?箔?嚗?敺??批甈策雿輻??

//SetVisible()
void ExcelFile::SetVisible( bool a )
{
    objApp->put_Visible( a );    //憿舐內Excel瑼?
    objApp->put_UserControl( a );//雿輻??嗅?嚗停銝隞亥????
}

//////////////////////////////////////////////////////////////////////////
//撠heet??雿?
//    ?嗡葉嚗?豢?sheet??撣詨虜??橘?蝔?蝣潭?摰????摮?heet嚗??粹鈭??芣?銝?征?賜?MessageBox()嚗?曆?銋??仿??箔?隞暻潔?嚗鋆∪鈭?憿??隞亙停?沒electSheet()??try-catch?身閮?

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Sheet??

//-------------------------
////?? Sheet ?
long ExcelFile::SheetTotal()
{
    return objSheets->get_Count();//edisonx
}

//-------------------------
////?啣?銝??Sheet
short ExcelFile::AddSheet( const TCHAR* SheetName )
{
    short scn = objSheets->get_Count();
    CWorksheet ws = objSheets->get_Item( COleVariant( (short)scn ) );
    _variant_t p1( ws.m_lpDispatch );
    // ?敺????訾??仿??箔?閬‵???隞??‵1
    objSheets->Add( vtMissing, p1, COleVariant( (short)1 ), COleVariant( (short)1 ) );
    scn += 1;
    ( *objSheet ) = objSheets->get_Item( COleVariant( scn ) );
    objSheet->put_Name( SheetName );//閮剖?sheet?迂
    return scn;
}

//-------------------------
//?崆heetIndex ??SheetName
void ExcelFile::SetSheetName( short SheetIndex, const TCHAR* SheetName )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
        objSheet->put_Name( SheetName );//閮剖?sheet?迂
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("閮剖?蝚?d?heet??摮%s?粹鈭?"), SheetIndex, SheetName);
        str.Format( _TEXT( "set Sheet %d name %s falure!" ), SheetIndex, SheetName );
        AfxMessageBox( str );
    }
}
//-------------------------
//?崆heetIndex ??SheetName
CString ExcelFile::GetSheetName( short SheetIndex )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("??蝚?d?heet???粹鈭?"), SheetIndex);
        str.Format( _TEXT( "get Sheet %d falure!" ), SheetIndex );
        AfxMessageBox( str );
    }
    return objSheet->get_Name();//edisonx
}
//-------------------------
//?崆heetName      ??SheetNumber
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
//?豢?Sheet
//?崆heetName
ExcelFile* ExcelFile::SelectSheet( const TCHAR* SheetName )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( _variant_t( SheetName ) );
        objSheet->Activate();//edisonx
        m_sheetname = SheetName;
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("?豢?Sheet: %s?粹鈭?"), SheetName);
        str.Format( _TEXT( "select Sheet %s failure!" ), SheetName );
        AfxMessageBox( str );
    }
    return this;
}
//?崆heetIndex 
ExcelFile* ExcelFile::SelectSheet( short SheetIndex )
{
    try
    {
        ( *objSheet ) = objSheets->get_Item( COleVariant( SheetIndex ) );
        objSheet->Activate();//edisonx
        m_sheetname = GetSheetName( SheetIndex );
    }
    catch ( ... )
    {
        CString str;
        //str.Format(_TEXT("?豢?蝚?d?heet?粹鈭?"), SheetIndex);
        str.Format( _TEXT( "select Sheet%d failure!" ), SheetIndex );
        AfxMessageBox( str );
    }
    return this;
}
//-------------------------
//銴ˊSheetName ???Sheet??蝵殷?銝行?摰Sheet?迂
void ExcelFile::CopySheet( const TCHAR* SheetName )
{
    objSheet->AttachDispatch( objSheets->get_Item( _variant_t( SheetName ) ), true );
    objSheet->Copy( vtMissing, _variant_t( objSheet ) );
}
//銴ˊSheetIndex ???Sheet??蝵殷?銝行?摰?蝔?
void ExcelFile::CopySheet( short SheetIndex )
{
    objSheet->AttachDispatch( objSheets->get_Item( COleVariant( SheetIndex ) ) );
    objSheet->Copy( vtMissing, _variant_t( objSheet ) );
}
//-------------------------
//?芷Sheet
//?祿heetName 
void ExcelFile::DelSheet( const TCHAR* SheetName )
{
    ( *objSheet ) = objSheets->get_Item( _variant_t( SheetName ) );
    objSheet->Delete();//edisonx
}
//?祿heetIndex
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
//閮剖?? 
//    2:璈怠?  1:?游?
void ExcelFile::SetOrientation( short )
{
    //range->OlePropertySet(TEXT("Orientation"), xlLandscape);
    //objSheet->
}
///////////////////////////////////////////////////////////////////////////////////////////
//撠摮(Cell)??雿?
//    銝?砍虜閬?cell??嚗?其??Ｖ?

///////////////////////////////////////////////////////////////////////////////////////////
//Cell??
//Cell閮閮?
// ??韏瑕???
long ExcelFile::GetHorztlStartCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    return usedrange.get_Column();
}
// ??韏瑕?銵?
long ExcelFile::GetVrticlStartCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    return usedrange.get_Row();
}
// ??蝮賢???
long ExcelFile::GetHorztlTotalCell()
{
    CRange usedrange;
    usedrange.AttachDispatch( objSheet->get_UsedRange() );
    range->AttachDispatch( usedrange.get_Columns() );
    return range->get_Count();
}
// ??蝮質???
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
//皜?脣???
void ExcelFile::ClearCell()
{
    //????????脣???
    range->Clear();//edisonx
}
//?蔥?脣???
ExcelFile* ExcelFile::SetMergeCells( short vMerge, bool isCenterAcross )
{
    //????????脣???
    range->put_MergeCells( _variant_t( vMerge ) );
    if ( isCenterAcross ) SetHorztlAlgmet( HA_CENTERACROSS );
    return this;
}

// ?銵?
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
//?豢??脣???Cell)
//    ?豢??澆???雿?撠勗??舐皛?暺?脣??潔?璅?閬綽????拍車嚗??豢摮?暺蝭?
//    ?嗡葉嚗??豢?sheet銝璅???亙?曆??豢?鈭???摮??刻艘??塚?撠勗?賣?頞?Z)嚗停???嚗?閮剛?鈭ry-catch靘＊蝷箏???摮??

 //-------------------------
 //Cell?澆?閮剖?
 //-------------------------
 //?豢摮?
 //?訾???
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
        //str.Format(_TEXT("?豢??脣???s?粹鈭?"), x);
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
        //str.Format(_TEXT("?豢??脣???s%d?粹鈭?"), x, y);
        str.Format( _TEXT( "select cell %s%d failure!" ), x, y );
        AfxMessageBox( str );
    }
    return this;
}

//撠Z
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
        //str.Format(_TEXT("?豢??脣???c%d?粹鈭?"), x, y);
        str.Format( _TEXT( "select cell %c%d failure!" ), x, y );
        AfxMessageBox( str );
        ASSERT( x >= 'A' && x <= 'Z' );
    }
    return this;
}
//憭扳Z嚗?憪AA
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
        //str.Format(_TEXT("?豢??脣???c%c%d?粹鈭?"), x1, x2, y);
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
//?貊???
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
        //str.Format(_TEXT("?豢?蝭?嚗?%s??s?粹鈭?"), x1, x2);
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
        //str.Format(_TEXT("?豢?蝭?敺?s%d??s%d??孵?臭?"), x1, y1, x2, y2);
        str.Format( _TEXT( "select cell range, from %s%d to %s%d failure !" ), x1, y1, x2, y2 );
        AfxMessageBox( str );
    }
    return this;
}

//撠Z
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
        //str.Format(_TEXT("?豢?蝭?嚗?%c%d??c%d?粹鈭?"), x1, y1, x2, y2);
        str.Format( _TEXT( "select cell range, from %c%d to %c%d failure !" ), x1, y1, x2, y2 );
        AfxMessageBox( str );
        ASSERT( x1 >= 'A' && x1 <= 'Z' );
    }
    return this;
}
//憭扳Z嚗?憪AA
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
        //str.Format(_TEXT("?豢?蝭?嚗?%c%c%d??c%c%d?粹鈭?"), xA1, xB1, y1, xA2, xB2, y2);
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
    long bc = GetHorztlStartCell();                                    // 韏瑕?銵?
    long br = GetVrticlStartCell();                                    // 韏瑕???
    long cc = GetHorztlTotalCell();                                    // 蝮質???
    long cr = GetVrticlTotalCell();                                    // 蝮賢???
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
//?椰/?/??/??/蝵桐葉 ??/???芸???
//    靽??xcel??隞?詨??撘??嗅??詨.h瑼葉嚗?????摮?銝?雿輻?詨??儘霅??舀?擃霈?扼?

//-------------------------
//撠??孵?
//瘞游像撠?
ExcelFile* ExcelFile::SetHorztlAlgmet( short position )
{
    range->put_HorizontalAlignment( COleVariant( position ) );
    return this;
}
//?撠?
ExcelFile* ExcelFile::SetVrticlAlgmet( short position )
{
    range->put_VerticalAlignment( COleVariant( position ) );
    return this;
}
//撠??孵???嗾摨佗?+90~-90嚗?
ExcelFile* ExcelFile::SetTextAngle( short Angle )
{
    range->put_Orientation( COleVariant( Angle ) );
    return this;
}
//閮剖????芸???
ExcelFile* ExcelFile::AutoNewLine( bool NewLine )
{
    if ( NewLine ) range->put_WrapText( VTRUE );
    else           range->put_WrapText( VFALSE );
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////
//閮剖?獢?蝎敦??蝺???
//    ?典?銝???Ｖ?閮剖?獢???蝺??莎?撠???”?潔?隤迎??舀靘輻?鈭??交??閬?銋隞亙?摰?????亥????臬??箸????圈見??瘙??隞亙停瘝?閮剛??見???Ｕ?

//閮剖?獢?蝎敦??蝺???
ExcelFile* ExcelFile::SetCellBorder( long BoarderStyle, int BoarderWeight, long BoarderColor )
{
    range->BorderAround( _variant_t( BoarderStyle ), BoarderWeight, BoarderColor, _variant_t( (long)RGB( 0, 0, 0 ) ), _variant_t( (long)RGB( 0, 0, 0 ) ) );
    return this;
}

///////////////////////////////////////////////////////////////////////////////////////////
//?芸?甈祝??擃?
//    ?汞xcel銝哨??芾?撠?隤踵?潛???寥??拐?嚗?舫?矽?湧?嗆?撖砍?????
//    Excel?身???ｇ???⊥?銝??豢?摰?閬??詨??湔?嚗?隤踵??韏瑚??身閮?瘥??渲死??

//閮剖?甈祝??
//?芸?隤踵??
void ExcelFile::AutoFitWidth()
{
    ( *col ) = range->get_EntireColumn();    //?詨?????銝?湔?
    col->AutoFit();                          //?芸?隤踵銝?湔???撖?
}
//?芸?隤踵甈祝
void ExcelFile::AutoFitHight()
{
    ( *row ) = range->get_EntireRow();       //?詨?????銝?湔?
    row->AutoFit();                          //?芸?隤踵銝?湔???擃?
}
//閮剖???
ExcelFile* ExcelFile::SetCellHeight( float height )
{
    range->put_RowHeight( _variant_t( height ) );
    return this;
}
//閮剖?甈祝
ExcelFile* ExcelFile::SetCellWidth( float height )
{
    range->put_ColumnWidth( _variant_t( height ) );
    return this;
}
///////////////////////////////////////////////////////////////////////////////////////////
//?脣??澆?????
//    摮????脯?擃??之撠?..?典極雿?銝?????虜?函??嚗撱箔?靘???
//    憿撠嚗???....??隞塚?XDDD

//閮剖?摮?
ExcelFile* ExcelFile::SetFont( const TCHAR* fontType )
{
    ( *font ) = range->get_Font();
    font->put_Name( _variant_t( fontType ) );//??舫?????
    return this;
}
//蝎?
ExcelFile* ExcelFile::SetFontBold( bool isBold )
{
    ( *font ) = range->get_Font();
    if ( isBold )  font->put_Bold( VTRUE );//蝎?
    else           font->put_Bold( VFALSE );
    return this;
}
//?芷蝺?
ExcelFile* ExcelFile::SetFontStrkthrgh( bool isStrike )
{
    ( *font ) = range->get_Font();
    if ( isStrike )  font->put_Strikethrough( VTRUE );    //edisonx
    else             font->put_Strikethrough( VFALSE );   //edisonx
    return this;
}
//摮?憭批?
ExcelFile* ExcelFile::SetFontSize( short fontSize )
{
    ( *font ) = range->get_Font();
    font->put_Size( _variant_t( fontSize ) );//摮?憭批?ptl
    return this;
}
//摮?憿
ExcelFile* ExcelFile::SetFontColor( short colorIndex )
{
    ( *font ) = range->get_Font();
    font->put_ColorIndex( _variant_t( colorIndex ) ); //摮(?身暺)
    return this;
}

//ExcelFile* ExcelFile::SetFontColor( DWORD color )
//{
//    ( *font ) = range->get_Font();
//    font->put_Color( _variant_t( color ) ); //摮(?身暺)
//    return this;
//}

///////////////////////////////////////////////////////////////////////////////////////////
//憛急遛?脣蔗
//    ?典‵皛輯敶拇?嚗身閮?雿輻摨扳??撘???嚗迨?臭蝙?汞xcel閬?頠?隞銝?隤輯?斤?雿蔭?箸???
//    ?斗迨銋?嚗??翰??脖??ｇ?銝??芣?撟曉??脫?湔迨隞??

//閮剖?摨
ExcelFile* ExcelFile::SetCellColor( short colorIndex )
{
    ( *cell ) = range->get_Interior();                   //???詨?蝭?嚗身摰摮???園?雿?
    cell->put_ColorIndex( _variant_t( colorIndex ) );   //閮剖?摨嚗銵剁?
    //cell->SetColor(_variant_t(colorIndex));
    return this;
}

ExcelFile* ExcelFile::SetCellColor( DWORD color )                                //閮剖?摨
{
    ( *cell ) = range->get_Interior();                   //???詨?蝭?嚗身摰摮???園?雿?
    cell->put_Color( _variant_t( color ) );   //閮剖?摨嚗銵剁?
    //cell->SetColor(_variant_t(colorIndex));
    return this;
}

//?豢?憿嚗???脣?摨嚗?excel隞?漣璅????
short ExcelFile::SelectColor( short x, short y )
{
    //Microsoft Excel ???脫?摨靘?
    //蝝??????????換?嚗嚗? - 8嚗瘛勗瘛綽?y嚗?1 - 7
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
    return 2;//?身?質
}
short ExcelFile::SelectColor( const TCHAR ColorChar )
{
    switch ( ColorChar )
    {
        //暺
    case 'D':
    case 'd':
        return 1;
        break;
        //?質
    case 'W':
    case 'w':
        return 2;
        break;
        //蝝
    case 'R':
    case 'r':
        return 3;
        break;
        //蝬
    case 'G':
    case 'g':
        return 4;
        break;
        //?
    case 'B':
    case 'b':
        return 5;
        break;
        //暺
    case 'Y':
    case 'y':
        return 6;
        break;
    }
    return 2;//?身?質
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
//Cell憛怠?
//    ?舀?潛車撣貊?摮?澆?嚗??臭??舀???澆??蝙?其?閬?敹?

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
    
    //return (TCHAR*)_bstr_t(range->get_Item(_variant_t((long)1), _variant_t((long)1)));  //?車雿? memory憌???敹?
    //COleVariant vResult = range->get_Value2(); //edisonx   ???唳 ???箔? ?箔??
    //vResult.ChangeType( VT_LPWSTR );
    //return (TCHAR*)vResult.bstrVal;            //edisonx 
}


CString ExcelFile::GetCellRange2CStr( long c, long r )
{
    VARIANT v = range->get_Item( _variant_t( (long)r ), _variant_t( (long)c ) );
    CString rs = (TCHAR*)_bstr_t( v );
    //v.pdispVal->Release();
    VariantClear( &v );
    return rs;
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
//??瞍?瘜?
//    ??瞍?瘜??蜓閬?摨?甈∟????身摰??臭誑??憛怠潘?銋隞亙‵摰澆?????

//??
void ExcelFile::Sort( CString IndexCell1, long DeCrement1, CString IndexCell2, long DeCrement2, CString IndexCell3, long DeCrement3 )
{
    VARIANT key1, key2, key3;

    V_VT( &key1 ) = VT_DISPATCH;
    V_DISPATCH( &key1 ) = objSheet->get_Range( COleVariant( IndexCell1 ), COleVariant( IndexCell1 ) );

    if ( IndexCell2.IsEmpty() )
    {
        range->Sort( key1, DeCrement1, VOptional, VOptional, 1, VOptional, 1, 2,//銝?祇??
            VOptional, _variant_t( (short)TRUE ),//?脤? 
            1, 1, 1, 1, 1 );//?芰?賊?//edisonx
    }
    else
    {
        V_VT( &key2 ) = VT_DISPATCH;
        V_DISPATCH( &key2 ) = objSheet->get_Range( COleVariant( IndexCell2 ), COleVariant( IndexCell2 ) );

        if ( IndexCell3.IsEmpty() )
        {
            range->Sort( key1, DeCrement1, key2, VOptional, DeCrement2, VOptional, 1, 2,
                VOptional, _variant_t( (short)TRUE ),                                //?脤? 
                1, 1, 1, 1, 1 );                                                    //?芰?賊?//edisonx
        }
        else
        {
            V_VT( &key3 ) = VT_DISPATCH;
            V_DISPATCH( &key3 ) = objSheet->get_Range( COleVariant( IndexCell3 ), COleVariant( IndexCell3 ) );

            range->Sort( key1, DeCrement1, key2, VOptional, DeCrement2, key3, DeCrement3, 2, //銝?祇??
                VOptional, _variant_t( (short)TRUE ),                                 //?脤? 
                1, 1, 1, 1, 1 );                                                     //?芰?賊?//edisonx
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//?嗅?
//    ?典摰?撠勗?銝虜?函??”/???啣??身摰????曉??
//    ?ㄐ?身閮停瘥?銝祥敹?雿輻銝?閮望??車蝔桀?整?敹蝙?剁?

void ExcelFile::newChart()
{
    //?沒heet?啣??”
    lpDisp = objSheet->ChartObjects( VOptional );
    chartobjects->AttachDispatch( lpDisp );
    ( *chartobject ) = chartobjects->Add(
        (float)range->get_Left().dblVal,
        (float)range->get_Top().dblVal,
        (float)range->get_Width().dblVal,
        (float)range->get_Height().dblVal );    //?”蝚血??脣??潛???憭批?
    xlsChart->AttachDispatch( chartobject->get_Chart() );    //鞈?靘?嚗??eft, top?身??瘥?Item?roup嚗?
}
///////////////////////////////////////////////////////////////////////////////////////////
//?”??
//?脣??”
//edisonx
void ExcelFile::SaveChart( CString FullBmpPathName )
{
    xlsChart->Export( LPCTSTR( FullBmpPathName ), VOptional, VOptional );
}
//?豢?銵冽鞈?????
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
//撠Z
ExcelFile* ExcelFile::SelectChartRange( TCHAR x1, int y1, TCHAR x2, int y2 )
{
    newChart();

    ZeroMemory( buf1, sizeof( buf1 ) );
    ZeroMemory( buf2, sizeof( buf2 ) );
    //?潛鈭?憿?雿敹?鈭銝???見撖怎?嚗?
    //?矩1, y1; x2, y2?
    _stprintf( buf1, _TEXT( "%c%d" ), x1, y2 );
    _stprintf( buf2, _TEXT( "%c%d" ), x1, y2 );

    lpDisp = objSheet->get_Range( COleVariant( buf1 ), COleVariant( buf2 ) );
    range->AttachDispatch( lpDisp );
    return this;
}
//憭扳Z嚗?憪AA
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
// 閮剖?銵冽?嚗?閮剜?憿舐內蝡??湔??
ExcelFile* ExcelFile::SetChart( short XaxisByToporLeft, bool isLabelVisable, CString TitleString, CString XaxisTitle, CString YaxisTitle )
{
    var.vt = VT_DISPATCH;
    var.pdispVal = lpDisp;

    short LabelVisable( FALSE );
    LabelVisable = ( isLabelVisable ) ? (short)TRUE : (short)FALSE;

    xlsChart->ChartWizard(
        var,                                   // const VARIANT& Source.
        COleVariant( (short)11 ),                // const VARIANT& fix please, Gallery: 3d Column. 1 or 11 ?臬頧?3D嚗?D憿?? 1頧?11銝?嚗?
        COleVariant( (short)1 ),                 // const VARIANT& fix please, Format, use default
        COleVariant( XaxisByToporLeft ),         // const VARIANT& PlotBy: 1.X  2.Y ?”?頠貉?雿輻 銵冽??:X-top?2:Y-left
        COleVariant( (short)1 ),                 // const VARIANT& Category Labels fix please 銝頠貊??????敺洵撟曉摮?憪?嚗?頛黎蝯????
        COleVariant( (short)1 ),                 // const VARIANT& Series Labels. Start X, 銝頠貊??????鞈??迂閬撟暹??澆?嚗?孵?摮?
        COleVariant( LabelVisable ),             // const VARIANT& HasLegend. ?臬閬＊蝷箇黎蝯???蝐?
        //隞乩??臭?憛?
        _variant_t( COleVariant( TitleString ) ),  // const VARITNT& Title
        _variant_t( COleVariant( XaxisTitle ) ),   // const VARIANT& CategoryTitle
        _variant_t( COleVariant( YaxisTitle ) ),   // const VARIANT& ValueTitle
        VOptional                              // const VARIANT& ExtraTitle
    );
    return this;
}
//??”
void ExcelFile::InsertHistogramChart( int shapeType, bool is3D,
    int isVrticl_Horztl_Other,
    int isNone_Stack_Percent )
{
    long ChartType = 51;
    if ( shapeType == 0 )//Area
    {
        if ( !is3D )   //2D
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 1;          //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 77;    //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 76;    //???
        }
        else        //3D
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = -4098;      //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 78;    //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 79;    //?曉?瘥?
        }
    }
    else if ( shapeType == 1 )//?湔??
    {
        if ( isVrticl_Horztl_Other == 0 )//?渡?
        {
            if ( !is3D )   //2D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 51;      //?∪???
                else if ( isNone_Stack_Percent == 1 ) ChartType = 52; //????
                else if ( isNone_Stack_Percent == 2 ) ChartType = 53; //???
            }
            else        //3D
            {
                if ( isNone_Stack_Percent == 0 ) ChartType = 54;      //?∪???
                else if ( isNone_Stack_Percent == 1 ) ChartType = 55; //????
                else if ( isNone_Stack_Percent == 2 ) ChartType = 56; //?曉?瘥?
            }
        }
        else if ( isVrticl_Horztl_Other == 1 )//璈怎?
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
        else                                        ChartType = -4100;    //撟喲 敹?D
    }
    else if ( shapeType == 2 )//CONE
    {
        if ( isVrticl_Horztl_Other == 0 )//?渡?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 92;       //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 93;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 94;  //?曉?瘥?
        }
        else if ( isVrticl_Horztl_Other == 1 )//璈怎?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 95;       //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 96;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 97;  //?曉?瘥?
        }
        else                                    ChartType = 98;  //撟喲 敹?D
    }
    else if ( shapeType == 3 )
    {
        if ( isVrticl_Horztl_Other == 0 )//?渡?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 99;       //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 100;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 101;  //?曉?瘥?
        }
        else if ( isVrticl_Horztl_Other == 1 )//璈怎?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 102;        //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 103;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 104;  //?曉?瘥?
        }
        else                    ChartType = 105;//撟喲 敹?D
    }
    else if ( shapeType == 4 )
    {
        if ( isVrticl_Horztl_Other == 0 )//?渡?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 106;       //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 107;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 108;  //?曉?瘥?
        }
        else if ( isVrticl_Horztl_Other == 1 )//璈怎?
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 109;       //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 110;  //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 111;  //?曉?瘥?
        }
        else                                    ChartType = 112;  //撟喲 敹?D
    }
    xlsChart->put_ChartType( (long)ChartType );
}
///////////////////////////////////////////////////////////////
//瘜⊥部
void ExcelFile::InsertBubleChart( bool is3D )
{
    long ChartType = 51;

    if ( is3D )    ChartType = 15;
    else        ChartType = 87;

    xlsChart->put_ChartType( (long)ChartType );
}
//?
void ExcelFile::InsertDoughnutChart( bool Explode )
{
    long ChartType = 51;

    if ( !Explode )    ChartType = -4120;
    else            ChartType = 80;

    xlsChart->put_ChartType( (long)ChartType );

}
//?脤
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
//?琿?
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
//??
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
            if ( isNone_Stack_Percent == 0 )      ChartType = 4;     //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 63;     //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 64;     //???
        }
        else
        {
            if ( isNone_Stack_Percent == 0 ) ChartType = 65;     //?∪???
            else if ( isNone_Stack_Percent == 1 ) ChartType = 66;     //????
            else if ( isNone_Stack_Percent == 2 ) ChartType = 67;     //???
        }
    }
    else                                        ChartType = -4101;  //3D

    xlsChart->put_ChartType( (long)ChartType );
}
//?Ｘ??
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

//?∠巨??
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
//???敺?獢?
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