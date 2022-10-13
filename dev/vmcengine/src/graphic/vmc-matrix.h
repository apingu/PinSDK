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
//
//                                               Copyright    (C)    1999    Pin
//
#ifndef VMCMATRIX_H
#define VMCMATRIX_H

#include <stdio.h>
#include <stdlib.h>
//#include <io.h>
#include <graphic2D.h>
#include <memio.inl>
#include <aintersect.inl>

//#include "..\\Math\\Math3D.h"
//#include "..\\Algorithmic\\Algori.h"




//=============================================================================
// NAME: VMC_Matrix_Triangle()
// DESC: Matrix triangle clipper 
//=============================================================================
inline void VMC_Matrix_Triangle_Clipper(int s_w, int s_h, ubyte_t* src,
                                         LG::Triangle2d Tria, LG::Point point,
                                         int d_w, int d_h, ubyte_t* dest,
                                         int ByteCount = 2)
{
    Tria.Vertical_Sort();

    s_w = s_w * ByteCount;
    d_w = d_w * ByteCount;

    LG::Linear poly0, poly1;

    Linear_Constants(&poly0, &Tria[2], &Tria[0]);

    int dy = Tria[2].y;
    Linear_Constants(&poly1, &Tria[2], &Tria[1]);
    for(int y = Tria[2].y; y < Tria[1].y; y++)
    {
        int begin_x = ((-((poly0.normal.y*y) + poly0.cnst)) / poly0.normal.x);
        int end_x   = ((-((poly1.normal.y*y) + poly1.cnst)) / poly1.normal.x);
        int line    = ((end_x - begin_x) + 1) * ByteCount;
        int rn      = Tria[2]. x - begin_x;
        //將來源記憶體的控制指標移至第一行開始的位置
        register ubyte_t* Srcptr = src  + (y * s_w) + (begin_x * ByteCount);
        //將目的記憶體的控制指標移至第一行開始的位置
        register ubyte_t* VMCptr = dest + (dy * d_w) + ((point.x - rn) * ByteCount);
        __pfmemcpy(VMCptr, Srcptr, line);
        dy ++;
    }

    LG::Linear poly2;
    Linear_Constants(&poly2, &Tria[1], &Tria[0]);

    for(int y = Tria[1].y; y < Tria[0].y; y++)
    {
        int begin_x = (-((poly0.normal.y*y) + poly0.cnst)) / poly0.normal.x ;
        int end_x   = (-((poly2.normal.y*y) + poly2.cnst)) / poly2.normal.x ;
        int line    = ((end_x - begin_x) + 1) * ByteCount;
        int rn      = Tria[2]. x - begin_x;
        //將來源記憶體的控制指標移至第一行開始的位置
        register ubyte_t* Srcptr = src  + (y * s_w) + (begin_x * ByteCount);
        //將目的記憶體的控制指標移至第一行開始的位置
        register ubyte_t* VMCptr = dest + (dy * d_w) + ((point.x - rn) * ByteCount);
        __pfmemcpy(VMCptr, Srcptr, line);
        dy ++;
    }
    return;
}



//=============================================================================
// NAME: VMC_Matrix_Scaling()
// DESC: Matrix enlarge or lessen
//=============================================================================
inline void VMC_Matrix_Scaling(int s_w, int s_h, ubyte_t* src,
                                uint_t width, uint_t height, LG::Point point,
                                int d_w, int d_h, ubyte_t* dest,
                                int ByteCount = 2)
{
    // width, height 是縮小後的

    double dx = (double)s_w / (double)width;
    double dy = (double)s_h / (double)height;

    s_w = s_w * ByteCount;
    d_w = d_w * ByteCount;
    int d_lw = (d_w - (width * ByteCount));

    dest = dest + (point.y * d_w) + (point.x * ByteCount);


    int* diffxcash=new int[width];
    //int* diffycash=new int[height];
    //DWMEMSET(diffxcash, 0, (height*sizeof(int)));



    float diffx = 0;
    for(uint_t px = 0; px < width; px++)
    {
        diffxcash[px]=((int)floor(diffx))*ByteCount;
        diffx += (float)dx;
    }


    float diffy = 0;
    unsigned char* bufptr=NULL;
    //float diffyf=0;
    for(uint_t py = 0; py < height; py++)
    {
        //diffyf=(diffy+(float)dy);
        //if(((int)(diffyf))!=((int)diffy))
        //{
        
        //{
        bufptr=(src + ((int)floor(diffy) * s_w));
    
        for(uint_t pcx = 0; pcx < width; pcx++)
        {
            __pfmemcpy((dest), 
                      (bufptr + diffxcash[pcx]), 
                      ByteCount
                    );
            dest += ByteCount;
        }
        //diffxcash[py]=1;
        //}
        //}
        diffy += (float)dy;
        //diffy = diffyf;
        dest  += d_lw;
    }

    delete [] diffxcash;
    //delete [] diffycash;

    
//    float diffy = 0;
//    for(uint_t py = 0; py < height; py++)
//    {
//        float diffx = 0;
//        for(uint_t px = 0; px < width; px++)
//        {
//            __pfmemcpy((dest), 
//                      (src  + ((int)floor(diffy) * s_w) + ((int)floor(diffx) * ByteCount)), 
//                      ByteCount
//                  );
//            diffx += (float)dx;
//            dest += ByteCount;
//        }
//        diffy += (float)dy;
//        dest  += d_lw;
//    }

    

    return;
}


//=============================================================================
// NAME: VMC_Matrix_Rotation()
// DESC: Rotation Matrix
//=============================================================================
inline void VMC_Matrix_Rotation(int s_w, int s_h, ubyte_t* src,
                                 double radian, LG::Point point,
                                 int d_w, int d_h, ubyte_t* dest,
                                 int ByteCount = 2)
{
    LG::Rect area(0, 0, d_w, d_h);

    int s_rw = s_w * ByteCount;
    int d_rw = d_w * ByteCount;

    for(int sy = 0; sy < s_h; sy++)
    {
        for(int sx = 0; sx < s_w ; sx++)
        {
            int dx = (int)((float)sy*cos(radian) - (float)sx*sin(radian)) + point.x;
            int dy = (int)((float)sx*cos(radian) + (float)sy*sin(radian)) + point.y;
            if(PA::Intersect::Test(&area, &LG::Point(dx, dy)))
            {
                __pfmemcpy((dest + (dy * d_rw) + (dx * ByteCount)), 
                         (src),
                         ByteCount
                     );
            }
            src += ByteCount;
        }
    }
    return;
}


//=============================================================================
// NAME: VMC_Matrix_Tensile()
// DESC: 
//=============================================================================
inline void VMC_Matrix_Tensile(int s_w, int s_h, ubyte_t* src,
                                LG::Point left_top, LG::Point top_right,
                                LG::Point right_bottom, LG::Point bottom_left,
                                int d_w, int d_h, ubyte_t* dest, int ByteCount = 2)
{







}

//=============================================================================
// NAME: VMC_Matrix_WavyCurve()
// DESC: 
//=============================================================================
inline void VMC_Matrix_WavyCurve(int s_w, int s_h, ubyte_t* src,
                                  int a, int b, LG::Point origin,
                                  int crest, int trough, int wavelength, int wavecount,
                                  int d_w, int d_h, ubyte_t* dest,
                                  int ByteCount = 2)
{
    int diff =  (crest - trough) / wavelength;
    s_w = s_w * ByteCount;
    d_w = d_w * ByteCount;

    for(int w = 0; w < wavecount; w++)
    {
        int i,xx,lx=a;
        for(i = 0;i<b+1;i++)
        {
            xx=(int)sqrt((double)(a*a)*((b*b)-(i*i)));
            xx=xx/b;

            __pfmemcpy((src + ((origin.y+i - diff) * s_w) + ((origin.x+xx) * ByteCount)),
                     (src + (origin.y+i * s_w) + (origin.x * ByteCount)),
                     ByteCount);

            __pfmemcpy((src + ((origin.y-i - diff) * s_w) + ((origin.x+xx) * ByteCount)),
                     (src + (origin.y-i * s_w) + (origin.x * ByteCount)),
                     ByteCount);

            __pfmemcpy((src + ((origin.y+i - diff) * s_w) + ((origin.x-xx) * ByteCount)),
                     (src + (origin.y+i * s_w) + (origin.x * ByteCount)),
                     ByteCount);

            __pfmemcpy((src + ((origin.y-i - diff) * s_w) + ((origin.x-xx) * ByteCount)),
                     (src + (origin.y-i * s_w) + (origin.x * ByteCount)),
                     ByteCount);

            //Line(LG::Point(origin.x+xx, origin.y+i), LG::Point(lx+origin.x+1, origin.y+i), color, Dest);
            //Line(LG::Point(origin.x+xx, origin.y-i), LG::Point(lx+origin.x+1, origin.y-i), color, Dest);
            //Line(LG::Point(origin.x-xx, origin.y+i), LG::Point(-lx+origin.x-1, origin.y+i), color, Dest);
            //Line(LG::Point(origin.x-xx, origin.y-i), LG::Point(-lx+origin.x-1, origin.y-i), color, Dest);

            lx=xx;
        }
    }

}

#endif
