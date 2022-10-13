/**
* base32 (de)coder implementation as specified by RFC4648.
*
* Copyright (c) 2010 Adrien Kunysz
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
**/

#pragma once


#include <pf-def.h>   // size_t
#include <string>


namespace base32
{
        
    /**
    * Encode the data pointed to by plain into base32 and store the
    * result at the address pointed to by coded. The "coded" argument
    * must point to a location that has enough available space
    * to store the whole coded string. The resulting string will only
    * contain characters from the [A-Z2-7=] set. The "len" arguments
    * define how many bytes will be read from the "plain" buffer.
    **/
    std::string Encode( const unsigned char* plain, size_t len );
    
    /**
    * Decode the null terminated string pointed to by coded and write
    * the decoded data into the location pointed to by plain. The
    * "plain" argument must point to a location that has enough available
    * space to store the whole decoded string.
    * Returns the length of the decoded string. This may be less than
    * expected due to padding. If an invalid base32 character is found
    * in the coded string, decoding will stop at that point.
    **/
    std::string Decode( const std::string& s );
}






