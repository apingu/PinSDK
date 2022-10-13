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
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
#include <gadget/raw_cells.h>
#include <database/xdatabase.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"


namespace cyberspace
{
    class JsonResult
    {
        std::map< _cs_string, _cs_string >         m_pairs;
        std::map< _cs_string, _cs_string_vector >  m_arrays;

    public:

        JsonResult();
        ~JsonResult();

        static bool IsJson( _IN const _cs_string& str );

        void        InsertPair( _IN _cs_string name, _IN _cs_string value );
        void        RemovePair( _IN _cs_string name );

        bool        InsertArrays( _IN _cs_string name, _IN const _cs_string_vector& record );
        bool        InsertArrays( _IN _cs_string name, _IN const _cs_string_tableii& table );
        bool        InsertArrays( _IN _cs_string name, _IN const RawCells* cells );
        bool        InsertArrays( _IN _cs_string name, _IN XDatabase* pXdb, _IN XDBResult result );
        void        RemoveArrays( _IN _cs_string name );

        _cs_string    ToJsonString();
    };
}