// StaticEx.cpp : implementation file
//

#include "stdafx.h"
//#include "TransparentControl.h"
#include "mfc-txmarquee.h"
//#include ".\staticex.h"


// CTXMarquee

IMPLEMENT_DYNAMIC(CTXMarquee, CStatic)
CTXMarquee::CTXMarquee()
{
    m_nResBGID  = 0;
    m_nAlphaVal = 255;
    m_nTimer    = 0;
    m_TextPos.x = 0;
    m_TextPos.y = 0;
    m_MoveDest  = -5;
    m_EndCnt    = 0;
}

CTXMarquee::~CTXMarquee()
{
}


BEGIN_MESSAGE_MAP(CTXMarquee, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_DESTROY()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CTXMarquee message handlers

void CTXMarquee::OnDestroy()
{
    CStatic::OnDestroy();

    // TODO: Add your message handler code here
    if(m_nTimer !=0)
        KillTimer(m_nTimer);
    m_bUpdate=false;
}

void CTXMarquee::SetWindowText(LPCTSTR lpszString, COLORREF col)
{
    m_EndCnt = 0;
    m_TextColor = col;
    CStatic::SetWindowText(lpszString);
    InvalidateRect(NULL, FALSE);
}


void CTXMarquee::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    CStatic::PreSubclassWindow();
    m_objHelper.Install( GetSafeHwnd());

    m_nTimer = SetTimer( 1, 0, 0 );
    m_nlastTime = ::GetTickCount();

    m_nElapse = 200;
    m_bUpdate = true;

}

void CTXMarquee::OnPaint()
//void CTXMarquee::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    //m_objHelper.Install( GetSafeHwnd());
    //CStatic::OnPaint();
    //return;

    CPaintDC dc(this); // device context for painting
    CRect rtText;
    GetClientRect( &rtText);

    // TODO: Add your message handler code here
    // Do not call CStatic::OnPaint() for painting messages
    //m_objHelper.TransparentBk( dc.GetSafeHdc(), GetSafeHwnd());

    CDC memdc;
    memdc.CreateCompatibleDC( &dc);
    CBitmap membmp;
    membmp.CreateCompatibleBitmap( &dc, rtText.Width(), rtText.Height());
    CBitmap* bmpOld = ( CBitmap*) memdc.SelectObject( &membmp);

    m_objTransparent.TransparentBk( memdc.GetSafeHdc(), GetSafeHwnd());
    CFont* pOldFont = ( CFont*)memdc.SelectObject( GetFont() );


    CString strText;
    GetWindowText( strText);
    memdc.SetBkMode(TRANSPARENT);
    memdc.SetTextColor(m_TextColor);

    memdc.TextOut(m_TextPos.x, m_TextPos.y, strText);

    dc.BitBlt( 0,0, rtText.Width(), rtText.Height(), &memdc, 0,0, SRCCOPY);
    memdc.SelectObject( bmpOld);
    memdc.SelectObject( pOldFont);
}


BOOL CTXMarquee::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    //return CStatic::OnEraseBkgnd(pDC);
}

void CTXMarquee::SetResID( DWORD nResBGID, COLORREF MaskColor )
{
    m_nResBGID = nResBGID;
    m_MaskColor = MaskColor;

    m_bitmapBG.DeleteObject();

    if(m_nResBGID !=0)
    {
        BOOL ret0 = m_bitmapBG.LoadBitmap(m_nResBGID);
    }

}

void CTXMarquee::SetAlpha ( DWORD AlphaVal )
{
    m_nAlphaVal = AlphaVal;
}

void CTXMarquee::Update()
{
    if(!m_bUpdate)
        return;

    DWORD currtime = ::GetTickCount();
    if( (currtime-m_nlastTime)>=m_nElapse )
    {
        m_TextPos.x += m_MoveDest;
        m_nlastTime=currtime;


        CFont* pFont =GetFont();

        LOGFONT lf;
        pFont->GetLogFont(&lf);

        CString strText;
        GetWindowText( strText);

        int d=(0-(strText.GetLength()*10));
        if(m_TextPos.x<=d)
        {
            CRect rtText;
            GetClientRect( &rtText);
            m_TextPos.x=rtText.right;
            m_EndCnt++;
        }


        InvalidateRect(NULL, FALSE);
    }

}

void CTXMarquee::OnTimer(UINT_PTR nIDEvent)
{
    CStatic::OnTimer( nIDEvent );
}
