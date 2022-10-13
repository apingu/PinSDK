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
#ifndef MFCLISTCTRL_H
#define MFCLISTCTRL_H

#include "StdAfx.h"

#ifndef _MAX_LENGTH
#define _MAX_LENGTH 512
#endif

#ifndef _MAX_ITEMS
#define _MAX_ITEMS  1024
#endif

class mfcCListCtrl : public CListCtrl
{

public:

	//BOOL                Create(CWnd* pParentWnd, UINT nID);

    bool    AddFileListItem(int nItem,
                             char lpszItem[ _MAX_ITEMS ][ _MAX_LENGTH ],
                             int nColumnCount);
    void    AddExStyle(DWORD dwNewStyle);
    int     GetCurSelectedCount(void);
    int     GetCurSelected(void);
	void    SetCurSelected(int nSelect);
	void    SetAllSel(void);

};

#endif// MFCLISTCTRL_H
