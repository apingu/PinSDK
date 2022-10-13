// CPShape.h header file of graphics shape
//
//
//  Graphics define
//
//
//
//
//
//
//
//
//
//              Copyright (C) 2001 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once


#include "pf-def.h"

#define _MAKEP565(r, g, b)   ((word_t)(((r >> 3) << 11) |  ((g >> 2) << 5)  | (b >> 3)))
#define _MAKEP555(r, g, b)   ((word_t)(((r >> 3) << 10) |  ((g >> 3) << 5)  | (b >> 3)))
#define _MAKEP556(r, g, b)   ((word_t)(((r >> 3) << 11) |  ((g >> 3) << 6)  | (b >> 2)))
#define _MAKEP655(r, g, b)   ((word_t)(((r >> 2) << 10) |  ((g >> 3) << 5)  | (b >> 3)))
//#define _MAKEPARGB(a, r, g, b)  ((dword_t)(a<<24 | r<<16 | g<<8 | b))


#define _SPLITP565(p, r, g, b)  r = (((p >> 11) & 0X1F) << 3);  g = (((p >> 5) & 0X3F) << 2) ; b = ((p & 0x1F) << 3)
#define _SPLITP555(p, r, g, b)  r = (((p >> 11) & 0X1F) << 3);  g = (((p >> 5) & 0X1F) << 3) ; b = ((p & 0x1F) << 3)
#define _SPLITP556(p, r, g, b)  r = (((p >> 11) & 0X1F) << 3);  g = (((p >> 6) & 0X1F) << 3) ; b = ((p & 0x3F) << 2)
#define _SPLITP655(p, r, g, b)  r = (((p >> 10) & 0X3F) << 2);  g = (((p >> 5) & 0X1F) << 3) ; b = ((p & 0x1F) << 3)
#define _SPLITPARGB(p, r, g, b, a) r = ((ubyte_t*)&p)[1]; g = ((ubyte_t*)&p)[2];   b = ((ubyte_t*)&p)[3]; a = ((ubyte_t*)&p)[0];
#define _SPLITPRGBA(p, r, g, b, a) r = ((ubyte_t*)&p)[0]; g = ((ubyte_t*)&p)[1];   b = ((ubyte_t*)&p)[2]; a = ((ubyte_t*)&p)[3];

typedef dword_t COLORTYPE;

namespace LC
{
    typedef dword_t Color32b;
    typedef word_t  Color16;
    
    template <class _Type>
    class RGBA
    {
    public:
    
        _Type   R;
        _Type   G;
        _Type   B;
        _Type   A;
        
        RGBA()
        {
            R = 0;
            G = 0;
            B = 0;
            A = 0;
        }
        
        RGBA( _Type r, _Type g, _Type b, _Type a = 1 )
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
        
        void operator()( _Type r, _Type g, _Type b, _Type a )
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
        
        RGBA<_Type>& operator=( const RGBA<_Type>& color )
        {
            R = color.R;
            G = color.G;
            B = color.B;
            A = color.A;
            return *this;
        }
    };
    
    typedef RGBA<ubyte_t> RGBAb;
    typedef RGBA<float> RGBAf;
};