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
// DESC : 用來存放暫存的指令訊息(用在將多序下的資料 一次處理)
//
#ifndef CSCOMMBUF_H
#define CSCOMMBUF_H

#include "CsDef.h"

namespace Cyberspace
{
    class commbuf_t
    {
    
    public:
        inline commbuf_t()
        {
            m_who = 0;
            m_cid = 0;
            m_buf = 0;
            m_len = 0;
        }
        
        inline ~commbuf_t()
        {
            SAFE_DELETE_ARRAY( m_buf );
        }
        
        uniqueid_t  m_who;
        channelid_t m_cid;
        ubyte_t*    m_buf;
        size32_t    m_len;
        //CsResBuf    m_buff;
    };
}

#endif //CSCOMMBUF_H