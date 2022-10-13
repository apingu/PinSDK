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
//
//
#ifndef CAMERA2D_H
#define CAMERA2D_H

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "game_def.h"

namespace GSO
{
    class Camera2d
    {

    public:
        Camera2d();
        ~Camera2d();

        GameVertex GetPosition(void);
        void       SetPosition(GameVertex pos);

        void       Update(void);
    };
};




#endif //CAMERA2D_H