//EditerDraw.cpp
//
//  draw shape in gdi mode
//
//
//
//
//
//                                                         Pin
//
#include <math.h>

#include "Draw_Graphic.h"

//#include <pstring.h>

//#define Line_pTo(dc, p1, p2)         {MoveToEx(dc, p1.x, p1.y, NULL);LineTo(dc, p2.x, p2.y);}
//#define Line_iTo(dc, x1, y1, x2, y2) {MoveToEx(dc, x1, y1, NULL);LineTo(dc, x2, y2);}


//============================================================================
//
//
//=============================================================================
void Draw_Pixel( HDC dDC, int x, int y, COLORREF color )
{
    SetPixel( dDC, x, y, color );
    return;
}


//=============================================================================
//
//
//=============================================================================
void Draw_Pixel( HDC dDC, LG::Point pfstl, COLORREF color )
{
    SetPixel( dDC, pfstl.x, pfstl.y, color );
    return;
}


//=============================================================================
//
//
//=============================================================================

void Draw_Line( HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, int linewidth )
{
    //if (hDC == 0)
    //  return;
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Line_To( hDC, x1, y1, x2, y2 );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}



//=============================================================================
//
//
//=============================================================================
void Draw_Line ( HDC hDC, LG::Point pt1, LG::Point pt2, COLORREF color, int linewidth )
{
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Line_To( hDC, pt1, pt2 );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}


//=============================================================================
//
//
//=============================================================================
void Draw_Triangle( HDC dDC, int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color, int linewidth )
{
}


//=============================================================================
//
//
//=============================================================================
void Draw_Triangle( HDC dDC, LG::Point pt1, LG::Point pt2, LG::Point pt3, COLORREF color, int linewidth )
{
}


//=============================================================================
//
//
//=============================================================================
void Draw_Triangle( HDC dDC, LG::Triangle2d triangle, COLORREF color, int linewidth )
{
}


//=============================================================================
//
//
//=============================================================================
void Draw_Circle( HDC hDC, LG::Point center, int radius, COLORREF color, int linewidth )
{
    HPEN hPen = CreatePen( PS_SOLID, 1, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
    Ellipse( hDC,   // handle to DC
             center.x - radius,  // x-coord of upper-left corner of rectangle
             center.y - radius,  // y-coord of upper-left corner of rectangle
             center.x + radius,  // x-coord of lower-right corner of rectangle
             center.y + radius ); // y-coord of lower-right corner of rectangle
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Rect( HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, int linewidth, int fnObject )
{
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    SelectObject( hDC, GetStockObject( fnObject ) );
    Rectangle( hDC, x1, y1, x2, y2 );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}

void Draw_Rect( HDC hDC, LG::Rect rect, COLORREF color, int linewidth, int fnObject )
{
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    SelectObject( hDC, GetStockObject( fnObject ) );
    Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Rhomb( HDC hDC, int CenterX, int CenterY, int Width, int Height, COLORREF color, int linewidth )
{
    int MRX    = ( Width  / 2 );
    int MRY    = ( Height / 2 );
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Quadr_To( hDC, CenterX - MRX, CenterY, CenterX, CenterY - MRY,
              CenterX + MRX, CenterY, CenterX, CenterY + MRY );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Rhomb( HDC hDC, LG::Rect rect, COLORREF color, int linewidth )
{
    LG::Quadr quadr = rect.change_Quadr();
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Quadr_To( hDC, quadr[0], quadr[1], quadr[2], quadr[3] );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Octagon( HDC hDC, LG::Rect rect, COLORREF color, int linewidth )
{
    LG::Octagon octagon = rect.change_Octagon();
    HPEN hPen2 = CreatePen( PS_SOLID, linewidth, color );
    if( hPen2 == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen2 );
    Octagon_To( hDC, octagon[0], octagon[4], octagon[1], octagon[5],
                octagon[2], octagon[6], octagon[3], octagon[7] );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen2 );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
bool Draw_Shape( HDC hDC, int CenterX, int CenterY, int Width, int Height, COLORREF color, int linewidth, int FloorShap )
{
    //if (hDC == 0)
    //  return false;
    switch( FloorShap )
    {
    case _P_POINT_:
        {
            Draw_Grid_Point( hDC, CenterX, CenterY, Width, Height );
            break;
        }
    case _P_RECT_:
        {
            int h_w = Width  / 2;
            int h_h = Height / 2;
            Draw_Rect( hDC, CenterX - h_w, CenterY - h_h, CenterX + h_w, CenterY + h_h, color, linewidth );
            break;
        }
    case _P_RHOMBUS_:
        {
            Draw_Rhomb( hDC, CenterX, CenterY, Width, Height, color, linewidth );
            break;
        }
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
bool Draw_Shape( HDC hDC, LG::Rect rect, COLORREF color, int linewidth, int FloorShap )
{
    switch( FloorShap )
    {
    case _P_RHOMBUS_:
        Draw_Rhomb( hDC, rect, color, linewidth );
        //Draw_Rhomb(hDC, rect, color, linewidth);
        break;
    case _P_OCTAGON_:
        Draw_Octagon( hDC, rect, color, linewidth );
        break;
    default:
        Draw_Rect( hDC, rect, color, linewidth );
        break;
    }
    return true;
}




//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Grid_Point( HDC hDC, int CenterX, int CenterY, int Width, int Height )
{
    //if (hDC == 0)
    //  return;
    int MRX     = ( Width  / 2 );
    int MRY     = ( Height / 2 );
    int FloorW = ( MRX / 2 );
    int FloorH = ( MRY / 2 );
    ///////////////////////////////////////////////////////////
    HPEN hPen = CreatePen( PS_SOLID, 1, RGB( 0, 0XFF, 0 ) );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Line_To( hDC, CenterX - MRX, CenterY, CenterX + MRX, CenterY );
    Line_To( hDC, CenterX, CenterY - MRY, CenterX, CenterY + MRY );
    HPEN hPen1 = CreatePen( PS_SOLID, 1, RGB( 0XFF, 0, 0 ) );
    if( hPen1 == 0 )
    { return; }
    SelectObject( hDC, hPen1 );
    DeleteObject( hPen );
    Quadr_To( hDC, CenterX - FloorW, CenterY - FloorH, CenterX + FloorW, CenterY + FloorH,
              CenterX - FloorW, CenterY + FloorH, CenterX - FloorW, CenterY - FloorH );
    HPEN hPen2 = CreatePen( PS_SOLID, 1, RGB( 0, 0xff, 0xff ) );
    if( hPen2 == 0 )
    { return; }
    SelectObject( hDC, hPen2 );
    DeleteObject( hPen1 );
    Quadr_To( hDC, CenterX - MRX , CenterY - MRY, CenterX + MRX , CenterY - MRY,
              CenterX + MRX , CenterY + MRY, CenterX - MRX , CenterY + MRY );
    DeleteObject( hPen2 );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen2 );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Grid_Rect( HDC hDC, int CenterX, int CenterY, int Width, int Height )
{
    //if (hDC == 0)
    //  return;
    int MRX     = ( Width  / 2 );
    int MRY     = ( Height / 2 );
    int FloorW = ( MRX / 2 );
    int FloorH = ( MRY / 2 );
    ///////////////////////////////////////////////////////////
    HPEN hPen = CreatePen( PS_SOLID, 1, RGB( 0, 0XFF, 0 ) );
    if( hPen != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
        Line_To( hDC, CenterX - MRX, CenterY, CenterX + MRX, CenterY );
        Line_To( hDC, CenterX, CenterY - MRY, CenterX, CenterY + MRY );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen );
    }
    HPEN hPen1 = CreatePen( PS_SOLID, 5, RGB( 0, 0xff, 0 ) );
    if( hPen1 != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen1 );
        Quadr_To( hDC, CenterX - FloorW , CenterY - FloorH,
                  CenterX + FloorW , CenterY - FloorH,
                  CenterX + FloorW , CenterY + FloorH,
                  CenterX - FloorW , CenterY + FloorH );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen1 );
    }
    HPEN hPen2 = CreatePen( PS_SOLID, 2, RGB( 0, 0xff, 0xff ) );
    if( hPen2 != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen2 );
        Quadr_To( hDC, CenterX - MRX , CenterY - MRY,
                  CenterX + MRX , CenterY - MRY,
                  CenterX + MRX , CenterY + MRY,
                  CenterX - MRX , CenterY + MRY );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen2 );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void  Draw_Grid_Rhomb( HDC hDC, int CenterX, int CenterY, int Width, int Height )
{
    //if (hDC == 0)
    //  return;
    int MRX    = ( Width  / 2 );
    int MRY    = ( Height / 2 );
    int FloorW = ( MRX / 2 );
    int FloorH = ( MRY / 2 );
    //HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0xff, 200, 100));
    HPEN hPen = CreatePen( PS_SOLID, 2, RGB( 200, 150, 150 ) );
    if( hPen != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
        Line_To( hDC, CenterX + FloorW, CenterY + FloorH, CenterX - FloorW, CenterY - FloorH );
        Line_To( hDC, CenterX - FloorW, CenterY + FloorH, CenterX + FloorW, CenterY - FloorH );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen );
    }
    //HPEN hPen1 = CreatePen(PS_SOLID, 5, RGB(0, 200, 100));
    HPEN hPen1 = CreatePen( PS_SOLID, 5, RGB( 150, 200, 150 ) );
    if( hPen1 != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen1 );
        Quadr_To( hDC, CenterX - FloorW, CenterY, CenterX, CenterY - FloorH,
                  CenterX + FloorW, CenterY, CenterX, CenterY + FloorH );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen1 );
    }
    //HPEN hPen2 = CreatePen(PS_SOLID, 2, RGB(100 , 200, 0xff));
    HPEN hPen2 = CreatePen( PS_SOLID, 2, RGB( 150 , 150, 200 ) );
    if( hPen2 != 0 )
    {
        HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen2 );
        Quadr_To( hDC, CenterX - MRX, CenterY, CenterX, CenterY - MRY,
                  CenterX + MRX, CenterY, CenterX, CenterY + MRY );
        SelectObject( hDC, hOldPen );
        DeleteObject( hPen2 );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
bool Draw_Grid( HDC hDC, int CenterX, int CenterY, int Width, int Height , int FloorShap )
{
    //if (hDC == 0)
    //  return false;
    switch( FloorShap )
    {
    case _P_POINT_:
        Draw_Grid_Point( hDC, CenterX, CenterY, 10, 10 );
        break;
    case _P_RECT_:
        Draw_Grid_Rect( hDC, CenterX, CenterY, Width, Height );
        break;
    case _P_RHOMBUS_:
        Draw_Grid_Rhomb( hDC, CenterX, CenterY, Width, Height );
        break;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_X( HDC hDC, int CenterX, int CenterY, int Width, int Height, COLORREF color )
{
    //if (hDC == 0)
    //  return;
    int helfW = Width  / 2;
    int helfH = Height / 2;
    HPEN hPen = CreatePen( PS_SOLID, 1, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    Line_To( hDC, CenterX - helfW, CenterY - helfH, CenterX + helfW, CenterY + helfH );
    Line_To( hDC, CenterX + helfW, CenterY - helfH, CenterX - helfW, CenterY + helfH );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Net( HDC hDC, LG::Rect rect, int Width, int Height, COLORREF color, int Shape, int linewidth, int fnObject )
{
    //if (hDC == 0)
    //  return;
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    SelectObject( hDC, GetStockObject( fnObject ) );
    if( Shape == _P_RECT_ )
    {
        for( int i = rect.left; i < rect.right; i = i + Width )
        {
            Line_To( hDC, i, rect.top, i, rect.bottom );
        }
        for( int j = rect.top; j < rect.bottom; j = j + Height )
        {
            Line_To( hDC, rect.left, j, rect.right, j );
        }
    }
    else if( Shape == _P_RHOMBUS_ )
    {
        float ratio = ( float )Width / ( float )Height;
        int wLv = ( int )ceil( ( double )( rect.width() / Width ) ) ;
        int hLv = ( int )ceil( ( double )( rect.height() / Height ) ) + 1 ;
        LG::Rect Drawrect( rect.left, rect.top, rect.left + ( Width * wLv ), rect.top + ( Height * hLv ) );
        int dw = Drawrect.width();
        int dh = Drawrect.height();
        int linew = ( int )( dh * ratio );
        for( int i = Drawrect.left; i <= Drawrect.right; i = i + Width )
        {
            Line_To( hDC, i, Drawrect.top, i + linew, Drawrect.bottom );
            Line_To( hDC, i, Drawrect.bottom, i + linew, Drawrect.top );
        }
        for( int i = ( Drawrect.top + Height ); i <= Drawrect.bottom; i = i + Height )
        {
            Line_To( hDC, Drawrect.left, i, Drawrect.left + ( int )( ( Drawrect.bottom - i ) * ratio ), Drawrect.bottom );
            Line_To( hDC, Drawrect.left, i, Drawrect.left + ( int )( ( i - Drawrect.top ) * ratio ), Drawrect.top );
        }
    }
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Stop( HDC hDC, LG::Point pfstl, int radius, COLORREF color )
{
    //if (hDC == 0)
    //  return;
    if( radius <= 4 )
    {
        //      SetPixel( hDC, pfstl.x - radius, pfstl.y - (radius << 1), color );
        SetTextColor( hDC, color );
        TextOut( hDC, pfstl.x - radius, pfstl.y - ( radius << 1 ), "x", 1 );
        return;
    }
    if( radius <= 8 )
    {
        SetTextColor( hDC, color );
        TextOut( hDC, pfstl.x - radius, pfstl.y - ( radius << 1 ), "x", 1 );
        return;
    }
    HPEN hPen = CreatePen( PS_SOLID, 1, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
    Ellipse( hDC,   // handle to DC
             pfstl.x - radius,  // x-coord of upper-left corner of rectangle
             pfstl.y - radius,  // y-coord of upper-left corner of rectangle
             pfstl.x + radius,  // x-coord of lower-right corner of rectangle
             pfstl.y + radius ); // y-coord of lower-right corner of rectangle
    //DeleteObject(hPen);
    /*
    int ra = radius/2 + 4;
    MoveToEx(hDC, pfstl.x + ra, pfstl.y - ra, NULL);
    LineTo  (hDC, pfstl.x - ra, pfstl.y + ra);
    */
    int ra = radius - 4;
    if( ra <= 0 )
    { return; }
    //hPen = CreatePen(PS_SOLID, 4, color);
    //SelectObject(hDC, hPen);
    //SelectObject(hDC, GetStockObject(NULL_BRUSH));
    MoveToEx( hDC, pfstl.x - radius, pfstl.y, NULL );
    LineTo  ( hDC, pfstl.x + radius, pfstl.y );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Brackets( HDC hDC, LG::Rect rect, COLORREF color, float rr, int linewidth )
{
    HPEN hPen = CreatePen( PS_SOLID, linewidth, color );
    if( hPen == 0 )
    { return; }
    HPEN hOldPen = ( HPEN )SelectObject( hDC, hPen );
    int ll =  ( ( float )__min( rect.width(), rect.height() ) ) * rr;
    Line_To( hDC, rect.left    , rect.top, rect.left + ll, rect.top );
    Line_To( hDC, rect.right - ll, rect.top, rect.right  , rect.top );
    Line_To( hDC, rect.right, rect.top      , rect.right, rect.top + ll );
    Line_To( hDC, rect.right, rect.bottom - ll, rect.right, rect.bottom );
    Line_To( hDC, rect.left    , rect.bottom, rect.left + ll, rect.bottom );
    Line_To( hDC, rect.right - ll, rect.bottom, rect.right  , rect.bottom );
    Line_To( hDC, rect.left, rect.top      , rect.left, rect.top + ll );
    Line_To( hDC, rect.left, rect.bottom - ll, rect.left, rect.bottom );
    SelectObject( hDC, hOldPen );
    DeleteObject( hPen );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
HFONT Draw_FontCreate( HDC dc, const char* font_type, int s_w, int s_h )
{
    HFONT hfont = CreateFont( s_w, s_h, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                              DEFAULT_PITCH, font_type );
    return ( HFONT )SelectObject( dc, hfont );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Text( HDC dc, int x, int y, LPCTSTR string, COLORREF col )
{
    if( string == 0 )
    { return; }
    SetBkMode( dc, TRANSPARENT );
    SetTextColor( dc, col );
    TextOut( dc, x, y, string, strlen( string ) );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_FontDestroy( HDC dc, HFONT hfont )
{
    HFONT font = ( HFONT )SelectObject( dc, hfont );
    DeleteObject( font );
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_Bitmap( HDC hdc, int x1, int y1, HBITMAP hbitmap )
{
    HDC hdcmem = CreateCompatibleDC( hdc );
    HBITMAP hOld = ( HBITMAP )SelectObject( hdcmem, hbitmap );
    BITMAP bitmap;
    GetObject( hbitmap, sizeof( bitmap ), &bitmap );
    BitBlt( hdc, x1, y1, bitmap.bmWidth, bitmap.bmHeight, hdcmem, 0, 0, SRCCOPY );
    SelectObject( hdcmem, hOld );
}

HBITMAP HIconToHBitmap( HICON hIcon )
{
    ICONINFO info;
    ::GetIconInfo( hIcon,   &info );
    BITMAP   bmp;
    ::GetObject( info.hbmColor, sizeof( BITMAP ), ( LPVOID )&bmp );
    HBITMAP hBmp = ( HBITMAP )::CopyImage( info.hbmColor, IMAGE_BITMAP, 0, 0, 0 );
    ::DeleteObject( info.hbmColor );
    ::DeleteObject( info.hbmMask );
    return hBmp;
}




























//////////////////////////////////////////////////////////////////////////////////////////
// other


//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
/*
void Anayfile(const char *path)
{
    FILE* f;

    if((f = fopen(path, "rb")) == NULL)
        return ;

    int filesize = filelength(fileno(f));
    char* fileptr = new char[filesize];
    fread(fileptr, filesize, 1, f);

    char String[_MAX_STRING_SIZE][_MAX_STRING_LEN];
    int size = divstr(fileptr, filesize, String);
}
*/
//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
/*
void WriteData(FILE *f, const char *type, const char *value)
{
    if(strcmp(type, "BYTE") == 0)
        fwrite((BYTE*)value, sizeof(BYTE), 1, f);

    else if(strcmp(type, "WORD") == 0)
    {
        WORD v = (WORD)atoi(value);
        fwrite(&v, sizeof(WORD), 1, f);
    }

    else if(strcmp(type, "DWORD") == 0)
    {
        DWORD v = (DWORD)atoi(value);
        fwrite(&v, sizeof(DWORD), 1, f);
    }

    else if(strcmp(type, "char") == 0)
        fwrite((char*)value, sizeof(char), 1, f);

    else if(strcmp(type, "short") == 0)
    {
        short v = (short)atoi(value);
        fwrite(&v, sizeof(short), 1, f);
    }

    else if(strcmp(type, "int") == 0)
    {
        int v = atoi(value);
        fwrite(&v, sizeof(int), 1, f);
    }

    else if(strcmp(type, "long") == 0)
    {
        long v = atoi(value);
        fwrite(&v, sizeof(long), 1, f);
    }

    else if(strcmp(type, "bool") == 0)
    {
        bool bvalue = false;
        if(strcmp(value, "true") == 0)
            bvalue = true;
        fwrite(&bvalue, sizeof(bool), 1, f);
    }
    else if(strcmp(type, "float") == 0)
        fwrite((double*)atof((char*)value), sizeof(double), 1, f);

}
*/



