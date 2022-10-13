#pragma once

//#import "C:\\Program Files (x86)\\Microsoft office\\Office14\\EXCEL.EXE" no_namespace
#pragma region Import the type libraries


#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
    rename("RGB", "MSORGB") \
    rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE15\\MSO.DLL" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")

using namespace office;


#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
//#import "C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA7.1\\VBE7.DLL"


using namespace VBIDE;


#import "libid:00020813-0000-0000-C000-000000000046" \
    rename("DialogBox", "ExcelDialogBox") \
    rename("RGB", "ExcelRGB") \
    rename("CopyFile", "ExcelCopyFile") \
    rename("ReplaceText", "ExcelReplaceText") \
    no_auto_exclude
// [-or-]
//#import "C:\\Program Files\\Microsoft office\\Office15\\EXCEL.EXE" \
//	rename("DialogBox", "ExcelDialogBox") \
//	rename("RGB", "ExcelRGB") \
//	rename("CopyFile", "ExcelCopyFile") \
//	rename("ReplaceText", "ExcelReplaceText") \
	no_auto_exclude

#pragma endregion

/* 蝑?憓?
//#import "C:\\Program Files (x86)\\Microsoft office\\Office14\\EXCEL.EXE" no_namespace
#pragma region Import the type libraries


//#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \

#import "C:\Windows\InfusedApps\Packages\Microsoft.office.Desktop_16000.8528.2136.0_x86__8wekyb3d8bbwe\VFS\ProgramFilesCommonX86\Microsoft Shared\OFFICE16\MSO.DLL" \
    rename("RGB", "MSORGB") \
    rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE15\\MSO.DLL" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")

using namespace office;


//#import "libid:0002E157-0000-0000-C000-000000000046"

#import "C:\Windows\InfusedApps\Packages\Microsoft.office.Desktop_16000.8528.2136.0_x86__8wekyb3d8bbwe\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB"
#//import "C:\Windows\InfusedApps\Packages\Microsoft.office.Desktop_16000.8528.2136.0_x86__8wekyb3d8bbwe\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA7.1\VBE7.DLL"
// [-or-]
//#import "C:\\Program Files (x86)\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA7.1\\VBE7.DLL"


using namespace VBIDE;


//#import "libid:00020813-0000-0000-C000-000000000046" \

#import "C:\Windows\InfusedApps\Packages\Microsoft.office.Desktop.Excel_16000.8528.2136.0_x86__8wekyb3d8bbwe\Office16\EXCEL.EXE" \
    rename("DialogBox", "ExcelDialogBox") \
    rename("RGB", "ExcelRGB") \
    rename("CopyFile", "ExcelCopyFile") \
    rename("ReplaceText", "ExcelReplaceText") \
    no_auto_exclude
// [-or-]
//#import "C:\\Program Files\\Microsoft office\\Office15\\EXCEL.EXE" \
//	rename("DialogBox", "ExcelDialogBox") \
//	rename("RGB", "ExcelRGB") \
//	rename("CopyFile", "ExcelCopyFile") \
//	rename("ReplaceText", "ExcelReplaceText") \
	no_auto_exclude

#pragma endregion
*/