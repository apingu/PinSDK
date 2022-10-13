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
#ifndef MFCLISTGRID_H
#define MFCLISTGRID_H

#include "vector"
#include "map"
#include "set"

#ifndef _MAX_LENGTH
#define _MAX_LENGTH 512
#endif

#ifndef _MAX_ITEMS
#define _MAX_ITEMS  1024
#endif

///////////////////////////////////////////////////////////////////////////////
//
class mfcHeaderCtrl;
class mfcListGrid;

class mfcListGrid : public CListCtrl
{
public:

    class EventDelegate
    {
    public:
        EventDelegate(){};
        virtual ~EventDelegate(){};

        //virtual void OnChange(UINT col, UINT row) = 0;   //call SetCellText
        virtual void OnEdited(UINT col, UINT row) = 0;     //after edit text
        virtual void OnLClick(UINT col, UINT row) = 0;
        virtual void OnLDBClk(UINT col, UINT row) = 0;
    };

    enum GRIDTYPE { fix_t, edit_t, combo_t, null_t };

private:

    UINT                   m_ColSelected;
    UINT                   m_RowSelected;
    UINT                   m_LineWidth;
    INT                    m_LaseColSelected;
    INT                    m_LaseRowSelected;
    COLORREF               m_LineColor;
    COLORREF               m_RowLineColor1;  // NO
    COLORREF               m_RowLineColor2;  // NO
    COLORREF               m_HighLightColor;
    COLORREF               m_SelectedColor;
    COLORREF               m_SelRowColor;
    BOOL                   m_bEditActive;    // 可不可以進行編輯
    BOOL                   m_bResize;        // 要不要跟隨 父視窗改變大小

    EventDelegate*         m_pEventDelegate;

    struct _tagGrid
    {
        GRIDTYPE    type;
        CStringList combolist;
    };

    std::vector<_tagGrid*>        m_GirdTypeVec;
    std::map<CString, _tagGrid*>  m_GirdTypeMap;
    std::set<CString>             m_GridMask;  // 記錄哪一些格子 不能夠被編輯，記錄  CString %d:%d  有存在的就是不能被編輯

    //std::vector<CEdit*>     m_EditList;
    //std::vector<CComboBox*> m_CBoxList;

    // Construction
public:
    mfcListGrid();
    ~mfcListGrid();

    // Attributes

private:

    // Overrides
    //{{AFX_VIRTUAL(CMainFrame)
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL

    // Implementation

    int          HitTestEx(CPoint& point,int* col) const;
    CEdit*       EditSubLabel(int nItem, int nCol);
    CComboBox*   ShowInPlaceList(int nItem,
                                 int nCol,
                                 CStringList& lstItems,
                                 int nSel);
    void         ClearAll();

public:

    void         Clear();
    void         ClearContents();
    void         RegisterEventDelegate(EventDelegate* pDelegate);

    long         AddNewRow(int num);
    long         InsertRows(int afteridx, int num);
    long         SetRowSize(int num);
    BOOL         RemoveRows(int afteridx, int num);
    BOOL         RemoveAllRows(void);
    long         AddNewCol(const TCHAR* name,int nFormat = LVCFMT_LEFT, int width = -1, const TCHAR* listtext = NULL,...);
    long         AddNewCol(const TCHAR* name,int nFormat, int width, std::vector<CString> listtext);
    long         AddCombobox(UINT col, UINT row, std::vector<CString> listtext);
    long         AddCombobox(UINT col, UINT row, std::set<CString> listtext);


    BOOL         DeleteColumn(int nCol);

    UINT         GetRowCount(void);

    UINT         GetColCount(void);

    void         SetRows(long nNewValue);
    void         SetCols(long nNewValue);

    long         GetColWidth(UINT nCol);
    void         SetColWidth(UINT nCol, long cx);
    long         GetRowHeight(UINT nRow);
    void         SetRowHeight(UINT nRow, long cy);

    RECT         GetCellRect(UINT x, UINT y);

    CString      GetCellText(UINT x, UINT y) const;
    BOOL         SetCellText(const TCHAR* buf, UINT x, UINT y);

    long         SetComboboxList(UINT col, UINT row, std::vector<CString> listtext);

    int          GetLastRow(int col);

    long         GetColSelected(void);
    void         SetColSelected(long nSelect);
    long         GetRowSelected(void);
    void         SetRowSelected(long nSelect);                // 可複數選擇
    void         SetCurRowSelected(long nSelect);             // 只會選一個 其他設為非選狀態
    BOOL         GetRowSelectedState(UINT i);
    int          GetRowSelectedCount(void);
    void         SetAllRowSelected(BOOL t = true);
    //int          GetFirstSel();

    void         SetGridLineColor(BYTE red, BYTE green, BYTE blue);
    void         SetGridLineWidth(UINT width);

    void         SetEnableEdit(BOOL b);                       // 允許不允許編輯
    void         SetEnableEditCell(UINT x, UINT y, BOOL b);   // 允不允編輯單一格
    BOOL         GetEnableEditCell(UINT x, UINT y);

    void         SetResize(BOOL b);                           // 要不要跟隨 父視窗改變大小
    void         SetAutoAlignLastCol(BOOL b);                 // 自動把最後一個欄位 拉到最底

    BOOL         DeleteAllItems();
    void         ResizeWithParent();

    void         GetCellByWindowPos(CPoint point, int& col, int& row) const;

    // Generated message map functions
protected:
    //{{AFX_MSG(CGfxListCtrl)
    afx_msg void OnInitialUpdate();
    //afx_msg BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    virtual void PreSubclassWindow();

private:
    //
    mfcHeaderCtrl* m_pHeader;
    CPtrList       m_ptrListCol;  
    CPtrList       m_ptrListItem; 
    CPtrList       m_colTextColor;
    CPtrList       m_ItemTextColor;
    COLORREF       m_color;
    BOOL           m_bInit;
    BOOL           m_bAutoAlignLastCol;
    RECT           m_DGRect;

    int           InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
                               int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
public:          
                 
    void          SetHeaderBKColor(int R, int G, int B, int Gradient);
    void          SetHeaderTextColor(COLORREF color);
    void          SetHeaderFont(int W, int H, const TCHAR* lpszFacename );
    BOOL          FindColColor(int col, COLORREF& color);
    BOOL          FindItemColor(int col, int row, COLORREF& color);
    BOOL          FindColTextColor(int col, COLORREF& color);
    BOOL          FindItemTextColor(int col, int row, COLORREF& color);
    void          SetColColor(int col, COLORREF color);
    void          SetItemColor(int col, int row, COLORREF color);
    void          SetColTextColor(int col, COLORREF color);
    void          SetItemTextColor(int col, int row, COLORREF color);
    BOOL          SetTextColor(COLORREF cr);
    //            SetItemColor(x, y, dlinecolor);
};


#endif//MFCGRID_H
