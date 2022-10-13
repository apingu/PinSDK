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
#include "VFWText.h"
#include <Camera.h>
#include <assert.h>


void Vfwputimage( int x, int y, char* BitMap );
void Vfwputimagetrans(int x, int y, char* BitMap, int colorkey);


//=============================================================================
//  Name : 
//  Desc : construction
//=============================================================================
VFWText::VFWText()
{
	SetRenderFunc( Vfwputimage, Vfwputimagetrans );
}


//=============================================================================
//  Name : 
//  Desc : distruction
//=============================================================================
VFWText::~VFWText()
{
}

namespace VfwText
{

    LGR::Camera PaintCamera;

};


void My2DRender(float x1, float y1, float x2, float y2, float z, float recipCamZ, float uvOffset);

void Vfwputimage( int x, int y, char* BitMap )
{
	VfImage image;
	image.width = (*(WORD*)BitMap)>>2;
	image.height = (*(WORD*)(BitMap + 2));
	image.depth = 32;
	image.pixels=(BYTE*)BitMap+4;
	VfwText::PaintCamera.Render( &image, LG::Rect( x, y, x+image.width, y+image.height ) );
	return;
}


void Vfwputimagetrans(int x, int y, char* BitMap, int colorkey)
{



}
