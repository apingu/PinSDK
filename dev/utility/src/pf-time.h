//
// HRISTime.h
//
//    HRIS系統使用的單位時間計算類別
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <time.h>
#include <string>

#ifdef _LINUX
#include <sys/time.h>
#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


#define _BASE_YEAR  1900
#define _BASE_MOMTH 1

#ifdef _WINDOWS
int   gettimeofday( struct timeval* tvp, struct timezone* tzp );
#endif


namespace pftml
{
    // 時間的資料
    class Time
    {
        uint_t  time_value_[ 6 ];

    public:

        // 預設為現在時間
        Time();
        Time( _IN const Time& t );
        Time( _IN uint_t year, _IN uint_t month = 1, _IN uint_t day = 1, _IN uint_t hour = 0, _IN uint_t minute = 0, _IN uint_t second = 0 );
        Time( _IN const char_t*  str );
        Time( _IN const wchar_t* str );
        ~Time();

        uint_t              GetYear() const;
        uint_t              GetMonth() const;
        uint_t              GetDay() const;
        uint_t              GetHour() const;
        uint_t              GetMinute() const;
        uint_t              GetSecond() const;
        uint_t              GetDayOfWeek() const;
        uint64_t            GetTimeValue() const;  // 20170832

        void                Clear( );
        bool                IsEmpty() const;

        // get timestamp
        stimen_t            GetTime() const;
        // 有修正的話傳回 -1 否則傳回1
        int                 SetTime( _IN stimen_t tt );
        int                 SetTime( _IN uint_t year, _IN uint_t month = 1, _IN uint_t day = 1, _IN uint_t hour = 0, _IN uint_t minute = 0, _IN uint_t second = 0 );
        void                SetToday();

        // 傳入開始的時間 傳出 傳入的時間 與 目前的相差秒數
        stimen_t            DiffTime( _IN const Time& ts );

        // 往前/後加幾秒
        void                PlusSecond( _IN int nsecond );
        // 往前/後加幾分鐘
        void                PlusMinute( _IN int nminute );
        // 往前/後加幾小時
        void                PlusHour( _IN int nhour );
        // 往前/後加幾天
        void                PlusDay( _IN int nday );
        // 往前/後加幾個月
        void                PlusMonth( _IN int nmonth );
        // 往前/後加幾年
        void                PlusYear( _IN int nyear );

        // 設為該月的第一天
        void                SetFirstDateTimeOfMonth();

        // 設為該月的最後一天
        void                SetLastDateTimeOfMonth();

        // 01:01:01
        void                SetFirstSecondOfDay();

        // 23:59:59
        void                SetLastSecondOfDay();

        _mstring            GetDateSTR( _IN const char_t*  fmt = "%04d-%02d-%02d" ) const;
        _wstring            GetDateWCS( _IN const wchar_t* fmt = L"%04d-%02d-%02d" ) const;
        //_tstring            GetDateTCS( const tchar_t* fmt = _T("%04d-%02d-%02d") );

        _mstring            GetDateTimeSTR( _IN const char_t*  fmt = "%04d-%02d-%02d %02d:%02d:%02d" ) const;
        _wstring            GetDateTimeWCS( _IN const wchar_t* fmt = L"%04d-%02d-%02d %02d:%02d:%02d" ) const;
        //_tstring            GetDateTimeTCS( const tchar_t* fmt = _T("%04d-%02d-%02d %02d:%02d:%02d") );

        // 都沒有 傳回 6  有年 5  年月 4 年月日3 年月日時 2 年月日時分 1 年月日時分秒 0
        int                 SetDateTimeSTR( _IN const char_t*  str );
        int                 SetDateTimeWCS( _IN const wchar_t* str );


#ifdef _UNICODE
#define GetDateTCS      GetDateWCS
#define GetDateTimeTCS  GetDateTimeWCS
#define SetDateTimeTCS  SetDateTimeWCS
#else
#define GetDateTCS      GetDateSTR
#define GetDateTimeTCS  GetDateTimeSTR
#define SetDateTimeTCS  SetDateTimeSTR
#endif


        //
        //int                 TryParseStr( const char_t*  str );
        //int                 TryParseWcs( const wchar_t*  str );

        // 取得秒 從1990始
        static   stimen_t   GetSTime( void );
        // 取得毫秒 從開機開始
        static   mstimen_t  GetTickMSTime( void );
        // 取得微秒 從開機開始
        static   ustimen_t  GetTickUSTime( void );

        //  0 = "星期日"; 1 = "星期一";  ......
        static   uint_t     GetWeekDay( _IN uint_t y, _IN uint_t m, _IN uint_t d );

        // 取得該月的最後一天
        static   uint_t     GetLastDayOfMonth( _IN uint_t y, _IN uint_t m );


        bool operator <  ( _IN const Time& t ) const;
        bool operator >  ( _IN const Time& t ) const;
        bool operator == ( _IN const Time& t ) const;
        bool operator != ( _IN const Time& t ) const;
        bool operator <= ( _IN const Time& t ) const;
        bool operator >= ( _IN const Time& t ) const;
    };

}