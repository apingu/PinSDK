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
#include "stdafx.h"
#include "MfcListCtrl.h"



/*
////////////////////////////////////////////////////////////////////////////
BOOL mfcCListCtrl::Create(CWnd* pParentWnd, UINT nID)
{
	//RECT rect;
	//GetDlgItem(nID)->GetWindowRect(&rect);
	Create(pParentWnd, nID);
}
*/

////////////////////////////////////////////////////////////////////////////
void mfcCListCtrl::AddExStyle(DWORD dwNewStyle)
{
    DWORD dwStyle   = SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);

    dwStyle |= dwNewStyle;

    SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
}


////////////////////////////////////////////////////////////////////////////
int mfcCListCtrl::GetCurSelected()
{
	int i = 0;
    for(i = 0;i < GetItemCount();i++)
        if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            break;

    if(i == GetItemCount())
        return -1;

    return i;
}


////////////////////////////////////////////////////////////////////////////
int mfcCListCtrl::GetCurSelectedCount()
{
    int count   = 0;
    for(int i = 0;i < GetItemCount();i++)
        if(GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            count ++;

    return count;
}


////////////////////////////////////////////////////////////////////////////
void mfcCListCtrl::SetCurSelected(int nSelect)
{
    SetItemState(-1, ~LVIS_SELECTED, LVIS_SELECTED);

    if(nSelect >= 0)
    {
        SetItemState(nSelect, LVIS_SELECTED | LVIS_FOCUSED, \
                              LVIS_SELECTED | LVIS_FOCUSED);
        EnsureVisible(nSelect, FALSE);
    }
}

////////////////////////////////////////////////////////////////////////////
void mfcCListCtrl::SetAllSel(void)
{
	for(UINT i=0; i<GetItemCount(); i++)
	{
	    SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

}


////////////////////////////////////////////////////////////////////////////
bool mfcCListCtrl::AddFileListItem(int nItem,
                                     char lpszItem[ _MAX_ITEMS ][ _MAX_LENGTH ],
                                     int nColumnCount)
{
    char szText[ _MAX_PATH ];
    int i;
    LV_ITEM lvitem;

    // Set main item
    wsprintf(szText, "%d", nItem);

    lvitem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvitem.iItem = nItem;        // zero-based index of the item
    lvitem.iSubItem = 0;            // zero-based index of the subitem
    lvitem.pszText = lpszItem[ 0 ];  // the item text 
    lvitem.iImage = 0;            // index of the list view item icon
    lvitem.lParam = LVM_SORTITEMS ;

    int nParentItemKey  = InsertItem(&lvitem);

    // Set sub items
    for(i = 1;i < nColumnCount;i++)
    {
        lvitem.mask = LVIF_TEXT;
        lvitem.iItem = nParentItemKey;
        lvitem.iSubItem = i;
        lvitem.pszText = lpszItem[ i ];
        if(!SetItem(&lvitem))
            return false;
    }
    return true;
}
