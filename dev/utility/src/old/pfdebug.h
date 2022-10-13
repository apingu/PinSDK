//===================================================================================
//! @file  PfDebug.h
//! @brief デバッグサポート
//!
//!	@author 	T.Ishida
//!	
//===================================================================================
#ifndef _MT_DEVELOP_H_
#define _MT_DEVELOP_H_

//===================================================================================
//Include
//===================================================================================
//#include "MtCore.h"
//#include "MtTraceLog.h"
#include "basedef.h"

//===================================================================================
//! 
//! デバッグサポート
//!
//! デバッグをサポートするための機能を提供します。
//! 
//! ・アサート
//!	・アボート
//! ・警告出力
//! ・トレース出力
//! ・エラー出力
//! 
//===================================================================================
class PfDebug
{
public:
#if !defined(SPU)
	//! 唯一のインスタンスを取得
	static PfDebug& getInstance(){return mInstance;}
#endif

	//! カスタムのトレース出力関数を設定
	void setTraceHandler(void (*tout)(const char* buf,void* param),void* param = NULL);

	//! カスタムの警告出力関数を設定
	void setWarningHandler(void (*tout)(const char* buf,void* param),void* param = NULL);

	//! カスタムのエラー出力関数を設定
	void setErrorHandler(void (*tout)(const char* buf,void* param),void* param = NULL);

	//! カスタムの情報出力関数を設定
	void setInfoHandler(void (*tout)(const char* buf,void* param),void* param = NULL);

	//! カスタムのアボート関数を設定
	void setAbortHandler(void (*tout)(const char* buf,void* param),void* param = NULL);

#if defined(_DEBUG) || MT_DEVELOP
	//! トレース出力
	void trace(const char*,...);
	void traceDirect(const char*);

	//! 警告出力
	void warning(const char*,...);
#endif

#if !(MT_MASTER)
	//! エラー出力
	void error(const char*,...);

	//! 情報出力
	void info(const char*,...);

	//! アサート出力
	void assertMessage(bool condition,const char* message);

	//! アサート
	bool doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...);

	//! アサートコールバックの型
	typedef bool (* PF_ASSERT_CB)(const char* szExpression, const char* szVaBuffer, const char* szFile, uint32_t nLine, void* pParam);

	//! アサートコールバックを設定
	void setAssertCallback(PF_ASSERT_CB pFunc, void* pParam){ mAssertCallback = pFunc; mpAssertParam = pParam; }

	//! メモリダンプをトレースログに出力
	//! メモリアドレス, サイズ
	static void dumpMemory(const void* padr, size_t size);

 #if !defined(SPU)
	//! メモリダンプをルートフォルダのファイルに出力
	//! メモリアドレス, サイズ, ダンプファイルに含める名前(****の部分)
	//! mem_dump_****_年月日_時_分.txt というファイルが作成されます
	static bool dumpMemoryFromFile(const void* padr, size_t size, const char* name);
	
	//! メモリダンプを指定したファイルストリームに出力
	static bool dumpMemoryFromFile(const void* padr, size_t size, class MtStream* pout);
 #endif
#endif

	//! 強制中断
	void abort(const char*,...);

	//! デフォルトのトレース出力関数
	static void defaultTraceHandler		(const char* buf,void* param);
	static void defaultWarningHandler	(const char* buf,void* param);
	static void defaultErrorHandler		(const char* buf,void* param);
	static void defaultAbortHandler		(const char* buf,void* param);
	static void defaultInfoHandler		(const char* buf,void* param);

#if defined(_PS3)
	//! assert daialog表示(TRC対策としてmasterでも使用可能に)
	void assertDialog(const char*);

	//! GPI DIPの状態の取得
	bool get_dip(uint32_t dip_no);

	//! GPO LEDの状態の取得
	bool get_led(uint32_t led_no);

	//! GPO LEDの状態の設定
	void set_led(uint32_t led_no, bool status);

	//! GPO LEDの状態の設定(２進数表現)
	void set_led(uint32_t num);
#endif

#if defined(_PS4)
	//! GPO LEDの状態の設定(２進数表現)
	void set_led(uint32_t num);
#endif

#if !(MT_MASTER)
	//MtTraceLog* getTraceLog(void) {
	//	return &mTraceLog;
	//}
#endif

private:
	PfDebug();
	~PfDebug();

	void (*mTraceHandler)(const char* buf,void* param);
	void (*mErrorHandler)(const char* buf,void* param);
	void (*mWarningHandler)(const char* buf,void* param);
	void (*mAbortHandler)(const char* buf,void* param);
	void (*mInfoHandler)(const char* buf,void* param);

	void* mTraceParam;
	void* mErrorParam;
	void* mWarningParam;
	void* mAbortParam;
	void* mInfoParam;

#if !(MT_MASTER)
	PF_ASSERT_CB mAssertCallback;
	void* mpAssertParam;
#endif

#if !(MT_MASTER)
	//MtTraceLog mTraceLog;
#endif

#if !defined(SPU)
	static PfDebug mInstance;
#endif
};

#if !defined(SPU)
//================================================================
//! アボート(エラーメッセージを表示し、アプリケーションの実行を強制中断)
//================================================================
#define MT_ABORT(...) PfDebug::getInstance().abort(__VA_ARGS__)

//================================================================
//! トレース出力(フォーマット指定あり)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_TRACE(...) PfDebug::getInstance().trace(__VA_ARGS__)
#else
#define MT_TRACE(...)
#endif

//================================================================
//! トレース出力(フォーマット指定なし)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_TRACE_D(a) PfDebug::getInstance().traceDirect(a)
#else
#define MT_TRACE_D(a)
#endif

//================================================================
//! 警告出力(フォーマット指定あり)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_WARNING(...) PfDebug::getInstance().warning(__VA_ARGS__)
#else
#define MT_WARNING(...)
#endif

//================================================================
//! エラー出力(フォーマット指定あり)
//================================================================
#if !(MT_MASTER)
#define MT_ERROR(...) PfDebug::getInstance().error(__VA_ARGS__)
#else
#define MT_ERROR(...)
#endif

//================================================================
//! 情報出力(フォーマット指定あり)
//================================================================
#if !(MT_MASTER)
#define MT_INFO(...) PfDebug::getInstance().info(__VA_ARGS__)
#else
#define MT_INFO(...)
#endif

//================================================================
//! 情報出力(フォーマット指定あり,ログ保存なし,プラットフォーム固有出力)
//================================================================
#if (MT_MASTER==0)
 #if defined(_CAFE)
  #define MT_REPORT	OSReport
 #else
  #define MT_REPORT(...)	
 #endif
#else
#define MT_REPORT(...)	
#endif


//================================================================
//! アサートマクロ
//================================================================
#if !MT_MASTER
 #if defined(_PS3)
	#if defined(__GCC__)
		#define MT_DEBUG_BREAK() asm __volatile__("tw 31,1,1")
	#elif defined(__SNC__)
		#define MT_DEBUG_BREAK() __builtin_snpause()
	#endif
 #elif defined(_LITE) && defined(_CTR)
	#define MT_DEBUG_BREAK() *reinterpret_cast< int* >(0) = 0xdec0ffee
 #elif defined(_WINDOWS)
	#if defined (_M_IX86)
		#define MT_DEBUG_BREAK() __asm { int 3 }
	#elif defined (_M_IA64)
		void __break(int);
		#pragma intrinsic (__break)

		#define MT_DEBUG_BREAK() __break(0x80016)
	#else
		#define MT_DEBUG_BREAK() DebugBreak()
	#endif
 #elif defined(_XBOX) || defined(_XBO)
	#define MT_DEBUG_BREAK() __debugbreak()
 #elif defined(_VITA)
	//TODO:とりあえず　残しておく
	void vitaAssert(const char* exp, const char* text, const char* file, s32 line);

	#if defined(__SNC__)
		#define MT_DEBUG_BREAK() __breakpoint( 0 );
	#else
		#define MT_DEBUG_BREAK() __asm volatile( "bkpt 0x0000" );
	#endif
 #elif defined(_CAFE)
	#define MT_DEBUG_BREAK() ASSERT(NULL)
 #elif defined(_PS4)
	#define MT_DEBUG_BREAK() SCE_BREAK()
 #else
	#error implement
 #endif
#else
	#define MT_DEBUG_BREAK() /* nop */
#endif

#ifndef MT_ASSERT

 #if defined(_PS4)
	// MT_ASSERTの第一引数にNULLを渡す人が多いため
	#define MT_ASSERT_CHECK(a)  (static_cast<bool>(a))
 #else
	#define MT_ASSERT_CHECK(a)  (a)
 #endif

	#ifdef _DEBUG
		#define MT_ASSERT(a,...)																									\
		{																															\
			bool isAssert = false;																									\
			(void)( (!!MT_ASSERT_CHECK(a)) || (!PfDebug::getInstance().doAssert(__FILE__,__LINE__,#a,__VA_ARGS__)) || ((isAssert = true),0) );		\
																																	\
			if( isAssert ){																											\
				MT_DEBUG_BREAK();																									\
			}																														\
		}
	#else //#ifdef _DEBUG
		#define MT_ASSERT(a,...)	\
		{							\
			(void)( 0 );			\
		}
	#endif //#ifdef _DEBUG
#endif //#ifndef MT_ASSERT

#else// SPU
#include "spuDebug.h"




//================================================================
//! トレース出力(フォーマット指定あり)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_TRACE(...) SPU_TRACE(__VA_ARGS__)
#else
#define MT_TRACE(...)
#endif

//================================================================
//! トレース出力(フォーマット指定なし)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_TRACE_D(a) MT_TRACE(a)
#else
#define MT_TRACE_D(a)
#endif

//================================================================
//! 警告出力(フォーマット指定あり)
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
#define MT_WARNING    MT_TRACE
#else
#define MT_WARNING(...)
#endif

//================================================================
//! エラー出力(フォーマット指定あり)
//================================================================
#if !(MT_MASTER)
#define MT_ERROR(...) SPU_TRACE(__VA_ARGS__)
#else
#define MT_ERROR(...)
#endif

//================================================================
//! アサートマクロ
//================================================================
#if  defined(_DEBUG)
	#define MT_ASSERT(a, ...)		SPU_ASSERT(a, __VA_ARGS__)
#else
	#define MT_ASSERT(a, ...)		/**/
#endif /* _DEBUG */

//================================================================
//! SPU Eixt
//================================================================
#if defined(_DEBUG) || MT_DEVELOP
	#define SPU_EXIT	          spuExit
#else
	#define SPU_EXIT(_exit_code_)	 /**/
#endif 
#endif

//================================================================
//! コンパイル時アサートマクロ
//================================================================
#if !(MT_MASTER)

namespace MT
{
  namespace compiletime_assert
  {
    template <int x> struct static_assert_test{};
    template <bool x> struct STATIC_ASSERTION_FAILURE;
    template <> struct STATIC_ASSERTION_FAILURE<true>{ enum { value = 1 }; };
  }
}

#define MT_STATIC_ASSERT_MACRO_CONCAT(t0m0, ecda)             MT_STATIC_ASSERT_MACRO_CONCAT_TRAMPOLINE(t0m0, ecda)
#define MT_STATIC_ASSERT_MACRO_CONCAT_TRAMPOLINE(t0m0, ecda)  MT_STATIC_ASSERT_MACRO_CONCAT_REAL(t0m0, ecda)
#define MT_STATIC_ASSERT_MACRO_CONCAT_REAL(t0m0, ecda)        t0m0##ecda

#if defined(__SNC__)
#define MT_STATIC_ASSERT(expression)\
  typedef ::MT::compiletime_assert::STATIC_ASSERTION_FAILURE< (bool)((expression)) > MT_STATIC_ASSERT_MACRO_CONCAT(STATIC_ASSERT_at_LINE_,__LINE__);\
  typedef ::MT::compiletime_assert::static_assert_test< sizeof(MT_STATIC_ASSERT_MACRO_CONCAT(STATIC_ASSERT_at_LINE_,__LINE__)) > MT_STATIC_ASSERT_MACRO_CONCAT(mt_static_assert_at_,__LINE__)
#else
#define MT_STATIC_ASSERT(expression)\
  struct MT_STATIC_ASSERT_MACRO_CONCAT(mt_sa_tester,__LINE__)\
  {\
    ::MT::compiletime_assert::STATIC_ASSERTION_FAILURE< (bool)((expression)) > MT_STATIC_ASSERT_MACRO_CONCAT(STATIC_ASSERT_at_LINE_, __LINE__);\
  };\
  typedef ::MT::compiletime_assert::static_assert_test< sizeof(MT_STATIC_ASSERT_MACRO_CONCAT(mt_sa_tester,__LINE__)) > MT_STATIC_ASSERT_MACRO_CONCAT(mt_static_assert_at_,__LINE__)
#endif

#else
#define MT_STATIC_ASSERT(exp)
#endif

//================================================================
//! 
//================================================================
#if defined(_PS3)
#define	MT_MSOUTPUT_INITIALIZE()
#define	MT_MSOUTPUT_FINALIZE()
#define	MT_HDDOUTPUT_INITIALIZE()
#define	MT_HDDOUTPUT_FINALIZE()
#define	MT_TRACEMS					MT_ERROR
#define	MT_TRACEHDD					MT_ERROR
#endif

#if defined(_PS3) && !(MT_MASTER)
//================================================================
//! GPI DIPの取得
//================================================================
#define MT_GETDIP(dip_no)			(PfDebug::getInstance().get_dip(dip_no))

//================================================================
//! GPO LEDの取得
//================================================================
#define MT_GETLED(led_no)			(PfDebug::getInstance().get_led(led_no))

//================================================================
//! GPO LEDの設定
//================================================================
#define MT_SETLED(led_no, status)	(PfDebug::getInstance().set_led(led_no, status))
#define MT_SETLEDON(led_no)			(PfDebug::getInstance().set_led(led_no, true))
#define MT_SETLEDOFF(led_no)		(PfDebug::getInstance().set_led(led_no, false))
#define MT_SETLEDBIN(num)			(PfDebug::getInstance().set_led(num))
#elif defined(_PS4) && !(MT_MASTER)
#define MT_GETDIP(dip_no)			(false)
#define MT_GETLED(led_no)			(false)
#define MT_SETLED(led_no, status)
#define MT_SETLEDON(led_no)
#define MT_SETLEDOFF(led_no)
#define MT_SETLEDBIN(num)			(PfDebug::getInstance().set_led(num))
#else
#define MT_GETDIP(dip_no)			(false)
#define MT_GETLED(led_no)			(false)
#define MT_SETLED(led_no, status)
#define MT_SETLEDON(led_no)
#define MT_SETLEDOFF(led_no)
#define MT_SETLEDBIN(num)
#endif

// LED・DIP番号定義
#define MT_LED_FREE0				0
#define MT_LED_FREE1				1
#define MT_LED_RESERVED				2	// 予約
#define MT_LED_CLEAR_CACHE_DISABLE	3	// HDDキャッシュをクリアしない
#define MT_LEDNUM					4

#define MT_DIP_FREE0				0
#define MT_DIP_FREE1				1
#define MT_DIP_RESERVED				2	// 予約
#define MT_DIP_CLEAR_CACHE_DISABLE	3	// HDDキャッシュをクリアしない
#define MT_DIPNUM					4

#ifdef	_PS3

#if !(MT_MASTER)
//	PS3用アサート(MT_ASSERTでローカル変数などが参照できなくて困る場合に使用)
//	XFramework以上の階層でのみ使用可（例えばZlib,MTなどでは使用不可）
extern	signed char	*assertFile;
extern	signed int	assertLine;
extern	char	assertInfo[];

#define	PS3_ASSERT(exp, mes)	if(!(exp)){			\
			assertFile = (signed char *)__FILE__;	\
			assertLine = __LINE__;					\
			PfDebug::getInstance().assertDialog(mes);	\
		}
#else
#define	PS3_ASSERT(exp, mes)
#endif

#ifdef _DEBUG
//	メモリ残量表示
#define	DISP_USER_MEMORY(str)									\
{																\
	sys_memory_info_t	mem_info;								\
	sys_memory_get_user_memory_size(&mem_info);					\
	std::printf("%s available 0x%08x\n" ,str, mem_info.available_user_memory);\
}

//	system確保のheap情報
#define	DISP_HEAP_INFO()										\
{																\
	void*	ptr;												\
	ptr	= std::malloc(0x1000);									\
	if(ptr){													\
		sys_page_attr_t		attr;								\
		if(sys_memory_get_page_attribute((u64)ptr,&attr)==CELL_OK){		\
			std::printf("heap size0x%08x,attr0x%08x\n",attr.page_size,attr.attribute);\
		}														\
		std::free(ptr);											\
	}															\
}

//	ローカル関数内処理時間msec計測用（計測したい箇所の時間をTICK_START,TICK_ENDで囲みます。局所的、一時的なテストに使用します。設置位置に制限が発生するので、ネストには対応していません。
//	それ以外はMT_PROFILEの使用を推奨します。)
#define	PS3_TICK_START()				\
	sys_time_sec_t	X_sec_base;		\
	sys_time_nsec_t	X_nsec_base;	\
	sys_time_get_current_time(&X_sec_base, &X_nsec_base);

#define	PS3_TICK_END()					\
	sys_time_sec_t	X_sec;			\
	sys_time_nsec_t	X_nsec;			\
	sys_time_get_current_time(&X_sec, &X_nsec);	\
	u64 X_time0 = X_sec - X_sec_base;		\
	u64 X_time1 = (X_nsec - X_nsec_base + 1000000000 * X_time0) / 1000;	\
	std::printf("#process finished in %lld usec\n", X_time1);

#else	//	if _DEBUG
 #define	DISP_USER_MEMORY(str)
 #define	DISP_HEAP_INFO()
 #define	PS3_TICK_START()
 #define	PS3_TICK_END()
#endif	//	if _DEBUG

#endif	// defined(_PS3)

// 移動しました。近い将来、この行は削除されます。
//#include "MtFunctionReplacement.h"

#endif	//	ifndef _MT_DEBUG_
