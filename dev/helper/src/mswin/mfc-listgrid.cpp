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
#include "StdAfx.h"
#include "stdio.h"
#include "stdlib.h"
#include "mfc-listgrid.h"
#include "mfc-input.h"

// 光影效果的header
// mfcHeaderCtrl
class mfcHeaderCtrl : public CHeaderCtrl
{
    DECLARE_DYNAMIC(mfcHeaderCtrl)

public:
    mfcHeaderCtrl();
    virtual ~mfcHeaderCtrl();

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    CStringArray m_HChar;
    CString      m_Format;
    CString      m_Facename;  // 字型

public:

    int          m_R;
    int          m_G;
    int          m_B;
    int          m_Gradient;
    float        m_Height;
    int          m_fontHeight;
    int          m_fontWith;
    COLORREF     m_color;
    LRESULT OnLayout(WPARAM wParam, LPARAM lParam);
};

IMPLEMENT_DYNAMIC(mfcHeaderCtrl, CHeaderCtrl)

mfcHeaderCtrl::mfcHeaderCtrl():m_R(180), m_G(180), m_B(180), m_Gradient(8)
{
    m_Format = "";
    m_Height = 1;
    m_fontHeight = 16;
    m_fontWith = 0;
    m_color = RGB(255, 255, 255);
    m_Facename = "微軟正黑體";
}

mfcHeaderCtrl::~mfcHeaderCtrl()
{

}


BEGIN_MESSAGE_MAP(mfcHeaderCtrl, CHeaderCtrl)
    ON_WM_PAINT()
    ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()

void mfcHeaderCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 
    //  CHeaderCtrl::OnPaint()
    int nItem; 
    nItem = GetItemCount();
    for(int i = 0; i < nItem; i++) 
    { 
        CRect tRect;
        GetItemRect(i,&tRect);
        int R = m_R,G = m_G,B = m_B;
        CRect nRect(tRect);
        nRect.left++;
        for(int j = tRect.top; j <= tRect.bottom; j++) 
        { 
            nRect.bottom = nRect.top+1; 
            CBrush _brush; 
            _brush.CreateSolidBrush(RGB(R,G,B));
            dc.FillRect(&nRect,&_brush);
            _brush.DeleteObject();
            R -= m_Gradient; G -= m_Gradient; B -= m_Gradient;
            if (R < 0)R = 0;
            if (G < 0)G = 0;
            if (B < 0)B = 0;
            nRect.top = nRect.bottom; 
        } 
        dc.SetBkMode(TRANSPARENT);
        
        //dc.SetTextColor(RGB(250,50,50)); 
        dc.SetTextColor(m_color);

        
        CFont nFont,* nOldFont; 
        //nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,(TCHAR*)m_Facename);
        nFont.CreateFont(m_fontHeight, m_fontWith, 0, 0, 0, FALSE,FALSE, 0, 0, 0, 0, 0, 0, m_Facename);
        nOldFont = dc.SelectObject(&nFont);

        UINT nFormat = 1;
        if (m_Format[i] == '0')
        {
            nFormat = DT_LEFT;
            tRect.left += 3;
        }
        else if (m_Format[i] == '1')
        {
            nFormat = DT_CENTER;
        }
        else if (m_Format[i] == '2')
        {
            nFormat = DT_RIGHT;
            tRect.right -= 3;
        }
        TEXTMETRIC metric;
        dc.GetTextMetrics(&metric);
        int ofst = 0;
        ofst = tRect.Height() - metric.tmHeight;
        tRect.OffsetRect(0, ofst / 2);
        dc.DrawText(m_HChar[i], &tRect, nFormat);

        dc.SelectObject(nOldFont); 
        nFont.DeleteObject();
    } 
    
    CRect rtRect;
    CRect clientRect;
    GetItemRect(nItem - 1, rtRect);
    GetClientRect(clientRect);
    rtRect.left = rtRect.right + 1;
    rtRect.right = clientRect.right;
    int R = m_R, G = m_G, B = m_B;
    CRect nRect(rtRect);
    
    for(int j = rtRect.top;j <= rtRect.bottom; j++) 
    { 
        nRect.bottom = nRect.top + 1; 
        CBrush _brush; 
        _brush.CreateSolidBrush(RGB(R, G, B));
        dc.FillRect(&nRect, &_brush);
        _brush.DeleteObject();
        R -= m_Gradient; G -= m_Gradient; B -= m_Gradient;
        if (R < 0)R = 0;
        if (G < 0)G = 0;
        if (B < 0)B = 0;
        nRect.top = nRect.bottom; 
    } 
}

LRESULT mfcHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
    HD_LAYOUT &hdl = *(HD_LAYOUT *) lParam; 
    RECT *prc = hdl.prc; 
    WINDOWPOS *pwpos = hdl.pwpos; 

    int nHeight = (int)(pwpos->cy * m_Height);
    pwpos->cy = nHeight; 
    prc->top = nHeight; 
    return lResult; 
}


///////////////////////////////////////////////////////////////////////////////
//
#define IDC_IPEDIT   1000

BEGIN_MESSAGE_MAP(mfcListGrid, CListCtrl)
    // {{AFX_MSG_MAP(mfcListGrid)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_KEYDOWN()
    ON_WM_SIZE()
    //ON_WM_PAINT()
    ON_WM_DRAWITEM()
    //ON_WM_MEASUREITEM()
    //ON_WM_MEASUREITEM_REFLECT()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
void ClearInputControl(std::vector<CEdit*>&     EditList,
                       std::vector<CComboBox*>& CBoxList)
{
    for(UINT i = 0; i < EditList.size(); i++)
    {
        delete EditList[i];
    }
    EditList.clear();

    for(UINT i = 0; i < CBoxList.size(); i++)
    {
        delete CBoxList[i];
    }
    CBoxList.clear();
}
*/

//=============================================================================
// NAME:
// DESC:
//=============================================================================
mfcListGrid::mfcListGrid()
{
    m_ColSelected = 0;
    m_RowSelected = 0;
    //m_FirstSel=-1;
    m_LineWidth = 1;
    m_LineColor = RGB(160, 160, 160);
    m_SelRowColor = RGB(255, 100, 100);
    m_RowLineColor1 = RGB(160, 160, 160);
    m_RowLineColor2 = RGB(120, 120, 160);
    for(UINT i = 0; i < m_GirdTypeVec.size(); i++)
        delete m_GirdTypeVec[i];
    m_GirdTypeVec.clear();
    m_pHeader = new mfcHeaderCtrl;
    m_pEventDelegate = NULL;
    m_bInit = false;
    m_bEditActive = false;
    m_LaseColSelected = -1;
    m_LaseRowSelected = -1; // 紀錄最後一個被選中的欄  複選的時候 才部會 永遠只能抓到最上面
    m_bAutoAlignLastCol = false;
    m_DGRect.top = 0;
    m_DGRect.left = 0;
    m_DGRect.right = 0;
    m_DGRect.bottom = 0;
    m_bResize = true;
    m_HighLightColor = GetSysColor(COLOR_HIGHLIGHT);
    m_SelectedColor  = RGB(255, 100, 100);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
mfcListGrid::~mfcListGrid()
{
    ClearAll();
    for(UINT i = 0; i < m_GirdTypeVec.size(); i++)
        delete m_GirdTypeVec[i];
    m_GirdTypeVec.clear();

    std::map<CString, _tagGrid*>::iterator iter = m_GirdTypeMap.begin();
    while (iter != m_GirdTypeMap.end())
    {
        delete iter->second;
        iter++;
    }

    if(m_pHeader)
        delete m_pHeader;
    if(m_pEventDelegate)
        delete m_pEventDelegate;
    m_bInit = false;

    //ClearInputControl(m_EditList, m_CBoxList);
}


BOOL mfcListGrid::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= LVS_EX_FULLROWSELECT | LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_EX_DOUBLEBUFFER;
    cs.style |= 0x00000001; //LVS_EX_GRIDLINE;

    return CListCtrl::PreCreateWindow(cs);
}


void mfcListGrid::PreSubclassWindow()
{
    // TODO:
    // 哪裡都不給呼叫 所以先放這了
    OnInitialUpdate();

    ModifyStyle(0,LVS_OWNERDRAWFIXED);
    CListCtrl::PreSubclassWindow();
    CHeaderCtrl *pHeader = GetHeaderCtrl();
    m_pHeader->SubclassWindow(pHeader->GetSafeHwnd());
}

// HitTestEx- Determine the row index and column index for a point
// Returns  - the row index or -1 if point is not over a row
// point    - point to be tested.
// col      - to hold the column index
int mfcListGrid::HitTestEx(CPoint &point,int* col) const
{
    int colnum  = 0;
    int row     = HitTest(point, NULL);

    if(col)
        *col = 0;

    // Make sure that the ListView is in LVS_REPORT
    if((GetWindowLong(m_hWnd, GWL_STYLE)  &LVS_TYPEMASK) != LVS_REPORT)
        return row;

    // Get the top and bottom row visible
    row = GetTopIndex();
    int bottom  = row + GetCountPerPage();

    if(bottom > GetItemCount())
        bottom = GetItemCount();

    // Get the number of columns
    CHeaderCtrl* pHeader= (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount    = pHeader->GetItemCount();

    // Loop through the visible rows
    for(;row <= bottom;row++)
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;

        GetItemRect(row, &rect, LVIR_BOUNDS);
        if(rect.PtInRect(point))
        {
            // Now find the column
            for(colnum = 0;colnum < nColumnCount;colnum++)
            {
                int colwidth= GetColumnWidth(colnum);

                if(point.x >= rect.left &&
                               point.x <= (rect.left + colwidth))
                {
                    if(col)
                        *col = colnum;
                    return row;
                }
                rect.left += colwidth;
            }
        }
    }
    return -1;
}

// EditSubLabel - Start edit of a sub item label
// Returns      - Temporary pointer to the new edit control
// nItem        - The row index of the item to edit
// nCol         - The column of the sub item.
CEdit* mfcListGrid::EditSubLabel(int nItem, int nCol)
{
    // The returned pointer should not be saved

    // Make sure that the item is visible
    if(!EnsureVisible(nItem, TRUE))
        return NULL;

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader= (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount    = pHeader->GetItemCount();

    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 5)
        return NULL;

    // Get the column offset
    int offset  = 0;

    for(int i = 0; i < nCol; i++)
        offset += GetColumnWidth(i);

    CRect rect;

    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;

    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;

        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }

    // Get Column alignment
    LV_COLUMN lvcol;

    lvcol.mask = LVCF_FMT;
    GetColumn(nCol, &lvcol);
    DWORD dwStyle;

    if((lvcol.fmt  &LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
        dwStyle = ES_LEFT;
    else if((lvcol.fmt  &LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
        dwStyle = ES_RIGHT;
    else
        dwStyle = ES_CENTER;

    rect.left += offset + 2;
    rect.right = rect.left + GetColumnWidth(nCol);// - 3;
    if(rect.right > rcClient.right)
        rect.right = rcClient.right;

    CEdit* m_pEdit= new mfcCInEdit(nItem, nCol, GetItemText(nItem, nCol));

    dwStyle |= WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
    m_pEdit->Create(dwStyle, rect, this, IDC_IPEDIT);
    m_pEdit->SetWindowText(GetCellText(nCol, nItem));

    //delete m_pEdit;
    //m_pEdit = NULL;

    return m_pEdit;
}

// ShowInPlaceList  - Creates an in-place drop down list for any
//                  - cell in the list view control
// Returns          - A temporary pointer to the combo-box control
// nItem            - The row index of the cell
// nCol             - The column index of the cell
// lstItems         - A list of strings to populate the control with
// nSel             - Index of the initial selection in the drop down list
/*
void SetDropDownHeight(CComboBox* pMyComboBox, int itemsToShow)
{
    //Get rectangles
    CRect rctComboBox, rctDropDown;
    //Combo rect
    pMyComboBox->GetClientRect(&rctComboBox); 
    //DropDownList rect
    pMyComboBox->GetDroppedControlRect(&rctDropDown); 

    //Get Item height
    int itemHeight = pMyComboBox->GetItemHeight(-1); 
    //Converts coordinates
    pMyComboBox->GetParent()->ScreenToClient(&rctDropDown); 
    //Set height
    rctDropDown.bottom = rctDropDown.top + rctComboBox.Height() + itemHeight*itemsToShow; 
    //apply changes
    pMyComboBox->MoveWindow(&rctDropDown); 
}
*/

CComboBox* mfcListGrid::ShowInPlaceList(int nItem,
                                        int nCol,
                                        CStringList& lstItems,
                                        int nSel)
{
    // The returned pointer should not be saved

    // Make sure that the item is visible
    if(!EnsureVisible(nItem, TRUE))
        return NULL;

    // Make sure that nCol is valid
    CHeaderCtrl* pHeader= (CHeaderCtrl*) GetDlgItem(0);
    int nColumnCount    = pHeader->GetItemCount();

    if(nCol >= nColumnCount || GetColumnWidth(nCol) < 10)
        return NULL;

    // Get the column offset
    int offset  = 0;

    for(int i = 0;i < nCol;i++)
        offset += GetColumnWidth(i);

    CRect rect;

    GetItemRect(nItem, &rect, LVIR_BOUNDS);

    // Now scroll if we need to expose the column
    CRect rcClient;

    GetClientRect(&rcClient);
    if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
    {
        CSize size;
        size.cx = offset + rect.left;
        size.cy = 0;
        Scroll(size);
        rect.left -= size.cx;
    }

    rect.left += offset + 4;
    rect.right = rect.left + GetColumnWidth(nCol) - 3;
    int height  = rect.bottom - rect.top;

    rect.bottom += 5 * height;    // 好像怎麼改都沒用
    if(rect.right > rcClient.right)
        rect.right = rcClient.right;

    CComboBox* pList = new mfcCInCombo(nItem, nCol, &lstItems, nSel);

    DWORD dwStyle   = WS_BORDER |
                      WS_CHILD |
                      WS_VISIBLE |
                      WS_VSCROLL |
                      WS_HSCROLL |
                      CBS_DROPDOWNLIST |
                      CBS_DISABLENOSCROLL;

    pList->Create(dwStyle, rect, this, IDC_IPEDIT);
    pList->SetItemHeight(-1, height);
    pList->SetHorizontalExtent(GetColumnWidth(nCol));

    // ???
    pList->SetWindowText(GetCellText(nCol, nItem));    

    //BOOL r = pList->SetWindowPos(&CWnd::wndTop, rect.left, rect.top, (rect.right - rect.left + 1), (1000 + 1), SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE );
    return pList;
}

void mfcListGrid::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
    if(GetFocus() != this)
        SetFocus();
    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void mfcListGrid::OnVScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
    if(GetFocus() != this)
        SetFocus();
    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void mfcListGrid::OnEndLabelEdit(NMHDR* pNMHDR,LRESULT* pResult)
{
    LV_DISPINFO* plvDispInfo= (LV_DISPINFO*) pNMHDR;
    LV_ITEM* plvItem        = &plvDispInfo->item;

    if(plvItem->pszText != NULL)
    {
        CString pre_text = GetItemText(plvItem->iItem, plvItem->iSubItem);
        SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
        if(m_pEventDelegate != NULL)
        {
            //  if text had be change
            if(pre_text != plvItem->pszText)
                m_pEventDelegate->OnEdited(m_ColSelected, m_RowSelected);
        }
    }
    //ClearInputControl(m_EditList, m_CBoxList);
    *pResult = FALSE;
}

void mfcListGrid::OnLButtonDown(UINT nFlags,CPoint point)
{
    CListCtrl::OnLButtonDown(nFlags, point);
    if(GetRowCount() >= 1)
    {
        // 紀錄最後一個被按到的
        m_LaseRowSelected = HitTestEx(point, (int*)&m_LaseColSelected);

        if(m_LaseRowSelected != -1)
        {
            if(m_pEventDelegate != NULL)
                m_pEventDelegate->OnLClick(m_LaseColSelected, m_LaseRowSelected);
        }

        // 
        if(GetRowSelectedState(m_LaseRowSelected))
            return;
    }
    // 誰都沒選到
    m_LaseColSelected=-1;
    m_LaseRowSelected=-1;
}


void mfcListGrid::OnLButtonDblClk(UINT nFlags,CPoint point)
{
    CListCtrl::OnLButtonDblClk(nFlags, point);

    //ClearInputControl(m_EditList, m_CBoxList);

    if((m_RowSelected = HitTestEx(point, (int*)&m_ColSelected)) != -1)
    {
        if(m_pEventDelegate != NULL)
            m_pEventDelegate->OnLDBClk(m_ColSelected, m_RowSelected);

        UINT flag   = LVIS_FOCUSED;

        //if ((GetItemState(m_RowSelected, flag)  &flag) == flag &&m_ColSelected > 0)
        if(GetEnableEditCell(m_ColSelected, m_RowSelected) && m_bEditActive)
        {
            // Add check for LVS_EDITLABELS
            //if (GetWindowLong(m_hWnd, GWL_STYLE)  &LVS_EDITLABELS)
            {
                //if(m_GirdTypeVec.empty())
                //    return;
                if(m_GirdTypeVec[m_ColSelected]->type == edit_t)
                {
                    EditSubLabel(m_RowSelected, m_ColSelected);
                    //m_EditList.push_back(EditSubLabel(m_RowSelected, m_ColSelected));
                }
                else if(m_GirdTypeVec[m_ColSelected]->type == combo_t)
                {
                    ShowInPlaceList(m_RowSelected, m_ColSelected, (m_GirdTypeVec[m_ColSelected]->combolist), 0);
                    //m_CBoxList.push_back();
                }
            }
        }
        //else
        // SetItemState(m_RowSelected, LVIS_SELECTED | LVIS_FOCUSED,
        //  LVIS_SELECTED | LVIS_FOCUSED);
    }

    return;
}

void mfcListGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default
    switch(nChar)
    {
    case VK_UP:
        {
            // 從上 第一個
            m_LaseRowSelected=-1;
            for(int i = 0;i < GetItemCount();i++)
            {
                if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
                {
                    m_LaseRowSelected=i-1;
                    break;
                }
            }
            if(m_LaseRowSelected<0)
                m_LaseRowSelected=0;
        }
        break;
    case VK_DOWN:
        {
            // 從下 第一個
            m_LaseRowSelected=-1;
            for(int i=GetItemCount()-1; i>=0 ; i--)
            {
                if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
                {
                    m_LaseRowSelected=i+1;
                    break;
                }
            }

            if(m_LaseRowSelected>=GetItemCount())
                m_LaseRowSelected=GetItemCount()-1;
        }
        break;
    case 'A':
    case 'a':
        {
            if(GetAsyncKeyState(VK_CONTROL))
            {
                SetAllRowSelected();
            }
        }
        break;
    }
    CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void mfcListGrid::OnInitialUpdate()
{
    LockWindowUpdate();

    BOOL bRet;

    //  if (m_bFullRowSelected)
    bRet = ModifyStyle(0L, 1);
    //  else
    //      bRet = ModifyStyle(LVS_OWNERDRAWFIXED, 0L);

    // repaint window if we are not changing view type
    if(bRet && (GetStyle()  &LVS_TYPEMASK) == LVS_REPORT)
        Invalidate();

    SetExtendedStyle(LVS_EX_HEADERDRAGDROP);

    // repaint changes
    UnlockWindowUpdate();
}

/*
BOOL mfcListGrid::PreTranslateMessage(MSG* pMsg)
{
    switch(pMsg->message)
    {
    case WM_CREATE:
        {
            OnInitialUpdate();
        }
        break;
    }
    return CListCtrl::PreTranslateMessage(pMsg);
}
*/

void mfcListGrid::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    TCHAR lpBuffer[10240];

    LV_ITEM lvi;

    lvi.mask = LVIF_TEXT | LVIF_PARAM ;
    lvi.iItem = lpDrawItemStruct->itemID ; 
    lvi.iSubItem = 0;
    lvi.pszText = lpBuffer ;
    lvi.cchTextMax = sizeof(lpBuffer);
    VERIFY(GetItem(&lvi));

    LV_COLUMN lvc, lvcprev ;
    ::ZeroMemory(&lvc, sizeof(lvc));
    ::ZeroMemory(&lvcprev, sizeof(lvcprev));
    lvc.mask = LVCF_WIDTH | LVCF_FMT;
    lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    //CDC MemDC;
    //MemDC.CreateCompatibleDC(pItemDC);
    //CDC* pDC = &MemDC;
    //pDC->SetBkMode(TRANSPARENT);

    CRect rtClient;
    GetClientRect(&rtClient);
    for (int nCol = 0; GetColumn(nCol, &lvc); nCol++)
    {
        if (nCol > 0) 
        {
            // Get Previous Column Width in order to move the next display item
            GetColumn(nCol - 1, &lvcprev) ;
            lpDrawItemStruct->rcItem.left += lvcprev.cx ;
            lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left; 
        }

        CRect rcItem;   
        if (!GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_LABEL, rcItem))
            continue;

        ::ZeroMemory(&lvi, sizeof(lvi));
        lvi.iItem = lpDrawItemStruct->itemID;
        lvi.mask = LVIF_TEXT | LVIF_PARAM;
        lvi.iSubItem = nCol;
        lvi.pszText = lpBuffer;
        lvi.cchTextMax = sizeof(lpBuffer);
        VERIFY(GetItem(&lvi));
        CRect rcTemp;
        rcTemp = rcItem;

        if (nCol == 0)
        {
            rcTemp.left -= 2;
        }

        if (lpDrawItemStruct->itemState  & ODS_SELECTED)
        {
            pDC->FillSolidRect(&rcTemp, m_HighLightColor) ;
            pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
        }
        else
        {
            COLORREF color;
            color = GetBkColor();
            pDC->FillSolidRect(rcTemp,color);

            if (FindColColor(nCol, color))
            {
                pDC->FillSolidRect(rcTemp,color);
            }
            if (FindItemColor(nCol,lpDrawItemStruct->itemID,color))
            {
                pDC->FillSolidRect(rcTemp,color);
            }
            //pDC->SetTextColor(m_color);
        }

        //pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

        UINT uFormat = DT_CENTER ;
        if (m_pHeader->m_Format[nCol] == '0')
        {
            uFormat = DT_LEFT;
        }
        else if (m_pHeader->m_Format[nCol] == '1')
        {
            uFormat = DT_CENTER;
        }
        else if (m_pHeader->m_Format[nCol] == '2')
        {
            uFormat = DT_RIGHT;
        }

        TEXTMETRIC metric;
        pDC->GetTextMetrics(&metric);
        int ofst;
        ofst = rcItem.Height() - metric.tmHeight;
        rcItem.OffsetRect(5, ofst / 2);
        pDC->SetTextColor(m_color);
        COLORREF color;
        if (FindColTextColor(nCol, color))
        {
            pDC->SetTextColor(color);
        }
        if (FindItemTextColor(nCol, lpDrawItemStruct->itemID, color))
        {
            pDC->SetTextColor(color);
        }
        //CFont nFont ,* nOldFont; 
        //nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT(""));//
        //nOldFont = pDC->SelectObject(&nFont);
        DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcsclen(lpBuffer), &rcItem, uFormat) ;

        //pDC->SelectStockObject(SYSTEM_FONT) ;
    }

    //pItemDC->BitBlt(rtClient.left, rtClient.top, 
    //                (rtClient.right - rtClient.left),
    //                (rtClient.bottom - rtClient.top), &MemDC, 0, 0, SRCCOPY );
    //ReleaseDC(pDC);

    //CDC::FromHandle(lpDrawItemStruct->hDC)->DeleteDC();
}

// 改用新的畫法 目前不會被呼叫到
void mfcListGrid::OnPaint()
{
    // First let the control do its default drawing.
    const MSG* msg  = GetCurrentMessage();
    DefWindowProc(msg->message, msg->wParam, msg->lParam);

    //******//return;
    // Draw the lines only for LVS_REPORT mode
    if((GetStyle()  &LVS_TYPEMASK) == LVS_REPORT)
    {
        // Get the number of columns
        CClientDC dc(this);
        HPEN hPen   = CreatePen(PS_SOLID, m_LineWidth, m_LineColor);
        if(hPen == 0)
            return;
        SelectObject(dc.m_hDC, hPen);

        CHeaderCtrl* pHeader= (CHeaderCtrl*)GetDlgItem(0);
        int nColumnCount    = pHeader->GetItemCount();

        // The bottom of the header corresponds to the top of the line 
        RECT rect;
        pHeader->GetClientRect(&rect);
        int top = rect.bottom;

        // Now get the client rect so we know the line length and
        // when to stop
        GetClientRect(&rect);

        int drawrows= GetCountPerPage();
        if(GetRowCount() < (UINT)GetCountPerPage())
            drawrows = GetRowCount();

        // Draw the horizontal grid lines
        // First get the height
        if(!GetItemRect(0, &rect, LVIR_BOUNDS))
            return;

        int height  = rect.bottom - rect.top;

        GetClientRect(&rect);
        int width   = rect.right;


        HPEN hPen1   = CreatePen(PS_SOLID, m_LineWidth, m_RowLineColor1);
        HPEN hPen2   = CreatePen(PS_SOLID, m_LineWidth, m_RowLineColor2);

         //CBrush brB;
         //brB.CreateSolidBrush(m_RowLineColor1);  
        //int selrow  = GetRowSelected();

        int i       = 0;
        for(i = 1;i <= drawrows;i++)
        {
            if(GetRowSelectedState(i-1))
            {
                //CBrush brB;
                //brB.CreateSolidBrush(m_SelRowColor);  

                HPEN hSelPen   = CreatePen(PS_SOLID, m_LineWidth, m_SelRowColor);

                SelectObject(dc.m_hDC, hSelPen);

                dc.MoveTo(2, (top + height * i)-1);
                dc.LineTo(width, (top + height * i)-1);

                dc.MoveTo(2, (top + height * i)-3);
                dc.LineTo(width, (top + height * i)-3);

                DeleteObject(hSelPen);

                SelectObject(dc.m_hDC, hPen);

                /*
                CRect rt;
                rt.top=(top + height * (i));
                rt.left=0;
                rt.right=width;
                rt.bottom = (top + height * (i));
                dc.FrameRect(&rt, &brB);
                */

            }
            else
            {
                if((i%2) == 0)
                {
                    SelectObject(dc.m_hDC, hPen1);
                    /*
                     CRect rt;
                     rt.top=top + height * (i-1);
                     rt.left=0;
                     rt.right=width;
                     rt.bottom = top + height * (i);
//                     dc.FillRect(&rt, &brB);
                    dc.SetBkMode(TRANSPARENT);
                    dc.FillSolidRect(&rt, RGB(255, 255, 213));
                    */
                }
                else
                {
                    SelectObject(dc.m_hDC, hPen2);
                }

                dc.MoveTo(0, top + height * i);
                dc.LineTo(width, top + height * i);
            }
            /*
            else
            {
                if((i%2) == 0)
                {
                    CRect rt;
                    rt.top=top + height * (i-1);
                    rt.left=0;
                    rt.right=width;
                    rt.bottom = top + height * (i);
                    dc.FillRect(&rt, &brB);
                }
            }
            */
        }

        DeleteObject(hPen1);
        DeleteObject(hPen2);

        SelectObject(dc.m_hDC, hPen);

        int colsheight  = top + height * (i - 1);

        // The border of the column is offset by the horz scroll
        int borderx     = 0 - GetScrollPos(SB_HORZ);
        for(i = 0;i < nColumnCount;i++)
        {
            // Get the next border
            borderx += GetColumnWidth(i);
            // if next border is outside client area, break out
            if(borderx >= rect.right)
                break;

            // Draw the line.
            dc.MoveTo(borderx - 1, top);
            dc.LineTo(borderx - 1, colsheight);
        }

        DeleteObject(hPen);
    }

    // Do not call CListCtrl::OnPaint() for painting messages
}

BOOL mfcListGrid::OnNotify(WPARAM wParam,LPARAM lParam,LRESULT* pResult)
{
    HD_NOTIFY* pHD  = (HD_NOTIFY*) lParam;

    if((pHD->hdr.code == HDN_ITEMCHANGINGA ||
        pHD->hdr.code == HDN_ITEMCHANGINGW) &&
        (GetStyle()  &LVS_TYPEMASK) == LVS_REPORT)
    {
        // Invalidate empty bottom part of control to force erase the previous 
        // position of column
        int nBottom, nLastItem = GetItemCount() - 1;
        if(nLastItem < 0)
        {
            // List is empty : invalidate whole client rect
            nBottom = 0;
        }else
        {
            // Get Y position of bottom of list (last item)
            RECT ItemRect;
            GetItemRect(nLastItem, &ItemRect, LVIR_BOUNDS);
            nBottom = ItemRect.bottom;
        }
        RECT rect;
        GetClientRect(&rect);
        if(nBottom < rect.bottom)
        {
            // Set top of rect as bottom of list (last item) : rect = empty part of list
            rect.top = nBottom;
            InvalidateRect(&rect);
        }
        // NB: We must go on with default processing. 
    } 

    *pResult = 0;

    return CListCtrl::OnNotify(wParam, lParam, pResult);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::Clear()
{
    int count = GetItemCount();
    DeleteAllItems();
    AddNewRow(count);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::ClearContents()
{
    DeleteAllItems();
    int nColumnCount = GetHeaderCtrl()->GetItemCount();
    // Delete all of the columns.
    for (int i=0; i < nColumnCount;i++)
    {
        DeleteColumn(0);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::RegisterEventDelegate(EventDelegate* pDelegate)
{
    if(m_pEventDelegate != NULL)
        delete m_pEventDelegate;
    m_pEventDelegate = pDelegate;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::AddNewRow(int num)
{
    //LV_ITEM lvitem;

    //if(x = 0)
    //{
    int ret = GetRowCount();
    if(num > 0)
    {
        for(int i = 0; i < num; i++)
        {
            LV_ITEM lvitem;
            lvitem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
            lvitem.iItem = ret + i;           // zero-based index of the item
            lvitem.iSubItem = 0;              // zero-based index of the subitem
            lvitem.pszText = NULL;            // the item text 
            lvitem.iImage = 0;                // index of the list view item icon
            //lvitem.lParam   = NULL;
            int nParentItemKey  = InsertItem(&lvitem);
        }
    }
    else if(num < 0)
    {
        for(int i = 0; i < abs(num); i++)
        {
            DeleteItem((ret - i - 1));
        }
    }

    return 1;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::InsertRows(int afteridx, int num)
{
    if(afteridx < 0)
        return FALSE;

    for(int i = 0; i < num; i++)
    {
        LV_ITEM lvitem;
        lvitem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        lvitem.iItem = afteridx + i;      // zero-based index of the item
        lvitem.iSubItem = 0;              // zero-based index of the subitem
        lvitem.pszText = NULL;            // the item text 
        lvitem.iImage = 0;                // index of the list view item icon
        //lvitem.lParam   = NULL;
        int nParentItemKey  = InsertItem(&lvitem);
    }

    return TRUE;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::SetRowSize(int num)
{
    AddNewRow(num-GetRowCount());
    return 1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcListGrid::RemoveRows(int afteridx, int num)
{
    if(afteridx < 0)
        return FALSE;

    BOOL bResult = TRUE;
    for(int i = 0; i < min(num, (int)GetRowCount() - afteridx); i++)
    {
        if(DeleteItem(afteridx + i) == FALSE)
            bResult = FALSE;
    }

    return bResult;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcListGrid::RemoveAllRows(void)
{
    return DeleteAllItems();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::AddNewCol(const TCHAR* name,int nFormat,int width,const TCHAR* listtext,...)
{
    int count            = 0;//, sum = 0;
    const TCHAR* i       = listtext;
    _tagGrid* lstItems   = new _tagGrid;


    va_list textList;
    va_start(textList, listtext);     /* Initialize variable arguments. */
    while(i != 0)
    {
        //sum += 1;
        lstItems->combolist.AddTail(i);
        count++;

        i = va_arg(textList, const TCHAR *);
        //printf("%s",i);
    }
    va_end(textList);              /* Reset variable arguments.      */
    //return(sum ? (sum / count) : 0);
    if(count == 0)
    {
        //delete lstItems;
        //lstItems = NULL;
        lstItems->type = edit_t;
    }
    else
    {
        lstItems->type = combo_t;
    }

    //for(int i = 0; i < num; i++)
    InsertColumn(GetColCount(), name, nFormat, width);
    m_GirdTypeVec.push_back(lstItems);
    return 1;
}

long mfcListGrid::AddNewCol(const TCHAR* name,int nFormat,int width, std::vector<CString> listtext)
{
    int count               = 0;//, sum = 0;
    _tagGrid* lstItems   = new _tagGrid;

    for(UINT i = 0; i<listtext.size(); i++)
    {
        lstItems->combolist.AddTail(listtext[i]);
    }


    //return(sum ? (sum / count) : 0);
    if(listtext.size() == 0)
    {
        //delete lstItems;
        //lstItems = NULL;
        lstItems->type = edit_t;
    }
    else
    {
        lstItems->type = combo_t;
    }
    //for(int i = 0; i < num; i++)
    InsertColumn(GetColCount(), name, nFormat, width);
    m_GirdTypeVec.push_back(lstItems);
    return 1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcListGrid::DeleteColumn(int nCol)
{
    if(nCol < 0)
    {
        return FALSE;
    }
    else
    {
        if(nCol < ((int)m_GirdTypeVec.size()))
        {
            delete m_GirdTypeVec[nCol];
            m_GirdTypeVec.erase(m_GirdTypeVec.begin() + nCol);
        }

        m_pHeader->m_HChar.RemoveAt(nCol);
        return CListCtrl::DeleteColumn(nCol);
    }
    return TRUE;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetGridLineColor(BYTE red,BYTE green,BYTE blue)
{
    m_LineColor = RGB(red, green, blue);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetGridLineWidth(UINT width)
{
    m_LineWidth = width;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
UINT mfcListGrid::GetRowCount(void)
{
    return GetItemCount();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
UINT mfcListGrid::GetColCount(void)
{
    CHeaderCtrl* pHeader= (CHeaderCtrl*) GetDlgItem(0);
    if(pHeader == NULL)
        return 0;
    int nColumnCount    = pHeader->GetItemCount();
    return nColumnCount;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetRows(long cy)
{
    SetItemCount(cy);
}



//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetCols(long cx)
{
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::GetColWidth(UINT cx)
{
    return GetColumnWidth(cx);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetColWidth(UINT nCol,long cx)
{
    SetColumnWidth(nCol, cx);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::GetRowHeight(UINT cy)
{
    return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetRowHeight(UINT nRow,long cy)
{
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
RECT mfcListGrid::GetCellRect(UINT x, UINT y)
{
    CRect rect;

    GetSubItemRect(y, x, LVIR_BOUNDS, rect);
    return rect;

    //GetItemRect(y, &rect, LVIR_BOUNDS);
    //
    //CRect rcClient;
    //GetClientRect(&rcClient);
    //if(rect.left < 0 || rect.left > rcClient.right)
    //{
    //    CSize size;
    //
    //    size.cx = rect.left;
    //    size.cy = 0;
    //    Scroll(size);
    //    rect.left -= size.cx;
    //}
    //rect.right = rect.left + GetColumnWidth(x);// - 3;
    //if(rect.right > rcClient.right)
    //    rect.right = rcClient.right;
    //return rect;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
CString mfcListGrid::GetCellText(UINT col,UINT row) const
{
    CString str = GetItemText(row, col);
    return str;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcListGrid::SetCellText(const TCHAR* buf,UINT col,UINT row)
{
    if((row >= GetRowCount()) || (col >= GetColCount()))
        return false;
    SetItemText(row, col, buf);
    //if(m_pEventDelegate != NULL)
    //    m_pEventDelegate->OnChange(m_ColSelected, m_RowSelected);
    return SetItemText(row, col, buf);
}

//=============================================================================
// NAME: Yu
// DESC: Set combo box data to a cell
//=============================================================================
CString GetGridTypePos( UINT Col, UINT Row )
{
    CString Pos;
    Pos.Format(_TEXT("x%d:y%d"), Col, Row);
    return Pos;
}

long mfcListGrid::SetComboboxList( UINT col, UINT row, std::vector<CString> listtext )
{
    if (col > GetColCount())
    {
        return 0;
    }

    //     if (row > GetRowCount())
    //     {
    //         return 0;
    //     }

    if(listtext.empty())
    {
        return 0;
    }

    CString Pos = GetGridTypePos(col, row);

    std::map<CString, _tagGrid*>::iterator iter = m_GirdTypeMap.find(Pos);
    if (iter != m_GirdTypeMap.end())
    {
        // Key has been exist, do something here..
    }

    _tagGrid* listItems = new _tagGrid;

    for(UINT i = 0; i < listtext.size(); i++)
    {
        listItems->combolist.AddTail(listtext[i]);
    }

    listItems->type = combo_t;
    m_GirdTypeMap[Pos] = listItems;

    return 1;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int mfcListGrid::GetLastRow(int col)
{
    for(int i = GetRowCount();i >= 0;i--)
    {
        CString str = GetCellText(col, i);
        if(!str.IsEmpty())
            return i;
    }
    return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::GetColSelected(void)
{
    return m_ColSelected;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetColSelected(long nSelect)
{
    m_ColSelected = nSelect;
    //SetSelColumn(nNewValu);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long mfcListGrid::GetRowSelected(void)
{
    if(GetRowSelectedCount()>1)
    {
        // 有最後一個找最後一個
        if(m_LaseRowSelected!=-1)
            return m_LaseRowSelected;
    }

    //反則傳回第一個
    int i = 0;
    for(i = 0;i < GetItemCount();i++)
    {
        if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            break;
    }
    if(i == GetItemCount())
        return -1;

    m_LaseRowSelected=i;
    return i;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetRowSelected(long nSelect)
{
    if((nSelect >= 0) &&
       (nSelect<GetItemCount()))
    {
        SetItemState(nSelect, LVIS_SELECTED | LVIS_FOCUSED, \
            LVIS_SELECTED | LVIS_FOCUSED);
        EnsureVisible(nSelect, FALSE);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcListGrid::SetCurRowSelected(long nSelect)
{
    SetItemState(-1, ~LVIS_SELECTED, LVIS_SELECTED);

    if((nSelect >=0 )&&
        (nSelect<GetItemCount()))
    {
        SetItemState(nSelect, LVIS_SELECTED | LVIS_FOCUSED, \
            LVIS_SELECTED | LVIS_FOCUSED);
        EnsureVisible(nSelect, FALSE);
        m_LaseRowSelected=nSelect;
    }
    else
    {
        m_LaseRowSelected = -1;
    }
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcListGrid::GetRowSelectedState(UINT i)
{
    if(i < (UINT)GetItemCount())
    {
        if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            return true;
    }
    return false;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int mfcListGrid::GetRowSelectedCount()
{
    //return GetRowSelectedCount();
    int count   = 0;
    for(int i = 0; i < GetItemCount(); i++)
        if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            count ++;
    return count;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
int mfcListGrid::GetFirstSel()
{
    return m_FirstSel;
}
*/

void mfcListGrid::SetAllRowSelected(BOOL t)
{
    //m_FirstSel=0;
    int ti=(t)? LVIS_SELECTED: 0;
    for(int i = 0; i < GetItemCount(); i++)
    {
        SetItemState(i, ti, LVIS_SELECTED);
    }
}

void mfcListGrid::SetEnableEdit(BOOL b)
{
    m_bEditActive = b;
    if(b == TRUE)
    {
        m_GridMask.clear();
    }
}

void mfcListGrid::SetEnableEditCell(UINT x, UINT y, BOOL b)
{
    CString cs;
    cs.Format(_TEXT("%d:%d"),x, y);

    if(b == TRUE)
    {
        m_GridMask.erase(cs);
    }
    else
    {
        m_GridMask.insert(std::set<CString>::value_type(cs));
    }
}

BOOL mfcListGrid::GetEnableEditCell(UINT x, UINT y)
{
    CString cs;
    cs.Format(_TEXT("%d:%d"),x, y);

    std::set<CString>::iterator iter = m_GridMask.find(cs);
    if(iter != m_GridMask.end())
        return FALSE;
    return TRUE;
}

void mfcListGrid::SetResize(BOOL b)            // 要不要跟隨 父視窗改變大
{
    m_bResize = b;
}

void mfcListGrid::SetAutoAlignLastCol(BOOL b)  // 自動把最後一個欄位 拉到最底
{
    m_bAutoAlignLastCol = b;
}

////////////////////////////////////////////////////////////////////////////
struct stColor
{
    int nRow;
    int nCol;
    COLORREF rgb;
};

int mfcListGrid::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat /* = LVCFMT_LEFT */, int nWidth /* = -1 */, int nSubItem /* = -1 */)
{
    if(!m_bInit)
    {
        SetWindowLong(m_hWnd ,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
        SetExtendedStyle(LVS_EX_GRIDLINES);
        ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE,
            LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
        m_bInit = true;
    }


    m_pHeader->m_HChar.Add(lpszColumnHeading);
    if (nFormat==LVCFMT_LEFT)
    {
        m_pHeader->m_Format = m_pHeader->m_Format + _TEXT("0");
    }
    else if (nFormat==LVCFMT_CENTER)
    {
        m_pHeader->m_Format = m_pHeader->m_Format + _TEXT("1");
    }
    else if (nFormat==LVCFMT_RIGHT)
    {
        m_pHeader->m_Format = m_pHeader->m_Format + _TEXT("2");
    }
    else
    {
        m_pHeader->m_Format = m_pHeader->m_Format + _TEXT("1");
    }
    return CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);
}

void mfcListGrid::SetHeaderBKColor(int R, int G, int B, int Gradient)
{
    m_pHeader->m_R = R;
    m_pHeader->m_G = G;
    m_pHeader->m_B = B;
    m_pHeader->m_Gradient = Gradient;
}

void mfcListGrid::SetHeaderTextColor(COLORREF color)
{
    m_pHeader->m_color = color;
}

void mfcListGrid::SetHeaderFont(int W, int H, const TCHAR* lpszFacename)
{
    m_pHeader->m_fontHeight = H;
    m_pHeader->m_fontWith   = W;
    m_pHeader->m_Facename   = lpszFacename;
}

BOOL mfcListGrid::FindColColor(int col,COLORREF& color)
{
    int flag = 0;
    for (POSITION pos = m_ptrListCol.GetHeadPosition(); pos != NULL ;)
    {
        stColor* pColor = (stColor*)m_ptrListCol.GetNext(pos);
        if (pColor->nCol==col)
        {
            flag = 1;
            color = pColor->rgb;
            break;
        }
    }
    if (1==flag)
    {
        return true;
    }
    return false;
}

BOOL mfcListGrid::FindItemColor(int col, int row, COLORREF& color)
{
    int flag = 0;
    for (POSITION pos = m_ptrListItem.GetHeadPosition(); pos != NULL ;)
    {
        stColor* pColor = (stColor*)m_ptrListItem.GetNext(pos);
        if (pColor->nCol == col && pColor->nRow == row)
        {
            flag = 1;
            color = pColor->rgb;
            break;
        }
    }
    if (1==flag)
    {
        return true;
    }
    return false;
}

void mfcListGrid::ClearAll()
{
    for (POSITION pos = m_ptrListCol.GetHeadPosition(); pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ptrListCol.GetNext(pos);
        delete pColor;
    }

    if(m_ptrListCol.GetCount() > 0)
        m_ptrListCol.RemoveAll();

    for (POSITION pos = m_ptrListItem.GetHeadPosition(); pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ptrListItem.GetNext(pos);
        delete pColor;
    }

    if(m_ptrListItem.GetCount() > 0)
        m_ptrListItem.RemoveAll();

    for (POSITION pos = m_colTextColor.GetHeadPosition();pos != NULL;)
    {
        stColor* pColor = (stColor*)m_colTextColor.GetNext(pos);
        delete pColor;
    }

    if(m_colTextColor.GetCount() > 0)
        m_colTextColor.RemoveAll();

    for (POSITION pos = m_ItemTextColor.GetHeadPosition();pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ItemTextColor.GetNext(pos);
        delete pColor;
    }

    if(m_ItemTextColor.GetCount() > 0)
        m_ItemTextColor.RemoveAll();

    m_GridMask.clear();
}

BOOL mfcListGrid::DeleteAllItems()
{
    ClearAll();
    return CListCtrl::DeleteAllItems();
}

void mfcListGrid::SetColColor(int col,COLORREF color)
{
    for (POSITION pos = m_ptrListCol.GetHeadPosition(); pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ptrListCol.GetNext(pos);
        if( pColor->nCol == col)
        {
            pColor->rgb = color;
            return;
        }
    }

    stColor* pColor  = new stColor;
    pColor->nCol = col;
    pColor->rgb = color;
    m_ptrListCol.AddTail(pColor);
}

void mfcListGrid::SetItemColor(int col, int row, COLORREF color)
{
    for (POSITION pos = m_ptrListItem.GetHeadPosition(); pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ptrListItem.GetNext(pos);
        if( (pColor->nCol == col) &&
            (pColor->nRow == row) )
        {
            pColor->rgb = color;
            return;
        }
    }

    stColor* pColor  = new stColor;
    pColor->nCol = col;
    pColor->nRow = row;
    pColor->rgb = color;

    m_ptrListItem.AddTail(pColor);
}

BOOL mfcListGrid::SetTextColor(COLORREF cr)
{
    m_color = cr;
    return TRUE;
}

void mfcListGrid::SetColTextColor(int col,COLORREF color)
{
    for (POSITION pos = m_colTextColor.GetHeadPosition();pos != NULL;)
    {
        stColor* pColor = (stColor*)m_colTextColor.GetNext(pos);
        if( pColor->nCol == col)
        {
            pColor->rgb = color;
            return;
        }
    }

    stColor* pColor = new stColor;
    pColor->nCol = col;
    pColor->rgb = color;
    m_colTextColor.AddTail(pColor);
}

BOOL mfcListGrid::FindColTextColor(int col,COLORREF& color)
{
    int flag = 0;
    for(POSITION pos = m_colTextColor.GetHeadPosition();pos != NULL;)
    {
        stColor* pColor = (stColor*)m_colTextColor.GetNext(pos);
        if (pColor->nCol==col)
        {
            flag = 1;
            color = pColor->rgb;
            break;
        }
    }
    if(1 == flag)
    {
        return true;
    }
    return false;
}

BOOL mfcListGrid::FindItemTextColor(int col, int row, COLORREF& color)
{
    int flag = 0;
    for(POSITION pos = m_ItemTextColor.GetHeadPosition(); pos != NULL;)
    {
        stColor* pColor = (stColor*)m_ItemTextColor.GetNext(pos);
        if (pColor->nCol == col && pColor->nRow == row)
        {
            flag = 1;
            color = pColor->rgb;
            break;
        }
    }
    if (1 == flag)
    {
        return true;
    }
    return false;
}
void mfcListGrid::SetItemTextColor(int col, int row, COLORREF color)
{
    stColor* pColor = new stColor;
    pColor->nCol = col;
    pColor->nRow = row;
    pColor->rgb = color;
    m_ItemTextColor.AddTail(pColor);
}

void mfcListGrid::OnSize(UINT nType, int cx, int cy)
{
    CListCtrl::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    if(m_bAutoAlignLastCol)
    {
        int allw=0;
        for(unsigned int i = 0; i < GetColCount() - 1; i++)
        {
            allw += GetColWidth(0);
        }

        RECT rt;
        GetClientRect(&rt);
        if(m_hWnd)
        {
            int w2 = rt.right - rt.left - allw;
            int lastcol = GetColCount() - 1;
            SetColWidth(lastcol, w2);
        }
    }
}

void mfcListGrid::ResizeWithParent()
{
    if(m_hWnd == 0)
        return;

    CWnd* pParent = GetParent();
    if(pParent)
    {
        RECT wndRect;
        ::GetWindowRect( m_hWnd, &wndRect );

        RECT rectParent;
        ::GetWindowRect( pParent->m_hWnd, &rectParent );


        if( ( m_DGRect.top == 0.0f ) &&
            ( m_DGRect.left == 0.0f ) &&
            ( m_DGRect.right == 0.0f ) && 
            ( m_DGRect.bottom == 0.0f ) )
        {
            m_DGRect.top = int(100 * double(wndRect.top) / double(rectParent.right - rectParent.left));
            m_DGRect.left = int(100 * double(wndRect.left) / double(rectParent.right - rectParent.left));
            m_DGRect.right = int(100 * double(wndRect.right - wndRect.left) / double(rectParent.right - rectParent.left));
            m_DGRect.bottom = int(100 * double(wndRect.bottom - wndRect.top) / double(rectParent.bottom - rectParent.top));
        }

        // Recalculate DataGrid relative size
        //RECT clientRect;
        //::GetClientRect( m_hWnd, &clientRect );

        ///int xx = int((double(m_DGRect.left) / 100.0) * (rectParent.right - rectParent.left)) - rectParent.left;
        //int yy = int((double(m_DGRect.left) / 100.0) * (rectParent.right - rectParent.left)) - rectParent.top;
        ::SetWindowPos( m_hWnd, HWND_NOTOPMOST, 0, 0, 
                        int((double(m_DGRect.right) / 100.0) * (rectParent.right-rectParent.left)),
                        int((double(m_DGRect.bottom) / 100.0) * (rectParent.bottom-rectParent.top)),
                        SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOMOVE);
        //RecalcWindow(m_hWnd);
        //::InvalidateRect( m_hWnd, NULL, FALSE );
        //::UpdateWindow(m_hWnd);
    }




    /*
    RECT rectParent;
    GetWindowRect( hParent, &rectParent );
    m_DGRect.left = int(100*double(wndRect.left)/double(rectParent.right-rectParent.left));
    m_DGRect.top = int(100*double(wndRect.top)/double(rectParent.right-rectParent.left));
    m_DGRect.right = int(100*double(wndRect.right-wndRect.left)/double(rectParent.right-rectParent.left));
    m_DGRect.bottom = int(100*double(wndRect.bottom-wndRect.top)/double(rectParent.bottom-rectParent.top));


    RECT rectParent;
    GetWindowRect( m_hParentWnd, &rectParent );
    SetWindowPos( m_hWnd, HWND_NOTOPMOST, int((double(m_DGRect.left)/100.0)*(rectParent.right-rectParent.left)), int((double(m_DGRect.top)/100.0)*(rectParent.bottom-rectParent.top)), 
    int((double(m_DGRect.right)/100.0)*(rectParent.right-rectParent.left)), int((double(m_DGRect.bottom)/100.0)*(rectParent.bottom-rectParent.top)), SWP_NOZORDER | SWP_SHOWWINDOW );
    RECT clientRect;
    GetClientRect( m_hWnd, &clientRect );
    RecalcWindow(m_hWnd);
    InvalidateRect( m_hWnd, NULL, TRUE );
    UpdateWindow(m_hWnd);

    */
}

void mfcListGrid::GetCellByWindowPos(CPoint point, int& col, int& row) const
{
    RECT rect;
    GetWindowRect( &rect );

    row = HitTestEx(CPoint(point.x - rect.left, point.y - rect.top), (int*)&col);
    if(row == -1)
        col = -1;
}

/*
BOOL CD_ImageList::AddFileListItem(int nItem, TCHAR lpszItem[_MAX_ITEMS][_MAX_LENGTH], int nColumnCount)
{
    TCHAR    szText[_MAX_LPATH];
    int     i;
    LV_ITEM lvitem;

    // Set main item
    wsprintf(szText, "%d", nItem);
    
    lvitem.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvitem.iItem    = nItem;        // zero-based index of the item
    lvitem.iSubItem = 0;            // zero-based index of the subitem
    lvitem.pszText  = lpszItem[0];  // the item text 
    lvitem.iImage   = 0;            // index of the list view item icon
    lvitem.lParam   = LVM_SORTITEMS ;

    int nParentItemKey = m_File_list.InsertItem(&lvitem);

    // Set sub items
    for (i = 1; i < nColumnCount; i++)
    {
        lvitem.mask     = LVIF_TEXT;
        lvitem.iItem    = nParentItemKey;
        lvitem.iSubItem = i;
        lvitem.pszText  = lpszItem[i];
        if(!m_File_list.SetItem(&lvitem))
           return false;
    }
    return true;
}
*/
