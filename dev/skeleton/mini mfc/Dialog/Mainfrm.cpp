// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message map

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
    ON_WM_KEYUP()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{	
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;

//	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Operations

void CMainFrame::Initialize()
{	
}

BOOL CMainFrame::Create()
{
	return CFrameWnd::Create(
		NULL,							// LPCTSTR lpszClassName
		"Application",			        // LPCTSTR lpszWindowName
		WS_OVERLAPPEDWINDOW,			// DWORD dwStyle
		CRect(100, 100, 500, 500),		    // const RECT& rect
		NULL,							// CWnd* pParentWnd
		MAKEINTRESOURCE(IDR_MAINFRAME),	// LPCTSTR lpszMenuName
		0,								// DWORD dwExStyle
		NULL							// CCreateContext* pContext
        );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable

	this->DragAcceptFiles(true);

	if(!::IsWindow(m_wndMain.m_hWnd))
		m_wndMain.Create(IDD_MAINDIALOG, this);
	m_wndMain.ShowWindow(SW_SHOWNORMAL);
	CRect rt;
	m_wndMain.GetWindowRect(&rt);
	rt.right = rt.left + 1;
	rt.bottom = rt.top + 1;
	this->MoveWindow(&rt);
	//m_wndMain.GetFocus();

	return 0;
}


void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_ESCAPE:
		if(MessageBox("Do you want to quit ?", "Quit", MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
			SendMessage(WM_CLOSE, 0, 0);		
		break;
	default:
		break;
	}
}



void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	m_wndMain.OnDropFiles(hDropInfo) ;
	
	//CFrameWnd::OnDropFiles(hDropInfo);
}
