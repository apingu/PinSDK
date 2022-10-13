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
#pragma once


#pragma warning(disable:4786)

//#include "CsRecord.h"
#include "CsDef.h"
#include <XDatabase.h>
#include <NetSocket.h>
#include <vmapl.inl>
#include <queue>
#include "CsAppServer.h"

//#include "MessageDispatcher.h"

namespace Cyberspace
{
	class PSockPeer;

    namespace Paper
    {
        //class CPTPAppServer;
        //class HttpAppServer;
        class SRecord;
        
        //! Paper server: 整個系統的管理類別
        //! 資料儲存用
        class Server : public AppServer
        {
            //friend CPTPAppServer;
            //friend HttpAppServer;
            
            typedef PfStl::vmapl<SRecord*> recmap_t;
            
            
            T_Lock                       m_utLock;
            //CsLogMode                    m_fLogMode;
			recmap_t                     m_RecStorage;
            //PfStl::vmapl<recmap_t*>      m_TabStorage;
            
            int                          m_MaxDB;
            std::vector<T_Lock>          m_dbLock;
            std::queue<SRecord*>*        m_DBQueue;
            std::vector<XDataBase*>      m_Database;
            
        public:
        
            Server( const tchar_t* name = NULL );
            ~Server();
            
            const tchar_t*          ObjectName() const { return _T( "PaperServer" ); }
            const tchar_t*          GroupName()  const { return _T( "PaperGroup" ); }
            
            
            //! 載入設定檔
            //virtual bool            LoadConfigure( _IN const tchar_t* filename );
            
            virtual int             StartUp( _IN unsigned int allowplayers, _IN ushort_t ctptport, _IN ushort_t httpport );
            
            int                     Shutdown( void );
            
            //aset log message in log file and termi
            //void                    OutputMessage( _IN CsLogMode m, _IN const tchar_t* format, _IN ... );
            //void                    SetLogMode( _IN CsLogMode m );   //開關輸出訊息
            //CsLogMode               GetLogMode( void );          //開關輸出訊息

			void                    PrintOut();  // print out all key (debug)

            virtual XDataBase*      GetPaperDB( _IN _cs_sessionid_t id );
            
        private:

            virtual bool            OnLoadConfigure( _IN std::map<_tstring, _tstring> setting );
        
            virtual XDataBase*      OnCreatePaperDB( void );
            virtual void            OnDeletePaperDB( _IN XDataBase* db );
            
        private:
        
            //!
            static void __cdecl     Execute( _IN void* );
            
            int                     DoSetValue( _IN AppSocket* s, _IN channelid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                     DoSetnxValue( _IN AppSocket* s, _IN channelid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                     DoGetValue( _IN AppSocket* s, _IN channelid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
            int                     DoDelValue( _IN AppSocket* s, _IN channelid_t channelid, _IN const ubyte_t* combin, _IN unsigned int comlen );
        };
    };
};