//
// 小型的 record_t dictionary
//
//
//
//
//
//
//
//
#pragma once

#include <pf-def.h>
#include <pf-binbuf.h>
#include <template/vmapl.inl>

//namespace pfget
//{
#define _recvalue_t PfBinBuf

class Recdict
{
public:

    typedef std::map<_tstring, _recvalue_t>  RecHash;

private:

    pfstl::vmapl<RecHash>    records_;    // 所有key value 值的容器

public:

    Recdict();
    ~Recdict();

    void                Init( int max_vec = 100 );
    void                Clear();
    // 要有一個獨立的迴圈 進行更新
    void                Update();

    typedef void        ( _CALLBACK *OnValue )( RecHash );
    void                ForAllValue( OnValue );

    // [ Key 不存在: 就建立新的並設值, 存在: 設值 ]
    void                Set( _IN const tchar_t* key, _IN const _recvalue_t val );
    // [ Key 不存在: 設值 true,       存在: 不設值 false ]
    bool                Setnx( _IN const tchar_t* key, _IN const _recvalue_t val );
    // [ Key 不存在: 不設值 false,    存在: 設值 true ]
    bool                Setex( _IN const tchar_t* key, _IN const _recvalue_t val );
    const _recvalue_t   Get( _IN const tchar_t* key );
    // [ Key 不存在: 建立新的設值,     存在: 設值 傳回舊值 ]
    const _recvalue_t   GetSet( _IN const tchar_t* key, _IN const _recvalue_t val );
    void                Del( _IN const tchar_t* key );

    // [ ( Key + fld ) 不存在: 就建立新的並設值, 存在: 設值 ]
    void                HSet( _IN const tchar_t* key, _IN const tchar_t* fld, _IN const _recvalue_t val );
    // [ ( Key + fld ) 不存在: 設值 true,       存在: 不設值 false ]
    bool                HSetnx( _IN const tchar_t* key, _IN const tchar_t* fld, _IN const _recvalue_t val );
    // [ ( Key )       不存在: 不設值 false,    存在: 設值 true ]
    bool                HSetex( _IN const tchar_t* key, _IN const tchar_t* fld, _IN const _recvalue_t val );
    const _recvalue_t   HGet( _IN const tchar_t* key, _IN const tchar_t* fld );
    void                HDel( _IN const tchar_t* key, _IN const tchar_t* fld );

    // Life time 設定有效時間 超過時間就會被移除 key 必須先存在 否則傳回 null
    // timeout <= 0 永遠存活
    bool                Setlt( _IN const tchar_t* key, stimen_t timeout = -1 );
};
//}