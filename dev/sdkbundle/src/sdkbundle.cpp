#include "sdkbundle.h"
#include <pf-stdlib.h>

#include <stdlib.h>
#include <time.h>      

namespace sdkbundle
{
    namespace
    {
        _tstring g_version;
        _tstring g_build_time;
    }

//#define DATE_AS_INT( dd )  (((YEAR - 2000) * 12 + MONTH) * 31 + DAY)

    const tchar_t* GetVersion()
    {
        if ( g_version.empty() )
        {
            pftml::Time time( __TIMESTAMP__ );
#ifdef _UNICODE
            _pf_tstrprintf( g_version, _T( "%04d%02d.%02d%02d_u" ), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour() );
#else
            _pf_tstrprintf( g_version, _T( "%04d%02d.%02d%02d" ),   time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour() );
#endif
        }
        return g_version.c_str();
    }

    const tchar_t* GetBuildTime()
    {
        if ( g_build_time.empty() )
        {
#ifdef _UNICODE
            _pf_tstrprintf( g_build_time, _T( "%s %s_u" ), _pf_strtotcs( __DATE__ ).c_str(), _pf_strtotcs( __TIME__ ).c_str() );
#else
            _pf_tstrprintf( g_build_time, _T( "%s %s" ), _pf_strtotcs( __DATE__ ).c_str(), _pf_strtotcs( __TIME__ ).c_str() );
#endif
        }

        return g_build_time.c_str();
    }
}