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

    template <class _Type> class array2d
    {
    private:
    
        typedef _Type*  array2d_t;
        
        int        xlength_;
        int        ylength_;
        array2d_t* buffer_;
        
    public:
    
        array2d()
        {
            buffer_ = NULL;
            xlength_ = 0;
            ylength_ = 0;
        };
        
        ~array2d()
        {
            clear();
        }
        
        
        inline void set_value( _Type value )
        {
            for ( int i = 0; i < ylength_; i++ )
            {
                for ( int j = 0; j < xlength_; j++ )
                {
                    buffer_[i][j] = value;
                }
            }
        }
        
        inline void release_element( void )
        {
            for ( int j = 0; j < ylength_; j++ )
                for ( int i = 0; i < xlength_; i++ )
                {
                    delete buffer_[j][i];
                }
        }
        
        inline void clear( void )
        {
            if ( buffer_ != 0 )
            {
                for ( int i = 0; i < ylength_; i++ )
                {
                    delete[] buffer_[i];
                }
                
                delete[] buffer_;
                buffer_ = NULL;
                xlength_ = 0;
                ylength_ = 0;
            }
        }
        
        inline void allot( int cols, int rows )
        {
            clear();
            xlength_ = cols;
            ylength_ = rows;
            buffer_ = new array2d_t[rows];
            
            for ( int i = 0; i < rows; i++ )
            {
                buffer_[i] = new _Type[cols];
            }
        }
        
        inline void reallot( int x, int y )
        {
        }
        
        inline void put( _Type t, int x, int y )
        {
            if ( ( buffer_ == 0 ) || ( col > xlength_ ) || ( row > ylength_ ) )
            {
                return;
            }
            
            buffer_[y][x] = t;
            return;
        }
        
        inline _Type& operator()( int x, int y )
        {
            return buffer_[y][x];
        }
        
        void operator=( const array2d<_Type>& array )
        {
            allot( array.col_size(), array.row_size() );
            
            for ( int j = 0; j < ylength_; j++ )
            {
                for ( int i = 0; i < xlength_; i++ )
                {
                    buffer_[i][j] = array.buffer_[i][j];
                }
            }
            
            return;
        }
        
        inline int col_size( void )
        {
            return xlength_;
        }
        
        inline int row_size( void )
        {
            return ylength_;
        }
        
        inline bool empty( void )
        {
            if ( ( xlength_ == 0 ) && ( ylength_ == 0 ) && ( buffer_ == NULL ) )
            {
                return true;
            }
            
            return false;
        }
    };
};