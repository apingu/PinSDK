// GfxInEdit.cpp : implementation file
//

#include "stdafx.h"
#include "mfc-input.h"


#ifdef _DEBUGS
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#endif


/////////////////////////////////////////////////////////////////////////////
// mfcCInEdit

mfcCInEdit::mfcCInEdit(int iItem,int iSubItem,CString sInitText):m_sInitText(sInitText)
{
    m_iItem = iItem;
    m_iSubItem = iSubItem;
    m_bESC = FALSE;
}

mfcCInEdit::~mfcCInEdit()
{
}


BEGIN_MESSAGE_MAP(mfcCInEdit, CEdit)
    // {{AFX_MSG_MAP(mfcCInEdit)
    ON_WM_CREATE()
    ON_WM_KILLFOCUS()
    ON_WM_NCDESTROY()
    ON_WM_CHAR()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// mfcCInEdit message handlers

BOOL mfcCInEdit::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_RETURN ||
            pMsg->wParam == VK_DELETE ||
            pMsg->wParam == VK_ESCAPE ||
            GetKeyState(VK_CONTROL))
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            return TRUE;        // DO NOT process further
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

int mfcCInEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CEdit::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Set the proper font
    CFont* font = GetParent()->GetFont();

    SetFont(font);

    SetWindowText(m_sInitText);
    SetFocus();
    //SetSel(0, -1);

    return 0;
}

void mfcCInEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    CString str;

    GetWindowText(str);

    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_iItem;
    dispinfo.item.iSubItem = m_iSubItem;
    dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR) str);
    dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();

    GetParent()->GetParent()->SendMessage(WM_NOTIFY,
                                          GetParent()->GetDlgCtrlID(),
                                          (LPARAM)  &dispinfo);

    DestroyWindow();
}

void mfcCInEdit::OnNcDestroy()
{
    CEdit::OnNcDestroy();

    delete this;
}

void mfcCInEdit::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
    if(nChar == VK_ESCAPE || nChar == VK_RETURN)
    {
        if(nChar == VK_ESCAPE)
            m_bESC = TRUE;

        GetParent()->SetFocus();

        return;
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);

    // Resize edit control if needed

    // Get text extent
    CString str;

    GetWindowText(str);

    CWindowDC dc    (this);
    CFont* pFont    = GetParent()->GetFont();
    CFont* pFontDC  = dc.SelectObject(pFont);
    CSize size      = dc.GetTextExtent(str);

    dc.SelectObject(pFontDC);

    size.cx += 5;               // add some extra buffer

    // Get client rect
    CRect rect, parentrect;

    GetClientRect(&rect);
    GetParent()->GetClientRect(&parentrect);

    // Transform rect to parent coordinates
    ClientToScreen(&rect);
    GetParent()->ScreenToClient(&rect);

    // Check whether control needs to be resized
    // and whether there is space to grow
    if(size.cx > rect.Width())
    {
        if(size.cx + rect.left < parentrect.right)
            rect.right = rect.left + size.cx;
        else
            rect.right = parentrect.right;

        MoveWindow(&rect);
    }
}


/////////////////////////////////////////////////////////////////////////////
// mfcCInCombo

mfcCInCombo::mfcCInCombo(int iItem,
                          int iSubItem,
                          CStringList* plstItems,
                          int nSel)
{
    m_iItem = iItem;
    m_iSubItem = iSubItem;

    m_lstItems.AddTail(plstItems);
    m_nSel = nSel;
    m_bESC = FALSE;
}

mfcCInCombo::~mfcCInCombo()
{
}


BEGIN_MESSAGE_MAP(mfcCInCombo, CComboBox)
    //{{AFX_MSG_MAP(mfcCInCombo)
    ON_WM_CREATE()
    ON_WM_KILLFOCUS()
    ON_WM_NCDESTROY()
    ON_WM_CHAR()
    ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// mfcCInCombo message handlers

BOOL mfcCInCombo::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            return TRUE;        // DO NOT process further
        }
    }

    return CComboBox::PreTranslateMessage(pMsg);
}

int mfcCInCombo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CComboBox::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Set the proper font
    CFont* font = GetParent()->GetFont();

    SetFont(font);

    for(POSITION pos = m_lstItems.GetHeadPosition();pos != NULL;)
    {
        AddString((LPCTSTR) (m_lstItems.GetNext(pos)));
    }

    SetFocus();
    SetCurSel(m_nSel); 

    return 0;
}

void mfcCInCombo::OnKillFocus(CWnd* pNewWnd)
{
    CComboBox::OnKillFocus(pNewWnd);

    CString str;

    GetWindowText(str);

    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_iItem;
    dispinfo.item.iSubItem = m_iSubItem;
    dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR) str);
    dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();

    GetParent()->GetParent()->SendMessage(WM_NOTIFY,
                                          GetParent()->GetDlgCtrlID(),
                                          (LPARAM) &dispinfo);

    DestroyWindow();
}


void mfcCInCombo::OnNcDestroy()
{
    CComboBox::OnNcDestroy();

    delete this;
}

void mfcCInCombo::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
    if(nChar == VK_ESCAPE || nChar == VK_RETURN)
    {
        if(nChar == VK_ESCAPE)
            m_bESC = TRUE;

        GetParent()->SetFocus();

        return;
    }

    CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void mfcCInCombo::OnCloseup()
{
    GetParent()->SetFocus();
}







/////////////////////////////////////////////////////////////////////////////
// mfcCInList

mfcCInList::mfcCInList(int iItem,int iSubItem,CStringList* plstItems,int nSel)
{
    m_iItem = iItem;
    m_iSubItem = iSubItem;

    m_lstItems.AddTail(plstItems);
    m_nSel = nSel;
    m_bESC = FALSE;
}

mfcCInList::~mfcCInList()
{
}


BEGIN_MESSAGE_MAP(mfcCInList, CListBox)
    //{{AFX_MSG_MAP(mfcCInList)
    ON_WM_CREATE()
    ON_WM_KILLFOCUS()
    ON_WM_NCDESTROY()
    ON_WM_CHAR()
    ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// mfcCInList message handlers

BOOL mfcCInList::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            return TRUE;        // DO NOT process further
        }
    }

    return CListBox::PreTranslateMessage(pMsg);
}

int mfcCInList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if(CListBox::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Set the proper font
    CFont* font = GetParent()->GetFont();

    SetFont(font);

    for(POSITION pos = m_lstItems.GetHeadPosition();pos != NULL;)
    {
        AddString((LPCTSTR) (m_lstItems.GetNext(pos)));
    }

    SetFocus();
    SetCurSel(m_nSel);  

    return 0;
}

void mfcCInList::OnKillFocus(CWnd* pNewWnd)
{
    CListBox::OnKillFocus(pNewWnd);

    CString str;

    GetWindowText(str);

    // Send Notification to parent of ListView ctrl
    LV_DISPINFO dispinfo;

    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
    dispinfo.hdr.idFrom = GetDlgCtrlID();
    dispinfo.hdr.code = LVN_ENDLABELEDIT;

    dispinfo.item.mask = LVIF_TEXT;
    dispinfo.item.iItem = m_iItem;
    dispinfo.item.iSubItem = m_iSubItem;
    dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR) str);
    dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();

    GetParent()->GetParent()->SendMessage(WM_NOTIFY,
                                           GetParent()->GetDlgCtrlID(),
                                           (LPARAM)  &dispinfo);

    DestroyWindow();
}


void mfcCInList::OnNcDestroy()
{
    CListBox::OnNcDestroy();

    delete this;
}

void mfcCInList::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
{
    if(nChar == VK_ESCAPE || nChar == VK_RETURN)
    {
        if(nChar == VK_ESCAPE)
            m_bESC = TRUE;

        GetParent()->SetFocus();

        return;
    }

    CListBox::OnChar(nChar, nRepCnt, nFlags);
}

void mfcCInList::OnCloseup()
{
    GetParent()->SetFocus();
}



