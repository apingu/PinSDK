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
//              Copyright (C) 1999 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>


template <class _Type> class DepthQueue: public std::vector<_Type>
{
    int depth_;

public:

    DepthQueue( void )
    {
        depth_ = 0;
    }

    //DepthQueue ( int d ) { depth_ = depth; }
    ~DepthQueue( void )
    {
        clear();
    }

    void depth( int d )
    {
        depth_ = d;
    }

    _Type push( const _Type& x )
    {
        push_back( x );
        if( size() > depth_ )
        {
            return pop();
        }
        return NULL;
    }

    _Type pop( void )
    {
        _Type e = operator[]( 0 );
        erase( begin() );
        return e;
    }
};