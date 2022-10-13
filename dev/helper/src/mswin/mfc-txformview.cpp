/////////////////////////////////////////////////////////////////////////////
//
// Author:    if (it works)
//                Author = _T("Patty You");
//            else
//                Author = _T("IUnknown");
// Date:    08/26/98
/////////////////////////////////////////////////////////////////////////////
//
// ColorFormView.cpp : implementation file
//

#include "stdafx.h"
#include "mfc-txformview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MfcTXFormView

IMPLEMENT_DYNAMIC(MfcTXFormView, CFormView)

MfcTXFormView::MfcTXFormView(LPCTSTR lpszTemplateName)
:CFormView(lpszTemplateName)
{
    m_crBackground = GetSysColor(COLOR_3DFACE);
    m_wndbkBrush.CreateSolidBrush(m_crBackground); 
}

MfcTXFormView::MfcTXFormView(UINT nIDTemplate)
:CFormView(nIDTemplate)
{
    m_crBackground = GetSysColor(COLOR_3DFACE);
    m_wndbkBrush.CreateSolidBrush(m_crBackground); 
}

MfcTXFormView::~MfcTXFormView()
{
    if(m_wndbkBrush.GetSafeHandle())
        m_wndbkBrush.DeleteObject();
}


BEGIN_MESSAGE_MAP(MfcTXFormView, CFormView)
    //{{AFX_MSG_MAP(MfcTXFormView)
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MfcTXFormView drawing

void MfcTXFormView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
}

void MfcTXFormView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// MfcTXFormView diagnostics

#ifdef _DEBUG
void MfcTXFormView::AssertValid() const
{
    CFormView::AssertValid();
}

void MfcTXFormView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MfcTXFormView message handlers
void MfcTXFormView::SetBackgroundColor(COLORREF crBackground)
{
    m_crBackground = crBackground;
    
    if(m_wndbkBrush.GetSafeHandle())
        m_wndbkBrush.DeleteObject();

    m_wndbkBrush.CreateSolidBrush(m_crBackground); 
}

BOOL MfcTXFormView::OnEraseBkgnd(CDC* pDC) 
{
    CFormView::OnEraseBkgnd(pDC);

    CRect rect;
    GetClientRect(rect);

    pDC->FillRect(&rect, &m_wndbkBrush);

    return TRUE;
}
