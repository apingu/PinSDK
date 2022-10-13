//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
//
//
//
//
//
#pragma once

#include "CsPacketConnection.h"
#include "CsSocketInterface.h"

namespace Cyberspace
{
    class Connection : public PacketConnection
    {
    
        CsSessionDescriptor  m_SessionDescriptor;
        
    public:
    
        Connection() {};
        Connection( CsSessionDescriptor sdp );
        ~Connection();
        
        virtual const tchar_t*     ObjectName() const { return _T( "Connection" ); }
        
        //void                     Initialize();
        //void                     SetDescriptor( CsSessionDescriptor sdp ) { m_SessionDescriptor = sdp; }
        inline CsSessionDescriptor GetDescriptor() { return m_SessionDescriptor; };
    };
}