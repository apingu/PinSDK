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

namespace crc32
{
//public:
//    CCrcStatic();
//    virtual ~CCrcStatic();

    unsigned int Encode( unsigned int crc, const char* buffer, unsigned int buflen );
    unsigned int Encode( const char* buffer, unsigned int buflen );    

//protected:

    //static inline void CalcCrc32( const unsigned char byte, unsigned long& dwCrc32 );
    
//    static DWORD s_arrdwCrcTable[256];
};