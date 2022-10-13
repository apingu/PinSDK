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
//
//
//
//
//
#include "stdafx.h"
#include "mfc-utility.h"
#include <pf-stdlib.h>



void mfcCreateDlg( UINT nID, CDialog* dlg, CWnd* pParentWnd )
{
    if( !::IsWindow( dlg->m_hWnd ) )
        dlg->Create( nID, pParentWnd );
    return;
}

BOOL mfcTurnDlg( CDialog* dlg )
{
    if( !::IsWindow( dlg->m_hWnd ) )
        return FALSE;
    //dlg->Create(nID);
    if( dlg->IsWindowVisible() )
    {
        dlg->ShowWindow( SW_HIDE );
        return 2;
    }
    else
    {
        dlg->ShowWindow( SW_SHOWNORMAL );
        return TRUE;
    }
}

BOOL mfcShowDlg( CDialog* dlg )
{
    if( !::IsWindow( dlg->m_hWnd ) )
        return FALSE;
    //dlg->Create(nID);
    dlg->ShowWindow( SW_SHOWNORMAL );
    return TRUE;
}

void mfcHideDlg( CDialog* dlg )
{
    if( ::IsWindow( dlg->m_hWnd ) )
        dlg->ShowWindow( SW_HIDE );
    return;
}

BOOL mfcIsDlgShow( CDialog* dlg )
{
    if( ::IsWindow( dlg->m_hWnd ) )
    {
        return IsWindowVisible( dlg->m_hWnd );
        //WINDOWPLACEMENT lpwndpl;
        //dlg->GetWindowPlacement( &lpwndpl );
        //if ( lpwndpl.showCmd == SW_SHOWNORMAL )
        //    return TRUE;
    }
    return FALSE;
}

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT msg, LPARAM lp, LPARAM pData )
{
    switch( msg )
    {
        case BFFM_INITIALIZED:
            ::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, ( LPARAM )pData );
            break;
    }
    return 0;
}

TCHAR* mfcOpenFolderBrowseDlg( PTSTR outpath, HWND hwndOwner, PCTSTR beginfolder, UINT ulFlags )
{
    TCHAR* result = outpath;
    CString szTextureDir( outpath );
    TCHAR sDisplayName[ MAX_PATH ];

    TCHAR fullbeginfolder[ 1024 ];
    GetFullPathName( beginfolder, 1024, fullbeginfolder, NULL );

    //Load prompt for dialog box from resource
    CString szTitle;
    //szTitle.LoadString(IDS_SELECT_TEXTUREDIR);

    //Create and initialise the browse info struct
    BROWSEINFO bi;
    bi.hwndOwner = hwndOwner;
    bi.pidlRoot = 0X0;
    bi.pszDisplayName = sDisplayName;
    bi.lpszTitle = szTitle;
    bi.ulFlags = 0x0;
    //bi.lpfn = 0x0;
    //int iRetVal       = IDYES;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = ( LPARAM )fullbeginfolder;
    //szPath=szDirName;

    //Get the full path of the folder
    ITEMIDLIST* idl = SHBrowseForFolder( &bi );
    if( idl )
    {
        SHGetPathFromIDList( idl, szTextureDir.GetBuffer( MAX_PATH ) );
        if( outpath != NULL )
            _tcscpy( outpath, szTextureDir.GetBuffer( MAX_PATH ) );
    }
    else
    {
        result = NULL;
    }

    //Force cstring to update
    szTextureDir.ReleaseBuffer();
    return result;

    /*
    TCHAR*  result = (TCHAR*)outpath;
    CString szStringureDir((TCHAR*)outpath);
    TCHAR   sDisplayName[_MAX_PATH];


    TCHAR ADir[1024];
    GetFullPathName(beginfolder, 1024, ADir, NULL);

    //Load prompt for dialog box from resource
    CString szTitle;
    //szTitle.LoadString(IDS_SELECT_StringUREDIR);

    //Create and initialise the browse info struct
    BROWSEINFO bi;
    bi.hwndOwner = hwndOwner;
    bi.pidlRoot = 0X0;
    bi.pszDisplayName = sDisplayName;
    bi.lpszTitle = szTitle;
    bi.ulFlags = ulFlags;
    //bi.lpfn = 0x0;
    //int iRetVal       = IDYES;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)ADir;
    //szPath=szDirName;

    //Get the full path of the folder
    ITEMIDLIST* idl = SHBrowseForFolder(&bi);
    if(idl)
    {
        SHGetPathFromIDList(idl, szStringureDir.GetBuffer(_MAX_PATH));
        if(outpath != NULL)
            _tcscpy((PTSTR)outpath, szStringureDir.GetBuffer(_MAX_PATH));
    }
    else
    {
        result = NULL;
    }

    //Force cstring to update
    szStringureDir.ReleaseBuffer();
    return result;
    */
}

BOOL mfcIsWindowVisible( CWnd* wnd )
{
    return wnd->IsWindowVisible();
}

CPoint mfcGetDlgPos( CDialog* dlg )
{
    CPoint pos;
    if( ::IsWindow( dlg->m_hWnd ) )
    {
        CRect rect;
        dlg->GetWindowRect( rect );
        pos.x = rect.left;
        pos.y = rect.top;
    }
    return pos;
}

void mfcMoveDlg( CDialog* dlg, CPoint pt )
{
    if( ::IsWindow( dlg->m_hWnd ) )
    {
        CRect rect;
        dlg->GetWindowRect( rect );
        int dx = rect.Width();
        int dy = rect.Height();

        RECT rt;
        rt.left = pt.x;
        rt.top = pt.y;
        rt.right = rt.left + dx;
        rt.bottom = rt.top + dy;

        dlg->MoveWindow( &rt );
    }
}

///////////////////////////////////////////////////////////////////////////////
int mfcGetDragFiles( _tcsvector& files, HDROP hDropInfo )
{
    UINT  uNumFiles;
    TCHAR szNextFile[ MAX_PATH ];

    // Get the # of files being dropped.
    uNumFiles = DragQueryFile( hDropInfo, -1, NULL, 0 );

    for( UINT uFile = 0; uFile < uNumFiles; uFile++ )
    {
        // Get the next filename from the HDROP info.
        if( DragQueryFile( hDropInfo, uFile, szNextFile, MAX_PATH ) > 0 )
        {
            // ***
            // Do whatever you want with the filename in szNextFile.
            // ***
            //TCHAR drive[ _MAX_DRIVE ];
            //TCHAR dir[ _MAX_DIR ];
            //TCHAR fname[ _MAX_FNAME ];
            //TCHAR ext[ _MAX_EXT ];
            //
            //_tsplitpath_s( szNextFile, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
            files.push_back( szNextFile );
        }
    }
    // Free up memory.
    DragFinish( hDropInfo );
    return uNumFiles;
}


///////////////////////////////////////////////////////////////////////////////////////
// CMFCButton
// 
// 設定 文字 與背景顏色
void MFCSetButtonColor( _IN CMFCButton* button, _IN COLORREF text_color, _IN COLORREF face_color )
{
    //CRect rc;
    //int fillet_radius = 10;
    //button->GetWindowRect( &rc );
    //CRgn rgn;
    //rgn.CreateRoundRectRgn( 0, 0, rc.Width(), rc.Height(), fillet_radius, fillet_radius);
    //button->SetWindowRgn( rgn, TRUE );

    button->EnableWindowsTheming( FALSE );
    button->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button->m_bTransparent = FALSE;
    button->SetTextColor( text_color );
    button->SetFaceColor( face_color, true );
}


///////////////////////////////////////////////////////////////////////////////
//    Class mfcCSheet


mfcCSheet::mfcCSheet()
{
    m_size.cx = 200;
    m_size.cy = 500;
    ClearAllPage();
}

mfcCSheet::~mfcCSheet()
{
    m_cDlgVec.clear();
}

void mfcCSheet::ClearAllPage( void )
{
    m_parent = 0;
    m_selectpage = -1;
    m_iDlgVec.clear();
    m_cDlgVec.clear();
}


void mfcCSheet::Create( CRect rt, CWnd* parent )
{
    m_parent = parent;
    m_situs.x = rt.left;
    m_situs.y = rt.top;
    m_size.cx = rt.Width();
    m_size.cy = rt.Height();
    return;
}

/*
void mfcCSheet::SetSize(CSize s)
{
    m_size = s;
    return;
}
*/

void mfcCSheet::PageInsert( UINT nID, CDialog* dlg )
{
    if( !::IsWindow( dlg->m_hWnd ) )
        dlg->Create( nID, m_parent );
    m_iDlgVec.push_back( nID );
    m_cDlgVec.push_back( dlg );
}

void mfcCSheet::PageRemove( UINT nID )
{
    for( UINT i = 0; i < m_cDlgVec.size(); i++ )
    {
        if( m_iDlgVec[ i ] == nID )
        {
            m_cDlgVec.erase( m_cDlgVec.begin() + i );
            m_iDlgVec.erase( m_iDlgVec.begin() + i );
            return;
        }
    }
}

void mfcCSheet::PageRemove( CDialog* dlg )
{
    for( UINT i = 0; i < m_cDlgVec.size(); i++ )
    {
        if( m_cDlgVec[ i ] == dlg )
        {
            m_cDlgVec.erase( m_cDlgVec.begin() + i );
            m_iDlgVec.erase( m_iDlgVec.begin() + i );
            return;
        }
    }
}

void mfcCSheet::PageOpen( UINT sID )
{
    for( UINT p = 0; p < m_iDlgVec.size(); p++ )
    {
        if( m_iDlgVec[ p ] == sID )
        {
            sID = p;
            break;
        }
    }

    if( m_selectpage == sID )
        return;

    if( m_selectpage >= 0 )
    {
        CRect rt;
        m_cDlgVec[ m_selectpage ]->GetWindowRect( &rt );
        m_situs.x = rt.left;
        m_situs.y = rt.top;
    }

    m_cDlgVec[ sID ]->MoveWindow( m_situs.x, m_situs.y, m_size.cx, m_size.cy );
    m_selectpage = sID;
    m_cDlgVec[ sID ]->ShowWindow( SW_SHOWNORMAL );

    for( UINT i = 0; i < m_cDlgVec.size(); i++ )
    {
        if( sID != i )
            m_cDlgVec[ i ]->ShowWindow( SW_HIDE );
    }

    return;
}


void mfcCSheet::PageClose( UINT sID )
{
    for( UINT p = 0; p < m_iDlgVec.size(); p++ )
    {
        if( m_iDlgVec[ p ] == sID )
            m_cDlgVec[ p ]->ShowWindow( SW_HIDE );
    }
    return;

    m_cDlgVec[ sID ]->ShowWindow( SW_HIDE );
    m_selectpage = -1;
    return;
}

UINT mfcCSheet::PageSize( void )
{
    return m_cDlgVec.size();
}


CDialog* mfcCSheet::CurPage( void )
{
    return m_cDlgVec[ m_selectpage ];
}


///////////////////////////////////////////////////////////////////////////////////////
// Popup menu

MfcCPopupMenu::MfcCPopupMenu( UINT nId, UINT sub, CWnd* pParent )
{
    m_pParent = pParent;
    m_menu.LoadMenu( nId );

    //if (m_pParent->nLayer == LAY_MOVEMENT)
    m_subMenu = m_menu.GetSubMenu( sub );
}


MfcCPopupMenu::~MfcCPopupMenu()
{
    m_menu.DestroyMenu();
}


BOOL MfcCPopupMenu::SetBitmap( UINT nSubId,
                               const CBitmap* pUnchecked,
                               const CBitmap* pChecked )
{
    return m_subMenu->SetMenuItemBitmaps( nSubId, MF_BYCOMMAND, pUnchecked, pChecked );
}


BOOL MfcCPopupMenu::AddBitmapItem( UINT nIDNewItem, const CBitmap* pBitmap )
{
    return m_subMenu->AppendMenu( nIDNewItem, MF_POPUP, pBitmap );
}


void MfcCPopupMenu::Popup( CPoint pos, bool aschild )
{
    //m_pParent = pParent;

    //CMenu menu;
    //CMenu *subMenu;

    //m_menu.LoadMenu(nId);

    //if (m_pParent->nLayer == LAY_MOVEMENT)
    //m_subMenu = m_menu.GetSubMenu(sub);
    //else
    //    subMenu = menu.GetSubMenu(0);

    //if (nLayer == LAY_TERRAIN)
    //{
    //subMenu->InsertMenu(1, MF_BYPOSITION | MF_STRING, IDM_CLEAR_AS_NULL, "Clear AS Null Cell");
    //}

    //submenu->SetMenuItemBitmaps(ID_HELP_TEST, MF_BYCOMMAND, &m_CheckBitmap, &m_UnCheckBitmap);

    if( aschild )
        m_pParent->ClientToScreen( &pos );
    m_subMenu->TrackPopupMenu( TPM_RIGHTBUTTON, pos.x, pos.y, m_pParent );
    //menu.DestroyMenu();
}


void mfcImageListCreate( CImageList* imagelist, UINT nID, int cx, int cy, COLORREF crMask, UINT nFlags )
{
    CBitmap bmp;
    bmp.LoadBitmap( nID );
    BITMAP bmpObj;
    bmp.GetBitmap( &bmpObj );

    imagelist->Create( cx, cy, nFlags, 0, 0 );
    imagelist->Add( &bmp, crMask );
}

HBITMAP mfcImageListGetBitmap( CImageList* imagelist, int nIndex )
{
    HICON hicon = imagelist->ExtractIcon( nIndex );

    ICONINFO info;
    ::GetIconInfo( hicon, &info );
    BITMAP   bmp;
    ::GetObject( info.hbmColor, sizeof( BITMAP ), ( LPVOID )&bmp );
    HBITMAP hBmp = ( HBITMAP )::CopyImage( info.hbmColor, IMAGE_BITMAP, 0, 0, 0 );
    ::DeleteObject( info.hbmColor );
    ::DeleteObject( info.hbmMask );
    return hBmp;


    //IMAGEINFO imageInfo;
    //imagelist->GetImageInfo(nIndex, &imageInfo);
    //return (HBITMAP)CopyImage(imageInfo.hbmImage, IMAGE_BITMAP,
    //                          0, 0, LR_COPYRETURNORG);
}

///////////////////////////////////////////////////////////////////////////////////////
// Sizing
MfcCtrlResize::MfcCtrlResize( CWnd* pMainWnd )
{
    m_pMainWnd = pMainWnd;
}


MfcCtrlResize::~MfcCtrlResize()
{

}


#define _MAX_MFC_RESING_RATE  10000
void MfcCtrlResize::AddDlgItem( int nID )
{
    if( m_pMainWnd == NULL )
        return;

    CWnd* pnWnd = m_pMainWnd->GetDlgItem( nID );
    if( pnWnd == NULL )
        return;

    CRect rtHwnd;
    m_pMainWnd->GetWindowRect( rtHwnd );

    CRect rtCtrl;
    pnWnd->GetWindowRect( rtCtrl );

    float w = ( float )rtHwnd.Width();
    float h = ( float )rtHwnd.Height();

    float px = ( float )( ( float )( rtCtrl.left - rtHwnd.left ) / w );
    float py = ( float )( ( float )( rtCtrl.top - rtHwnd.top ) / h );
    float cx = ( float )( ( float )( rtCtrl.Width() ) / w );
    float cy = ( float )( ( float )( rtCtrl.Height() ) / h );

    CRect dis;
    dis.top = py * _MAX_MFC_RESING_RATE;
    dis.left = px * _MAX_MFC_RESING_RATE;
    dis.right = cx * _MAX_MFC_RESING_RATE;
    dis.bottom = cy * _MAX_MFC_RESING_RATE;

    _tstring debug_output;
    _pf_tstrprintf( debug_output, _T( "MfcCtrlResize::AddDlgItem [ id: %d, t:%d, l:%d, r:%d, d:%d]\n" ), nID, dis.top, dis.left, dis.right, dis.bottom );
    OutputDebugString( debug_output.c_str() );

    m_CtrlhWnd.push_back( nID );
    m_CtrlRect.push_back( dis );
    //dis.top = (float)((rtHwnd.bottom - rtHwnd.top) / ( rtCtrl.top - rtHwnd.top )) ;

}


void MfcCtrlResize::DelDlgItem( int nID )
{
    if( m_pMainWnd == NULL )
        return;

}

//void MfcCtrlResize::FitToBmp( int nID )
//{
//    CBitmap  BkBmp;
//    BkBmp.LoadBitmap( nID );
//    BITMAP hbmp;
//    BkBmp.GetBitmap( &hbmp );
//    Resizing( hbmp.bmWidth, hbmp.bmHeight );
//}

void MfcCtrlResize::Resizing()
{
    float w = 0;
    float h = 0;
    int menu_height = 0;

    //if( ( w == 0 ) && ( h == 0 ) )
    {
        CRect rtHwnd;
        m_pMainWnd->GetWindowRect( rtHwnd );
        w = rtHwnd.Width();
        h = rtHwnd.Height();

        int cy_border = GetSystemMetrics( SM_CYFRAME );
        int cy_caption = GetSystemMetrics( SM_CYCAPTION );
        POINT client_top_left = { 0, 0 };
        m_pMainWnd->ClientToScreen( &client_top_left );
        //menu_height = client_top_left.y - rtHwnd.top - cy_caption - cy_border;
        menu_height = cy_caption + cy_border;
    }

    for( uint_t wi = 0; wi < m_CtrlhWnd.size(); wi++ )
    {
        CWnd* pnWnd = m_pMainWnd->GetDlgItem( m_CtrlhWnd[ wi ] );
        if( pnWnd == NULL )
            return;

        float px = ( float )m_CtrlRect[ wi ].left / ( float )_MAX_MFC_RESING_RATE;
        float py = ( float )m_CtrlRect[ wi ].top / ( float )_MAX_MFC_RESING_RATE;
        float cx = ( float )m_CtrlRect[ wi ].right / ( float )_MAX_MFC_RESING_RATE;
        float cy = ( float )m_CtrlRect[ wi ].bottom / ( float )_MAX_MFC_RESING_RATE;


        CRect dis;
        dis.top = ( ( float )h * py ) - menu_height;
        dis.left = ( ( float )w * px );
        dis.right = ( ( float )w * cx );
        dis.bottom = ( ( float )h * cy );

        _tstring debug_output;
        _pf_tstrprintf( debug_output, _T( "MfcCtrlResize::Resizing [ id: %d, t:%d, l:%d, r:%d, d:%d]\n" ), m_CtrlhWnd[ wi ], dis.top, dis.left, dis.right, dis.bottom );
        OutputDebugString( debug_output.c_str() );

        pnWnd->SetWindowPos( NULL, dis.left, dis.top, dis.right, dis.bottom, 0 );
        //pnWnd->SetWindowPos( NULL, 100, 100, 200, 200, 0 );
        //::SetWindowPos( m_CtrlhWnd[wi]->m_hWnd, HWND_NOTOPMOST, dis.top, dis.left, dis.Width(), dis.Height(), SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE );
    }
}
