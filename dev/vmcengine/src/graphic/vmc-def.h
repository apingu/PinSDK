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
#ifndef VMCDEF_H
#define VMCDEF_H

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "icon.h"

#define COPY             0XFF
#define JUMP             0X00

#define _P_ERROR         0
#define _P_OK            1

#define _WINDOWS_T       0
#define _FULLSCREEN_T    1

#define _RECT_LENGTH(e, s)    ((e - s) + 1)

namespace LGB
{
    typedef CPIcon  CMemCanvas;

    enum MATRIX_TYPE
    {
        MT_MODELVIEW_MATRIX,
        MT_PROJECTION_MATRIX
    };
};



#endif