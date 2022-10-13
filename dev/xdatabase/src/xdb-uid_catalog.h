//////////////////////////////////////////////////////////////////////////
#pragma once

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "xdatabase.h"

namespace xdb
{
    class UIDCatalog
    {
        XDatabase*       p_xdb_;
        int              tb_num_;
        int              tb_category_;

    public:

        UIDCatalog( XDatabase* pXDB, _IN int category = 0 );
        ~UIDCatalog();

        //void       Create();

        void       SetUniqueId( _IN uint64_t unique_id, _IN _mstring table_name );
        _mstring   GetUniqueIdTable( _IN uint64_t unique_id );
        bool       DelUniqueId( _IN uint64_t unique_id );

        bool       MoveRecord( _IN XDatabase* xdb, _IN _mstring id_field_name,  _IN uint64_t unique_id, _IN _mstring new_table_name );
    };
}

