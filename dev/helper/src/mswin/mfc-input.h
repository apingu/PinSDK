#if !defined(AFX_INPLACE_CTRL_H__INCLUDED_)
#define AFX_INPLACE_CTRL_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// mfcCInEdit window

class mfcCInEdit : public CEdit
{
    // Construction
public:
    mfcCInEdit()
    {
        CEdit::CEdit();
    }
    mfcCInEdit(int iItem,int iSubItem,CString sInitText);

    // Attributes
private:
    int m_iItem;
    int m_iSubItem;
    CString m_sInitText;
    BOOL m_bESC;     // To indicate whether ESC key was pressed

    // Operations
public:

    // Overrides
    //{{AFX_VIRTUAL(mfcCInEdit)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~mfcCInEdit();

protected:
    //{{AFX_MSG(mfcCInEdit)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnNcDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// mfcCInCombo window

class mfcCInCombo : public CComboBox
{
    // Construction
public:
    mfcCInCombo() {CComboBox::CComboBox();}
    mfcCInCombo(int iItem,int iSubItem,CStringList* plstItems,int nSel);

    // Attributes
public:

    // Operations
private:
    int m_iItem;
    int m_iSubItem;
    CStringList m_lstItems;
    int m_nSel;
    BOOL m_bESC;             // To indicate whether ESC key was pressed

    // Overrides
    //{{AFX_VIRTUAL(mfcCInList)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~mfcCInCombo();

protected:
    //{{AFX_MSG(mfcCInList)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnNcDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnCloseup();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// mfcCInCombo window

class mfcCInList : public CListBox
{
    // Construction
public:
    mfcCInList()
    {
        CListBox::CListBox();
    }
    mfcCInList(int iItem,int iSubItem,CStringList* plstItems,int nSel);

    // Attributes
public:

    // Operations
private:
    int m_iItem;
    int m_iSubItem;
    CStringList m_lstItems;
    int m_nSel;
    BOOL m_bESC;             // To indicate whether ESC key was pressed

    // Overrides
    //{{AFX_VIRTUAL(mfcCInList)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~mfcCInList();

protected:
    //{{AFX_MSG(mfcCInList)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnNcDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnCloseup();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_INPLACE_CTRL_H__INCLUDED_)
