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
#ifndef CHESSBOARDDEF_H
#define CHESSBOARDDEF_H

#include <Graphic2D.h>

#define _WALKABLE      1
#define _UN_WALKABLE   10000


enum BoradType{ record_t, norecord_t };

struct grid_t
{
	UINT    width;
	UINT    height;
	shape_t shape;
};

typedef void* PBOARD;


#endif