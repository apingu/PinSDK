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

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


namespace pfstl
{
    template <class _Type> class arraymap
    {
    private:

        unsigned int  xlength_;
        unsigned int  ylength_;
        _Type*        buffer_;


    public:

        arraymap()  { buffer_ = NULL; xlength_ = 0; ylength_ = 0; };
        ~arraymap() { clear(); }


        inline void set_element( _Type value )
        {
            for( unsigned int i = 0; i < xlength_ * ylength_; i++ )
            {
                buffer_[ i ] = value;
            }
        }

        inline void zero_value( void )
        {
            memset( buffer_, 0, xlength_ * ylength_ * sizeof( _Type ) );
        }

        inline void clear( void )
        {
            if( buffer_ != 0 )
            {
                //release_element();
                delete[] buffer_;
                buffer_ = NULL;
                xlength_ = 0;
                ylength_ = 0;
            }
        }

        inline void allot( unsigned int cols, unsigned int rows )
        {
            clear();
            xlength_ = cols;
            ylength_ = rows;

            buffer_ = new _Type[ cols * rows ];
        }

        inline void reallot( unsigned int cols, unsigned int rows )
        {
            if( buffer_ != NULL )
            {
                if( ( xlength_ == cols ) && ( ylength_ == rows ) )
                {
                    return;
                }
            }
            allot( cols, rows );
        }

        inline void put( _Type t, unsigned int x, unsigned int y )
        {
            if( ( buffer_ == 0 ) || ( x > xlength_ ) || ( y > ylength_ ) )
            {
                return;
            }
            buffer_[ ( y * xlength_ ) + x ] = t;
            return;
        }

        inline _Type& operator()( unsigned int x, unsigned int y )
        {
            if( buffer_ == 0 )
            {
                buffer_ = new _Type[ xlength_ * ylength_ ];
            }
            return buffer_[ ( y * xlength_ ) + x ];
        }

        void operator=( const  arraymap<_Type>& array )
        {
            allot( array.col_size(), array.row_size() );
            for( int j = 0; j < xlength_ * ylength_; j++ )
            {
                buffer_[ j ] = array.buffer_[ j ];
            }
            return;
        }

        inline unsigned int col_size( void )
        {
            return xlength_;
        }

        inline unsigned int row_size( void )
        {
            return ylength_;
        }

        inline bool empty( void )
        {
            if( ( xlength_ == 0 ) && ( ylength_ == 0 ) && ( buffer_ == NULL ) )
            {
                return true;
            }
            return false;
        }

        inline _Type* operator()( void )
        {
            return buffer_;
        }
    };
};