// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MyApp.h"

#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP

    // will have to define some message handlers for our controls.
    ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)
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

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
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


    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);



    return 0;   
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
/*  cs.lpszClass = AfxRegisterWndClass(
                    CS_DBLCLKS,                            // if you need double-clicks
                    NULL,                                  // no cursor (use default)
                    NULL,                                  // no background brush
                    AfxGetApp()->LoadIcon(IDR_MAINFRAME)); // app icon

    ASSERT(cs.lpszClass);
*/
    return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnFileNew()
{
    // create a child window
    CChildFrame *pWnd = new CChildFrame;

    if (!pWnd->Create(_T("Untitled"),
        WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, rectDefault, this))
        return;
    // the default PostNcDestroy handler will delete this object when destroyed

    // initilize this child window and map
    pWnd->SetWindowText(_T("Untitled"));
}

void CMainFrame::OnFileOpen() 
{
    // ask a filename to open
    CString     str;
    CFileDialog fileDlg(TRUE, NULL, NULL, \
                        OFN_HIDEREADONLY, \
		                "All Files (*.*)|*.*||");

	fileDlg.DoModal();

	str = fileDlg.GetPathName();

	if (str.IsEmpty()) 
        return;

    // create a child window
	CChildFrame *pWnd = new CChildFrame;

    if (!pWnd->Create(_T(str),
		              WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, 
                      rectDefault, 
                      this))
		return;
	// the default PostNcDestroy handler will delete this object when destroyed

    // initilize this child window and load the map
    pWnd->SetWindowText(_T(str));    
}

void CMainFrame::OnToolbarDropDown(NMTOOLBAR *pnmtb, LRESULT *plr)
{
    CWnd *pWnd;
    UINT nID;

    if(pnmtb->iItem == ID_FILE_OPEN) 
    {
        pWnd = &m_wndToolBar;
        nID = IDR_POPUP;
    }
    else 
        return;

    // load and display popup menu
    CMenu menu;
    menu.LoadMenu(nID);
    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup);

    CRect rc;
    pWnd->SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
    pWnd->ClientToScreen(&rc);

    pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
                           rc.left, rc.bottom, this, &rc);
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{
    CRect rect;
    DWORD dw;
    UINT  n;
    
    // get MFC to adjust the dimensions of all docked ToolBars
    // so that GetWindowRect will be accurate
    RecalcLayout(TRUE);
    
    LeftOf->GetWindowRect(&rect);
    rect.OffsetRect(1,0);
    dw=LeftOf->GetBarStyle();
    n = 0;
    n = (dw&CBRS_ALIGN_TOP)              ? AFX_IDW_DOCKBAR_TOP      : n;
    n = (dw&CBRS_ALIGN_BOTTOM   && n==0) ? AFX_IDW_DOCKBAR_BOTTOM   : n;
    n = (dw&CBRS_ALIGN_LEFT     && n==0) ? AFX_IDW_DOCKBAR_LEFT     : n;
    n = (dw&CBRS_ALIGN_RIGHT    && n==0) ? AFX_IDW_DOCKBAR_RIGHT    : n;
    
    // When we take the default parameters on rect, DockControlBar will dock
    // each Toolbar on a seperate line. By calculating a rectangle, we in effect
    // are simulating a Toolbar being dragged to that location and docked.
    DockControlBar(Bar,n,&rect);
}

