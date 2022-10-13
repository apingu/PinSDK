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
#pragma once

//01. == C 系統文件/C++ 系統文件 系統include
#include <vector>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <graphic2d.h>
#include <pf-stdlib.h>
//

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件 自製include


namespace xlutility
{

    // 從1, 1
    class Cell: public LG::Point
    {
    public:

        Cell(){};
        Cell( _IN int col, _IN int row ){ x = col; y = row; };
        Cell( _IN const tchar_t* address ){ SetAddress( address ); };
        ~Cell(){};

        // "A1" to (1, 1)
        inline void SetAddress( _IN const tchar_t* address )
        {
            int ii = 0, jj, colVal = 0;
            while( address[ ii++ ] >= _T( 'A' ) ) {};
            ii--;
            // ii now points to first character of row address
            for( jj = 0; jj < ii; jj++ ) colVal = 26 * colVal + _totupper( address[ jj ] ) - _T( 'A' ) + 1;
            x = colVal;
            y = _ttoi( address + ii );
        }

        // (1, 1) to "A1"
        inline _tstring GetAddress()
        {
            int dividend = x;
            _tstring columnName;
            int modulo;

            while( dividend > 0 )
            {
                modulo = ( dividend - 1 ) % 26;
                _tstring letter;
                letter = ( char )( 65 + modulo );
                columnName = letter + columnName;
                dividend = ( int )( ( dividend - modulo ) / 26 );
            }
            return columnName + _pf_itot( y );
        }
    };


    class Range: public LG::Rect
    {
    public:

        Range(){};
        Range( _IN const tchar_t* address ){ SetAddress( address ); };
        ~Range(){};

        inline void SetAddress( _IN const tchar_t* address )
        {
            _tcsvector addrs = _pf_tstring_split( address, _T(":") );

            Cell c1, c2;
            if( addrs.size() == 1 )
            {
                c1.SetAddress( addrs[ 0 ].c_str() );
                c2.SetAddress( addrs[ 0 ].c_str() );
            }
            else if( addrs.size() == 2 )
            {
                c1.SetAddress( addrs[ 0 ].c_str() );
                c2.SetAddress( addrs[ 1 ].c_str() );
            }
            p_l_t( c1 );
            p_r_b( c2 );
        }

        inline _tstring GetAddress()
        {
            _tstring address;

            Cell c1( left, top );
            Cell c2( right, bottom );
            return c1.GetAddress() + _T(":") + c2.GetAddress();
        }
    };
}