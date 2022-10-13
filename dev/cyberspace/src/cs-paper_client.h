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
//    Level 2 = scs-Host
//    Level 3 = Client
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "cs-record.h"
#include <net/net-client.h>
//#include "CsAbtpCClient.h"

namespace cyberspace
{
    class PacketFactory;
    
    namespace Paper
    {
        //////////////////////////////////////////////////////////////////////////
        //
        class Client: private net::TCPClient
        {
            T_Lock              paperclient_ut_lock_;
            _cs_string          host_name_;               // server address
            ushort_t            host_port_;               // port number
            _csbinbuf           comm_buff_;
            
        public:
        
            Client();
            ~Client();
            
            const _cs_char_t*   ObjectName() { return ( "PaperClinet" ); }
            
            int                 StartUp( const _cs_char_t* hname, unsigned short port ); // 允許幾個scs server連線 與連線的port
            int                 Shutdown( void );
            
            // @live_sec : time for paper
            // return : 1 = success, 0 = success but value already exist
            _cs_string          Set( _IN _cs_string key, _IN _cs_string value, _IN int live_sec = 0 );
            // set if key not exist, return != NULL false
            _cs_string          Setnx( _IN _cs_string key, _IN _cs_string value, _IN int live_sec = 0 );
            
            _cs_string          Get( _IN _cs_string key );
            // return : old value if key had exist
            _cs_string          Del( _IN _cs_string key );
            // Join key into table
            void                Join( _IN _cs_string_list key, _IN _cs_string tables  );
            // part key from table
            void                Part( _IN _cs_string_list key, _IN _cs_string tables );
            
            _cs_string_map             GetTb( _IN _cs_string table );
            
            void                Insert( _IN _cs_string key, _IN _cs_string_list tables );
            void                Remove( _IN _cs_string key, _IN _cs_string_list tables );
            
        private:
        
            virtual void        OnReceive();
            virtual int         Close( void );
            bool                Ping( void );
        };
    }
};



