// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "MyApp.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CChildFrame)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CMenu CChildFrame::menu;        // menu for all child windows
int   CChildFrame::count = 0;   // count of all child windows

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
    count++;
}

CChildFrame::~CChildFrame()
{
    count--;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION 
        | WS_SYSMENU | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX 
        | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_HSCROLL | WS_VSCROLL;

    return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CChildFrame::Create(LPCTSTR szTitle, 
    LONG style           /* =0 */,
    const RECT& rect     /* =rectDefault */,
    CMDIFrameWnd* parent /* =NULL */)
{
    // Setup the shared menu
    if (menu.m_hMenu == NULL)
        menu.LoadMenu(IDR_CHILDFRAME);

    m_hMenuShared = menu.m_hMenu;

    // Register a custom WndClass and create a window.
    // This must be done because CChildFrame has a custom icon.

    LPCTSTR lpszHelloClass = AfxRegisterWndClass(
            CS_HREDRAW | CS_VREDRAW,
            LoadCursor(NULL, IDC_ARROW),
            (HBRUSH) 0,
            LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_CHILDFRAME)));

    ASSERT(lpszHelloClass);

    return CMDIChildWnd::Create(lpszHelloClass, szTitle, style, rect, parent);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CMDIChildWnd::OnMouseMove(nFlags, point);
}
