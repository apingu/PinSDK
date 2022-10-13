// StaticEx.cpp : implementation file
//

#include "stdafx.h"
//#include "TransparentControl.h"
#include "mfc-txstatic.h"
//#include ".\staticex.h"


// CTXStatic

IMPLEMENT_DYNAMIC(CTXStatic, CStatic)
CTXStatic::CTXStatic()
{
    m_nResBGID  = 0;
    m_nAlphaVal = 255;
}

CTXStatic::~CTXStatic()
{
}


BEGIN_MESSAGE_MAP(CTXStatic, CStatic)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CTXStatic message handlers


void CTXStatic::SetWindowText(LPCTSTR lpszString, COLORREF col)
{
    m_TextColor = col;
    CStatic::SetWindowText(lpszString);
    InvalidateRect(NULL, FALSE);
}


void CTXStatic::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    CStatic::PreSubclassWindow();
    m_objHelper.Install( GetSafeHwnd());

}

void CTXStatic::OnPaint()
//void CTXStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    //m_objHelper.Install( GetSafeHwnd());
    //CStatic::OnPaint();
    //return;

    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CStatic::OnPaint() for painting messages
    //m_objHelper.TransparentBk( dc.GetSafeHdc(), GetSafeHwnd());


    if(m_nResBGID !=0)
    {
        RECT rt;
        GetClientRect(&rt);

        CDC memdc;
        memdc.CreateCompatibleDC( &dc);
        CBitmap membmp;
        membmp.CreateCompatibleBitmap( &dc, rt.right-rt.left, rt.bottom-rt.top);
        CBitmap* bmpOld = ( CBitmap*) memdc.SelectObject( &membmp);

        m_objTransparent.TransparentBk( memdc.GetSafeHdc(), GetSafeHwnd());
        CFont* pOldFont = ( CFont*)memdc.SelectObject( GetFont());


        BITMAP bmpInfo;
        m_bitmapBG.GetBitmap( &bmpInfo);

        if(m_nAlphaVal==255)
        {
            CDC dctemp;
            dctemp.CreateCompatibleDC( &dc);
            dctemp.SelectObject(m_bitmapBG);

            memdc.TransparentBlt( 0,0, rt.right, rt.bottom, &dctemp, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor);
        }
        else
        {
            CDC memdc;
            memdc.CreateCompatibleDC( &dc);
            memdc.SelectObject(m_bitmapBG);
            BLENDFUNCTION bf = {0};
            bf.BlendOp = AC_SRC_OVER;
            bf.SourceConstantAlpha = m_nAlphaVal;

            dc.AlphaBlend( 0,0, rt.right, rt.bottom, &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf);
        }
    }

    
    CString strText;
    GetWindowText( strText);

    CRect rtText;
    GetClientRect( &rtText);
    dc.SetBkMode(TRANSPARENT);

    dc.SetTextColor(m_TextColor);
    
    dc.SelectObject( GetFont());
    dc.DrawText( strText, &rtText, DT_LEFT|DT_SINGLELINE);
    

    //InvalidateRect( NULL, FALSE);
}


BOOL CTXStatic::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    //return CStatic::OnEraseBkgnd(pDC);
}

void CTXStatic::SetResID( DWORD nResBGID, COLORREF MaskColor )
{
    m_nResBGID = nResBGID;
    m_MaskColor = MaskColor;

    m_bitmapBG.DeleteObject();

    if(m_nResBGID !=0)
    {
        BOOL ret0 = m_bitmapBG.LoadBitmap(m_nResBGID);
    }

}

void CTXStatic::SetAlpha ( DWORD AlphaVal )
{
    m_nAlphaVal = AlphaVal;
}
