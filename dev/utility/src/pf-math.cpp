

//01. == C 系統文件/C++ 系統文件
#include <math.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件
#include "pf-math.h"

//04. == 此專案內.h 文件




namespace pfmat
{
    double DecimalPoint( _IN double value, _IN int index )
    {
        // 
        int    base = pow( 10, index );
        double offs = 5 / pow( 10, ( index - 1 ) );
        if ( value < 0 )
        {
            offs = 0 - offs;
        }
        return  (double)( (int)( ( value * base ) + offs ) / ( (double)base ) );
    }
}