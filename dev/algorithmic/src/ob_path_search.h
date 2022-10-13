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
#ifndef OBPATHSEARCH_H
#define OBPATHSEARCH_H

#include "path_search.h"

class OBPathSearch : public CPathSearch
{

public:

    OBPathSearch();
    ~OBPathSearch();
    
    int  Init( int* map, int width, int height );
    // map is a width * height array
    // which 1    : walkable
    //    10000 : nonwalkable
    // return 0 for failed else successful
    
    void Close( void );
    
    int  SearchInMap( int x1, int y1, int x2, int y2 );
    // call this routine to find a path from (x1, y1) to (x2, y2) in map
    // return the steps of path
    
    
    int  Find( int x1, int y1, int x2, int y2 );
    // call this routine to find a path from (x1, y1) to (x2, y2) in world
    // return the steps of path
    
    
    /*
    int  PathStep;
    
    char PathDir[1024];
    // search result (direction)
    //   7:L-U 0:U 1:R-U
    //   6:L    2:R
    //   5:L-D 4:D 3:R-D
    
    int  PathX[512];
    int  PathY[512];
    */
};

#endif//CSEARCH