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
#pragma once
//01.== C 系統文件/C++ 系統文件
#include <vector>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"


namespace cyberspace
{
    namespace helper
    {
        namespace name
        {
            // 把id 跟資料表的位置包裝成一個address 傳給client 使用
            _cs_string MakeRecordAddress( _IN const _cs_char_t* tbname, const _cs_char_t* uid );
            bool       SplitRecordAddress( _OUT _cs_string& tbname, _OUT _cs_string& uid, _IN const _cs_char_t* address );
        }
    }
}