//
//
//
//
//
//
//
//    set Owner DrawTrue
//
//
#pragma once


#include "mfc-txhelper.h"
#include "vector"
// CTXButton

class CTXButton : public CButton
{
    DECLARE_DYNAMIC(CTXButton)


public:
    CTXButton();
    virtual ~CTXButton();

protected:
    DECLARE_MESSAGE_MAP()
public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual void PreSubclassWindow();


    // 設定各個狀態 要使用的資源圖檔
    // 請在DIALOG的建構中呼叫
    void         SetStateResID( DWORD    nNorlID,
                                DWORD    nOverID,
                                DWORD    nDownID,
                                DWORD    nDisbID,
                                COLORREF MaskColor );

    void         SetStateResID( CBitmap& NorlBmp,
                                CBitmap& OverBmp,
                                CBitmap& DownBmp,
                                CBitmap& DisbBmp,
                                COLORREF MaskColor );


    // 設定各個狀態的顏色
    void         SetStateTextColor( COLORREF    NorlColor,
                                    COLORREF    OverColor,
                                    COLORREF    DownColor,
                                    COLORREF    DisbColor);

    void         SetTextPosition( CPoint point, UINT align );

    // 大小 (120 = 12) 與 字型 "" 系統自訂
    void         SetFont( int size, const TCHAR* type );

    // 背景透明色
    void         SetAlpha ( DWORD AlphaVal=255 );

    // 設定動畫模式播放的間隔時間
    void         SetAnimTime ( DWORD mstime );


    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    LRESULT      OnMouseLeave(WPARAM wParam, LPARAM lParam);


    // 假裝是一個CheckBox
    void         SetCheckStyle(bool f);

    BOOL         GetCheck();
    void         SetCheck(BOOL c);

    // 如果滑鼠正在滑過 傳回TRUE
    BOOL         GetOverTracking();

private:

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //afx_msg void OnDestroy();

    INT       m_nAlphaVal;

    COLORREF  m_MaskColor;
    DWORD     m_nNorlID;
    DWORD     m_nOverID;
    DWORD     m_nDownID;
    DWORD     m_nDisbID;

    COLORREF  m_NorlTextColor;
    COLORREF  m_OverTextColor;
    COLORREF  m_DownTextColor;
    COLORREF  m_DisbTextColor;

    CBitmap   m_bitmapNor;
    CBitmap   m_bitmapOver;
    CBitmap   m_bitmapDown;
    CBitmap   m_bitmapDisb;

    CPoint    m_TextPosition;
    UINT      m_TextAlign;
    int       m_FontSize;
    CString   m_FontType;

    INT       m_dwWidth;
    INT       m_dwHeight;

    BOOL      m_bTracking;
    CTXHelper m_objTransparent;
    bool      m_bCheckStyle;
    bool      m_bChecked;
};


