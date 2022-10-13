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
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "mesh.h"

template <class _Type> class tridmension
{
    typedef MESH<_Type> xyMesh;
    PCC::RList<xyMesh*> zlist;

public:

    tridmension(){};
    ~tridmension(){};

    inline void operator()( int x, int y, int z, _Type node )
    {
        xyMesh* mesh = zlist.Get( z );
        if( mesh == 0 )
        {
            mesh = new xyMesh;
        }
        ( *mesh ) ( y, x, node );
        zlist.Put( z, mesh );
    }

    inline _Type& operator()( int x, int y, int z )
    {
        xyMesh* mesh = zlist( z );
        return ( *mesh ) ( y, x );
    }
};