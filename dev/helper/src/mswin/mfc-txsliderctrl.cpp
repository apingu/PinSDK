// SliderCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "mfc-txhelper.h"
#include "mfc-txsliderctrl.h"
//#include ".\sliderctrlex.h"


// CTXSliderCtrl

IMPLEMENT_DYNAMIC(CTXSliderCtrl, CSliderCtrl)
CTXSliderCtrl::CTXSliderCtrl()
{
    m_nDragID=0;
    m_nBgID=0;
    m_MaskColor=RGB(0, 0, 0);
}

CTXSliderCtrl::~CTXSliderCtrl()
{
    m_bmpBg.DeleteObject();
    m_bmpDrag.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTXSliderCtrl, CSliderCtrl)
    ON_WM_DESTROY()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    ON_MESSAGE( WM_NOTIFY_TRANSPARENT, OnNotifyTransparent)
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CTXSliderCtrl message handlers

// void CTXSliderCtrl::OnDestroy()
// {
//     CDialog::OnDestroy();
// 
//     // TODO: Add your message handler code here
//     m_bmpBg.DeleteObject();
//     m_bmpDrag.DeleteObject();
// }


void CTXSliderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW lpNMCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    CDC* dcPaint = CDC::FromHandle(lpNMCustomDraw->hdc) ;

    CRect rcPaint( lpNMCustomDraw->rc );

    switch( lpNMCustomDraw->dwDrawStage ) 
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:  // Before an item is drawn. This is where we perform our item-specific custom drawing
        if( lpNMCustomDraw->dwItemSpec == TBCD_THUMB )
        {
            if( OnSliderDrawThumb( dcPaint, rcPaint,lpNMCustomDraw->uItemState) )
                *pResult = CDRF_SKIPDEFAULT;
        } 
        else if( lpNMCustomDraw->dwItemSpec == TBCD_CHANNEL )
        {
            CRect rcClient;
            GetClientRect( &rcClient);

            if( OnSliderDrawChannel( dcPaint,rcClient,lpNMCustomDraw->uItemState))
                *pResult = CDRF_SKIPDEFAULT;
        }
        else if( lpNMCustomDraw->dwItemSpec == TBCD_TICS )
        {
            CRect rcClient;
            GetClientRect( &rcClient);
            if( OnSliderDrawTics( dcPaint,rcClient, lpNMCustomDraw->uItemState))
                *pResult = CDRF_SKIPDEFAULT;
        }
        else
        {
            _ASSERT( FALSE );
        }

        break;
    } // switc

}



BOOL CTXSliderCtrl::OnSliderDrawThumb( CDC* pDC,  CRect &rect, UINT nState)
{

    if ( nState == CDIS_SELECTED)
    {
        //DrawDisplayItem( hdc, rect, STATE_DOWN);
        CDC memdc;
        memdc.CreateCompatibleDC( pDC);
        memdc.SelectObject(m_bmpDrag);
        pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, SRCCOPY);
    }
    else if ( nState == CDIS_HOT)
    {
        //you may have another hot bitmap to draw the thumb. I doesn't have another picture,so use the same picture.
        CDC memdc;
        memdc.CreateCompatibleDC( pDC);
        memdc.SelectObject(m_bmpDrag);
        pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, SRCCOPY);
    }
    else 
    {
        CDC memdc;
        memdc.CreateCompatibleDC( pDC);
        memdc.SelectObject(m_bmpDrag);
        pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, SRCCOPY);
    }

    return TRUE;

}


BOOL CTXSliderCtrl::OnSliderDrawChannel( CDC* pDC,  CRect &rect, UINT nState)
{
    if ( m_objTrans.IsValid() )
    {
        m_objTrans.TransparentBk( pDC->GetSafeHdc(),  GetSafeHwnd());
    }

    CDC memdc;
    memdc.CreateCompatibleDC( pDC);
    memdc.SelectObject(m_bmpBg);
    BITMAP bmpInfo;
    if(m_bmpBg.GetBitmap( &bmpInfo) !=0)
    {
        //pDC->BitBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, SRCCOPY);
        pDC->TransparentBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, m_MaskColor);
    }

    return TRUE;
}

void CTXSliderCtrl::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    m_nDragID=0;
    m_nBgID=0;
    m_MaskColor=RGB(0, 0, 0);

    CSliderCtrl::PreSubclassWindow();
    m_objTrans.Install( GetSafeHwnd());
}

BOOL CTXSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    return TRUE;
    //return CSliderCtrl::OnEraseBkgnd(pDC);
}

LRESULT CTXSliderCtrl::OnNotifyTransparent( WPARAM wParam, LPARAM lParam) 
{
    if ( ::IsWindowEnabled( GetSafeHwnd()))
    {
        ::EnableWindow( GetSafeHwnd(),FALSE); 
        ::EnableWindow( GetSafeHwnd(),TRUE);
    }
    else
    {
        ::EnableWindow( GetSafeHwnd(),TRUE); 
        ::EnableWindow( GetSafeHwnd(),FALSE);
    }

    //This operation is for the repaint of slider control, because Invalidate cann't bring the NM_CUSTOMDRAW message.
    //M..., this may not the best method to solve the problem.
    return TRUE;
}

void CTXSliderCtrl::SetStateResID( DWORD nDragID,  DWORD nBgID, COLORREF MaskColor )
{
    m_nBgID = nBgID;
    m_nDragID = nDragID;
    m_MaskColor = MaskColor;

    m_bmpBg.DeleteObject();
    if(m_nBgID !=0)
        m_bmpBg.LoadBitmap( m_nBgID);

    m_bmpDrag.DeleteObject();
    if(m_nDragID !=0)
        m_bmpDrag.LoadBitmap( m_nDragID);
}

