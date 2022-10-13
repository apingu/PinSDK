#ifndef __SHAPEDISPLAY_H
#define __SHAPEDISPLAY_H

#include <list>
#include <string>
#include "ansistring.h"

#if defined(_WIN32) && !defined(_XBOX)

#pragma warning (disable:4786 4244 4018)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(_XBOX)

#include <xtl.h>

#else

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
}
RECT;

#endif

#define gdInvalidColorKey  (0xffffffff)

typedef enum { gdbfNone, gdbfSrcAlpha, gdbfOneOne } gdBlendFunc ;

class GuiDisplay;

//---------------- class GuiSurface ----------------/

class GuiSurface
{
public:
    GuiSurface(GuiDisplay *disp);
    virtual ~GuiSurface();

    virtual bool LoadImage(const AnsiString &file_name, const AnsiString &mask_name)=0;
    virtual bool LoadImage(void *image, int width, int height, int depth)=0;
    virtual void DestroyImage()=0;
    virtual bool SetColorKey(unsigned int color)=0;

    int GetWidth() const                { return Width; }
    int GetHeight() const               { return Height; }
    int GetDepth() const                { return Depth; }
    unsigned int GetColorKey() const    { return ColorKey; }

    gdBlendFunc  BlendFunc;

protected:
    int Width, Height, Depth;
    unsigned int ColorKey;
    GuiDisplay *Display;
};

//---------------- class GuiDisplay ----------------/

class GuiDisplay
{
public:
    GuiDisplay();
    virtual ~GuiDisplay();

    virtual bool InitDevice(void *device_id, int width, int height)=0;
    virtual void DeinitDevice()=0;
    virtual void SwapBuffers()=0;
    virtual void UpdateBounds();
    virtual void Clear(unsigned int color)=0;
    virtual void BeginFrame()=0;
    virtual void EndFrame()=0;
    virtual bool Draw(GuiSurface *bmp, int x, int y)=0;
    virtual bool Draw(GuiSurface *bmp, RECT *src, RECT *dest)=0;
    virtual bool SetClipRgn(RECT *rect);
    virtual void Line(int x1, int y1, int x2, int y2)=0;
    virtual void Lineloop(int *points, int npoints)=0;
    virtual void Rectangle(int left, int top, int width, int height);
    virtual void FillRect(int left, int top, int width, int height)=0;
    virtual void Polygon(int *points, int npoints)=0;
    virtual void TextOut(const char *text, int x, int y)=0;
    virtual bool GetTextExtent(const char *text, int *width, int *height)=0;
    virtual GuiSurface *CreateSurface()=0;
    virtual void SetColor(unsigned int color)   { Color = color; }
    unsigned int GetColor() const               { return Color; }
    RECT GetBounds() const                      { return WindowRect; }
    RECT GetClipRgn() const                     { return ClipRect; }

protected:
    RECT WindowRect;
    RECT ClipRect;
    unsigned int Color;

    int  DeviceID;

private:

};



#endif
