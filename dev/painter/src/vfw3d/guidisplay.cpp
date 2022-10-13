#include "guidisplay.h"


GuiDisplay*	    GlobalDisplay	= NULL;


#define SAFE_DELETE(x)  if(x){ delete x; x = 0; }

inline int Clamp(int value, int min, int max)
{
    if(value<min)
        return min;
    if(value>max)
        return max;
    return value;
}

//---------------- class GuiSurface ----------------/

GuiSurface::GuiSurface(GuiDisplay *disp):
    Display(disp),
    Width(0),Height(0),
    ColorKey(gdInvalidColorKey),
    BlendFunc(gdbfNone)
{}

GuiSurface::~GuiSurface()
{}

//---------------- class GuiDisplay ----------------/

GuiDisplay::~GuiDisplay()
{}

bool GuiDisplay::SetClipRgn(RECT *rect)
{
    if(rect)
    {
        ClipRect.left = Clamp(rect->left, 0, WindowRect.right - WindowRect.left);
        ClipRect.right = Clamp(rect->right, 0, WindowRect.right - WindowRect.left);
        ClipRect.top = Clamp(rect->top, 0, WindowRect.bottom - WindowRect.top);
        ClipRect.bottom = Clamp(rect->bottom, 0, WindowRect.bottom - WindowRect.top);
    }
    else
    {
        ClipRect.left = 0;
        ClipRect.top = 0;
        ClipRect.right = WindowRect.right - WindowRect.left;
        ClipRect.bottom = WindowRect.bottom - WindowRect.top;
    }

    return true;
}

void GuiDisplay::Rectangle(int left, int top, int width, int height)
{
    int right = left + width;
    int bottom = top + height;

    int points[4][2]=
        {
            {left, top}, {left, bottom},
            {right, bottom}, {right, top}
        };

    Lineloop(points[0], 4);

}

#if defined(_WIN32) && !defined(_XBOX)

GuiDisplay::GuiDisplay():
    DeviceID(0),
    Color(0xffffffff)
{
}

void GuiDisplay::UpdateBounds()
{
}

#else

GuiDisplay::GuiDisplay():
    Color(0xffffffff)
{
}

void GuiDisplay::UpdateBounds()
{}

#endif

