// UsefulSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MfcSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//The mfcCSplitterWnd class primary extened use is for Replacing Views.
//In the function mfcCSplitterWnd::ReplaceView pDoc->AutoDelete has to be changed to prevent
//the one and only doc object from being deleted.
/////////////////////////////////////////////////////////////////////////////
// mfcCSplitterWnd

mfcCSplitterWnd::mfcCSplitterWnd()
{
	m_bBarLocked=FALSE;
}

mfcCSplitterWnd::~mfcCSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(mfcCSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(mfcCSplitterWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// mfcCSplitterWnd message handlers

void mfcCSplitterWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bBarLocked)
		CSplitterWnd::OnLButtonDown(nFlags, point);
}

void mfcCSplitterWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bBarLocked)
		CSplitterWnd::OnMouseMove(nFlags, point);
	else
		CWnd::OnMouseMove(nFlags, point);
}

BOOL mfcCSplitterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (!m_bBarLocked)
		return CWnd::OnSetCursor(pWnd, nHitTest, message);

	return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL mfcCSplitterWnd::ReplaceView(int row, int col,CRuntimeClass * pViewClass,SIZE size)
{
  CCreateContext context;
  BOOL bSetActive;
	       
   
  if ((GetPane(row,col)->IsKindOf(pViewClass))==TRUE)
       return FALSE;
				    
   
   // Get pointer to CDocument object so that it can be used in the creation 
   // process of the new view
   CDocument * pDoc= ((CView *)GetPane(row,col))->GetDocument();
   CView * pActiveView=GetParentFrame()->GetActiveView();
   if (pActiveView==NULL || pActiveView==GetPane(row,col))
      bSetActive=TRUE;
   else
      bSetActive=FALSE;

    // set flag so that document will not be deleted when view is destroyed
	pDoc->m_bAutoDelete=FALSE;    
    // Delete existing view 
   ((CView *) GetPane(row,col))->DestroyWindow();
    // set flag back to default 
    pDoc->m_bAutoDelete=TRUE;
 
    // Create new view                      
   
   context.m_pNewViewClass=pViewClass;
   context.m_pCurrentDoc=pDoc;
   context.m_pNewDocTemplate=NULL;
   context.m_pLastView=NULL;
   context.m_pCurrentFrame=NULL;
   
   CreateView(row,col,pViewClass,size, &context);
   
   CView * pNewView= (CView *)GetPane(row,col);
   
   if (bSetActive==TRUE)
      GetParentFrame()->SetActiveView(pNewView);
   
   RecalcLayout(); 
   GetPane(row,col)->SendMessage(WM_PAINT);
   
   return TRUE;
}
    
