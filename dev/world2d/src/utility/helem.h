//
//
//  element file header
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
#ifndef HELEM_H
#define HELEM_H

//#include <stdio.h>

#ifdef _WINDOWS
#pragma pack (push, 1)
#else
__attribute__ ((aligned (1)))
#endif

//typedef struct
class HElem2d
{
public:

    HElem2d()
    {
        MoveDistance = 5;
        ClockCycle   = 150;
        Visible      = true;
        Enable       = true;
        Moveable     = true;
        Dragable     = false;
    }
    
    unsigned int  ApReserved1[5];
    //unsigned int  AngleOfView ;
    //unsigned int  MaxAspect   ;  //8   4    方向數
    unsigned int  MoveDistance  ;  //12  4    移動點數
    unsigned int  ClockCycle    ;  //16  4    動作的週期時間
    int           Visible  : 2  ,  //20  1    可見
                  Dragable : 2  ,  //         可拖曳
                  Enable   : 2  ,  //         可作用
                  Moveable : 2  ;  //         可移動
    //int           GID;
	//int           ID;
    unsigned char ApReserved2[96];
};

//typedef Gene2D   Gene;

typedef struct
{
    unsigned char ApReserved2[255];
}Gene3D;

#ifdef _WINDOWS
#pragma pack (pop)
#else
__attribute__ ((packed))
#endif

#endif//PGENE_H
