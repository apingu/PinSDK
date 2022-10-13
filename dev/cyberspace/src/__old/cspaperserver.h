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

#include "CsRecord.h"
#include <XDatabase.h>
#include <queue>
#include <vmaps.inl>
#include <NetServer.h>
//#include "CsLog::FileExporter.h"
//#include "MessageDispatcher.h"


namespace CsLog
{
	class FileExporter;
}

namespace Cyberspace
{
    class MessageDispatcher;
    
    namespace Paper
    {
        struct Record;
        
        //! Paper server: 整個系統的管理類別
        //! 資料儲存用
        class Server : public Net::TCPServer//, public MessageDispatcher
        {
			class SRecord : public CsRecord
			{
			public:

				mstimen_t     create_time;
    			mstimen_t     update_time;
				mstimen_t     live_sec;
				int           ctl;

				void operator = ( CsRecord& record )
				{
					key   = record.key;
					table = record.table;
					value = record.value;
				}
			};

			typedef ptl::vmaps<SRecord*> recmap_t;

            CsLog::FileExporter*        m_logfile;
            MessageDispatcher*          m_MessageDispatcher;
            
            T_Lock                      m_utLock;
			long32_t                    m_fLogMode;
            stimen_t                    m_StartUpSec;
            serverid_t                  m_Id;
            tstring                     m_Name;
			recmap_t                    m_RecStorage;
			ptl::vmaps<recmap_t*>       m_TabStorage;
            //std::map<tstring, PfTimer*> m_WatchMap;

			int                         m_MaxDB;
			std::vector<T_Lock>         m_dbLock;
			std::queue<SRecord*>*       m_DBQueue;
            std::vector<XDataBase*>     m_Database;

        public:
        
            Server( const tchar_t* name = NULL );
            ~Server();
            
            const tchar_t*          ObjectName() const { return _T( "PaperServer" ); }
            
            serverid_t              GetLocalServerId() { return m_Id; }
            
            //! 載入設定檔
            virtual bool            LoadConfigure( const tchar_t* filename );
            
            virtual int             StartUp( unsigned int allowplayers, unsigned short port );
            
            int                     Shutdown( void );
            
            //aset log message in log file and termi
            void                    SetLogMessage( long32_t mode, const tchar_t* format, ... );
            void                    SetLogMode( long32_t mode );   //開關輸出訊息
			long32_t                GetLogMode( void );     //開關輸出訊息
            
        private:
        
            virtual XDataBase*      OnCreateDataBase( void );
            virtual void            OnDeleteDataBase( XDataBase* db );
            
        private:
        
            //! 取代上層的對應 callback
            //virtual void   OnInitiate(void);
            //! 取代上層的對應 callback
            virtual void            OnAccept( Net::TCPSocket* skt );
            //! 取代上層的對應 callback
            virtual void            OnReceive( Net::TCPSocket* skt );
            //! 取代上層的對應 callback
            virtual void            OnKickout( Net::TCPSocket* skt );
            //! 取代上層的對應 callback
            //virtual void   OnShutDown(void);
            //! kickout one client
            //virtual int             Kickout( Net::TCPSocket* s );
            
            virtual Net::TCPSocket* OnNewSocket( void );
            
            //!
            static void __cdecl     Execute( void* );
            
            static int              DoSetValue( void* evocator, const void* pdata, unsigned int len );
            static int              DoSetnxValue( void* evocator, const void* pdata, unsigned int len );
            static int              DoGetValue( void* evocator, const void* pdata, unsigned int len );
            static int              DoDelValue( void* evocator, const void* pdata, unsigned int len );
        };
        
    };
};