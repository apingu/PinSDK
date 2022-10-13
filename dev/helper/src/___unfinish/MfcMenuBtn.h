#ifndef MFCMENUBTN_H
#define MFCMENUBTN_H


#include "MfcSXButton.h"


class MfcCMenuBtn : public CSXButton
{
public:

    BOOL SetSubMenuBitmap( UINT nID,UINT nCheckID,UINT nUnchcekID );

    // Generated message map functions
protected:
    //{{AFX_MSG(CWndCombo)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};



#endif