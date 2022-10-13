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
//
//
//
//
#ifndef WENTITY_H
#define WENTITY_H



#include <WSocket.h>
#include "WZone.h"

namespace WNE
{
    class Entity
    {
    public:
        Entity();
        ~Entity();

        IO::TCPSocket* m_Sockfd;
        //WNE::Zone*     m_Dwelling;

        unsigned int m_Len;
        char* m_pData;
    };
};



#endif  //WENTITY_H