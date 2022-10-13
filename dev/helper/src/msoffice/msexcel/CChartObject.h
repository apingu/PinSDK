// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard
#include "ImportDef.h"
/*
//#import "C:\\Program Files (x86)\\Microsoft office\\Office14\\EXCEL.EXE" no_namespace
#pragma region Import the type libraries

#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
    rename("RGB", "MSORGB") \
    rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")

using namespace office;

#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

#import "libid:00020813-0000-0000-C000-000000000046" \
    rename("DialogBox", "ExcelDialogBox") \
    rename("RGB", "ExcelRGB") \
    rename("CopyFile", "ExcelCopyFile") \
    rename("ReplaceText", "ExcelReplaceText") \
    no_auto_exclude
// [-or-]
//#import "C:\\Program Files\\Microsoft office\\Office12\\EXCEL.EXE" \
//	rename("DialogBox", "ExcelDialogBox") \
//	rename("RGB", "ExcelRGB") \
//	rename("CopyFile", "ExcelCopyFile") \
//	rename("ReplaceText", "ExcelReplaceText") \
//	no_auto_exclude

#pragma endregion
*/
// CChartObject wrapper class

class CChartObject : public COleDispatchDriver
{
public:
    CChartObject(){} // Calls COleDispatchDriver default constructor
    CChartObject(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
    CChartObject(const CChartObject& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

    // Attributes
public:

    // Operations
public:


    // ChartObject methods
public:
    LPDISPATCH get_Application()
    {
        LPDISPATCH result;
        InvokeHelper(0x94, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    long get_Creator()
    {
        long result;
        InvokeHelper(0x95, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH get_Parent()
    {
        LPDISPATCH result;
        InvokeHelper(0x96, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH get_BottomRightCell()
    {
        LPDISPATCH result;
        InvokeHelper(0x267, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    VARIANT BringToFront()
    {
        VARIANT result;
        InvokeHelper(0x25a, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    VARIANT _Copy()
    {
        VARIANT result;
        InvokeHelper(0xa31, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    VARIANT CopyPicture(long Appearance, long Format)
    {
        VARIANT result;
        static BYTE parms[] = VTS_I4 VTS_I4 ;
        InvokeHelper(0xd5, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, Appearance, Format);
        return result;
    }
    VARIANT Cut()
    {
        VARIANT result;
        InvokeHelper(0x235, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    VARIANT Delete()
    {
        VARIANT result;
        InvokeHelper(0x75, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH Duplicate()
    {
        LPDISPATCH result;
        InvokeHelper(0x40f, DISPATCH_METHOD, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    BOOL get_Enabled()
    {
        BOOL result;
        InvokeHelper(0x258, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_Enabled(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x258, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    double get_Height()
    {
        double result;
        InvokeHelper(0x7b, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
        return result;
    }
    void put_Height(double newValue)
    {
        static BYTE parms[] = VTS_R8 ;
        InvokeHelper(0x7b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    long get_Index()
    {
        long result;
        InvokeHelper(0x1e6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
        return result;
    }
    double get_Left()
    {
        double result;
        InvokeHelper(0x7f, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
        return result;
    }
    void put_Left(double newValue)
    {
        static BYTE parms[] = VTS_R8 ;
        InvokeHelper(0x7f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    BOOL get_Locked()
    {
        BOOL result;
        InvokeHelper(0x10d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_Locked(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x10d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    CString get_Name()
    {
        CString result;
        InvokeHelper(0x6e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
        return result;
    }
    void put_Name(LPCTSTR newValue)
    {
        static BYTE parms[] = VTS_BSTR ;
        InvokeHelper(0x6e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    CString get_OnAction()
    {
        CString result;
        InvokeHelper(0x254, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
        return result;
    }
    void put_OnAction(LPCTSTR newValue)
    {
        static BYTE parms[] = VTS_BSTR ;
        InvokeHelper(0x254, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    VARIANT get_Placement()
    {
        VARIANT result;
        InvokeHelper(0x269, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    void put_Placement(VARIANT& newValue)
    {
        static BYTE parms[] = VTS_VARIANT ;
        InvokeHelper(0x269, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, &newValue);
    }
    BOOL get_PrintObject()
    {
        BOOL result;
        InvokeHelper(0x26a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_PrintObject(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x26a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    VARIANT Select(VARIANT& Replace)
    {
        VARIANT result;
        static BYTE parms[] = VTS_VARIANT ;
        InvokeHelper(0xeb, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, &Replace);
        return result;
    }
    VARIANT SendToBack()
    {
        VARIANT result;
        InvokeHelper(0x25d, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    double get_Top()
    {
        double result;
        InvokeHelper(0x7e, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
        return result;
    }
    void put_Top(double newValue)
    {
        static BYTE parms[] = VTS_R8 ;
        InvokeHelper(0x7e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    LPDISPATCH get_TopLeftCell()
    {
        LPDISPATCH result;
        InvokeHelper(0x26c, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    BOOL get_Visible()
    {
        BOOL result;
        InvokeHelper(0x22e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_Visible(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x22e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    double get_Width()
    {
        double result;
        InvokeHelper(0x7a, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
        return result;
    }
    void put_Width(double newValue)
    {
        static BYTE parms[] = VTS_R8 ;
        InvokeHelper(0x7a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    long get_ZOrder()
    {
        long result;
        InvokeHelper(0x26e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH get_ShapeRange()
    {
        LPDISPATCH result;
        InvokeHelper(0x5f8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    VARIANT Activate()
    {
        VARIANT result;
        InvokeHelper(0x130, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH get_Chart()
    {
        LPDISPATCH result;
        InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    BOOL get_ProtectChartObject()
    {
        BOOL result;
        InvokeHelper(0x5f9, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_ProtectChartObject(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x5f9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    BOOL get_RoundedCorners()
    {
        BOOL result;
        InvokeHelper(0x26b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_RoundedCorners(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x26b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    LPDISPATCH get_Border()
    {
        LPDISPATCH result;
        InvokeHelper(0x80, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    LPDISPATCH get_Interior()
    {
        LPDISPATCH result;
        InvokeHelper(0x81, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, NULL);
        return result;
    }
    BOOL get_Shadow()
    {
        BOOL result;
        InvokeHelper(0x67, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
        return result;
    }
    void put_Shadow(BOOL newValue)
    {
        static BYTE parms[] = VTS_BOOL ;
        InvokeHelper(0x67, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
    }
    VARIANT Copy()
    {
        VARIANT result;
        InvokeHelper(0x227, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
        return result;
    }

    // ChartObject properties
public:

};
