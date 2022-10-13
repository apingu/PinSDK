//
//
//
//
//
//
//
//
//
//  platform foundation file function
//
//
//
//              Copyright (C) 2003 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>

typedef void    PfsFILE;


#if defined(_WINDOWS) || defined(_XBOX) || defined(_LINUX)
    // Seek types
    #define PF_SEEK_SET   SEEK_SET
    #define PF_SEEK_CUR   SEEK_CUR
    #define PF_SEEK_END   SEEK_END
#endif
#if defined(_PS2)
    #define PF_SEEK_SET   SCE_SEEK_SET
    #define PF_SEEK_CUR   SCE_SEEK_CUR
    #define PF_SEEK_END   SCE_SEEK_END
#endif


void            PfsInit         ( void );
void            PfsSetDevice    ( int device = 1 );
PfsFILE*        Pfsopen         ( const char* name, const char* access );
int             Pfsclose        ( PfsFILE* stream );
unsigned int    Pfsread         ( void* buffer, unsigned int size, unsigned int count, PfsFILE* stream );
unsigned int    Pfswrite        ( const void* buffer, unsigned int size, unsigned int count, PfsFILE* stream );
int             Pfsgetc         ( PfsFILE* stream );
int             Pfsputc         ( int c, PfsFILE* stream );
char*           Pfsgets         ( char* buffer, int maxlength, PfsFILE* stream );
int             Pfsputs         ( const char* buffer, PfsFILE* stream );
int             Pfseof          ( PfsFILE* stream );
int             Pfsseek         ( PfsFILE* stream , long32_t offset, int origin );
int             Pfsflush        ( PfsFILE* stream );
int             Pfstell         ( PfsFILE* stream );
int             Pfsfilelength   ( PfsFILE* stream );
void            PfsSetDirectory ( const char* directory );