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
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

    //not braw background brush
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
		CRect(0, 0, 640, 480),		    // const RECT& rect
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
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	this->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	this->DragAcceptFiles(true);

	return 0;
}
   
void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_ESCAPE:
		SendMessage(WM_CLOSE, 0, 0);		
		break;
	case VK_F1:
		MessageBox("This is a test message!", "Test");		
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
