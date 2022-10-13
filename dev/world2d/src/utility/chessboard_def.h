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

#include <graphic2D.h>

#define _WALKABLE      1
#define _UN_WALKABLE   10000


enum BoradType{ record_t, norecord_t };

struct grid_t
{
    uint_t    width;
    uint_t    height;
    shape_t shape;
};

typedef void* PBOARD;


#endif