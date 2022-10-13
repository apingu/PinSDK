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
//
//
//
//
//////////////////////////////////////////////////////////////////////////

//01. == C 系統文件/C++ 系統文件
#if defined (_WINDOWS) || defined (_XBOX)
#include <WinSock2.h>
#endif
#if defined (_LINUX)
#include <sys/time.h>
#endif
#ifdef _PS2
#include  <kernel.h>
#endif

#include <string.h>
#include <sys/timeb.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-time.h"
#include "pf-stdlib.h"
#include "Unicode.h"




#if defined (_WINDOWS) || defined (_XBOX)
//=============================================================================
// NAME: gettimeofday
// DESC:
//=============================================================================
int gettimeofday( struct timeval* tvp, struct timezone* tzp )
{
    struct _timeb tb;
    _ftime( &tb );

    if( tvp )
    {
        tvp->tv_sec = tb.time;
        tvp->tv_usec = tb.millitm * 1000;
    }

    if( tzp )
    {
        tzp->tz_minutestwest = tb.timezone;
        tzp->tz_dsttime = tb.dstflag;
    }

    return 0;
}
#endif

namespace pftml
{
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::Time()
    {
        SetToday();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::Time( _IN const Time& t )
    {
        ( *this ) = t;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::Time( _IN uint_t year, _IN uint_t month, _IN uint_t day, _IN uint_t hour, _IN uint_t minute, _IN uint_t second )
    {
        SetTime( year, month, day, hour, minute, second );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::Time( _IN const char_t* str )
    {
        SetDateTimeSTR( str );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::Time( _IN const wchar_t* str )
    {
        SetDateTimeWCS( str );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Time::~Time()
    {
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint_t Time::GetYear() const
    {
        return time_value_[ 0 ];
    }

    uint_t Time::GetMonth() const
    {
        return time_value_[ 1 ];
    }

    uint_t Time::GetDay() const
    {
        return time_value_[ 2 ];
    }

    uint_t Time::GetHour() const
    {
        return time_value_[ 3 ];
    }

    uint_t Time::GetMinute() const
    {
        return time_value_[ 4 ];
    }

    uint_t Time::GetSecond() const
    {
        return time_value_[ 5 ];
    }

    uint_t Time::GetDayOfWeek() const
    {
        int year = time_value_[ 0 ];
        int m = time_value_[ 1 ];
        int d = time_value_[ 2 ];

        if( m <= 2 )
        {
            year -= 1;
            m += 12;
        }

        int c = year / 100;
        int y = year % 100;
        int w = ( int )( c / 4 ) - 2 * c + y + ( int )( y / 4 ) + ( int )( 13 * ( m + 1 ) / 5 ) + d - 1;
        return w % 7;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint64_t Time::GetTimeValue() const  // 20170832103220
    {
        _mstring pmtime;
        pfstd::strprintf( pmtime, ( "%04d%02d%02d%02d%02d%02d" ), time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ], time_value_[ 3 ], time_value_[ 4 ], time_value_[ 5 ] );
        return atoll( pmtime.c_str() );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::Clear()
    {
        time_value_[ 0 ] = 0;
        time_value_[ 1 ] = 0;
        time_value_[ 2 ] = 0;
        time_value_[ 3 ] = 0;
        time_value_[ 4 ] = 0;
        time_value_[ 5 ] = 0;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Time::IsEmpty() const
    {
        for( uint_t ti = 0; ti < 6; ti++ )
        {
            if( time_value_[ ti ] != 0 )
            {
                return false;
            }
        }
        return true;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t Time::GetTime() const
    {
        struct tm ttm;
        memset( &ttm, 0, sizeof( tm ) );
        ttm.tm_year = time_value_[ 0 ] - _BASE_YEAR;
        ttm.tm_mon = time_value_[ 1 ] - _BASE_MOMTH;
        ttm.tm_mday = time_value_[ 2 ];
        ttm.tm_hour = time_value_[ 3 ];
        ttm.tm_min = time_value_[ 4 ];
        return mktime( &ttm );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Time::SetTime( _IN stimen_t tt )
    {
        struct tm* ltm;
        time_t tmt = ( time_t )tt;
        ltm = localtime( &tmt );
        time_value_[ 0 ] = ltm->tm_year + _BASE_YEAR;
        time_value_[ 1 ] = ltm->tm_mon + _BASE_MOMTH;
        time_value_[ 2 ] = ltm->tm_mday;
        time_value_[ 3 ] = ltm->tm_hour;
        time_value_[ 4 ] = ltm->tm_min;
        time_value_[ 5 ] = ltm->tm_sec;
        return 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Time::SetTime( _IN uint_t year, _IN uint_t month, _IN uint_t day, _IN uint_t hour, _IN uint_t minute, _IN uint_t second )
    {
        int ret = 1;
        time_value_[ 0 ] = year;

        if( ( month >= 0 ) && ( month <= 12 ) )
            time_value_[ 1 ] = month;
        else
        {
            ret = -1;
        }

        if( month != 0 )
        {
            // 如果只有到月份 就會是0
            if( ( day >= 0 ) && ( day <= GetLastDayOfMonth( time_value_[ 0 ], time_value_[ 1 ] ) ) )
                time_value_[ 2 ] = day;
            else
            {
                ret = -1;
            }
        }
        else
        {
            time_value_[ 2 ] = 0;
        }


        if( ( hour >= 0 ) && ( hour <= 23 ) )
            time_value_[ 3 ] = hour;
        else
        {
            ret = -1;
        }

        if( ( minute >= 0 ) && ( minute <= 59 ) )
            time_value_[ 4 ] = minute;
        else
        {
            ret = -1;
        }

        if( ( second >= 0 ) && ( second <= 59 ) )
            time_value_[ 5 ] = second;
        else
        {
            ret = -1;
        }

        if( ret == -1 )
        {
            time_value_[ 0 ] = 0;
            time_value_[ 1 ] = 0;
            time_value_[ 2 ] = 0;
            time_value_[ 3 ] = 0;
            time_value_[ 4 ] = 0;
            time_value_[ 5 ] = 0;
        }
        return ret;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::SetToday()
    {
        time_t    now;
        struct tm today;
        time( &now ); /* get current time; same as: now = time(NULL)  */
        today = *localtime( &now );
        SetTime( today.tm_year + _BASE_YEAR, today.tm_mon + _BASE_MOMTH, today.tm_mday, today.tm_hour, today.tm_min, today.tm_sec );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t Time::DiffTime( _IN const Time& ts )
    {
        return difftime( GetTime(), ts.GetTime() );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusSecond( _IN int nsecond )
    {
        stimen_t tt = GetTime();
        tt += nsecond;
        SetTime( tt );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusMinute( _IN int nminute )
    {
        stimen_t tt = GetTime();
        tt += 60 * nminute;
        SetTime( tt );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusHour( _IN int nhour )
    {
        stimen_t tt = GetTime();
        tt += ( 60 * 60 ) * nhour;
        SetTime( tt );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusDay( _IN int nday )
    {
        stimen_t tt = GetTime();
        tt += ( 60 * 60 * 24 ) * nday;
        SetTime( tt );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusMonth( _IN int nmonth )
    {
        if( nmonth == 0 )
            return;
        //int mm = ( time_value_[1] - 1 ) + nmonth;
        //time_value_[0] = time_value_[0] + ( ( ( time_value_[1] - 1 ) + nmonth ) / 12 );
        //time_value_[1] = ( ( ( time_value_[1] - 1 ) + nmonth ) % 12 ) + 1;
        int mm = time_value_[ 1 ] - 1;// 從0開始運算
        mm += nmonth;
        if( mm >= 0 )
        {
            int m_mm = ( mm % 12 );
            int s_mm = ( mm / 12 );
            time_value_[ 1 ] = m_mm;
            time_value_[ 0 ] += s_mm;
            time_value_[ 1 ]++;  //一年從一月開始
        }
        else
        {
            mm += 1;
            int m_mm = ( mm % 12 );
            int s_mm = ( mm / 12 );
            s_mm--; // 再多減一年
            time_value_[ 1 ] = 12 + m_mm;
            time_value_[ 0 ] += s_mm;
        }

        time_value_[ 2 ] = __min( time_value_[ 2 ], GetLastDayOfMonth( time_value_[ 0 ], time_value_[ 1 ] ) );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::PlusYear( _IN int nyear )
    {
        PlusMonth( nyear * 12 );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::SetFirstDateTimeOfMonth()
    {
        time_value_[ 2 ] = 1; // 日
        time_value_[ 3 ] = 0; // 月
        time_value_[ 4 ] = 0; // 分
        time_value_[ 5 ] = 0; // 秒
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::SetLastDateTimeOfMonth()
    {
        time_value_[ 2 ] = GetLastDayOfMonth( time_value_[ 0 ], time_value_[ 1 ] );
        SetLastSecondOfDay();
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::SetFirstSecondOfDay()
    {
        time_value_[ 3 ] = 1;
        time_value_[ 4 ] = 1;
        time_value_[ 5 ] = 1;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Time::SetLastSecondOfDay()
    {
        time_value_[ 3 ] = 23;
        time_value_[ 4 ] = 59;
        time_value_[ 5 ] = 59;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _mstring Time::GetDateSTR( _IN const char_t* fmt ) const
    {
        _mstring ts;
        pfstd::strprintf( ts, fmt, time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ] );
        return ts;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _wstring Time::GetDateWCS( _IN const wchar_t* fmt ) const
    {
        _wstring ts;
        pfstd::strprintf( ts, fmt, time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ] );
        return ts;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _mstring Time::GetDateTimeSTR( _IN const char_t* fmt ) const
    {
        _mstring ts;
        pfstd::strprintf( ts, fmt, time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ], time_value_[ 3 ], time_value_[ 4 ], time_value_[ 5 ] );
        return ts;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    _wstring Time::GetDateTimeWCS( _IN const wchar_t* fmt ) const
    {
        _wstring ts;
        pfstd::strprintf( ts, fmt, time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ], time_value_[ 3 ], time_value_[ 4 ], time_value_[ 5 ] );
        return ts;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    // __DATE__ ==> Sep  5 2021 ==> YEAR ((((dd[7] - '0') * 10 + (dd[8] - '0')) * 10 + (dd[9] - '0')) * 10 + (dd[10] - '0'))
    //                          ==> DAY  ((dd[4] == ' ' ? 0 : dd[4] - '0') * 10 + (dd[5] - '0'))
    //#define __PFTML_YEAR( dd )  ((((dd[7] - '0') * 10 + (dd[8] - '0')) * 10 + (dd[9] - '0')) * 10 + (dd[10] - '0'))
    //#define __PFTML_DAY( dd )   ((dd[4] == ' ' ? 0 : dd[4] - '0') * 10 + (dd[5] - '0'))

    // __TIME__ ==> Sep  5 2021 ==> ((((dd[7] - '0') * 10 + (dd[8] - '0')) * 10 + (dd[9] - '0')) * 10 + (dd[10] - '0'))
#if 0 //fail when input "Jun"
#define __PFTML_MONTH( dd ) (  dd[2] == 'n' ? 0 \
                             : dd[2] == 'b' ? 1 \
                             : dd[2] == 'r' ? (dd[0] == 'M' ? 2 : 3) \
                             : dd[2] == 'y' ? 4 \
                             : dd[2] == 'n' ? 5 \
                             : dd[2] == 'l' ? 6 \
                             : dd[2] == 'g' ? 7 \
                             : dd[2] == 'p' ? 8 \
                             : dd[2] == 't' ? 9 \
                             : dd[2] == 'v' ? 10 : 11 ) + 1
#endif

    //fixed
#define __PFTML_MONTH( dd ) (  dd[2] == 'n' ? (dd[1] == 'a' ? 0 : 5)  \
                             : dd[2] == 'b' ? 1 \
                             : dd[2] == 'r' ? (dd[0] == 'M' ? 2 : 3) \
                             : dd[2] == 'y' ? 4 \
                             : dd[2] == 'l' ? 6 \
                             : dd[2] == 'g' ? 7 \
                             : dd[2] == 'p' ? 8 \
                             : dd[2] == 't' ? 9 \
                             : dd[2] == 'v' ? 10 : 11 ) + 1

    // Sun Sep  5 15:11:01 2021
    //int SetAscTime( _IN const char_t* str )
    //{
    //    if ( ( str != NULL ) || ( strcmp( str, "" ) != 0 ) )
    //    {
    //        char_t sztime[ 7 ][ 10 ] = { "0", "0", "0", "0", "0", "0" };
    //        int sr = sscanf( str, "%s %s %[0-9] %[0-9] : %[0-9] : %[0-9] %[0-9] ", sztime[ 6 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ], sztime[ 0 ] );
    //        time_value_[ 0 ] = __PFTML_YEAR( sztime[ 0 ] );
    //        time_value_[ 1 ] = __PFTML_MONTH( sztime[ 1 ] );
    //        time_value_[ 2 ] = __PFTML_DAY( sztime[ 2 ] );
    //        time_value_[ 3 ] = atoi( sztime[ 3 ] );
    //        time_value_[ 4 ] = atoi( sztime[ 4 ] );
    //        time_value_[ 5 ] = atoi( sztime[ 5 ] );
    //    }
    //
    //    return 1;
    //}


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Time::SetDateTimeSTR( _IN const char_t* str )
    {
        if( strlen( str ) > 99 )
        {
            return -1;
        }
        SetTime( 0, 0 );

        char_t sztime[ 7 ][ 100 ] = { "0", "0", "0", "0", "0", "0", "" };
        int get_num = 0;
        //int ret = -1;

        // asctime ==> Sun Sep  5 15:11:01 2021
        {
            get_num = sscanf( str, "%s %s %[0-9] %[0-9] : %[0-9] : %[0-9] %[0-9]", sztime[ 6 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ], sztime[ 0 ] );
            if( get_num == 7 )
            {
                time_value_[ 0 ] = atoi( sztime[ 0 ] );
                time_value_[ 1 ] = __PFTML_MONTH( sztime[ 1 ] );
                time_value_[ 2 ] = atoi( sztime[ 2 ] );
                time_value_[ 3 ] = atoi( sztime[ 3 ] );
                time_value_[ 4 ] = atoi( sztime[ 4 ] );
                time_value_[ 5 ] = atoi( sztime[ 5 ] );
                return 1;
            }
        }

        if( strstr( str, "/" ) != 0 )      // 2018/11/20 14:20:17
        {
            get_num = sscanf( str, "%[0-9] / %[0-9] / %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( strstr( str, "\\" ) != 0 ) // 2018\11\20 14:20:17
        {
            get_num = sscanf( str, "%[0-9] \\ %[0-9] \\ %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( strstr( str, "-" ) != 0 ) // 2018-11-20 14:20:17
        {
            get_num = sscanf( str, "%[0-9] - %[0-9] - %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( strstr( str, "." ) != 0 ) // 2018.11.20 14:20:17
        {
            get_num = sscanf( str, "%[0-9] . %[0-9] . %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else
        {

            // 全部都是數字  ex 20171120
            size32_t sl = ( size32_t )strlen( str );
            // 至少要有到月吧
            if( sl < 6 )
            {
                return -1;
            }
            for( size32_t i = 0; i < sl; i++ )
            {
                if( !IsNumber( str[ i ] ) )
                {
                    return -1;
                }
            }
            sscanf( str, "%4s%2s%2s%2s%2s%2s", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }

        for( int ti = 0; ti < 6; ti++ )
        {
            time_value_[ ti ] = atoi( sztime[ ti ] );
        }

        // 檢查
        return SetTime( time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ], time_value_[ 3 ], time_value_[ 4 ], time_value_[ 5 ] );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    int Time::SetDateTimeWCS( _IN const wchar_t* str )
    {
        if( wcslen( str ) > 99 )
        {
            return -1;
        }

        SetTime( 0, 0 );

        wchar_t sztime[ 7 ][ 100 ] = { L"0", L"0", L"0", L"0", L"0", L"0", L"0" };
        int get_num = 0;
        //int ret = -1;

        // asctime ==> Sun Sep  5 15:11:01 2021
        {
            get_num = swscanf( str, L"%s %s %[0-9] %[0-9] : %[0-9] : %[0-9] %[0-9] ", sztime[ 6 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ], sztime[ 0 ] );
            if( get_num == 7 )
            {
                time_value_[ 0 ] = wtoi( sztime[ 0 ] );
                time_value_[ 1 ] = __PFTML_MONTH( sztime[ 1 ] );
                time_value_[ 2 ] = wtoi( sztime[ 2 ] );
                time_value_[ 3 ] = wtoi( sztime[ 3 ] );
                time_value_[ 4 ] = wtoi( sztime[ 4 ] );
                time_value_[ 5 ] = wtoi( sztime[ 5 ] );
                return 1;
            }
        }

        if( wcsstr( str, L"/" ) != 0 )
        {
            get_num = swscanf( str, L"%[0-9] / %[0-9] / %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( wcsstr( str, L"\\" ) != 0 )
        {
            get_num = swscanf( str, L"%[0-9] \\ %[0-9] \\ %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( wcsstr( str, L"-" ) != 0 )
        {
            get_num = swscanf( str, L"%[0-9] - %[0-9] - %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else if( wcsstr( str, L"." ) != 0 ) // 2018.11.20 14:20:17
        {
            swscanf( str, L"%[0-9] . %[0-9] . %[0-9] %[0-9] : %[0-9] : %[0-9]", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }
        else
        {
            // 全部都是數字  ex 20171120
            size32_t sl = ( size32_t )wcslen( str );
            // 至少要有到月吧
            if( sl < 6 )
            {
                return -1;
            }
            for( size32_t i = 0; i < sl; i++ )
            {
                if( !IsNumber( str[ i ] ) )
                {
                    return -1;
                }
            }
            get_num = swscanf( str, L"%4s%2s%2s%2s%2s%2s", sztime[ 0 ], sztime[ 1 ], sztime[ 2 ], sztime[ 3 ], sztime[ 4 ], sztime[ 5 ] );
        }

        for( int ti = 0; ti < 6; ti++ )
        {
            time_value_[ ti ] = _wtoi( sztime[ ti ] );
        }

        // 檢查
        return SetTime( time_value_[ 0 ], time_value_[ 1 ], time_value_[ 2 ], time_value_[ 3 ], time_value_[ 4 ], time_value_[ 5 ] );

    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    stimen_t Time::GetSTime( void )
    {
        return time( NULL );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t Time::GetTickMSTime( void )
    {
#if defined (_WINDOWS) || defined (_XBOX)
        return GetTickCount();
#endif
#if defined (_PS2)
        /*
        ulong32_t    high0, low0, high1, low1;
        high0 = SkyState.sweHighcount_;
        low0 = *T0_COUNT;
        high1 = SkyState.sweHighcount_;
        low1 = *T0_COUNT;
        if (high0 == high1)
        {
        return ((Puint) ((high0 | (low0  &0xffff)) / 9216));
        }
        else
        {
        return ((Puint) ((high1 | (low1  &0xffff)) / 9216));
        }
        */
        SysClock clock;

        if( GetSystemTime( &clock ) == KE_OK )
        {
            return clock;
    }

#endif
#if defined (_LINUX)
        //static timeval tv;
        //gettimeofday(&tv, 0);
        //return (uint32_t)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
        struct timeb tb;
        ftime( &tb );
        return ( ( ( tb.time - 1000000000 ) * 1000 ) + ( tb.millitm ) );
#endif
        return 0;
}

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    ustimen_t Time::GetTickUSTime( void )
    {
#if defined (_WINDOWS) || defined (_XBOX)
        LARGE_INTEGER large;
        QueryPerformanceFrequency( &large );
        return large.QuadPart;
#endif
#if defined (_PS2)
#endif
#if defined (_LINUX)
        struct timeval tv;
        gettimeofday( &tv, NULL );
        return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    }

    //=============================================================================
    // NAME: GetWeekDay()
    // DESC:
    //=============================================================================
    uint_t Time::GetWeekDay( _IN uint_t y, _IN uint_t m, _IN uint_t d )
    {
        if( m == 1 )
        {
            m = 13;
        }

        if( m == 2 )
        {
            m = 14;
        }

        int week = ( d + 2 * m + 3 * ( m + 1 ) / 5 + y + y / 4 - y / 100 + y / 400 ) % 7;

        if( week == 7 )
        {
            week = 0;
        }

        return week;
        //switch(week)
        //{
        //case 1: weekstr="星期一"; break;
        //case 2: weekstr="星期二"; break;
        //case 3: weekstr="星期三"; break;
        //case 4: weekstr="星期四"; break;
        //case 5: weekstr="星期五"; break;
        //case 6: weekstr="星期六"; break;
        //case 7: weekstr="星期日"; break;
        //}
        //return weekstr;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
#define leap(year) (((year%4==0)&&(year%100!=0))||((year%4==0)&&(year%400==0)))

    uint_t Time::GetLastDayOfMonth( _IN uint_t y, _IN uint_t m )
    {
        m--;
        uint_t month12[ 12 ] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
        if( m >= 12 )
            return 0;

        // 2月
        if( m == 1 )
        {
            // 閏年
            if( leap( y ) )
            {
                return month12[ m ] + 1;
            }
        }
        return month12[ m ];
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    uint32_t Time_GetTimeValue( _IN uint_t year, _IN uint_t month, _IN uint_t day, _IN uint_t hour, _IN uint_t minute, _IN uint_t second )
    {
        _mstring pmtime;
        pfstd::strprintf( pmtime, "%d%d%d%d%d%d", year, month, day, hour, minute, second );
        return atoi( pmtime.c_str() );
    }

    //=============================================================================
    // NAME: <
    // DESC:
    //=============================================================================
    bool Time::operator < ( _IN const Time& t ) const
    {
        return GetTimeValue() < t.GetTimeValue();
    }

    //=============================================================================
    // NAME: >
    // DESC:
    //=============================================================================
    bool Time::operator > ( _IN const Time& t ) const
    {
        uint64_t tm1 = GetTimeValue();
        uint64_t tm2 = t.GetTimeValue();
        return tm1 > tm2;
    }

    //=============================================================================
    // NAME: >
    // DESC:
    //=============================================================================
    bool Time::operator == ( _IN const Time& t ) const
    {
        return GetTimeValue() == t.GetTimeValue();
    }

    //=============================================================================
    // NAME: >
    // DESC:
    //=============================================================================
    bool Time::operator != ( _IN const Time& t ) const
    {
        return GetTimeValue() != t.GetTimeValue();
    }

    //=============================================================================
    // NAME: <
    // DESC:
    //=============================================================================
    bool Time::operator <= ( _IN const Time& t ) const
    {
        return GetTimeValue() <= t.GetTimeValue();
    }

    //=============================================================================
    // NAME: >
    // DESC:
    //=============================================================================
    bool Time::operator >= ( _IN const Time& t ) const
    {
        uint64_t tm1 = GetTimeValue();
        uint64_t tm2 = t.GetTimeValue();
        return tm1 >= tm2;
    }
}