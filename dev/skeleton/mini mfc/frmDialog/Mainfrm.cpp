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
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_MOVE()
	ON_WM_SIZE()
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


	/*	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);
	this->DragAcceptFiles(true);

	
	if(!::IsWindow(m_wndMain.m_hWnd))
		m_wndMain.Create(IDD_MAIN_DIALOG, this);
	m_wndMain.ShowWindow(SW_SHOWNORMAL);
	RECT wndrect;
	m_wndMain.GetWindowRect( &wndrect );
	AdjustWindowRectEx(&wndrect, lpCreateStruct->style, FALSE, lpCreateStruct->dwExStyle);
	MoveWindow( &wndrect );


	return 0;
}

void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	
	// TODO: Add your message handler code here
	RECT dlg_rect;
	this->GetClientRect(&dlg_rect);
	this->ClientToScreen(&dlg_rect);

	RECT status_rect;
	m_wndStatusBar.GetClientRect(&status_rect);

	//dlg_rect.top = dlg_rect.top + 24;
	dlg_rect.bottom = dlg_rect.bottom - (status_rect.bottom-status_rect.top);

	m_wndMain.MoveWindow(&dlg_rect);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	RECT dlg_rect;
	this->GetClientRect(&dlg_rect);
	this->ClientToScreen(&dlg_rect);

	RECT status_rect;
	m_wndStatusBar.GetClientRect(&status_rect);

	//dlg_rect.top = dlg_rect.top + 24;
	//dlg_rect.top = dlg_rect.top;// + 24;
	dlg_rect.bottom = dlg_rect.bottom - (status_rect.bottom-status_rect.top);
	m_wndMain.MoveWindow(&dlg_rect);
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

void CMainFrame::OnFileOpen() 
{
	// TODO: Add your command handler code here
	// ask a filename to open
    CString     str;
    CFileDialog fileDlg(TRUE, NULL, NULL, \
                        OFN_HIDEREADONLY, \
		                "All Files (*.*)|*.*||");

	fileDlg.DoModal();

	str = fileDlg.GetPathName();

	if (str.IsEmpty()) 
        return;
}


void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFrameWnd::OnDropFiles(hDropInfo);
}
