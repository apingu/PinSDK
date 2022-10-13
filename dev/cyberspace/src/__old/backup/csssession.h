#ifndef _CSSSESSION_H
#define _CSSSESSION_H

#include <CsDef.h>

namespace Cyberspace
{
    class PacketFactory;

    class SSession
    {

        // 用指標的目的只是不想要大家都需要include他
        PacketFactory*     m_PacketFactory;
        stimen_t           m_IdleTime;

    public:
        SSession(connectionid_t cid);
        ~SSession();

        connectionid_t GetConnectionId() const;

        void           SetIdle(bool t);
        stimen_t       GetIdleTime();

    };
}

#endif  //_CSSSESSION_H