//
//
//
//
//     not support 32bits mode yet
//
//
//
//
//
//
//
//                                               Copyright    (C)    1999    Pin
//
#include <math.h>
#include <memio.inl>
#include "vmc-draw.h"
#include "vmc-matrix.h"
#include "vmc-clipper.h"


namespace LGB
{
    extern  word_t _PIXEL_WIDTH_SHIFT;

    namespace VMCDraw
    {
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Pixel(LG::Point point, COLORTYPE color, CMemCanvas *Dest)
        {
            LG::Rect rt (0, 0, Dest->GetClipWidth() - 1, Dest->GetClipHeight() - 1);
            if(!PA::Intersect::Test(&rt, &point))
                return;

            //將目的記憶體的控制指標移至第一行開始的位置
            __pfmemcpy(Dest->GetPixel() +
                     (point.y * (Dest->GetClipWidth() << _PIXEL_WIDTH_SHIFT)) +
                     (point.x << _PIXEL_WIDTH_SHIFT),
                     &color,
                     _PIXEL_WIDTH_SHIFT <<
                     1);
            return;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Rect(LG::Rect rect, COLORTYPE color, CMemCanvas *Dest)
        {
            LG::Point pfstl(rect.left, rect.top);
            if(VMC_Clipper(&rect, &pfstl, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return ;
            int length  = _RECT_LENGTH(rect.right, rect.left) <<
                          _PIXEL_WIDTH_SHIFT;
            int tell    = _RECT_LENGTH(rect.bottom, rect.top);

            return;
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Line(LG::Point begin,
                        LG::Point end,
                        COLORTYPE color,
                        CMemCanvas *Dest)
        {
            if(begin.x < 0)
                begin.x = 0;
            if(begin.y < 0)
                begin.y = 0;
            if(end.x < 0)
                end.x = 0;
            if(end.y < 0)
                end.y = 0;

            if(begin.x > (int) Dest->GetClipWidth()-1)
                begin.x = Dest->GetClipWidth()-1;
            if(begin.y > (int) Dest->GetClipHeight()-1)
                begin.y = Dest->GetClipHeight()-1;
            if(end.x > (int) Dest->GetClipWidth()-1)
                end.x = Dest->GetClipWidth()-1;
            if(end.y > (int) Dest->GetClipHeight()-1)
                end.y = Dest->GetClipHeight()-1;


            if(begin.y == end.y)// Draws horizontal line
            {
                int b   = begin.x;
                int e   = end.x;
                if(e < b)
                {
                    int tmp = e; e = b; b = tmp;
                } 

                ubyte_t* ls= Dest->GetPixel() +
                          (begin.y * (Dest->GetClipWidth() << _PIXEL_WIDTH_SHIFT)) +
                          (b << _PIXEL_WIDTH_SHIFT);

                for(int m = b;m < e;m++)
                {
                    __pfmemcpy(ls, &color, _PIXEL_WIDTH_SHIFT << 1);
                    ls += _PIXEL_WIDTH_SHIFT << 1;
                }
                return;
            }


            //確定向量的分量
            int Dx          = end.x - begin.x;
            int Dy          = end.y - begin.y;

            //計算增量數
            double d        = max(abs(Dx), abs(Dy));
            int steps       = (int) (floor(d) + 1) ;

            double dx_step  = ((double) Dx / (double) steps)  ;
            double dy_step  = ((double) Dy / (double) steps)  ;

            float x         = (float) (begin.x + 0.5);// first must can't now situs;
            float y         = (float) (begin.y + 0.5); 


            for(int u = 0;u < steps;u++)
            {
                __pfmemcpy(Dest->GetPixel() +
                         ((int)
                           floor(y) * (Dest->GetClipWidth() << _PIXEL_WIDTH_SHIFT)) +
                         ((int) floor(x) << _PIXEL_WIDTH_SHIFT),
                         &color,
                         _PIXEL_WIDTH_SHIFT <<
                         1);
                //*(word_t*)(Dest->GetPixel() + ((int)floor(y) * (Dest->GetClipWidth() << _PIXEL_WIDTH_SHIFT)) +
                //    ((int)floor(x) << _PIXEL_WIDTH_SHIFT)) = color;
                x = x + (float) (dx_step);
                y = y + (float) (dy_step);
            }

            //m_Route.push_back(end);
            return ;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Circle(LG::Point origin,
                          int radius,
                          COLORTYPE color,
                          CMemCanvas *Dest)
        {
            int i, xx, lx = radius;
            for(i = 0;i < radius + 1;i++)
            {
                //Repeats only one quarter
                xx = (int)
                     sqrt((float) ((radius * radius) - (i * i))); //This is the circle engine (not the fastest one but it works)
                //Look more info from ceh.doc

                Line(LG::Point(origin.x + xx, origin.y + i),
                      LG::Point(lx + origin.x + 1, origin.y + i),
                      color,
                      Dest);
                Line(LG::Point(origin.x + xx, origin.y - i),
                      LG::Point(lx + origin.x + 1, origin.y - i),
                      color,
                      Dest);
                Line(LG::Point(origin.x - xx, origin.y + i),
                      LG::Point(-lx + origin.x - 1, origin.y + i),
                      color,
                      Dest);
                Line(LG::Point(origin.x - xx, origin.y - i),
                      LG::Point(-lx + origin.x - 1, origin.y - i),
                      color,
                      Dest);

                lx = xx;
            }
            return;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Ellipse(LG::Point origin,
                           int a,
                           int b,
                           COLORTYPE color,
                           CMemCanvas *Dest)
        {
            int i, xx, lx = a;
            for(i = 0;i < b + 1;i++)
            {
                xx = (int) sqrt((double)
                                   (a * a) * ((b * b) - (i * i)));
                xx = xx / b;
                Line(LG::Point(origin.x + xx, origin.y + i),
                      LG::Point(lx + origin.x + 1, origin.y + i),
                      color,
                      Dest);
                Line(LG::Point(origin.x + xx, origin.y - i),
                      LG::Point(lx + origin.x + 1, origin.y - i),
                      color,
                      Dest);
                Line(LG::Point(origin.x - xx, origin.y + i),
                      LG::Point(-lx + origin.x - 1, origin.y + i),
                      color,
                      Dest);
                Line(LG::Point(origin.x - xx, origin.y - i),
                      LG::Point(-lx + origin.x - 1, origin.y - i),
                      color,
                      Dest);

                lx = xx;
            }
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Radiate_Line(LG::Point origin,
                                int radius,
                                COLORTYPE color,
                                CMemCanvas *Dest)
        {
            //Pixel(origin, color, Dest);
            for(float i = 0;i < 2.0;i = i + (float) 0.05)
            {
                int X   = (int) (radius* sin(i* _PI_)) + origin.x;
                int Y   = (int) (radius* cos(i* _PI_)) + origin.y;
                Line(origin, LG::Point(X, Y), color, Dest);
            }
            return;
        }
    };
};//namespace LGB

