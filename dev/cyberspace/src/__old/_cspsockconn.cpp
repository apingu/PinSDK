//
//
//   作為server端最基本建立的socket 傳送 經過 加密 壓縮 加入header的資料
//
//
//
//
//

#include "CsConnection.h"
#include "CsSCSSystem.h"


namespace Cyberspace
{
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Connection::Connection( CsSessionDescriptor sdp )
    {
        m_SessionDescriptor = sdp;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Connection::~Connection()
    {
    }

}

