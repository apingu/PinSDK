#ifndef PELMDEF_H
#define PELMDEF_H

#include "PDEF.h"

#pragma pack (push,1)

typedef struct
{
    //char     Title[4]     ;
    int      ID            ;  //0   4    element id
    int      GID           ;
    int      DirectionCount;  //8   4    方向數
    int      MoveDistance  ;  //12  4    移動點數
    int      clockcycle    ;  //16  4    動作的週期時間
    bool     Visible:2     ,  //20  1    可見
             Dragable:2    ,  //         可拖曳
             Enable:2      ,  //         可作用 
			 Moveable:2    ;  //         可移動
	Pbyte    Job           ;  //21  1    角色
	Pbyte    Identity      ;  //22  1    特性
    int      InitSitusX    ;  //23  4    出現時位於畫面的位置
    int      InitSitusY    ;  //27  4    出現時位於畫面的位置
    char     ApReserved[97];
}ElmData;

#pragma pack (pop)

#endif
