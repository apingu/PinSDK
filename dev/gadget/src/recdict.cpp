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

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <pf-time.h>
#include <pf-thread.h>
#include <algorithmic/crypt-crc64.h>
#include <pf-debug.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "recdict.h"


//namespace pfget
//{
//////////////////////////////////////////////////////////////////////////
//
namespace
{
    const _tstring RecditTheKeyName = _T( "RecditTheKeyValue" );  // 第一個 vlaue   call: set 放入的值
    const _tstring RecditUpdateName = _T( "RecditUpdateSTime" );  // 觸發時間
    const _tstring RecditLifetmName = _T( "RecditLifetmSTime" );  // 存活時間
    bool           g_Init = false;

    // 測試有沒有用到保留的key
    bool TestDefKeyExist( const tchar_t* key )
    {
        if( ( RecditTheKeyName == key ) || ( RecditUpdateName == key ) || ( RecditLifetmName == key ) )
        {
            return true;
        }
        return false;
    }

    // 取的key 的crc 當index
    qword_t GetKeyIndex( const tchar_t* key )
    {
        return crc64::Encode( ( const char* )key, ( unsigned int )( _tcslen( key ) * sizeof( tchar_t ) ) );
    }
}


//////////////////////////////////////////////////////////////////////////
//

//=============================================================================
//
//
//=============================================================================
Recdict::Recdict()
{

}

//=============================================================================
//
//
//=============================================================================
Recdict::~Recdict()
{
    g_Init = false;
}

//=============================================================================
//
//
//=============================================================================
void Recdict::Init( int max_vec )
{
    if( g_Init == false )
    {
        records_.init( max_vec );
        g_Init = true;
        //pfkrl::Thread::BeginProcess( DoUpdate, this, _T( "Recdict::DoUpdate" ) );
    }
}

//=============================================================================
//
//
//=============================================================================
void Recdict::Clear()
{
    records_.clear();
}

//=============================================================================
//
//
//=============================================================================
void Recdict::Update()
{
    stimen_t starttime = pftml::Time::GetSTime();
    //while ( g_Init )
    {
        //pfstd::MSSleep( 100 );

        for( uint_t i = 0; i < records_.vsize(); i++ )
        {
            std::vector<qword_t> erase_session;
            TS_Lock lock( &records_.lock( i ) );
            pfstl::vmapl<RecHash>::iterator miter = records_.begin( i );
            while( miter != records_.end( i ) )
            {
                stimen_t curtime = pftml::Time::GetSTime();
                RecHash cmap = miter->second;
                RecHash::iterator titer = cmap.find( RecditUpdateName );
                RecHash::iterator alive = cmap.find( RecditLifetmName );
                if( ( titer != cmap.end() ) && ( alive != cmap.end() ) )
                {
                    stimen_t trigger_tm = _ttoll( titer->second.c_str() );
                    stimen_t timeout_tm = _ttoll( alive->second.c_str() );

                    //_tstring errmsg;
                    //_pf_tstrprintf( errmsg, _T( "%lld %lld %lld\n" ), trigger_tm, timeout_tm, curtime - trigger_tm );
                    //Pf_INFO( errmsg.c_str() );

                    if( timeout_tm > 0 )
                    {
                        // 現在時間 撿到 上次觸發的時間 已經超過 存在的週期
                        if( ( curtime - trigger_tm ) > timeout_tm )
                        {
                            erase_session.push_back( miter->first );
                        }
                    }
                }
                miter++;
            }

            for( int h = 0; h < ( int )erase_session.size(); h++ )
            {
                records_.erase( erase_session[ h ] );
            }
        }
    }
}

//=============================================================================
//
//
//=============================================================================
void Recdict::ForAllValue( OnValue func )
{
    for( unsigned i = 0; i < records_.vsize(); i++ )
    {
        TS_Lock lock( &records_.lock( i ) );
        _vmapl_iterator( RecHash ) it = records_.begin( i );
        while( !records_.miss( i, it ) )
        {
            ( func )( it->second );
            //printf( "%s : %s\n", it->second->key.c_str(), it->second->value.c_str() );
            it++;
        }

        records_.clear( i );
    }
    records_.destroy();
}

//=============================================================================
//
//
//=============================================================================
void  Recdict::Set( const tchar_t* key, const _recvalue_t val )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        //設定值
        iter->second[ RecditTheKeyName ] = ( val );
    }
    else
    {
        RecHash thiskeyvalue;
        // 設定觸發時間
        thiskeyvalue.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        thiskeyvalue.insert( RecHash::value_type( RecditTheKeyName, ( val ) ) );
        records_.insert( keyindex, thiskeyvalue );
    }
}

//=============================================================================
//
//
//=============================================================================
bool Recdict::Setnx( const tchar_t* key, const _recvalue_t val )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 已存在 所以 加入建立失敗
        return false;
    }
    else
    {
        RecHash thiskeyvalue;
        // 更新觸發時間
        thiskeyvalue.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        thiskeyvalue.insert( RecHash::value_type( RecditTheKeyName, ( val ) ) );
        records_.insert( keyindex, thiskeyvalue );
    }
    return true;
}

//=============================================================================
//
//
//=============================================================================
bool Recdict::Setex( const tchar_t* key, const _recvalue_t val )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        //設定值
        iter->second[ RecditTheKeyName ] = ( val );
        return true;
    }
    // 原先不存在 所以加入失敗
    return false;
}

//=============================================================================
//
//
//=============================================================================
const _recvalue_t Recdict::Get( const tchar_t* key )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        RecHash::iterator itvl = iter->second.find( RecditTheKeyName );
        if( itvl != iter->second.end() )
        {
            return ( itvl->second );
        }
    }
    return _T( "" );
}

//=============================================================================
//
//
//=============================================================================
const _recvalue_t Recdict::GetSet( _IN const tchar_t* key, _IN _recvalue_t val )
{
    const tchar_t* oldval;
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        //iter->second.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        RecHash::iterator itvl = iter->second.find( RecditTheKeyName );
        if( itvl != iter->second.end() )
        {
            oldval = itvl->second.c_str();
        }
        iter->second[ RecditTheKeyName ] = ( val );
    }
    else
    {
        RecHash thiskeyvalue;
        // 更新觸發時間
        thiskeyvalue.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        thiskeyvalue.insert( RecHash::value_type( RecditTheKeyName, ( val ) ) );
        records_.insert( keyindex, thiskeyvalue );
    }
    return oldval;
}

//=============================================================================
//
//
//=============================================================================
void Recdict::Del( const tchar_t* key )
{
    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    records_.erase( keyindex );
}

//=============================================================================
//
//
//=============================================================================
void Recdict::HSet( const tchar_t* key, const tchar_t* fld, const _recvalue_t val )
{
    // 保留的key名稱不能使用
    if( TestDefKeyExist( fld ) )
    {
        return;
    }

    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        //iter->second.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        iter->second[ fld ] = ( val );
    }
    else
    {
        RecHash thiskeyvalue;
        // 設定觸發時間
        thiskeyvalue.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        thiskeyvalue.insert( RecHash::value_type( fld, ( val ) ) );
        records_.insert( keyindex, thiskeyvalue );
    }
    return;
}

//=============================================================================
//
//
//=============================================================================
bool Recdict::HSetnx( const tchar_t* key, const tchar_t* fld, const _recvalue_t val )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        RecHash::iterator itvl = iter->second.find( fld );
        if( itvl != iter->second.end() )
        {
            // 已存在 所以 加入建立失敗
            return false;
        }
        else
        {
            // 更新觸發時間
            iter->second.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
            //設定值
            iter->second.insert( RecHash::value_type( fld, ( val ) ) );
        }
    }
    // key不存在 建立一個新的
    else
    {
        RecHash thiskeyvalue;
        // 更新觸發時間
        thiskeyvalue.insert( RecHash::value_type( RecditUpdateName, ( trigger_tm_buff ) ) );
        //設定值
        thiskeyvalue.insert( RecHash::value_type( fld, ( val ) ) );
        records_.insert( keyindex, thiskeyvalue );
    }
    return true;
}

//=============================================================================
//
//
//=============================================================================
bool Recdict::HSetex( const tchar_t* key, const tchar_t* fld, const _recvalue_t val )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        //設定值
        iter->second[ fld ] = ( val );
        return true;
    }
    // key不存在 建立一個新的
    return false;
}

//=============================================================================
//
//
//=============================================================================
const _recvalue_t Recdict::HGet( const tchar_t* key, const tchar_t* fld )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        RecHash::iterator itvl = iter->second.find( fld );
        if( itvl != iter->second.end() )
        {
            return ( itvl->second );
        }
    }
    return _T( "" );
}

//=============================================================================
//
//
//=============================================================================
void Recdict::HDel( const tchar_t* key, const tchar_t* fld )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        iter->second.erase( fld );
    }
    return;
}

//=============================================================================
//
//
//=============================================================================
bool Recdict::Setlt( _IN const tchar_t* key, stimen_t timeout )
{
    tchar_t trigger_tm_buff[ 512 ];
    _lltot( pftml::Time::GetSTime(), trigger_tm_buff, 10 );
    tchar_t timeout_tm_buff[ 512 ];
    _lltot( timeout, timeout_tm_buff, 10 );

    qword_t keyindex = GetKeyIndex( key );
    TS_Lock lock( &records_.lock( keyindex ) );
    pfstl::vmapl<RecHash>::iterator iter = records_.find( keyindex );
    if( iter != records_.end( keyindex ) )
    {
        // 更新觸發時間
        iter->second[ RecditUpdateName ] = ( trigger_tm_buff );
        if( timeout > 0 )
        {
            iter->second[ RecditLifetmName ] = ( timeout_tm_buff );
        }
        else
        {
            iter->second.erase( RecditLifetmName );
        }
        return true;
    }
    return false;
}
//}