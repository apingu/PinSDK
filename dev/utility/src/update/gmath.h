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
//                                               Copyright    (C)    1999    Pin
//
#ifndef MATH3D_H
#define MATH3D_H

#include "../Base/Graphic3D.h"


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
//
//  
//  2D line formula ax + by + c = 0 have p1( x0, y0 ) p2( x1, y1 )
//  a = ( y0 - y1 )
//  b = ( x1 - x0 )
//  c = ( x0y1 - y0x1 )
//
//
//

typedef struct Polynomial2D
{
	int a;
	int b;
	int c;
} POLY2D_T;

typedef struct Polynomial3D
{
	int a;
	int b;
	int c;
	int d;
} POLY3D_T;


inline POLY2D_T Linear_Equation2D( CPPoint p0, CPPoint p1 )
{
	Polynomial2D poly;
	poly.a = p0.y - p1.y;
	poly.b = p1.x - p0.x;
	poly.c = p0.x * p1.y - p1.x * p0.y;
	return poly;
}


///////////////////////////////////////////////////////////////////////////////
//  result = (p) + lambda(pq)
#define Point_on_Line( resule, p, q, lambda )\
	( result.x = p.x + lambda * ( q.x - p.x),\
	  result.y = p.y + lambda * ( q.y - p.y),\
	  result.z = p.z + lambda * ( q.z - p.z) )

// dot product of two vector if perpendicular == 0
#define Dot_product( a, b )         ( ( a.x * b.x ) + ( a.y * b.y ) + ( a.z * b.z ) )

// cross product of two vector if n perpendicular a and b == 0
// 兩向量的法向量
#define Cross_product( n, a, b )    ( n.x = ( a.y * b.z ) - ( a.z * b.y ),\
	                                  n.y = ( a.z * b.x ) - ( a.x * b.z ),\
                                      n.z = ( a.x * b.y ) - ( a.y * b.x ) )

#define Subt_vec( AB, a, b )        ( AB.x = b.x - a.x,\
	                                  AB.y = b.y - a.y,\
	                                  AB.z = b.z - a.z )

#define EPS 1e-7

#define Length( a )    sqrt( Dot_product( a, a ) )

// 3點的法向量和它們的平面常數
typedef struct
{
	CPVertex3f normal;  //法向量
	float      cnst;
}Plane;


inline void plane_constants( Plane* pqr, CPVertex3f p, CPVertex3f q, CPVertex3f r )
{
	CPVertex3f pq, pr; //difference vector PQ and PR
	CPVertex3f* n = ( CPVertex3f*)(&(pqr->normal));
	// To speedup the macros
	Subt_vec( pq, p, q );
	Cross_product( (*n), pq, pr );
	pqr->cnst = Dot_product( (*n), p );
}

//
inline void normalize_vec( CPVector3f* v )
{
    register float len = (float)Length( *v );
	if( len < EPS )
	{
		//DebugMessage( "cannot normalize vector\n", "failure" );
		return;
	}
	v->x /= len; v->y /= len; v->z /= len;
}


#define INFINITE3D 1e20

#define Linear_comb( r, p, pq, l )\
	( r.x = (p).x + l * (pq).x,/*Similar to Point_on_line()*/\
	  r.y = (p).y + l * (pq).y,\
	  r.z = (p).z + l * (pq).z )
#define Is_zero( x )  ( fabs(x) < EPS )

/*
void sect_line_and_plane( CPVertex3f s, float* lambda, Pbool* parallel,
						  CPVertex3f a, CPVertex3f ab, Plane *p )
{

	//begin
	CPVertex3f *n = (CPVertex3f*)&(p->normal);
	//speed up the macro
	float n_ab = Dot_product( (*n), ab );
	if( Is_zero( n_ab ) )
	{
		//Either the line coincides with the plane or it is parallel to the plane
		*parallel = TRUE;
		if( Is_zero( Dot_product( (*n), a ) - p->cnst ) ) * lambda = 0;
		else 
			(*lambda) = (float)(INFINITE3D);
	}
	else
	{
		parallel = FALSE;
		*lambda = ( p->cnst - Dot_product( (*n), a ) ) / n_ab;
	}
	Linear_comb( s, a, ab, *lambda );
}
*/
#endif
