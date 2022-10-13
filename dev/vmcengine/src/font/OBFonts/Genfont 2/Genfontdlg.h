// GenFontDlg.h : header file
//

#ifndef _GENFONTDLG_H_
#define _GENFONTDLG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include  "VirtualPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGenFontDlg dialog

class CGenFontDlg : public CDialog
{
// Construction
public:
	CGenFontDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGenFontDlg)
	enum { IDD = IDD_GENFONT_DIALOG };
	CButton     m_chkSecCharset;
	CEdit		m_edGraspWidth;
	CEdit		m_edGraspHeight;
	CButton		m_btnGrasp;
	CButton		m_chkUnderLine;
	CButton		m_chkStrikeOut;
	CButton		m_chkItalic;
	CEdit		m_edWidth;
	CEdit		m_edWeight;
	CEdit		m_edQuality;
	CEdit		m_edPitch;
	CEdit		m_edOutPrecision;
	CEdit		m_edHeight;
	CEdit		m_edFaceName;
	CEdit		m_edCharSet;
	CEdit		m_edTest;
	CStatic		m_DrawArea;
	int			m_nGraspHeight;
	int			m_nGraspWidth;
	int			m_nWeight;
	int			m_nHeight;
	//}}AFX_DATA  
    CSpinButtonCtrl	 m_spnGraspWidth;
	CSpinButtonCtrl	 m_spnGraspHeight;


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenFontDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON        m_hIcon;

    CRect        m_rectPreview;

    LOGFONT      m_LogFont;

    CVirtualPage m_VPage;

    BOOL         m_bSmooth;

    int          m_nCharSet;

    char         m_szGrasp[3];
	WCHAR        m_szWGrasp[2];

    void ResetFont(void);
    int  ConvertLogUnitToPoint(int nLogUnitSize, CDC* pDC = NULL);

	int  GraspASCII(FILE *fp);
    int  GraspBIG5(FILE *fp, int extend);
    int  GraspGB2312(FILE *fp);
    int  GraspKSC(FILE *fp);
    int  GraspJIS(FILE *fp);
	int  GraspUCS_2(FILE *fp);

    void UpdateSmoothRadio(void);
    void UpdateCharSetRadio(void);

	// Generated message map functions
	//{{AFX_MSG(CGenFontDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFont();
	afx_msg void OnGrasp();
    afx_msg void OnDestroy();
	afx_msg void OnChangeEditHeight();
	afx_msg void OnChangeEditWeight();    
    afx_msg void OnRadioNonSmooth();
	afx_msg void OnRadioSmooth();
	afx_msg void OnRadioAscii();
	afx_msg void OnRadioBig5();
	afx_msg void OnRadioBig5ex();
	afx_msg void OnRadioGb();
	afx_msg void OnRadioKsc();
    afx_msg void OnRadioJis();
	afx_msg void OnRadioUCS_2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // _GENFONTDLG_H_
