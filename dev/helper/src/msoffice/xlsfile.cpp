
#include <xlnt/xlnt.hpp>
#include <PfStdlib.h>
#include "XlsFile.h"
#include <Debug.h>
//#include <ProgressLog.h>


namespace MsOffice
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    XlsFile::XlsFile()
    {
        m_pWorkBook = NULL;
        m_pWorkSheet = NULL;
        m_pSelectRange = NULL;
        m_pSelectCell = NULL;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    XlsFile::~XlsFile()
    {
        SAFE_DELETE( m_pWorkBook );
        SAFE_DELETE( m_pWorkSheet );
        SAFE_DELETE( m_pSelectCell );
        SAFE_DELETE( m_pSelectRange );
    }

    bool XlsFile::New()
    {
        m_filename.clear();
        m_password.clear();

        SAFE_DELETE( m_pWorkBook );
        m_pWorkBook = new xlnt::workbook();
        SAFE_DELETE( m_pWorkSheet );
        m_pWorkSheet = new xlnt::worksheet( m_pWorkBook->active_sheet() );
        return true;
    }

    bool XlsFile::Load( const tchar_t* filename )
    {
        //if( !m_pWorkBook )
        //    return false;

        dword_t currt = PfTime::GetTickMSTime();

        if( !PfFileExist( filename ) )
            return false;

        m_filename = filename;
        m_password.clear();

        SAFE_DELETE( m_pWorkBook );
        m_pWorkBook = new xlnt::workbook();
        m_pWorkBook->load( m_filename );
        SAFE_DELETE( m_pWorkSheet );
        m_pWorkSheet = new xlnt::worksheet( m_pWorkBook->active_sheet() );

        dword_t currf = ::PfTime::GetTickMSTime() - currt;
#ifdef _UNICODE
        Pf_INFO( _u8( "XlsFile::Open load file %s spend time: %d\n" ), PfStd::UnicodeToUtf8( filename ).c_str(), currf );
#else
        Pf_INFO( ( "XlsFile::Open load file %s spend time: %d\n" ), filename, currf );
#endif
        return true;
    }

    bool XlsFile::Load( const tchar_t* filename, const tchar_t* password )
    {
        //if( !m_pWorkBook )
        //   return false;

        dword_t currt = PfTime::GetTickMSTime();

        if( !PfFileExist( filename ) )
            return false;

        m_filename = filename;
        m_password = password;

        SAFE_DELETE( m_pWorkBook );
        m_pWorkBook = new xlnt::workbook();
#ifdef _UNICODE
        m_pWorkBook->load( PfStd::UnicodeToUtf8( m_filename ), PfStd::UnicodeToUtf8( m_password ) );
#else
        m_pWorkBook->load( ( const _tstring& )m_filename, ( const _tstring& )m_password );
#endif
        SAFE_DELETE( m_pWorkSheet );
        m_pWorkSheet = new xlnt::worksheet( m_pWorkBook->active_sheet() );

        dword_t currf = ::PfTime::GetTickMSTime() - currt;
        //_stprintf_s( buff, _T( "XlsFile::Open load file %s spend time: %d\n" ), filename, currf );
#ifdef _UNICODE
        Pf_INFO( _u8( "XlsFile::Open load file %s spend time: %d\n" ), PfStd::UnicodeToUtf8( filename ).c_str(), currf );
#else
        Pf_INFO( ( "XlsFile::Open load file %s spend time: %d\n" ), filename, currf );
#endif
        return true;
    }

    bool XlsFile::Save()
    {
        if( !m_pWorkBook )
            return false;

        if( m_password.empty() )
        {
            m_pWorkBook->save( m_filename );
        }
        else
        {
#ifdef _UNICODE
            m_pWorkBook->save( PfStd::UnicodeToUtf8( m_filename ), PfStd::UnicodeToUtf8( m_password ) );
#else
            m_pWorkBook->save( m_filename, m_password );
#endif
        }
        return true;
    }

    bool XlsFile::Save( const tchar_t* filename )
    {
        if( !m_pWorkBook )
            return false;

        m_pWorkBook->save( filename );
        return true;
    }

    bool XlsFile::Save( const tchar_t* filename, const tchar_t* password )
    {
        if( !m_pWorkBook )
            return false;

        if( password == NULL )
        {
            m_pWorkBook->save( filename );
        }
        else
        {
#ifdef _UNICODE
            m_pWorkBook->save( PfStd::UnicodeToUtf8( m_filename ), PfStd::UnicodeToUtf8( m_password ) );
#else
            m_pWorkBook->save( m_filename, m_password );
#endif
        }
        return true;
    }

    void XlsFile::ExportAs( const TCHAR*, int pagefrom, int pageto ) //產出的pdf 會從第幾頁到 第幾頁    
    {

    }

    //取得 Sheet 個數
    int XlsFile::SheetTotal()
    {
        if( !m_pWorkBook )
            return -1;

        return ( int )m_pWorkBook->sheet_count();
    }

    //指定SheetName
    void XlsFile::SetSheetName( const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return;

#ifdef _UNICODE
        m_pWorkSheet->title( PfStd::UnicodeToUtf8( sheetname ) );
#else
        m_pWorkSheet->title( sheetname );
#endif    
    }

    //由SheetNumber    指定SheetName
    void XlsFile::SetSheetName( _IN short index, const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return;

        if( index <= 0 ) { index = 1; }
        else if( index >= ( int )m_pWorkBook->sheet_count() ) { index = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws = m_pWorkBook->sheet_by_index( index - 1 );
#ifdef _UNICODE
        ws.title( PfStd::UnicodeToUtf8( sheetname ) );
#else
        ws.title( sheetname );
#endif    
    }

    //由SheetNumber    取得SheetName 從 1 開始
    _tstring XlsFile::GetSheetName( _IN short index )
    {
        if( !m_pWorkBook )
            return _T( "" );

        if( index <= 0 ) { index = 1; }
        else if( index >= ( int )m_pWorkBook->sheet_count() ) { index = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws = m_pWorkBook->sheet_by_index( index - 1 );
#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( ws.title() );
#else
        return ( _tstring )ws.title();
#endif
    }

    //取得所有的sheetname
    _tstring_vector XlsFile::GetSheetNames()
    {
        _tstring_vector sheetnames;
        if( !m_pWorkBook )
            return sheetnames;

#ifdef _UNICODE
        std::vector<std::string> ss = m_pWorkBook->sheet_titles();
        for( std::vector<std::string>::iterator iter = ss.begin(); iter != ss.end(); iter++ )
        {
            sheetnames.push_back( PfStd::Utf8ToUnicode( *iter ) );
        }
#else
        sheetnames = m_pWorkBook->sheet_titles();
#endif
        return sheetnames;
    }

    //由SheetName      取得SheetNumber
    short XlsFile::FindSheetIndex( _IN const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return false;

        return m_pWorkBook->index( ( *m_pWorkSheet ) ) + 1;
    }

    bool XlsFile::AddSheet( _IN const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return false;

        xlnt::worksheet ws = m_pWorkBook->create_sheet();
#ifdef _UNICODE
        ws.title( PfStd::UnicodeToUtf8( sheetname ) );
#else
        ws.title( sheetname );
#endif          
        return true;
    }

    // 在後面加入一個sheet
    bool XlsFile::AddSheet( const tchar_t* sheetname, short indexafter )
    {
        if( !m_pWorkBook )
            return false;

        if( indexafter <= 0 ){ indexafter = 1; }
        else if( indexafter >= ( int )m_pWorkBook->sheet_count() ) { indexafter = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws = m_pWorkBook->create_sheet( indexafter - 1 );
#ifdef _UNICODE
        ws.title( PfStd::UnicodeToUtf8( sheetname ) );
#else
        ws.title( sheetname );
#endif    
        return true;
    }

    //複製SheetName    指定插入Sheet的位置，並指定新Sheet名稱
    bool XlsFile::CopySheet( const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return false;

        xlnt::worksheet ws;
#ifdef _UNICODE
        ws = m_pWorkBook->sheet_by_title( PfStd::UnicodeToUtf8( sheetname ) );

#else
        ws = m_pWorkBook->sheet_by_title( sheetname );
#endif
        m_pWorkBook->copy_sheet( ws );
        return true;
    }

    //複製SheetName    傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet名稱
    bool XlsFile::CopySheet( const tchar_t* sheetname, short indexafter )
    {
        if( !m_pWorkBook )
            return false;

        if( indexafter <= 0 ){ indexafter = 1; }
        else if( indexafter >= ( int )m_pWorkBook->sheet_count() ) { indexafter = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws;
#ifdef _UNICODE
        ws = m_pWorkBook->sheet_by_title( PfStd::UnicodeToUtf8( sheetname ) );

#else
        ws = m_pWorkBook->sheet_by_title( sheetname );
#endif
        m_pWorkBook->copy_sheet( ws, indexafter - 1 );
        return true;
    }

    //複製SheetNumber  指定插入Sheet的位置，並指定名稱
    bool XlsFile::CopySheet( _IN short index )
    {
        if( !m_pWorkBook )
            return false;

        if( index <= 0 ) { index = 1; }
        else if( index >= ( int )m_pWorkBook->sheet_count() ) { index = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws;
        ws = m_pWorkBook->sheet_by_index( index - 1 );
        m_pWorkBook->copy_sheet( ws );
        return true;
    }

    //複製SheetNumber  傳回新Sheet位置 指定插入Sheet的位置，指定要複製的Sheet位置
    bool XlsFile::CopySheet( _IN short index, short indexafter )
    {
        if( !m_pWorkBook )
            return false;

        if( index <= 0 ) { index = 1; }
        else if( index >= ( int )m_pWorkBook->sheet_count() ) { index = ( int )m_pWorkBook->sheet_count(); }
        if( indexafter <= 0 ){ indexafter = 1; }
        else if( indexafter >= ( int )m_pWorkBook->sheet_count() ) { indexafter = ( int )m_pWorkBook->sheet_count(); }

        xlnt::worksheet ws;
        ws = m_pWorkBook->sheet_by_index( index - 1 );
        m_pWorkBook->copy_sheet( ws, indexafter - 1 );
        return true;
    }

    //選SheetName      刪除Sheet
    bool XlsFile::DelSheet( _IN const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return false;

        xlnt::worksheet ws;
#ifdef _UNICODE
        ws = m_pWorkBook->sheet_by_title( PfStd::UnicodeToUtf8( sheetname ) );

#else
        ws = m_pWorkBook->sheet_by_title( sheetname );
#endif
        if( ws == ( *m_pWorkSheet ) )
            ( *m_pWorkSheet ) = m_pWorkBook->sheet_by_index( 0 );
        m_pWorkBook->remove_sheet( ws );
        return true;
    }

    //選SheetNumber    刪除Sheet
    bool XlsFile::DelSheet( _IN short index )
    {
        if( !m_pWorkBook )
            return false;

        xlnt::worksheet ws;
        ws = m_pWorkBook->sheet_by_index( index );
        if( ws == ( *m_pWorkSheet ) )
            ( *m_pWorkSheet ) = m_pWorkBook->sheet_by_index( 0 );
        m_pWorkBook->remove_sheet( ws );
        return true;
    }

    //由SheetName      選擇Sheet
    XlsFile* XlsFile::SelectSheet( _IN const tchar_t* sheetname )
    {
        if( !m_pWorkBook )
            return NULL;

#ifdef _UNICODE
        ( *m_pWorkSheet ) = m_pWorkBook->sheet_by_title( PfStd::UnicodeToUtf8( sheetname ) );
#else
        ( *m_pWorkSheet ) = m_pWorkBook->sheet_by_title( sheetname );
#endif    
        return this;
    }

    //由SheetNumber    選擇Sheet
    XlsFile* XlsFile::SelectSheet( short index )
    {
        if( !m_pWorkBook )
            return NULL;

        ( *m_pWorkSheet ) = m_pWorkBook->sheet_by_index( index );
        return this;
    }

    // 第一個non-empty 起始行
    long  XlsFile::GetColStartCell()
    {
        if( !m_pWorkBook )
            return NULL;

        return m_pWorkSheet->lowest_column().index;
    }

    // 第一個non-empty 起始列
    long  XlsFile::GetRowStartCell()
    {
        if( !m_pWorkBook )
            return NULL;

        return m_pWorkSheet->lowest_row();
    }

    // 總行數 (沒資料的部分也會被取出)
    // 最後一個non-empty 結束行
    long  XlsFile::GetColTotalCell()
    {
        if( !m_pWorkBook )
            return NULL;

        return m_pWorkSheet->highest_column().index;
    }

    // 總列數 (沒資料的部分也會被取出)
    // 最後一個non-empty 結束列
    long  XlsFile::GetRowTotalCell()
    {
        if( !m_pWorkBook )
            return NULL;

        return m_pWorkSheet->highest_row();
    }

    //--------------------------------------------------------------------------
    cell_pos XlsFile::GetCellPos( const tchar_t* refstr )
    {
#ifdef _UNICODE
        xlnt::cell_reference cell_ref( PfStd::UnicodeToUtf8( refstr ) );
#else
        xlnt::cell_reference cell_ref( refstr );
#endif
        cell_pos pos = { cell_ref.column_index(), cell_ref.row() };
        return pos;
    }

    tstring XlsFile::GetCellRef( const cell_pos pos )
    {
        xlnt::cell_reference cell_ref( xlnt::column_t( pos.col ), pos.row );
#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( cell_ref.to_string() );
#else
        return cell_ref.to_string();
#endif
    }

    //clear cell by SelectCell or SelectRange
    void XlsFile::ClearCell()
    {
        if( !m_pWorkBook )
            return;

        if( m_pSelectCell )
        {
            m_pSelectCell->clear_value();
        }
        else if( m_pSelectRange )
        {
            m_pSelectRange->clear_cells();
        }
    }

    //clear cell by ref
    void XlsFile::ClearCell( _IN const tchar_t* ref )
    {
        if( !m_pWorkBook )
            return;

#ifdef _UNICODE
        m_pWorkSheet->clear_cell( PfStd::UnicodeToUtf8( ref ).c_str() );
#else
        m_pWorkSheet->clear_cell( ref );
#endif
    }

    //clear all cell
    void XlsFile::ClearAllCell()
    {
        if( !m_pWorkBook )
            return;

        _mstring cell_ref;
#ifdef _UNICODE
        cell_ref = PfStd::UnicodeToUtf8( _T( "A1" ) );
#else
        cell_ref = _T( "A1" );
#endif

        xlnt::range_reference all_range( xlnt::cell_reference( cell_ref ), xlnt::cell_reference( GetColTotalCell() - 1, GetRowTotalCell() - 1 ) );
        xlnt::range r_range( ( *m_pWorkSheet ), all_range );
        r_range.clear_cells();
    }

    //--------------------------------------------------------------------------
    XlsFile* XlsFile::SelectRange( const tchar_t* refstr )
    {
        if( !m_pWorkBook )
            return NULL;

#ifdef _UNICODE
        xlnt::range select_range = m_pWorkSheet->range( PfStd::UnicodeToUtf8( refstr ) );
#else
        xlnt::range select_range = m_pWorkSheet->range( refstr );
#endif
        SAFE_DELETE( m_pSelectCell );
        SAFE_DELETE( m_pSelectRange );
        m_pSelectRange = new xlnt::range( select_range );
        return this;
    }

    int XlsFile::GetRangeWidth()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return -1;

        return m_pSelectRange->reference().width();
    }

    int XlsFile::GetRangeHeight()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return -1;

        return m_pSelectRange->reference().height();
    }

    _tstring XlsFile::GetRangeTopLeft()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return _T( "" );

#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectRange->reference().top_left().to_string() );
#else
        return m_pSelectRange->reference().top_left().to_string();
#endif
    }

    _tstring XlsFile::GetRangeTopRight()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return _T( "" );

#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectRange->reference().top_right().to_string() );
#else
        return m_pSelectRange->reference().top_left().to_string();
#endif
    }

    _tstring XlsFile::GetRangeBottomLeft()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return _T( "" );

#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectRange->reference().bottom_left().to_string() );
#else
        return m_pSelectRange->reference().top_left().to_string();
#endif
    }

    _tstring XlsFile::GetRangeBottomRight()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectRange ) )
            return _T( "" );

#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectRange->reference().bottom_right().to_string() );
#else
        return m_pSelectRange->reference().top_left().to_string();
#endif
    }


    //-------------------------------------------------------------------------
    XlsFile* XlsFile::SelectCell( const tchar_t* refstr )
    {
        if( !m_pWorkBook )
            return NULL;

#ifdef _UNICODE
        xlnt::cell select_cell = m_pWorkSheet->cell( PfStd::UnicodeToUtf8( refstr ) );
#else
        xlnt::cell select_cell = m_pWorkSheet->cell( refstr );
#endif
        SAFE_DELETE( m_pSelectCell );
        SAFE_DELETE( m_pSelectRange );
        m_pSelectCell = new xlnt::cell( select_cell );
        return this;
    }

    XlsFile* XlsFile::SelectCell( int col, int row )
    {
        if( !m_pWorkBook )
            return NULL;

        xlnt::cell select_cell = m_pWorkSheet->cell( col, row );
        SAFE_DELETE( m_pSelectCell );
        SAFE_DELETE( m_pSelectRange );
        m_pSelectCell = new xlnt::cell( select_cell );
        return this;
    }

    // 傳回位在的行數
    int XlsFile::GetCellCol()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return -1;

        return m_pSelectCell->column().index;
    }

    // 傳回位在的列數
    int XlsFile::GetCellRow()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return -1;

        return m_pSelectCell->row();
    }

    //設定字型（預設新細明體）
    XlsFile* XlsFile::SetFont( const TCHAR* fontType )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return this;

        _mstring fontname;
#ifdef _UNICODE
        fontname = PfStd::UnicodeToUtf8( fontType );
#else
        fontname = fontType;
#endif
        xlnt::font fnt = m_pSelectCell->font();
        fnt.name( fontname );
        m_pSelectCell->font( fnt );
        return this;
    }

    //粗體
    XlsFile* XlsFile::SetFontBold( bool isBold )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return this;

        xlnt::font fnt = m_pSelectCell->font();
        fnt.bold( isBold );
        m_pSelectCell->font( fnt );
        return this;
    }

    //刪除線
    XlsFile* XlsFile::SetFontStrkthrgh( bool isStrkthrgh )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return this;

        xlnt::font fnt = m_pSelectCell->font();
        fnt.strikethrough( isStrkthrgh );
        m_pSelectCell->font( fnt );
        return this;
    }

    //設定字體大小（預設12pt）
    XlsFile* XlsFile::SetFontSize( short fontSize )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return this;

        xlnt::font fnt = m_pSelectCell->font();
        fnt.size( fontSize );
        m_pSelectCell->font( fnt );
        return this;
    }

    //字型顏色（預設黑色）
    XlsFile* XlsFile::SetFontColor( short colorIndex )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return this;

        xlnt::font fnt = m_pSelectCell->font();
        fnt.color( xlnt::color( xlnt::indexed_color( colorIndex ) ) );
        m_pSelectCell->font( fnt );
        return this;
    }

    //  "@";  //設定C欄儲存格格式為文字; "yyyy/MM/dd";  //設定C欄儲存格格式  #,##
    _tstring XlsFile::GetFormat()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return _T( "" );

        xlnt::format fmt = m_pSelectCell->format();
        _tstring fmttext;
#ifdef _UNICODE
        fmttext = PfStd::Utf8ToUnicode( fmt.style().name() );
#else
        fmttext = fmt.style().name();
#endif
        return fmttext;
    }

    //  https://support.office.com/en-us/article/Number-format-codes-5026bbd6-04bc-48cd-bf33-80f18b4eae68
    void XlsFile::SetFormat( const TCHAR* fmtstr )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        xlnt::format fmt = m_pSelectCell->format();
        _mstring fmttext;
#ifdef _UNICODE
        fmttext = PfStd::UnicodeToUtf8( fmtstr );
#else
        fmttext = fmtstr;
#endif
        fmt.style( fmttext );
    }

    // 設巨集
    void XlsFile::SetFormula( const TCHAR* formula )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        _mstring formulatext;
#ifdef _UNICODE
        formulatext = PfStd::UnicodeToUtf8( formula );
#else
        formulatext = formula;
#endif
        m_pSelectCell->formula( formulatext );
    }

    //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
    _tstring XlsFile::GetFormula()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return _T( "" );

#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectCell->formula() );
#else
        return m_pSelectCell->formula();
#endif
    }

    //"$0.00",  #,##0 (千分位)  SelectCell(X, X)->SetNumberFormat(_TEXT("#,##0"));
    void XlsFile::SetNumberFormat( const TCHAR* fmt )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        _mstring fmttext;
#ifdef _UNICODE
        fmttext = PfStd::UnicodeToUtf8( fmt );
#else
        fmttext = fmt;
#endif
        xlnt::number_format fnt( fmttext );
        m_pSelectCell->number_format( fnt );
    }

    void XlsFile::SetCell( bool boolean_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( boolean_value );
    }

    void XlsFile::SetCell( int int_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( int_value );
    }

    void XlsFile::SetCell( uint_t uint_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( uint_value );
    }

    void XlsFile::SetCell( int64_t lint_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( ( long long int )lint_value );
    }

    void XlsFile::SetCell( uint64_t ulint_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( ( unsigned long long int )ulint_value );
    }

    void XlsFile::SetCell( float float_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( float_value );
    }

    void XlsFile::SetCell( double float_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

        m_pSelectCell->value( float_value );
    }


    void XlsFile::SetCell( const tchar_t* string_value )
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return;

#ifdef _UNICODE
        m_pSelectCell->value( PfStd::UnicodeToUtf8( string_value ).c_str() );
#else
        m_pSelectCell->value( string_value );
#endif
    }

    //格式必須是 數值才能取出
    int XlsFile::GetCell2Int()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return 0;

        return PfStd::strtoi( m_pSelectCell->to_string() );
    }

    //格式必須是 文字才能取出
    _tstring XlsFile::GetCell2CStr()
    {
#ifdef _UNICODE
        return PfStd::Utf8ToUnicode( m_pSelectCell->to_string() );
#else
        return m_pSelectCell->to_string();
#endif
    }

    double XlsFile::GetCell2Double()
    {
        if( ( !m_pWorkBook ) || ( !m_pSelectCell ) )
            return 0.0;

        return PfStd::strtof( m_pSelectCell->to_string() );
    }

    //_tstring XlsFile::GetCellRange2CStr( long c, long r )
    //{
    //    return _T( "" );
    //}



}
