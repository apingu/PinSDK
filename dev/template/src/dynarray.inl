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
#include <vector>
#include <iterator>
#include <algorithm>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


namespace pfstl
{
    template <class _Type> class dynarray: public std::vector<_Type>
    {

    public:

        dynarray() { };
        dynarray( int count ) { allot( count ); };
        ~dynarray() { clear(); };

        inline void operator()( int count )
        {
            allot( count );
        }

        inline _Type operator()( void )
        {
            //if( lptr == 0 )
            return *begin();
        }

        /*
        inline void operator ++(void)
        {
         lptr++;
        }
        inline void operator --(void)
        {
         lptr--;
        }
        inline void is_tail(void)
        {
         if(lptr == ::end())
          return true;
         return false;
        }
           */
        inline void operator =( const  dynarray<_Type>& array )
        {
            clear();
            for( int i = 0; i < array.size(); i++ )
            {
                push_back( array[ i ] );
            }
            return;
        }

        inline int find( _Type cmp )
        {
            for( unsigned int i = 0; i < size(); i++ )
            {
                if( cmp == operator[]( i ) )
                {
                    return i;
                }
            }
            return -1;
        }

        inline _Type& end( void )
        {
            return ::operator[]( end() );
        }

        inline void allot( int count )
        {
            resize( count );
        }


        inline void dinsert( int addr, _Type value )
        {
            insert( begin() + addr, value );
            return;
        }

        inline void derase( int index )
        {
            erase( begin() + index );
            return;
        }

        inline void set( int index, _Type value )
        {
            if( index < size() )
            {
                erase( begin() + index );
            }

            insert( begin() + index, value );
            return;
        }

        /*
        inline void DeleteAll(void)
        {
         for(unsigned int i = 0 ; i < size() ; i++)
          delete operator[](i) ;
         //clear();
        }
        */

        inline bool is_in_array( _Type value )
        {
            for( unsigned int i = 0; i < size(); i++ )
            {
                if( ::operator[]( i ) == value )
                {
                    return true;
                }
            }
            return false;
        }

        inline int find_index( _Type index )
        {
            for( int i = 0; i < size(); i++ )
            {
                if( node == index )
                {
                    return i;
                }
            }
            return -1;
        }

        inline void swap( int i, int j )
        {
            swap( begin() + i, begine() + j );
        }

        inline void qSort( int( *compare )( const void* elem1, const void* elem2 ) )
        {
            int  sortsize = size();
            _Type*  array = new _Type[ sortsize ];

            for( int i = 0; i < sortsize; i++ )
            {
                array[ i ] = operator[]( i );
            }

            qsort( ( void* )array, ( size_t )size(), sizeof( _Type ), compare );
            clear();

            for( int j = 0; j < sortsize; j++ )
            {
                push_back( array[ j ] );
            }

            delete[] array;
        }



        inline void s_sort( int( *compare ) ( _Type elem1, _Type elem2 ) )
        {
            //std::stable_sort(begin(), end(), compare);
        }
    };
};