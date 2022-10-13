//
//  created:    2006/07/05
//  filename:   EMServer.h
//  file path:
//  file base:
//  file ext:   h
//  author:     Pin
//
//  purpose:
//


#ifndef CSERPSERVER_H
#define CSERPSERVER_H

#include <CsDef.h>
#include <Cyberspace_server.h>

namespace Cyberspace
{
    namespace ERP
    {
        class Server : public Cyberspace::Server
        {
        public:
            Server();
            ~Server();
            
            const tchar_t* ObjectName() const { return "EnSysServer"; }
            
            int            StartUp            ( uint_t allowplayers, unsigned short port );
            
            void           OnClientConnect    ( CsSessionDescriptor sdp );
            void           OnClientDisconnect ( CsSessionDescriptor sdp );
            
        private:
        
            static int     DoSelectBulletin   ( const ServerEvocator* evocator, const void* pData, unsigned int len );
        };
    };
};


#endif