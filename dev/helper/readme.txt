// ctrl + j

// linux
../../include;
./src/office/ExcelFormat/include/ExcelFormat/;
./src/office/libxl/include_cpp;

mkdir -p  $(RemoteRootDir)/../include/helper;
cp    -ru $(RemoteProjectDir)/src/office/*.h         $(RemoteRootDir)/../include/helper


// msvc
copy /y src\mswin\mfc-utility.h             ..\..\include\helper\
copy /y src\mswin\mfc-listgrid.h            ..\..\include\helper\
copy /y src\mswin\mfc-txhelper.h            ..\..\include\helper\
copy /y src\mswin\mfc-txdialog.h            ..\..\include\helper\
copy /y src\mswin\mfc-txformview.h          ..\..\include\helper\
copy /y src\mswin\mfc-txstatic.h            ..\..\include\helper\
copy /y src\mswin\mfc-txbutton.h            ..\..\include\helper\
copy /y src\mswin\mfc-txsliderctrl.h        ..\..\include\helper\
copy /y src\mswin\mfc-txmarquee.h           ..\..\include\helper\
copy /y src\mswin\mfc-treectrl.h            ..\..\include\helper\
copy /y src\mswin\mfc-sxbutton.h            ..\..\include\helper\
copy /y src\mswin\win32_utility.h           ..\..\include\helper\
copy /y  src\net\csbrowser.h                ..\..\include\helper\
copy /y src\Openoffice\OOoHelper.h          ..\..\include\helper\
copy /y src\msoffice\ExcelFile.h            ..\..\include\helper\
copy /y src\msoffice\xlsdb-sheet.h          ..\..\include\helper\
copy /y src\msoffice\xlsdb-query.h          ..\..\include\helper\
copy /y src\dictionary\dictionary.h         ..\..\include\helper\
copy /y src\msoffice\excel_xml.h            ..\..\include\helper\
copy /y src\net\nh-urlbundle.h              ..\..\include\helper\
copy /y src\office\xl_file.h                ..\..\include\helper\
copy /y src\office\xldb-sheet.h             ..\..\include\helper\
copy /y src\office\xldb-query.h             ..\..\include\helper\
copy /y src\office\libxl\lib\libxl.lib      ..\..\lib\Win32\Debug
copy /y src\office\libxl\lib\libxl.lib      ..\..\lib\Win32\Release
copy /y src\office\libxl\lib64\libxl.lib    ..\..\lib\x64\Debug
copy /y src\office\libxl\lib64\libxl.lib    ..\..\lib\x64\Release
copy /y src\office\libxl\bin\libxl.dll      ..\..\lib\Win32\Debug
copy /y src\office\libxl\bin\libxl.dll      ..\..\lib\Win32\Release
copy /y src\office\libxl\bin64\libxl.dll    ..\..\lib\x64\Debug
copy /y src\office\libxl\bin64\libxl.dll    ..\..\lib\x64\Release
copy /y src\office\libxl\bin\libxl.dll      ..\bin

      
//============================================================================================
// predefine


// curl ======================================================================================
// https://blog.51cto.com/fengyuzaitu/2433565
//因为宏定义主要作用于curl/curl.h头文件，用于头文件的宏定义条件编译，必须定义这两个宏定义，否则编译出错，看出错分析
BUILDING_LIBCURL
CURL_STATICLIB

//USE_WINDOWS_SSPI和USE_SCHANNEL两个预编译处理定义是用来定义HTTPS，为了使用https协议需要指定，切记！
USE_WINDOWS_SSPI
USE_SCHANNEL

// 如果不想額外安裝 openssl 跟 libssh2 用這個 windows 標準替代
USE_WINDOWS_SSPI
WANT_IDN_PROTOTYPES
USE_WIN32_IDN
// 使用 SSPI WinLDN 需要使用這個 lib  替代 openssl 跟 libssh2
#pragma comment ( lib, "Normaliz.lib" )

DEBUGBUILD
USE_SCHANNEL
USE_IPV6

單純引入 lib 需要
CURL_STATICLIB

// winsockt =================================================================================
WIN32_LEAN_AND_MEAN   // winsock

// open office ==============================================================================
_OOoSDK_

// 標準 =====================================================================================
_WINDOWS
_LIB
%(PreprocessorDefinitions)


//============================================================================================
xlslib
http://m.blog.csdn.net/zskylark/article/details/6210403

http://www.libxl.com/

#include "libxl.h"
using namespace libxl;

int main() 
{
    Book* book = xlCreateBook(); // xlCreateXMLBook() for xlsx
    if(book)
    {
        Sheet* sheet = book->addSheet(L"Sheet1");
        if(sheet)
        {
            sheet->writeStr(2, 1, L"Hello, World !");
            sheet->writeNum(3, 1, 1000);
        }
        book->save(L"example.xls");
        book->release();
    } 
    return 0;
}


#include <locale.h>
#include "ocilib.h"
#include "xlslib.h"
using namespace xlslib_core;
#define RECORDCOUNT                65536
int main(int argc, char *argv[])
{
        setlocale(LC_ALL, "zh_CN.utf-8");
        enum { UTF8, GBK };
        workbook wb1, wb2;
        worksheet* wb1sh1 = wb1.sheet("Sheet1");
        worksheet* wb2sh1 = wb2.sheet("Sheet1");
        int row = 0, coloumn = 0;
        if (argc != 4) {
                printf("Example: ./demo SID USER PASSWORD/n");
                return 0;
        }
        OCI_Connection* cn;
    OCI_Statement* st;
    OCI_Resultset* rs;
    OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT);
    cn = OCI_ConnectionCreate(argv[1], argv[2], argv[3], OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    OCI_ExecuteStmt(st, "select * from t_test_a");
    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
                if (row < RECORDCOUNT) {
                        wb1sh1->label(UTF8, row, coloumn,        OCI_GetString(rs, 1));
                        wb1sh1->label(UTF8, row, coloumn+1,        OCI_GetString(rs, 5));
                        wb1sh1->label(UTF8, row, coloumn+2,        OCI_GetString(rs, 6));
                } else {
                        wb2sh1->label(UTF8, row-RECORDCOUNT, coloumn,        OCI_GetString(rs, 1));
                        wb2sh1->label(UTF8, row-RECORDCOUNT, coloumn+1, OCI_GetString(rs, 5));
                        wb2sh1->label(UTF8, row-RECORDCOUNT, coloumn+2, OCI_GetString(rs, 6));
                }
                printf("/r%d", row+1);
                fflush(stdout);
                row++;
    }
        printf("/n");
        wb1.Dump("./demo1.xls");
        wb2.Dump("./demo2.xls");
    OCI_Cleanup();
    return EXIT_SUCCESS;