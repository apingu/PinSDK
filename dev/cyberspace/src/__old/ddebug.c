#include"ddebug.h"
#include<stdio.h>
#include<assert.h>
#include<stdarg.h>
#include<string.h>
#include<time.h>

#define BUFSIZE	512

/*
	ddebug: Leveled debugging system which may print debugging message which is
	at desired level. For example, on normal server operation, we may only need critical
	error messages; But it is better to have much verbosed message for debugging.
	Written by Wood Woo, 2004.
 */

static char fmtbuf[BUFSIZE];
/* debug 旗標分為兩部分. 最低的 8 bit 是(0-255)的等級數值. 只有(g_dbgflg)中的>=dbg_printf要求的
   方才印出(也就是 user 要求等級). 請用 dbg_init 設定預設等級. 高 24 bit 則是種類號碼. dbg_printf
   中可設定一個 bit mask 當作本模組用, 如此 run-time 時可以指定要印哪些模組的訊息.
 */
static unsigned g_dbgflg;
static FILE *fperr;			/* 輸出的 File pointer */

void dbg_printtime()
{
	_strdate(fmtbuf);
	strcat(fmtbuf," ");
	_strtime(fmtbuf+strlen(fmtbuf));
	strcat(fmtbuf,":");
	fputs(fmtbuf,fperr);
}

void dbg_printf(unsigned flg,const char *fmt,...)
{
	va_list va;
	int f1,f2;
	static const int fmsk=0xffffff00,flev=0xff;

	f1=(g_dbgflg & flev);
	f2=(flg & flev);
	if(f1<f2)
		return;
	f1=(g_dbgflg & fmsk);
	f2=(flg & fmsk);
	if((f1 & f2)==0)
		return;
	va_start(va,fmt);
	vsprintf(fmtbuf,fmt,va);
	va_end(va);
	fputs(fmtbuf,fperr);
	fflush(fperr);
}

void dbg_init(unsigned flg,const char *logfile)
{
	g_dbgflg=flg;
	if(logfile){
		fperr=freopen(logfile,"wt",stderr);
		dbg_printtime();
		fputs("Log start\n",fperr);
	} else
		fperr=stderr;
}

void dbg_end()
{
	fclose(fperr);
}
