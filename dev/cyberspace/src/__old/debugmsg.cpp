#include<stdarg.h>
#include<stdio.h>
#include "debug.h"

static char fmtbuf[1024];

#define VARTOSTR(fmt)	\
	va_list va;			\
	va_start(va,fmt);	\
	vsprintf(fmtbuf,fmt,va);	\
	va_end(va)

static unsigned g_DbgFlags;
static FILE *g_DbgFile=NULL;

ConsoleOutput ConsoleOutput::m_Instance;


void DBGSetLv(int flgs,unsigned char level)	// 設定顯示的子系統以及等級
{
	g_DbgFlags=(flgs | level);
}

void DBGSetLogFile(FILE *fptr)
{
	g_DbgFile=fptr;
}

bool _test_flags(unsigned flg)
{
	int f1,f2;
	static const int fmsk=0xffffff00,flev=0xff;

	f1=(g_DbgFlags & flev);	// test against level
	f2=(flg & flev);
	if(f1<f2)
		return false;
	f1=(g_DbgFlags & fmsk);	// test against subsystem
	f2=(flg & fmsk);
	if((f1 & f2)==0)
		return false;
	return true;
}

// 基本方法
void InfoDebugOutput::GetLocalTimeString(char *outbuf)	// 目前時間->字串的方法.
{
	struct tm *ct;
	int year;
	time_t t;

	time(&t);
	ct=localtime(&t);
	year = (ct->tm_year+1900)%100;
	sprintf(outbuf,"%02d-%02d-%02d %02d:%02d:%02d:",
		year,ct->tm_mon+1,ct->tm_mday,ct->tm_hour,ct->tm_min,ct->tm_sec);
}

void ConsoleOutput::Write(const char *buf)
{
	char buff[64];
	if(m_OutputTime){
		GetLocalTimeString(buff);
		printf(buff);
	}
	printf(buf);
}


bool TextFileOutput::Open(const char *filename)
{
	m_FP=fopen(filename,"wt");
	return m_FP!=NULL;
}

void TextFileOutput::Close()
{
	fclose(m_FP);
}

void TextFileOutput::Flush()	// Special method
{
	if(m_FP)	fflush(m_FP);
}

void TextFileOutput::Write(const char *buf)
{
	char buff[64];

	if(m_FP!=NULL){
		if(m_OutputTime){
			GetLocalTimeString(buff);
			fputs(buff,m_FP);
		}
		fputs(buf,m_FP);	fputs("\n",m_FP);
	}
}

#ifdef WIN32
#include<windows.h>

void DBGPrint(const char *fmt,...)
{
	VARTOSTR(fmt);
	// 兩邊一起輸出就沒問題吧!!
	if(g_DbgFile)	fprintf(g_DbgFile,fmtbuf);
	else			printf(fmtbuf);
	// This must be later!
	OutputDebugString(fmtbuf);
}

void DBGPrintLv(unsigned flags,const char *fmt,...)	// 有設定子系統以及等級的
{
	if(!_test_flags(flags))	return;
	VARTOSTR(fmt);
	if(g_DbgFile)	fprintf(g_DbgFile,fmtbuf);
	else			printf(fmtbuf);
	// This must be later!
	OutputDebugString(fmtbuf);
}

WindowsOutput WindowsOutput::m_Instance;

void WindowsOutput::Write(const char *buf)
{
	char buff[64];
	if(m_OutputTime){
		GetLocalTimeString(buff);
		OutputDebugString(buff);
	}
	OutputDebugString(buf);
}

#else	// !WIN32

void DBGPrint(const char *fmt,...)
{
	VARTOSTR(fmt);
	if(g_DbgFile)	fprintf(g_DbgFile,fmtbuf);
	else			printf(fmtbuf);
}

void DBGPrintLv(unsigned flags,const char *fmt,...)	// 有設定子系統以及等級的
{
	if(!_test_flags(flags))	return;
	VARTOSTR(fmt);
	if(g_DbgFile)	fprintf(g_DbgFile,fmtbuf);
	else			printf(fmtbuf);
}

#endif
