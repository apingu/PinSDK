//
//
//    https://www.itread01.com/content/1549013595.html
//
//
//
//
//
//
//

//01. == C 系統文件/C++ 系統文件
#include <assert.h>
#include <vector>
#include <iterator>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "string_arithmetic.h"


namespace string_arithmetic
{
    std::vector<_tstring> split( const _tstring& s )
    {
        std::vector<_tstring> ret;
        if ( s.length() == 0 ) return ret;
        // bracket
        if ( s.at( 0 ) == _T( '(' ) )
        {
            int cnt = 0;
            for ( int i = 0; i < s.length(); i++ )
            {
                if ( s.at( i ) == _T( '(' ) ) cnt++;
                else if ( s.at( i ) == _T( ')' ) ) cnt--;
                if ( cnt == 0 )
                {
                    ret.push_back( s.substr( 0, i + 1 ) );
                    auto r = split( s.substr( i + 1 ) );
                    std::copy( r.begin(), r.end(), std::back_inserter( ret ) );
                    break;
                }
            }
        }
        // number
        else if ( ( _T( '0' ) <= s.at( 0 ) && s.at( 0 ) <= _T( '9' ) ) || s.at( 0 ) == _T( '.' ) )
        {
            ret.push_back( _T( "" ) );
            for ( int i = 0; i < s.length(); i++ )
            {
                if ( ( _T( '0' ) <= s.at( i ) && s.at( i ) <= _T( '9' ) ) || s.at( i ) == _T( '.' ) )
                    ret.back() += s.at( i );
                else
                {
                    auto r = split( s.substr( i ) );
                    std::copy( r.begin(), r.end(), std::back_inserter( ret ) );
                    break;
                }
            }
        }
        // operator
        else
        {
            ret.push_back( s.substr( 0, 1 ) );
            auto r = split( s.substr( 1 ) );
            std::copy( r.begin(), r.end(), std::back_inserter( ret ) );
        }
        return ret;
    }

    // 支援s中含有括號、小數、整數、加號、減號
    double Operator( _tstring s )
    {
        auto parts = split( s );
        if ( parts.size() == 0 )
            return 0;
        if ( parts.size() == 1 )
        {
            // 如果給定字串是括號括起來的表示式
            if ( s.at( 0 ) == _T( '(' ) )
                return Operator( s.substr( 1, s.length() - 2 ) );
            else
            {
                assert( ( _T( '0' ) <= s.at( 0 ) && s.at( 0 ) <= _T( '9' ) ) || s.at( 0 ) == _T( '.' ) );
                // stod函式是c++11標準支援的，需要編譯器不能太老
                return std::stod( s );
            }
        }
        double ret = Operator( parts.at( 0 ) );
        for ( int i = 1; i < parts.size(); i += 2 )
        {
            assert( i + 1 < parts.size() );
            if ( parts.at( i ) == _T( "+" ) )
                ret += Operator( parts.at( i + 1 ) );
            else if ( parts.at( i ) == _T( "-" ) )
                ret -= Operator( parts.at( i + 1 ) );
            else
                assert( false );
        }
        return ret;
    }


    //int main( int argc, char *argv[] )
    //{
    //    int n; std::cin >> n;
    //    std::string s;
    //    std::cin >> s;
    //    double ans = calc( s );
    //    if ( s.find( "." ) == std::string::npos )
    //        std::cout << (long long)ans << std::endl;
    //    else
    //        printf( "%.6f\n", ans );
    //    return 0;
    //}

}
