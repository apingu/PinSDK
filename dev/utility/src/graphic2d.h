// CPShape.h header file of graphics shape
//
//
//  Graphics define
//
//
//
//
//
//
//
//
//
//              Copyright (C) 2001 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "pf-def.h"
//#include <math.h>


typedef  unsigned int shape_t;

#define  _P_SHAPELESS_ 0X0000 //
#define  _P_CIRCLE_    0X2000 //圓形
#define  _P_POINT_     0X2001 //點
#define  _P_LINE_      0X2002 //線
#define  _P_TRIANGLE_  0X2003 //三角形
#define  _P_RECT_      0X2004 //矩形
#define  _P_RHOMBUS_   0X2014 //菱形
#define  _P_QUADR_     0X2013 //扇形
#define  _P_OCTAGON_   0X2008 //八邊形
#define  _P_POLYGON_   0X200F //多邊形


#if defined (_WINDOWS) || defined (_CONSOLE)
#include <wtypes.h>
#endif
#if defined(_XBOX)
#include <xtl.h>
#endif
#if !defined (_WINDOWS) && !defined(_XBOX)
typedef struct tagRECT
{
    long32_t left;
    long32_t top;
    long32_t right;
    long32_t bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;

typedef const RECT* LPCRECT;

typedef struct tagPOINT
{
    long32_t  x;
    long32_t  y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

typedef struct tagSIZE
{
    long32_t  cx;
    long32_t  cy;
} SIZE, *PSIZE, *LPSIZE;
#endif


///////////////////////////////////////////////////////////////////////////////
//  graphic funtion

namespace LG
{
    ///////////////////////////////////////////////////////////////////////////////
    //  point class
    class Point: public tagPOINT
    {
    public:

        Point()                                                  { operator()( 0, 0 ); }
        Point( int X, int Y )                                    { operator()( X, Y ); }
        Point( const POINT& point )                              { operator()( point.x, point.y ); }
                                                                 
                                                                 
        Point& operator=( const Point& point )                   { operator()( point.x, point.y ); return *this; }
        void operator()( int X, int Y )                          { x = X; y = Y; }
                                                                 
        bool  operator == ( Point point )  const                 { return ( ( x == point.x ) && ( y == point.y ) ); }
        bool  operator != ( Point point )  const                 { return ( ( x != point.x ) || ( y != point.y ) ); }
        Point operator+   ( Point point )  const                 { return Point( x + point.x, y + point.y ); }
        Point operator-   ( Point point )  const                 { return Point( x - point.x, y - point.y ); }
        Point operator*   ( int num )   const                    { return Point( x * num, y * num ); }
        // Here we overload the / operator so we can divide by a scalar
        Point operator/   ( int num )   const                    { return Point( x / num, y / num ); }

        void swap( Point* point )
        {
            int tmpx = point->x;
            point->x = this->x;
            this->x = tmpx;
            int tmpy = point->y;
            point->y = this->y;
            this->y = tmpy;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //  Vertex3

    //=============================================================================
    // NAME:
    // DESC: This is our basic 3D point/Vertex class
    //=============================================================================
    class Point3: public Point
    {
    public:

        long32_t   z;

        Point3()          { operator()( 0, 0, 0 ); }

        // This is our constructor that allows us to initialize our data upon creating an instance
        Point3( long32_t nx, long32_t ny, long32_t nz )          { operator()( nx, ny, nz ); }
        Point3( long32_t afCoordinate[ 3 ] )                     { x = afCoordinate[ 0 ]; y = afCoordinate[ 1 ]; z = afCoordinate[ 2 ]; }
        Point3( const Point3& v3 )                               { operator()( v3.x, v3.y, v3.z ); }
        Point3( const Point& v2 )                                { operator()( v2.x, v2.y, 0 ); }

        long32_t& operator[]( unsigned int i ) const             { return ( long32_t& ) * ( &x + i ); }

        Point3& operator=( const Point& p2 )                     { operator()( p2.x, p2.y, 0 ); return *this; }
        Point3& operator=( const Point3& p3 )                    { operator()( p3.x, p3.y, p3.z ); return *this; }
        void operator()( long32_t nx, long32_t ny, long32_t nz ) { x = nx, y = ny, z = nz; }
        bool operator ==( const Point3& p3 ) const               { return ( ( x == p3.x ) && ( y == p3.y ) && ( z == p3.z ) ); }
        bool operator!=( const Point3& p3 ) const                { return ( ( x != p3.x ) || ( y != p3.y ) || ( z != p3.z ) ); }

        // Here we overload the + operator so we can add Vertexs together
        Point3 operator+( Point3 p3 )  const                     { return Point3( x + p3.x, y + p3.y, z + p3.z ); }
        // Here we overload the - operator so we can subtract Vertexs
        Point3 operator-( Point3 p3 )  const                     { return Point3( x - p3.x, y - p3.y, z - p3.z ); }

        // Here we overload the * operator so we can multiply by scalars
        Point3 operator*( long32_t num )  const                  { return Point3( x * num, y * num, z * num ); }

        // Here we overload the / operator so we can divide by a scalar
        Point3 operator/( long32_t num ) const                   { return Point3( x / num, y / num, z / num ); }
    };

    //typedef Point3<int>  Point3i;

    /*
    void operator=(Point3i p0, Point p1)
    {
        p0.x = p1.x;
        p0.y = p1.y;
        p0.z = 0;
    }

    void operator=(Point p0, Point3i p1)
    {
        p0.x = p1.x;
        p0.y = p1.y;
    }
    */




    ///////////////////////////////////////////////////////////////////////////////
    //
    class Size: public tagSIZE
    {
    public:
        Size()                                                   { operator()( 0, 0 ); }
        Size( int w, int h )                                     { operator()( w, h ); };
        Size( const Size& sz )                                   { operator()( sz.cx, sz.cy ); }
                                                                 
        void operator() ( int w, int h )                         { cx = w; cy = h; };
                                                                 
        Size operator+ ( Size size ) const                       { return Size( cx + size.cx, cy + size.cy ); }
                                                                 
        Size operator- ( Size size ) const                       { return Size( cx - size.cx, cy - size.cy ); }
                                                                 
        Size operator* ( int num ) const                         { return Size( cx * num, cy * num ); }

        // Here we overload the / operator so we can divide by a scalar
        Size operator/ ( int num ) const                         { return Size( cx / num, cy / num ); }
    };


    ///////////////////////////////////////////////////////////////////////////////
    //  Box
    template<class _Type>
    class Box
    {

    public:

        _Type width;
        _Type height;
        _Type depth;

        Box()                                                    { operator()( 0, 0, 0 ); }
        Box( _Type w, _Type h, _Type d )                         { operator()( w, h, d ); }
        ~Box() {}                                                
                                                                 
        void operator()( _Type w, _Type h, _Type d )             { width = w; height = h; depth = d; };
        Box& operator=( const Box& box )                         { width = box.width; height = box.height; depth = box.depth; return *this; }
    };

    typedef Box<int> Boxi;
    typedef Box<UINT> Boxui;


    ///////////////////////////////////////////////////////////////////////////////
    //  CIRCUIT 圓形
    class Circuit
    {
    public:
        Point center;
        int   radius;

        Circuit() { radius = 0; };
        Circuit( const Circuit& circuit )                        { operator = ( circuit ); }
        ~Circuit() {};

        Circuit& operator=( const Circuit& circuit )             { *( Circuit* )this = circuit; return *this; }
    };


    ///////////////////////////////////////////////////////////////////////////////
    //  Line 線
    class Line2d
    {
    public:

        Point vertex[ 2 ];

        Line2d() {};
        Line2d( Point p0, Point p1 )                             { vertex[ 0 ] = p0; vertex[ 1 ] = p1; }
        Line2d( int X, int Y, int X1, int Y1 )                   { operator()( X, Y, X1, Y1 ); }
        ~Line2d() {};

        Point& operator[]( int i )                               { return vertex[ i ]; }

        void operator()( int X, int Y, int X1, int Y1 )
        {
            vertex[ 0 ].x = X;
            vertex[ 0 ].y = Y;
            vertex[ 1 ].x = X1;
            vertex[ 1 ].y = Y1;
        }

        void operator()( Point p0, Point p1 )
        {
            vertex[ 0 ] = p0;
            vertex[ 1 ] = p1;
        }

        Line2d& operator = ( const Line2d& line )
        {
            vertex[ 0 ] = line.vertex[ 0 ];
            vertex[ 1 ] = line.vertex[ 1 ];
            return *this;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    //  Linear 一次;線性
    class Linear
    {

    public:

        Point normal;
        int  cnst;

        Linear() { cnst = 0; }
        ~Linear() {}

        Linear& operator = ( const Linear& linear )
        {
            normal = linear.normal;
            cnst = linear.cnst;
            return *this;
        }
    };


    ///////////////////////////////////////////////////////////////////////////////
    //  Triangle 三角形
    class Triangle2d
    {
    public:

        Point vertex[ 3 ];


        Triangle2d() {};
        Triangle2d( Point p0, Point p1, Point p2 ) { vertex[ 0 ] = p0; vertex[ 1 ] = p1; vertex[ 2 ] = p2; }
        ~Triangle2d() {}

        Point& operator[]( int i )
        {
            return vertex[ i ];
        }

        void Vertical_Sort( void )
        {
            if( vertex[ 0 ].y < vertex[ 1 ].y )
            {
                vertex[ 0 ].swap( &vertex[ 1 ] );
            }
            if( vertex[ 0 ].y < vertex[ 2 ].y )
            {
                vertex[ 0 ].swap( &vertex[ 2 ] );
            }
            if( vertex[ 1 ].y < vertex[ 2 ].y )
            {
                vertex[ 1 ].swap( &vertex[ 2 ] );
            }
        }

        void Horizontal_Sort( void )
        {
            if( vertex[ 0 ].x < vertex[ 1 ].x )
            {
                vertex[ 0 ].swap( &vertex[ 1 ] );
            }
            if( vertex[ 0 ].y < vertex[ 2 ].y )
            {
                vertex[ 0 ].swap( &vertex[ 2 ] );
            }
            if( vertex[ 1 ].y < vertex[ 2 ].y )
            {
                vertex[ 1 ].swap( &vertex[ 2 ] );
            }
        }


        void Order_Sort( void )
        {
            if( vertex[ 0 ].x < vertex[ 1 ].x )
            {
                vertex[ 0 ].swap( &vertex[ 1 ] );
            }
            if( vertex[ 0 ].x < vertex[ 2 ].x )
            {
                vertex[ 0 ].swap( &vertex[ 2 ] );
            }
            if( vertex[ 1 ].y < vertex[ 0 ].y )
            {
                vertex[ 1 ].swap( &vertex[ 0 ] );
            }
            if( vertex[ 1 ].y < vertex[ 2 ].y )
            {
                vertex[ 1 ].swap( &vertex[ 2 ] );
            }
            if( vertex[ 2 ].y > vertex[ 0 ].y )
            {
                vertex[ 2 ].swap( &vertex[ 0 ] );
            }
            if( vertex[ 2 ].y > vertex[ 1 ].y )
            {
                vertex[ 2 ].swap( &vertex[ 1 ] );
            }
        }

        void operator()( Point p0, Point p1, Point p2 )
        {
            vertex[ 0 ] = p0;
            vertex[ 1 ] = p1;
            vertex[ 2 ] = p2;
            Order_Sort();
        }

        Point top( void )
        {
            Point max = vertex[ 0 ];
            if( vertex[ 1 ].y > max.y )
            {
                max = vertex[ 1 ];
            }
            if( vertex[ 2 ].y > max.y )
            {
                max = vertex[ 2 ];
            }
            return max;
        }

        Point bottom( void )
        {
            Point bp = vertex[ 0 ];
            if( vertex[ 1 ].y < bp.y )
            {
                bp = vertex[ 1 ];
            }
            if( vertex[ 2 ].y < bp.y )
            {
                bp = vertex[ 2 ];
            }
            return bp;
        }

        Point left( void )
        {
            Point lp = vertex[ 0 ];
            if( vertex[ 1 ].x < lp.x )
            {
                lp = vertex[ 1 ];
            }
            if( vertex[ 2 ].x < lp.x )
            {
                lp = vertex[ 2 ];
            }
            return lp;
        }

        Point right( void )
        {
            Point rp = vertex[ 0 ];
            if( vertex[ 1 ].x > rp.x )
            {
                rp = vertex[ 1 ];
            }
            if( vertex[ 2 ].x < rp.x )
            {
                rp = vertex[ 2 ];
            }
            return rp;
        }
    };


    ///////////////////////////////////////////////////////////////////////////////
    //  4邊形
    class Quadr
    {
    public:

        Point vertex[ 4 ];

        Quadr() {};
        Quadr( Point p0, Point p1, Point p2, Point p3 )
        {
            operator () ( p0, p1, p2, p3 );
        }
        ~Quadr() {};

        Point& operator[]( int i )
        {
            return vertex[ i ];
        }

        void operator()( POINT LT, POINT TR, POINT RB, POINT BL )
        {
            vertex[ 0 ] = LT;
            vertex[ 1 ] = TR;
            vertex[ 2 ] = RB;
            vertex[ 3 ] = BL;
        }

        /*
        bool collision(Point pt)
        {
            if((IS_UNDER(vertex[0],  vertex[1], pt) <= 0) && (IS_UNDER(vertex[1], vertex[2], pt) <= 0) &&
                   (IS_UNDER(vertex[3], vertex[2], pt) >= 0) && (IS_UNDER(vertex[0], vertex[3], pt) >= 0))
                     return true;
            return false;
        }
        */

        Quadr operator()( Point pt )
        {
            return Quadr( vertex[ 0 ] + pt, vertex[ 1 ] + pt,
                vertex[ 2 ] + pt, vertex[ 3 ] + pt );
        }

        Quadr operator+( Point pt ) const
        {
            return Quadr( vertex[ 0 ] + pt, vertex[ 1 ] + pt,
                vertex[ 2 ] + pt, vertex[ 3 ] + pt );
        }

    };


    ///////////////////////////////////////////////////////////////////////////////
    //  8形
    class Octagon
    {
    public:

        Point vertex[ 8 ];


        Octagon() {};
        Octagon( Point l, Point l_t, Point t, Point t_r,
            Point r, Point r_b, Point b, Point b_l )
        {
            operator() ( l, l_t, t, t_r, r, r_b, b, b_l );
        }
        ~Octagon() {};


        Point& operator[]( int i )
        {
            return vertex[ i ];
        }

        void operator()( Point l, Point l_t, Point t, Point t_r,
            Point r, Point r_b, Point b, Point b_l )
        {
            vertex[ 0 ] = l;
            vertex[ 1 ] = l_t;
            vertex[ 2 ] = t;
            vertex[ 3 ] = t_r;
            vertex[ 4 ] = r;
            vertex[ 5 ] = r_b;
            vertex[ 6 ] = b;
            vertex[ 7 ] = b_l;
        }

        Octagon operator()( Point pt )
        {
            return Octagon( vertex[ 0 ] + pt, vertex[ 1 ] + pt,
                vertex[ 2 ] + pt, vertex[ 3 ] + pt,
                vertex[ 4 ] + pt, vertex[ 5 ] + pt,
                vertex[ 6 ] + pt, vertex[ 7 ] + pt );
        }

    };


    ///////////////////////////////////////////////////////////////////////////////
    //  矩形
    class Rect: public tagRECT
    {
    public:

        Rect()                                                   { operator()( 0, 0, 0, 0 ); }
        Rect( const RECT&  rect )                                { operator()( rect.left, rect.top, rect.right, rect.bottom ); }
        Rect( Point p0, Point p1 )                               { operator()( p0, p1 ); }
        Rect( int Left, int Top, int Right, int Bottom )         { operator()( Left, Top, Right, Bottom ); }
        ~Rect() {};                                              
                                                                 
        Point operator[] ( int i )                               { return ( i == 0 ) ? Point( left, top ) : Point( right, bottom ); }
        void  operator () ( Point p0, Point p1 )                 { operator()( p0.x, p0.y, p1.x, p1.y ); }
        void  operator () ( int Left, int Top, int Right, int Bottom )
        {
            left = Left;
            top = Top;
            right = Right;
            bottom = Bottom;
        }

        Rect& operator=( const RECT& rect )                      { operator()( rect.left, rect.top, rect.right, rect.bottom ); return *this; }
        int width()                                              { return ( right - left ) + 1; }
        int height()                                             { return ( bottom - top ) + 1; }

        Rect operator+( Point pt ) const
        {
            return Rect( left + pt.x, top + pt.y,
                right + pt.x, bottom + pt.y );
        }

        bool operator==( const RECT& rect )
        {
            if( ( left == rect.left ) &&
                ( top == rect.top ) &&
                ( right == rect.right ) &&
                ( bottom == rect.bottom ) )
            {
                return true;
            }
            return false;
        }

        Point p_l_t( void )                                       { return Point( left, top ); }
        Point p_r_t( void )                                       { return Point( right, top ); }
        Point p_r_b( void )                                       { return Point( right, bottom ); }
        Point p_l_b( void )                                       { return Point( left, bottom ); }
        void  p_l_t( Point pt )                                   { left = pt.x; top = pt.y; return; }
        void  p_r_b( Point pt )                                   { right = pt.x; bottom = pt.y; return; }
                                                                  
        Point center( void )                                      { return Point( ( right - left ) / 2, ( top - bottom ) / 2 ); }
                                                                 
        void setxywh( int x, int y, uint_t w, uint_t h )          { left = x; top = y; right = left + w - 1; bottom = top + h - 1; }

        void Revise( void )
        {
            if( right < left )
            {
                SWAPL( right, left );
            }
            if( bottom < top )
            {
                SWAPL( bottom, top );
            }
        }

        Rect Translate( Point pt )
        {
            left = left + pt.x;
            top = top + pt.y;
            right = right + pt.x;
            bottom = bottom + pt.y;
            return *this;
        }

        Rect TranslateByCenter( Point pt )
        {
            Point off( pt.x - ( ( right - left ) / 2 ),
                pt.y - ( ( top - bottom ) / 2 ) );
            return Rect( left + off.x, top + off.y, right + off.x, bottom + off.y );
        }

        Quadr change_Quadr( void )
        {
            int py = ( bottom - top ) / 2;
            int px = ( right - left ) / 2;
            return Quadr( Point( left, top + py ), Point( left + px, top ),
                Point( right, top + py ), Point( left + px, bottom ) );
        }

        Octagon change_Octagon( void )
        {
            int px = ( right - left ) / 2;
            int py = ( bottom - top ) / 2;
            int dpx = px / 3;
            int dpy = py / 3;
            return Octagon( Point( left, top + py ), Point( left + dpx, top + dpy ),
                Point( left + px, top ), Point( right - dpx, top + dpy ),
                Point( right, top + py ), Point( right - dpx, bottom - dpy ),
                Point( left + px, bottom ), Point( left + dpx, bottom - dpy ) );
        }
    };
}