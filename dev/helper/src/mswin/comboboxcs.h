////////////////////////////////////////////////////////////////////////////
// File:	ComboBoxCS.h
// Version:	2
// Created:	10-Jun-2002
//
// Author:	Paul S. Vickery
// E-mail:	paul@vickeryhome.freeserve.co.uk
//
// Implementation of CComboBoxCS which incorporates case-sensitivity for
// searching, selection etc
//
// You are free to use or modify this code, with no restrictions, other than
// you continue to acknowledge me as the original author in this source code,
// or any code derived from it.
//
// If you use this code, or use it as a base for your own code, it would be 
// nice to hear from you simply so I know it's not been a waste of time!
//
// Copyright (c) 2001-2002 Paul S. Vickery
//
////////////////////////////////////////////////////////////////////////////
// Version History:
//
// Version 2 - 10-Jun-2002
// =======================
// Produced new version with changes as below:
// * new (clean) method of subclassing - uses API function GetComboBoxInfo() 
//   if available, else falls back to 'messy' method
// * added handling of CB_SELECTSTRING message
// * minor changes to the searching code
//
// Version 1 - 11-Sep-2001
// =======================
// Initial version
// 
////////////////////////////////////////////////////////////////////////////
// PLEASE LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBOBOXCS_H__AA8D9CF6_96F6_11D5_A8AD_0000B48746CF__INCLUDED_)
#define AFX_COMBOBOXCS_H__AA8D9CF6_96F6_11D5_A8AD_0000B48746CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboBoxCS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListBoxCS window

class CListBoxCS : public CListBox
{
// Construction
public:
	CListBoxCS();

// Attributes
public:

// Operations
public:
	BOOL GetCaseSensitiveSearch() { return m_bCaseSensitiveSearch; }
	void SetCaseSensitiveSearch(BOOL bCaseSensitive = TRUE) { m_bCaseSensitiveSearch = bCaseSensitive; }
	static BOOL GetCaseSensitiveSearchDefault() { return ms_bCaseSensitiveSearchDefault; }
	static void SetCaseSensitiveSearchDefault(BOOL bCaseSensitive = TRUE)  { ms_bCaseSensitiveSearchDefault = bCaseSensitive; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxCS)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxCS();

	// Generated message map functions
protected:
	BOOL m_bCaseSensitiveSearch;
	static BOOL ms_bCaseSensitiveSearchDefault;
	int FindString(int iStart, LPCSTR pszString, BOOL fExact);
	//{{AFX_MSG(CListBoxCS)
	//}}AFX_MSG
	LRESULT OnLBFindString(WPARAM wParam, LPARAM lParam);
	LRESULT OnLBFindStringExact(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCS window

class CComboBoxCS : public CComboBox
{
// Construction
public:
	CComboBoxCS();

// Attributes
public:

// Operations
public:
	BOOL GetCaseSensitiveSearch();
	void SetCaseSensitiveSearch(BOOL bCaseSensitive = TRUE);
	static BOOL GetCaseSensitiveSearchDefault();
	static void SetCaseSensitiveSearchDefault(BOOL bCaseSensitive = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxCS)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboBoxCS();

	// Generated message map functions
protected:
	BOOL m_bInDropDown;
	BOOL m_bInForceListBoxSubclass;
	void ForceListBoxSubclass(BOOL bCleanlyOnly = FALSE);
	LRESULT FindString(WPARAM wParam, LPARAM lParam, BOOL bExact = FALSE);
	CListBoxCS m_listbox;
	//{{AFX_MSG(CComboBoxCS)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnEditchange();
	afx_msg void OnDropdown();
	//}}AFX_MSG
	LRESULT OnCBFindString(WPARAM wParam, LPARAM lParam);
	LRESULT OnCBFindStringExact(WPARAM wParam, LPARAM lParam);
	LRESULT OnCBSelectString(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOBOXCS_H__AA8D9CF6_96F6_11D5_A8AD_0000B48746CF__INCLUDED_)
