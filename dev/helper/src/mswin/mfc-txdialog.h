// Leading DialogDlg.h : header file
//
// ****Modelless dialog 一定要在 mainidele 裡面 PeekMessage  這樣畫面才會被更新 請參考 Window 的 MainIdle
//     類別似乎得是全區域的 才行
//     如果要自行繪圖 請繼承OnMyPaint
// 01) 在 建構式的地方 接上 TX的建構式
// 02) OnInitDialog 傳入 mask color
// 03) OnMyPaint 一定要繼承 並且呼叫CTXDialog的 OnMyPaint

#pragma once
#include "afxwin.h"
#include "mfc-txstatic.h"
#include "mfc-txbutton.h"
#include "mfc-txmarquee.h"
#include "mfc-txsliderctrl.h"


// CUpdateDlg dialog
class CTXDialog : public CDialog
{
    // Construction

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


public:
    // 不好意思 這邊要連背景一起傳進來
    explicit CTXDialog(UINT nIDTemplate, UINT nIDResource, CWnd* pParentWnd = NULL);
    virtual ~CTXDialog();

    

    virtual BOOL    Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
    void            CloseWindow();
    virtual BOOL    DestroyWindow();

    virtual void    OnMyPaint();

    INT             ModelUpdate();

    void            SetDragRange(RECT rt);
    void            SetDragable(BOOL bd);

    void            SetFullScreen(BOOL fs);
    static BOOL     GetFullScreen(void);

    void            ForceWindowToForeground();

    

    // Implementation
protected:

    DECLARE_MESSAGE_MAP()

    // Generated message map functions
    virtual BOOL    OnInitDialog(COLORREF MaskColor);
    afx_msg void    OnDestroy();
    afx_msg void    OnPaint();
    afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void    OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void    OnMouseMove(UINT nFlags, CPoint point);

    // 以下與 OnPaint 有關 一但 OnPaint 被繼承後
    // 以下FUNCTION 將不會被喚起 因此 儘量不要繼承
    // 若有繼承 也請重複 繼承 下列FUNCTION
    afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    void            BuildBkDC();
    LRESULT         OnTransaprentBk( WPARAM wParam, LPARAM lParam);

    //LRESULT       WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

private:

    INT      m_nID;

    int      m_nBkResID;
    CPoint   m_pntMouse;
    DWORD    m_dwFlags;
    INT      m_dwWidth;
    INT      m_dwHeight;
    COLORREF m_MaskColor;

    //
    CDC*     m_pMemDC;
    CBitmap* m_pOldBitmap;
    CBitmap* m_pBitmapMem;
    CBitmap  m_BkBmp;

    bool     m_bDragable;
    RECT     m_DragRange;  // 允許拖拉的範圍

    // 全螢幕Dialog
    static BOOL                s_bFullScreen;
    static WINDOWPLACEMENT     s_OldWndPlacement;

};