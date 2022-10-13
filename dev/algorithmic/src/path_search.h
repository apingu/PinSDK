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
#ifndef PATHSEARCH_H
#define PATHSEARCH_H

#include <vector>

class CPathSearch
{
protected:

    unsigned int CELL_W;
    unsigned int CELL_H;
    
public:

    CPathSearch() {};
    ~CPathSearch() {};
    
    virtual int  Init( int* map, int width, int height ) = 0;
    // map is a width * height array
    // which 1    : walkable
    //    10000 : nonwalkable
    // return 0 for failed else successful
    
    virtual void Close( void ) = 0;
    
    virtual int  SearchInMap( int x1, int y1, int x2, int y2 ) = 0;
    // call this routine to find a path from (x1, y1) to (x2, y2) in map
    // return the steps of path
    
    int          SetCellWH( int w, int h ) { CELL_W = w; CELL_H = h; }
    // use in Find func beacuse find func
    // is search by point
    
    virtual int  Find( int x1, int y1, int x2, int y2 ) = 0;
    // call this routine to find a path from (x1, y1) to (x2, y2) in world
    // return the steps of path
    
    int              PathStep;
    
    //char PathDir[1024];
    std::vector<int> PathDir;
    // search result (direction)
    //   7:L-U 0:U 1:R-U
    //   6:L    2:R
    //   5:L-D 4:D 3:R-D
    
    //int  PathX[512];
    //int  PathY[512];
    std::vector<int> PathX;
    std::vector<int> PathY;
};

#endif//CSEARCH