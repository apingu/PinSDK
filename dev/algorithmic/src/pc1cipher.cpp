/*
 * A symmatric encryption algorithm called PC1
 * By Alexander PUKALL 1991 (http://membres.lycos.fr/pc1/)
 * Arranger: Wood Woo, Feb 1, 2005. Version 1.0
 */

namespace Cryptology
{

class PC1Coder
{
private:
	unsigned short ax, bx, cx, dx, si, tmp, x1a2, x1a0[8], res, inter;
	void code(int i)
	{
		dx = x1a2 + i;
		ax = x1a0[i];
		cx = 0x015a;
		bx = 0x4e35;

		tmp = ax;
		ax = si;
		si = tmp;

		tmp = ax;
		ax = dx;
		dx = tmp;

		if (ax != 0)
		{
			ax = ax * bx;
		}

		tmp = ax;
		ax = cx;
		cx = tmp;

		if (ax != 0)
		{
			ax = ax * si;
			cx = ax + cx;
		}

		tmp = ax;
		ax = si;
		si = tmp;
		ax = ax * bx;
		dx = cx + dx;

		ax = ax + 1;

		x1a2 = dx;
		x1a0[i] = ax;

		res = ax ^ dx;
	}
public:
	PC1Coder()
	{
		int i;
		ax=bx=cx=dx=si=tmp=x1a2=res=inter=0;
		for(i=0;i<8;++i)
			x1a0[i]=0;
	}
	int Update(unsigned char *k16)
	{
		x1a0[0] = (k16[0] * 256) + k16[1];
		code(0);
		inter = res;

		x1a0[1] = x1a0[0] ^ ((k16[2] * 256) + k16[3]);
		code(1);
		inter = inter ^ res;

		x1a0[2] = x1a0[1] ^ ((k16[4] * 256) + k16[5]);
		code(2);
		inter = inter ^ res;

		x1a0[3] = x1a0[2] ^ ((k16[6] * 256) + k16[7]);
		code(3);
		inter = inter ^ res;

		x1a0[4] = x1a0[3] ^ ((k16[8] * 256) + k16[9]);
		code(4);
		inter = inter ^ res;

		x1a0[5] = x1a0[4] ^ ((k16[10] * 256) + k16[11]);
		code(5);
		inter = inter ^ res;

		x1a0[6] = x1a0[5] ^ ((k16[12] * 256) + k16[13]);
		code(6);
		inter = inter ^ res;

		x1a0[7] = x1a0[6] ^ ((k16[14] * 256) + k16[15]);
		code(7);
		inter = inter ^ res;
		return inter;
	}
};


#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "PC1Cipher.h"

/* ==============================================================================
 * binary -> binary 加密
 * @param key : Key
 * @param msg : 訊息(binary)
 * @param mlen : 訊息長度
 * @param output : 輸出 buffer (至少等於 mlen 長度)(binary格式)
 * @return : 輸出長度
 * ==============================================================================
 */
 int PC1Enc(const char *key,const unsigned char *msg,int mlen,unsigned char *output)
{
	int i,c,cfc,cfd,ic,v;
	unsigned char key16[16];
	PC1Coder pc1c;

	memset(key16,0,16);
	strncpy((char *)key16,key,16);
	for(i=0;i<mlen;++i)
	{
		v=pc1c.Update(key16);
		cfc = v >> 8;
		cfd = v & 255;
		ic = msg[i];
		for (c=0; c <= 15; c++)
		{
			key16[c] = key16[c] ^ ic;
		}
		// 編解碼只是這邊順序差別!!
		output[i] = msg[i] ^ (cfc ^ cfd);
	}
	return i;
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
 int PC1Dec(const char *key,const unsigned char *msg,int mlen,unsigned char *output)
{
	int i,c,cfc,cfd,ic,v;
	unsigned char key16[16];
	PC1Coder pc1c;

	memset(key16,0,16);
	strncpy((char *)key16,key,16);
	for(i=0;i<mlen;++i)
	{
		v=pc1c.Update(key16);
		cfc = v >> 8;
		cfd = v & 255;
		output[i] = ic = msg[i] ^ (cfc ^ cfd);
		for (c=0; c <= 15; c++)
		{
			key16[c] = key16[c] ^ ic;
		}
	}
	return i;
}

/* ==============================================================================
 * Text -> Text 加密
 * @param key : Key
 * @param msg : 文字訊息(Text)
 * @param output : 輸出 buffer (至少等於 msg 長度的兩倍+1)(Text 格式)
 * @param begin : 編碼後編成的文字碼開始值(共16種), 如 A-P(0x61)
 * @return : 輸出長度
 * ==============================================================================
 */
 int PC1EncT(const char *key,const char *msg,char *output,char begin)
{
	int n,i,j;
	char *buf;
	unsigned char *ubuf;
	n=strlen(msg);
	buf=(char *)alloca(n);
	if(buf)
	{
		ubuf=(unsigned char *)buf;
		n=PC1Enc(key,(const unsigned char *)msg,n,(unsigned char *)buf);
		for(j=i=0;i<n;i++)
		{
			output[j++]=(ubuf[i] & 0x0f) + begin;
			output[j++]=(ubuf[i] >> 4) + begin;
		}
		output[j]='\0';
		return j;
	} else
		return 0;
}

/* ==============================================================================
 * Text -> Text 解密
 * @param key : Key
 * @param msg : 加密過的文字訊息(Text)
 * @param output : 輸出 buffer (至少等於 msg 長度的一半+1)(Text 格式)
 * @param begin : 編碼後編成的文字碼開始值(共16種), 如 A-P(0x61)(要和當初加密時一致)
 * @return : 輸出長度
 * ==============================================================================
 */
 int PC1DecT(const char *key,const char *msg,char *output,char begin)
{
	int n,i,j,cs[2];
	char *buf;
	n=strlen(msg);
	buf=(char *)alloca((1+n)/2);
	if(buf)
	{
		for(j=i=0;i<n;++j)
		{
			cs[0]=(msg[i++]-begin);
			cs[1]=(msg[i++]-begin);
			buf[j]=(cs[1]<<4)+cs[0];
		}
		n=PC1Dec(key,(const unsigned char *)buf,j,(unsigned char *)output);
		output[n]='\0';
		return n;
	} else
		return 0;
}


/* ==============================================================================
 * 密碼編碼 --> 單向不可逆編碼
 * @param passwd : 原始密碼(最多使用 30 bytes)
 * @param compose30 : 至少 30 byte 的填充文字
 * @param output : 輸出 buffer (需要 32 bytes)
 * @return : 輸出長度
 * ==============================================================================
 */
 int PasswdEnc32(const char *passwd,const char *compose30,char *output)
{
	char buf[33],pw[33],n;

	n=strlen(passwd);
	if(n>30)	n=30;
	memset(buf,' ',30);
	strncpy(buf,compose30,30);
	strncpy(pw,passwd,n);	pw[n]='\0';
	strncpy(buf,pw,n);
	sprintf(buf+30,"%2d",strlen(pw));
	return PC1EncT(pw,buf,output,0x41);
}

};