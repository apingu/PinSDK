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
//////////////////////////////////////////////////////////////////////////

#pragma once

//01.== C 系統文件/C++ 系統文件
#include <string>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <net/net-socket.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-record.h"
#include "cs-protocol.h"


#define CsID_RECORD   (_CsMAXCHANNEL+1500)

//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::cyberspace::Abtp::SendPacket(s, CsID_ARBIT, m.Header.command_uid, m.Buff, m.Header.command_len)
//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::cyberspace::Abtp::SendPacket(s, CsINVALID_ID, m.Header.command_uid, m.Buffer.Param, m.Header.command_len)

enum CsPAPERCOMMAND
{
    CsC_S_REQUEST_SET_VALUE = CsID_RECORD + 1, // set value into paper server
    CsS_C_FEEDBACK_SET_VALUE,                  //

    CsC_S_REQUEST_SETNX_VALUE,                 // set value if not exist
    CsS_C_FEEDBACK_SETNX_VALUE,                //

    CsC_S_REQUEST_GET_VALUE,
    CsS_C_FEEDBACK_GET_VALUE,

    CsC_S_REQUEST_DEL_VALUE,
    CsS_C_FEEDBACK_DEL_VALUE,

    CsC_S_REQUEST_JOIN_TABLE,                  // set key into table
    CsS_C_REQUEST_JOIN_TABLE,                  //

    CsC_S_REQUEST_PART_TABLE,                  // set key exit table
    CsS_C_REQUEST_PART_TABLE,                  //


    CsC_S_REQUEST_TABLE_INSERT,                // set key into table
    CsS_C_REQUEST_TABLE_INSERT,                //

    CsC_S_REQUEST_TABLE_DELETE,                // set key exit table
    CsS_C_REQUEST_TABLE_DELETE,                //


    CsC_S_REQUEST_GET_TABLE_KEY,               // get key-value from table
    CsS_C_REQUEST_GET_TABLE_KEY,               //
};

//using namespace CsCore;
namespace cyberspace
{
    namespace Paper
    {
        extern _cs_size_t csrecord_fsize( _IN const CsRecord& records );
        extern _cs_size_t csrecord_printfb( _OUT ubyte_t* bufptr, _IN const CsRecord& records );
        extern _cs_size_t csrecord_scanfb( _OUT CsRecord& records, _IN const ubyte_t* bufptr, _IN _cs_size_t buflen );
    }
}


typedef std::vector<CsRecord> CsRecordList;

//=====================================================================================
//向server註冊自己的資料
//
//
namespace CsPaperCommand
{
    inline _cs_size_t tstring_list_fsize( _IN const _cs_string_list& stringlist )
    {
        _cs_size_t blen = _cssizeof( u_short );

        for( _cs_string_list::const_iterator it = stringlist.begin(); it != stringlist.end(); it++ )
        {
            blen += sizeofcsstr( ( *it ).c_str() );
        }

        return blen;
    }

    inline _cs_size_t tstring_list_printfb( _OUT ubyte_t* param, _IN const _cs_string_list& stringlist )
    {
        ubyte_t* outptr = param;
        param += CsFactory::MakeArgument( outptr, "%h", stringlist.size() );

        for( _cs_string_list::const_iterator it = stringlist.begin(); it != stringlist.end(); it++ )
        {
            outptr += CsFactory::MakeArgument( param, "%s", ( *it ).c_str() );
        }

        return _cs_size_t( outptr - param );
    }

    inline _cs_size_t tstring_list_sacnfb( _OUT ubyte_t* param, _IN const _cs_string_list& stringlist )
    {
        ubyte_t* outptr = param;
        param += CsFactory::MakeArgument( outptr, "%h", stringlist.size() );

        for( _cs_string_list::const_iterator it = stringlist.begin(); it != stringlist.end(); it++ )
        {
            outptr += CsFactory::MakeArgument( param, "%s", ( *it ).c_str() );
        }

        return _cs_size_t( outptr - param );
    }

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace SetValue
    {
        //  [  2bs size of records ][ [          record             ][  4bs live_sec ] ][ [          record             ][  4bs live_sec ] ][ [          record             ][  4bs live_sec ] ]
        namespace Client
        {
            inline int SetRequest( _OUT _csbinbuf& buf, _IN const CsRecordList& records, _IN const std::vector<int>& live_secs )
            {
                if( records.size() != live_secs.size() )
                {
                    return -1;
                }

                // compute size of all record buffer
                _cs_size_t recslen = _cssizeof( ushort_t );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    recslen += cyberspace::Paper::csrecord_fsize( records[ i ] ) + _cssizeof( int );
                }

                // resize output buffer
                buf.resize( recslen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += CsFactory::MakeArgument( param, "%h", records.size() );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    param += cyberspace::Paper::csrecord_printfb( param, records[ i ] );
                    param += CsFactory::MakeArgument( param, "%d", live_secs[ i ] );
                }

                return 1;
            }

            inline int GetFeedback( _IN const Databuf* data, _OUT CsRecordList& records, _OUT std::vector<int>& rets )
            {
                ubyte_t* param = data->buf;
                ushort_t recslen = 0;
                param += CsFactory::SplitArgument( param, "%h", &recslen );

                for( int i = 0; i < recslen; i++ )
                {
                    CsRecord record;
                    param += cyberspace::Paper::csrecord_scanfb( record, param, ( data->len - ( param - data->buf ) ) );
                    records.push_back( record );
                    int ret;
                    param += CsFactory::SplitArgument( param, "%d", &ret );
                    rets.push_back( ret );
                }

                return 1;
            }
        }

        namespace Server
        {
            inline int GetRequest( _IN const Databuf* data, _OUT CsRecordList& records, _OUT std::vector<int>& rets )
            {
                ubyte_t* param = data->buf;
                ushort_t recslen = 0;
                param += CsFactory::SplitArgument( param, "%h", &recslen );

                for( int i = 0; i < recslen; i++ )
                {
                    CsRecord record;
                    param += cyberspace::Paper::csrecord_scanfb( record, param, ( data->len - ( param - data->buf ) ) );
                    records.push_back( record );
                    int live_sec;
                    param += CsFactory::SplitArgument( param, "%d", &live_sec );
                    rets.push_back( live_sec );
                }

                return 1;
            }

            inline int SetFeedback( _OUT _csbinbuf& buf, _IN const CsRecordList& records, const _IN std::vector<int>& rets )
            {
                if( records.size() != rets.size() )
                {
                    return -1;
                }

                _cs_size_t recslen = _cssizeof( ushort_t );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    recslen += cyberspace::Paper::csrecord_fsize( records[ i ] ) + _cssizeof( int );
                }

                // resize output buffer
                buf.resize( recslen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += CsFactory::MakeArgument( param, "%h", records.size() );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    param += cyberspace::Paper::csrecord_printfb( param, records[ i ] );
                    param += CsFactory::MakeArgument( param, "%d", rets[ i ] );
                }

                return 1;
            }
        }
    };

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace GetValue  // Get/Del
    {
        namespace Client
        {
            inline int SetRequest( _OUT _csbinbuf& buf, _IN const _cs_string_list& keys )
            {
                int blen = CsPaperCommand::tstring_list_fsize( keys );
                buf.resize( blen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                tstring_list_printfb( param, keys );
                return 1;
            }

            inline int GetFeedback( _IN const Databuf* data, _OUT CsRecordList& records, _OUT std::vector<int>& rets )
            {
                ubyte_t* param = data->buf;
                ushort_t recslen = 0;
                param += CsFactory::SplitArgument( param, "%h", &recslen );

                for( int i = 0; i < recslen; i++ )
                {
                    CsRecord record;
                    param += cyberspace::Paper::csrecord_scanfb( record, data->buf, ( data->len - ( param - data->buf ) ) );
                    records.push_back( record );
                    int ret = 0;
                    param += CsFactory::SplitArgument( param, "%d", &ret );
                    rets.push_back( ret );
                }

                return 1;
            }
        }

        namespace Server
        {
            inline int GetRequest( _IN const Databuf* data, _OUT _cs_string_list& keys )
            {
                tstring_list_sacnfb( data->buf, keys );
                return 1;
            }

            inline int SetFeedback( _OUT _csbinbuf& buf, _IN const CsRecordList& records, _IN const std::vector<int> rets )
            {
                if( records.size() != rets.size() )
                {
                    return -1;
                }

                _cs_size_t recslen = _cssizeof( ushort_t );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    recslen += cyberspace::Paper::csrecord_fsize( records[ i ] ) + _cssizeof( int );
                }

                // resize output buffer
                buf.resize( recslen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += CsFactory::MakeArgument( param, "%h", records.size() );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    param += cyberspace::Paper::csrecord_printfb( param, records[ i ] );
                    param += CsFactory::MakeArgument( param, "%d", rets[ i ] );
                }

                return 1;
            }
        }
    };

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace DelValue  // Get/Del
    {
        namespace Client
        {
            inline int SetRequest( _OUT _csbinbuf& buf, _IN const _cs_string_list& keys )
            {
                int blen = CsPaperCommand::tstring_list_fsize( keys );
                buf.resize( blen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                tstring_list_printfb( param, keys );
                return 1;
            }

            inline int GetFeedback( _IN const Databuf* data, _OUT std::vector<int>& rets )
            {
                ubyte_t* param = data->buf;
                ushort_t recslen = 0;
                param += CsFactory::SplitArgument( param, "%h", &recslen );

                for( int i = 0; i < recslen; i++ )
                {
                    int ret = 0;
                    param += CsFactory::SplitArgument( param, "%d", &ret );
                    rets.push_back( ret );
                }

                return 1;
            }
        }

        namespace Server
        {
            inline int GetRequest( _IN const Databuf* data, _OUT _cs_string_list& keys )
            {
                tstring_list_sacnfb( data->buf, keys );
                return 1;
            }

            inline int SetFeedback( _OUT _csbinbuf& buf, _IN const std::vector<int> rets )
            {
                _cs_size_t recslen = _cssizeof( ushort_t ) + ( _cssizeof( int ) * rets.size() );
                // resize output buffer
                buf.resize( recslen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += CsFactory::MakeArgument( param, "%h", rets.size() );

                for( _cs_size_t i = 0; i < rets.size(); i++ )
                {
                    param += CsFactory::MakeArgument( param, "%d", rets[ i ] );
                }

                return 1;
            }
        }
    };


    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace JoinPartTable
    {
        namespace Client
        {
            inline void SetRequest( _OUT _csbinbuf& buf, _IN _cs_string_list keys, _IN _cs_string table )
            {
                int blen = CsPaperCommand::tstring_list_fsize( keys ) + sizeofcsstr( table.c_str() );
                buf.resize( blen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += tstring_list_printfb( param, keys );
                CsFactory::MakeArgument( param, "%s", table.c_str() );
                return;
            }

            inline void GetFeedback( _IN const Databuf* data, _OUT _cs_string& table, _OUT int& cnt )
            {
                _cs_char_t* tb = new _cs_char_t[ data->len + 1 ];
                ubyte_t* param = data->buf;
                CsFactory::SplitArgument( param, "%s%d", tb, &cnt );
                table = tb;
                SAFE_DELETE_ARRAY( tb );
                return;
            }
        }

        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT _cs_string_list& keys, _OUT _cs_string& table )
            {
                ubyte_t* param = data->buf;
                param += tstring_list_sacnfb( param, keys );
                _cs_char_t* tb = new _cs_char_t[ data->len + 1 ];
                CsFactory::SplitArgument( param, "%s", tb );
                table = tb;
                SAFE_DELETE_ARRAY( tb );
                return;
            }

            inline void SetFeedback( _OUT _csbinbuf& buf, _IN _cs_string table, _IN int cnt )
            {
                int blen = sizeofcsstr( table.c_str() );
                buf.resize( blen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s%d", table.c_str(), cnt );
            }
        }
    };

    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace GetTable
    {
        namespace Client
        {
            inline void SetRequest( _OUT _csbinbuf& buf, _IN _cs_string table )
            {
                buf.resize( sizeofcsstr( table.c_str() ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s", table.c_str() );
                return;
            }

            inline void GetFeedback( _IN const Databuf* data, _OUT _cs_string& table, _OUT CsRecordList& records )
            {
                _cs_char_t* tb = new _cs_char_t[ data->len + 1 ];
                ubyte_t* param = data->buf;
                ushort_t recslen = 0;
                param += CsFactory::SplitArgument( param, "%s%h", tb, &recslen );
                table = tb;
                SAFE_DELETE_ARRAY( tb );
                ushort_t buflen = data->len - ( param - data->buf );

                for( int i = 0; i < recslen; i++ )
                {
                    CsRecord record;
                    param += cyberspace::Paper::csrecord_scanfb( record, data->buf, ( buflen - ( param - data->buf ) ) );
                    records.push_back( record );
                }

                return;
            }
        }

        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT _cs_string& table )
            {
                _cs_char_t* tb = new _cs_char_t[ data->len + 1 ];
                CsFactory::SplitArgument( data->buf, "%s%", tb );
                table = tb;
                SAFE_DELETE_ARRAY( tb );
                return;
            }

            inline void SetFeedback( _OUT _csbinbuf& buf, _OUT _cs_string table, _OUT const CsRecordList& records )
            {
                _cs_size_t recslen = sizeofcsstr( table.c_str() ) + _cssizeof( ushort_t );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    recslen += cyberspace::Paper::csrecord_fsize( records[ i ] );
                }

                // resize output buffer
                buf.resize( recslen );
                ubyte_t* param = _CsGetResBufPtr( buf );
                param += CsFactory::MakeArgument( param, "%s%h", table.c_str(), records.size() );

                for( _cs_size_t i = 0; i < records.size(); i++ )
                {
                    param += cyberspace::Paper::csrecord_printfb( param, records[ i ] );
                }

                return;
            }
        }
    };
};//CsRecordCommand
