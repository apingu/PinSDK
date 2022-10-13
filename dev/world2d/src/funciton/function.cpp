#include "Function.h"
#include <time.h>


///////////////////////////////////////////////////////////////////////////////////////////////
///    骰子

int Dice(int Limit)
{
	int number;
	number = (rand() % Limit) ;
	return number ;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///在一區塊產生n個點
void RectPoint(RECT rect, int n, std::vector<POINT> *pts)
{
	int width  = rect.right  - rect.left;
	int height = rect.bottom - rect.top; 
	POINT pt;
	for(int i = 0; i < n; i ++)
	{
		pt.x = Dice(width)  + rect.left;
	    pt.y = Dice(height) + rect.top;
		pts->push_back(pt);
	}
}