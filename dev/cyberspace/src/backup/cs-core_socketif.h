//
//
//
//
//  server ����  �H������¦�l�ͥX�U�ؤ��P�� �]��(�q�T��w parser)
//
//
//
//
//
#pragma once

#include "cs-def.h"

namespace cyberspace
{
    class CoreSocketIf
    {
    public:

        CoreSocketIf();
        ~CoreSocketIf();

        const ntchar_t*        ObjectName() const { return _T( "CsCoreSocketIf" ); }

        void                   SetSessionUid( _IN cs_session_uid_t cid ) = 0;
        cs_session_uid_t       GetSessionUid() = 0;

        int                    RecvPacket( _OUT CsHttpEncType& enctype, _OUT _mstring& actstr, _OUT const ubyte_t** bp, _OUT cs_size_t& comlen ) = 0;
        int                    SendPacket( _IN const _binbuf& response_str ) = 0;
    }
}