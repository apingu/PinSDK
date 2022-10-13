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
	
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

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
		"MiniMFC Application",			// LPCTSTR lpszWindowName
		WS_OVERLAPPEDWINDOW,			// DWORD dwStyle
		rectDefault,					// const RECT& rect
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

    // Create a status bar
    if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
    }
	
    // Create a tool bar
    if (!m_wndToolBar.Create(this) ||
        !m_wndToolBar.LoadBitmap(IDB_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

   	// Set the toolbar to show only partial commmand list
	m_wndToolBar.SetButtons(NULL, 5);
	m_wndToolBar.SetButtonInfo(0, ID_FILE_NEW, TBBS_BUTTON, 0);
    m_wndToolBar.SetButtonInfo(1, ID_FILE_OPEN,TBBS_BUTTON, 1);
	m_wndToolBar.SetButtonInfo(2, ID_FILE_SAVE,TBBS_BUTTON, 2);
	m_wndToolBar.SetButtonInfo(3, ID_SEPARATOR,TBBS_SEPARATOR, 6);
	m_wndToolBar.SetButtonInfo(4, ID_APP_ABOUT,TBBS_BUTTON, 7);

	// invalidate the call update handlers before painting
	m_wndToolBar.Invalidate();

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