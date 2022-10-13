#include <windows.h>


/* DataGrid definitions */
#define DG_MAXGRID          20
#define DG_MAXCOLUMN        1000
#define DG_MAXROW           32000
#define DGTA_LEFT           DT_LEFT
#define DGTA_CENTER         DT_CENTER
#define DGTA_RIGHT          DT_RIGHT
#define DGBGR_COLOR         RGB(255,255,255)
#define DGTXT_COLOR         RGB(0,0,0)
#define DGRONLY_COLOR       RGB(220,220,220)


/* DataGrid messages */
#define DGM_ITEMCHANGED          0x0001
#define DGM_ITEMTEXTCHANGED      0x0002
#define DGM_ITEMADDED            0x0003
#define DGM_ITEMREMOVED          0x0004
#define DGM_COLUMNRESIZED        0x0005
#define DGM_COLUMNCLICKED        0x0006
#define DGM_STARTSORTING         0x0007
#define DGM_ENDSORTING           0x0008


/* Custom callback procedure */
typedef int(CALLBACK *DGCOMPARE)(TCHAR* item1, TCHAR* item2, int column);


/* DataGrid enumerations */
enum DG_MASK {DG_TEXTEDIT, DG_TEXTALIGN, DG_TEXTHIGHLIGHT, DG_TEXTRONLY, DG_TEXTBGCOLOR};


/* DataGrid item information structure */
typedef struct _DG_ITEMINFO
{
    DG_MASK  dgMask;
    int      dgItem;
    int      dgSubitem;
    TCHAR*   dgText;
	int      dgTextLen;
    int      dgTextAlign;
    bool     dgSelected;
    bool     dgReadOnly;
    COLORREF dgBgColor;
} DG_ITEMINFO;





class CDataGrid
{
private:
        /* DataGrid members */
        HWND     m_hWnd;
        HWND     m_hParentWnd;
        RECT     m_DGRect;
        
public:
       /* Basic DataGrid methods */
       CDataGrid();
       virtual ~CDataGrid();
       BOOL Create(RECT wndRect, HWND hParent, int numCols);
       void Resize();
       void Update();
       HWND GetWindowHandle();


	   //
	   void AddNewCol  ( const TCHAR* text, int col, int width, int align=DGTA_CENTER );
	   void AddRow     ( const TCHAR* text, int align=DGTA_CENTER );
	   void SetCellText( const TCHAR* text, int col, int row, int align=DGTA_CENTER, bool readOnly=false );
	   
	   

       
       /* General DataGrid methods */
       BOOL InsertItem(TCHAR* itemText, int textAlign);
       BOOL RemoveItem(int row);
       void RemoveAllItems();
       void EnableSort(BOOL enable);
       void EnableEdit(BOOL enable);
       void EnableResize(BOOL enable);
       void EnableGrid(BOOL enable);
	   void EnsureVisible(int row, int column);
	   void SelectItem(int row, int column);
       int  GetResizedColumn();
       int  GetRowCount();
       int  GetSelectedRow();
       int  GetSelectedColumn();
       void SetCompareFunction(DGCOMPARE CompareFunction);
       
       /* DataGrid SET attribute methods */
       BOOL SetColumnInfo(int columnIndex, TCHAR* columnText, int columnWidth, int textAlign);
       void SetColumnTxtColor(COLORREF txtColor);
       void SetColumnFont(LOGFONT* lf);
       BOOL SetItemInfo(int rowIndex, int columnIndex, TCHAR* itemText, int textAlign, bool readOnly);
       BOOL SetItemInfo(DG_ITEMINFO* itemInfo);
       void SetItemBgColor(int rowIndex, COLORREF bgColor);
       void SetRowFont(LOGFONT* lf);
       void SetRowTxtColor(COLORREF txtColor);
       
       /* DataGrid GET attribute methods */
       COLORREF GetColumnTxtColor();
       void GetColumnFont(LOGFONT* lf);
       void GetItemInfo(DG_ITEMINFO* itemInfo);
       BOOL GetItemText(int rowIndex, int columnIndex, TCHAR* buffer, int buffer_size);
       void GetRowFont(LOGFONT* lf);
       COLORREF GetRowTxtColor();
};
