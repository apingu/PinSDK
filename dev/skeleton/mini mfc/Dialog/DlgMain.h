#if !defined(AFX_MAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_)
#define AFX_MAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_

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
	enum { IDD = IDD_MAINDIALOG };
	CButton	m_appbout;
	//}}AFX_DATA

	void OnDropFiles(HDROP hDropInfo);

	//CPFile     m_cfgfile;

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
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RefreshLoadFile(void);
	void RefreshConvFile(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAIN_H__59501939_A293_4C9F_89BA_3E6A71DA147A__INCLUDED_)
