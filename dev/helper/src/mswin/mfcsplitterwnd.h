#if !defined(AFX_USEFULSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
#define AFX_USEFULSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UsefulSplitterWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// mfcCSplitterWnd window

class mfcCSplitterWnd : public CSplitterWnd
{
// Construction
public:
	mfcCSplitterWnd();

// Attributes
public:

private:
	BOOL m_bBarLocked;

// Operations
public:
	BOOL IsBarLocked(){return m_bBarLocked;}
	void LockBar(BOOL bState=TRUE){m_bBarLocked=bState;}
	BOOL ReplaceView(int row, int col,CRuntimeClass * pViewClass,SIZE size);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(mfcCSplitterWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~mfcCSplitterWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(mfcCSplitterWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USEFULSPLITTERWND_H__61D2F7E7_7AAA_11D1_9F4C_008029E98A75__INCLUDED_)
