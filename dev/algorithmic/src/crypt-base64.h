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

#include <string>

namespace base64
{
    std::string Encode( unsigned char const*, unsigned int len );
    std::string Decode( std::string const& s );
}


//const std::string s = "test";
//std::string encoded = base64_encode( reinterpret_cast< const unsigned char* >( s.c_str() ), s.length() );
//std::string decoded = base64_decode( encoded );