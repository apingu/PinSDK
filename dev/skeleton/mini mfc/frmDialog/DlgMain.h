#if !defined(AFX_DLGMAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_)
#define AFX_DLGMAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Main.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMain dialog

class CDlgMain : public CDialog
{
// Construction
public:
	CDlgMain(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMain)
	enum { IDD = IDD_MAIN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void OnDropFiles(HDROP hDropInfo);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMain)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_)
