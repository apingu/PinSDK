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

#include <pf-def.h>

namespace crc64
{
//public:
//    CCrcStatic();
//    virtual ~CCrcStatic();

    qword_t Encode( qword_t crc, const char* buffer, unsigned int buflen );
    qword_t Encode( const char* buffer, unsigned int buflen );



//protected:

    //static inline void CalcCrc32( const unsigned char byte, unsigned long& dwCrc32 );
    
//    static DWORD s_arrdwCrcTable[256];
};