//VMCBlit.cpp 2D stick function
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
#include <memio.inl>
#include "vmc-draw.h"
#include "vmc-matrix.h"
#include "vmc-clipper.h"

namespace LGB
{
    extern word_t    _PIXEL_WIDTH_SHIFT;  //image width per pixel
    extern void (* __Alpha_Blend)(ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count);

    namespace VMCDraw
    {
        //=============================================================================
        // NAME: 
        // DESC: 虛擬的記憶體畫布貼圖
        //=============================================================================
        int Blit(const Icon* Src, LG::Point point, CMemCanvas *Dest)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;

            /*取出來源檔的檔頭*/
            point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            point.y = point.y - ((Icon*)Src)->GetClipOffsetY();

            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, ((Icon*)Src)->GetClipWidth() - 1, ((Icon*)Src)->GetClipHeight() - 1);
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;
            int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) <<
                          _PIXEL_WIDTH_SHIFT;
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);



            /*---------- 搬移 ------------------*/
            /*
            IOMAPCPY(Dest->GetPixel() + (point.y * Dest->GetWidthSize()) + (point.x << _PIXEL_WIDTH_SHIFT),
                     Dest->GetWidthSize(), 
                     ((CPIcon*)Src)->GetPixel() + (clipper_rect.top * ((Icon*)Src)->GetWidthSize()) + (clipper_rect.left << _PIXEL_WIDTH_SHIFT),
                     ((Icon*)Src)->GetWidthSize(),
                     tell,
                     length);
                     */
            ubyte_t* dptr  = Dest->GetPixel() +
                          (point.y* Dest->GetWidthSize()) +
                          (point.x << _PIXEL_WIDTH_SHIFT); 
            ubyte_t* sptr  = ((CPIcon*)Src)->GetPixel() +
                          (clipper_rect.top* ((Icon*)Src)->GetWidthSize()) +
                          (clipper_rect.left << _PIXEL_WIDTH_SHIFT); 
            for(register int row = 0 ;row < tell ;row++)
            {
                __pfmemcpy(dptr, sptr, length);
                //SSE_IOMEMCPY(dptr, sptr, length);
                //將目的記憶體的控制指標移至下一行開始的位置
                dptr = dptr + Dest->GetWidthSize();
                //將來源記憶體的控制指標移至下一行開始的位置
                sptr = sptr + ((Icon*)Src)->GetWidthSize();
            }


            return _P_OK;
        }


        //=============================================================================
        // NAME: 
        // DESC: 虛擬的記憶體畫布矩形切割貼圖                                      true /
        //=============================================================================
        int ClipperBlit(const Icon* Src,
                              LG::Point point,
                              CMemCanvas *Dest,
                              LG::Rect Rect)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;

            /*取出來源檔的檔頭*/
            point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            point.y = point.y - ((Icon*)Src)->GetClipOffsetY();

    //        //檢查範圍
    //        if(Rect.top < 0)
    //            Rect.top = 0;
    //        if(Rect.left < 0)
    //            Rect.left = 0;
    //        if(Rect.width() > (int) ((Icon*)Src)->GetClipWidth())
    //            Rect.right = ((Icon*)Src)->GetClipWidth() - 1;
    //        if(Rect.height() > (int) ((Icon*)Src)->GetClipHeight())
    //            Rect.bottom = ((Icon*)Src)->GetClipHeight() - 1;

            LG::Point srcp=point;


            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, ((Icon*)Src)->GetClipWidth() - 1, ((Icon*)Src)->GetClipHeight() - 1);
            //LG::Rect clipper_rect   ={Rect.left, Rect.top, Rect.right, Rect.bottom};
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;

            if(Rect.top < clipper_rect.top)
                Rect.top = clipper_rect.top;
            if(Rect.left < clipper_rect.left)
                Rect.left = clipper_rect.left;

            if(Rect.right > clipper_rect.right)
                Rect.right = clipper_rect.right;
            if(Rect.bottom > clipper_rect.bottom)
                Rect.bottom = clipper_rect.bottom;

            //int xx = point.x - srcp.x;
            //int yy = point.y - srcp.y;

            point.x+=(Rect.left-(point.x - srcp.x));
            point.y+=(Rect.top-(point.y - srcp.y));

            


    //        if(Rect.width() > _RECT_LENGTH(clipper_rect.right, clipper_rect.left))
    //            Rect.right = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) - 1;
    //        if(Rect.height() > _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top))
    //            Rect.bottom = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top) - 1;


            int length  = _RECT_LENGTH(Rect.right, Rect.left) <<
                          _PIXEL_WIDTH_SHIFT ;
            int tell    = _RECT_LENGTH(Rect.bottom, Rect.top);

            /*---------- 搬移 ------------------*/
            ubyte_t* dptr  = Dest->GetPixel() +
                          (point.y* Dest->GetWidthSize()) +
                          (point.x << _PIXEL_WIDTH_SHIFT); 
            ubyte_t* sptr  = ((CPIcon*)Src)->GetPixel() +
                          (Rect.top* ((Icon*)Src)->GetWidthSize()) +
                          (Rect.left << _PIXEL_WIDTH_SHIFT);
            for(register int row = 0 ;row < tell ;row++)
            {
                __pfmemcpy(dptr, sptr, length);
                //將目的記憶體的控制指標移至下一行開始的位置
                dptr = dptr + Dest->GetWidthSize();
                //將來源記憶體的控制指標移至下一行開始的位置
                sptr = sptr + ((Icon*)Src)->GetWidthSize();
            }
    //        IOMAPCPY(Dest->GetPixel() +
    //                 (point.y * Dest->GetWidthSize()) +
    //                 (point.x << _PIXEL_WIDTH_SHIFT),
    //                 Dest->GetWidthSize(),
    //                 ((CPIcon*)Src)->GetPixel() +
    //                 (clipper_rect.top * ((Icon*)Src)->GetWidthSize()) +
    //                 (clipper_rect.left << _PIXEL_WIDTH_SHIFT),
    //                 ((Icon*)Src)->GetWidthSize(),
    //                 tell,
    //                 length);


            return _P_OK;
        }


        //=============================================================================
        // NAME: 
        // DESC: 虛擬的記憶體畫布三角形切割貼圖                                      true /
        //=============================================================================
    //     int ClipperBlit(const Icon* Src,
    //                           LG::Triangle2d Tria,
    //                           LG::Point point,
    //                           CMemCanvas *Dest)
    //     {
    //         /*錯誤檢查*/
    //         if(Error_Condition(Src))
    //             return _P_ERROR;
    // 
    //         /*取出來源檔的檔頭*/
    //         point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
    //         point.y = point.y - ((Icon*)Src)->GetClipOffsetY();
    // 
    //         /*-----------裁圖------------*/
    //         LG::Rect clipper_rect(0, 0, ((Icon*)Src)->GetClipWidth() - 1, ((Icon*)Src)->GetClipHeight() - 1);
    //         if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
    //             return _P_OK;
    //         int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) <<
    //                       _PIXEL_WIDTH_SHIFT;
    //         int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);
    // 
    //         /*---------- 搬移 ------------------*/
    //         VMC_Matrix_Triangle_Clipper(((Icon*)Src)->GetClipWidth(),
    //                                      ((Icon*)Src)->GetClipHeight(),
    //                                      ((CPIcon*)Src)->GetPixel(),
    //                                      Tria,
    //                                      point,
    //                                      Dest->GetClipWidth(),
    //                                      Dest->GetClipHeight(),
    //                                      Dest->GetPixel(),
    //                                      m_WidgetSize.depth);
    // 
    //         return _P_OK;
    //     }




        //=============================================================================
        // NAME: 
        // DESC: 階層  對虛擬的記憶體畫布作透明貼圖
        //=============================================================================
        int Darken16Blit(const Icon* Src,
                               LG::Point point,
                               CMemCanvas *Dest,
                               int Diaphaneity)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;

            if(Diaphaneity > 256)
                Diaphaneity = 256;

            /*取出來源檔的檔頭*/
            point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            point.y = point.y - ((Icon*)Src)->GetClipOffsetY();

            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, ((Icon*)Src)->GetClipWidth() - 1, ((Icon*)Src)->GetClipHeight() - 1);
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;
            int a_len   = _RECT_LENGTH(clipper_rect.right, clipper_rect.left);
            int length  = a_len << _PIXEL_WIDTH_SHIFT ;
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);

            /*---------- 搬移 ------------------*/
            //將來源記憶體的控制指標移至第一行開始的位置    
            register ubyte_t * Srcptr = ((CPIcon*)Src)->GetPixel() +
                                     (clipper_rect.top * ((Icon*)Src)->GetWidthSize()) +
                                     (clipper_rect.left << _PIXEL_WIDTH_SHIFT);
            //將目的記憶體的控制指標移至第一行開始的位置
            register ubyte_t * VMCptr = Dest->GetPixel() +
                                     (point.y * Dest->GetWidthSize()) +
                                     (point.x << _PIXEL_WIDTH_SHIFT);


            int DestLineMinus   = Dest->GetWidthSize() - length;
            int Crop            = ((Icon*)Src)->GetWidthSize() - length;

            ubyte_t* v_alpha       = new ubyte_t[a_len];
            memset(v_alpha, Diaphaneity, a_len);
            for(register int row = 0 ;row < tell ;row++)
            {
                __Alpha_Blend(VMCptr, v_alpha, Srcptr, length);
                //將目的記憶體的控制指標移至下一行開始的位置
                VMCptr = VMCptr + Dest->GetWidthSize();
                //將來源記憶體的控制指標移至下一行開始的位置
                Srcptr = Srcptr + ((Icon*)Src)->GetWidthSize();
            }
            delete [] v_alpha;

            return _P_OK;
        }


        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作透明貼圖
        //=============================================================================
        int Lighten16Blit(const Icon* Src,
                                LG::Point point,
                                CMemCanvas *Dest,
                                int Lum)
        {
            return _P_ERROR;
        }


        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作區塊破碎
        //=============================================================================
        int ExplosionBlit(const Icon* Src,
                                LG::Point* situs,
                                int situs_size,
                                CMemCanvas *Dest)
        {
            int c_c = (int) sqrt((float) situs_size);
            int i_w = ((Icon*)Src)->GetClipWidth() / c_c;
            int i_h = ((Icon*)Src)->GetClipHeight() / c_c;

            int s_c = 0;
            for(int i = 0;i < c_c;i++)
            {
                for(int j = 0;j < c_c;j++)
                {
                    int left        = i_w* j;
                    int top         = i_h* i;
                    LG::Rect i_rect (left, top, left + i_w - 1, top + i_h - 1);
                    ClipperBlit(Src, situs[s_c], Dest, i_rect);
                    s_c ++;
                }
            }

            return s_c;
        }


        //=============================================================================
        // NAME: 
        // DESC: 縮放貼圖 
        //=============================================================================
        int ScaleBlit(const Icon* Src,
                            LG::Point point,
                            CMemCanvas *Dest,
                            LG::Size ssize)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;
            if((ssize.cx <= 0) || (ssize.cy <= 0))
                return _P_OK;

            float Dx= (float) ssize.cx / (float) ((Icon*)Src)->GetClipWidth();
            float Dy= (float) ssize.cy / (float) ((Icon*)Src)->GetClipHeight();
            float Fy= (float) ((Icon*)Src)->GetClipHeight() / (float) ssize.cy;

            /*取出來源檔的檔頭*/
            point.x = point.x - (int) ((float) ((Icon*)Src)->GetClipOffsetX() * Dx);
            point.y = point.y - (int) ((float) ((Icon*)Src)->GetClipOffsetY() * Dy);

            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, ssize.cx - 1, ssize.cy - 1);
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;
            int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left);
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);

            /*---------- 搬移 ------------------*/
            //將來源記憶體的控制指標移至第一行開始的位置    
            register ubyte_t * Srcptr = ((CPIcon*)Src)->GetPixel() +
                                     ((int)
                                       floor(clipper_rect.top * Fy) * ((Icon*)Src)->GetWidthSize()) +
                                     (clipper_rect.left << _PIXEL_WIDTH_SHIFT);
            //將目的記憶體的控制指標移至第一行開始的位置
            //register ubyte_t* VMCptr = Dest->GetPixel() + (point.y * Dest->GetWidthSize()) + (point.x << _PIXEL_WIDTH_SHIFT) ;

            VMC_Matrix_Scaling(((Icon*)Src)->GetClipWidth(),
                                ((Icon*)Src)->GetClipHeight(),
                                Srcptr,
                                ssize.cx,
                                ssize.cy,
                                point,
                                Dest->GetClipWidth(),
                                Dest->GetClipHeight(),
                                Dest->GetPixel());
            return _P_OK;
        }
        

        //=============================================================================
        // NAME: 
        // DESC: 透視法
        //=============================================================================
        int PerspectiveBlit(const Icon* Src, int nearview, int farview, int dist, CMemCanvas *Dest)
        {

           /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;
    //        if((ssize.cx <= 0) || (ssize.cy <= 0))
    //            return _P_OK;
    //
    //        float Dx= (float) ssize.cx / (float) ((Icon*)Src)->GetClipWidth();
    //        float Dy= (float) ssize.cy / (float) ((Icon*)Src)->GetClipHeight();
    //        float Fy= (float) ((Icon*)Src)->GetClipHeight() / (float) ssize.cy;
    //
    //        /*取出來源檔的檔頭*/
    //        point.x = point.x - (int) ((float) ((Icon*)Src)->GetClipOffsetX() * Dx);
    //        point.y = point.y - (int) ((float) ((Icon*)Src)->GetClipOffsetY() * Dy);
    //
    //        /*-----------裁圖------------*/
    //        LG::Rect clipper_rect(0, 0, ssize.cx - 1, ssize.cy - 1);
    //        if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
    //            return _P_OK;
    //        int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left);
    //        int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);
    //
    //        /*---------- 搬移 ------------------*/
    //        //將來源記憶體的控制指標移至第一行開始的位置    
    //        register ubyte_t * Srcptr = ((CPIcon*)Src)->GetPixel() +
    //                                 ((int)
    //                                   floor(clipper_rect.top * Fy) * ((Icon*)Src)->GetWidthSize()) +
    //                                 (clipper_rect.left << _PIXEL_WIDTH_SHIFT);
    //        //將目的記憶體的控制指標移至第一行開始的位置
    //        //register ubyte_t* VMCptr = Dest->GetPixel() + (point.y * Dest->GetWidthSize()) + (point.x << _PIXEL_WIDTH_SHIFT) ;
    //
    ////        VMC_Matrix_Scaling(((Icon*)Src)->GetClipWidth(),
    ////                            ((Icon*)Src)->GetClipHeight(),
    ////                            Srcptr,
    ////                            ssize.cx,
    ////                            ssize.cy,
    ////                            point,
    ////                            Dest->GetClipWidth(),
    ////                            Dest->GetClipHeight(),
    ////                            Dest->GetPixel());
    ////        return _P_OK;
    //
    //
    //        //double dx = (double)((Icon*)Src)->GetClipWidth() / (double)ssize.cx;
    //        double dy = (double)((Icon*)Src)->GetClipHeight() / (double)ssize.cy;
    //
    //        int s_w = ((Icon*)Src)->GetClipWidth() * Dest->GetDepth();
    //        int d_w = ((Icon*)Dest)->GetClipWidth() * Dest->GetDepth();
    //        int d_lw = (d_w - (ssize.cx * Dest->GetDepth()));
    //
    //        ubyte_t* dest = Dest->GetPixel() + (point.y * d_w) + (point.x * Dest->GetDepth());
    //        
    //        double slen=ssize.cx;
    //        float diffy = 0;
    //        for(Puint py = 0; py < ssize.cy; py++)
    //        {
    //            double dx = (double)((Icon*)Src)->GetClipWidth() / slen;
    //            float diffx = 0;
    //            for(Puint px = 0; px < ssize.cx; px++)
    //            {
    //                __pfmemcpy((dest), 
    //                         (Srcptr + ((int)floor(diffy) * s_w) + ((int)floor(diffx) * Dest->GetDepth())), 
    //                         Dest->GetDepth()
    //                     );
    //                diffx += (float)dx;
    //                dest += Dest->GetDepth();
    //            }
    //            //slen-=1;
    //            diffy += (float)dy;
    //            //d_lw-=1;
    //            dest  += d_lw;
    //        }

            return _P_OK;
            
        }


        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作條狀貼圖
        //=============================================================================
        int SeamBlit(const Icon* Src,
                           LG::Point point,
                           CMemCanvas *Dest,
                           int Scaling)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;

            /*取出來源檔的檔頭*/
            point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            point.y = point.y - ((Icon*)Src)->GetClipOffsetY();


            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, ((Icon*)Src)->GetClipWidth() - 1, ((Icon*)Src)->GetClipHeight() - 1);
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;
            int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) <<
                          _PIXEL_WIDTH_SHIFT;
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);

            /*---------- 搬移 ------------------*/
            //將來源記憶體的控制指標移至第一行開始的位置    
            register ubyte_t * Srcptr = ((CPIcon*)Src)->GetPixel() +
                                     (clipper_rect.top * ((Icon*)Src)->GetWidthSize()) +
                                     (clipper_rect.left << _PIXEL_WIDTH_SHIFT);
            //將目的記憶體的控制指標移至第一行開始的位置
            register ubyte_t * VMCptr = Dest->GetPixel() +
                                     (point.y * Dest->GetWidthSize()) +
                                     (point.x << _PIXEL_WIDTH_SHIFT) ;

            int count   = 0;
            for(register int row = 0 ;row < tell ;row++)
            {
                if(count < Scaling)
                {
                    __pfmemcpy(VMCptr, Srcptr, length);
                    count ++;
                }else
                {
                    count = 0;
                }
                //將來源記憶體的控制指標移至下一行開始的位置
                Srcptr = Srcptr + ((Icon*)Src)->GetWidthSize();
                //將目的記憶體的控制指標移至下一行開始的位置
                VMCptr = VMCptr + Dest->GetWidthSize();
            }

            return _P_OK;
        }


        //=============================================================================
        // NAME: 
        // DESC: 網狀貼圖
        //=============================================================================
        int InterlactedBlit(const Icon* Src,
                            LG::Point point,
                            CMemCanvas *Dest,
                            int percent)
        {
            return _P_ERROR;
        }




        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作旋轉
        //         (x',y') = (x cos(t) + y sin(t), y cos(t) - x sin(t);
        //=============================================================================
        int RotateBlit(const Icon* Src,
                             LG::Point point,
                             CMemCanvas *Dest,
                             int angle)
        {
            return _P_ERROR;
        }



        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作扭曲貼圖網狀貼圖
        //=============================================================================
        int WarpBlit(const Icon* Src,
                           LG::Point point,
                           CMemCanvas *Dest,
                           int Left,
                           int Right)
        {
            /*錯誤檢查*/
            if(Error_Condition(Src))
                return _P_ERROR;

            /*取出來源檔的檔頭*/
            point.x = point.x - ((Icon*)Src)->GetClipOffsetX();
            point.y = point.y - ((Icon*)Src)->GetClipOffsetY();

            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, (((Icon*)Src)->GetClipWidth() - 1), (((Icon*)Src)->GetClipHeight() - 1));
            if(VMC_Clipper(&clipper_rect, &point, Dest->GetClipWidth(), Dest->GetClipHeight()))
                return _P_OK;
            int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) <<
                          _PIXEL_WIDTH_SHIFT;
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);

            /*---------- 搬移 ------------------*/
            //將來源記憶體的控制指標移至第一行開始的位置    
            register ubyte_t * Srcptr = ((CPIcon*)Src)->GetPixel() +
                                     (clipper_rect.top * ((Icon*)Src)->GetWidthSize()) +
                                     (clipper_rect.left << _PIXEL_WIDTH_SHIFT);
            //將目的記憶體的控制指標移至第一行開始的位置
            register ubyte_t * VMCptr = Dest->GetPixel() +
                                     (point.y * Dest->GetWidthSize()) +
                                     (point.x << _PIXEL_WIDTH_SHIFT);

            Pflag Dir       = 0;
            int Deflection  = 0;

            for(register int row = 0 ;row < tell ;row++)
            {
                int m_shift = (Deflection << 1);
                int p_length= length - m_shift;
                __pfmemcpy(VMCptr + m_shift, Srcptr, p_length);
                if(Dir == 0)
                {
                    Deflection ++;
                    if(Deflection > Right)
                        Dir = 1;
                }else
                {
                    Deflection --;
                    if(Deflection < -Left)
                        Dir = 0;
                }
                Srcptr = Srcptr + ((Icon*)Src)->GetWidthSize();
                VMCptr = VMCptr + Dest->GetWidthSize();
            }
            return _P_OK;
        }


        //=============================================================================
        // NAME: 
        // DESC: 對虛擬的記憶體畫布作色偏貼圖
        //=============================================================================
        int PigmentBlit(const Icon* Src,
                               LG::Point point,
                               CMemCanvas *Dest,
                               char chrominance,
                               char pigment,
                               int value)
        {
            return _P_ERROR;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        int Blit(const ubyte_t*     pSrc,
                        word_t    s_rw,
                        word_t    s_h,
                        LG::Point point,
                        ubyte_t*     pDest,
                        word_t    d_rw,
                        word_t    d_h,
                        short     r_x,
                        short     r_y) //?????         /*
        {
            /*錯誤檢查*/
            if((pSrc == 0) || (pDest == 0))
                return _P_ERROR;


            /*取出來源檔的檔頭*/
            point.x = point.x - r_x;
            point.y = point.y - r_y;

            /*-----------裁圖------------*/
            LG::Rect clipper_rect(0, 0, s_rw, s_h);
            if(VMC_Clipper(&clipper_rect, &point, d_rw - 1, d_h - 1))
                return _P_OK;
            int length  = _RECT_LENGTH(clipper_rect.right, clipper_rect.left) <<
                          _PIXEL_WIDTH_SHIFT;
            int tell    = _RECT_LENGTH(clipper_rect.bottom, clipper_rect.top);

            /*---------- 搬移 ------------------*/
            IOMAPCPY(pDest +
                     (point.y * d_rw) +
                     (point.x << _PIXEL_WIDTH_SHIFT),
                     d_rw,
                     pSrc +
                     (clipper_rect.top * s_rw) +
                     (clipper_rect.left << _PIXEL_WIDTH_SHIFT),
                     s_rw,
                     tell,
                     length);


            return _P_OK;
        }

    };
};
