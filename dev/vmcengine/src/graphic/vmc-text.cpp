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
//
//
#include "vmc-text.h"
#include "vmc-draw.h"


namespace LGB
{
    extern  word_t _PIXEL_WIDTH_SHIFT;

    VMCText::VMCText() :Text()
    {
        m_pFont=new CPIcon;
        p_canvas_=NULL;
    }

    VMCText::~VMCText()
    {
        SAFE_DELETE(m_pFont);
    }

    void VMCText::SetCanvas(CPIcon* canvas)
    {
        //VText::gCanvas=canvas;
        p_canvas_ = canvas;
    }

    void VMCText::OnBlitRender(int x, int y, ubyte_t* BitMap)
    {
        if(p_canvas_ == NULL)
            return;
        
        m_pFont->Allot(*(word_t*)BitMap >> LGB::_PIXEL_WIDTH_SHIFT,
                       *(word_t*)(BitMap + 2),
                       LGB::_PIXEL_WIDTH_SHIFT*2,
                       (ubyte_t*)(BitMap + 4));

        LGB::VMCDraw::Blit(m_pFont, LG::Point(x, y), p_canvas_);
        m_pFont->pixels=0;

    }

    void VMCText::OnTransRender(int x, int y, int colorkey, ubyte_t* BitMap)
    {
        if(p_canvas_ == NULL)
            return;

        m_pFont->Allot(*(word_t*)BitMap >> LGB::_PIXEL_WIDTH_SHIFT,
                       *(word_t*)(BitMap + 2),
                       LGB::_PIXEL_WIDTH_SHIFT*2,
                       (ubyte_t*)(BitMap + 4));

        LGB::VMCDraw::ColorKeyBlit(m_pFont, LG::Point(x, y), p_canvas_, colorkey);
        m_pFont->pixels=0;
    }

    void VMCText::OnMask16Render(int x, int y, int maskcolor, ubyte_t* BitMap)
    {

    }



};

