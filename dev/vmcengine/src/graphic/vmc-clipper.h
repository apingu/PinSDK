//vmc-clipper.h 
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
#ifndef VMCCILPPER_H
#define VMCCILPPER_H

#include <pf-def.h>


//=============================================================================
// NAME: VMC_Cilpper()
// DESC: trim image off 
//  clipper_rect ==> source clip rect==> rect.left=0, rect.bottom=0 maybe
//                   left and bottom will be clip
//  dest.x =0, dest.y=0
//=============================================================================
inline int VMC_Clipper(LG::Rect* clipper_rect, LG::Point* pfstl, int dest_w, int dest_h)
{
    int src_rw = clipper_rect->right - clipper_rect->left + 1;
    int src_h  = clipper_rect->bottom - clipper_rect->top + 1;

    if(pfstl->x >= dest_w)  //bigger than right
        return 1;

    int sw=pfstl->x + src_rw;
    if(pfstl->x < 0)
    {
        if((sw) <= 0)       //less than left
            return 1;

        clipper_rect->left = (0 - pfstl->x);

        //if((pfstl.x + src_rw) < dest_w) //right between 0 <---> dest_w
        //    clipper_rect->right = src_rw - clipper_rect->left;

        if((sw) > dest_w)   //right bigger than dest_w
            clipper_rect->right = ((dest_w - pfstl->x) - 1);

        pfstl->x = 0;
            //clipper_rect->right = dest_w;

    }
    else if((pfstl->x < dest_w) && (sw > dest_w)) //left between between 0 <---> dest_w
        clipper_rect->right -= (sw - dest_w);

    ///////////////////////////////////////////

    if(pfstl->y >= dest_h)
        return 1;

    int sh=pfstl->y + src_h;
    if(pfstl->y < 0)
    {
        if((sh) <= 0)
            return 1;

        clipper_rect->top = (0 - pfstl->y);

        //else if((pfstl.y + src_h) < dest_h)  because no to change
        //    clipper_rect->bottom = src_h - clipper_rect->top;

        if((sh) > dest_h) 
            clipper_rect->bottom = ((dest_h - pfstl->y) - 1);

        pfstl->y = 0;
    }
    else if((pfstl->y < dest_h) && (sh > dest_h))
        clipper_rect->bottom -= (sh - dest_h);
        //clipper_rect->bottom = (dest_h - pfstl->y) - 1;

    return 0;
}


//=============================================================================
// NAME: VMC_Alpha_Cilpper()
// DESC: trim alpha image off
//=============================================================================
inline int VMC_Alpha_Cilpper(ubyte_t* alpha_ptr, ubyte_t* dest_buf, LG::Rect Trim_Rect, int s_rw, int s_h)
{
    //move pointer to alpah begin line first
    int step  = 0;
    int clipper_count = Trim_Rect.top * s_rw;  //top want trim count
    alpha_ptr++;                               //move to start
    while(step < clipper_count)
    {
        step = step + *(word_t*)alpha_ptr;
        alpha_ptr = alpha_ptr + 3;
    }
    alpha_ptr--;
    return 1;
}

inline int VMC_RLE_Alpha_Cilpper(ubyte_t* alpha_ptr, ubyte_t* dest_buf, LG::Rect Trim_Rect, int s_rw, int s_h)
{

    //==============================

    /*
    int row = 0;
    int tell = Trim_Rect.bottom - Trim_Rect.top
    while (row < tell)//for(register int row  = 0 ; row  < tell ; row++)
    {
        register int  i    = 1;
        register int  Step = 0;

        //??????Alpha?????????======================================
        while(1)
        {
            Step = Step + *(word_t *)(alpha_ptr + i); //?????????????????????
            if((Step - clipper_rect.left) <= 0)     //??????????????????????????????????????????
            {
                i = i + 3;
                continue;
            }
            else
            {
                Step = Step - clipper_rect.left;                                      //if startx == 0 ?????????????????????
                break;
            }
        }

        snipWidth = 0;
        register int count = 0;                                            //??????????????????????????? 
        register ubyte_t Alpha = *(ubyte_t *)(Alpha_Ptr + i - 1);                //????????????????????????
        (*dest_buf) = Alpha;
        dest_buf++;

        /*
        
        Alpha_Ptr = Alpha_Ptr + i + 2;                                    //????????????????????????????????????
        do
        {
            if((count + Step) > length)                                        //??????????????????        
            {
                snipWidth = (count + Step) - length;
                Step = length - count;
            }
            (*dest_buf) = Step;
            dest_buf = dest_buf + 2;

            VMCptr = (VMCptr + Step) ;                                    //????????????????????????????????????????????????????????????(??????????????????)
            Srcptr = (Srcptr + Step) ;                                    //????????????????????????????????????????????????????????????(??????????????????)

            count = count + Step;
            Alpha = *(ubyte_t *)Alpha_Ptr;                                    //??????????????????
            Step  = *(word_t *)(Alpha_Ptr + 1);                              //?????????????????? 
            Alpha_Ptr = Alpha_Ptr + 3;
        }while(count < length);
        Alpha_Ptr = Alpha_Ptr - 2;
        
        //???Alpha???????????????====================================
        while(1)
        {
            if(snipWidth == RightCrop)
                break;
            else
            {
                snipWidth = snipWidth + *(word_t *)(Alpha_Ptr);
                Alpha_Ptr  = Alpha_Ptr + 3;
            }
        }
        Alpha_Ptr -- ;

        //END ???Alpha???????????????================================
    }
        */
    return 1;
}


#endif
