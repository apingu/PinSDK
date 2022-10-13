//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

#ifndef MFCDIALOG_H
#define MFCDIALOG_H

//#include "stdlib.h"
//#include "stdio.h"
#include <pf-def.h>
#include <vector>
#include <string>


///////////////////////////////////////////////////////////////////////////////////////
// dialog menu

void   mfcCreateDlg( UINT nID, CDialog* dlg, CWnd* pParentWnd );
BOOL   mfcTurnDlg( CDialog* dlg );
BOOL   mfcShowDlg( CDialog* dlg );
void   mfcHideDlg( CDialog* dlg );
BOOL   mfcIsDlgShow( CDialog* dlg );
BOOL   mfcIsWindowVisible( CWnd* wnd );
CPoint mfcGetDlgPos( CDialog* dlg );
void   mfcMoveDlg( CDialog* dlg, CPoint pt );

// open select folder dialog
TCHAR* mfcOpenFolderBrowseDlg( PTSTR outpath, HWND hwndOwner, PCTSTR beginfolder = NULL, UINT ulFlags = ~BIF_NONEWFOLDERBUTTON );

///////////////////////////////////////////////////////////////////////////////////////
// Drag file
// 取得拖曳的檔案
int    mfcGetDragFiles( _OUT _tcsvector& files, _IN HDROP hDropInfo );

///////////////////////////////////////////////////////////////////////////////////////
// CMFCButton
// 
// 設定 文字 與背景顏色
void   MFCSetButtonColor( _IN CMFCButton* button, _IN COLORREF text_color, _IN COLORREF face_color );

///////////////////////////////////////////////////////////////////////////////////////
// sheet page
class mfcCSheet
{
    CWnd* m_parent;
    CSize m_size;
    CPoint m_situs;
    INT m_selectpage;
    std::vector<UINT> m_iDlgVec;
    std::vector<CDialog*> m_cDlgVec;

public:

    mfcCSheet();
    ~mfcCSheet();

    UINT PageSize( void );
    void ClearAllPage( void );
    void Create( CRect rt, CWnd* parent = 0 );
    void SetRect( CRect rt );
    void PageInsert( UINT nID, CDialog* dlg );
    void PageRemove( UINT nID );
    void PageRemove( CDialog* dlg );
    void PageOpen( UINT nID );
    void PageClose( UINT nID );
    CDialog* CurPage( void );
};


///////////////////////////////////////////////////////////////////////////////////////
// popup menu
class MfcCPopupMenu
{
    CWnd*  m_pParent;

    CMenu  m_menu;
    CMenu* m_subMenu;

public:

    MfcCPopupMenu( UINT nId, UINT sub, CWnd* pParent );
    ~MfcCPopupMenu();

    BOOL SetBitmap( UINT nSubId,
        const CBitmap* pUnchecked,
        const CBitmap* pChecked );

    BOOL AddBitmapItem( UINT nIDNewItem,
        const CBitmap* pBitmap );

    void Popup( CPoint pos, bool aschild = true );
};

////////////////////////////////////////////////////////////////////////////
// ImageList
void  mfcImageListCreate( CImageList* imagelist,
    UINT nID,
    int cx, int cy,
    COLORREF crMask,
    UINT nFlags = ILC_MASK | ILC_COLOR24 );


HBITMAP mfcImageListGetBitmap( CImageList* imagelist, int nIndex );

///////////////////////////////////////////////////////////////////////////////////////
//
class MfcCtrlResize
{
    CWnd*               m_pMainWnd;
    std::vector< int>   m_CtrlhWnd;
    std::vector<CRect>  m_CtrlRect;

public:
    MfcCtrlResize( CWnd* pMainWnd );
    ~MfcCtrlResize();

    void AddDlgItem( int nID );
    void DelDlgItem( int nID );

    //void FitToBmp( int nID );

    // put on OnSizing
    void Resizing();
};

///////////////////////////////////////////////////////////////////////////////////////
// menu
//1. CMainFrame* pMainWnd = (CMainFrame*) AfxGetMainWnd();
//2. CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;

//1. CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd; 
//   CMyAppView* pView    = (CMyAppView*) pMainWnd->GetActiveView();

//1. CFrameWnd* pMainFrm = (CFrameWnd*) AfxGetApp()->m_pMainWnd;
//   CDocument* pCurDoc = (CFrameWnd*) pMainFrm ->GetActiveDocument();


//CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
// Get the active MDI child window.
//CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();

//  child get view
// MDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
// or CMDIChildWnd *pChild = pFrame->MDIGetActive();
// pChild->GetActiveView();

//statue message
//GetParentFrame()->SetMessageText(buff);


#endif