#ifndef VMCDRAW_H
#define VMCDRAW_H

#include "vmc-def.h"

namespace LGB
{
    namespace VMCDraw
    {
        //----------Blit---------------------------------------------------------------------
        int         Blit                  (const ubyte_t* pSrc, word_t s_w, word_t s_h, LG::Point point, ubyte_t* pDest, word_t d_w, word_t d_h, short r_x, short r_y); //?????            
        //----------Blit---------------------------------------------------------------------
        int         Blit                  (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                 //貼圖
        int         Darken16Blit          (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Diaphaneity = 128);          //透明比例貼圖
        int         Lighten16Blit         (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Lum);                        //加亮透明貼圖
        int         SeamBlit              (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Scaling);                    //條網狀
        int         RotateBlit            (const Icon* Src, LG::Point point, CMemCanvas* Dest, int angle);                      //旋轉
        int         InterlactedBlit       (const Icon* Src, LG::Point point, CMemCanvas* Dest, int percent);                    //網狀貼圖
        int         WarpBlit              (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Left, int Right);            //彎曲貼圖
        int         PigmentBlit           (const Icon* Src, LG::Point point, CMemCanvas* Dest, char chrominance, char pigment, int value);

        //int       ClipperBlit           (const Icon* Src, LG::Triangle2d Tria,  LG::Point point, CMemCanvas *Dest);            //貼圖
        int         ClipperBlit           (const Icon* Src, LG::Point point, CMemCanvas *Dest, LG::Rect rect);                    //貼圖
        int         ScaleBlit             (const Icon* Src, LG::Point point, CMemCanvas *Dest, LG::Size ssize);                 //縮放貼圖 
        int         ExplosionBlit         (const Icon* Src, LG::Point* situs, int situs_size,  CMemCanvas *Dest);
        int         PerspectiveBlit       (const Icon* Src, int nearview, int farview, int dist, CMemCanvas *Dest);

        //----------Alpha---------------------------------------------------------------------
        int         Iconcpy               (const Icon* Src, LG::Point point, Icon* Dest);
        int         AlphaIconcpy          (const Icon* Src, LG::Point point, Icon* Dest);
        int         AlphaBlit             (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                 //Alpha貼圖
        int         AlphaRLEBlit          (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                 //AlphaRLE加速貼圖 
        int         AlphaClipperBlit      (const Icon* Src, LG::Point point, CMemCanvas* Dest, LG::Rect rect);                //Alpha貼圖 
        int         Alpha16DarkenBlit     (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Diaphaneity = 128);          //Alpha圖透明比例貼圖
        int         Alpha16LightenBlit    (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Lum);                        //Alpha圖透明比例貼圖
        int         AlphaWarpBlit         (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Left, int Right);            //Alpha彎曲貼圖
        int         AlphaRippleBlit       (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                 //Alpha波紋貼圖 
        int         AlphaScaleBlit        (const Icon* Src, LG::Point point, CMemCanvas* Dest, LG::Size  ssize);                //Alpha縮放貼圖 


        //----------ColorKey-------------------------------------------------------------------
        int         ColorKeyBlit          (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                      //ColorKey透空貼圖(以第一點為透明點)
        int         ColorKeyBlit          (const Icon* Src, LG::Point point, CMemCanvas* Dest, COLORTYPE ColorKey);                  //ColorKey透空貼圖(傳入透明點)
        int         ColorKey16DarkenBlit  (const Icon* Src, LG::Point point, CMemCanvas* Dest);                                      //ColorKey圖透明貼圖(以第一點為透明點, 50% 透明)
        int         ColorKey16DarkenBlit  (const Icon* Src, LG::Point point, CMemCanvas* Dest, COLORTYPE ColorKey);                  //ColorKey圖透明貼圖(傳入透明點, 50% 透明)
        int         ColorKey16DarkenBlit  (const Icon* Src, LG::Point point, CMemCanvas* Dest, int Diaphaneity);                     //ColorKey圖透明比例貼圖(以第一點為透明點)
        int         ColorKey16DarkenBlit  (const Icon* Src, LG::Point point, CMemCanvas* Dest, COLORTYPE ColorKey, int Diaphaneity); //ColorKey圖透明比例貼圖(傳入透明點 , 與透明比例)

        //----------draw graphic func----------------------------------------------------------
        void        Pixel                 (LG::Point point,  COLORTYPE color, CMemCanvas* Dest);
        void        Rect                  (LG::Rect  rect,   COLORTYPE color, CMemCanvas* Dest);
        void        Line                  (LG::Point begin,  LG::Point end, COLORTYPE color, CMemCanvas* Dest);
        void        Circle                (LG::Point origin, int radius,    COLORTYPE color, CMemCanvas* Dest);
        void        Ellipse               (LG::Point origin, int a,int b,   COLORTYPE color, CMemCanvas* Dest);
        void        Radiate_Line          (LG::Point origin, int radius,    COLORTYPE color, CMemCanvas* Dest);
    };

};



#endif