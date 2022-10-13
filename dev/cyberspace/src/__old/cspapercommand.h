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
//
//
#pragma once


#include "CsRecord.h"
#include "CsProtocol.h"
//#include "CstSDataDef.h"
#include <string>


#define CsID_RECORD   (_CsMAXCHANNEL+1500)

//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::CsSendPacket(s, CsID_ARBIT, m.Header.CommandId, m.Buff, m.Header.CommandLen)
//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::CsSendPacket(s, CsINVALID_ID, m.Header.CommandId, m.Buffer.Param, m.Header.CommandLen)

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
    
    CsC_S_REQUEST_GET_TABLE,                   // get key-value from table
    CsS_C_REQUEST_GET_TABLE,                   //
} ;

//using namespace CsCore;
namespace Cyberspace
{
	namespace Paper
	{
		extern size32_t       csrecord_fsize( _IN const CsRecord* record );
		extern size32_t       csrecord_printf( _OUT  ubyte_t* bufpt, _IN const CsRecord* record );
		extern const ubyte_t* csrecord_scanf( _OUT CsRecord& record, _IN const ubyte_t* bufptr, _IN size32_t buflen );
	}
}



//=====================================================================================
//向server註冊自己的資料
//
//
namespace CsPaperCommand
{
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace SetValue
    {
        namespace Client
        {
            //inline void SetRequest( _OUT CsResBuf& buf, _IN std::string key, _IN std::string table, _IN const ubyte_t* valbuf, _IN int vallen, _IN int live_sec )
            //{
            //    buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsstr( table.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
            //    ubyte_t* param = _CsGetResBufPtr( buf );
            //    CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), table.c_str(), valbuf, vallen, live_sec );
            //    return;
            //}

			inline void SetRequest( _OUT CsResBuf& buf, _IN const CsRecord& record, _IN int live_sec )
			{
				size32_t reclen = Cyberspace::Paper::csrecord_fsize( &record );
				buf.resize( reclen + sizeof( int ) );
				Cyberspace::Paper::csrecord_printf( _CsGetResBufPtr(buf), &record );
				ubyte_t* param = _CsGetResBufPtr( buf ) + reclen;
				CsFactory::MakeArgument( param, "%d", live_sec );
				return;
			}
            
			//inline void GetFeedback( _IN const Databuf* data, _OUT std::string& key, _OUT std::string& table, _OUT ubyte_t** valbuf, _OUT int& vallen, _OUT int& ret )
			//{
			//    tchar_t* kb = new tchar_t[ data->len + 1 ];
			//	  tchar_t* tb = new tchar_t[ data->len + 1 ];
			//    //tchar_t* vb = new tchar_t[ data->len + 1 ];
			//    CsFactory::SplitArgument( data->buf, "%s%s%p%d", kb, tb, valbuf, &vallen, &ret );
			//    key = kb;
			//	  table = tb;
			//    //oldval = vb;
			//    SAFE_DELETE_ARRAY( kb );
			//	  SAFE_DELETE_ARRAY( tb );
			//    //SAFE_DELETE_ARRAY( vb );
			//    return;
			//}

			inline void GetFeedback( _IN const Databuf* data, _OUT CsRecord& record, _OUT int& ret )
			{
				Cyberspace::Paper::csrecord_scanf( record, data->buf, data->len );
			    CsFactory::SplitArgument( data->buf + Cyberspace::Paper::csrecord_fsize( &record ), "%d", &ret );
			    return;
			}
        }
        
        namespace Server
        {
            //inline void GetRequest( _IN const Databuf* data, _OUT std::string& key, _OUT std::string& table, _OUT ubyte_t** valbuf, _OUT int& vallen, _OUT int& live_sec )
            //{
            //    tchar_t* kb = new tchar_t[ data->len + 1 ];
			//	tchar_t* tb = new tchar_t[ data->len + 1 ];
            //    //tchar_t* vb = new tchar_t[ data->len + 1 ];
            //    CsFactory::SplitArgument( data->buf, "%s%s%p%d", kb, tb, valbuf, &vallen, &live_sec );
            //    key = kb;
			//	table = tb;
            //    //valbuf = vb;
            //    SAFE_DELETE_ARRAY( kb );
			//	SAFE_DELETE_ARRAY( tb );
            //    //SAFE_DELETE_ARRAY( vb );
            //    return;
            //}
            //
            //inline void SetFeedback( _OUT CsResBuf& buf, _IN std::string key, _IN std::string table, _IN const ubyte_t* valbuf, _IN int vallen, _IN int ret )
            //{
            //    buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsstr( table.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
            //    ubyte_t* param = _CsGetResBufPtr( buf );
            //    CsFactory::MakeArgument( param, "%s%s%v%d", key.c_str(), table.c_str(), valbuf, vallen, ret );
            //    return;
            //}

			inline void GetRequest( _IN const Databuf* data, _OUT CsRecord& record, _OUT int& live_sec )
			{
				Cyberspace::Paper::csrecord_scanf( record, data->buf, data->len );
				CsFactory::SplitArgument( data->buf + Cyberspace::Paper::csrecord_fsize( &record ), "%d", &live_sec );
				return;
			}

			inline void SetFeedback( _OUT CsResBuf& buf, _IN const CsRecord& record, _IN int ret )
			{
				size32_t reclen = Cyberspace::Paper::csrecord_fsize( &record );
				buf.resize( reclen + sizeof( int ) );
				Cyberspace::Paper::csrecord_printf( _CsGetResBufPtr( buf ), &record );
				ubyte_t* param = _CsGetResBufPtr( buf ) + reclen;
				CsFactory::MakeArgument( param, "%d", ret );
				return;
			}
        }
    };
    
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace GetValue
    {
        namespace Client
        {
            inline void SetRequest( _OUT CsResBuf& buf, _IN std::string key )
            {
                buf.resize( sizeofcsstr( key.c_str() ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s", key.c_str() );
                return;
            }
            
            inline void GetFeedback( _IN const Databuf* data, _OUT std::string& key, _OUT ubyte_t** valbuf, _OUT int& vallen, _OUT int& ret )
            {
                tchar_t* kb = new tchar_t[ data->len + 1 ];
                //tchar_t* vb = new tchar_t[ data->len + 1 ];
                CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &ret );
                key = kb;
                //valbuf = vb;
                SAFE_DELETE_ARRAY( kb );
                //SAFE_DELETE_ARRAY( vb );
                return;
            }
        }
        
        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT std::string& key )
            {
                tchar_t* kb = new tchar_t[ data->len + 1 ];
                CsFactory::SplitArgument( data->buf, "%s", kb );
                key = kb;
                SAFE_DELETE_ARRAY( kb );
                return;
            }
            
            inline void SetFeedback( _OUT CsResBuf& buf, _IN std::string key, _IN const ubyte_t* valbuf, _IN int vallen, _IN int live_sec )
            {
                buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, live_sec );
                return;
            }
        }
    };
    
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace DelValue
    {
        namespace Client
        {
            inline void SetRequest( _OUT CsResBuf& buf, _IN std::string key )
            {
                buf.resize( sizeofcsstr( key.c_str() ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s", key.c_str() );
                return;
            }
            
            inline void GetFeedback( _IN const Databuf* data, _OUT std::string& key, _OUT ubyte_t** valbuf, _OUT int& vallen, _OUT int& ret )
            {
                tchar_t* kb = new tchar_t[ data->len + 1 ];
                //tchar_t* vb = new tchar_t[ data->len + 1 ];
                CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &ret );
                key = kb;
                //valbuf = vb;
                SAFE_DELETE_ARRAY( kb );
                //SAFE_DELETE_ARRAY( vb );
                return;
            }
        }
        
        
        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT std::string& key )
            {
                tchar_t* kb = new tchar_t[ data->len + 1 ];
                CsFactory::SplitArgument( data->buf, "%s", kb );
                key = kb;
                SAFE_DELETE_ARRAY( kb );
                return;
            }
            
            inline void SetFeedback( _OUT CsResBuf& buf, _IN std::string key, _IN const ubyte_t* valbuf, _IN int vallen, _IN int ret )
            {
                buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, ret );
                return;
            }
        }
    };
    
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace JoinTable
    {
        namespace Client
        {
            inline void SetRequest( _OUT CsResBuf& buf, _IN std::vector<std::string> key, _IN std::vector<std::string> table )
            {
                //buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                //ubyte_t* param = _CsGetResBufPtr( buf );
                //CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, live_sec );
                return;
            }
            
            inline void GetFeedback( _IN const Databuf* data, _OUT std::string& table, _OUT int& cnt, _OUT int& ret )
            {
                //tchar_t* kb = new tchar_t[ data->len + 1 ];
                ////tchar_t* vb = new tchar_t[ data->len + 1 ];
                //CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &ret );
                //key = kb;
                ////oldval = vb;
                //SAFE_DELETE_ARRAY( kb );
                ////SAFE_DELETE_ARRAY( vb );
                return;
            }
        }
        
        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT vector<string>& key, _OUT std::string& table )
            {
                //tchar_t* kb = new tchar_t[ data->len + 1 ];
                ////tchar_t* vb = new tchar_t[ data->len + 1 ];
                //CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &live_sec );
                //key = kb;
                ////valbuf = vb;
                //SAFE_DELETE_ARRAY( kb );
                ////SAFE_DELETE_ARRAY( vb );
                return;
            }
            
            inline void SetFeedback( _OUT CsResBuf& buf, _IN std::string table, _IN int cnt, _IN int ret )
            {
                //buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                //ubyte_t* param = _CsGetResBufPtr( buf );
                //CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, ret );
                //return;
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
            inline void SetRequest( _OUT CsResBuf& buf, _IN std::string table )
            {
                //buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                //ubyte_t* param = _CsGetResBufPtr( buf );
                //CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, live_sec );
                return;
            }
            
            inline void GetFeedback( _IN const Databuf* data, _OUT std::string& table, _OUT std::vector<CsRecord>& records )
            {
                //tchar_t* kb = new tchar_t[ data->len + 1 ];
                ////tchar_t* vb = new tchar_t[ data->len + 1 ];
                //CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &ret );
                //key = kb;
                ////oldval = vb;
                //SAFE_DELETE_ARRAY( kb );
                ////SAFE_DELETE_ARRAY( vb );
                return;
            }
        }
        
        namespace Server
        {
            inline void GetRequest( _IN const Databuf* data, _OUT std::string& table )
            {
                //tchar_t* kb = new tchar_t[ data->len + 1 ];
                ////tchar_t* vb = new tchar_t[ data->len + 1 ];
                //CsFactory::SplitArgument( data->buf, "%s%p%d", kb, valbuf, &vallen, &live_sec );
                //key = kb;
                ////valbuf = vb;
                //SAFE_DELETE_ARRAY( kb );
                ////SAFE_DELETE_ARRAY( vb );
                return;
            }
            
            inline void SetFeedback( _OUT CsResBuf& buf, _OUT std::string& table, _OUT std::vector<CsRecord>& records )
            {
                //buf.resize( sizeofcsstr( key.c_str() ) + sizeofcsbuf( vallen ) + sizeof( int ) );
                //ubyte_t* param = _CsGetResBufPtr( buf );
                //CsFactory::MakeArgument( param, "%s%v%d", key.c_str(), valbuf, vallen, ret );
                //return;
            }
        }
    };
    
};//CsRecordCommand
