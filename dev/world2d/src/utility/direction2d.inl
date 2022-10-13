
#include <aMath.inl>

//////////////////////////////////////////////////////////////////////////////
//  direction
const int DIR_SOUTH_EAST      =  0;
const int DIR_SOUTH           =  1;
const int DIR_SOUTH_WEST      =  2;
const int DIR_WEST            =  3;
const int DIR_NORTH_WEST      =  4;
const int DIR_NORTH           =  5;
const int DIR_NORTH_EAST      =  6;
const int DIR_EAST            =  7;

inline int dirangle( LG::Point p1, LG::Point p2 )
{
	int x0 = p2.x - p1.x;
	int y0 = p2.y - p1.y;
	int x1 = 0, y1 = 1;

	return 0;
}

inline int dir360_by_slope( LG::Point p1, LG::Point p2 )
{
	LG::Point quadrant = p2 - p1;

	if( quadrant.x == 0)
	{
		if( quadrant.y == 0 )
			return -1;
		else if( quadrant.y > 0 )
			return 0;
		else
		    return 180;
	}

    else if( quadrant.x > 0 )
	{
		if( _SLOPE( &p1, &p2 ) > 2.414f )//↑
		    return 0;
		else if( _SLOPE( &p1, &p2 ) >= 0.414f )//↗
		    return 45;
		else if( _SLOPE( &p1, &p2 ) > -0.414f )//→
		    return 90;
		else if( _SLOPE( &p1, &p2 ) >= -2.414f )//↘
		    return 135;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↓
		    return 180;
	}
	else
	{
		if( _SLOPE( &p1, &p2 ) > 2.414f )//↓
		    return 180;
		else if( _SLOPE( &p1, &p2 ) >= 0.414f )//↙
    	    return 225;
		else if( _SLOPE( &p1, &p2 ) > -0.414f )//←
		    return 270;
        else if( _SLOPE( &p1, &p2 ) >= -2.414f ) //↖
	        return 315;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↑
	        return 0;
	}
	return -2;
/*
	LG::Point quadrant = p2 - p1;

	if( quadrant.x == 0)
	{
		if( quadrant.y == 0 )
			return -1;
		else if( quadrant.y > 0 )
			return 0;
		else
		    return 180;
	}

    else if( quadrant.x > 0 )
	{
		if( _SLOPE( p1, p2 ) > 2 )//↑
		    return 0;
		else if( _SLOPE( p1, p2 ) >= 0.5 )//↗
		    return 45;
		else if( _SLOPE( p1, p2 ) > -0.5 )//→
		    return 90;
		else if( _SLOPE( p1, p2 ) >= -2 )//↘
		    return 135;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↓
		    return 180;
	}
	else
	{
		if( _SLOPE( p1, p2 ) > 2 )//↓
		    return 180;
		else if( _SLOPE( p1, p2 ) >= 0.5 )//↙
    	    return 225;
		else if( _SLOPE( p1, p2 ) > -0.5 )//←
		    return 270;
        else if( _SLOPE( p1, p2 ) >= -2 ) //↖
	        return 315;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↑
	        return 0;
	}
	return -2;
*/
}

inline int dir8_by_slope( LG::Point p1, LG::Point p2 )
{
	LG::Point quadrant = p2 - p1;

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
		if( _SLOPE( &p1, &p2 ) > 2 )//↑
		    return 0;
		else if( _SLOPE( &p1, &p2 ) >= 0.5 )//↗
		    return 1;
		else if( _SLOPE( &p1, &p2 ) > -0.5 )//→
		    return 2;
		else if( _SLOPE( &p1, &p2 ) >= -2 )//↘
		    return 3;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↓
		    return 4;
	}
	else
	{
		if( _SLOPE( &p1, &p2 ) > 2 )//↓
		    return 4;
		else if( _SLOPE( &p1, &p2 ) >= 0.5 )//↙
    	    return 5;
		else if( _SLOPE( &p1, &p2 ) > -0.5 )//←
		    return 6;
        else if( _SLOPE( &p1, &p2 ) >= -2 ) //↖
	        return 7;
	    else// if( _SLOPE( p1, p2 ) > 2 )//↑
	        return 0;
	}
	return -2;
}


inline int dir4_by_slope( LG::Point p1, LG::Point p2 )
{
	LG::Point quadrant = p2 - p1;

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
        if(	_SLOPE( &p1, &p2 ) >= 0 )
	        return  0;
		else// if( _SLOPE( p1, p2 ) > 0 )
	        return  1;
	}
	else
	{
        if(	_SLOPE( &p1, &p2 ) > 0 )
	        return  2;
		else// if( _SLOPE( p1, p2 ) >= 0 )
	        return  3;
	}
	return -2;
}


inline int dir4_by_point( LG::Point p1, LG::Point p2 )
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


inline int dir8_by_point( LG::Point p1, LG::Point p2 )
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

