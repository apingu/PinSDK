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
#ifndef FRAME_H
#define FRAME_H

#include <Graphic3D.h>

#define _VMC_MAX_LIGHT       7
#define _VMC_MAX_TEXTURE     256
#define _VMC_MAX_SECTOR      5120


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
struct CP3DVertex : public CPVertex3f
{
    //float u, v;               // (u, v) texture coordinate
    Pfloat      vWeight;
    CPVertex2f  vTextureCoord;  // (u, v) texture coordinate
    CPVertex2f  vLightmapCoord; // (u, v) lightmap coordinate
    CPVertex2f  vNormal;            // (x, y, z) normal Vertex
    Pbyte       color[ 4 ];             // RGBA color for the vertex 
};



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
typedef struct tag3DTRIANGLE
{
    CP3DVertex  vertex[ 3 ];
} TRIANGLE;



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
class CPSector
{
public:

    CPSector()
    {
        numtriangles = 0; triangle = 0;
    }
    ~CPSector()
    {
        if( triangle != 0 )
            delete [] triangle;
    }

    int         numtriangles;
    CP3DVertex* triangle;
};











/*
//=============================================================================
//  Name :
//  Desc :
//=============================================================================
class VMC3DModel
{
public:

    VMC3DModel()  { numsector = 0; sector = 0; }
    ~VMC3DModel() { if( sector != 0 ) delete [] sector;}

    int numsector;
    V3DSector* sector;
};

 */

#endif
