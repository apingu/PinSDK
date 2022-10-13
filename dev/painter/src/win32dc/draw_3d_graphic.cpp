//Draw_3D_Graphic.cpp
//
//
//      use 2D to simulat 3D graphic
//
//
//
//
//
//

#include "Draw_Graphic.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Draw 3D function



//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Rect(HDC hDC, int x1, int y1, int x2, int y2, int z1, int z3, COLORREF color, int linewidth, int fnObject)
{
    HPEN hPen = CreatePen(PS_SOLID, linewidth, color);
    HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
    SelectObject(hDC, GetStockObject(fnObject));
    SelectObject( hDC, hPen );
    DeleteObject(hPen);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Rhomb(HDC hDC, int CenterX, int CenterY, int Width, int Height, int Tall, COLORREF color, int linewidth)
{



}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Shape(HDC hDC, int CenterX, int CenterY, int Width, int Height, int Tall, COLORREF color, int linewidth, int FloorShap)
{




}



























//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Rect(HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth, int fnObject)
{
    LG::Point Upbr_array[4];
    Upbr_array[0](rect.left,  rect.top + Tall);
    Upbr_array[1](rect.right, rect.top + Tall);
    Upbr_array[2](rect.right, rect.bottom + Tall);
    Upbr_array[3](rect.left,  rect.bottom + Tall);


    HPEN hPen = CreatePen(PS_DASH, linewidth, color);
    HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
    SelectObject(hDC, GetStockObject(fnObject));
    Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
    Line_To(hDC, rect.p_l_t(), Upbr_array[0]);
    Line_To(hDC, rect.p_r_t(), Upbr_array[1]);
    

    HPEN hPen2 = CreatePen(PS_SOLID, linewidth, color);
    SelectObject(hDC, hPen2);
    DeleteObject(hPen);


    Polygon(hDC, Upbr_array, 4);
    Line_To(hDC, rect.p_r_b(), Upbr_array[2]);
    Line_To(hDC, rect.p_l_b(), Upbr_array[3]);

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen2);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Rhomb(HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth)
{
    LG::Quadr quadr = rect.change_Quadr();
    
    LG::Point Upbr_array[4];
    Upbr_array[0](quadr[0].x,  quadr[0].y + Tall);
    Upbr_array[1](quadr[1].x,  quadr[1].y + Tall);
    Upbr_array[2](quadr[2].x,  quadr[2].y + Tall);
    Upbr_array[3](quadr[3].x,  quadr[3].y + Tall);

    // draw
    HPEN hPen = CreatePen(PS_DASH, linewidth, color);
    HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
    Quadr_To(hDC, quadr[0], quadr[1], quadr[2], quadr[3]);
    Line_To(hDC,  quadr[0],  Upbr_array[0]);
    Line_To(hDC,  quadr[1],  Upbr_array[1]);
    Line_To(hDC,  quadr[2],  Upbr_array[2]);
    
    HPEN hPen2 = CreatePen(PS_SOLID, linewidth, color);
    SelectObject(hDC, hPen2);
    DeleteObject(hPen);

    Polygon(hDC, Upbr_array, 4);
    Line_To(hDC, quadr[3], Upbr_array[3]);

    SelectObject(hDC, hOldPen);
    DeleteObject(hPen2);

}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void Draw_3D_Octagon (HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth)
{
    LG::Octagon octagon = rect.change_Octagon();

    LG::Point Upbr_array[8];
    Upbr_array[0](octagon[0].x, octagon[0].y + Tall);
    Upbr_array[1](octagon[1].x, octagon[1].y + Tall);
    Upbr_array[2](octagon[2].x, octagon[2].y + Tall);
    Upbr_array[3](octagon[3].x, octagon[3].y + Tall);
    Upbr_array[4](octagon[4].x, octagon[4].y + Tall);
    Upbr_array[5](octagon[5].x, octagon[5].y + Tall);
    Upbr_array[6](octagon[6].x, octagon[6].y + Tall);
    Upbr_array[7](octagon[7].x, octagon[7].y + Tall);


    HPEN hPen = CreatePen(PS_DASH, linewidth, color);
    HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

    Octagon_To(hDC, octagon[0], octagon[1], octagon[2], octagon[3],
                    octagon[4], octagon[5], octagon[6], octagon[7]);
    Line_To(hDC, octagon[0],    Upbr_array[0]);
    Line_To(hDC, octagon[1],    Upbr_array[1]);
    Line_To(hDC, octagon[2],    Upbr_array[2]);
    Line_To(hDC, octagon[3],    Upbr_array[3]);
    Line_To(hDC, octagon[4],    Upbr_array[4]);


    HPEN hPen2 = CreatePen(PS_SOLID, linewidth, color);
    SelectObject(hDC, hPen2);
    DeleteObject(hPen);

    Polygon(hDC, Upbr_array, 8);
    Line_To(hDC, octagon[5], Upbr_array[5]);
    Line_To(hDC, octagon[6],       Upbr_array[6]);
    Line_To(hDC, octagon[7],  Upbr_array[7]);

    SelectObject(hDC, hOldPen);    
    DeleteObject(hPen2);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////
void  Draw_3D_Shape(HDC hDC, LG::Rect rect, int Tall, COLORREF color, int linewidth, int FloorShap)
{
    switch(FloorShap)
    {
        case _P_RHOMBUS_:
                Draw_3D_Rhomb(hDC, rect, Tall, color, linewidth);
                break;
        case _P_OCTAGON_:
                Draw_3D_Octagon(hDC, rect, Tall, color, linewidth);
                break;
        default:
                Draw_3D_Rect(hDC, rect, Tall, color, linewidth);
                break;
    }
    return;
}