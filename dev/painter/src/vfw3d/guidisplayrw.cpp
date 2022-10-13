#include "guidisplayrw.h"
#include "pffile.h"
#include "Text.h"
#include "ConfigReader.h"

#define ALPHA(color)	(((color) >> 24) & 0xff)
#define BLUE(color)	    (((color) >> 16) & 0xff)
#define GREEN(color)	(((color) >> 8) & 0xff)
#define RED(color)	    ((color) & 0xff)

char GuiDisplayRw::ImagePath[256];
char GuiDisplayRw::DbcsPath[64];
char GuiDisplayRw::SbcsPath[64];
char GuiDisplayRw::Dbcs2Path[64];
char GuiDisplayRw::Sbcs2Path[64];

//---------------- class GuiSurfaceRw ----------------/

GuiSurfaceRw::GuiSurfaceRw(GuiDisplay *disp):
    GuiSurface(disp),
    rwtexture(0),
    rwimage(0),
    rwraster(0)
{
}

GuiSurfaceRw::~GuiSurfaceRw()
{
    DestroyImage();
}

bool GuiSurfaceRw::LoadImage(const AnsiString &file_name, const AnsiString &mask_name)
{
    RwImageSetPath( GuiDisplayRw::ImagePath );

    rwtexture = RwTextureRead( file_name.c_str(), mask_name.c_str() );
    if ( !rwtexture )
        return false;

    rwraster = RwTextureGetRaster(rwtexture);

    Width = RwRasterGetWidth(rwraster);
    Height = RwRasterGetHeight(rwraster);
    Depth = RwRasterGetDepth(rwraster);

    return true;
}

bool GuiSurfaceRw::LoadImage(void *image, int width, int height, int depth)
{
    if ( depth != 32 ) // Now only support 32 bit depth
        return false;

    rwimage = RwImageCreate( width, height, depth );
    rwimage->stride = rwimage->width*(rwimage->depth>>3);

    RwImageSetPixels( rwimage, (unsigned char*)image );

    rwraster = RwRasterCreate( width, height, depth, rwRASTERTYPETEXTURE );
    RwRasterSetFromImage( rwraster, rwimage );

    return true;
}

void GuiSurfaceRw::DestroyImage()
{
    if (rwimage)
    {
        RwImageFreePixels(rwimage);
        RwImageDestroy(rwimage);
        rwimage = NULL;
    }
    if (rwtexture)
    {
        RwTextureDestroy(rwtexture);
        rwtexture = NULL;
    }

    return;
}

bool GuiSurfaceRw::SetColorKey(unsigned int color)
{
    return true;
}

//---------------- class GuiSurfaceRw ----------------/

GuiDisplayRw::GuiDisplayRw()
{}

GuiDisplayRw::~GuiDisplayRw()
{}

bool GuiDisplayRw::InitDevice(void *device_id, int width, int height)
{
    DeviceID = *(int *)device_id;
    int winWidth = width;
    int winHeight = height;
    WindowRect.left = WindowRect.top = 0;
    WindowRect.right = width;
    WindowRect.bottom = height;

    // create camera
    camera = RwCameraCreate();
    RwCameraSetFrame(camera, RwFrameCreate());
    RwRaster *frameBuf = RwRasterCreate((int)winWidth, (int)winHeight,
                                        0, rwRASTERTYPECAMERA);

    RwCameraSetRaster(camera, frameBuf);
    RwCameraSetZRaster(camera, RwRasterCreate(
                           (int)winWidth, (int)winHeight,
                           0, rwRASTERTYPEZBUFFER));

    //Set up camera view window
    RwV2d pView = { 0.01f, 0.01f };
    RwCameraSetViewWindow   (camera, &pView );
    RwCameraSetNearClipPlane(camera, 0.1f );
    RwCameraSetFarClipPlane (camera, 50.f );

    bool ret = true;

    RwImageSetPath( GuiDisplayRw::ImagePath );

    if ( !font.LoadFont(FONT_SET_DBCS, GuiDisplayRw::DbcsPath, 16, 16, 4, FONT_FMT_0, 1) ||
         !font.LoadFont(FONT_SET_SBCS, GuiDisplayRw::SbcsPath,  8, 16, 4, FONT_FMT_0, 1) )
    {
        OutputDebugString("Load font error!");
        ret = false;
    }

    if ( !font2.LoadFont(FONT_SET_DBCS, GuiDisplayRw::Dbcs2Path, 16, 16, 4, FONT_FMT_0, 1) ||
         !font2.LoadFont(FONT_SET_SBCS, GuiDisplayRw::Sbcs2Path,  16, 32, 4, FONT_FMT_0, 1) )
    {
        OutputDebugString("Load font error!");
        ret = false;
    }

    return ret;
}

void GuiDisplayRw::DeinitDevice()
{
    RwFrameDestroy(RwCameraGetFrame(camera));
    RwRasterDestroy(RwCameraGetRaster(camera));
    RwRasterDestroy(RwCameraGetZRaster(camera));
    RwCameraDestroy(camera);

    return;
}

void GuiDisplayRw::SwapBuffers()
{
    RwCameraShowRaster( camera, (void *)DeviceID, 0 );

    return;
}

void GuiDisplayRw::Clear(unsigned int color)
{
    //clear the frame buffer
    RwCameraClear(camera, (RwRGBA *)&color, rwCAMERACLEARZ);//|rwCAMERACLEARIMAGE);

    return;
}

void GuiDisplayRw::BeginFrame()
{
    RwCameraBeginUpdate(camera);

    recipCameraZ = 1.0f;// / RwCameraGetNearClipPlane(camera);
    nearScreenZ = RwIm2DGetNearScreenZ();

    // Enable alpha
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *)TRUE);

    return;
}

void GuiDisplayRw::EndFrame()
{
    RwCameraEndUpdate(camera);

    return;
}

bool GuiDisplayRw::Draw(GuiSurface *bmp, int x, int y)
{
    return true;
}

bool GuiDisplayRw::Draw(GuiSurface *bmp, RECT *src, RECT *dest)
{
    GuiSurfaceRw* surface = reinterpret_cast<GuiSurfaceRw*>(bmp);

    RwRaster *camRas;
    RwIm2DVertex vertex[4];
    RwReal recipCameraZ;
    RwReal minX, minY, maxX, maxY;
    RwReal minUX, minUY, maxUX, maxUY;

    camRas = RwCameraGetRaster(camera);

    minX = dest->left;
    minY = dest->top;
    maxX = dest->right;
    maxY = dest->bottom;

    minUX = (RwReal) src->left / surface->Width;
    minUY = (RwReal) src->top / surface->Height;
    maxUX = (RwReal) src->right / surface->Width;
    maxUY = (RwReal) src->bottom / surface->Height;

    recipCameraZ = 1.0f / RwCameraGetNearClipPlane(camera);

    RwIm2DVertexSetScreenX(&vertex[0], minX);
    RwIm2DVertexSetScreenY(&vertex[0], maxY);
    RwIm2DVertexSetScreenZ(&vertex[0], nearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertex[0], recipCameraZ);
    RwIm2DVertexSetIntRGBA(&vertex[0], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vertex[0], minUX, recipCameraZ);
    RwIm2DVertexSetV(&vertex[0], maxUY, recipCameraZ);

    RwIm2DVertexSetScreenX(&vertex[1], maxX);
    RwIm2DVertexSetScreenY(&vertex[1], maxY);
    RwIm2DVertexSetScreenZ(&vertex[1], nearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertex[1], recipCameraZ);
    RwIm2DVertexSetIntRGBA(&vertex[1], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vertex[1], maxUX, recipCameraZ);
    RwIm2DVertexSetV(&vertex[1], maxUY, recipCameraZ);

    RwIm2DVertexSetScreenX(&vertex[2], minX);
    RwIm2DVertexSetScreenY(&vertex[2], minY);
    RwIm2DVertexSetScreenZ(&vertex[2], nearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertex[2], recipCameraZ);
    RwIm2DVertexSetIntRGBA(&vertex[2], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vertex[2], minUX, recipCameraZ);
    RwIm2DVertexSetV(&vertex[2], minUY, recipCameraZ);

    RwIm2DVertexSetScreenX(&vertex[3], maxX);
    RwIm2DVertexSetScreenY(&vertex[3], minY);
    RwIm2DVertexSetScreenZ(&vertex[3], nearScreenZ);
    RwIm2DVertexSetRecipCameraZ(&vertex[3], recipCameraZ);
    RwIm2DVertexSetIntRGBA(&vertex[3], 255, 255, 255, 255);
    RwIm2DVertexSetU(&vertex[3], maxUX, recipCameraZ);
    RwIm2DVertexSetV(&vertex[3], minUY, recipCameraZ);

    int tRaster, tShade, tMode;

    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, (void*) &tRaster);
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESS, (void*) &tMode);
    RwRenderStateGet(rwRENDERSTATESHADEMODE, (void*) &tShade);

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*) surface->rwraster);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*) rwTEXTUREADDRESSCLAMP);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) rwSHADEMODEGOURAUD);

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, vertex, 4);

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*) tRaster);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*) tMode);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) tShade);

    return true;
}

void GuiDisplayRw::Line(int x1, int y1, int x2, int y2)
{
    RwIm2DVertex vertex[2];

    // Set up the vertices...
    RwIm2DVertexSetScreenX(&vertex[0], (RwReal)((x1)) );
    RwIm2DVertexSetScreenY(&vertex[0], (RwReal)((y1)) );
    RwIm2DVertexSetScreenZ(&vertex[0], nearScreenZ );
    RwIm2DVertexSetRecipCameraZ(&vertex[0], recipCameraZ );
    RwIm2DVertexSetIntRGBA(&vertex[0], RED(Color), GREEN(Color), BLUE(Color), ALPHA(Color));

    RwIm2DVertexSetScreenX(&vertex[1], (RwReal)((x2)) );
    RwIm2DVertexSetScreenY(&vertex[1], (RwReal)((y2)) );
    RwIm2DVertexSetScreenZ(&vertex[1], nearScreenZ );
    RwIm2DVertexSetRecipCameraZ(&vertex[1], recipCameraZ );
    RwIm2DVertexSetIntRGBA(&vertex[1], RED(Color), GREEN(Color), BLUE(Color), ALPHA(Color));

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
    RwIm2DRenderLine(vertex, 2, 0, 1);
}

void GuiDisplayRw::Lineloop(int *points, int npoints)
{
    RwIm2DVertex *vertex = new RwIm2DVertex[npoints];

    for (int i=0; i < npoints; i++)
    {
        RwIm2DVertexSetScreenX(&vertex[i], (RwReal)((points[2*i])) );
        RwIm2DVertexSetScreenY(&vertex[i], (RwReal)((points[2*i+1])) );
        RwIm2DVertexSetScreenZ(&vertex[i], nearScreenZ );
        RwIm2DVertexSetRecipCameraZ(&vertex[i], recipCameraZ );
        RwIm2DVertexSetIntRGBA( &vertex[i], RED(Color), GREEN(Color), BLUE(Color), ALPHA(Color));
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
    RwIm2DRenderPrimitive(rwPRIMTYPEPOLYLINE , vertex, npoints);

    delete [] vertex;

    return;
}

void GuiDisplayRw::FillRect(int left, int top, int width, int height)
{
    // Set Clip Region
    if (left < ClipRect.left) 
        left = ClipRect.left;
    if (top < ClipRect.top) 
        top= ClipRect.top;
    if (left+width > ClipRect.right)
        width = ClipRect.right-left;
    if (top+height > ClipRect.bottom)
        height = ClipRect.bottom-top;

    RwIm2DVertex vertex[4];

    // Set up X, Y
    RwIm2DVertexSetScreenX(&vertex[0], (RwReal)((left)) );
    RwIm2DVertexSetScreenY(&vertex[0], (RwReal)((top)) );

    RwIm2DVertexSetScreenX(&vertex[1], (RwReal)((left)) );
    RwIm2DVertexSetScreenY(&vertex[1], (RwReal)((top+height)) );

    RwIm2DVertexSetScreenX(&vertex[2], (RwReal)((left+width)) );
    RwIm2DVertexSetScreenY(&vertex[2], (RwReal)((top)) );

    RwIm2DVertexSetScreenX(&vertex[3], (RwReal)((left+width)) );
    RwIm2DVertexSetScreenY(&vertex[3], (RwReal)((top+height)) );

    // Set up Z, RecipCameraZ, Color
    for (int i=0; i < 4; i++ )
    {
        RwIm2DVertexSetScreenZ(&vertex[i], nearScreenZ );
        RwIm2DVertexSetRecipCameraZ(&vertex[i], recipCameraZ );
        RwIm2DVertexSetIntRGBA( &vertex[i], RED(Color), GREEN(Color), BLUE(Color), ALPHA(Color));
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
    RwIm2DRenderPrimitive( rwPRIMTYPETRISTRIP, vertex, 4 );
    return;
}

void GuiDisplayRw::Polygon(int *points, int npoints)
{
    RwIm2DVertex *vertex = new RwIm2DVertex[npoints];

    for (int i=0; i < npoints; i++)
    {
        RwIm2DVertexSetScreenX(&vertex[i], (RwReal)((points[2*i])) );
        RwIm2DVertexSetScreenY(&vertex[i], (RwReal)((points[2*i+1])) );
        RwIm2DVertexSetScreenZ(&vertex[i], nearScreenZ );
        RwIm2DVertexSetRecipCameraZ(&vertex[i], recipCameraZ );
        RwIm2DVertexSetIntRGBA( &vertex[i], RED(Color), GREEN(Color), BLUE(Color), ALPHA(Color));
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
    RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, vertex, npoints);

    delete [] vertex;

    return;
}

GuiSurface *GuiDisplayRw::CreateSurface()
{
    GuiSurface* surface = new GuiSurfaceRw(this);
    return surface;
}

void GuiDisplayRw::SetCamera(RwV3d	off, RwV3d ang)
{
    RwV3d at = {0, 0, 1};
    RwV3d up = {0, 1, 0};
    RwV3d rt = {1, 0, 0};

    RwFrame *frame = RwCameraGetFrame(camera);
    RwFrameRotate(frame, &rt, ang.x, rwCOMBINEREPLACE);
    RwFrameRotate(frame, &up, ang.y, rwCOMBINEPOSTCONCAT);
    RwFrameRotate(frame, &at, ang.z, rwCOMBINEPOSTCONCAT);
    RwFrameTranslate(frame, &off, rwCOMBINEPOSTCONCAT);
}

void GuiDisplayRw::TextOut(const char *text, int x, int y)
{
    font2.SetColor( Color, 0x00ffffff );
    font2.PutsXY( x, y, text);
}

bool GuiDisplayRw::GetTextExtent(const char *text, int *width, int *height)
{
    *width = font2.GetTextWidth(text);
    *height = font2.GetTextHeight(text);
    return true;
}
