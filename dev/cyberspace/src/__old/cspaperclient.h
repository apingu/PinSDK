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
//    Level 1 = Arbitration-server
//    Level 2 = SCS-Host
//    Level 3 = Client
//
//
//
//
//
#pragma once


#include "CsRecord.h"
#include <NetClient.h>

namespace Cyberspace
{
    class PacketFactory;
    
    namespace Paper
    {
        //////////////////////////////////////////////////////////////////////////
        //
        class Client: private Net::TCPClient
        {
            T_Lock              m_utLock;
            PacketFactory*      m_PacketFactory;
            tstring             m_Addr;               // server address
            ushort_t            m_Port;               // port number
			CsResBuf            m_ComBuf;
            
        public:
        
            Client();
            ~Client();
            
            const tchar_t*      ObjectName() { return _T( "PaperClinet" ); }
            
            int                 StartUp( const tchar_t* hname, unsigned short port ); // 允許幾個scs server連線 與連線的port
            int                 Shutdown( void );
            
            // @live_sec : time for paper
            // return : 1 = success, 0 = success but value already exist
			tstring             Set( _IN tstring key, _IN tstring value, _IN int live_sec = 0 );
            // set if key not exist, return != NULL false
			tstring             Setnx( _IN tstring key, _IN tstring value, _IN int live_sec = 0 );

			tstring             Get( _IN tstring key );
			// return : old value if key had exist
			tstring             Del( _IN tstring key );

			// Join key into table
			void                Join( _IN tstring tables, _IN tstringlist key  );

			tstringmap          GetTb( _IN tstring table );

			// part key from table
			void                Part( _IN tstring tables, _IN tstringlist key );
            
        private:
        
            virtual void        OnReceive( Net::TCPSocket* s );
            virtual int         Close( void );
            bool                Ping( void );
        };
    }
};



