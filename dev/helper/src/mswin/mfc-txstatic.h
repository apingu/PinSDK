#pragma once
#include "mfc-txhelper.h"

// CTXStatic

class CTXStatic : public CStatic
{
    DECLARE_DYNAMIC(CTXStatic)

    COLORREF  m_TextColor;
public:
    CTXStatic();
    virtual ~CTXStatic();

    void SetWindowText(LPCTSTR lpszString, COLORREF col=0);

    void SetResID( DWORD nResBGID, COLORREF MaskColor );

    void SetAlpha ( DWORD AlphaVal=255 );

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
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


