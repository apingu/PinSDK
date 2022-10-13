// ButtonEx1.cpp : implementation file
//

#include "stdafx.h"
#include "mfc-txbutton.h"
#include "win32_utility.h"


// CTXButton

IMPLEMENT_DYNAMIC( CTXButton, CButton )
CTXButton::CTXButton()
{
    m_bTracking = FALSE;
    m_nAlphaVal = 255;
    m_nNorlID = 0;
    m_nOverID = 0;
    m_nDownID = 0;
    m_nDisbID = 0;
    m_NorlTextColor = RGB( 0, 0, 0 );
    m_OverTextColor = RGB( 0, 0, 0 );
    m_DownTextColor = RGB( 0, 0, 0 );
    m_DisbTextColor = RGB( 0, 0, 0 );
    m_bCheckStyle = false;
    m_bChecked = false;
    m_TextAlign = TA_LEFT;
    m_FontSize = 100;
    m_FontType = _T( "" );
    //m_FontType = _T( "Arial" );
}

CTXButton::~CTXButton()
{
    m_bitmapNor.DeleteObject();
    m_bitmapOver.DeleteObject();
    m_bitmapDown.DeleteObject();
    m_bitmapDisb.DeleteObject();
}


BEGIN_MESSAGE_MAP( CTXButton, CButton )
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_DESTROY()
    ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )
END_MESSAGE_MAP()


// CTXButton message handlers

// void CTXButton::OnDestroy()
// {
//     CDialog::OnDestroy();
// 
//     // TODO: Add your message handler code here
//     m_bitmapNor.DeleteObject();
//     m_bitmapOver.DeleteObject();
//     m_bitmapDown.DeleteObject();
//     m_bitmapDisb.DeleteObject();
// }

void CTXButton::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{

    // TODO:  Add your code to draw the specified item

    CDC dc;
    dc.Attach( lpDrawItemStruct->hDC );
    CRect rtBtn( lpDrawItemStruct->rcItem );

    CDC memdc;
    memdc.CreateCompatibleDC( &dc );
    CBitmap membmp;
    membmp.CreateCompatibleBitmap( &dc, rtBtn.Width(), rtBtn.Height() );
    CBitmap* bmpOld = (CBitmap*)memdc.SelectObject( &membmp );
    m_objTransparent.TransparentBk( memdc.GetSafeHdc(), GetSafeHwnd() );

    COLORREF textcolor = m_NorlTextColor;

    if ( !m_bCheckStyle )
    {
        if ( m_bTracking )
        {
            textcolor = m_OverTextColor;

            if ( m_nOverID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapOver.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapOver );

                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapOver );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }

        if ( lpDrawItemStruct->itemState  &ODS_SELECTED )
        {
            textcolor = m_DownTextColor;

            if ( m_nDownID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapDown.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapDown );
                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapDown );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }
        else if ( lpDrawItemStruct->itemState  &ODS_DISABLED )
        {
            textcolor = m_DisbTextColor;

            if ( m_nDisbID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapDisb.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapDisb );
                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapDisb );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }
        else if ( !m_bTracking )
        {
            textcolor = m_NorlTextColor;

            if ( m_nNorlID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapNor.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapNor );
                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapNor );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }
    }
    else
    {
        if ( m_bChecked )
        {
            textcolor = m_DownTextColor;

            if ( m_nDownID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapDown.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapDown );
                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapDown );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }
        else
        {
            textcolor = m_NorlTextColor;

            if ( m_nNorlID != 0 )
            {
                BITMAP bmpInfo;
                m_bitmapNor.GetBitmap( &bmpInfo );

                if ( m_nAlphaVal == 255 )
                {
                    CDC dctemp;
                    dctemp.CreateCompatibleDC( &dc );
                    dctemp.SelectObject( m_bitmapNor );
                    memdc.TransparentBlt( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &dctemp, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor );
                }
                else
                {
                    CDC memdc;
                    memdc.CreateCompatibleDC( &dc );
                    memdc.SelectObject( m_bitmapNor );
                    BLENDFUNCTION bf = { 0 };
                    bf.BlendOp = AC_SRC_OVER;
                    bf.SourceConstantAlpha = m_nAlphaVal;
                    dc.AlphaBlend( 0, 0, lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.bottom, &memdc, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
                }
            }
        }
    }

    CString strText;
    GetWindowText( strText );
    CRect rtText;
    GetClientRect( &rtText );
    rtText.top += m_TextPosition.y;
    rtText.left += m_TextPosition.x;
    rtText.right += m_TextPosition.x;
    rtText.bottom += m_TextPosition.y;

    //CFont* pOldFont00 = GetFont();
    CFont* pOldFont = (CFont*)memdc.SelectObject( GetFont() );
    CFont font;
    //  用系統的
    if ( m_FontType != "" )
    {
        BOOL b = font.CreatePointFont( m_FontSize, (LPCTSTR)m_FontType );
        memdc.SelectObject( &font );
    }
    else
    {
        //font.CreatePointFont( m_FontSize, NULL );
        //memdc.SelectObject( &font );
    }

    memdc.SetBkMode( TRANSPARENT );
    memdc.SetTextColor( textcolor );

    //memdc.SetTextAlign( m_TextAlign );
    //switch(memdc.GetTextAlign())
    switch ( m_TextAlign )
    {
    case TA_LEFT:
        memdc.DrawText( strText, &rtText, DT_LEFT | DT_SINGLELINE );
        break;
    case TA_RIGHT:
        memdc.DrawText( strText, &rtText, DT_RIGHT | DT_SINGLELINE );
        break;
    case TA_CENTER:
        memdc.DrawText( strText, &rtText, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
        break;
    }

    dc.BitBlt( 0, 0, rtBtn.Width(), rtBtn.Height(), &memdc, 0, 0, SRCCOPY );
    
    memdc.SelectObject( bmpOld );
    //memdc.SelectObject( pOldFont );

    //font.DeleteObject();
    memdc.DeleteDC();
}

void CTXButton::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    m_bTracking = FALSE;
    m_nAlphaVal = 255;
    m_nNorlID = 0;
    m_nOverID = 0;
    m_nDownID = 0;
    m_nDisbID = 0;
    m_NorlTextColor = RGB( 0, 0, 0 );
    m_OverTextColor = RGB( 0, 0, 0 );
    m_DownTextColor = RGB( 0, 0, 0 );
    m_DisbTextColor = RGB( 0, 0, 0 );
    m_bCheckStyle = false;
    m_bChecked = false;

    CButton::PreSubclassWindow();
    m_objTransparent.Install( GetSafeHwnd() );
}

BOOL CTXButton::OnEraseBkgnd( CDC* pDC )
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    //return CButton::OnEraseBkgnd(pDC);
}

void CTXButton::OnMouseMove( UINT nFlags, CPoint point )
{
    // TODO: Add your message handler code here and/or call default

    CButton::OnMouseMove( nFlags, point );

    if ( !m_bTracking )
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof( tme );
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        m_bTracking = _TrackMouseEvent( &tme );
        Invalidate( FALSE );
    }
}

LRESULT CTXButton::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
    m_bTracking = FALSE;
    Invalidate( FALSE );
    return TRUE;
}

void CTXButton::SetStateResID( DWORD nNorlID, DWORD nOverID, DWORD nDownID, DWORD nDisbID, COLORREF MaskColor )
{
    m_bTracking = FALSE;
    m_nNorlID = 0;
    m_nOverID = 0;
    m_nDownID = 0;
    m_nDisbID = 0;
    m_MaskColor = MaskColor;

    m_bitmapNor.DeleteObject();
    if ( nNorlID != 0 )
    {
        if ( m_bitmapNor.LoadBitmap( nNorlID ) )
            m_nNorlID = nNorlID;
    }

    m_bitmapOver.DeleteObject();
    if ( nOverID != 0 )
    {
        if ( m_bitmapOver.LoadBitmap( nOverID ) )
            m_nOverID = nOverID;
    }

    m_bitmapDown.DeleteObject();
    if ( nDownID != 0 )
    {
        if ( m_bitmapDown.LoadBitmap( nDownID ) )
            m_nDownID = nDownID;
    }

    m_bitmapDisb.DeleteObject();
    if ( nDisbID != 0 )
    {
        if ( m_bitmapDisb.LoadBitmap( nDisbID ) )
            m_nDisbID = nDisbID;
    }
    InvalidateRect( NULL, FALSE );
}

void CTXButton::SetStateResID( CBitmap &NorlBmp, CBitmap &OverBmp, CBitmap &DownBmp, CBitmap &DisbBmp, COLORREF MaskColor )
{
    m_bTracking = FALSE;
    m_nNorlID = 0;
    m_nOverID = 0;
    m_nDownID = 0;
    m_nDisbID = 0;
    m_MaskColor = MaskColor;

    m_bitmapNor.DeleteObject();
    if ( m_bitmapNor.Attach( HBitmapCopy( NorlBmp ) ) )
        m_nNorlID = 1;

    m_bitmapOver.DeleteObject();
    if ( m_bitmapOver.Attach( HBitmapCopy( OverBmp ) ) )
        m_nOverID = 1;

    m_bitmapDown.DeleteObject();
    if ( m_bitmapDown.Attach( HBitmapCopy( DownBmp ) ) )
        m_nDownID = 1;

    m_bitmapDisb.DeleteObject();
    if ( m_bitmapDisb.Attach( HBitmapCopy( DisbBmp ) ) )
        m_nDisbID = 1;

    InvalidateRect( NULL, FALSE );
}

void CTXButton::SetStateTextColor( COLORREF NorlColor, COLORREF OverColor, COLORREF DownColor, COLORREF DisbColor )
{
    m_bTracking = FALSE;
    m_NorlTextColor = NorlColor;
    m_OverTextColor = OverColor;
    m_DownTextColor = DownColor;
    m_DisbTextColor = DisbColor;
    InvalidateRect( NULL, FALSE );
}

void CTXButton::SetTextPosition( CPoint point, UINT align )
{
    m_TextPosition = point;
    m_TextAlign = align;
}

void CTXButton::SetFont( int size, const TCHAR* type )
{
    m_FontSize = size;
    m_FontType = type;
}

void CTXButton::SetAlpha( DWORD AlphaVal )
{
    m_nAlphaVal = AlphaVal;
    InvalidateRect( NULL, FALSE );
}

void CTXButton::SetCheckStyle( bool f )
{
    m_bCheckStyle = f;
    InvalidateRect( NULL, FALSE );
}

BOOL CTXButton::GetCheck()
{
    return m_bChecked;
}

void CTXButton::SetCheck( BOOL c )
{
    m_bChecked = c;
    InvalidateRect( NULL, FALSE );
}

BOOL CTXButton::GetOverTracking()
{
    return m_bTracking;
}