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
#pragma once

#include "cs-def.h"


namespace cyberspace
{
    namespace Paper
    {
        class Record
        {
        public:
            _cs_string   key_;
            _csbinbuf    value_;
        };
    }
}

typedef cyberspace::Paper::Record  CsRecord;
