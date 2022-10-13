//
//
//   將原本的struct加上lock 的機制
//
//
//
//
#ifndef CSTCDATADEF_H
#define CSTCDATADEF_H

#include "CstSDataDef.h"
#include <T_Lock.h>
//#include <time.h>

namespace Net
{
    class TCPSocket;
}

namespace Cyberspace
{

    //////////////////////////////////////////////////////////////////////////
    //
    typedef _cstARBITSYMBOL  ArbitSymbol;
    
    
    //////////////////////////////////////////////////////////////////////////
    //
    typedef _cstSERVERSYMBOL ServerSymbol;
    
    
    //////////////////////////////////////////////////////////////////////////
    //
    //#define  _CONNECTNODE_ISCLONE(s)      (s->GetSocket() == NULL)? true: false;
    
    
}



#endif //CSTUTILITY_H