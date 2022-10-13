// Cyber space common define file
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
#include <list>
#include <map>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-binbuf.h>
#include <net/unp.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


//new version define mask
//#define _NEW_VERSION_0005
#define CsSERVER_ID                        0X0000000000000000
#define CsINVALID_ID                       -1    // 寫成 -1 而不是 0XFFFFFFFFFFFFFFFF 因為在gcc底下 會因為長度的不同 而不給過
//#define CsINVALID_ID  0XFFFFFFFFFFFFFFFF


//typedef unsigned int unsigned int;
//typedef std::map<int,void *> CsPtrMap;    // 這個很常用...


#define _CsMAXCHANNEL                      50000
#define _CsMAXSTRING                       256
#define _CsMAXPROPERTY                     2048

#define _CsMAX_CRYPTKEY_SIZE               16
#define _CsMAX_DIVISION                    1000
#define _CsMAX_LOG_STRING                  256

#ifndef _MAX_IPNSIZE                      
#define _MAX_IPNSIZE                       10
#endif                                    


typedef char                               _cs_char_t;
typedef size64_t                           _cs_size_t;

typedef _mstring                           _cs_string;
typedef _mbsvector                         _cs_string_vector;
typedef _mbslist                           _cs_string_list;
typedef _mbsmap                            _cs_string_map;
typedef _mbstableii                        _cs_string_tableii;


typedef uniqueid_t                         cs_section_uid_t;
typedef uniqueid_t                         cs_session_uid_t;

typedef int                                cs_flag_t;
typedef _cs_string                         cs_group_uid_t;
typedef longlong_t                         cs_permissionll_t;
typedef _cs_string                         cs_permissionss_t;
typedef std::map<_cs_string, _cs_string>   cs_permissionfs_t;  //field string
//typedef _cs_string           power_t;


//typedef uniqueid_t             cs_session_uid_t;   //as a UniqueId_t
typedef uint64_t                           channel_uid_t;


//typedef PfBinBuf                           _csbinbuf;
#define _csbinbuf                          PfBinBuf

#define CsINVALID_SESSIONID                0    // 寫成 -1 而不是 0XFFFFFFFFFFFFFFFF 因為在gcc底下 會因為長度的不同 而不給過

// class _csbinbuf : public std::vector<char>
// {
//
// public:
//  //operator char*() { return begin(); }
//
// };

#define _CsGetResBufPtr(b)                 ((ubyte_t*)b)
#define _CsGetResBufCPtr(b)                ((const ubyte_t*)b)
#define _CsGetResBufCStr(b)                ((const char*)b.c_str())
#define _CsGetResBufLen(b)                 (b.size())
#define _cssizeof(b)                       (_cs_size_t)sizeof(b)
#define _csstrlen(b)                       (_cs_size_t)strlen(b)



//#define _MAX_HOST_NAME                      32
#define _MAX_SERVERNAME                    16
#define _MAX_FUNCTIONNAME                  16
#define _MAX_ALIASNAME                     64
#define _MAX_ERROR_RETRY_TIMES             20

#define IsCsInvalidId(id)                  ((id == CsINVALID_ID) || (id <= 0))? true: false

#define _MIN_SLEEP                         10


/// 用來計算更新service運行時間的單位
//typedef ulong32_t GameFrame_t;

// for debugging
#ifdef _WINDOWS
#define DBGTRAP _asm int 3
#endif


enum CseMEMBER_STATE
{
    ACCSTATE_ERROR = -1,
    ACCSTATE_OFFLINE = 0,
    ACCSTATE_ONLINE = 1,   // 上線中
    ACCSTATE_FAILURE = 2,   // 永久不允許
    ACCSTATE_FREEZE = 3,   // 帳號封鎖
    ACCSTATE_MAX,
    //ACCSTATE_LEAVING,  //目前似乎沒用到
};


enum CseENTITY_STATE
{
    ENTITYSTATE_ERROR = -1,
    ENTITYSTATE_OFFLINE = 0,
    ENTITYSTATE_ONLINE,
    ENTITYSTATE_FREEZE,
    ENTITYSTATE_DELETE
};


enum CseCONNECT_TYPE
{
    CONNECTTYPE_MASTER,//
    CONNECTTYPE_CLONE,
};

typedef uint_t CsLogMode;
enum CseLOG_MODE
{
    LOGMODE_NONE = 0,
    LOGMODE_PRINTOUT = 1,   // 要列印在終端上
    LOGMODE_SAVEFILE = 2,   // 要寫入記錄檔(嚴重才寫入 減少時間)
    LOGMODE_OUTDEBUG = 4,   // 要寫入除錯
    LOGMODE_DEFINE = 12,  // 使用原本的設定
};


enum CsePAPER_RESULT
{
    PAPERRESULT_NOT_RETURN = -4,
    PAPERRESULT_OVERDUE = -3,
    PAPERRESULT_NOT_EXIST = -2,
    PAPERRESULT_EXIST_FAILURE = -1,
    PAPERRESULT_UNKNOW = 0,
    PAPERRESULT_SUCCESS = 1,
    PAPERRESULT_EXIST_SUCCESS = 2,  //SET
    PAPERRESULT_NOT_EXIST_SUCCESS = 3,  //DEL
};

//////////////////////////////////////////////////////////////////////////
//
typedef int CsEncType;


//////////////////////////////////////////////////////////////////////////
enum CsAbtpEncType
{
    CsAbtpEncType_Binary = 1,

    CsAbtpEncType_Http_Application = 14,  // 
    CsAbtpEncType_Http_Multipart = 15,
    CsAbtpEncType_Http_Text = 16,
};

//////////////////////////////////////////////////////////////////////////
// Session note
typedef ulonglong_t              notetid_t;
class CsSessionNote
{
public:

    CsSessionNote() {};
    virtual ~CsSessionNote() {};
    virtual const notetid_t GetTypeId() = 0;
};

//////////////////////////////////////////////////////////////////////////
//
namespace cyberspace
{
    struct _SectionEntity;
    struct _SessionEntity;
}


//typedef cyberspace::scs::PeerSection*        CsSectionDescriptor;  // server
typedef cyberspace::_SectionEntity* CsSectionDescriptor;

//typedef cyberspace::scs::SessionSymbol*      CsSessionDescriptor;  // connect client  Session  代表每一個連結到SERVER的CLINET
typedef cyberspace::_SessionEntity* CsSessionDescriptor;      // connect client  Session  代表每一個連結到SERVER的CLINET

//////////////////////////////////////////////////////////////////////////
//

namespace cyberspace
{
    typedef ulong32_t frame_t;

    //////////////////////////////////////////////////////////////////////////
    //
    /**
    \fn 這是每個註冊的callback函式的原型。
    @param who 呼叫這個函式的entity id。
    @param combin 送進來的封包，請實作者自行轉型。
    @param len 封包的長度。
    */

    class __SLAPI AppServer;
    class __SLAPI AppSocket;

    /*
    namespace app
    {
        struct ServerEvocator
        {
            AppServer*   p_server;
            AppSocket*   p_client;
        };
        //typedef __SLAPI int( *const ServerAbtpChannelFunction )( _IN const App::ServerEvocator* evocator, _IN const void* combin, _IN unsigned int len );
        //typedef __SLAPI std::string( *const ServerHttpChannelFunction )( _IN const App::ServerEvocator* evocator, _IN CsHttpContentType enctype, _IN const char* actstr, _IN const char* comstr, _IN int comlen );
    }

    //class __SLAPI CoreServer;
    //class __SLAPI UnitClient;
    namespace core
    {
        struct Evocator
        {
            CsSessionDescriptor sessiondescriptor;
            uniqueid_t          uniqueid;
        };

        struct ServerEvocator : public Evocator
        {
            UnitServer*         pserver;             //呼叫的 server
        };
        typedef __SLAPI int( *const ServerAbtpChannelFunction )( _IN const ServerEvocator* evocator, _IN const void* combin, _IN unsigned int len );

        struct ClientEvocator : public Evocator
        {
            UnitClient*         pclient;             //呼叫的client
        };
        typedef __SLAPI int( *const ClientChannelFunction )( const ClientEvocator* evocator, const void* combin, unsigned int len );
    }

    namespace scs
    {
        struct ServerEvocator
        {
            uniqueid_t         uniqueid;
            cs_section_uid_t   sectionid;           //從哪個sever 來的
            UnitServer*        pserver;             //呼叫的 server
        };
        typedef __SLAPI int( *const ServerAbtpChannelFunction )( const scs::ServerEvocator* evocator, const void* combin, unsigned int len );
    }
    */


    //////////////////////////////////////////////////////////////////////////
    //
    // Server 使用
    struct ServerState
    {
        stimen_t      startup_stime;            // server startup的時間
        mstimen_t     initate_mstime;           // server 初始化的時間
        channel_uid_t max_proc_channel_id;      // 目前花最多時間的channel id
        mstimen_t     max_proc_channel_mstime;  // 目前花最多時間的channel 處理時間
        mstimen_t     max_update_mstime;        // 目前最高的update處理時間
        uint_t        max_connected_client;     // 目前最高的連線數量
        uint_t        cnt_living_thread;        // 還活著的 polling thread
        uint_t        cnt_long_process;         // 處理超過(1秒的訊號數)
        uint_t        cnt_recv_message;         // 總接收的處理訊息次數
        uint_t        limit_allow_connected;    // 設定允許最高連線數量
    };

    //////////////////////////////////////////////////////////////////////////
    //
    class ServerInfo
    {

    public:

        cs_section_uid_t _sid;
        char             _ip[ _MAX_IPNSIZE ][ _MAX_ALIASNAME ];
        ushort_t         _port;
        int              _hostlen;
        char             _hostname[ _MAX_HOST_NAME ];
        char             _aliasname[ _MAX_SERVERNAME ];
        //char            _functionname[ _MAX_FUNCTIONNAME ];
        unsigned int     _maxconnect;
        unsigned int     _usedconnect;
        unsigned int     _limitconnect;
        Pflag            _state;

        ServerInfo()
        {
            memset( this, 0, _cssizeof( ServerInfo ) );
        }

        ServerInfo( const ServerInfo& rhs )
        {
            memcpy( this, &rhs, _cssizeof( ServerInfo ) );
        }

        char* GetIp( int i ) { return _ip[ i ]; }

    };
};