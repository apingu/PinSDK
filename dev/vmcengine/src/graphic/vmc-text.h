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
// -----------------------------------------------------------------------------------------
//                                                                by Apingu
//                                                                    Copyright  2000/05/04
// -----------------------------------------------------------------------------------------
#ifndef VMCFONT_H
#define VMCFONT_H

#include "Text.h"
#include "vmc-def.h"


namespace LGB
{
    class VMCText : public Text
    {

    public:
        CPIcon* p_canvas_;
        CPIcon* m_pFont;

        VMCText();
        ~VMCText();

        void SetCanvas(CPIcon* canvas);
        void OnBlitRender(int x, int y, ubyte_t* BitMap);
        void OnTransRender(int x, int y, int colorkey, ubyte_t* BitMap);
        void OnMask16Render(int x, int y, int maskcolor, ubyte_t* BitMap);        

    };
}




#endif