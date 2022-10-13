/* PC1 對稱式加解密函式
 * 原始作者: PC1 Cipher Algorithm ( Pukall Cipher 1 ) By Alexander PUKALL 1991
 * Wood Woo 整理函式 (Feb 1, 2005)
 * Version 1.0
 */
namespace Cryptology
{

/* ==============================================================================
 * binary -> binary 加密
 * @param key : Key(16bytes char)
 * @param msg : 訊息(binary)
 * @param mlen : 訊息長度
 * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
 * @return : 輸出長度
 * ==============================================================================
 */
int PC1Enc(const char *key,const char *msg,int mlen,char *output);
/* ==============================================================================
 * binary -> binary 解密
 * @param key : Key(16bytes char)
 * @param msg : 加密過的訊息(binary)
 * @param mlen : 訊息長度
 * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
 * @return : 輸出長度
 * ==============================================================================
 */
int PC1Dec(const char *key,const char *msg,int mlen,char *output);
/* ==============================================================================
 * Text -> Text 加密
 * @param key : Key(16bytes char)
 * @param msg : 文字訊息(Text)
 * @param output : 輸出 buffer (至少等於 msg 長度的兩倍+1)(Text 格式)
 * @param begin : 編碼後編成的文字碼開始值(共16種), 如 A-P(0x61)
 * @return : 輸出長度
 * ==============================================================================
 */
int PC1EncT(const char *key,const char *msg,char *output,char begin=0x61);
/* ==============================================================================
 * Text -> Text 解密
 * @param key : Key(16bytes char)
 * @param msg : 加密過的文字訊息(Text)
 * @param output : 輸出 buffer (至少等於 msg 長度的一半+1)(Text 格式)
 * @param begin : 編碼後編成的文字碼開始值(共16種), 如 A-P(0x61)(要和當初加密時一致)
 * @return : 輸出長度
 * ==============================================================================
 */
int PC1DecT(const char *key,const char *msg,char *output,char begin=0x61);
/* ==============================================================================
 * 密碼編碼 --> 單向不可逆編碼
 * @param passwd : 原始密碼(最多使用 30 bytes)
 * @param compose30 : 至少 30 byte 的填充文字
 * @param output : 輸出 buffer (需要 32 bytes)
 * @return : 輸出長度
 * ==============================================================================
 */
int PasswdEnc32(const char *passwd,const char *compose30,char *output);

};
