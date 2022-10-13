#ifndef WIN32DRAWBITMAP_H
#define WIN32DRAWBITMAP_H

inline void DrawBitmap( HDC hDC, HBITMAP hbitmap, int X=0, int Y=0 )
{
    HDC hDCMem=CreateCompatibleDC(hDC);
                
    HBITMAP hBitmapOld=(HBITMAP)SelectObject( hDCMem, hbitmap );
    BITMAP bitmap;
    GetObject( hbitmap, sizeof(bitmap), &bitmap );

    //BitBlt( hDC, nLeft, nTop, bitmap.bmWidth, bitmap.bmHeight, hDCMem, 0, 0, SRCCOPY );
    BitBlt( hDC, X, Y, bitmap.bmWidth, bitmap.bmHeight, hDCMem, 0, 0, SRCCOPY );

    SelectObject( hDCMem, hBitmapOld );
}

inline void StretchDrawBitmap( HDC hDC, HBITMAP hbitmap, int X, int Y, int W, int H )
{
    HDC hDCMem=CreateCompatibleDC(hDC);
        
    HBITMAP hBitmapOld=(HBITMAP)SelectObject( hDCMem, hbitmap );
    BITMAP bitmap;
    GetObject( hbitmap, sizeof(bitmap), &bitmap );

    StretchBlt( hDC, X, Y,
                W,
                H,
                hDCMem,
                0, 0,                                                                             
                bitmap.bmWidth,
                bitmap.bmHeight,
                SRCCOPY );
    SelectObject( hDCMem, hBitmapOld );
}

inline void TransparentDrawBitmap( HDC hDC, HBITMAP hbitmap, RECT rect, UINT crTransparent )
{
    HDC hDCMem=CreateCompatibleDC(hDC);
                        
    HBITMAP hBitmapOld=(HBITMAP)SelectObject( hDCMem, hbitmap );
    BITMAP bitmap;
    GetObject( hbitmap, sizeof(bitmap), &bitmap );

    TransparentBlt( hDC, 0, 0,
                    (rect.right-rect.left),
                    (rect.bottom-rect.top),
                    hDCMem,
                    0, 0,                                                                             
                    bitmap.bmWidth,
                    bitmap.bmHeight,
                    crTransparent );
    SelectObject( hDCMem, hBitmapOld );
}

#endif// WIN32DRAWBITMAP_H
