#ifndef _RULE_H_
#define _RULE_H_

#include <windows.h>
#include <vector>

//#include "..\\Main\\WinMain.h"

void RectPoint(RECT rect, int n, std::vector<POINT> *pts);
int  Dice     (int Limit);

#endif