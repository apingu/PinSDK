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
#ifndef ALGO_H
#define ALGO_H

//#include "..//Base//Graphic.h"
#include <math.h>
#include <Graphic3D.h>


#define  _ERR_ENOBUFS_    -2


//=============================================================================
//  Name : DDA()
//  Desc : point on line from begin to end
//=============================================================================

inline int DDA( CPPoint begin, CPPoint end, CPPoint* buf, int len )
{
	//確定向量的分量
    int Dx = end.x - begin.x;
    int Dy = end.y - begin.y;

    //計算增量數
    int steps = (int)( floor( (double)(max( abs(Dx), abs(Dy) )) ) + 1 ) ;

	if( len < steps )
		return _ERR_ENOBUFS_;

    double dx_step =  ( (double)Dx / (double)steps )  ;
    double dy_step =  ( (double)Dy / (double)steps )  ;
	
	float x = (float)(begin.x + 0.5);// first must can't now situs;
    float y = (float)(begin.y + 0.5); 


	for(int u = 0; u < steps; u++ )
	{
		buf[u].x = (int)floor(y);
		buf[u].y = (int)floor(y);
        x = x + (float)( dx_step );
        y = y + (float)( dy_step );
	}

	return steps;
}


#endif