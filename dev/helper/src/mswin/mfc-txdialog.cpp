// Leading DialogDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "mfc-txdialog.h"
#include "win32_utility.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTXDialog dialog
#define DRAGGING    0x01    // dragging flag

BOOL                CTXDialog::s_bFullScreen = FALSE;
WINDOWPLACEMENT     CTXDialog::s_OldWndPlacement;


CTXDialog::CTXDialog( UINT nIDTemplate, UINT nIDResource, CWnd* pParentWnd )
    : CDialog( nIDTemplate, pParentWnd )
{
    m_pMemDC = NULL;
    m_pOldBitmap = NULL;
    m_pBitmapMem = NULL;
    m_nID = nIDTemplate;
    m_nBkResID = nIDResource;
    m_bDragable = true;
    m_dwFlags = 0;
}

CTXDialog::~CTXDialog()
{
    m_BkBmp.DeleteObject();
    if ( m_pMemDC )
        delete m_pMemDC;
    if ( m_pBitmapMem )
        delete m_pBitmapMem;
}

BOOL CTXDialog::Create( UINT nIDTemplate, CWnd* pParentWnd )
{
    if ( !::IsWindow( m_hWnd ) )
    {
        m_nID = nIDTemplate;
        return CDialog::Create( nIDTemplate, pParentWnd );
    }
    return FALSE;
}

void CTXDialog::CloseWindow()
{
    if ( ::IsWindow( m_hWnd ) )
    {
        CDialog::CloseWindow();
    }
}

BOOL CTXDialog::DestroyWindow()
{
    if ( ::IsWindow( m_hWnd ) )
    {
        return CDialog::DestroyWindow();
    }
    return TRUE;
}

void CTXDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    m_BkBmp.DeleteObject();
}

void CTXDialog::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CTXDialog, CDialog )
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_MESSAGE( WM_TRANSPARENT_BK, OnTransaprentBk )
END_MESSAGE_MAP()


// CTXDialog message handlers

BOOL CTXDialog::OnInitDialog( COLORREF MaskColor )
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    //SetIcon(m_hIcon, TRUE);            // Set big icon
    //SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here  
    //{
    //    CBitmap bm;
    //    bm.LoadBitmap( m_nBkResID );
    //
    //    HBITMAP hbmp = ( HBITMAP )bm.GetSafeHandle();  
    //
    //    CImage img;
    //    //img.LoadFromResource( AfxGetInstanceHandle(), m_nBkResID );
    //    img.Attach( hbmp );
    //    
    //    RECT rect;
    //    GetWindowRect( &rect );
    //
    //    //CImage img2;
    //    //CreateStretchImage( &img, &img2, rect.bottom - rect.top, rect.right - rect.left );
    //
    //    //hbmp = ( HBITMAP )img.operator HBITMAP();
    //    m_BkBmp.DeleteObject();
    //    m_BkBmp.Attach( hbmp );
    //}

    {
        CBitmap bm;
        bm.LoadBitmap( m_nBkResID );
        RECT rect;
        GetWindowRect( &rect );
               
        //CBitmap* dstBmp = CBitmap::FromHandle( ( HBITMAP )CopyImage( ( HBITMAP )bm, IMAGE_BITMAP, rect.right - rect.left, rect.bottom - rect.top, 0 ) );
        //CBitmapCopy( &m_BkBmp, dstBmp );
        CBitmapCopy( &m_BkBmp, &bm, rect.right - rect.left, rect.bottom - rect.top );
        //CBitmapCopy( &m_BkBmp, &bm );
    }
           
    GetClientRect( &m_DragRange );
    //m_DragRange.bottom = m_DragRange.top + 25;

    m_MaskColor = MaskColor;
    m_dwFlags = 0;

    //m_BkBmp.DeleteObject();
    //m_BkBmp.LoadBitmap( m_nBkResID );

    HRGN hRgn = CreateRgnFromFile( ( HBITMAP )m_BkBmp, m_MaskColor, m_dwWidth, m_dwHeight );
    SetWindowRgn( hRgn, TRUE );

    if ( !s_bFullScreen )
        GetWindowPlacement( &s_OldWndPlacement );

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTXDialog::OnPaint()
{
    OnMyPaint();
}

void CTXDialog::OnMyPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // device context for painting

    }
    else
    {
        CPaintDC dc( this ); // device context for painting

        CBitmap  *pOldBmp;
        CDC BmpDc;
        VERIFY( BmpDc.CreateCompatibleDC( &dc ) );
        pOldBmp = ( CBitmap * )BmpDc.SelectObject( &m_BkBmp );

        //CRect rt;
        //GetWindowRect( &rt );
        dc.BitBlt( 0, 0, m_dwWidth, m_dwHeight, &BmpDc, 0, 0, SRCCOPY );
        BmpDc.SelectObject( pOldBmp );
    }
}


#define DRAGGING    0x01    // dragging flag

void CTXDialog::OnLButtonDown( UINT nFlags, CPoint point )
{
    // TODO: Add your message handler code here and/or call default
    // 全螢幕也不給拖
    if ( ( m_bDragable ) && ( !s_bFullScreen ) )
    {
        if ( ( point.x >= m_DragRange.left ) &&
             ( point.x <= m_DragRange.right ) &&
             ( point.y >= m_DragRange.top ) &&
             ( point.y <= m_DragRange.bottom ) )
        {
            if ( !( m_dwFlags&DRAGGING ) )
            {
                m_pntMouse = point;
                m_dwFlags |= DRAGGING;
                SetCapture();
            }
        }
    }
    CDialog::OnLButtonDown( nFlags, point );
}

void CTXDialog::OnLButtonUp( UINT nFlags, CPoint point )
{
    // TODO: Add your message handler code here and/or call default
    if ( m_dwFlags  &DRAGGING )
    {
        m_dwFlags &= ~DRAGGING;
        ReleaseCapture();
    }
    CDialog::OnLButtonUp( nFlags, point );
}

void CTXDialog::OnMouseMove( UINT nFlags, CPoint point )
{
    // TODO: Add your message handler code here and/or call default
    if ( m_dwFlags  &DRAGGING )
    {
        RECT rect;
        GetWindowRect( &rect );

        rect.left += point.x - m_pntMouse.x;
        rect.top += point.y - m_pntMouse.y;

        SetWindowPos( NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
    }
    CDialog::OnMouseMove( nFlags, point );
}

BOOL CTXDialog::OnEraseBkgnd( CDC* pDC )
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    //return CDialog::OnEraseBkgnd(pDC);
}

LRESULT CTXDialog::OnTransaprentBk( WPARAM wParam, LPARAM lParam )
{
    HDC hdc = ( HDC )wParam;
    HWND hwnd = ( HWND )lParam;
    CTXHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), ( WPARAM )hdc, ( LPARAM )hwnd );
    return TRUE;
}

void CTXDialog::BuildBkDC()
{
    if ( m_nBkResID == 0 )
        return;

    CBitmap bmpbk;
    bmpbk.LoadBitmap( m_nBkResID );
    BITMAP bmpInfo;
    bmpbk.GetBitmap( &bmpInfo );

    CDC* pWndDC = GetDC();
    CDC memdc;
    memdc.CreateCompatibleDC( pWndDC );
    CBitmap* pOld = memdc.SelectObject( &bmpbk );


    CRect rtClient;
    GetClientRect( &rtClient );

    if ( m_pMemDC )
    {
        m_pMemDC->SelectObject( m_pOldBitmap );
        delete m_pBitmapMem;
        m_pBitmapMem = NULL;
        delete m_pMemDC;
        m_pMemDC = NULL;
    }

    if ( m_pMemDC == NULL )
    {
        m_pMemDC = new CDC;
        m_pMemDC->CreateCompatibleDC( pWndDC );
        m_pBitmapMem = new CBitmap;
        m_pBitmapMem->CreateCompatibleBitmap( pWndDC, rtClient.Width(), rtClient.Height() );
        m_pOldBitmap = ( CBitmap* )m_pMemDC->SelectObject( m_pBitmapMem );
    }

    m_pMemDC->StretchBlt( 0, 0, rtClient.Width(), rtClient.Height(), &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );
    memdc.SelectObject( pOld );
    ReleaseDC( pWndDC );

    //when the parent dialog's background is rebuild, notify the child which has an transparent tag.
    CTXHelper::NotifyTransparentChild( GetSafeHwnd() );
}

void CTXDialog::OnSize( UINT nType, int cx, int cy )
{
    CDialog::OnSize( nType, cx, cy );

    BuildBkDC();
    Invalidate( FALSE );
    // TODO: Add your message handler code here
}

INT CTXDialog::ModelUpdate()
{
    MSG *pMsg = AfxGetCurrentMessage();
    do
    {
        // pump message, but quit on WM_QUIT
        if ( !AfxPumpMessage() )
        {
            AfxPostQuitMessage( 0 );
            return -1;
        }

        // reset "no idle" state after pumping "normal" message
        if ( AfxIsIdleMessage( pMsg ) )
        {
            //bIdle = TRUE;
            //lIdleCount = 0;
        }

    } while ( ::PeekMessage( pMsg, NULL, NULL, NULL, PM_NOREMOVE ) );

    return 1;
}

void CTXDialog::SetDragRange( RECT rt )
{
    m_DragRange = rt;
}

void CTXDialog::SetDragable( BOOL bd )
{
    m_bDragable = bd;
}

BOOL CTXDialog::GetFullScreen( void )
{
    return s_bFullScreen;
}

void CTXDialog::SetFullScreen( BOOL fs )
{
    //2、在相应的响应函数中加如下代码：
    if ( s_bFullScreen == fs )
        return;

    s_bFullScreen = fs;
    if ( !s_bFullScreen )
    {
        if ( ChangeDisplaySettings( NULL, 0 ) != DISP_CHANGE_SUCCESSFUL )
        {
            // If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.

        }

        SetWindowPlacement( &s_OldWndPlacement );
    }
    else
    {
        HWND hwnd = m_hWnd;

        RECT rt;
        ::GetClientRect( hwnd, &rt );


        DEVMODE dmScreenSettings;                             // Device Mode
        memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) ); // Makes Sure Memory's Cleared

        dmScreenSettings.dmSize = sizeof( dmScreenSettings );    // Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth = rt.right - rt.left;     // Selected Screen Width
        dmScreenSettings.dmPelsHeight = rt.bottom - rt.top;     // Selected Screen Height
        dmScreenSettings.dmBitsPerPel = 16;                   // Selected Bits Per Pixel
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if ( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
        {
            // If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.

        }

        GetWindowPlacement( &s_OldWndPlacement ); //取得原始窗口位置
        int nFullWidth = rt.right - rt.left;
        int nFullHeight = rt.bottom - rt.top;

        RECT WindowRect, ClientRect;
        GetWindowRect( &WindowRect );
        GetWindowRect( &ClientRect );

        RECT FullScreenRect;
        FullScreenRect.left = WindowRect.left - ClientRect.left;
        FullScreenRect.top = WindowRect.top - ClientRect.top;
        FullScreenRect.right = WindowRect.right - ClientRect.right + nFullWidth -
            FullScreenRect.left;
        FullScreenRect.bottom = WindowRect.bottom - ClientRect.bottom + nFullHeight -
            FullScreenRect.top;

        WINDOWPLACEMENT wndpl;
        wndpl.length = sizeof( WINDOWPLACEMENT );
        wndpl.flags = 0;
        wndpl.showCmd = SW_SHOWNORMAL;
        wndpl.rcNormalPosition = FullScreenRect;
        SetWindowPlacement( &wndpl );

        ::SetWindowPos( m_hWnd, HWND_TOPMOST, 0, 0, nFullWidth, nFullHeight, SWP_NOMOVE );
    }
     InvalidateRect( NULL, FALSE );

}

void CTXDialog::ForceWindowToForeground()
{
    ::ForceWindowToForeground( m_hWnd );
}