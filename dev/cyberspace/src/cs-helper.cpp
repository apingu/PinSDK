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
//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "cs-helper.h"
#include "cs-db_query_base.h"


namespace cyberspace
{
    //////////////////////////////////////////////////////////////////////////
    //
    namespace helper
    {
        namespace name
        {
            _cs_string MakeRecordAddress( const _cs_char_t* tbname, const _cs_char_t* uid )
            {
                return cs_db_query::utility::MakeRecordAddress( tbname, uid );
            }

            bool SplitRecordAddress( _cs_string& tbname, _cs_string& uid, const _cs_char_t* address )
            {
                return cs_db_query::utility::SplitRecordAddress( tbname, uid, address );
            }
        }
    }
}