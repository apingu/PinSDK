// PCShape.h header file of graphics shape
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
//                                               Copyright    (C)    2001    Pin
//

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "pdef.h"

#define _p_circuit_  0X00    //圓形
#define _p_point_    0X01    //點
#define _p_line_     0X02    //線
#define _p_triangle_ 0X03    //三角形
#define _p_rect_     0X04    //矩形
#define _p_rhombus_  0X14    //菱形
#define _p_quadr_    0X13    //扇形
#define _p_octagon_  0X08    //八邊形
#define _p_polygon_  0X0F    //多邊形


#ifdef _WIN32

    #include <windows.h>

#else

    #include <X11/Xlib.h>
    #include <X11/X.h>
    #include <X11/Xutil.h>
    #include <unistd.h>

    typedef struct tagRECT
    {
        LONG    left;
        LONG    top;
        LONG    right;
        LONG    bottom;
    } RECT, *PRECT, *NPRECT, *LPRECT;

    typedef const RECT *LPCRECT;

    typedef struct tagPOINT
    {
        LONG  x;
        LONG  y;
    } POINT, *PPOINT, *NPPOINT, *LPPOINT;


    typedef struct tagSIZE
    {
        LONG        cx;
        LONG        cy;
    } SIZE, *PSIZE, *LPSIZE;



#endif


///////////////////////////////////////////////////////////////////////////////
//  graphic funtion

#define _PIXEL565( r, g, b )        ( (WORD)( ((r >> 3) << 11) |  ((g >> 2) << 5)  | (b >> 3) ) )
#define _PIXEL555( r, g, b )        ( (WORD)( ((r >> 3) << 10) |  ((g >> 3) << 5)  | (b >> 3) ) )
#define _PIXEL556( r, g, b )        ( (WORD)( ((r >> 3) << 11) |  ((g >> 3) << 6)  | (b >> 2) ) )
#define _PIXEL655( r, g, b )        ( (WORD)( ((r >> 2) << 10) |  ((g >> 3) << 5)  | (b >> 3) ) )
#define _PIXELARGB( r, g, b, a )    ( (DWORD)( a<<24 | r<<16 | g<<8 | b ) )
#define _PIXELRGBA( r, g, b, a )    ( (DWORD)( r<<24 | g<<16 | b<<8 | a ) )

#define SLOPE( p1, p2 )             (float)( (float)( p2.y - p1.y ) / (float)( p2.x - p1.x ) )
#define IS_UNDER(p1,p2,p3)          ((p2.y-p1.y)*(p3.x-p1.x)-(p3.y-p1.y)*(p2.x-p1.x))


typedef Pdword  COLORTYPE;


typedef Pdword  Color32b;
typedef Pword   Color16;

class Color32
{
public:

    float R;
    float G;
    float B;
    float A;

    Color32 ()                                            { R = 0; G = 0; B = 0; A = 0; }
    Color32 ( float r, float g, float b, float a = 1)     { R = r; G = g; B = b; A = a; }
    void operator() ( float r, float g, float b, float a) { R = r; G = g; B = b; A = a; }

    Color32& operator=(const Color32 &color)
    {
        R = color.R;
        G = color.G;
        B = color.B;
        A = color.A;
        return *this;
    }

};




///////////////////////////////////////////////////////////////////////////////
//  point class
class PCPoint : public tagPOINT
{
public:

    PCPoint(){x = 0; y = 0;}
    PCPoint(int X, int Y){ this->x = X; this->y = Y;}
    PCPoint(const POINT & point)
    {
        *(POINT *) this = point;
    }

    PCPoint& operator=(const PCPoint &point)
    {
        x = point.x;
        y = point.y;
        return *this;
    }

    PCPoint operator()(int X, int Y)
    {
       x = X;
       y = Y;
       return *this;
    }

    bool operator == (PCPoint point)
    {
        if((x == point.x) && (y == point.y))
            return true;
        return false;
    }

    bool operator != (PCPoint point)
    {
        if((x == point.x) && (y == point.y))
            return false;
        return true;
    }

    PCPoint operator+ (PCPoint point)
    {
        return PCPoint(x + point.x, y + point.y);
    }

    PCPoint operator- (PCPoint point)
    {
        return PCPoint(x - point.x, y - point.y);
    }

	PCPoint operator*( int num)
	{
		// Return the scaled vector
		return PCPoint(x * num, y * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	PCPoint operator/( int num)
	{
		// Return the scale vector
		return PCPoint(x / num, y / num);
	}

    void swap( PCPoint *point )
    {
        int tmpx = point->x;
        point->x = this->x;
        this->x  = tmpx;

        int tmpy = point->y;
        point->y = this->y;
        this->y  = tmpy;
    }

};



///////////////////////////////////////////////////////////////////////////////
//

class PCSize : public tagSIZE
{
public:
    PCSize()              {cx = 0; cy = 0;}
    PCSize( int w, int h ){ cx = w; cy = h; };
    void operator() ( int w, int h ){ cx = w; cy = h; };

	PCSize operator+ (PCSize size)
    {
        return PCSize(cx + size.cx, cy + size.cy);;
    }

    PCSize operator- (PCSize size)
    {
        return PCSize(cx - size.cx, cy - size.cy);;
    }

	PCSize operator*( int num)
	{
		// Return the scaled vector
		return PCSize(cx * num, cy * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	PCSize operator/( int num)
	{
		// Return the scale vector
		return PCSize(cx / num, cy / num);
	}


};

///////////////////////////////////////////////////////////////////////////////
//  CIRCUIT 圓形

class PCCircuit
{
public:

    PCCircuit(){width = 0; height = 0;};
    PCCircuit(int Width, int Height){ this->width = Width; this->height = Height;};
    PCCircuit(const PCCircuit & circuit)
    {
        *(PCCircuit *) this = circuit;
    }
    PCCircuit(const POINT & point)
    {
        *(PCCircuit *) this = point;
    }

    int width;
    int height;

    PCCircuit& operator=(const POINT &point)
    {
        width  = point.x;
        height = point.y;
        return *this;
    }

    PCCircuit& operator=(const PCCircuit &circuit)
    {
        width  = circuit.width;
        height = circuit.height;
        return *this;
    }

    void operator()(int Width, int Height)
    {
       width  = Width;
       height = Height;
    }


};



///////////////////////////////////////////////////////////////////////////////
//  Line 線
class PCLine
{
public:

    PCPoint LP;
    PCPoint RP;

    PCLine(){LP.x = 0; LP.y = 0; RP.x = 0; RP.y = 0;};
    PCLine(PCPoint LP, PCPoint RP)
    {
       LP.x = LP.x;
       LP.y = LP.y;
       RP.x = RP.x;
       RP.y = RP.y;
    }
    PCLine(int X, int Y, int X1, int Y1)
    {
       LP.x = X  ;
       LP.y = Y  ;
       RP.x = X1 ;
       RP.y = Y1 ;
    }

    ~PCLine(){};

    void operator()(int X, int Y, int X1, int Y1)
    {
       LP.x = X  ;
       LP.y = Y  ;
       RP.x = X1 ;
       RP.y = Y1 ;
    }

    void operator()(PCPoint pt1, PCPoint pt2)
    {
       LP = pt1  ;
       RP = pt2  ;
    }

    PCLine& operator = (const PCLine &line)
    {
        LP = line.LP   ;
        RP = line.RP   ;
        return *this   ;
    }

};


///////////////////////////////////////////////////////////////////////////////
//  Triangle 三角形
class PCTriangle
{
public:

    PCPoint pt0;//t
    PCPoint pt1;//r
    PCPoint pt2;//l

    PCTriangle( PCPoint p0, PCPoint p1, PCPoint p2 ){ pt0 = p0; pt1 = p1; pt2 = p2; }
    ~PCTriangle(){}


    void Vertical_Sort( void )
    {
        if ( pt0.y < pt1.y )
            pt0.swap( &pt1 );
        if ( pt0.y < pt2.y )
            pt0.swap( &pt2 );

        if ( pt1.y < pt2.y )
            pt1.swap( &pt2 );
    }

    void Horizontal_Sort( void )
    {
        if ( pt0.x < pt1.x )
            pt0.swap( &pt1 );
        if ( pt0.y < pt2.y )
            pt0.swap( &pt2 );

        if ( pt1.y < pt2.y )
            pt1.swap( &pt2 );
    }


    void Order_Sort( void )
    {
        if ( pt0.x < pt1.x )
            pt0.swap( &pt1 );
        if ( pt0.x < pt2.x )
            pt0.swap( &pt2 );

        if ( pt1.y < pt0.y )
            pt1.swap( &pt0 );
        if ( pt1.y < pt2.y )
            pt1.swap( &pt2 );

        if ( pt2.y > pt0.y )
            pt2.swap( &pt0 );
        if ( pt2.y > pt1.y )
            pt2.swap( &pt1 );
    }

    void operator() ( PCPoint p0, PCPoint p1, PCPoint p2 )
    {
        pt0 = p0;
        pt1 = p1;
        pt2 = p2;
        Order_Sort();
    }

    PCPoint top( void )
    {
        PCPoint max = pt0;
        if( pt1.y > max.y)
            max = pt1;
        if( pt2.y > max.y )
            max = pt2;
        return max;
    }

    PCPoint bottom( void )
    {
        PCPoint bp = pt0;
        if( pt1.y < bp.y)
            bp = pt1;
        if( pt2.y < bp.y )
            bp = pt2;
        return bp;
    }


    PCPoint left( void )
    {
        PCPoint lp = pt0;
        if( pt1.x < lp.x)
            lp = pt1;
        if( pt2.x < lp.x )
            lp = pt2;
        return lp;
    }

    PCPoint right( void )
    {
        PCPoint rp = pt0;
        if( pt1.x > rp.x)
            rp = pt1;
        if( pt2.x < rp.x )
            rp = pt2;
        return rp;
    }
};




///////////////////////////////////////////////////////////////////////////////
//  4邊形
class PCQuadr
{
public:

    PCPoint lt;
    PCPoint tr;
    PCPoint rb;
    PCPoint bl;

    PCQuadr(){lt(0, 0); tr(0, 0); rb(0, 0); bl(0, 0);};
    PCQuadr(PCPoint pt1, PCPoint pt2, PCPoint pt3, PCPoint pt4)
    {
         operator () (pt1, pt2, pt3, pt4);
    }
    ~PCQuadr(){};


    void operator () ( POINT LT, POINT TR, POINT RB, POINT BL )
    {
         lt = LT;
         tr = TR;
         rb = RB;
         bl = BL;
    }


    bool is_in_area(PCPoint pt)
    {
        if((IS_UNDER(lt,  tr, pt) <= 0) && (IS_UNDER(tr, rb, pt) <= 0) &&
               (IS_UNDER(bl, rb, pt) >= 0) && (IS_UNDER(lt, bl, pt) >= 0))
                 return true;
        return false;
    }

    PCQuadr operator() (PCPoint pt)
    {
        PCQuadr shp( lt + pt, tr + pt,
                     rb + pt, bl + pt);
        return shp;
    }

    PCQuadr operator + (PCPoint pt)
    {
        PCQuadr shp( lt + pt, tr + pt,
                     rb + pt, bl + pt);
        return shp;
    }

};




///////////////////////////////////////////////////////////////////////////////
//  8形
class PCOctagon
{
public:

    PCPoint left;
    PCPoint top;
    PCPoint right;
    PCPoint bottom;
    PCPoint left_top;
    PCPoint top_right;
    PCPoint right_bottom;
    PCPoint bottom_left;

    PCOctagon(){};
    PCOctagon(POINT l, POINT t, POINT r, POINT b,
                      POINT l_t, POINT t_r, POINT r_b, POINT b_l)
    {
        operator() (l, t, r, b, l_t, t_r, r_b, b_l);
    }
    ~PCOctagon(){};

    void operator () (POINT l, POINT t, POINT r, POINT b,
                      POINT l_t, POINT t_r, POINT r_b, POINT b_l)
    {
         left         = l;
         top          = t;
         right        = r;
         bottom       = b;
         left_top     = l_t;
         top_right    = t_r;
         right_bottom = r_b;
         bottom_left  = b_l;
    }

    PCOctagon operator() (PCPoint pt)
    {
        PCOctagon shp( left + pt, top + pt, right + pt, bottom + pt,
                       left_top + pt, top_right + pt,
                       right_bottom + pt, bottom_left + pt);
        return shp;
    }



    bool is_in_area(PCPoint pt)
    {
        if((IS_UNDER(left, left_top, pt) <= 0) && (IS_UNDER(left_top, top, pt) <= 0) &&
            (IS_UNDER(top, top_right, pt) <= 0) && (IS_UNDER(top_right, right, pt) <= 0) &&
              (IS_UNDER(right, right_bottom, pt) <= 0) && (IS_UNDER(right_bottom, bottom, pt) <= 0) &&
               (IS_UNDER(bottom, bottom_left, pt) <= 0) && (IS_UNDER(bottom_left, left, pt) <= 0))
               return true;
        return false;
    }

};





///////////////////////////////////////////////////////////////////////////////
//  矩形
class PCRect: public tagRECT
{
public:

    PCRect(){top = 0; bottom = 0; left = 0; right = 0;};
    PCRect(const RECT & rect)
    {
        *(PCRect *) this = rect;
    }

    PCRect(PCPoint pt1, PCPoint pt2)
    {
        operator()(pt1, pt2);

        return;
    }

    PCRect(int Left, int Top, int Right, int Bottom)
    {
        operator()(Left, Top, Right, Bottom);
        return;
    }

    ~PCRect(){};


    PCRect& operator =  (const RECT &rect)
    {
        top    = rect.top   ;
        bottom = rect.bottom;
        left   = rect.left  ;
        right  = rect.right ;

        return *this;
    }

    int width()
    {
        return (right - left) + 1;
    }

    int height()
    {
        return (bottom - top) + 1;
    }

    void operator () (PCPoint pt1, PCPoint pt2)
    {
       left   = pt1.x ;
       top    = pt1.y ;
       right  = pt2.x ;
       bottom = pt2.y ;
    }

    void operator () (int Left, int Top, int Right, int Bottom)
    {
       left   = Left  ;
       top    = Top   ;
       right  = Right ;
       bottom = Bottom;
    }

    PCRect operator + (PCPoint pt)
    {
        return PCRect( left + pt.x, top + pt.y,
                       right + pt.x, bottom + pt.y );
    }


    PCRect operator - (PCPoint pt)
    {
        return PCRect( left - pt.x, top - pt.y,
                       right - pt.x, bottom - pt.y );
    }


    PCPoint p_l_t(void)
    {
        PCPoint pt(left, top);
        return pt;
    }

    PCPoint p_r_t(void)
    {
        PCPoint pt(right, top);
        return pt;
    }

    PCPoint p_r_b(void)
    {
        PCPoint pt(right, bottom);
        return pt;
    }

    PCPoint p_l_b(void)
    {
        PCPoint pt(left, bottom);
        return pt;
    }

    void p_l_t(PCPoint pt)
    {
        left = pt.x;
        top  = pt.y;
        return;
    }

    void p_r_b(PCPoint pt)
    {
        right  = pt.x;
        bottom = pt.y;
        return;
    }

    PCPoint center(void)
    {
        PCPoint cpt( (right - left)/2, (top - bottom)/2 );
        return cpt;
    }



    bool is_in_area(PCPoint pt)
    {
        if ((pt.x <= right) && (pt.x >= left)
            && (pt.y <= bottom) && (pt.y >= top))
                return true;
        return false;
    }

    bool is_collision(PCRect rect)
    {
        return  (min((rect.right), (bottom)) >= max((rect.left), (left)) &&
                 min((rect.bottom), (bottom)) >= max((rect.top), (bottom)));
    }

	PCRect OffsetByCenter(PCPoint pt)
	{
		PCPoint off( pt.x - ( (right - left)/2 ),
			         pt.y - ( (top - bottom)/2 ) );

		return PCRect( left + off.x, top + off.y, right + off.x, bottom + off.y );
	}


    PCRect Translate(PCPoint pt)
    {
       left   = left   + pt.x ;
       top    = top    + pt.y ;
       right  = right  + pt.x ;
       bottom = bottom + pt.y ;
       return *this;
    }

    PCQuadr change_Quadr(void)
    {
        int py = (bottom - top)/2;
        int px = (right - left)/2;

        PCPoint pleft  (left, top + py);
        PCPoint ptop   (left + px, top);
        PCPoint pright (right, top + py);
        PCPoint pbottom(left + px, bottom);
        PCQuadr p4(pleft, ptop, pright, pbottom);

        return p4;
    }

    PCOctagon change_Octagon(void)
    {
        int px = (right - left)/2;
        int py = (bottom - top)/2;

        int dpx = px/3;
        int dpy = py/3;

        PCPoint pleft  (left, top + py);
        PCPoint ptop   (left + px, top);
        PCPoint pright (right, top + py);
        PCPoint pbottom(left + px, bottom);
        PCPoint left_top(left + dpx, top + dpy);
        PCPoint top_right(right - dpx, top + dpy);
        PCPoint right_bottom(right - dpx, bottom - dpy);
        PCPoint bottom_left(left + dpx, bottom - dpy);

        PCOctagon p8(pleft,ptop,pright,pbottom,
                     left_top, top_right,
                     right_bottom, bottom_left);
        return p8;
    }

};


///////////////////////////////////////////////////////////////////////////////
//  direction

const int DIR_SOUTH_EAST      =  0;
const int DIR_SOUTH           =  1;
const int DIR_SOUTH_WEST      =  2;
const int DIR_WEST            =  3;
const int DIR_NORTH_WEST      =  4;
const int DIR_NORTH           =  5;
const int DIR_NORTH_EAST      =  6;
const int DIR_EAST            =  7;

inline int dir8_by_slope( PCPoint p1, PCPoint p2 )
{
	PCPoint quadrant = p2 - p1;

	if( quadrant.x == 0)
	{
		if( quadrant.y == 0 )
			return -1;
		else if( quadrant.y > 0 )
			return 0;
		else
		    return 4;
	}

    else if( quadrant.x > 0 )
	{
		if( SLOPE( p1, p2 ) > 2 )//↑
		    return 0;
		else if( SLOPE( p1, p2 ) >= 0.5 )//↗
		    return 1;
		else if( SLOPE( p1, p2 ) > -0.5 )//→
		    return 2;
		else if( SLOPE( p1, p2 ) >= -2 )//↘
		    return 3;
	    else// if( SLOPE( p1, p2 ) > 2 )//↓
		    return 4;
	}
	else
	{
		if( SLOPE( p1, p2 ) > 2 )//↓
		    return 4;
		else if( SLOPE( p1, p2 ) >= 0.5 )//↙
    	    return 5;
		else if( SLOPE( p1, p2 ) > -0.5 )//←
		    return 6;
        else if( SLOPE( p1, p2 ) >= -2 ) //↖
	        return 7;
	    else// if( SLOPE( p1, p2 ) > 2 )//↑
	        return 0;
	}
	return -2;
}

inline int dir4_by_slope( PCPoint p1, PCPoint p2 )
{
	PCPoint quadrant = p2 - p1;

	if( quadrant.x == 0 )
	{
		if( quadrant.y == 0 )
			return -1;
		if( quadrant.y > 0 )
			return 0;
		else
		    return 2;
	}
	else if( quadrant.x > 0 )
	{
        if(	SLOPE( p1, p2 ) >= 0 )
	        return  0;
		else// if( SLOPE( p1, p2 ) > 0 )
	        return  1;
	}
	else
	{
        if(	SLOPE( p1, p2 ) > 0 )
	        return  2;
		else// if( SLOPE( p1, p2 ) >= 0 )
	        return  3;
	}
	return -2;
}

inline int dir4_by_point( PCPoint p1, PCPoint p2 )
{
	if( (p1.x == p2.x) && (p1.y < p2.y) ) //↑
	    return 0;
	else if( (p2.x > p1.x) && (p2.y > p1.y) )  //↗
	    return 0;
  	else if( (p1.y == p2.y) && (p1.x < p2.x) ) //→
	    return 1;
	else if( (p2.x > p1.x) && (p2.y < p1.y) )  //↘
        return 1;
	else if( (p1.x == p2.x) && (p1.y > p2.y) ) //↓
	    return 2;
	else if( (p2.x < p1.x) && (p2.y < p1.y) )  //↙
	    return 2;
	else if( (p1.y == p2.y) && (p1.x > p2.x) ) //←
		return 3;
    else if( (p2.x < p1.x) && (p2.y > p1.y) )   //↖
        return 3;
	else//( (p1.x == p2.x) && (p1.y == p2.y) )
	    return -1;

    return -2;
}

inline int dir8_by_point( PCPoint p1, PCPoint p2 )
{
	if( (p1.x == p2.x) && (p1.y < p2.y) ) //↑
	    return 0;
	else if( (p2.x > p1.x) && (p2.y > p1.y) )  //↗
	    return 1;
  	else if( (p1.y == p2.y) && (p1.x < p2.x) ) //→
	    return 2;
	else if( (p2.x > p1.x) && (p2.y < p1.y) )  //↘
        return 3;
	else if( (p1.x == p2.x) && (p1.y > p2.y) ) //↓
	    return 4;
	else if( (p2.x < p1.x) && (p2.y < p1.y) )  //↙
	    return 5;
	else if( (p1.y == p2.y) && (p1.x > p2.x) ) //←
		return 6;
    else if( (p2.x < p1.x) && (p2.y > p1.y) )   //↖
        return 7;
	else//( (p1.x == p2.x) && (p1.y == p2.y) )
	    return -1;

    return -2;
}

#endif
