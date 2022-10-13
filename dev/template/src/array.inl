//  Array.h  array template class
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
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


namespace pfstl
{
    template <class _Type> class array
    {
    private:
    
        unsigned int length_;
        _Type*       buffer_;
        unsigned int buffend_;
        
        
    public:
    
        array()                      { buffer_ = NULL; length_ = 0; buffend_ = 0;}
        array( int count )           { buffer_ = NULL; length_ = 0; allot( count ); buffend_ = 0; }
        array( _Type* a, int count ) { buffer_ = a; length_ = 0; allot( count ); buffend_ = count - 1; }
        ~array()                     { clear(); }
        
        
        operator  void* () { return buffer_; }
        
        /*
        inline _Type* operator() (void)
        {
         return buffer_;
        }
        */
        
        inline _Type& operator[]( unsigned int number )
        {
            if ( ( buffer_ == NULL ) || ( number > buffend_ ) )
            {
                assert( 0 || "overflow array !!" );
                return;
            }
            
            return buffer_[number];
        }
        
        inline void clone( array<_Type>* a )
        {
            a->length_    = length_;
            a->buffer_   = buffer_;
            a->buffend_ = buffend_;
        }
        
        inline void operator=( const array<_Type>& a )
        {
            if ( buffer_ != NULL )
            {
                delete [] buffer_;
            }
            
            // copy null array
            if ( ( a.length_ == 0 ) && ( a.buffer_ == NULL ) )
            {
                length_ = 0;
                buffer_ = NULL;
                buffend_ = 0;
                return;
            }
            
            length_ = a.length_;
            buffer_ = new _Type[ length_ ];
            memcpy( buffer_, a.buffer_, ( sizeof( _Type ) * length_ ) );
            buffend_ = a.buffend_;
            return;
        }
        
        inline _Type& back( void )
        {
            if ( buffer_ == NULL )
            {
                assert( 0 || "empty array !!" );
                return;
            }
            
            return buffer_[ buffend_ ];
        }
        
        inline void allot( int count )
        {
            clear();
            
            if ( count == 0 )
            {
                return;
            }
            
            length_ = count;
            buffer_ = new _Type[ count ];
            memset( buffer_, 0, sizeof( _Type ) * length_ );
            buffend_ = 0;
            return;
        }
        
        inline void reallot( int count )
        {
            if ( buffer_ == NULL )
            {
                allot( count );
            }
            else
            {
                _Type*  temp = new _Type[ count ];
                memset( temp, 0, count );
                int new_size = __min( count, ( int )length_ );
                memcpy( temp, buffer_, ( sizeof( _Type ) * new_size ) );
                clear();
                buffer_ = temp;
                length_ = count;
                //delete [] m_Temp;
            }
            
            return;
        }
        
        inline int add( int count = 1 )
        {
            int newsize  = count + length_;
            reallot( newsize );
            return newsize;
        }
        
        inline void push_back( _Type value )
        {
            add();
            buffer_[ length_ - 1 ] = value;
            return;
        }
        
        inline void set( _Type value )
        {
            for ( unsigned int i = 0 ; i < length_ ; i++ )
            {
                buffer_[ i ] = value;
            }
            
            return;
        }
        
        inline void zero( void )
        {
            memset( buffer_, 0, length_ );
        }
        
        inline void clear( void )
        {
            if ( !empty() )
            {
                delete [] buffer_;
                length_ = 0;
                buffer_ = NULL;
                buffend_ = 0;
            }
            
            return;
        }
        
        inline void release_element( void )
        {
            for ( unsigned int i = 0 ; i < length_ ; i++ )
            {
                if ( buffer_[ i ] != 0 )
                {
                    delete buffer_[ i ];
                    buffer_[ i ] = 0;
                }
            }
            
            return;
        }
        
        inline unsigned int size( void )
        {
            return length_;
        }
        
        inline bool empty( void )
        {
            return ( ( length_ == 0 ) && ( buffer_ == 0 ) ) ? true : false;
        }
        
        inline void move_to( const array<_Type>& parray, int from, int dest, int size )
        {
            int copysize  = size;
            
            if ( ( parray.length_ - dest ) < size )
            {
                copysize = ( parray.length_ - dest );
            }
            
            memcpy( parray.buffer_ + ( sizeof( _Type ) * dest ), buffer_ + ( sizeof( _Type ) * from ), copysize * sizeof( _Type ) );
            return;
        }
        
        inline void lean_R( int len, int start = 0 )
        {
            memcpy( buffer_ + sizeof( _Type ) * ( length_ - len ), buffer_ + sizeof( _Type ) * start, sizeof( _Type ) * len );
            return;
        }
        
        inline bool is_in_array( _Type value )
        {
            for ( unsigned int i = 0; i < length_; i++ )
            {
                if ( buffer_[ i ] == value )
                {
                    return true;
                }
            }
            
            return false;
        }
        
        inline void swap( int i, int j )
        {
            //Array<_Type> temp;
            _Type   temp;
            memcpy( temp, buffer_[ i ], sizeof( _Type ) );
            memcpy( buffer_[ i ], buffer_[ j ], sizeof( _Type ) );
            memcpy( buffer_[ j ], temp, sizeof( _Type ) );
            return;
        }
        
        inline void qSort( int ( * compare )( const void* elem1, const void* elem2 ) )
        {
            qsort( ( void* ) buffer_, ( size_t ) length_, sizeof( _Type ), compare );
            return;
        }
        
        inline void stable_Sort( int ( * compare )( const void* elem1, const void* elem2 ) )
        {
            std::stable_sort( buffer_, buffer_ + ( length_ - 1 ), compare );
            return;
        }
        
        
        inline void pop_top( int index )
        {
            _Type buffer = buffer_[ index ];
            
            for ( int i = ( length_ - 1 ) ; i > index; i++ )
            {
                buffer_[ i - 1 ] = buffer_[ i ];
            }
            
            buffer_[ length_ - 1 ] = buffer;
            return;
        }
        
        inline void dearse( int index )
        {
            array<_Type> temp;
            temp = buffer_;
            delete [] buffer_;
            allot( length_ - 1 );
            
            for ( int i = 0; i < index; i++ )
            {
                buffer_[ i ] = temp[ i ];
            }
            
            for ( int i = index + 1; i < length_; i++ )
            {
                buffer_[ i - 1 ] = temp[ i ];
            }
            
            return;
        }
    };
    
};//namespace PT