//回收機制
#ifndef CSCLOOECTPOOL_H
#define CSCLOOECTPOOL_H

#include <T_Lock.h>

namespace Cyberspace
{
    template <class _Type>
    class CollectPool
    {
        T_Lock                   m_utLock;
        std::map<_Type*, int>    m_tRecycleBin;  // 回收區
        std::map<_Type*, int>    m_tContainer;   // 建立的收集區

    public:

        CollectPool(){}
        ~CollectPool(){}


        // 要取出 被回收閒置多久以上的 否則就建立新的
        _Type* Create(int overtm)
        {
            DWORD currtime = ::GetTickCount();

            TS_Lock lock(&m_utLock);
            std::map<_Type*, int>::iterator iter =m_tRecycleBin.begin();
            while(iter!=m_tRecycleBin.end())
            {
                // 這個已經放在回收桶 超過 n 秒了 拿來用
                if( (currtime-iter->second)>overtm )
                {
                    _Type* tt = iter->first;
                    m_tRecycleBin.erase(iter);                                        
                    return tt;
                }
                iter++;
            }

            // 找不到 建立新的吧
            _Type* tt = new _Type;
            m_tContainer.insert(std::map<_Type*, int>::value_type(tt, currtime));

            return tt;
        }


        void  Delete(_Type* t)
        {
            DWORD currtime = ::GetTickCount();

            TS_Lock lock(&m_utLock);
            std::map<_Type*, int>::iterator iter =m_tRecycleBin.find(t);
            if(iter!=m_tRecycleBin.end())
            {
                iter->second = currtime;
                return;
            }
            m_tRecycleBin.insert(std::map<_Type*, int>::value_type(t, currtime));
            return;
        }

        // 要清出 被回收多久的 要清除幾個 為了 避免lock 住太久的時間
        // 全部都找過了 傳回 1 否則傳回0
        int ClearUp(int overtm, int n)
        {
            DWORD currtime = ::GetTickCount();

            TS_Lock lock(&m_utLock);
            std::vector<_Type*> delvec;
            int cnt=0;
            int ret=1;
            std::map<_Type*, int>::iterator iter =m_tRecycleBin.begin();
            while(iter!=m_tRecycleBin.end())
            {
                cnt++;
                // 這個已經放在回收桶 超過 n 秒了 拿來用
                if( (currtime-iter->second)>overtm )
                {
                    _Type* tt = iter->first;
                    delvec.push_back(tt);
                }
                if(delvec.size()>=n)
                {
                    ret = 0;
                    break;
                }
                iter++;
            }
            

            for(int i=0; i<delvec.size(); i++)
            {
                m_tRecycleBin.erase(delvec[i]);
                m_tContainer.erase(delvec[i]);
                SAFE_DELETE(delvec[i]);
            }

            delvec.clear();
            return ret;
        }

        void Destroy()
        {
            TS_Lock lock(&m_utLock);
            m_tRecycleBin.clear();
            std::map<_Type*, int>::iterator iter =m_tContainer.begin();
            while(iter!=m_tContainer.end())
            {
                _Type* tt = iter->first;
                SAFE_DELETE(tt);
                iter++;
            }
            m_tContainer.clear();
        }

    };


}




#endif