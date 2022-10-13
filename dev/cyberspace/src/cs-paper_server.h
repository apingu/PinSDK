// Paper server class
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
//        Pin
//           2016/07/05
//////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable:4786)

//01.== C 系統文件/C++ 系統文件
#include <queue>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <template/vmapl.inl>
#include <database/xdatabase.h>
#include <net/net-socket.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-core_server.h"


namespace cyberspace
{
    class AbtpPrSocket;

    namespace Paper
    {
        //class CPTPAppServer;
        //class HttpAppServer;
        class SRecord;
        
        //! Paper server: 整個系統的管理類別
        //! 資料儲存用
        class Server : public CoreServer
        {
            typedef std::vector<XDatabase*>  XDBPool;
            //friend CPTPAppServer;
            //friend HttpAppServer;
            
            //typedef pfstl::vmapl<SRecord*> recmap_t;
            
            
            //T_Lock                       coreserver_ut_lock_;
            //CsLogMode                    m_fLogMode;
            void*                        rec_storage_;
            //pfstl::vmapl<recmap_t*>      m_TabStorage;
            
            int                          max_db_;
            std::vector<T_Lock>          db_lock_;
            std::queue<SRecord*>*        db_queue_;
            //std::vector<XDatabase*>      database_;
            std::map< uint_t, XDBPool >  database_pool_;
            
        public:
        
            Server( const _cs_char_t* name = NULL );
            ~Server();
            
            const _cs_char_t*             ObjectName() const { return ( "PaperServer" ); }
            const _cs_char_t*             GroupName()  const { return ( "PaperGroup" ); }
            
            
            //! 載入設定檔
            //virtual bool            LoadConfigure( _IN const _cs_char_t* filename );
           
            
            int                           Shutdown( void );
            
            //aset log message in log file and termi
            //void                        OutputMessage( _IN CsLogMode m, _IN const _cs_char_t* format, _IN ... );
            //void                        SetLogMode( _IN CsLogMode m );   //開關輸出訊息
            //CsLogMode                   GetLogMode( void );              //開關輸出訊息
                                         
            void                          PrintOut();  // print out all key (debug)
                                         
            virtual XDatabase*            GetPaperDB( _IN cs_session_uid_t session_uid );
                                         
        private:                         
                                         
            virtual int                   OnServerInitialing( void );
                                         
            virtual bool                  OnLoadConfigure( _IN std::map<_cs_string, _cs_string> setting );
                                         
            virtual XDatabase*            OnCreatePaperDB( void );
            virtual void                  OnDeletePaperDB( _IN XDatabase* db );
                                         
        private:                         
                                         
            int                           SendMessage( _IN AppSocket* skt, _IN uniqueid_t eid, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN uint_t len ) { return 0; };
                                         
            //!                          
            static void __cdecl           Execute( _IN void* );
                                           
            int                           DoSetValue( _IN AppSocket* s, _IN channel_uid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                           DoSetnxValue( _IN AppSocket* s, _IN channel_uid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                           DoGetValue( _IN AppSocket* s, _IN channel_uid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                           DoDelValue( _IN AppSocket* s, _IN channel_uid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
        };
    };
};