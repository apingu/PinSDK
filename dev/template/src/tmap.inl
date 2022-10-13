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

namespace pfstl
{
    template <class _TypeI, class _TypeV> class tmap
    {
        struct column
        {
            _TypeI      index;
            _TypeV      value;
        };

        std::vector<column*> column_;

    public:

        tmap()
        {
        }

        ~tmap()
        {
            clear();
        }

        inline void clear( void )
        {
            for( unsigned int i = 0; i < column_.size(); i++ )
            {
                delete column_[ i ];
            }
            column_.clear();
        }

        inline _TypeV& operator ()( int index )
        {
            return column_[ index ]->value;
        }

        inline _TypeV& operator []( _TypeI index )
        {
            for( int i = 0; i < column_.size(); i++ )
            {
                if( column_[ i ]->index == index )
                {
                    return column_[ i ]->value;
                }
            }
            return NULL;
        }

        inline unsigned int size( void )
        {
            return column_.size();
        }

        inline void push_back( _TypeI index, _TypeV value )
        {
            column* c = new column;
            c->index = index;
            c->value = value;
            column_.push_back( c );
            return;
        }
    };
}