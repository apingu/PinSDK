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

//01.== C 系統文件/C++ 系統文件
#include <vector>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-record.h"
#include "cs-protocol.h"


namespace cyberspace
{
    namespace Paper
    {
        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecord_fsize( _IN const CsRecord& record )
        {
            int len = sizeofcsstr( record.key_.c_str() ) + sizeofcsbuf( record.value_.size() );
            return len;
        }

        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecord_printfb( _OUT ubyte_t* bufptr, _IN const CsRecord& record )
        {
            ubyte_t* outptr = bufptr;
            outptr += CsFactory::MakeArgument( bufptr, "%s%v", record.key_.c_str(), _CsGetResBufCPtr( record.value_ ), _CsGetResBufLen( record.value_ ) );
            return (_cs_size_t)( outptr - bufptr );
        }

        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecord_scanfb( _OUT CsRecord& record, _IN const ubyte_t* bufptr, _IN _cs_size_t buflen )
        {
            _cs_char_t* kb = new _cs_char_t[buflen + 1];
            const ubyte_t* outptr = bufptr;
            ubyte_t* vb = NULL;
            _cs_size_t vs = 0;
            outptr += CsFactory::SplitArgument( bufptr, "%s%p", kb, &vb, &vs );
            record.key_ = kb;
            //records[ i ].table = tb;
            record.value_.copy( vb, vs );
            SAFE_DELETE_ARRAY( kb );
            return (_cs_size_t)( outptr - bufptr );
        }

        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecords_fsize( _IN const std::vector<CsRecord>& records )
        {
            int len = 2;

            for ( _cs_size_t i = 0; i < records.size(); i++ )
            {
                //len +=  sizeofcsstr( records[i].key_.c_str() ) + sizeofcsstr( records[ i ].table.c_str() ) + sizeofcsbuf( records[ i ].value_.size() );
                len += sizeofcsstr( records[i].key_.c_str() ) + sizeofcsbuf( records[i].value_.size() );
            }

            return len;
        }

        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecords_printfb( _OUT ubyte_t* bufptr, _IN const std::vector<CsRecord>& records )
        {
            ubyte_t* outptr = bufptr;
            outptr += CsFactory::MakeArgument( bufptr, "%h", records.size() );

            for ( _cs_size_t i = 0; i < records.size(); i++ )
            {
                //outptr += CsFactory::MakeArgument( bufptr, "%s%s%v", records[i].key_.c_str(), records[ i ].table.c_str(), _CsGetResBufCPtr( records[ i ].value_ ), records[ i ].value_.size() );
                outptr += CsFactory::MakeArgument( bufptr, "%s%v", records[i].key_.c_str(), _CsGetResBufCPtr( records[i].value_ ), records[i].value_.size() );
            }

            return (_cs_size_t)( outptr - bufptr );
        }

        //===============================================================================
        //
        //
        //===============================================================================
        _cs_size_t csrecords_scanfb( _OUT std::vector<CsRecord>& records, _IN const ubyte_t* bufptr, _IN _cs_size_t buflen )
        {
            _cs_char_t* kb = new _cs_char_t[buflen + 1];
            const ubyte_t* outptr = bufptr;
            _cs_size_t recsize = 0;
            outptr += CsFactory::SplitArgument( bufptr, "%h", &recsize );
            records.resize( recsize );

            for ( _cs_size_t i = 0; i < recsize; i++ )
            {
                ubyte_t* vb = NULL;
                _cs_size_t vs = 0;
                outptr += CsFactory::SplitArgument( bufptr, "%s%p", kb, &vb, &vs );
                records[i].key_ = kb;
                //records[ i ].table = tb;
                records[i].value_.copy( vb, vs );
            }

            SAFE_DELETE_ARRAY( kb );
            return (_cs_size_t)( outptr - bufptr );
        }
    }
}
