#ifndef _DDEBUG_H_
#define _DDEBUG_H_

/*
	ddebug: Leveled debugging system which may print debugging message which is
	at desired level. For example, on normal server operation, we may only need critical
	error messages; But it is better to have much verbosed message for debugging.
	Written by Wood Woo, 2004.
 */

#ifdef __cplusplus
#define _C_TYPE	"C"
#else
#define _C_TYPE
#endif

extern _C_TYPE void _dbg_printf(unsigned flg,const char *fmt,...);
extern _C_TYPE void _dbg_init(unsigned flg,const char *logfile);
extern _C_TYPE void _dbg_end();

#ifndef NDEBUG
#define dbg_init(f,l)	_dbg_init(f,l)
#define dbg_printf		_dbg_printf
#define dbg_end()		_dbg_end()
// For Server assert, which won't stop program but will output info
#define dbgAssert2(expr,mesg) \
	{	if(!(expr))	\
		_dbg_printf(0x7fffffff,"Assert failed at %s:%d\n%s\n%s\n",	\
			__FILE__,__LINE__,#expr,mesg);	\
	}
#define dbgAssert(expr)	\
	{	if(!(expr))	\
		_dbg_printf(0x7fffffff,"Assert failed at %s:%d\n%s\n",	\
			__FILE__,__LINE__,#expr);	\
	}
#else	// NDEBUG
#define dbg_init(f,l)
#define dbg_printf		//
#define dbg_end()
#define dbgAssert2(expr,mesg)
#define dbgAssert(expr)
#endif

#endif	// _DDEBUG_H_

