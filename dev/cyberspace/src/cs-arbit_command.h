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
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-protocol.h"
#include "cs-entity_def.h"

#define CsID_ARBIT   (_CsMAXCHANNEL+15450)

//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::cyberspace::Abtp::SendPacket(s, CsID_ARBIT, m.Header.command_uid, m.Buff, m.Header.command_len)
//#define SEND_ARBIT_MESSAGE(s, m)  CsCore::cyberspace::Abtp::SendPacket(s, CsINVALID_ID, m.Header.command_uid, m.Buffer.Param, m.Header.command_len)

enum CsARBITCOMMAND
{
    CsS_C_REQUEST_REPORT_HOSTINFO = CsID_ARBIT + 1, // arbit ask arbit client to send it's info
    CsC_S_FEEDBACK_REPORT_HOSTINFO,               // arbit client feedback it's info to server

    //CsC_S_REQUEST_REGISTERSERVERDATA,
    //CsS_C_REQUEST_SET_SERVERID,

    //CsC_S_REQUEST_GETHOSTINFOLIST,
    //CsS_C_FEEDBACK_GETHOSTINFOLIST,

    CsS_C_JOIN_HOSTINFO_POST,                    // arbit server post new client to all it's client
    CsS_C_EXIT_HOSTINFO_POST,                    // arbit server post new client to all it's client

    //CsS_C_SEVER_COMMAND_REQUEST,
    //CsS_C_ENTER_HOSTINFO_POST,
    //CsS_C_DEPART_HOSTINFO_POST,

    //CsC_S_REGISTER_CONNECT_REQUEST,
    //CsC_S_ESTABLISH_CsPSockConn_REQUEST,
    //CsS_C_ESTABLISH_CsPSockConn_FEEDBACK,
    //CsS_C_ESTABLISH_CsPSockConn_FEEDBACK,

    //CsC_S_JOIN_CsPSockConn_REQUEST,
    //CsS_C_JOIN_CsPSockConn_FEEDBACK,

    //CsC_S_EXIT_CsPSockConn_REQUEST,
    //CsS_C_EXIT_CsPSockConn_FEEDBACK,

    //CsS_C_SWAP_CsPSockConn_REQUEST, //將連線實體轉移到別台機器

    //CsC_S_PASS_SERVER_MESSAGE_REQUEST,
    //CsS_C_PASS_SERVER_MESSAGE_FEEDBACK,

    //CsC_S_KICKOUT_CsPSockConn_REQUEST,
    //CsS_C_KICKOUT_CsPSockConn_REQUEST,


    //CsS_C_UNREGISTER_CsPSockConn_FEEDBACK

    /*
    CsC_S_REQUEST_CREATE_UNIQUE_KEY, //client request unique key from server
    CsS_C_FEEDBACK_CREATE_UNIQUE_KEY,   //server feedback unique key to clinet

    CsC_S_REQUEST_VAILDATE_UNIQUE_KEY,  //client request validate unique key from server
    CsS_C_FEEDBACK_VAILDATE_UNIQUE_KEY,  //server feedback validate unique key to clinet

    CsC_S_REQUEST_DESTROY_UNIQUE_KEY,  //client request validate unique key from server
    CsS_C_FEEDBACK_DESTROY_UNIQUE_KEY  //server feedback validate unique key to clinet
    */



    /*
    CsC_S_REQUEST_CREATEVAILDATEKEY,//向server要求唯一的認證碼
    CsS_C_FEEDBACK_CREATEVTAILDATEKEY,//向server給client要求唯一的認證碼
    CsC_S_REQUEST_CANCELVAILDATEKE,
    CsS_C_FEEDBACK_CANCELVAILDATEKE,
    CsC_S_REQUEST_VERIFYVAILDATEKEY,
    CsS_C_FEEDBACK_VERIFYVAILDATEKEY,
    */

};// __ENUM_ATTRIBUTE;

//using namespace CsCore;


//=====================================================================================
//向server註冊自己的資料
//
//
namespace CsArbitCommand
{
    //=====================================================================================
    //
    //
    //=====================================================================================
    namespace ReportHostInfo
    {
        namespace Server
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, cs_section_uid_t sectionid, const CsArbitEntity* ahost )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_REQUEST_REPORT_HOSTINFO;
                //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%v", ahost, _cssizeof(CsArbitEntity)));
                buf.resize( sizeofcsbuf( _cssizeof( CsArbitEntity ) ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%q%v", sectionid, ahost, _cssizeof( CsArbitEntity ) );
            }

            inline void GetFeedback( ubyte_t* buff, CsSectionEntity& hnode )
            {
                //_cs_size_t len=0;
                //CsFactory::SplitArgument(buff, "%v", hnode, len);
                memcpy( &hnode, ( buff + _cssizeof( short ) ), _cssizeof( CsSectionEntity ) );
                return;
            }
        }

        namespace Client
        {
            inline void GetRequest( ubyte_t* buff, cs_section_uid_t& sectionid, CsArbitEntity& ahost )
            {
                ubyte_t* propbuf = ( ubyte_t* )&ahost;
                size32_t proplen = 0;

                //if((msg->Header.command_uid == CsS_C_REQUEST_GET_HOSTINFO) &&
                //  (msg->Header.command_len == _cssizeof(CsArbitEntity)))
                //{
                //_cs_size_t len=0;
                //CsFactory::SplitArgument(buff, "%v", ahost, &len);
                CsFactory::SplitArgument( buff, "%q%v", &sectionid, propbuf, proplen );
                //memcpy( &ahost, ( buf + _cssizeof( short ) ), _cssizeof( CsArbitEntity ) );
                //}
                return;
            }

            inline _cs_size_t SetFeedback( _csbinbuf& buf, const CsSectionEntity* hnode )
            {
                buf.resize( sizeofcsbuf( _cssizeof( CsSectionEntity ) ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v", hnode, _cssizeof( CsSectionEntity ) );
            }
        }
    };


    //=====================================================================================
    //
    //
    //=====================================================================================
    //namespace GetHostInfoList
    //{
    //    namespace Client
    //    {
    //        inline void SetRequest( _csbinbuf& buf )
    //        {
    //            //msg->Header.who = CsINVALID_ID;
    //            //msg->Header.command_uid=CsC_S_REQUEST_GETHOSTINFOLIST;
    //            //msg->Header.command_len=1;
    //            buf.resize( 1 );
    //            //char* param=&(buf[0]);
    //            return;
    //        }
    //    };
    //};

    //=====================================================================================
    // 分派一個ID給SERVER
    //
    //=====================================================================================
    //namespace SetServerUID
    //{
    //    namespace Server
    //    {
    //        inline _cs_size_t SetRequest( _csbinbuf& buf, cs_section_uid_t sid )
    //        {
    //            //msg->Header.command_uid = CsS_C_REQUEST_SET_SERVERID;
    //            //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%h", sid));
    //            buf.resize( _cssizeof( cs_section_uid_t ) );
    //            ubyte_t* param = _CsGetResBufPtr( buf );
    //            return CsFactory::MakeArgument( param, "%h", sid );
    //        }
    //    };
    //    
    //    namespace Client
    //    {
    //        inline void GetRequest( ubyte_t* buff, cs_section_uid_t& sid )
    //        {
    //            sid = 0;
    //            CsFactory::SplitArgument( buff, "%h", &sid );
    //            return;
    //        }
    //    };
    //};

    //=====================================================================================
    // server 告訴自己資料
    //
    //=====================================================================================
    namespace PostJoinHostInfo
    {
        namespace Client
        {
            inline void GetFeedback( ubyte_t* buff, CsSectionEntity& hnode )
            {
                //_cs_size_t len=0;
                //CsFactory::SplitArgument(buff, "%v", hnode, len);
                memcpy( &hnode, ( buff + _cssizeof( short ) ), _cssizeof( CsSectionEntity ) );
                return;
            }
        };


        namespace Server
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, const CsSectionEntity* hnode )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_JOIN_HOSTINFO_POST;
                //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%v", hnode, _cssizeof(CsSectionEntity)));
                buf.resize( sizeofcsbuf( _cssizeof( CsSectionEntity ) ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v", hnode, _cssizeof( CsSectionEntity ) );
            }
        };
    };

    namespace PostExitHostInfo
    {
        namespace Client
        {
            inline void GetFeedback( ubyte_t* buff, cs_section_uid_t& sid )
            {
                CsFactory::SplitArgument( buff, "%q", &sid );
                return;
            }
        };


        namespace Server
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, cs_section_uid_t sid )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_EXIT_HOSTINFO_POST;
                //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%h", sid));
                buf.resize( _cssizeof( cs_section_uid_t ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%q", sid );
            }
        };
    };



    //=====================================================================================
    //
    //node 給server自己的自定資料
    //=====================================================================================
    //namespace RegisterUserData
    //{
    //    namespace Client
    //    {
    //        inline _cs_size_t SetRequest( _csbinbuf& buf, const ubyte_t* bp, _cs_size_t len )
    //        {
    //            //msg->Header.who = CsINVALID_ID;
    //            //msg->Header.command_uid = CsC_S_REQUEST_REGISTERSERVERDATA;
    //            //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%v", bp, len));
    //            buf.resize( sizeofcsbuf( len ) );
    //            ubyte_t* param = _CsGetResBufPtr( buf );
    //            return CsFactory::MakeArgument( param, "%v", bp, len );
    //        }
    //    };
    //    
    //    namespace Server
    //    {
    //        inline const char* GetUserData( ubyte_t* buff, _cs_size_t& len )
    //        {
    //            //CsFactory::SplitArgument(buff, "%v", buff, &len);
    //            len = *( short* )buff;
    //            //if (msg->Header.command_uid == CsC_S_REQUEST_REGISTERSERVERDATA)
    //            return (char*)(buff + _cssizeof( short ));
    //        }
    //    };
    //};

    //=====================================================================================
    //
    // DESC: server 要求 連上來的server 做某件事
    //=====================================================================================
    /*
#define _ARBIT_SERVER_COMMAND_ERROR_SERVERID   5   // 設定的sectionid錯誤 不要在連線了

    namespace ServerMessage
    {
        namespace Server
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, int command, longlong_t param )
            {
                buf.resize( _cssizeof( int ) + _cssizeof( longlong_t ) );
                ubyte_t* sndbuff = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( sndbuff, "%d%q", command, param );
            }
        }


        namespace Client
        {
            inline void GetRequest( ubyte_t* buff, int& command, longlong_t& param )
            {
                CsFactory::SplitArgument( buff, "%d%q", &command, &param );
            }
        }
    }
    */

    //=====================================================================================
    // register one CsAbtpCnSocket to arbit-server when one client connect to
    //
    //=====================================================================================
    /*
            namespace EstablishConnect
            {
                namespace Server
                {
                    inline void GetRequest(ubyte_t* buff,
                             CsSessionEntity& node)
                    {
                        //_cs_size_t len=0;
                        //CsFactory::SplitArgument(buff, "%v", node, len);
                        memcpy(&node, (buff+_cssizeof(short)), _cssizeof(CsSessionEntity));
                        //if((msg->Header.command_uid == CsC_S_ESTABLISH_CsPSockConn_REQUEST) &&
                        //  (msg->Header.command_len == _cssizeof(CsSessionEntity)))
                        return;
                    }

           inline _cs_size_t SetFeedback(_csbinbuf& buf,
                    const CsSessionEntity* node)
                    {
                        //msg->Header.who = CsINVALID_ID;
                        //msg->Header.command_uid = CsS_C_ESTABLISH_CsPSockConn_FEEDBACK;
                        //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%v", node, _cssizeof(CsSessionEntity)));
                        buf.resize(sizeofcsbuf(_cssizeof(CsSessionEntity)));
                        char* param=_CsGetResBufPtr(buf);
                        return CsFactory::MakeArgument(param, "%v", node, _cssizeof(CsSessionEntity));
                    }
                };

                namespace Client
                {
                    inline _cs_size_t SetRequest(_csbinbuf& buf,
                               const CsSessionEntity* node)

                    {
                        //msg->Header.who = CsINVALID_ID;
                        //msg->Header.command_uid = CsC_S_ESTABLISH_CsPSockConn_REQUEST;
                        //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%v", node, _cssizeof(CsSessionEntity)));

                        buf.resize(sizeofcsbuf(_cssizeof(CsSessionEntity)));
                        char* param=_CsGetResBufPtr(buf);
                        return CsFactory::MakeArgument(param, "%v", node, _cssizeof(CsSessionEntity));

                    }

           inline void GetFeedback(ubyte_t* buff,
                  CsSessionEntity& node)
                    {
                        //_cs_size_t len=0;
                        //CsFactory::SplitArgument(buff, "%v", node, len);
                        memcpy(&node, (buff+_cssizeof(short)), _cssizeof(CsSessionEntity));
                        //if((msg->Header.command_uid == CsS_C_ESTABLISH_CsPSockConn_FEEDBACK) &&
                        return;

                    }

                };
            }*/



            //=====================================================================================
            //
            //
            //=====================================================================================
    namespace JoinClient
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, CsSessionEntity& node )

            {
                memcpy( &node, ( buff + _cssizeof( short ) ), _cssizeof( CsSessionEntity ) );
                return;
            }

            inline _cs_size_t SetFeedback( _csbinbuf& buf, const CsSessionEntity* node, bool result )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_JOIN_CsPSockConn_FEEDBACK;
                //return (msg->Header.command_len=CsFactory::MakeArgument(msg->Buffer.Param, "%q%c", cid, result));
                buf.resize( sizeofcsbuf( _cssizeof( CsSessionEntity ) ) + _cssizeof( bool ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v%c", node, _cssizeof( CsSessionEntity ), result );
            }
        };

        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, const CsSessionEntity* node )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsC_S_JOIN_CsPSockConn_REQUEST;
                //return msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%v", node, _cssizeof(CsSessionEntity));
                buf.resize( sizeofcsbuf( _cssizeof( CsSessionEntity ) ) + _cssizeof( bool ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v", node, _cssizeof( CsSessionEntity ) );
            }

            inline void GetFeedback( ubyte_t* buff, CsSessionEntity& node, bool& result )
            {
                //CsS_C_JOIN_CsPSockConn_FEEDBACK
                memcpy( &node, buff + _cssizeof( ushort_t ), _cssizeof( CsSessionEntity ) );
                result = ( *( bool* )( buff + _cssizeof( ushort_t ) + _cssizeof( CsSessionEntity ) ) );
                return;
            }

        };
    };



    //=====================================================================================
    // un-register one connect to arbit-server when one client kick out from server
    //
    //=====================================================================================
    namespace ExitClient
    {
        namespace Server
        {
            inline void GetRequest( ubyte_t* buff, CsSessionEntity& node )
            {
                //_cs_size_t len=0;
                //CsFactory::SplitArgument(buff, "%v", node, len);
                memcpy( &node, ( buff + _cssizeof( short ) ), _cssizeof( CsSessionEntity ) );
                return;
                //if((msg->Header.command_uid == CsC_S_EXIT_CsPSockConn_REQUEST) &&
                //(msg->Header.command_len == _cssizeof(CsSessionEntity)))
            }

            inline _cs_size_t SetFeedback( _csbinbuf& buf, const CsSessionEntity* node )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_EXIT_CsPSockConn_FEEDBACK;
                //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%v", node, _cssizeof(CsSessionEntity)));
                buf.resize( sizeofcsbuf( _cssizeof( CsSessionEntity ) ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v", node, _cssizeof( CsSessionEntity ) );
            }

        };


        namespace Client
        {
            inline _cs_size_t SetRequest( _csbinbuf& buf, const CsSessionEntity* node )
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsC_S_EXIT_CsPSockConn_REQUEST;
                //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%v", node, _cssizeof(CsSessionEntity)));
                buf.resize( sizeofcsbuf( _cssizeof( CsSessionEntity ) ) );
                ubyte_t* param = _CsGetResBufPtr( buf );
                return CsFactory::MakeArgument( param, "%v", node, _cssizeof( CsSessionEntity ) );
            }

            inline void GetFeedback( ubyte_t* buff, CsSessionEntity& node )
            {
                //_cs_size_t len=0;
                //CsFactory::SplitArgument(buff, "%v", node, len);
                memcpy( &node, ( buff + _cssizeof( short ) ), _cssizeof( CsSessionEntity ) );
                return;
                //if((msg->Header.command_uid == CsS_C_EXIT_CsPSockConn_FEEDBACK) &&
                //  (msg->Header.command_len == _cssizeof(CsSessionEntity)))
            }

        };

    };


    //=====================================================================================
    //
    // DESC: 用來傳遞各伺服器間的資料
    //  2008/04/01  該功能在加入dis各似服器互相各自連線後，似忽已用不到
    //=====================================================================================
    /*
    namespace PassMessage
    {
        namespace Server
        {
            inline void GetRequest(ubyte_t* msg,
                     cs_section_uid_t& fsid, cs_section_uid_t& tsid,
                                    channel_uid_t& cid,
                     ubyte_t* &buff, _cs_size_t& len)
            {
                CsFactory::SplitArgument(msg, "%h%h%d%v", &fsid, &tsid, &cid, &buff, &len);
                return;
            }

    inline _cs_size_t SetFeedback(_csbinbuf& buf,
                        cs_section_uid_t fsid, cs_section_uid_t tsid,
                                       channel_uid_t cid,
            const ubyte_t* buff, _cs_size_t len)
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsS_C_PASS_SERVER_MESSAGE_FEEDBACK;
                //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%h%h%d%v", fsid, tsid, cid, buff, len));
                buf.resize(_cssizeof(cs_section_uid_t)+_cssizeof(cs_section_uid_t)+_cssizeof(channel_uid_t)+sizeofcsbuf(len));
                char* param=_CsGetResBufPtr(buf);
                return CsFactory::MakeArgument(param, "%h%h%d%v", fsid, tsid, cid, buff, len);
            }
        };

        namespace Client
        {
            inline _cs_size_t SetRequest(_csbinbuf& buf,
                       cs_section_uid_t fsid, cs_section_uid_t tsid,
                                      channel_uid_t cid,
                       const ubyte_t* buff, _cs_size_t len)
            {
                //msg->Header.who = CsINVALID_ID;
                //msg->Header.command_uid = CsC_S_PASS_SERVER_MESSAGE_REQUEST;
                //return (msg->Header.command_len = CsFactory::MakeArgument(msg->Buffer.Param, "%h%h%d%v", fsid, tsid, cid, buff, len));

                buf.resize(_cssizeof(cs_section_uid_t)+_cssizeof(cs_section_uid_t)+_cssizeof(channel_uid_t)+sizeofcsbuf(len));
                char* param=_CsGetResBufPtr(buf);
                CsFactory::MakeArgument(param, "%h%h%d%v", fsid, tsid, cid, buff, len);
            }

    inline void GetFeedback(ubyte_t* msg,
                      cs_section_uid_t& fsid, cs_section_uid_t& tsid,
                                     channel_uid_t& cid,
          ubyte_t* &buff, _cs_size_t& len)
            {
                CsFactory::SplitArgument(msg, "%h%h%d%v", &fsid, &tsid, &cid, &buff, &len);
                return;
            }

        };


    }
    */
    /*
    //=====================================================================================
    // Create one unique key from arbit-server
    //
    //=====================================================================================
    namespace CreateUniqueKey
    {
    namespace Client
    {
    inline void SetRequest(CsCommandPacket* msg, ushort_t count)
    {
               CsFactory::MakeCommand(msg,
                                       CsC_S_REQUEST_CREATE_UNIQUE_KEY,
                                       "%h",
                                       count);
    }

    long32_t GetFeedbackUniqueKey(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsS_C_FEEDBACK_CREATE_UNIQUE_KEY)
    {
                    long32_t key;
                    CsFactory::SplitCommand(msg,
                              CsS_C_FEEDBACK_CREATE_UNIQUE_KEY,
                                             "%l", &key);
                    return key;
    }
    return 0;
    }
    };

    namespace Server
    {
      ushort_t GetRequestUniqueKeyCount(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsC_S_REQUEST_CREATE_UNIQUE_KEY)
    {
                    ushort_t count;
                    CsFactory::SplitCommand(msg,
                              CsC_S_REQUEST_CREATE_UNIQUE_KEY,
                                             "%h", &count);
                    return count;
    }
    return 0;
    }

    inline void SetFeedback(CsCommandPacket* msg, long32_t ukey)
    {
               CsFactory::MakeCommand(msg,
                                       CsS_C_FEEDBACK_CREATE_UNIQUE_KEY,
                                       "%l", ukey);

    }
    };

    };
    */

    /*

    //=====================================================================================
    //取得驗證碼
    //
    //=====================================================================================
    namespace CreateVaildateKey
    {
    namespace Client
    {
    inline void SetRequest(CsCommandPacket* msg)
    {
    msg->Header.command_uid = CsC_S_REQUEST_CREATEVAILDATEKEY;
    msg->Header.command_len = 1;
    msg->Buffer.Param[0] = 1;
    //memcpy(msg->Buffer.Param, , len);
    }

    long32_t GetValidateKey(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsS_C_FEEDBACK_CREATEVTAILDATEKEY)
    {
     return *((long32_t *) msg->Buffer.Param);
    }
    return 0;
    }
    };

    namespace Server
    {
    inline void SetFeedback(CsCommandPacket* msg, long32_t validatekey)
    msg->Header.command_uid = CsS_C_FEEDBACK_CREATEVTAILDATEKEY;
    msg->Header.command_len = _cssizeof(long32_t);
    (*(long32_t *) (msg->Buffer.Param)) = validatekey;
    //memcpy(msg->Buffer.Param, , len);
    }
    };
    };

    //=====================================================================================
    //取消驗證碼
    //
    //=====================================================================================
    namespace CancelVaildateKey
    {
    namespace Client
    {
    inline void SetRequest(CsCommandPacket* msg, long32_t validatekey)
    {
    msg->Header.command_uid = CsC_S_REQUEST_CANCELVAILDATEKE;
                msg->Header.command_len = _cssizeof(long32_t);
    (*(long32_t *) (msg->Buffer.Param)) = validatekey;
    //memcpy(msg->Buffer.Param, , len);
    }

    inline bool GetResult(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsS_C_FEEDBACK_CANCELVAILDATEKE)
    {
     return *((bool *) msg->Buffer.Param);
    }
    return 0;
    }

    };

    namespace Server
    {
    inline void SetFeedback(CsCommandPacket* msg, bool b)
    {
    msg->Header.command_uid = CsS_C_FEEDBACK_CANCELVAILDATEKE;
                msg->Header.command_len = 1;
    msg->Buffer.Param[0] = b;
    //memcpy(msg->Buffer.Param, , len);
    }

    inline long32_t GetValidateKey(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsC_S_REQUEST_CANCELVAILDATEKE)
    {
     return *((long32_t *) msg->Buffer.Param);
    }
    return 0;
    }
    };
    };

    //=====================================================================================
    //驗證驗證碼
    //
    //=====================================================================================
    namespace VerifyVaildateKey
    {
    namespace Client
    {
    inline void SetRequest(CsCommandPacket* msg, long32_t validatekey)
    {
    msg->Header.command_uid = CsC_S_REQUEST_VERIFYVAILDATEKEY;
                msg->Header.command_len = _cssizeof(long32_t);
    (*(long32_t *) (msg->Buffer.Param)) = validatekey;
    //memcpy(msg->Buffer.Param, , len);
    }

    inline bool GetResult(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsS_C_FEEDBACK_VERIFYVAILDATEKEY)
    {
     return *((bool *) msg->Buffer.Param);
    }
    return 0;
    }

    };

    namespace Server
    {
    inline long32_t GetValidateKey(CsCommandPacket* msg)
    {
    if (msg->Header.command_uid == CsS_C_FEEDBACK_VERIFYVAILDATEKEY)
    {
     return *((long32_t *) msg->Buffer.Param);
    }
    return 0;
    }

    inline void SetFeedback(CsCommandPacket* msg, bool b)
    {
    msg->Header.command_uid = CsS_C_FEEDBACK_VERIFYVAILDATEKEY;
                msg->Header.command_len = 1;
    msg->Buffer.Param[0]=b;
    //memcpy(msg->Buffer.Param, , len);
    }

    };
    };
    */
};//CsArbitCommand