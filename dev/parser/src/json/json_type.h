//
// Json 物件
//
//
//
//
//
//
//
//
#pragma once

#include <map>
#include <string>
#include <pf-def.h>


class JsonType
{
    _mstring    type_name_;
    _mbsmap     pairs_;

public:

    JsonType();
    ~JsonType();

    const char* ObjectName() { return type_name_.c_str(); }

    operator _mbsmap () { return pairs_; }

    uint_t       size();
    // jo["AAAA"] = "bbaa"......
    _mstring&    operator[]( const _mstring& field );
    _mstring&    operator[]( const uint_t& index );

    _mstring     Stringify();

    bool         Parser( const char* str );

    // 去掉前後的 ""
    static _mstring ToString( _mstring str );

    // 跳脫字元
    static _mstring EscapeString( _mstring str );

    //bool         ParserUri( const tchar_t* str );  //放在這物件內有點怪怪的 先註解起來
};
