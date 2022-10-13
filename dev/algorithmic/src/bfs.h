#ifndef BFS_H
#define BFS_H

#include <PGDEF.h>
#include <PList.h>
#include <Tarray.h>

void ADiffuse(int x, int y, int range,
                int DirectionCount, TARRAY<bool> walkable, TARRAY<short> *WeightMap);

int  ASearchTo(int fromx, int fromy, int tox, int toy, PCC::RList<PCPOINT> *RouteList, int maxStep,
                int DirectionCount, TARRAY<short> WeightMap);

#endif