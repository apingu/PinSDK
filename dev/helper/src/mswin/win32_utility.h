#ifndef WIN32UTILITY_H
#define WIN32UTILITY_H


#pragma once
#include "afxwin.h"
#include <string>
#include <atlimage.h>


HRGN            CreateRgnFromFile       ( HBITMAP hBmp, COLORREF color, INT& width, INT& height );

// 強制視窗跳到最上層
void            ForceWindowToForeground ( HWND hwnd );

HBITMAP         HBitmapCopy             ( HBITMAP hSourceHbitmap );
BOOL            CBitmapCopy             ( CBitmap* dest, const CBitmap* src );
BOOL            CBitmapCopy             ( CBitmap* dest, const CBitmap* src, int width, int height );

BOOL            RegDelnode              ( HKEY hKeyRoot, LPTSTR lpSubKey );

bool            CopyDir                 ( LPCTSTR* source_folder, LPCTSTR* target_folder );
// Fully qualified name of the directory being deleted, without trailing backslash
int             RemoveDir               ( LPCTSTR dir );

void            CreateStretchImage      ( CImage *pImage, CImage *ResultImage, int StretchHeight, int StretchWidth );

/*
// WString not unicode
std::wstring    UTF8ToWString           ( const std::string s );
std::string     WStringToUTF8           ( const std::wstring ws );

std::wstring    UTF8ToUnicode           ( const std::string s );
std::string     UnicodeToUTF8           ( const std::wstring ws );

std::wstring    JISToUnicode            ( const std::string s );
std::string     UnicodeToJIS            ( const std::wstring ws );

//   "tw","big5"; "jp", jis"; "ka"; "ca", "gb"; "us"; "utf8"
std::wstring    MultiByteToUnicode      ( const std::string s, const char* codepage );
std::string     UnicodeToMultiByte      ( const std::wstring ws, const char* codepage );

std::string     UTF8ToUTF8BOM           ( const std::string s );
*/





// 系統預設語言 會影響產出的字串是否為亂碼
// mnti-byte to unicode
std::wstring    AsciiToUnicode          ( const std::string s, int codepage = 0 );
// unicode to mnti-byte 
std::string     UnicodeToAscii          ( const std::wstring ws, int codepage = 0 );
void            killProcessByName       ( const TCHAR* filename );

#endif