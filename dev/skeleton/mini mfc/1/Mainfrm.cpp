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
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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
	
    cs.lpszClass = AfxRegisterWndClass(
	    CS_DBLCLKS,									// UINT nClassStyle
    	AfxGetApp()->LoadStandardCursor(IDC_ARROW),	// HCURSOR hCursor
	    NULL,										// HBRUSH hbrBackground
    	AfxGetApp()->LoadIcon(IDR_MAINFRAME)		// HICON hIcon
	    );
	    
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
