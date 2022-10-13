#include <stdio.h>

#include "template/catalog.inl"

#include "sdkbundle.h"

int main( int argc, char *argv[] )
{

    pfstl::_catalog< _tstring, _tstring, int > test;

    test.insert( _T( "AA" ), _T( "BB" ), 3 );

    pfstl::_catalog< _tstring, _tstring, int >::iterator iter = test.find( _T( "AA" ), _T( "BB" ) );
    if( iter != test.end() )
    {
        int hhh = 0;
    }

    return 1; // delete if dont need Qt
}
