#pragma once


#include "mfc-txhelper.h"
#include "vector"
// CTXMarquee

class CTXMarquee: public CStatic
{
    DECLARE_DYNAMIC( CTXMarquee )

    COLORREF  m_TextColor;
    DWORD     m_nTimer;
    DWORD     m_nlastTime;
    DWORD     m_nElapse;
    POINT     m_TextPos;
    int       m_MoveDest;
    BOOL      m_bUpdate;
    UINT      m_EndCnt;


public:
    CTXMarquee();
    virtual ~CTXMarquee();


    void SetWindowText( LPCTSTR lpszString, COLORREF col = 0 );

    void SetResID( DWORD nResBGID, COLORREF MaskColor );

    void SetAlpha( DWORD AlphaVal = 255 );

    UINT GetEndCount(){ return m_EndCnt; }

    void Update();

protected:
    DECLARE_MESSAGE_MAP()

    CTXHelper m_objHelper;
    virtual void PreSubclassWindow();

    INT      m_nAlphaVal;
    COLORREF m_MaskColor;
    DWORD    m_nResBGID;
    CBitmap  m_bitmapBG;
    CTXHelper m_objTransparent;

public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd( CDC* pDC );

    afx_msg void OnTimer( UINT_PTR nIDEvent );
    afx_msg void OnDestroy();
};


