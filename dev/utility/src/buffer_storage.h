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
//   Management container of memory buffer, so that the memory buffer can be reused need not always new
//                  by Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <vector>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>
#include <t_lock.h>

struct BufferBlock;
class BufferStorage
{
    T_Lock                      buffer_lock_;
    std::vector<BufferBlock*>   buffer_used_;
    std::vector<BufferBlock*>   buffer_pool_;
    unsigned int                buffer_init_count_;  // 一開始預建的buffer數量
    int                         buffer_min_length_;

public:

    BufferStorage();
    ~BufferStorage();

    // 0.要預先建立幾個 如果為0 取不到時會建立新的
    // 1.最小的buffer 長度
    void     Create( int count, int minlen = 0 );
    void     Destroy();

    ubyte_t* Borrow( int buflen );
    void     Return( ubyte_t* bufptr );
};