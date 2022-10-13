#ifndef __GUIDISPLAYRW_H
#define __GUIDISPLAYRW_H

#include <vector>
#include <string>

#include <rwcore.h>
#include <rpworld.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
#include <rtcharse.h>
#include <rt2d.h>

#include "guidisplay.h"
#include "VFWText.h"

//---------------- class GuiSurfaceRw ----------------/

class GuiSurfaceRw: public GuiSurface
{
public:

    GuiSurfaceRw(GuiDisplay *disp);
    virtual ~GuiSurfaceRw();

    virtual bool LoadImage(const AnsiString &file_name, const AnsiString &mask_name);
    virtual bool LoadImage(void *image, int width, int height, int depth);
    virtual void DestroyImage();
    virtual bool SetColorKey(unsigned int color);

protected:
    RwTexture*	rwtexture;
    RwImage*	rwimage;
    RwRaster*	rwraster;
    friend class GuiDisplayRw;
};

//---------------- class GuiDisplayRw ----------------/

class GuiDisplayRw: public GuiDisplay
{
public:

    GuiDisplayRw();
    virtual ~GuiDisplayRw();

    virtual bool InitDevice(void *device_id, int width, int height);
    virtual void DeinitDevice();
    virtual void SwapBuffers();
    virtual void Clear(unsigned int color);
    virtual void BeginFrame();
    virtual void EndFrame();
    virtual bool Draw(GuiSurface *bmp, int x, int y);
    virtual bool Draw(GuiSurface *bmp, RECT *src, RECT *dest);
    virtual void Line(int x1, int y1, int x2, int y2);
    virtual void Lineloop(int *points, int npoints);
    virtual void FillRect(int left, int top, int width, int height);
    virtual void Polygon(int *points, int npoints);
    virtual void TextOut(const char *text, int x, int y);
    virtual bool GetTextExtent(const char *text, int *width, int *height);
    virtual GuiSurface *CreateSurface();

    static char ImagePath[256];
    static char DbcsPath[64];
    static char SbcsPath[64];
    static char Dbcs2Path[64];
    static char Sbcs2Path[64];
protected:
    RwCamera*	camera;
    RtCharset*	charset;
    RpLight*	ambient;
    RpWorld*	world;
    RwRGBA		wColor;

    VFWText		font,font2;

    void SetCamera( RwV3d Off, RwV3d Ang );

    RwReal recipCameraZ;
    RwReal nearScreenZ;
};



#endif
