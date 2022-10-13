________________________________________________________________________________________
CTXDialog
a. include Win32Utility.cpp and .h

b. repace all public form CDialong or CDialogEx to CTXDialog

c. when construction do like this
   CResConvertDlg::CResConvertDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CResConvertDlg::IDD, pParent)
   ==> background image
   CResConvertDlg::CResConvertDlg(CWnd* pParent /*=NULL*/)
    : CTXDialog(CResConvertDlg::IDD, IDB_BACKGROUND, pParent)

d. OnInitDialog
   CDialog::OnInitDialog();    ==> must have
   to mask color
   CTXDialog::OnInitDialog(RGB(255, 0, 0));

e. on properties set Border as none

________________________________________________________________________________________
CTXButton
SetStateResID(IDB_COLOR1,  IDB_COLOR1,  IDB_COLOR1,  IDB_COLOR1,  RGB(255, 0, 255) );


