//pstring.h
//
//
// string
//
//
//
//
//
//
//
//
//
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <string>
#include <stdio.h>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>
#include <t_lock.h>

namespace pfstd
{
    class CString : public _tstring//, public ThreadSafe
    {
        T_Lock utLock;
    
    public:

        CString();
        ~CString();
        CString( const tchar_t* s );
        CString( const CString& s );
        CString( const _tstring& s );
        
        //CString operator = (_mstring s);
        //CString operator = (const tchar_t* s);
        const tchar_t  operator[]( int i );
        operator const tchar_t* ();
        void           format( const tchar_t* fm, ... );
        int            toint ( void );
        void           clear ( void );
        const tchar_t* copy  ( const tchar_t* sstr );
        void           cat   ( const tchar_t* s );
        tchar_t        front ( void );
        tchar_t        back  ( void );
        //
        // the destroy timing is wrong, too early
    };
    
    
    /*
    class CString
    {
     T_Lock _StringLock;
    
    protected:
    
     tchar_t*   m_array;
    
    public:
    
     CString()
     {
      m_array = NULL;
     }
    
     CString(const tchar_t* s)
     {
      m_array = NULL; operator = (s);
     }
    
     CString(CString &s)
     {
      m_array = 0; operator = (s);
     }
    
     ~CString()
     {
      release();
     }
    
     CString &operator=(const tchar_t* s)
     {
      TS_Lock lock(&_StringLock);
      release();
      if(s == NULL)
       return *this;
      m_array = new tchar_t[strlen(s) + 1];
      strcpy(m_array, s);
      return *this;
     }
    
     CString &operator=(const CString &s)
     {
      return operator = (s.m_array);
     }
    
     int operator==(CString s)
     {
      return operator==((tchar_t*) s);
     }
    
     int operator==(const tchar_t* s)
     {
      TS_Lock lock(&_StringLock);
      if(s == NULL)
       return 0;
      if(strcmp(m_array, s) == 0)
       return 1;
      return 0;
     }
    
     int operator!=(CString s)
     {
      TS_Lock lock(&_StringLock);
      if(strcmp(m_array, (tchar_t*) s) == 0)
       return 0;
      return 1;
     }
    
     int operator!=(const tchar_t* s)
     {
      TS_Lock lock(&_StringLock);
      if(strcmp(m_array, s) == 0)
       return 0;
      return 1;
     }
    
     friend CString operator+(CString &s1, const tchar_t* s2)
     {
      TS_Lock lock(&_StringLock);
      if(s2 == NULL)
       return s1.m_array;
      tchar_t s[10240];
      if(s1.m_array != 0)
       _stprintf(s, "%s%s", s1.m_array, s2);
      else
       strcpy(s, s2);
      return s;
     }
    
     operator tchar_t*()
     {
      TS_Lock lock(&_StringLock);
      return m_array;
     }
    
     operator const tchar_t*()
     {
      TS_Lock lock(&_StringLock);
      return m_array;
     }
    
    
     tchar_t &operator[](int i)
     {
      TS_Lock lock(&_StringLock);
      return m_array[i];
     }
    
     void clear(void)
     {
      release();
     }
    
     void release(void)
     {
      TS_Lock lock(&_StringLock);
      if(m_array != 0)
      {
       delete [] m_array;
       m_array = 0;
      }
     }
    
     void allot(int len)
     {
      release();
      TS_Lock lock(&_StringLock);
      m_array = new tchar_t[len];
      memset(m_array, 0, len);
     }
    
     int len(void)
     {
      TS_Lock lock(&_StringLock);
      if(m_array == 0)
       return 0;
      return strlen(m_array);
     }
    
     int ncmp(const tchar_t* s, size_t n)
     {
      TS_Lock lock(&_StringLock);
      return strncmp(m_array, s, n);
     }
    
     tchar_t* partcpy(tchar_t* s, size_t n, int b, int e)
     {
      TS_Lock lock(&_StringLock);
      tchar_t*   ptr   = strchr(m_array, b);
      ptr++;
      unsigned int i = 0;
      while(ptr[i] != e)
      {
       if(i < n)
       {
        s[i] = ptr[i];
        i++;
       }else
       {
        i--;
        break;
       }
      }
      s[i] = '\0';
      return s;
     }
    
     int toint(void)
     {
      TS_Lock lock(&_StringLock);
      return atoi(m_array);
     }
    
     tchar_t last(void)
     {
      TS_Lock lock(&_StringLock);
      return m_array[strlen(m_array)];
     }
    
     void cat(const tchar_t* string)
     {
      TS_Lock lock(&_StringLock);
      //release();
      if(string == 0)
       return;
    
      int  len   = strlen(string);
      tchar_t*   tmp   = NULL;
      if(m_array != NULL)
      {
       len += strlen(m_array);
       tmp = new tchar_t[len + 1];
       _stprintf(tmp, "%s%s", m_array, string);
      }else
      {
       tmp = new tchar_t[len + 1];
       strcpy(tmp, string);
      }
    
      release();
      m_array = tmp;
      return;
     }
    
     bool empty(void)
     {
      TS_Lock lock(&_StringLock);
      if(m_array == 0 || *m_array == '\0')
       return true;
      return false;
     }
    
     bool null(void)
     {
      TS_Lock lock(&_StringLock);
      if(strlen(m_array) == 0)
       return true;
      return false;
     }
    
     void format(const tchar_t* fm, ...)
     {
      TS_Lock lock(&_StringLock);
      tchar_t string[1024];
      va_list arglist;
      const tchar_t* i = fm;
    
      va_start(arglist, fm);
    
      assert(string != NULL);
      assert(format != NULL);
      vsprintf(string, fm, arglist);
      va_end(arglist);
      operator=(string);
      return;
     }
    
     tchar_t* copy(const tchar_t* strSource)
     {
      TS_Lock lock(&_StringLock);
      release();
      if(strSource == 0)
       return *this;
      m_array = new tchar_t[strlen(strSource) + 1];
      strcpy(m_array, strSource);
      return *this;
     }
    
     tchar_t front(void)
     {
      TS_Lock lock(&_StringLock);
      return m_array[0];
     }
    
     tchar_t back(void)
     {
      TS_Lock lock(&_StringLock);
      return m_array[strlen(m_array) - 1];
     }
    
     int printf(const tchar_t* format, ...)
     {
      int count = 0, sum = 0;
      const tchar_t* i = format;
      va_list ouString_t;
    
      va_start(ouString_t, format);  // Initialize variable arguments.
      while(i != 0)
      {
       i = va_arg(ouString_t, const tchar_t*);
       printf("%s",i);
      }
      va_end(ouString_t);     //Reset variable arguments.
      return(sum ? (sum / count) : 0);
     }
     */
    //};
    
};//PS