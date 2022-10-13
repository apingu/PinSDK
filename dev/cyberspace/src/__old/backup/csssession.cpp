#include "CsSSession.h"
#include "CsPacketFactory.h"
#include <time.h>


namespace Cyberspace
{

    //===============================================================================//
    //
    //
    //===============================================================================//
    SSession::SSession(connectionid_t cid)
    {
        m_PacketFactory = new Cyberspace::PacketFactory();
        m_PacketFactory->Initate(cid);
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    SSession::~SSession()
    {
        SAFE_DELETE(m_PacketFactory);
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    connectionid_t SSession::GetConnectionId() const
    {
        return m_PacketFactory->GetConnectionId();
    }

    //===============================================================================//
    //
    //
    //===============================================================================//
    void SSession::SetIdle(bool t)
    {
        if(t)
        {
            time_t ltime;
            m_IdleTime = time( &ltime );
        }
        else
        {
            m_IdleTime=0;
        }
    }


    //===============================================================================//
    //
    //
    //===============================================================================//
    stimen_t SSession::GetIdleTime()
    {
        return m_IdleTime;
    }

}