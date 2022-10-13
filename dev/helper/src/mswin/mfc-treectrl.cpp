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
#include <Commctrl.h>
#include "mfc-treectrl.h"


BEGIN_MESSAGE_MAP(mfcTreeCtrl, CTreeCtrl)
    //{{AFX_MSG_MAP(mfcTreeCtrl)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()    
    ON_WM_LBUTTONUP()    
    ON_WM_MOUSEMOVE()    
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
    ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnTvnBegindragItemviewtree)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
// NAME:
// DESC:
//=============================================================================
mfcTreeCtrl::mfcTreeCtrl()
{
    m_SelectedItem   = NULL;
    m_pEdit          = NULL;
    m_bEdit          = TRUE;
    m_bDragable      = TRUE;
    m_uDragImageId   = 0;
    m_bDragging      = FALSE;
    m_pEventDelegate = NULL;
    //m_nDropIndex    = -1;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
mfcTreeCtrl::~mfcTreeCtrl()
{
    if(m_pEdit != NULL)
        delete m_pEdit;
    if(m_pEventDelegate != NULL)
        delete m_pEventDelegate;
    m_ImageList.DeleteImageList();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::CreateImage( UINT nBitmapID, UINT cx, UINT cy, COLORREF crMask, UINT nFlags)
{
    m_ImageSize.cx = cx;
    m_ImageSize.cy = cy;

    //m_ImageList.Create( cx, cy, nFlags, 0, 0 );
    m_ImageList.Create( nBitmapID, cx, cy, RGB( 255, 0, 255 ) );
    SetImageList(&m_ImageList, TVSIL_NORMAL);

    //if ( nBitmapID != 0 )
    //{
    //    AddBmap( nBitmapID, crMask );
    //}
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddIcon(UINT IconID)
{
    void* icon  = ::LoadImage(AfxGetInstanceHandle(),
                               MAKEINTRESOURCE(IconID),
                               IMAGE_ICON,
                               m_ImageSize.cx,
                               m_ImageSize.cy,
                               LR_LOADTRANSPARENT);

    //Add the icon to the image list
    m_ImageList.Add(static_cast<HICON>(icon));
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddIcon(HICON hIcon)
{
    m_ImageList.Add(hIcon);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddBmap(UINT BmapID, COLORREF crMask)
{
    CBitmap bm;
    bm.LoadBitmap(BmapID);
    m_ImageList.Add(&bm, crMask);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddBmap(HBITMAP hBmap, COLORREF crMask)
{
    m_ImageList.Add(CBitmap::FromHandle(hBmap), crMask);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::SetItemDragable(BOOL t)
{
    m_bDragable = t;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::SetDragImageId(UINT idx)
{
    m_uDragImageId = idx;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
HTREEITEM mfcTreeCtrl::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent,  HTREEITEM hInsertAfter)
{
    return CTreeCtrl::InsertItem(lpszItem, -1, -1, hParent, hInsertAfter);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
HTREEITEM mfcTreeCtrl::InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent,  HTREEITEM hInsertAfter)
{
    return CTreeCtrl::InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::RegisterEventDelegate(EventDelegate* pDelegate)
{
    if(m_pEventDelegate != NULL)
        delete m_pEventDelegate;
    m_pEventDelegate = pDelegate;
}

/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::CreateDragImage(UINT cx, UINT cy, UINT nFlags)
{
    //m_DragImage.Create(cx, cy, nFlags, 0, 0);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddDragIcon(UINT IconID)
{
    void* icon  = ::LoadImage(AfxGetInstanceHandle(),
        MAKEINTRESOURCE(IconID),
        IMAGE_ICON,
        m_DragImageSize.cx,
        m_DragImageSize.cy,
        LR_LOADTRANSPARENT);

    //Add the icon to the image list
    //m_DragImage.Add(static_cast<HICON>(icon));
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddDragIcon(HICON hIcon)
{
    //m_DragImage.Add(hIcon);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddDragBmap(UINT BmapID)
{
    CBitmap bm;
    bm.LoadBitmap(BmapID);
    //m_DragImage.Add(&bm, RGB(0, 0, 0));
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::AddDragBmap(HBITMAP hBmap)
{
    //m_DragImage.Add(CBitmap::FromHandle(hBmap), RGB(0, 0, 0));
}
*/

//=============================================================================
// NAME:
// DESC:
//=============================================================================
BOOL mfcTreeCtrl::SetItemText(HTREEITEM hItem, LPCTSTR lpszItem)
{
    //if(m_pEventDelegate != NULL)
    //    m_pEventDelegate->OnChange(hItem);
    return CTreeCtrl::SetItemText(hItem, lpszItem);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
INT mfcTreeCtrl::GetCurSelected(void)
{
    return GetItemIndex(GetSelectItem());
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
UINT mfcTreeCtrl::GetItemIndex(HTREEITEM hItem)
{
    HTREEITEM hStart = NULL;
    HTREEITEM hParent = GetParentItem(hItem);
    
    if(hParent == NULL)
    {
        hStart = GetRootItem();
    }
    else
    {
        hStart = GetChildItem(hParent);
    }
    
    int index = 0;
    do
    {
        if(hStart == hItem)
            return index;
        index++;
    }while((hStart = GetNextItem(hStart, TVGN_NEXT)) != NULL);
    return 0;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
UINT mfcTreeCtrl::GetItemDepth(HTREEITEM hItem)
{
    UINT dep = 0;
    HTREEITEM hParent = hItem;
     while((hParent = GetParentItem(hParent)) != NULL)
     {
         dep++;
     }
     return dep;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
HTREEITEM GetChildItemByText(CTreeCtrl*treectrl, HTREEITEM item, CString text, bool cmp)
{
    CString str = treectrl->GetItemText(item);

    if(cmp == true)
    {
        if(str == text)
        {
            return item;
        }
    }
    else
    {
        if(str.Find(text) != -1)
        {
            return item;
        }
    }

    HTREEITEM hStart = treectrl->GetNextItem(item, TVGN_CHILD);
    if(hStart!=NULL)
    {
        do
        {
            HTREEITEM citem=GetChildItemByText(treectrl, hStart, text, cmp);
            if(citem!=NULL)
                return citem;
        }while((hStart = treectrl->GetNextItem(hStart, TVGN_NEXT)) != NULL);
    }

    return NULL;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
HTREEITEM mfcTreeCtrl::GetItemByText(HTREEITEM hStart, CString text)
{
    if(hStart == NULL)
        hStart = GetRootItem();
    return GetChildItemByText(this, hStart, text, true);
}


HTREEITEM mfcTreeCtrl::GetItemBySubText(HTREEITEM hStart, CString text)
{
    if(hStart == NULL)
        hStart = GetRootItem();
    return GetChildItemByText(this, hStart, text, false);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
CString mfcTreeCtrl::GetSelectText(void)
{
    if(m_SelectedItem != NULL)
        return GetItemText(m_SelectedItem);
    return _TEXT("");
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
HTREEITEM mfcTreeCtrl::GetSelectItem()
{
    return m_SelectedItem;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::ShowEditBoxe(POINT point)
{
    if (m_pEdit)
    {
        delete m_pEdit;
    }

    if(m_SelectedItem)
    {
        DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
        CRect m_labelRect;
        GetItemRect(m_SelectedItem, &m_labelRect,TRUE);
        m_pEdit = new CEdit;
        m_pEdit->Create(dwStyle, m_labelRect, this, 1);

        CString s = GetItemText(m_SelectedItem);
        m_pEdit->SetWindowText(s);
    }    
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::DestroyEditBox(void)
{
    if (m_pEdit != NULL)
    {
        CString String;
        m_pEdit->GetWindowText(String);
        if(String != "")
        {
            SetItemText(m_SelectedItem, String);
        }
            
        m_pEdit->DestroyWindow();        
        delete m_pEdit;
        m_pEdit = NULL;        
    }
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
    LV_ITEM    *plvItem = &plvDispInfo->item;    

    HTREEITEM hSelItem = NULL;
    BOOL bHadEscape = FALSE;

    if (m_pEdit != NULL)
    {
        m_pEdit->DestroyWindow();        
        m_pEdit = NULL;        
        
        if((plvItem->pszText != NULL) && (!bHadEscape))
        {            
            SetItemText(hSelItem, plvItem->pszText);
            if(m_pEventDelegate != NULL)
                m_pEventDelegate->OnEdited(hSelItem);
        }
    }
    *pResult = FALSE;    
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::SetCanEdit(BOOL b)
{
    m_bEdit = b;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::SetItemDisable(HTREEITEM hItem, BOOL b)
{
    //TVIS_EX_DISABLED
    UINT s = 0x0002;
    this->CTreeCtrl::SetItemState(hItem, b? s: ~s, 0);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::ExpandTree(HTREEITEM hItem)
{
    Expand(hItem,TVE_EXPAND);

    HTREEITEM hPrevItem = GetParentItem(hItem);
    if(hPrevItem != NULL)
    {
        ExpandTree(hPrevItem);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::ExpandChildrenTree(HTREEITEM hItem) // expand all child tree
{
    if(!ItemHasChildren(hItem))
    {
        return;
    }
    HTREEITEM hNextItem = GetChildItem(hItem);
    while (hNextItem != NULL)
    {
        ExpandChildrenTree(hNextItem);
        hNextItem = GetNextItem(hNextItem, TVGN_NEXT);
    }
    Expand(hItem,TVE_EXPAND);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::CollapseTree(HTREEITEM hItem)
{
    Expand(hItem,TVE_COLLAPSE);

    HTREEITEM hPrevItem = GetParentItem(hItem);
    if(hPrevItem != NULL)
    {
        CollapseTree(hPrevItem);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::CollapseChildrenTree(HTREEITEM hItem)
{
    if(!ItemHasChildren(hItem))
    {
        return;
    }
    HTREEITEM hNextItem = GetChildItem(hItem);
    while (hNextItem != NULL)
    {
        CollapseChildrenTree(hNextItem);
        hNextItem = GetNextItem(hNextItem, TVE_COLLAPSE);
    }
    Expand(hItem,TVE_COLLAPSE);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::ToggleTree(HTREEITEM hItem)
{
    Expand(hItem,TVE_TOGGLE);
    HTREEITEM hPrevItem = GetParentItem(hItem);
    if(hPrevItem != NULL)
    {
        ToggleTree(hPrevItem);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::ToggleChildrenTree(HTREEITEM hItem)
{
    if(!ItemHasChildren(hItem))
    {
        return;
    }
    HTREEITEM hNextItem = GetChildItem(hItem);
    while (hNextItem != NULL)
    {
        ToggleChildrenTree(hNextItem);
        hNextItem = GetNextItem(hNextItem, TVE_TOGGLE);
    }
    Expand(hItem,TVE_TOGGLE);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    m_SelectedItem = HitTest(point, &nFlags);

    if(m_SelectedItem != NULL)
    {
        if(m_pEventDelegate != NULL)
            m_pEventDelegate->OnLDBClk(m_SelectedItem);
    }
    
    if(m_ImageList.m_hImageList != NULL)
    {
        if((m_ImageList.GetImageCount() == 0) || (m_bEdit == false) )
        {
            CTreeCtrl::OnLButtonDblClk(nFlags, point);
            return;
        }
    }

    if(m_SelectedItem != NULL)
    {
        CRect m_labelRect;
        GetItemRect(m_SelectedItem, &m_labelRect, TRUE);                
        if(point.x < m_labelRect.left)
        {

            CTreeCtrl::OnLButtonDblClk(nFlags, point);
            //return;
        }
        else
        {
            SetFocus();    
            if(m_bEdit)
                ShowEditBoxe(point);
        }
    }
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    HTREEITEM item = HitTest(point, &nFlags);

    if(item != NULL)
    {
        if(m_pEventDelegate != NULL)
            m_pEventDelegate->OnLClick(item);
    }

    UINT flags;
    item = HitTest(point, &flags);

    if((flags & TVHT_ONITEMRIGHT) || (flags & TVHT_ONITEMINDENT) || (flags & TVHT_ONITEM))
    {
        SelectItem(item);
    }
    if((m_SelectedItem != item) && (m_SelectedItem != NULL))
        DestroyEditBox();
    m_SelectedItem = item;

    CTreeCtrl::OnLButtonDown(nFlags, point);
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{    
    // find what item is selected
    HTREEITEM item = HitTest(point, &nFlags);

    if((nFlags & TVHT_ONITEMRIGHT) || (nFlags & TVHT_ONITEMINDENT) || (nFlags & TVHT_ONITEM))
    {
        SelectItem(item);
    }

    if(m_SelectedItem != item)
        DestroyEditBox();
    m_SelectedItem = item;

    CTreeCtrl::OnRButtonDown(nFlags, point);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    if(m_bDragging == TRUE)
    {
        // end dragging
        VERIFY(m_ImageList.DragLeave(GetDesktopWindow()));
        m_ImageList.EndDrag();

        // stop interception all mouse messages
        VERIFY(::ReleaseCapture());

        //m_DraggedItem = NULL;
        m_bDragging = false;
        if(m_pEventDelegate != NULL)
            m_pEventDelegate->OnDragDrop(m_SelectedItem, point.x, point.y);
    }
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    if(m_bDragging == TRUE)
    {
        CPoint pt(point);
        ClientToScreen(&pt);

        //move the drag image
        VERIFY(m_ImageList.DragMove(pt));

        // unlock window updates
        //VERIFY(m_ImageList.DragShowNolock(FALSE));

        //get the CWnd poiner of the window that is under the mouse cursor
        //CWnd* pDropWnd = WindowFromPoint(pt);
        //ASSERT(pDropWnd);

        // if we drag outside current window
        //if(pDropWnd != this)
        //{
        //    // turn off drag hilite for tree control
        //    if(m_DraggedItem)
        //    {
        //        this->SelectDropTarget(NULL);
        //        m_DraggedItem = NULL;
        //    }
        //
        //    // turn off drag hilite for list control
        //    if(m_nDropIndex != -1)
        //    {
        //
        //        m_nDropIndex = -1;
        //    }
        //}

        //VERIFY(m_ImageList.DragShowNolock(TRUE));
        if(m_pEventDelegate != NULL)
            m_pEventDelegate->OnDragging(m_SelectedItem, point.x, point.y);

    }
    CTreeCtrl::OnMouseMove(nFlags, point);
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void mfcTreeCtrl::OnTvnBegindragItemviewtree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
    if(m_bDragable != TRUE)
        return ;

    m_SelectedItem = pNMTreeView->itemNew.hItem;
    CImageList* pDragImage  = CreateDragImage(m_SelectedItem);
    if(pDragImage != NULL)
    {
        pDragImage->BeginDrag(m_uDragImageId, CPoint(m_ImageSize.cx / 2, m_ImageSize.cy / 2));
        pDragImage->DragEnter(GetDesktopWindow(), ((NM_TREEVIEW*)pNMHDR)->ptDrag);
        m_bDragging   = TRUE;
        //m_pDropWnd  = this;
        //m_DraggedItem = NULL;
        //m_nDropIndex  = -1; 
        SetCapture();
    }
}

