#pragma once
#include <afxcmn.h>
#include "mfc-txhelper.h"
// CTXSliderCtrl

class CTXSliderCtrl : public CSliderCtrl
{
    DECLARE_DYNAMIC(CTXSliderCtrl)

public:
    CTXSliderCtrl();
    virtual ~CTXSliderCtrl();


    void         SetStateResID( DWORD    nDragID,
                                DWORD    nBgID,
                                COLORREF MaskColor );

protected:
    DECLARE_MESSAGE_MAP()
    BOOL OnSliderDrawThumb( CDC* pDC,  CRect &rect, UINT nState);
    BOOL OnSliderDrawChannel( CDC* pDC,  CRect &rect, UINT nState);
    BOOL OnSliderDrawTics( CDC* pDC,  CRect &rect, UINT nState){ return TRUE;};

private:
    CTXHelper m_objTrans;
    DWORD    m_nDragID;
    DWORD    m_nBgID;
    CBitmap  m_bmpDrag;
    CBitmap  m_bmpBg;
    COLORREF m_MaskColor;
public:
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
protected:
    virtual void PreSubclassWindow();
public:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //afx_msg void OnDestroy();
    LRESULT OnNotifyTransparent( WPARAM wParam, LPARAM lParam);
};


