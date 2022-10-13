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

#ifndef MFCTREECTRL_H
#define MFCTREECTRL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000




class mfcTreeCtrl : public CTreeCtrl
{
public:

    class EventDelegate
    {
    public:
        EventDelegate(){};
        virtual ~EventDelegate(){};

        //virtual void OnChange(HTREEITEM hItem) = 0;   //call SetCellText
        virtual void OnEdited(HTREEITEM hItem) = 0;   //after edit text
        virtual void OnLClick(HTREEITEM hItem) = 0;
        virtual void OnLDBClk(HTREEITEM hItem) = 0;
        virtual void OnDragging(HTREEITEM hItem, UINT tx, UINT ty) = 0;
        virtual void OnDragDrop(HTREEITEM hItem, UINT tx, UINT ty) = 0;
    };

private:

    CEdit*         m_pEdit;
    HTREEITEM      m_SelectedItem;
    BOOL           m_bEdit;
   
    CSize          m_ImageSize;
    CImageList     m_ImageList;

    BOOL           m_bDragable;
    UINT           m_uDragImageId;
    //HTREEITEM    m_DraggedItem;
    BOOL           m_bDragging;

    EventDelegate* m_pEventDelegate;

    //CImageList*  pDragImage;
    //INT          m_nDropIndex;
    //CWnd*        m_pDropWnd;
    //CImageList*  m_pDragImage;

    void           ShowEditBoxe(POINT point);
    void           DestroyEditBox(void);

public:


    mfcTreeCtrl();
    ~mfcTreeCtrl();

    //   example : CreateImage(16, 16);
    //             AddBmap(IDB_REFERENCE_TREE_IMAGES);  // can be one image or image list bitmap
    void         CreateImage( UINT nBitmapID = 0, UINT cx = 16, UINT cy = 16, COLORREF crMask = RGB( 255, 0, 255 ), UINT nFlags = ILC_COLOR32);
    void         AddIcon(UINT  IconID);
    void         AddIcon(HICON hIcon);
    void         AddBmap(UINT BmapID, COLORREF crMask = RGB(0, 0, 0));
    void         AddBmap(HBITMAP hBmap, COLORREF crMask = RGB(0, 0, 0));


    void         SetItemDragable(BOOL t);
    // set drag image index in imagelist
    void         SetDragImageId(UINT idx);

    HTREEITEM    InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,  HTREEITEM hInsertAfter = TVI_LAST);
    HTREEITEM    InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, HTREEITEM hParent = TVI_ROOT,  HTREEITEM hInsertAfter = TVI_LAST);

    void         RegisterEventDelegate(EventDelegate* pDelegate);

    BOOL         SetItemText(HTREEITEM hItem, LPCTSTR lpszItem);

    INT          GetCurSelected(void);
    UINT         GetItemIndex(HTREEITEM hItem);
    UINT         GetItemDepth(HTREEITEM hItem);
    //void       SetItemText(HTREEITEM hItem, CString str);
    //bool       SetImage(HTREEITEM hItem, int nImage, int nSelectedImage);
    CString      GetSelectText(void);
    HTREEITEM    GetSelectItem();
    HTREEITEM    GetItemByText(HTREEITEM hStart, CString text);
    HTREEITEM    GetItemBySubText(HTREEITEM hStart, CString text);  // sub string like XXAAAABBBSXXX  AAAABBBS

    void         SetCanEdit(BOOL b);
    void         SetItemDisable(HTREEITEM hItem, BOOL b);

    HTREEITEM    GetRootItem(){ return CTreeCtrl::GetRootItem(); }

    void         ExpandTree(HTREEITEM hItem);
    void         ExpandChildrenTree(HTREEITEM hItem);   // expand all child tree

    void         CollapseTree(HTREEITEM hItem);
    void         CollapseChildrenTree(HTREEITEM hItem); // Collapse all child tree

    void         ToggleTree(HTREEITEM hItem);           // Collapses the list if it is currently expanded or expands it if it is currently collapsed.
    void         ToggleChildrenTree(HTREEITEM hItem);


    //SelectSetFirstVisible
    //HTREEITEM InsertItem("", itme, 0);
    //BOOL SetItemImage(hItem, 0, 0);

    // Generated message map functions
protected:
    //{{AFX_MSG(CTreeListCtrl)
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTvnBegindragItemviewtree(NMHDR *pNMHDR, LRESULT *pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

};


/*
Step: 01
void CMyResViewF::DoDataExchange(CDataExchange* pDX)
{
CFormView::DoDataExchange( pDX );
DDX_Control( pDX, IDC_WIDGET_LIST, m_WidgetList );
}


*/

#endif


