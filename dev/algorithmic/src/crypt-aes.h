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

#include <stdint.h>


// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// CBC enables AES128 encryption in CBC-mode of operation and handles 0-padding.
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
//#ifndef CBC
//    #define CBC 1
//#endif
//
//#ifndef ECB
//    #define ECB 1
//#endif

namespace aes128
{
    namespace ECB
    {
        //#if defined(ECB) && ECB
        // 128bit max length = 16
        void Encrypt( uint8_t* input, const uint8_t* key, uint8_t* output );
        void Decrypt( uint8_t* input, const uint8_t* key, uint8_t* output );
        //#endif // #if defined(ECB) && ECB
    }

    namespace CBC
    {
        //#if defined(CBC) && CBC
        void Encrypt_buffer( uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv );
        void Decrypt_buffer( uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv );
        //#endif // #if defined(CBC) && CBC
    }
}