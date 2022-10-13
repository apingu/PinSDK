
#ifndef __CWEBPAGE_H_INCLUDED
#define __CWEBPAGE_H_INCLUDED

#include <windows.h>

#include <ole2.h>
#pragma comment ( lib, "ole32.lib" )   /* link with Win32 lib */ 
#pragma comment ( lib, "oleaut32.lib")
//uuid.lib odbc32.lib odbccp32.lib
//do this first
//** OleInitialize(NULL);
//** OleUninitialize


#ifdef __cplusplus
extern "C"
{
#endif

    //long CsBrowserinitialize( void )   { return OleInitialize(NULL); }
    //void CsBrowserUninitialize( void ) { OleUninitialize(); }
   
    long CreateCsBrowser(HWND);
    #define EMBEDBROWSEROBJECT CreateCsBrowser
    //typedef long WINAPI EmbedBrowserObjectPtr(HWND);

    void ReleaseCsBrowser(HWND);
    #define UNEMBEDBROWSEROBJECT ReleaseCsBrowser
    //typedef long WINAPI UnEmbedBrowserObjectPtr(HWND);

    long UpdateHTMLPage(HWND, LPCTSTR);
    #define DISPLAYHTMLPAGE UpdateHTMLPage
    //typedef long WINAPI DisplayHTMLPagePtr(HWND, LPCTSTR);

    long UpdateHTMLStr(HWND, LPCTSTR);
    #define DISPLAYHTMLSTR UpdateHTMLStr
    //typedef long WINAPI DisplayHTMLStrPtr(HWND, LPCTSTR);

#ifdef __cplusplus
}
#endif

#endif /* __CWEBPAGE_H_INCLUDED */
