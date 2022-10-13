#ifndef _CSSESSIONSTORAGE_H
#define _CSSESSIONSTORAGE_H

#include "CsSSession.h"
#include <vecmap.inl>

namespace Cyberspace
{
    class SessionStorage
    {

        pt::vecmap<Cyberspace::SSession*> m_SSessionPool;  // 列表
        pt::vecmap<Cyberspace::SSession*> m_SSessionIdle;  // 斷線無效狀態

    public:

        SessionStorage(){m_SSessionPool.init(_CsMAX_DIVISION);}
        ~SessionStorage();


        void DoDisconnect(connectionid_t cid)
        {
            TS_Lock lock(&m_SSessionPool.lock(cid));
            TS_Lock lock(&m_SSessionIdle.lock(cid));
            _vecmap_iterator(Cyberspace::SSession*) iter=m_SSessionPool.find(cid);
            if(iter!=m_SSessionPool.end(cid))
            {
                m_SSessionPool.erase(iter);
                iter->second->SetIdle(true);
                m_SSessionIdle.insert(cid, iter->second);                                
            }
            return;
        }

        Cyberspace::SSession* SelectSSession(connectionid_t cid)
        {
            TS_Lock lock(&m_SSessionPool.lock(cid));
            {
                _vecmap_iterator(Cyberspace::SSession*) iter=m_SSessionPool.find(cid);
                if(iter!=m_SSessionPool.end(cid))
                {
                    return iter->second;
                }
            }

            TS_Lock lock(&m_SSessionIdle.lock(cid));
            {
                _vecmap_iterator(Cyberspace::SSession*) iter=m_SSessionIdle.find(cid);
                if(iter!=m_SSessionIdle.end(cid))
                {
                    m_SSessionIdle.erase(iter);
                    iter->second->SetIdle(false);
                    m_SSessionPool.insert(cid, iter->second);
                    return iter->second;
                }
            }
            return NULL;
        }

        bool InsertSSession(Cyberspace::SSession* sen)
        {
            connectionid_t cid = sen->GetConnectionId();
            if(cid==CsINVALID_ID)
                return false;

            TS_Lock lock(&m_SSessionPool.lock(cid));
            if(m_SSessionPool.insert(cid, sen))
                return true;
            return false;
        }

        bool RemoveSSession(Cyberspace::SSession* sen)
        {
            connectionid_t cid = sen->GetConnectionId();
            if(cid==CsINVALID_ID)
                return false;

            TS_Lock lock(&m_SSessionPool.lock(cid));
            m_SSessionPool.erase(cid);
            return true;
        }
    };
}

#endif //_CSSESSIONSTORAGE_H