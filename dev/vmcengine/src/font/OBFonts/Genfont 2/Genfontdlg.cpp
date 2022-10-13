// GenFontDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GenFont.h"
#include "GenFontDlg.h"

#include "V.h"
#include "Grasp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenFontDlg dialog

CGenFontDlg::CGenFontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenFontDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenFontDlg)
	m_nGraspHeight = 1;
	m_nGraspWidth  = 1;
	m_nWeight = 0;
	m_nHeight = 0;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGenFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenFontDlg)
	DDX_Control(pDX, IDC_EDIT_GRASP_WIDTH, m_edGraspWidth);
	DDX_Control(pDX, IDC_EDIT_GRASP_HEIGHT, m_edGraspHeight);
	DDX_Control(pDX, IDC_GRASP, m_btnGrasp);
	DDX_Control(pDX, IDC_CHECK_UNDERLINE, m_chkUnderLine);
	DDX_Control(pDX, IDC_CHECK_STRIKEOUT, m_chkStrikeOut);
	DDX_Control(pDX, IDC_CHECK_ITALIC, m_chkItalic);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edWidth);
	DDX_Control(pDX, IDC_EDIT_WEIGHT, m_edWeight);
	DDX_Control(pDX, IDC_EDIT_QUALITY, m_edQuality);
	DDX_Control(pDX, IDC_EDIT_PITCH, m_edPitch);
	DDX_Control(pDX, IDC_EDIT_OUTPRECISION, m_edOutPrecision);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edHeight);
	DDX_Control(pDX, IDC_EDIT_FACENAME, m_edFaceName);
	DDX_Control(pDX, IDC_EDIT_CHARSET, m_edCharSet);
	DDX_Control(pDX, IDC_EDIT_TEST, m_edTest);
	DDX_Control(pDX, IDC_DRAW_AREA, m_DrawArea);
	DDX_Text(pDX, IDC_EDIT_GRASP_HEIGHT, m_nGraspHeight);
	DDV_MinMaxInt(pDX, m_nGraspHeight, 1, 64);
	DDX_Text(pDX, IDC_EDIT_GRASP_WIDTH, m_nGraspWidth);
	DDV_MinMaxInt(pDX, m_nGraspWidth, 1, 64);
	DDX_Text(pDX, IDC_EDIT_WEIGHT, m_nWeight);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGenFontDlg, CDialog)
	//{{AFX_MSG_MAP(CGenFontDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FONT, OnFont)
	ON_BN_CLICKED(IDC_GRASP, OnGrasp)
    ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_WEIGHT, OnChangeEditWeight)
    ON_BN_CLICKED(IDC_RADIO_NONSMOOTH, OnRadioNonSmooth)
	ON_BN_CLICKED(IDC_RADIO_SMOOTH, OnRadioSmooth)
	ON_BN_CLICKED(IDC_RADIO_SMOOTH, OnRadioSmooth)
	ON_BN_CLICKED(IDC_RADIO_ASCII, OnRadioAscii)
	ON_BN_CLICKED(IDC_RADIO_BIG5, OnRadioBig5)
	ON_BN_CLICKED(IDC_RADIO_BIG5EX, OnRadioBig5ex)
	ON_BN_CLICKED(IDC_RADIO_GB, OnRadioGb)
	ON_BN_CLICKED(IDC_RADIO_KSC, OnRadioKsc)
    ON_BN_CLICKED(IDC_RADIO_JIS, OnRadioJis)
	ON_BN_CLICKED(IDC_RADIO_UCS_2, OnRadioUCS_2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenFontDlg message handlers

BOOL CGenFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

    // Create a virtual page
    if (!m_VPage.Create(m_hWnd, 16))
    {
        AfxMessageBox("CGenFontDlg::OnInitDialog : Fail to create vpage", MB_ICONEXCLAMATION | MB_OK);
        
        return FALSE;   // fail to create vpage
    }

    // Map to super vpage format
    BYTE  *pBits  = m_VPage.GetBits();
    long   stride = m_VPage.GetStride();
    int    width  = m_VPage.GetWidth();
    int    height = m_VPage.GetHeight();

    ASSERT(pBits != NULL);

    assignvpage(0, (char *) pBits, stride, width, height);
    switchactvpage(0);	

    // Get preview holder
    m_DrawArea.GetWindowRect(&m_rectPreview);

    ScreenToClient(&m_rectPreview);

    vfillbox(0, 0, m_rectPreview.Width(), m_rectPreview.Height(), _RGB(128,128,128));

    // Set spin controls
    RECT  rect;
   
    m_spnGraspWidth.Create(WS_VISIBLE | WS_CHILD | 
        UDS_SETBUDDYINT | UDS_ALIGNRIGHT, rect, this, IDC_SPIN_GRASP_WIDTH);
    m_spnGraspHeight.Create(WS_VISIBLE | WS_CHILD |
        UDS_SETBUDDYINT | UDS_ALIGNRIGHT, rect, this, IDC_SPIN_GRASP_HEIGHT);

    m_spnGraspWidth.SetRange(1, 64);
    m_spnGraspWidth.SetBuddy(&m_edGraspWidth);	

    m_spnGraspHeight.SetRange(1, 64);
    m_spnGraspHeight.SetBuddy(&m_edGraspHeight);

    CSpinButtonCtrl  *spin;
    
    spin = (CSpinButtonCtrl *) GetDlgItem(IDC_SPIN_HEIGHT);
    spin->SetRange(-1, -64);

    spin = (CSpinButtonCtrl *) GetDlgItem(IDC_SPIN_WEIGHT);
    spin->SetRange(0, 900);

    // Set test string
    m_edTest.SetLimitText(64);

    // Set a defualt font
    m_LogFont.lfHeight         = -16;   // -12(Size9), -15(Size11), -16(Size12)               
    m_LogFont.lfWidth          = 0;                    
    m_LogFont.lfEscapement     = 0; 
    m_LogFont.lfOrientation    = 0;
    m_LogFont.lfWeight         = FW_NORMAL;                   
    m_LogFont.lfItalic         = FALSE;
    m_LogFont.lfUnderline      = FALSE;
    m_LogFont.lfStrikeOut      = FALSE;
    m_LogFont.lfCharSet        = DEFAULT_CHARSET;
    m_LogFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;    
    m_LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;       
    m_LogFont.lfQuality        = DEFAULT_QUALITY;  
    m_LogFont.lfPitchAndFamily = DEFAULT_PITCH;         
    strcpy(m_LogFont.lfFaceName, _T("細明體"));            
    
    // Set non smooth effect default
    OnRadioNonSmooth();

    // Set ASCII char set default
    OnRadioAscii();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGenFontDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC  dc(this);     // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int  cxIcon = GetSystemMetrics(SM_CXICON);
		int  cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;

		GetClientRect(&rect);

		int  x = (rect.Width()  - cxIcon + 1) / 2;
		int  y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        PAINTSTRUCT  ps;
        HDC          hdc;

        // Paint to this area
        hdc = ::BeginPaint(m_hWnd, &ps);

        m_VPage.Paint(hdc, m_rectPreview);

        ::EndPaint(m_hWnd, &ps);
	}
}

HCURSOR CGenFontDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGenFontDlg::OnFont() 
{
    CFontDialog  dlg;
    
    if (dlg.DoModal() != IDOK)
        return;

    dlg.GetCurrentFont(&m_LogFont);

    if (m_bSmooth)
        OnRadioSmooth();
    else
        OnRadioNonSmooth();
}

void CGenFontDlg::ResetFont(void)
{
	char  outstr[80]; 
    
    m_VPage.ChangeFont(m_LogFont);
 
    // Set dialog values
	m_chkUnderLine.SetCheck(m_LogFont.lfUnderline);
	m_chkStrikeOut.SetCheck(m_LogFont.lfStrikeOut);
	m_chkItalic.SetCheck(m_LogFont.lfItalic);

    sprintf(outstr, "%d", m_LogFont.lfWidth);
	m_edWidth.SetWindowText(outstr);
    sprintf(outstr, "%d", m_LogFont.lfWeight);
	m_edWeight.SetWindowText(outstr);
    sprintf(outstr, "%d", m_LogFont.lfQuality);
	m_edQuality.SetWindowText(outstr);
    sprintf(outstr, "%d", m_LogFont.lfPitchAndFamily);
	m_edPitch.SetWindowText(outstr);
    sprintf(outstr, "%d", m_LogFont.lfOutPrecision);
	m_edOutPrecision.SetWindowText(outstr);
    sprintf(outstr, "%d", m_LogFont.lfHeight);
	m_edHeight.SetWindowText(outstr);
	sprintf(outstr, "%s %d pt", m_LogFont.lfFaceName,
                             (ConvertLogUnitToPoint(m_LogFont.lfHeight)+5)/10);
    m_edFaceName.SetWindowText(outstr);
    
    sprintf(outstr, "%d", m_LogFont.lfCharSet);
	m_edCharSet.SetWindowText(outstr);

    // Set default width & height of grasping
	if( m_nCharSet ==6 )
	{
		wchar_t str[80];
		int  x, y, w, h;

		WCHAR SW[2] = { 0X0057, 0X0000 };
		WCHAR JW[2] = { 0X006A, 0X0000 };

		m_VPage.GetWTextSize(SW, x, y);      
		m_VPage.GetWTextSize(JW, w, h);      

		if (x != w)
			MessageBox("This English charset don't use a fixed pitch!\n"
					   "The width of grasping will be wrong.", "Warning", MB_OK | MB_ICONWARNING);
		if (y != h)
			MessageBox("The height of grasping is not congruent!", "Warning", MB_OK | MB_ICONWARNING);

		// Redraw preview area
		vfillbox(0, 0, m_rectPreview.Width(), m_rectPreview.Height(), _RGB(0,0,0));

		// Output text
		int  top = 0;

		str[0]=0X0061; str[1]=0X0062; str[2]=0X0063; str[3]=0X0064; str[4]=0X0065; str[5]=0X0000;
		//wcscpy(str, (const wchar_t*)"a b c d e f g h i j k l m n o p q r s t u v w x y z" );
		m_VPage.GetWTextSize(str, w, h);

		
		m_VPage.WTextXY(0, top, RGB(255,255,255), str);

		top += h;

		str[0]=0X0041; str[1]=0X0042; str[2]=0X0043; str[3]=0X0044; str[4]=0X0045; str[5]=0X0000;
		//wcscpy(str, (const wchar_t*)"A B C D E F G H I J K L M N O P Q R S T U V W X Y Z ");
		m_VPage.GetWTextSize(str, w, h);
		m_VPage.WTextXY(0, top, RGB(255,255,255), str);

		top += h;

		str[0]=0X0031; str[1]=0X0032; str[2]=0X0033; str[3]=0X0034; str[4]=0X0035; str[5]=0X0036;
		str[6]=0X0028; str[7]=0X005E; str[8]=0X0024; str[9]=0X0021; str[10]=0X00B9; str[11]=0X005D;
		str[12]=0X0000;
		//wcscpy(str, (const wchar_t*)"1 2 3 4 5 6 7 8 9 0 . : , ; ( \" * ! ? ' ) ");
		m_VPage.GetWTextSize(str, w, h);
		m_VPage.WTextXY(0, top, RGB(255,255,255), str);

		top += h;
   
		m_VPage.GetWTextSize(m_szWGrasp, m_nGraspWidth, m_nGraspHeight);

		sprintf(outstr, "%d", m_nGraspWidth);
		m_edGraspWidth.SetWindowText(outstr);

		sprintf(outstr, "%d", m_nGraspHeight);
		m_edGraspHeight.SetWindowText(outstr);

		x = m_nGraspWidth;
		y = top;

		for (int i = 0; i < 4; i++)
		{
			vdrawrectangle(x, y, x + m_nGraspWidth - 1, y + m_nGraspHeight -1, _RGB(0,255,0));
			x += m_nGraspWidth;
			x += m_nGraspWidth;
		}

		GetWindowTextW( (HWND)m_edTest, str, 80);
		m_VPage.WTextXY(0, top, RGB(255,255,255), str);
	}
	else
	{
		char  str[80];
		int  x, y, w, h;

		m_VPage.GetTextSize("W", x, y);      
		m_VPage.GetTextSize("j", w, h);      

		if (x != w)
			MessageBox("This English charset don't use a fixed pitch!\n"
					   "The width of grasping will be wrong.", "Warning", MB_OK | MB_ICONWARNING);
		if (y != h)
			MessageBox("The height of grasping is not congruent!", "Warning", MB_OK | MB_ICONWARNING);

		// Redraw preview area
		vfillbox(0, 0, m_rectPreview.Width(), m_rectPreview.Height(), _RGB(0,0,0));

		// Output text
		int  top = 0;



		strcpy(str, "abcdefghijklmnopqrstuvwxyz");
		m_VPage.GetTextSize(str, w, h);

		
		m_VPage.TextXY(0, top, RGB(255,255,255), str);

		top += h;

		strcpy(str, "ABCDEFGHIJKLMNOPQRSTUCWXYZ");
		m_VPage.GetTextSize(str, w, h);
		m_VPage.TextXY(0, top, RGB(255,255,255), str);

		top += h;

		strcpy(str, "1234567890.:,;(\"*!?')");
		m_VPage.GetTextSize(str, w, h);
		m_VPage.TextXY(0, top, RGB(255,255,255), str);

		top += h;
   
		m_VPage.GetTextSize(m_szGrasp, m_nGraspWidth, m_nGraspHeight);

		sprintf(outstr, "%d", m_nGraspWidth);
		m_edGraspWidth.SetWindowText(outstr);

		sprintf(outstr, "%d", m_nGraspHeight);
		m_edGraspHeight.SetWindowText(outstr);

		x = m_nGraspWidth;
		y = top;

		for (int i = 0; i < 4; i++)
		{
			vdrawrectangle(x, y, x + m_nGraspWidth - 1, y + m_nGraspHeight -1, _RGB(0,255,0));
			x += m_nGraspWidth;
			x += m_nGraspWidth;
		}

		m_edTest.GetWindowText(str, 80);
		m_VPage.TextXY(0, top, RGB(255,255,255), str);
	}

    InvalidateRect(NULL);
}

int CGenFontDlg::ConvertLogUnitToPoint(int nLogUnitSize, CDC* pDC/*=NULL*/)
{
	HDC  hDC;

	if (pDC != NULL)
	{
		ASSERT_VALID(pDC);
		ASSERT(pDC->m_hAttribDC != NULL);
		
        hDC = pDC->m_hAttribDC;
	}
	else
		hDC = ::GetDC(NULL);

    POINT  ptOrg = { 0, 0 };

    ::DPtoLP(hDC, &ptOrg, 1);
	
    POINT  pt = { 0, 0 };

	pt.y = abs(nLogUnitSize) + ptOrg.y;

	::LPtoDP(hDC,&pt,1);
	
    pt.y *= 720;    // 72 points/inch, 10 decipoints/point
	
	int  nPointSize= (pt.y) / (::GetDeviceCaps(hDC, LOGPIXELSY));

	if (pDC == NULL)
		::ReleaseDC(NULL, hDC);

	return nPointSize;
}

void CGenFontDlg::OnGrasp() 
{
    CString  str;

    if (!UpdateData())
        return;     // invalid data

    str.Format("%02d%02d.FNT", m_nGraspWidth, m_nGraspHeight);

    // Ask filename to save
    CFileDialog fileDlg(FALSE,              // Construct file save box
                        NULL,               // Default filename extension
                        str,                // Initial filename
                        OFN_HIDEREADONLY,
                        "All Files (*.*)|*.*||");
    
    if (fileDlg.DoModal() == IDCANCEL)
        return;

    // Retrive the filename
    str = fileDlg.GetPathName();

    if (str.IsEmpty())
        return; 
    
    // Open this file
    FILE  *fp;

    if ((fp = fopen(str, "wb")) == NULL)
    {
        MessageBox("Can not open file!");

        return;
    }

    // Grasp fonts
    ASSERT(m_nCharSet >= 0 && m_nCharSet <= 6);

    int  count;

    switch (m_nCharSet)
    {
    case 0:  count = GraspASCII(fp);    break;
    case 1:  count = GraspBIG5(fp, 0);  break;
    case 2:  count = GraspBIG5(fp, 1);  break;
    case 3:  count = GraspGB2312(fp);   break;
    case 4:  count = GraspKSC(fp);      break;
    case 5:  count = GraspJIS(fp);      break;
	case 6:  count = GraspUCS_2(fp);    break;
    }

    // Close this file
    fclose(fp);

    // Report
    str.Format("Total\t\t%d characters\n"
               "Char size:\t%d x %d\n"
               "Char packed:\t%d bytes", 
               count, m_nGraspWidth, m_nGraspHeight, nPackSize);

    MessageBox(str, "Saved OK!", MB_ICONINFORMATION);
}

int  CGenFontDlg::GraspASCII(FILE *fp)
{
    // Grasp to file
    int   count;
    BYTE  ch[2];

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;
    ch[1] = 0;

    // ASCII code:
    //  Control Code    0x00 ~ 0x1F     31  (don't use)
    //  Visiable Char.  0x20 ~ 0x7F     96                

    for (ch[0] = 0x20; ch[0] <= 0x7F; ch[0]++)
    {
        vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

        m_VPage.TextXY(0, 0, RGB(255,255,255), (char *) ch);

        GraspFontToBuff(0, 0);

        fwrite(pPack, 1, nPackSize, fp);

        count++;
    }	

    return count;
}

int  CGenFontDlg::GraspBIG5(FILE *fp, int extend)
{
    // Grasp to file
    int            count;
    char           ch[3];
    unsigned char  hb, lb;
    unsigned short wordLimit;   // NOTE: 0xhblb

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;
    ch[2] = 0;  

    // BIG-5 Traditional Chinese charset code:
    //  Valid hi bytes  is 0xA1 ~ 0xFE
    //  Valid low bytes is 0x40 ~ 0x7E, 0xA1 ~ 0xFE
    //
    //  
    //  Full Symbols         0xA140 ~ 0xA3BF             406 
    //  Frequent Chars       0xA440 ~ 0xC67E            5401
    //  Added Symbols                 0xC6A1 ~ 0xC8D3    365
    //  Infrequent Chars     0xC940 ~ 0xF9D5            7652
    //  Eten Chars                    0xF9D6 ~ 0xF9FE     41

    if (extend == 1)
        wordLimit = 0xFEF9;     // 406+5401+365+7652+41   (13865)
    else
        wordLimit = 0xD3C8;     // 406+5401+365           (6172)
    
    for (hb = 0xA1; hb <= 0xFE; hb++)
    {
        for (lb = 0x40; lb <= 0xFE; lb++)
        {
            if (lb == 0x7F) lb = 0xA1;
                
            ch[0] = hb;
            ch[1] = lb;           

            vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

            m_VPage.TextXY(0, 0, RGB(255,255,255), ch);

            GraspFontToBuff(0, 0); 

/*          vfillbox(0, 0, m_nGraspWidth + 4, m_nGraspHeight + 4, _RGB(0,0,0));

            m_VPage.TextXY(2, 0, 0x00FF, ch);
            m_VPage.TextXY(0, 0, 0x0100, ch);
            m_VPage.TextXY(1, 0, 0x00FF, ch);

            GraspFontToBuff(1, 0); */

            fwrite(pPack, 1, nPackSize, fp);

            count++;

            if (wordLimit == *(unsigned short *) ch)
            {
                hb = 0xFE;  // break hb loop
                break;
            }
        }	
    }

    return count;
}

int  CGenFontDlg::GraspGB2312(FILE *fp)
{
    // Grasp to file
    int            count;
    char           ch[3];
    unsigned char  hb, lb;

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;
    ch[2] = 0;  

    // GB2312-1980 Simple Chinese code:
    //  Valid hi bytes  is 0xA1 ~ 0xF7
    //  Valid low bytes is 0xA1 ~ 0xFE

    for (hb = 0xA1; hb <= 0xF7; hb++)
    {
        for (lb = 0xA1; lb <= 0xFE; lb++)
        {               
            ch[0] = hb;
            ch[1] = lb;           

            vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

            m_VPage.TextXY(0, 0, RGB(255,255,255), ch);

            GraspFontToBuff(0, 0); 

            fwrite(pPack, 1, nPackSize, fp);

            count++;
        }	
    }

    return count;
}

int  CGenFontDlg::GraspKSC(FILE *fp)
{
    // Grasp to file
    int            count;
    char           ch[3];
    unsigned char  hb, lb;

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;
    ch[2] = 0;  

    // KSC-5601-1987 Hangul charset code:
    //  Valid hi bytes  is 0x81 ~ 0xFE
    //  Valid low bytes is 0x41 ~ 0x5A, 0x61 ~ 0x7A, 0x81 ~ 0xFE

    for (hb = 0x81; hb <= 0xFE; hb++)
    {
        for (lb = 0x41; lb <= 0xFE; lb++)
        {
            if (lb == 0x5B)      lb = 0x61;
            else if (lb == 0x7B) lb = 0x81;
                
            ch[0] = hb;
            ch[1] = lb;           

            vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

            m_VPage.TextXY(0, 0, RGB(255,255,255), ch);

            GraspFontToBuff(0, 0); 

            fwrite(pPack, 1, nPackSize, fp);

            count++;
        }	
    }

    return count;
}

int  CGenFontDlg::GraspJIS(FILE *fp)
{

	//MessageBox("Not implemented yet!");
    // Grasp to file
    int            count;
    char           ch[3];
    unsigned char  hb, lb;

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;
    ch[2] = 0;  

    // Shift-JIS Hangul charset code:
    //  Valid hi bytes  is 0x81-0x9F， 0xE0-0xFC
    //  Valid low bytes is 0x40-0xFC (beside 0x7F)
    for (hb = 0x81; hb <= 0xFC; hb++)
    {
		if (hb == 0xA0) hb = 0xE0;

        for (lb = 0x40; lb <= 0xFC; lb++)
        {
            if (lb == 0x7)      continue;
                
            ch[0] = hb;
            ch[1] = lb;           

            vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

            m_VPage.TextXY(0, 0, RGB(255,255,255), ch);

            GraspFontToBuff(0, 0); 

            fwrite(pPack, 1, nPackSize, fp);

            count++;
        }	
    }

    return count;
}

int  CGenFontDlg::GraspUCS_2(FILE *fp)
{
    int            count;
    //char           ch[3];
    unsigned short ub;

    // Allocate font buffer 
    ReallocFontBuff(m_nGraspWidth, m_nGraspHeight, m_bSmooth);

    count = 0;

    // USC-2 Hangul charset code:
    //  Valid bytes  is from 0X0000 ~ 0XD7FF
    
    for ( ub = 0x0000; ub <= 0xD7FF; ub++)
    {
        vfillbox(0, 0, m_nGraspWidth, m_nGraspHeight, _RGB(0,0,0));

        m_VPage.WTextXY(0, 0, RGB(255,255,255), (LPCWSTR)&ub);

        GraspFontToBuff(0, 0); 

        fwrite(pPack, 1, nPackSize, fp);

        count++;
    }

	return count;
}


void CGenFontDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
    FreeFontBuff();
}

void CGenFontDlg::OnChangeEditHeight() 
{
    CString  s;
    int      n;

    if (m_edHeight.m_hWnd)
    {
        m_edHeight.GetWindowText(s);	

        n = atoi(s);

        if (n != m_LogFont.lfHeight)
        {
            m_LogFont.lfHeight = n;

            ResetFont();
        }
    }
}

void CGenFontDlg::OnChangeEditWeight() 
{
    CString  s;
    int      n;

    if (m_edHeight.m_hWnd)
    {
        m_edWeight.GetWindowText(s);	

        n = atoi(s);

        if (n != m_LogFont.lfWeight)
        {
            m_LogFont.lfWeight = n;

            ResetFont();
        }
    }	
}

void CGenFontDlg::OnRadioNonSmooth() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfQuality = NONANTIALIASED_QUALITY;
    
    m_bSmooth = FALSE;

    UpdateSmoothRadio();

    ResetFont();	
}

void CGenFontDlg::OnRadioSmooth() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfQuality = ANTIALIASED_QUALITY;
    
    m_bSmooth = TRUE;

    UpdateSmoothRadio();

    ResetFont();	
}

void CGenFontDlg::OnRadioAscii() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = DEFAULT_CHARSET; // ANSI_CHARSET;

    m_edTest.SetWindowText("Fuck! Keep TP on your little ass hole...");
    
    strcpy(m_szGrasp, "F");

    m_nCharSet = 0;

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioBig5()
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = CHINESEBIG5_CHARSET;
    
    m_nCharSet = 1;	
    
    m_edTest.SetWindowText(_T("白癡！這是一條永無止盡的路啊。"));

    strcpy(m_szGrasp, _T("白"));

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioBig5ex() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = CHINESEBIG5_CHARSET;

    m_nCharSet = 2;	
    
    m_edTest.SetWindowText(_T("柺杖、符籙～暴殄天物？"));
    
    strcpy(m_szGrasp, _T("白"));

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioGb() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = GB2312_CHARSET;

    m_nCharSet = 3;	
    
    m_edTest.SetWindowText(_T("掛璃忳翍釬楊垀悵誘ㄛ峊氪蔚甡楊薺芴噤袚噶ㄐ"));

    strcpy(m_szGrasp, _T("掛"));

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioKsc() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = HANGUL_CHARSET;

    m_nCharSet = 4;	
    
    m_edTest.SetWindowText(_T("綴瓚攪葬, 絲 儅晦賊 釭醞縑 棻衛 諦!"));

    strcpy(m_szGrasp, _T("綴"));

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioJis() 
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = SHIFTJIS_CHARSET;

    m_nCharSet = 5;
    
    m_edTest.SetWindowText(_T("??"));

    strcpy(m_szGrasp, _T(""));

    UpdateCharSetRadio();

    ResetFont();
}

void CGenFontDlg::OnRadioUCS_2()
{
    if (m_LogFont.lfFaceName[0] == 0)
        return;

    m_LogFont.lfCharSet = SHIFTJIS_CHARSET;

	m_nCharSet = 6;

	//WCHAR str[80];
	//str[0]=0X01FF; str[1]=0X30CF; str[2]=0X30AE; str[3]=0X4E83; str[4]=0XAF4D; str[5]=0XAFC1; str[6]=0X0000;
    m_edTest.SetWindowText( _T("豬頭，這是萬碼奔騰的世界"));

	m_szWGrasp[0]=0X30d1;
	m_szWGrasp[1]=0X0000;

    //wcscpy(m_szWGrasp, "%x30d1");

    UpdateCharSetRadio();

    ResetFont();

}

void CGenFontDlg::UpdateSmoothRadio(void)
{
    CButton  *radio0 = (CButton *) GetDlgItem(IDC_RADIO_NONSMOOTH);
    CButton  *radio1 = (CButton *) GetDlgItem(IDC_RADIO_SMOOTH);

    radio0->SetCheck(m_bSmooth == FALSE);
    radio1->SetCheck(m_bSmooth == TRUE);
}

void CGenFontDlg::UpdateCharSetRadio(void)
{
    CButton  *radio0 = (CButton *) GetDlgItem(IDC_RADIO_ASCII);
    CButton  *radio1 = (CButton *) GetDlgItem(IDC_RADIO_BIG5);
    CButton  *radio2 = (CButton *) GetDlgItem(IDC_RADIO_BIG5EX);
    CButton  *radio3 = (CButton *) GetDlgItem(IDC_RADIO_GB);
    CButton  *radio4 = (CButton *) GetDlgItem(IDC_RADIO_KSC);
    CButton  *radio5 = (CButton *) GetDlgItem(IDC_RADIO_JIS);
	CButton  *radio6 = (CButton *) GetDlgItem(IDC_RADIO_UCS_2);
	

    radio0->SetCheck(m_nCharSet == 0);
    radio1->SetCheck(m_nCharSet == 1);
    radio2->SetCheck(m_nCharSet == 2);
    radio3->SetCheck(m_nCharSet == 3);
    radio4->SetCheck(m_nCharSet == 4);
    radio5->SetCheck(m_nCharSet == 5);
	radio6->SetCheck(m_nCharSet == 6);
}
