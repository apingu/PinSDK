//===================================================================================
//! @file  PfDebug.cpp
//! @brief デバッグサポート
//!
//!	@author 	T.Ishida
//!	
//===================================================================================

//===================================================================================
//Include
//===================================================================================
#include "PfDebug.h"
//#include "MtMemory.h"
//#include "MtTime.h"

#if !defined(SPU)
//#include "MtDataWriter.h"
#endif

#if defined(_VITA)
#include <assert.h>
#endif


//===================================================================================
//PfDebug
//===================================================================================

//-----------------------------------------------------------------------------
//! 唯一のインスタンス
//-----------------------------------------------------------------------------
#if !defined(SPU)
PfDebug PfDebug::mInstance;
#endif

//-----------------------------------------------------------------------------
//! デフォルトコンストラクタ
//-----------------------------------------------------------------------------
PfDebug::PfDebug(void):	
mTraceHandler(defaultTraceHandler),
mErrorHandler(defaultErrorHandler),
mWarningHandler(defaultWarningHandler),
mAbortHandler(defaultAbortHandler),
mInfoHandler(defaultInfoHandler),
mTraceParam(NULL),
mErrorParam(NULL),
mWarningParam(NULL),
mInfoParam(NULL),
mAbortParam(NULL)
#if !(MT_MASTER)
, mAssertCallback(NULL), mpAssertParam(NULL)
#endif
{
	
}

//-----------------------------------------------------------------------------
//! デストラクタ
//-----------------------------------------------------------------------------
PfDebug::~PfDebug(void)
{
}


#if !(MT_MASTER)
//-----------------------------------------------------------------------------
//! アサート出力
//-----------------------------------------------------------------------------
void PfDebug::assertMessage(bool condition,const char* message)
{
	if (!condition){
		error("[ASSERT] : %s\n",message);
	}
}
#endif

//-----------------------------------------------------------------------------
//! アボート出力関数を設定
//!
//! アボート出力時に使用するハンドラを設定します。
//-----------------------------------------------------------------------------
void PfDebug::setAbortHandler(void (*tout)(const char* buf,void* param),void* param)
{
	if (tout){
		mAbortHandler = tout;
		mAbortParam   = param;
	}else{
		mAbortHandler = defaultAbortHandler;
		mAbortParam   = NULL;
	}
}

//-----------------------------------------------------------------------------
//! トレース出力関数を設定
//!
//! トレース出力時に使用するハンドラを設定します。
//-----------------------------------------------------------------------------
void PfDebug::setTraceHandler(void (*tout)(const char* buf,void* param),void* param)
{
	if (tout){
		mTraceHandler = tout;
		mTraceParam   = param;
	}else{
		mTraceHandler = defaultTraceHandler;
		mTraceParam   = NULL;
	}
}

//-----------------------------------------------------------------------------
//! エラー出力関数を設定
//!
//! エラー出力時に使用するハンドラを設定します。
//-----------------------------------------------------------------------------
void PfDebug::setErrorHandler(void (*tout)(const char* buf,void* param),void* param)
{
	if (tout){
		mErrorHandler = tout;
		mErrorParam   = param;
	}else{
		mErrorHandler = defaultErrorHandler;
		mErrorParam   = NULL;
	}
}

//-----------------------------------------------------------------------------
//! 情報出力関数を設定
//!
//! 情報出力時に使用するハンドラを設定します。
//-----------------------------------------------------------------------------
void PfDebug::setInfoHandler(void (*tout)(const char* buf,void* param),void* param)
{
	if (tout){
		mInfoHandler = tout;
		mInfoParam   = param;
	}else{
		mInfoHandler = defaultInfoHandler;
		mInfoParam   = NULL;
	}
}

//-----------------------------------------------------------------------------
//! 警告出力関数を設定
//!
//! 警告出力時に使用するハンドラを設定します。
//-----------------------------------------------------------------------------
void PfDebug::setWarningHandler(void (*tout)(const char* buf,void* param),void* param)
{
	if (tout){
		mWarningHandler = tout;
		mWarningParam   = param;
	}else{
		mWarningHandler = defaultWarningHandler;
		mWarningParam   = NULL;
	}
}

//-----------------------------------------------------------------------------
//! アボート
//!
//! エラーメッセージを表示し、アプリケーションを強制中断します
//-----------------------------------------------------------------------------
void PfDebug::abort(const char* format,...)
{
	va_list marker;

	MT_CHAR buf[1024];

	va_start( marker, format );
	vsnprintf(buf,sizeof(buf)-1,format,marker);
	va_end( marker );

	if (mAbortHandler)
		mAbortHandler(buf,mAbortParam);

#if defined(_CTR)
	*reinterpret_cast<s32*>(0) = 0xdeadbeef;	//! exit()ではカレントスレッドしか停止しないため、例外アクセスで無理やり止める
#elif defined(_PS3)
	_Exit(1);
#else
	exit(1);
#endif
}

#if defined(_DEBUG) || MT_DEVELOP

//-----------------------------------------------------------------------------
//! トレース出力
//!
//! ダイレクト文字列を出力します。
//-----------------------------------------------------------------------------
void PfDebug::traceDirect(const char* str)
{
	if (mTraceHandler)
		mTraceHandler(str,mTraceParam);
}


//-----------------------------------------------------------------------------
//! トレース出力
//!
//! フォーマットして出力します。
//!
//! @param format ... 出力形式
//-----------------------------------------------------------------------------
void PfDebug::trace(const char* format,...)
{
	if (!mTraceHandler) {
		return;
	}

	va_list marker;

	MT_CHAR buf[1024];

	va_start( marker, format );
	vsnprintf(buf,sizeof(buf)-1,format,marker);
	va_end( marker );

	mTraceHandler(buf,mTraceParam);
}

//-----------------------------------------------------------------------------
//! 警告出力
//!
//! フォーマットして出力します。
//!
//! @param format ... 出力形式
//-----------------------------------------------------------------------------
void PfDebug::warning(const char* format,...)
{
	if (!mWarningHandler) {
		return;
	}

	va_list marker;

	MT_CHAR buf[1024];

	va_start( marker, format );
	vsnprintf(buf,sizeof(buf)-1,format,marker);
	va_end( marker );

	mWarningHandler(buf,mWarningParam);
}
#endif

#if MT_MASTER == 0
//-----------------------------------------------------------------------------
//! エラー出力
//!
//! フォーマットして出力します。
//!
//! @param format ... 出力形式
//-----------------------------------------------------------------------------
void PfDebug::error(const char* format,...)
{
	if (!mErrorHandler) {
		return;
	}

	va_list marker;

	MT_CHAR buf[1024];

	va_start( marker, format );
	vsnprintf(buf,sizeof(buf)-1,format,marker);
	va_end( marker );

	mErrorHandler(buf,mErrorParam);
}

//-----------------------------------------------------------------------------
//! 情報出力
//!
//! フォーマットして出力します。
//!
//! @param format ... 出力形式
//-----------------------------------------------------------------------------
void PfDebug::info(const char* format,...)
{
	if (!mInfoHandler) {
		return;
	}

	va_list marker;

	MT_CHAR buf[1024];

	va_start( marker, format );
	vsnprintf(buf,sizeof(buf)-1,format,marker);
	va_end( marker );

	mInfoHandler(buf,mInfoParam);
}

#endif

//-----------------------------------------------------------------------------
//! デフォルト出力ハンドラ
//!
//! 標準入出力を利用して、文字列を出力します。
//!
//! @param buf ... 文字列
//! @param param ... カスタムパラメータ
//-----------------------------------------------------------------------------
void PfDebug::defaultTraceHandler(const char* buf,void*)
{
#if MT_MASTER == 0 && !defined(SPU)
	mInstance.mTraceLog.traceLog(buf, MtTraceLog::TRACE_NORMAL);
#endif
}
void PfDebug::defaultWarningHandler(const char* buf,void*)
{
#if MT_MASTER == 0 && !defined(SPU)
	mInstance.mTraceLog.traceLog(buf, MtTraceLog::TRACE_WARNING);
#endif
}
void PfDebug::defaultErrorHandler(const char* buf,void*)
{
#if MT_MASTER == 0 && !defined(SPU)
	mInstance.mTraceLog.traceLog(buf, MtTraceLog::TRACE_ERROR);
#endif
}
void PfDebug::defaultAbortHandler(const char* buf,void*)
{
#if MT_MASTER == 0 && !defined(SPU)
	mInstance.mTraceLog.traceLog(buf, MtTraceLog::TRACE_ERROR);
#endif
}
void PfDebug::defaultInfoHandler(const char* buf,void*)
{
#if MT_MASTER == 0 && !defined(SPU)
	mInstance.mTraceLog.traceLog(buf, MtTraceLog::TRACE_INFO);
#endif
}

#if defined(_PS3) && !(MT_MASTER)
//-----------------------------------------------------------------------------
//! GPI DIPの状態の取得
//!
//! @param dip_no ... DIP番号
//-----------------------------------------------------------------------------
bool PfDebug::get_dip(uint32_t dip_no)
{
	MT_ASSERT(dip_no < MT_DIPNUM, "dip_no error.");

	u64 value;
	s32 ret;

	ret = sys_gpio_get(SYS_GPIO_DIP_SWITCH_DEVICE_ID, &value);
	if( ret!=CELL_OK ){
		MT_ASSERT(ret == CELL_OK, "sys_gpio_get() error.");
	}

	value &= SYS_GPIO_DIP_SWITCH_USER_AVAILABLE_BITS;
	value &= (1 << dip_no);

	if(value){
		return true;
	}else{
		return false;
	}
}


//-----------------------------------------------------------------------------
//! GPO LEDの状態の取得
//!
//! @param led_no ... LED番号
//-----------------------------------------------------------------------------
bool PfDebug::get_led(uint32_t led_no)
{
	MT_ASSERT(led_no < MT_LEDNUM, "led_no error.");

	u64 value;
	s32 ret;

	ret = sys_gpio_get(SYS_GPIO_LED_DEVICE_ID, &value);
	if( ret!=CELL_OK ){
		MT_ASSERT(ret == CELL_OK, "sys_gpio_get() error.");
	}

	value &= SYS_GPIO_LED_USER_AVAILABLE_BITS;
	value &= (1 << led_no);

	if(value){
		return true;
	}else{
		return false;
	}
}

//-----------------------------------------------------------------------------
//! GPO LEDの状態の設定
//!
//! @param led_no ... LED番号
//! @param status ... LEDの状態
//-----------------------------------------------------------------------------
void PfDebug::set_led(uint32_t led_no, bool status)
{
	MT_ASSERT(led_no < MT_LEDNUM, "led_no error.");

	u64 mask = (1 << led_no);
	u64 value = (status << led_no);
	s32 ret;

	ret = sys_gpio_set(SYS_GPIO_LED_DEVICE_ID, mask, value);
	if( ret!=CELL_OK ){
		MT_ASSERT(ret == CELL_OK, "sys_gpio_set() error.");
	}
}

//-----------------------------------------------------------------------------
//! GPO LEDの状態の設定(2進数表現)
//!
//! @param num ... 設定する値
//-----------------------------------------------------------------------------
void PfDebug::set_led(uint32_t num)
{
	u64 mask = 0x0f;
	u64 value = num & mask;
	s32 ret;

	ret = sys_gpio_set(SYS_GPIO_LED_DEVICE_ID, mask, value);
	if( ret!=CELL_OK ){
		MT_ASSERT(ret == CELL_OK, "sys_gpio_set() error.");
	}
}

#endif

#if	defined(_PS4) && !(MT_MASTER)
//-----------------------------------------------------------------------------
//! GPO LEDの状態の設定(2進数表現)
//!
//! @param num ... 設定する値
//-----------------------------------------------------------------------------
void PfDebug::set_led(uint32_t num)
{
	uint32_t mask  = 0xff;
	uint32_t value = num & mask;
	sceKernelSetGPO(value);
}
#endif

#if	defined(_PS3) && !(MT_MASTER)
signed char	*assertFile = NULL;
signed int	assertLine;
char	assertInfo[256];
#endif

#if	defined(_PS3) && !(MT_MASTER)
//-----------------------------------------------------------------------------
//! dialogでassert表示を行う
//-----------------------------------------------------------------------------
void PfDebug::assertDialog(const char* buf)
{
	if(assertFile){
		sprintf(assertInfo, "assert info:%s @%s :%d\n", buf, assertFile, assertLine);
		printf("%s\n", assertInfo);
		cellMsgDialogOpen(CELL_MSGDIALOG_DIALOG_TYPE_ERROR, assertInfo, NULL, NULL, NULL);
		assertFile = NULL;
	}
	else{
		strcpy(assertInfo, (const char*)buf);
		cellMsgDialogOpen(CELL_MSGDIALOG_DIALOG_TYPE_ERROR, assertInfo, NULL, NULL, NULL);
	}
#ifndef _ROM
	asm __volatile__("tw 31,1,1");
#endif
}
#endif

#if defined(_PS3) && defined(_DEBUG)
void assertms(const char* str, const char* file, s32 line)
{
	CellFsStat status;
	CellFsErrno err;

	cellSysmoduleLoadModule(CELL_SYSMODULE_FS);

	for(s32 i = 0; i < 3; i++){
		err = cellFsStat(SYS_DEV_MS, &status);
		if(err == CELL_FS_SUCCEEDED){
			int fd;
			char buf[1024];
			u64 wb;
			MT_MSOUTPUT_FINALIZE();

			// ファイル名生成
			MT_CHAR filename[32];
			s32 file_idx = 0;
			while(err == CELL_FS_SUCCEEDED){
				sprintf(filename, "%s/assert%05d.txt", SYS_DEV_MS, file_idx++);
				if(file_idx >= 0x10000) break;
				err = cellFsStat(filename, &status);
			}
			cellFsOpen(filename, CELL_FS_O_RDWR|CELL_FS_O_CREAT, &fd, NULL, 0);
			memset(buf, 0, 1024);
			sprintf(buf, "%s:%d\n%s\n", file, line, str);
			cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);

			MtTime wtime = MtTime::getCurrent();
			sprintf(buf, "%s\n", (const char*)(wtime.format("%c")));
			cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);

			cellFsClose(fd);
			break;
		}
		MT_SLEEP(500);
	}

	for(s32 i = 0; i < 3; i++){
		err = cellFsStat(SYS_DEV_USB, &status);
		if(err == CELL_FS_SUCCEEDED){
			int fd;
			char buf[1024];
			u64 wb;
			MT_MSOUTPUT_FINALIZE();

			// ファイル名生成
			MT_CHAR filename[32];
			s32 file_idx = 0;
			while(err == CELL_FS_SUCCEEDED){
				sprintf(filename, "%s/assert%05d.txt", SYS_DEV_USB, file_idx++);
				if(file_idx >= 0x10000) break;
				err = cellFsStat(filename, &status);
			}
			cellFsOpen(filename, CELL_FS_O_RDWR|CELL_FS_O_CREAT, &fd, NULL, 0);
			memset(buf, 0, 1024);
			sprintf(buf, "%s:%d\n%s\n", file, line, str);
			cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);

			MtTime wtime = MtTime::getCurrent();
			sprintf(buf, "%s\n", (const char*)(wtime.format("%c")));
			cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);

			cellFsClose(fd);
			break;
		}
		MT_SLEEP(500);
	}

	for(s32 i = 0; i < 3; i++){
		err = cellFsStat(MtFile::getCachePath(), &status);
		if(err == CELL_FS_SUCCEEDED){
			// フォルダ生成
			MT_CHAR folderName[64];
			snprintf(folderName, sizeof(folderName)/sizeof(*folderName)-1, "%s/Logs", MtFile::getCachePath());
			cellFsMkdir(folderName, CELL_FS_DEFAULT_CREATE_MODE_1);

			int fd;
			char buf[1024];
			u64 wb;
			MT_HDDOUTPUT_FINALIZE();

			// ファイル名生成
			MT_CHAR filename[32];
			s32 file_idx = 0;
			while(err == CELL_FS_SUCCEEDED){
				sprintf(filename, "%s/assert%05d.txt", folderName, file_idx++);
				if(file_idx >= 0x10000) break;
				err = cellFsStat(filename, &status);
			}
			cellFsOpen(filename, CELL_FS_O_RDWR|CELL_FS_O_CREAT, &fd, NULL, 0);
			memset(buf, 0, 1024);
			sprintf(buf, "%s:%d\n%s\n", file, line, str);
			cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);
			{
				sys_memory_info_t mem;
				sys_memory_get_user_memory_size(&mem);
				sprintf(buf, "mem total - %d/avail - %d\n",mem.total_user_memory, mem.available_user_memory);
				cellFsWrite(fd, (const void*)buf, strlen(buf), &wb);
			}
			cellFsClose(fd);
			break;
		}
		MT_SLEEP(500);
	}
}
#elif defined(_VITA)
void vitaAssert(const char* exp, const char* text, const char* file, s32 line)
{
#if defined(_DEBUG)
#pragma diag_push
#pragma diag_suppress=237
	fprintf(stderr, "[Assertion failed] %s:%d %s %s\n", file, line, exp, text);
#if defined(__SNC__)
	__breakpoint( 0 );
#else
	__asm volatile( "bkpt 0x0000" );
#endif
#pragma diag_pop
#endif
}
#endif

#if !MT_MASTER

//-----------------------------------------------------------------------------
//! メモリダンプをトレース出力
//-----------------------------------------------------------------------------
void PfDebug::dumpMemory(const void* padr, size_t size)
{
	if(padr == NULL || size == 0) return;

	const u8* p = (const u8*)padr;
	uintptr adr = (reinterpret_cast<uintptr>(padr) >> 4) << 4;
	size_t sub = reinterpret_cast<uintptr>(padr) - adr;
	
	MT_WARNING("-------------------------------------\n");
	MT_WARNING("MemoryDump adr:0x%p, size:%ld[0x%lx]\n", (void*)padr, size, size);

	// 実機デバッガのトレースが遅いので、１行作ってから出力
	MT_CHAR str[256];
	//uint32_t len = 0;
	size_t count = 0;
	if(sub){
		s32	len = sprintf(str, "0x%p | ", adr);

		uint32_t space = 0;
		uint32_t i;
		for(i=0; i<sub; i++){
			len += sprintf(&str[len], "**");
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
		}
		for(; i<16 && count < size; i++){
			len += sprintf(&str[len], "%02x", p[count]);
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
			count++;
		}
		if(count == size){
			for(; i<16; i++){
				len += sprintf(&str[len], "**");
				if(++space == 4){
					len += sprintf(&str[len], " ");
					space = 0;
				}
			}
		}
		MT_WARNING("%s\n",str);
		adr += 0x10;
	}
	while(count < size){
		s32	len = sprintf(str, "0x%p | ", adr);
		uint32_t space = 0;
		uint32_t i;
		for(i = 0; i<16 && count < size; i++){
			len += sprintf(&str[len], "%02x", p[count]);
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
			count++;
		}
		if(i!=16){
			for(; i<16; i++){
				len += sprintf(&str[len], "**");
				if(++space == 4){
					len += sprintf(&str[len], " ");
					space = 0;
				}
			}
		}
		MT_WARNING("%s\n",str);
		adr += 0x10;
	}

	MT_WARNING("-------------------------------------\n");
}

#if !defined(SPU)

//-----------------------------------------------------------------------------
//! メモリダンプをファイル出力
//-----------------------------------------------------------------------------
bool PfDebug::dumpMemoryFromFile(const void* padr, size_t size, const char* name)
{
	if(padr == NULL || size == 0 || name == NULL){
		return false;
	}

	bool flag = false;
	MT_CHAR path[256];
	MtTime time = MtTime::getCurrent();
	// ルートに書き出し
	{
		snprintf(path, sizeof(path),"mem_dump_%s_%d%02d%02d_%d_%d.txt",name,time.getYear(),time.getMonth(),time.getDay(),time.getHour(),time.getMinute());
		MtFile file(path, MtFile::MODE_WRITE);
		if(file.isWritable()){
			MtFileStream out(file);
			if(dumpMemoryFromFile(padr, size, &out)){
				flag = true;
			}
		}
	}
#if defined(_CTR)
	// SDカードに書き出し
	{
		sprintf(path,"sdmc:\\mem_dump_%s_%d%02d%02d_%d_%d.txt",name,time.getYear(),time.getMonth(),time.getDay(),time.getHour(),time.getMinute());
		MtFile file(path, MtFile::MODE_WRITE);
		if(file.isWritable()){
			MtFileStream out(file);
			if(dumpMemoryFromFile(padr, size, &out)){
				flag = true;
			}
		}
	}
#endif
	return flag;
}
//-----------------------------------------------------------------------------
//! メモリダンプをファイル出力
//-----------------------------------------------------------------------------
bool PfDebug::dumpMemoryFromFile(const void* padr, size_t size, MtStream* pout)
{
	if(padr == NULL || size == 0 || pout == NULL || !pout->isWritable()){
		return false;
	}
	MtDataWriter dw(*pout);
	MT_CHAR str[512];

	const u8* p = (const u8*)padr;
	uintptr adr = (reinterpret_cast<uintptr>(padr) >> 4) << 4;
	size_t sub = reinterpret_cast<uintptr>(padr) - adr;
	uint32_t count = 0;
	//uint32_t len = 0;

	sprintf(str,"------------------------------------------------------------------------------------------------------------------\n");
	dw.writeString(str);
	sprintf(str, "MemoryDump adr:0x%p, size:%ld[0x%lx]\n", reinterpret_cast<uintptr>(padr), size, size);
	dw.writeString(str);

	// 実機デバッガのトレースが遅いので、１行作ってから出力
	if(sub){
		s32	len = sprintf(str, "0x%p | ", adr);

		uint32_t space = 0;
		uint32_t i;
		for(i=0; i<sub; i++){
			len += sprintf(&str[len], "**");
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
		}
		for(; i<16 && count < size; i++){
			len += sprintf(&str[len], "%02x", p[count]);
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
			count++;
		}
		if(count == size){
			for(; i<16; i++){
				len += sprintf(&str[len], "**");
				if(++space == 4){
					len += sprintf(&str[len], " ");
					space = 0;
				}
			}
		}
		sprintf(&str[len], "\n");
		dw.writeString(str);
		adr += 0x10;
	}
	while(count < size){
		s32	len = sprintf(str, "0x%p | ", adr);
		uint32_t space = 0;
		uint32_t i;
		for(i = 0; i<16 && count < size; i++){
			len += sprintf(&str[len], "%02x", p[count]);
			if(++space == 4){
				len += sprintf(&str[len], " ");
				space = 0;
			}
			count++;
		}
		if(i!=16){
			for(; i<16; i++){
				len += sprintf(&str[len], "**");
				if(++space == 4){
					len += sprintf(&str[len], " ");
					space = 0;
				}
			}
		}
		sprintf(&str[len], "\n");
		dw.writeString(str);
		adr += 0x10;
	}

	return true;
}

#endif

//------- PfDebug::doAssert()
#define EXTRACT_VA_ARG()									\
	MT_CHAR vaBuffer[512] = {0};							\
	va_list marker;											\
	va_start( marker, format );								\
	vsnprintf(vaBuffer,sizeof(vaBuffer)-1,format,marker);	\
	va_end( marker )

#if defined(_WINDOWS) || defined(_XBOX) || defined(_XBO)
#include <signal.h>

// アサートメッセージ用のデータ
static const MT_CHAR* BOXINTRO = "Assertion failed! : MTFramework 2.0";
static const MT_CHAR* PROGINTRO= "Program: ";
static const MT_CHAR* FILEINTRO= "File: ";
static const MT_CHAR* LINEINTRO= "Line: ";
static const MT_CHAR* EXPRINTRO= "Expression: ";
static const MT_CHAR* VAINTRO  = "Comment: ";
static const MT_CHAR* HELPINTRO= "(Press Retry to debug the application - JIT must be enabled)";
static const MT_CHAR* szNewLine = "\n";

static const s32 ASSERTBUFSZ= 1024;

// Windowsのラップ実装
#if defined(_XBOX)
enum { IDABORT, IDRETRY, IDIGNORE };

static s32 XbMessageBox__(MT_CHAR *assertbuf)
{
    USHORT usLen = (USHORT)strlen(assertbuf);

 #ifdef _UNICODE
    CHAR assertbufA[ASSERTBUFSZ];
    char *psz = assertbufA;
    WideCharToMultiByte(CP_ACP, 0, assertbuf, usLen, assertbufA, sizeof(assertbufA), NULL, NULL);
 #else  /* _UNICODE */
    char *psz = assertbuf;
 #endif  /* _UNICODE */

    int iAssert;
 #if _XBOX_VER >= 200
    #define DEBUG_RTLASSERT_BREAKPOINT      26  // RtlAssert()
    _asm
    {
        lhz     r4, usLen
        lwz     r3, psz
        twi     31, r0, DEBUG_RTLASSERT_BREAKPOINT
        stw     r3, iAssert
    }
 #else  /* _XBOX_VER >= 200 */
    ANSI_STRING ast, *past;
    ast.Buffer = psz;
    ast.Length = usLen;
    ast.MaximumLength = ast.Length + 1;
    past = &ast;


    _asm {
        mov eax, BREAKPOINT_RTLASSERT
        mov ecx, past
        int 2dh
        int 3h
        mov iAssert, eax
    }
 #endif  /* _XBOX_VER >= 200 */

    s32 nCode;
    switch(iAssert) {
    case 'i':
    case 'I':
        nCode = IDIGNORE;
        break;
    case 't':
    case 'T':
        nCode = IDABORT;
        break;
    default:
        nCode = IDRETRY;
        break;
    }

    return nCode;
}
#endif

//! @brief 文字列連結
static void concatString(char* szSource, uint32_t& sizeBuffer, const char* szString)
{
	const uint32_t len = static_cast<uint32_t>(strlen(szString));
	if( !len ){
		return ;
	}

	if( sizeBuffer>=len ){
	    strcat( szSource, szString );
		sizeBuffer -= len;
	}
}

//! @brief アサート
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	MT_CHAR assertBuf[ASSERTBUFSZ] = {0};

	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

	uint32_t sizeBuffer = sizeof(assertBuf)/sizeof(*assertBuf)-1;

    // box intro
	concatString(assertBuf, sizeBuffer, BOXINTRO);
	concatString(assertBuf, sizeBuffer, szNewLine);
	concatString(assertBuf, sizeBuffer, szNewLine);

#if defined(_WINDOWS)
    // program info
	//MT_CHAR szProgname[MAX_PATH] = {0};

	concatString(assertBuf, sizeBuffer, PROGINTRO);

	const uint32_t beforeProgname = static_cast<uint32_t>(strlen(assertBuf));
	if( GetModuleFileName( NULL, assertBuf + strlen(assertBuf), static_cast<s32>(ASSERTBUFSZ-strlen(assertBuf) -1 )) ){
		sizeBuffer -= static_cast<uint32_t>(strlen(assertBuf))-beforeProgname;
	}else{
		concatString(assertBuf, sizeBuffer, "<program name unknown>");
	}

	concatString(assertBuf, sizeBuffer, szNewLine);
#endif

     // file info
	concatString(assertBuf, sizeBuffer, FILEINTRO);
	concatString(assertBuf, sizeBuffer, szFile);
	concatString(assertBuf, sizeBuffer, szNewLine);

     // line 
	concatString(assertBuf, sizeBuffer, LINEINTRO);
	const uint32_t beforeItoA = static_cast<uint32_t>(strlen(assertBuf));
    _itoa_s( (s32)nLine, assertBuf + strlen(assertBuf), ASSERTBUFSZ - strlen(assertBuf), 10 );
	sizeBuffer -=  static_cast<uint32_t>(strlen(assertBuf))-beforeItoA;
	concatString(assertBuf, sizeBuffer, szNewLine);
	concatString(assertBuf, sizeBuffer, szNewLine);

     // message line
	concatString(assertBuf, sizeBuffer, EXPRINTRO);
	concatString(assertBuf, sizeBuffer, szExpression);
	concatString(assertBuf, sizeBuffer, szNewLine);
	concatString(assertBuf, sizeBuffer, VAINTRO);
	concatString(assertBuf, sizeBuffer, vaBuffer);
	concatString(assertBuf, sizeBuffer, szNewLine);
	concatString(assertBuf, sizeBuffer, szNewLine);

#if defined(_WINDOWS)
     // help line
	concatString(assertBuf, sizeBuffer, HELPINTRO);
#endif

#if defined(_WINDOWS)
	HWND hOwner = GetActiveWindow();
	const s32 nCode = MessageBox(hOwner ? GetLastActivePopup(hOwner) : NULL, assertBuf, "MTFramework 2.0", MB_ABORTRETRYIGNORE|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
#elif defined(_XBOX)
    const s32 nCode = XbMessageBox__(assertBuf);
#elif defined(_XBO)
	const s32 nCode = 0;
	MT_ERROR(assertBuf);
	MT_DEBUG_BREAK();
#endif

	switch( nCode ){
	case IDABORT:
		{
			::raise(SIGABRT);
			::_exit(3);
		}break;
	case IDRETRY:
		{
			return true;
		}break;
	case IDIGNORE:
		{
		}break;
	default:
		{
			::abort();
		}break;
	}

	return false;
}
#elif defined(_PS3)
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

 #if defined(_DEBUG)
	assertms(szExpression,szFile,nLine);
 #endif
	std::printf("assert info:%s %s @%s :%d\n", szExpression, vaBuffer, szFile, nLine);

	return true;
}
#elif defined(_LITE) && defined(_CTR)
// CTR
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

	nn::dbg::detail::PutString(vaBuffer, strlen(vaBuffer));
	return true;
}
#elif defined(_CAFE)
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

	MT_REPORT("assert info:%s %s @%s :%d\n", szExpression, vaBuffer, szFile, nLine);
	return true;
}
#elif defined(_PS4)
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

	std::printf("assert info:%s %s @%s :%d\n", szExpression, vaBuffer, szFile, nLine);
	return true;
}
#else
bool PfDebug::doAssert(const char* szFile, uint32_t nLine, const char* szExpression, const char* format, ...)
{
	EXTRACT_VA_ARG();

	// コールバック
	if( mAssertCallback && !mAssertCallback(szExpression, vaBuffer, szFile, nLine, mpAssertParam) ){
		return false;// コールバック側で適切な処理を実装した。
	}

	//TODO: 実装
	return true;
}
#endif

#endif// !(MT_MASTER)
