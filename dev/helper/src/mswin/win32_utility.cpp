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

//01. == C 系統文件/C++ 系統文件
#include "StdAfx.h"
#include <vector>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-string.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "win32_utility.h"


HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color, INT& width, INT& height )
{
    // get image properties
    BITMAP bmp = { 0 };
    GetObject( hBmp, sizeof( BITMAP ), &bmp );
    // allocate memory for extended image information
    LPBITMAPINFO bi = ( LPBITMAPINFO ) new BYTE[ sizeof( BITMAPINFO ) + 8 ];
    memset( bi, 0, sizeof( BITMAPINFO ) + 8 );
    bi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    // set window size
    width = bmp.bmWidth;      // bitmap width
    height = bmp.bmHeight;     // bitmap height
    // create temporary dc
    HDC dc = CreateIC( _TEXT( "DISPLAY" ), NULL, NULL, NULL );
    // get extended information about image (length, compression, length of color table if exist, ...)
    DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
    // allocate memory for image data (colors)
    LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
    // allocate memory for color table
    if( bi->bmiHeader.biBitCount == 8 )
    {
        // actually color table should be appended to this header(BITMAPINFO),
        // so we have to reallocate and copy it
        LPBITMAPINFO old_bi = bi;
        // 255 - because there is one in BITMAPINFOHEADER
        bi = ( LPBITMAPINFO )new TCHAR[ sizeof( BITMAPINFO ) + 255 * sizeof( RGBQUAD ) ];
        memcpy( bi, old_bi, sizeof( BITMAPINFO ) );
        // release old header
        delete old_bi;
    }
    // get bitmap info header
    BITMAPINFOHEADER& bih = bi->bmiHeader;
    // get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
    LPDWORD clr_tbl = ( LPDWORD )&bi->bmiColors;
    // fill bits buffer
    res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
    DeleteDC( dc );
    BITMAP bm;
    GetObject( hBmp, sizeof( BITMAP ), &bm );
    // shift bits and byte per pixel (for comparing colors)
    LPBYTE pClr = ( LPBYTE )&color;
    // swap red and blue components
    BYTE tmp = pClr[ 0 ];
    pClr[ 0 ] = pClr[ 2 ];
    pClr[ 2 ] = tmp;
    // convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
    if( bih.biBitCount == 16 )
    {
        // for 16 bit
        color = ( ( DWORD )( pClr[ 0 ] & 0xf8 ) >> 3 ) |
            ( ( DWORD )( pClr[ 1 ] & 0xfc ) << 3 ) |
            ( ( DWORD )( pClr[ 2 ] & 0xf8 ) << 8 );
        // for 15 bit
        //      color = ((DWORD)(pClr[0]  &0xf8) >> 3) |
        //              ((DWORD)(pClr[1]  &0xf8) << 2) |
        //              ((DWORD)(pClr[2]  &0xf8) << 7);
    }
    const DWORD RGNDATAHEADER_SIZE = sizeof( RGNDATAHEADER );
    const DWORD ADD_RECTS_COUNT = 40;           // number of rects to be appended
    // to region data buffer
    // BitPerPixel
    BYTE    Bpp = bih.biBitCount >> 3;              // bytes per pixel
    // bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
    // so, both of them not
    DWORD m_dwAlignedWidthBytes = ( bmp.bmWidthBytes  & ~0x3 ) + ( !!( bmp.bmWidthBytes & 0x3 ) << 2 );
    // DIB image is flipped that's why we scan it from the last line
    LPBYTE  pColor = pBits + ( bih.biHeight - 1 ) * m_dwAlignedWidthBytes;
    DWORD   dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;  // offset of previous scan line
    // (after processing of current)
    DWORD   dwRectsCount = bih.biHeight;            // number of rects in allocated buffer
    INT     i, j;                                   // current position in mask image
    INT     first = 0;                              // left position of current scan line
    // where mask was found
    bool    wasfirst = false;                       // set when mask has been found in current scan line
    bool    ismask;                                 // set when current color is mask color
    // allocate memory for region data
    // region data here is set of regions that are rectangles with height 1 pixel (scan line)
    // that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
    RGNDATAHEADER* pRgnData =
        ( RGNDATAHEADER* )new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof( RECT ) ];
    // get pointer to RECT table
    LPRECT pRects = ( LPRECT )( ( LPBYTE )pRgnData + RGNDATAHEADER_SIZE );
    // zero region data header memory (header  part only)
    memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof( RECT ) );
    // fill it by default
    pRgnData->dwSize = RGNDATAHEADER_SIZE;
    pRgnData->iType = RDH_RECTANGLES;
    for( i = 0; i < bih.biHeight; i++ )
    {
        for( j = 0; j < bih.biWidth; j++ )
        {
            // get color
            switch( bih.biBitCount )
            {
            case 8:
                ismask = ( clr_tbl[ *pColor ] != color );
                break;
            case 16:
                ismask = ( *( LPWORD )pColor != ( WORD )color );
                break;
            case 24:
                ismask = ( ( *( LPDWORD )pColor & 0x00ffffff ) != color );
                break;
            case 32:
                ismask = ( *( LPDWORD )pColor != color );
            }
            // shift pointer to next color
            pColor += Bpp;
            // place part of scan line as RECT region if transparent color found after mask color or
            // mask color found at the end of mask image
            if( wasfirst )
            {
                if( !ismask )
                {
                    // save current RECT
                    pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
                    // if buffer full reallocate it with more room
                    if( pRgnData->nCount >= dwRectsCount )
                    {
                        dwRectsCount += ADD_RECTS_COUNT;
                        // allocate new buffer
                        LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof( RECT ) ];
                        // copy current region data to it
                        memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof( RECT ) );
                        // delte old region data buffer
                        delete pRgnData;
                        // set pointer to new regiondata buffer to current
                        pRgnData = ( RGNDATAHEADER* )pRgnDataNew;
                        // correct pointer to RECT table
                        pRects = ( LPRECT )( ( LPBYTE )pRgnData + RGNDATAHEADER_SIZE );
                    }
                    wasfirst = false;
                }
            }
            else if( ismask )      // set wasfirst when mask is found
            {
                first = j;
                wasfirst = true;
            }
        }
        if( wasfirst && ismask )
        {
            // save current RECT
            pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
            // if buffer full reallocate it with more room
            if( pRgnData->nCount >= dwRectsCount )
            {
                dwRectsCount += ADD_RECTS_COUNT;
                // allocate new buffer
                LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof( RECT ) ];
                // copy current region data to it
                memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof( RECT ) );
                // delte old region data buffer
                delete pRgnData;
                // set pointer to new regiondata buffer to current
                pRgnData = ( RGNDATAHEADER* )pRgnDataNew;
                // correct pointer to RECT table
                pRects = ( LPRECT )( ( LPBYTE )pRgnData + RGNDATAHEADER_SIZE );
            }
            wasfirst = false;
        }
        pColor -= dwLineBackLen;
    }
    // release image data
    delete[] pBits;
    delete[] bi;
    // create region
    HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof( RECT ), ( LPRGNDATA )pRgnData );
    // release region data
    delete[] pRgnData;
    return hRgn;
}


void ForceWindowToForeground( HWND hwnd )
{
#ifdef _WIN32_WCE
    SetForegroundWindow( hwnd );
    //  BringWindowToTop(hwnd);
#else
    // NOTE: an application cannot force a window to the foreground while
    //       the user is working with another window - from MSDN remarks
    //       about SetForeGroundWindow and it appears that the same for
    //       BringWindowToTop
    //       The only way...see http://www.tek-tips.com/faqs.cfm?fid=4262
    HWND  hForegndWnd = GetForegroundWindow();
    DWORD dwForegndID = GetWindowThreadProcessId( hForegndWnd, NULL );
    DWORD dwCallingID = GetCurrentThreadId();
    if( dwForegndID != dwCallingID )
    {
        AttachThreadInput( dwForegndID, dwCallingID, TRUE );
        BringWindowToTop( hwnd );
        ShowWindow( hwnd, SW_SHOWNORMAL ); //SW_RESTORE);
        AttachThreadInput( dwForegndID, dwCallingID, FALSE );
    }
    else
    {
        BringWindowToTop( hwnd );
        ShowWindow( hwnd, SW_SHOWNORMAL ); // SW_RESTORE);
    }
#endif
}


HBITMAP HBitmapCopy( HBITMAP hSourceHbitmap )
{
    CDC sourceDC;
    CDC destDC;
    sourceDC.CreateCompatibleDC( NULL );
    destDC.CreateCompatibleDC( NULL );
    //The bitmap information.
    BITMAP bm = { 0 };
    //Get the bitmap information.
    ::GetObject( hSourceHbitmap, sizeof( bm ), &bm );
    // Create a bitmap to hold the result
    HBITMAP hbmResult = ::CreateCompatibleBitmap( CClientDC( NULL ), bm.bmWidth, bm.bmHeight );

    HBITMAP hbmOldSource = ( HBITMAP )::SelectObject( sourceDC.m_hDC, hSourceHbitmap );
    HBITMAP hbmOldDest = ( HBITMAP )::SelectObject( destDC.m_hDC, hbmResult );
    destDC.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &sourceDC, 0, 0, SRCCOPY );

    // Restore DCs
    ::SelectObject( sourceDC.m_hDC, hbmOldSource );
    ::SelectObject( destDC.m_hDC, hbmOldDest );
    ::DeleteObject( sourceDC.m_hDC );
    ::DeleteObject( destDC.m_hDC );

    return hbmResult;
}

BOOL CBitmapCopy( CBitmap* dest, const CBitmap* src )
{
    return dest->Attach( HBitmapCopy( ( *src ) ) );
}

BOOL CBitmapCopy( CBitmap* dest, const CBitmap* src, int width, int height )
{
    CBitmap* dstBmp = CBitmap::FromHandle( ( HBITMAP )CopyImage( ( HBITMAP )(*src), IMAGE_BITMAP, width, height, 0 ) );
    return CBitmapCopy( dest, dstBmp );
}


//////////////////////////////////////////////////////////////////////////
//
#include <strsafe.h>
//RegDelnode(HKEY_CURRENT_USER, TEXT("Software\\Local AppWizard-Generated Applications\\ResEditorXY"));
BOOL RegDelnodeRecurse( HKEY hKeyRoot, LPTSTR lpSubKey )
{
    LPTSTR lpEnd;
    LONG lResult;
    DWORD dwSize;
    TCHAR szName[ MAX_PATH ];
    HKEY hKey;
    FILETIME ftWrite;
    // First, see if we can delete the key without having
    // to recurse.
    lResult = RegDeleteKey( hKeyRoot, lpSubKey );
    if( lResult == ERROR_SUCCESS )
    {
        return TRUE;
    }
    lResult = RegOpenKeyEx( hKeyRoot, lpSubKey, 0, KEY_READ, &hKey );
    if( lResult != ERROR_SUCCESS )
    {
        if( lResult == ERROR_FILE_NOT_FOUND )
        {
            printf( "Key not found.\n" );
            return TRUE;
        }
        else
        {
            printf( "Error opening key.\n" );
            return FALSE;
        }
    }
    // Check for an ending slash and add one if it is missing.
    lpEnd = lpSubKey + lstrlen( lpSubKey );
    if( *( lpEnd - 1 ) != TEXT( '\\' ) )
    {
        *lpEnd = TEXT( '\\' );
        lpEnd++;
        *lpEnd = TEXT( '\0' );
    }
    // Enumerate the keys
    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx( hKey, 0, szName, &dwSize, NULL,
        NULL, NULL, &ftWrite );
    if( lResult == ERROR_SUCCESS )
    {
        do
        {
            StringCchCopy( lpEnd, MAX_PATH * 2, szName );
            if( !RegDelnodeRecurse( hKeyRoot, lpSubKey ) )
            {
                break;
            }
            dwSize = MAX_PATH;
            lResult = RegEnumKeyEx( hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite );
        } while( lResult == ERROR_SUCCESS );
    }
    lpEnd--;
    *lpEnd = TEXT( '\0' );
    RegCloseKey( hKey );
    // Try again to delete the key.
    lResult = RegDeleteKey( hKeyRoot, lpSubKey );
    if( lResult == ERROR_SUCCESS )
    {
        return TRUE;
    }
    return FALSE;
}

BOOL RegDelnode( HKEY hKeyRoot, LPTSTR lpSubKey )
{
    TCHAR szDelKey[ MAX_PATH * 2 ];
    StringCchCopy( szDelKey, MAX_PATH * 2, lpSubKey );
    return RegDelnodeRecurse( hKeyRoot, szDelKey );
}

bool CopyDir( LPCTSTR* source_folder, LPCTSTR* target_folder )
{
    TCHAR sf[ _MAX_PATH + 1 ];
    TCHAR tf[ _MAX_PATH + 1 ];
    _tprintf( sf, _TEXT( "%s\\*" ), source_folder );
    _tcscpy( tf, ( LPCTSTR )target_folder );
    sf[ _tcslen( sf ) + 1 ] = 0;
    tf[ _tcslen( tf ) + 1 ] = 0;
    SHFILEOPSTRUCT s = { 0 };
    s.wFunc = FO_COPY;
    s.pTo = tf;
    s.pFrom = sf;
    s.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI;
    int res = SHFileOperation( &s );
    return res == 0;
}

int RemoveDir( LPCTSTR dir ) // Fully qualified name of the directory being deleted, without trailing backslash
{
    SHFILEOPSTRUCT file_op =
    {
        NULL,
        FO_DELETE,
        dir,
        _TEXT( "" ),
        FOF_NOCONFIRMATION |
        FOF_NOERRORUI |
        FOF_SILENT,
        false,
        0,
        _TEXT( "" )
    };
    return SHFileOperation( &file_op );
}


/*
std::wstring UTF8ToWString( const std::string s )
{
#ifdef _UNICODE
    int len = ( strlen( s.c_str() ) + 1 ) * 4;
    UInt16* unc = new UInt16[len];
    UTF8ToUnicode( unc, s.c_str(), len );
    std::wstring rr = ( WCHAR* )unc;
    delete [] unc;
    return rr;
#else
    return L"";
#endif
}

std::string WStringToUTF8( const std::wstring ws )
{
#ifdef _UNICODE
    int len = ( wcslen( ws.c_str() ) + 1 ) * 4;
    char* s = new char[len];
    UnicodeToUTF8( s, ( UInt16* )ws.c_str(), len );
    std::string rr = s;
    delete [] s;
    return rr;
#else
    return "";
#endif
}


std::wstring UTF8ToUnicode( const std::string s )
{
    return MultiByteToUnicode( s, "utf8" );
}

std::string UnicodeToUTF8( const std::wstring ws )
{
    return UnicodeToMultiByte( ws, "utf8" );
}

std::wstring JISToUnicode( const std::string s )
{
    return MultiByteToUnicode( s, "jis" );
}

std::string UnicodeToJIS( const std::wstring ws )
{
    return UnicodeToMultiByte( ws, "jis" );
}

std::wstring MultiByteToUnicode( const std::string s, const char* codepage )
{
    int len = ( s.size() + 1 ) * 2;
    //wchar_t* strW = new wchar_t[len];
    std::vector<wchar_t> unic( len );
    mtow( s.c_str(), &unic[0], len, codepage );
    //std::wstring ws = strW;
    //SAFE_DELETE_ARRAY( strW );
    return &unic[0];
}

std::string UnicodeToMultiByte( const std::wstring ws, const char* codepage )
{
    // 因為如果是 muti-byte 至少要有兩個byte
    int len = ( ws.size() + 1 ) * 2;
    //char_t* strA = new char_t[len];
    std::vector<char> utf8( len );
    wtom( ws.c_str(), &utf8[0], len, codepage );
    //std::string ss = strA;
    //SAFE_DELETE_ARRAY( strA );
    return &utf8[0];
}

std::string UTF8ToUTF8BOM( const std::string s )
{
    int len = s.size() + 10;
    std::vector<char> utf8( len );
    UTF8ToUTF8BOM( &utf8[0], s.c_str() );
    return &utf8[0];
}
*/

void killProcessByName( const TCHAR* filename )
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPALL, NULL );
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof( pEntry );
    BOOL hRes = Process32First( hSnapShot, &pEntry );
    while( hRes )
    {
        if( _tcscmp( pEntry.szExeFile, filename ) == 0 )
        {
            HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, 0,
                ( DWORD )pEntry.th32ProcessID );
            if( hProcess != NULL )
            {
                TerminateProcess( hProcess, 9 );
                CloseHandle( hProcess );
            }
        }
        hRes = Process32Next( hSnapShot, &pEntry );
    }
    CloseHandle( hSnapShot );
}

void CreateStretchImage( CImage *pImage, CImage *ResultImage, int StretchHeight, int StretchWidth )
{
    if( pImage->IsDIBSection() )
    {
        // 取得 pImage 的 DC
        CDC* pImageDC1 = CDC::FromHandle( pImage->GetDC() ); // Image 因為有自己的 DC, 所以必須使用 FromHandle 取得對應的 DC

        CBitmap *bitmap1 = pImageDC1->GetCurrentBitmap();
        BITMAP bmpInfo;
        bitmap1->GetBitmap( &bmpInfo );

        // 建立新的 CImage
        ResultImage->Create( StretchWidth, StretchHeight, bmpInfo.bmBitsPixel );
        CDC* ResultImageDC = CDC::FromHandle( ResultImage->GetDC() );

        // 當 Destination 比較小的時候, 會根據 Destination DC 上的 Stretch Blt mode 決定是否要保留被刪除點的資訊
        ResultImageDC->SetStretchBltMode( HALFTONE ); // 使用最高品質的方式
        ::SetBrushOrgEx( ResultImageDC->m_hDC, 0, 0, NULL ); // 調整 Brush 的起點

        // 把 pImage 畫到 ResultImage 上面
        StretchBlt( *ResultImageDC, 0, 0, StretchWidth, StretchHeight, *pImageDC1, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY );
        // pImage->Draw(*ResultImageDC,0,0,StretchWidth,StretchHeight,0,0,pImage->GetWidth(),pImage->GetHeight());

        pImage->ReleaseDC();
        ResultImage->ReleaseDC();
    }
}


