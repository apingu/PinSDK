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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <net/unp.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"


#pragma pack (push,1)

// Arbitserver data struct
namespace cyberspace
{
    struct _ArbitEntity
    {
        char           hostname[ _MAX_HOST_NAME ];
        ushort_t       salt;
        unsigned int   maxnodesize;
        mstimen_t      starttime;     // 伺服器開始啟動的時間
        mstimen_t      currtime;
    };

    // cyberspace server data struct

    struct _SectionEntity
    {
        cs_section_uid_t sectionid;                         //
        stimen_t         createtime;
        stimen_t         connecttime;                      // set by arbit    
        char             hostname[ _MAX_HOST_NAME ];
        char             objectname[ _MAX_SERVERNAME ];      // object name
        char             aliasname[ _MAX_ALIASNAME ];        // alias name
        char             functionname[ _MAX_FUNCTIONNAME ];  // function name, for scs group
        ushort_t         maxprocess;
        ushort_t         limitconnect;
        ushort_t         usedconnect;
        ulong64_t        maxmemorysize;
        ulong64_t        usedmemorysize;
        Pflag            _state;
        //Pflag            fvisible  : 1,                  //package version
        //                 fstate    : 63;
        ushort_t         sym_addrlen;
        ipn6_t           sym_addr[ _MAX_IPNSIZE ];
        ushort_t         pub_port;                         // 開給clinet 的port
        ushort_t         scs_port;                         // 給scs的port


        Pflag             _param;                          // server 自行定義的
        // 以下 private 由 收到端自行設定
        // 之後再考慮要不要做成public 與 private 多種結構
        ushort_t         connect_ip;                       // 指定有連結到的 sym_addr 的位置
                                                           //sockaddr_in    addr;
                                                           //unsigned int   starttime;
    };


    // _state
#define _HOST_FLAG_HIDE_             1

#define ___SET_HOST_INVISIBLE(f)     (SET_BIT_FLAG( f, _HOST_FLAG_HIDE_))
#define ___SET_HOST_VISIBLE(f)       (CANCEL_BIT_FLAG( f, _HOST_FLAG_HIDE_))
#define ___IF_HOST_VISIBLE(f)        (!IF_BIT_FLAG( f, _HOST_FLAG_HIDE_))

    //_param
#define _SECTION_PARAM_LOCAL_       3  // 被設定 代表 是 收到的那台自己的  資料    

#define ___SET_SECTION_LOCAL(s)     (SET_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))
#define ___SET_SECTION_NONLOCAL(s)  (CANCEL_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))
#define ___IF_SECTION_LOCAL(s)      (IF_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))

#define _SECTION_PARAM_ENABLE_      7  // 被設定 代表 被丟到回收資料裡了    

#define ___SET_SECTION_ENABLE(s)    (SET_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))
#define ___SET_SECTION_DISABLE(s)   (CANCEL_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))
#define ___IF_SECTION_ENABLEL(s)    (IF_BIT_FLAG( s->_param, _SECTION_PARAM_LOCAL_))


    typedef struct _NetAdapter
    {
        ipn4_t           ip;
    } NetAdapter;

    //every client connect node


    enum ConnectNodeStateType
    {
        _CONNECTNODE_MASTER = 1,      //0 是clone 1 是master
        _CONNECTNODE_SWAPPING = 5,      //1 是正在交換狀態中
        _CONNECTNODE_KICKOUT = 9,      //1 已經被踢除了 (給 baseserver 用)我也覺得怪怪的??
        _CONNECTNODE_INVALID = 10,     //1 已經被移除了
    };


    struct _SessionEntity
    {
        cs_session_uid_t  sessionid;
        cs_section_uid_t  sectionid;       // the connected server
                                          //ipn4_t          ipn4;             // clinet 端是從哪一個ip連出來的
        ipn6_t            ipn6;            // clinet 端是從哪一個ip連出來的
        ushort_t          port;            // clinet 端是從哪一個port連出來的
        stimen_t          createsec;       // 建立的日期
        long32_t          flag;            // 該連線目前的狀態
        stimen_t          lifttime;        // 存在的時間
        //macn_t           mac;
        //int              group;            // 該帳號群組
        //cs_authority_t        power;            // 權力代號
        //uniqueid_t       index;            // DBHistory的歷史紀錄 初始值應該為 0
        //mstimen_t        createmst;        // 建立的同步時間

        // 以下 private 由 收到端自行設定
        // 之後再考慮要不要做成public 與 private 多種結構        
        Pflag           _param;             //session 自行定義的  // 1 master, 2 clone
    };

    //_param
#define _SESSION_PARAM_MASTER_      2     // 1 master, 2 clone

#define ___SET_SESSION_MASTER(s)    (SET_BIT_FLAG( s->_param, _SESSION_PARAM_MASTER_))
#define ___SET_SESSION_CLONE(s)     (CANCEL_BIT_FLAG( s->_param, _SESSION_PARAM_MASTER_))
#define ___IF_SESSION_MASTER(s)     (IF_BIT_FLAG( s->_param, _SESSION_PARAM_MASTER_))

#define _SESSION_PARAM_ENABLE_       7  // 被設定 代表 被丟到回收資料裡了    

#define ___SET_SESSION_ENABLE(s)    (SET_BIT_FLAG( s->_param, _SESSION_PARAM_ENABLE_))
#define ___SET_SESSION_DISABLE(s)   (CANCEL_BIT_FLAG( s->_param, _SESSION_PARAM_ENABLE_))
#define ___IF_SESSION_ENABLEL(s)    (IF_BIT_FLAG( s->_param, _SESSION_PARAM_ENABLE_))


}



//#define _CSTSYMBOL_GET_IPA( s, h )     net::Socket::IPN6ToA( s->ip[h] )
//#define _CSTSYMBOL_SET_IPN( s, h, n )   s->ip[h] =  net::Socket::GetSockAddrIPN6( n )


#pragma pack ( pop )

//////////////////////////////////////////////////////////////////////////
//
typedef cyberspace::_ArbitEntity   CsArbitEntity;


//////////////////////////////////////////////////////////////////////////
//
typedef cyberspace::_SectionEntity CsSectionEntity;


//////////////////////////////////////////////////////////////////////////
//
typedef cyberspace::_SessionEntity CsSessionEntity;
