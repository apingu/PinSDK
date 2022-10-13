/* PC1 對稱式加解密函式
* 原始作者: PC1 Cipher Algorithm (Pukall Cipher 1) By Alexander PUKALL 1991
* Wood Woo 整理函式 (Feb 1, 2005)
* Version 1.0
*/
#include <pf-def.h>

#define _MAX_CRYPTKEY_SIZE 16



namespace cryptology
{
    /* ==============================================================================
    * 填入一個長度為_MAX_CRYPTKEY_SIZE的KEY值
    
    
    *  ==============================================================================
    */
    int CreateKey( _INOUT char* key );
    
    /* ==============================================================================
    * binary -> binary 加密
    * @param key : Key(16bytes char)
    * @param msg : 訊息(binary)
    * @param mlen : 訊息長度
    * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
    * @return : 輸出長度
    * ==============================================================================
    */
    bool EncCode( _IN const char* key, _INOUT char* encbuf, _IN size32_t enclen );
    
    
    
    /* ==============================================================================
    * binary -> binary 解密
    * @param key : Key(16bytes char)
    * @param msg : 加密過的訊息(binary)
    * @param mlen : 訊息長度
    * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
    * @return : 輸出長度
    * ==============================================================================
    */
    bool DecCode( _IN const char* key, _INOUT char* decbuf, _IN size32_t declen );     
};
