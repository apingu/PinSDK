// Main.cpp : implementation file
//

#include "stdafx.h"
#include "myapp.h"
#include "AboutDlg.h"
#include "DlgMain.h"
#include <vector>
#include <io.h>
//#include <process.h>



/////////////////////////////////////////////////////////////////////////////
// CDlgMain dialog


CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMain)
	//}}AFX_DATA_INIT
}


void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMain)
	DDX_Control(pDX, IDC_APP_ABOUT, m_appbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMain, CDialog)
	//{{AFX_MSG_MAP(CDlgMain)
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMain message handlers

void CDlgMain::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
	this->GetParent()->SendMessage(WM_CLOSE, 0, 0);
}


void CDlgMain::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	CRect dlg_rect(x, y, x +1, y + 1);
	this->GetParent()->MoveWindow(&dlg_rect);
	this->GetFocus();	
}


void CDlgMain::OnAppAbout() 
{
	// TODO: Add your control notification handler code here
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CDlgMain::OnDropFiles(HDROP hDropInfo)
{
	UINT nNumFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);				
	
    char  szPath[_MAX_PATH];
    //DWORD dwAttrib;
    
	for (UINT count = 0; count < nNumFiles; count++)
	{
		::DragQueryFile(hDropInfo, count, szPath, sizeof(szPath));
 	}
	
    ::DragFinish(hDropInfo);


	InvalidateRect(NULL, false);
	return;		
}
