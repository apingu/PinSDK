/*
 * A symmatric encryption algorithm called PC1
 * By Alexander PUKALL 1991 (http://membres.lycos.fr/pc1/)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <memio.inl>
#include "cryptology.h"


namespace cryptology
{
    int KeyCodeMask = _MAX_CRYPTKEY_SIZE - 1;
    int CreateKey( char* key )
    {
        //IOMEMSET(key, 0, _MAX_CRYPTKEY_SIZE);
        //return _MAX_CRYPTKEY_SIZE;
        // 因為 window底下的rand是到 32767
        // 所以 用short做 否則會有一些byte沒有加密到
        short* ptr = ( short* )key;
        int cc     = ( _MAX_CRYPTKEY_SIZE / sizeof( short ) );
        
        for ( int i = 0; i < cc; i++ )
        {
            ( *ptr ) = rand();
            ptr++;
        }
        
        return _MAX_CRYPTKEY_SIZE;
    }
    
    
    /* ==============================================================================
     * binary -> binary 加密
     * @param key : Key
     * @param msg : 訊息(binary)
     * @param mlen : 訊息長度
     * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
     * @return : 輸出長度
     * ==============================================================================
     */
    bool EncCode( const char* key, char* encbuf, size32_t enclen )
    {
        for ( unsigned i = 0; i < enclen; i++ )
        {
            int j   = ( i + 7 )  &KeyCodeMask; //0xF; // 長度為16所以是F(15)
            encbuf[i] = ( encbuf[i] ^ key[j] );
        }
        
        return true;
    }
    
    /* ==============================================================================
     * binary -> binary 解密
     * @param key : Key
     * @param msg : 加密過的訊息(binary)
     * @param mlen : 訊息長度
     * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
     * @return : 輸出長度
     * ==============================================================================
     */
    bool DecCode( const char* key, char* encbuf, size32_t enclen )
    {
        for ( unsigned i = 0; i < enclen; i++ )
        {
            int j   = ( i + 7 )  &KeyCodeMask; //0xF; // 長度為16所以是F(15)
            encbuf[i] = ( encbuf[i] ^ key[j] );
        }
        
        return true;
    }    
};