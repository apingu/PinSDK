////////////////////////////////////////////////////////////////////////////
// File:	ComboBoxCS.cpp
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

#include "stdafx.h"
#include "ComboBoxCS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCS

CComboBoxCS::CComboBoxCS()
{
	m_bInForceListBoxSubclass = FALSE;
	m_bInDropDown = FALSE;
}

CComboBoxCS::~CComboBoxCS()
{
}

BEGIN_MESSAGE_MAP(CComboBoxCS, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxCS)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CB_FINDSTRING, OnCBFindString)
	ON_MESSAGE(CB_FINDSTRINGEXACT, OnCBFindStringExact)
	ON_MESSAGE(CB_SELECTSTRING, OnCBSelectString)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxCS message handlers

HBRUSH CComboBoxCS::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (m_listbox.GetCaseSensitiveSearch() &&  // we need to subclass the list
		  nCtlColor == CTLCOLOR_LISTBOX && pWnd != NULL && 
		  m_listbox.GetSafeHwnd() == NULL)
	{
		// ListBox control - so subclass it
		m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
		// if we're doing this a a result of a FindString, then
		// reduce flickering effect by selecting a hollow brush
		if (m_bInForceListBoxSubclass)
			return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
	}
	return CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
}

// the relevant stuff won't have been included in winuser.h
// if WINVER is less than 0x0500
#if(WINVER < 0x0500)

// Combobox information
typedef struct tagCOMBOBOXINFO
{
    DWORD cbSize;
    RECT  rcItem;
    RECT  rcButton;
    DWORD stateButton;
    HWND  hwndCombo;
    HWND  hwndItem;
    HWND  hwndList;
} COMBOBOXINFO, *PCOMBOBOXINFO, *LPCOMBOBOXINFO;

#endif // WINVER < 0x0500

typedef BOOL (WINAPI *PFNGETCOMBOBOXINFO)(HWND hwndCombo, PCOMBOBOXINFO pcbi);

// Subclass the list-box of the combo box.
// NT4 SP6 and higher, or Win98 and higher have a GetComboBoxInfo
// function which will give us info such as the handle of the list
// so try and get a pointer to the function, and call it.
// If we can't use this function (or the function fails), we can't 
// subclass until we get a WM_CTLCOLOR message, so we need to force the
// combo to get the WM_CTLCOLOR, by temporarily showing the list.
// We only want to do this if we are case-sensitive, and we haven't already
// subclassed successfully
void CComboBoxCS::ForceListBoxSubclass(BOOL bCleanlyOnly/*=FALSE*/)
{
	if (! m_listbox.GetCaseSensitiveSearch() ||  // we don't need to subclass the list
			m_listbox.GetSafeHwnd() != NULL)  // already subclassed
		return;

	PFNGETCOMBOBOXINFO pfnGetComboBoxInfo = NULL;
	HMODULE hLibUser32 = LoadLibrary("USER32.DLL");
	if (hLibUser32 != NULL)
		pfnGetComboBoxInfo = (PFNGETCOMBOBOXINFO)GetProcAddress(hLibUser32, "GetComboBoxInfo");
	if (pfnGetComboBoxInfo != NULL)
	{
		COMBOBOXINFO cbi;
		ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
		cbi.cbSize = sizeof(COMBOBOXINFO);
		if (pfnGetComboBoxInfo(GetSafeHwnd(), &cbi))
			m_listbox.SubclassWindow(cbi.hwndList);
	}

	// GetComboBoxInfo function not available, or the call to it failed
	// so do it the messy way
	if (! bCleanlyOnly && m_listbox.GetSafeHwnd() == NULL)
	{
		// make windows paint the list, so we can subclass it
		// by showing the drop down, and then hiding it again
		m_bInForceListBoxSubclass = TRUE;
		SetRedraw(FALSE);
		ShowDropDown();	  // show the list - causes a WM_CTLCOLOR to be sent
		ShowDropDown(FALSE);  // hide the list
		SetRedraw(TRUE);
		// invalidate self
		Invalidate();
		m_bInForceListBoxSubclass = FALSE;
	}
}

void CComboBoxCS::OnDestroy() 
{
	if (m_listbox.GetSafeHwnd() != NULL)
		m_listbox.UnsubclassWindow();
	CComboBox::OnDestroy();
}

void CComboBoxCS::SetCaseSensitiveSearch(BOOL bCaseSensitive/*=TRUE*/)
{
	m_listbox.SetCaseSensitiveSearch(bCaseSensitive);
}

BOOL CComboBoxCS::GetCaseSensitiveSearch()
{
	return m_listbox.GetCaseSensitiveSearch();
}

/*static */ void CComboBoxCS::SetCaseSensitiveSearchDefault(BOOL bCaseSensitive/*=TRUE*/)
{
	CListBoxCS::SetCaseSensitiveSearchDefault(bCaseSensitive);
}

/*static */ BOOL CComboBoxCS::GetCaseSensitiveSearchDefault()
{
	return CListBoxCS::GetCaseSensitiveSearchDefault();
}

LRESULT CComboBoxCS::OnCBFindString(WPARAM wParam, LPARAM lParam)
{
	return FindString(wParam, lParam);
}

LRESULT CComboBoxCS::OnCBFindStringExact(WPARAM wParam, LPARAM lParam)
{
	return FindString(wParam, lParam, TRUE);
}

LRESULT CComboBoxCS::OnCBSelectString(WPARAM wParam, LPARAM lParam)
{
	int nIndex = FindString(wParam, lParam);
	if (nIndex != -1)
		SetCurSel(nIndex);
	return nIndex;
}

void CComboBoxCS::OnEditchange() 
{
	// in change - have a go at subclassing now, but only do it cleanly
	ForceListBoxSubclass(TRUE);
}

void CComboBoxCS::OnDropdown() 
{
	// dropping list - if not subclassed by now then we need to
	// fiddle the auto-selection of the current text
	if (! m_bInDropDown && m_listbox.GetSafeHwnd() == NULL)
	{
		m_bInDropDown = TRUE;	// prevent recursion if needing to subclass the messy way
		CString sText;
		GetWindowText(sText);
		SetCurSel(FindStringExact(-1, sText));	// this will cause subclassing
		SetWindowText(sText);
		SetEditSel(sText.GetLength(), -1);
		m_bInDropDown = FALSE;
	}
}

LRESULT CComboBoxCS::FindString(WPARAM wParam, LPARAM lParam, BOOL bExact)
{
	// make sure the list box is subclassed before the FindString
	// don't do it if the search string is empty, so as to
	// reduce the chance of us needing to do it this way.
	if (lParam != 0 && (*(LPCSTR)lParam) != '\0')   // string not null or empty
		ForceListBoxSubclass();

	BOOL bCaseSensitive = m_listbox.GetCaseSensitiveSearch();

	if (bCaseSensitive && m_listbox.GetSafeHwnd() != NULL)
		return m_listbox.SendMessage(bExact ? LB_FINDSTRINGEXACT : 
						LB_FINDSTRING, wParam, lParam);

	int nIndex = DefWindowProc(bExact ? CB_FINDSTRINGEXACT : CB_FINDSTRING, wParam, lParam);
	if (bCaseSensitive && nIndex != CB_ERR)
	{
		// combo thinks it's found - but is it really?
		CString sSearch((LPCSTR)lParam), sFound;
		GetLBText(nIndex, sFound);
		if (sFound != sSearch)
		{
			nIndex = CB_ERR;
			// carry on searching from here
		}
	}
	return nIndex;
}

/////////////////////////////////////////////////////////////////////////////
// CListBoxCS

/*static*/ BOOL CListBoxCS::ms_bCaseSensitiveSearchDefault = FALSE;

CListBoxCS::CListBoxCS()
{
	m_bCaseSensitiveSearch = ms_bCaseSensitiveSearchDefault;
}

CListBoxCS::~CListBoxCS()
{
}

BEGIN_MESSAGE_MAP(CListBoxCS, CListBox)
	//{{AFX_MSG_MAP(CListBoxCS)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(LB_FINDSTRING, OnLBFindString)
	ON_MESSAGE(LB_FINDSTRINGEXACT, OnLBFindStringExact)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCS message handlers

// Finds string in a listbox (case-sensitive search)
// return as for LB_FINDSTRING with CB_ERR if not found
int CListBoxCS::FindString(int iStart, LPCSTR lpszString, BOOL bExact)
{
	BOOL bFound = FALSE;

	if (lpszString == NULL)
		return LB_ERR;

	int i=0;
	int nItems = GetCount();
	for (i = iStart + 1; i < nItems; i++)	// search from iStart to end
	{
		if (GetTextLen(i) == LB_ERR)
			return CB_ERR;
		CString sBuf;
		GetText(i, sBuf);
		CString sString = lpszString;
		if (! m_bCaseSensitiveSearch)
		{
			sBuf.MakeUpper();
			sString.MakeUpper();
		}
		LPCSTR lpszBuf = sBuf.GetBuffer(0);
		LPSTR lpszStrstr = (LPSTR)strstr(lpszBuf, sString);
		if (sString.IsEmpty())
			lpszStrstr = NULL;
		if ((bExact && strcmp(sBuf, sString) == 0)
				|| (! bExact && lpszStrstr == lpszBuf))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)		// continue search from start
	{
		for (i = 0; i <= iStart; i++)
		{
			if (GetTextLen(i) == LB_ERR)
				return CB_ERR;
			CString sBuf;
			GetText(i, sBuf);
			CString sString = lpszString;
			if (! m_bCaseSensitiveSearch)
			{
				sBuf.MakeUpper();
				sString.MakeUpper();
			}
			LPCSTR lpszBuf = sBuf.GetBuffer(0);
			LPSTR lpszStrstr = (LPSTR)strstr(lpszBuf, sString);
			if (sString.IsEmpty())
				lpszStrstr = NULL;
			if ((bExact && strcmp(sBuf, sString) == 0)
					|| (! bExact && lpszStrstr == lpszBuf))
			{
				bFound = TRUE;
				break;	// found
			}
		}
	}
	return (bFound ? i : LB_ERR);
}

LRESULT CListBoxCS::OnLBFindString(WPARAM wParam, LPARAM lParam)
{
	// wParam is index, lParam is text to find
	return FindString(wParam, (LPCSTR)lParam, FALSE);
}

LRESULT CListBoxCS::OnLBFindStringExact(WPARAM wParam, LPARAM lParam)
{
	// wParam is index, lParam is text to find
	return FindString(wParam, (LPCSTR)lParam, TRUE);
}
