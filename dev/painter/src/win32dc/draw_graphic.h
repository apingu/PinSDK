//GraphicDraw.h    header file of GarphicDraw.cpp
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
//
//
//                                              Pin
//                                                           2001/11/01

#ifndef  _EditerDraw_H_
#define  _EditerDraw_H_


#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
//#include <math.h>
#include <graphic2D.h>


/////////////////////////////////////////////////////////////////////////////////////////////////
//  draw 2D graphic

//////////////////////////////////////////////////////////////////////////////////////////////////
//

inline void Line_To     ( HDC dc, LG::Point p1, LG::Point p2 )     { MoveToEx( dc, p1.x, p1.y, NULL ); LineTo( dc, p2.x, p2.y ); }
inline void Line_To     ( HDC dc, int x1, int y1, int x2, int y2 ) { MoveToEx( dc, x1, y1, NULL ); LineTo( dc, x2, y2 ); }
inline void Quadr_To    ( HDC dc, LG::Point p1, LG::Point p2, LG::Point p3, LG::Point p4 ) {LG::Point b_a[4]; b_a[0] = p1; b_a[1] = p2; b_a[2] = p3; b_a[3] = p4; Polygon( dc, b_a, 4 );}
inline void Quadr_To    ( HDC dc, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 ) {LG::Point b_a[4]; b_a[0]( x1, y1 ); b_a[1]( x2, y2 ); b_a[2]( x3, y3 ); b_a[3]( x4, y4 ); Polygon( dc, b_a, 4 );}
inline void Octagon_To  ( HDC dc, LG::Point p1, LG::Point p2, LG::Point p3, LG::Point p4, LG::Point p5, LG::Point p6, LG::Point p7, LG::Point p8 )
{LG::Point b_a[8]; b_a[0] = p1; b_a[1] = p2; b_a[2] = p3; b_a[3] = p4; b_a[4] = p5; b_a[5] = p6; b_a[6] = p7; b_a[7] = p8; Polygon( dc, b_a, 8 );}

void   Draw_Pixel       ( HDC hDC, int x, int y, COLORREF color );
void   Draw_Pixel       ( HDC hDC, LG::Point pfstl, COLORREF color );
                        
void   Draw_Line        ( HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, int linewidth = 1 );
void   Draw_Line        ( HDC hDC, LG::Point pt1, LG::Point pt2, COLORREF color, int linewidth = 1 );
                        
void   Draw_Triangle    ( HDC hDC, int x1, int y1, int x2, int y2, int x3, int y3, COLORREF color, int linewidth = 1 );
void   Draw_Triangle    ( HDC hDC, LG::Point pt1, LG::Point pt2, LG::Point pt3, COLORREF color, int linewidth = 1 );
void   Draw_Triangle    ( HDC hDC, LG::Triangle2d triangle, COLORREF color, int linewidth = 1 );
void   Draw_Circle      ( HDC hDC, LG::Point center, int radius, COLORREF color, int linewidth = 1 );
                        
void   Draw_Rect        ( HDC hDC, int x1, int y1, int x2, int y2, COLORREF color, int linewidth = 1, int fnObject = NULL_BRUSH );
void   Draw_Rhomb       ( HDC hDC, int CenterX, int CenterY, int Width, int Height, COLORREF color, int linewidth = 1 );
bool   Draw_Shape       ( HDC hDC, int CenterX, int CenterY, int Width, int Height, COLORREF color, int linewidth, int FloorShap );
                        
void   Draw_Rect        ( HDC hDC, LG::Rect rect, COLORREF color, int linewidth = 1, int fnObject = NULL_BRUSH );
void   Draw_Rhomb       ( HDC hDC, LG::Rect rect, COLORREF color, int linewidth = 1 );
void   Draw_Octagon     ( HDC hDC, LG::Rect rect, COLORREF color, int linewidth = 1 );
bool   Draw_Shape       ( HDC hDC, LG::Rect rect, COLORREF color, int linewidth, int FloorShap );
                        
                        
void   Draw_Grid_Point  ( HDC hDC, int CenterX, int CenterY, int Width, int Height );
void   Draw_Grid_Rhomb  ( HDC hDC, int CenterX, int CenterY, int Width, int Height );
bool   Draw_Grid        ( HDC hDC, int CenterX, int CenterY, int Width, int Height , int FloorShap );
                        
void   Draw_X           ( HDC hDC, int CenterX,   int CenterY, int Width, int Height, COLORREF color = RGB( 0XFF, 100, 0 ) );
void   Draw_Net         ( HDC hDC, LG::Rect rect, int Width, int Height, COLORREF color, int Shape = 2, int linewidth = 1, int fnObject = NULL_BRUSH );
void   Draw_Stop        ( HDC hDC, LG::Point pfstl,  int radius, COLORREF color );
void   Draw_Brackets    ( HDC hDC, LG::Rect rect, COLORREF color, float rr = 0.2f, int linewidth = 1 ); // [ ]

/////////////////////////////////////////////////////////////////////////////////////////////////
//  draw 3DD graphic

void   Draw_3D_Rect     ( HDC hDC, int x1, int y1, int x2, int y2, int z1, int z2, COLORREF color, int linewidth = 1, int fnObject = NULL_BRUSH );
void   Draw_3D_Rhomb    ( HDC hDC, int CenterX, int CenterY, int Width, int Height, int Tall, COLORREF color, int linewidth = 1 );
void   Draw_3D_Shape    ( HDC hDC, int CenterX, int CenterY, int Width, int Height, int Tall, COLORREF color, int linewidth, int FloorShap );
                        
void   Draw_3D_Rect     ( HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth = 1, int fnObject = NULL_BRUSH );
void   Draw_3D_Rhomb    ( HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth = 1 );
void   Draw_3D_Octagon  ( HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth = 1 );
void   Draw_3D_Shape    ( HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth, int FloorShap );
                        
                        
HFONT  Draw_FontCreate  ( HDC dc, const char* font_type, int s_w, int s_h );
void   Draw_Text        ( HDC dc, int x, int y, LPCTSTR string, COLORREF col = 0 );
void   Draw_FontDestroy ( HDC dc, HFONT hfont );
                        
void   Draw_Bitmap      ( HDC dc, int x1, int y1, HBITMAP bitmap );
                        
HBITMAP HIconToHBitmap  ( HICON hIcon );





/////////////////////////////////////////////////////////////////////////////////////////////////
//  draw 3DD graphic

//void   Draw_3D_ShapeB   (HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth, int FloorShap);
//void   Draw_3D_ShapeF   (HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth, int FloorShap);




#endif