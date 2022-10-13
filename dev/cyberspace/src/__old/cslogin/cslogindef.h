//
// Cyberspace Server common define file
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
#ifndef CSLGOINDEF_H
#define CSLGOINDEF_H

#include "CsDef.h"


namespace Cyberspace
{
    typedef byte_t* publicbuf_t;  //the entity public buffer
    typedef byte_t* privatebuf_t; //the entity private buffer
    typedef byte_t* entitybuf_t;  //the entity all data buffer, include publicbuf and privatebuf
    /**
    \fn 這是每個註冊的callback函式的原型。
    @param who 呼叫這個函式的entity id。
    @param pData 送進來的封包，請實作者自行轉型。
    @param len 封包的長度。
    */
    
    struct GameServerEvocator : public ServerEvocator
    {
        CsEntityDescriptor entitydescriptor;
    };
    typedef int( *GameServerMsgChannelFunction )( const ServerEvocator* evocator, const void* pData, unsigned int len );
    
    struct GameClientEvocator : public ClientEvocator
    {
    
    };
    typedef int( *GameClientMsgChannelFunction )( const GameClientEvocator* evocator, const void* pData, unsigned int len );
}



#endif