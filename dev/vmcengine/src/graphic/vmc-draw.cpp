
#include <memio.inl>
//#include "vmc-matrix.h"
#include "vmc-draw.h"
#include "vmc-clipper.h"
#include "vmc-blend_func.h"

#include <pf-debug.h>



namespace LGB
{
    // 加速用
    // VMCWidget _Widget;             //設定模式的長寬與點的大小
    word_t _PIXEL_WIDTH_SHIFT = 0;  //image width per pixel
    void (* __Alpha_Blend)   (ubyte_t *dest, const ubyte_t* alpha, const ubyte_t *src, int count) = VMC_Alpha_565_Blend;
    void (* __ColorKey_Blend)(ubyte_t* dest, int colorkey, const ubyte_t* src, int count)         = VMC_ColorKey_16b_Blend;
    void (* __Alpha_combine)  (ubyte_t* dest, ubyte_t* srca0, ubyte_t* srca1, int len)            = VMC_Alpha_combine;
};


/*
extern "C"
{
    __SLAPI std::set<CPIcon*>* GetIconList(void)
    {
        return &LGB::_CPIconList;
    }

    __SLAPI PixelFormat GetPixelFmt(void)
    {
        return LGB::_VGA_PixelFormat;
    }

    __SLAPI void SetPixelFmt(PixelFormat fmt)
    {
        //if(_CPIconList !=NULL )
        {
            if(!LGB::_CPIconList.empty())
            {
                std::set<CPIcon*>::iterator iter = LGB::_CPIconList.begin();
                while(iter!=LGB::_CPIconList.end())
                {
                    CPIcon* icon = (*iter);
                    switch (fmt)
                    {
                    case PIXEL_R5G6B5:
                        icon->ToR5G6B5();  break;
                    case PIXEL_R5G5B5:
                        icon->ToR5G5B5();  break;
                    case PIXEL_R5G5B6:
                        icon->ToR5G5B6();  break;
                    case PIXEL_R6G5B5:
                        icon->ToR6G5B5();  break;
                    case PIXEL_R8G8B8:
                        icon->ToR8G8B8();  break;
                    case PIXEL_A8R8G8B8:
                        icon->ToA8R8G8B8(); break;
                    case PIXEL_A8B8G8R8:
                        icon->ToA8B8G8R8(); break;
                    default:
                        {
                            char msg[256];
                            sprintf(msg,
                                " %s : file can't change to define VGA format yet !!! ",
                                icon->Getfilename());
                            DebugMessage(msg, "CPIcon");
                            break;
                        }
                    }

                    iter++;
                }
            }
        }

        LGB::_VGA_PixelFormat = fmt;

    }
}
*/
