#ifndef _Tarray_H_
#define _Tarray_H_


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


template <class _Type>
class TArray
{
private:

    typedef _Type*  Tarray;

    int             xSize;
    int             ySize;
    Tarray*         m_array;


public:

    TArray()
    {
        m_array = 0; xSize = 0; ySize = 0;
    };
    ~TArray()
    {
        release();
    };


    inline set_value( _Type value )
    {
        for( int i = 0 ;i < ySize ;i++ )
            for( int j = 0 ;j < xSize ;j++ )
                m_array[ i ][ j ] = value;
    }

    inline release( void )
    {
        if( m_array != 0 )
        {
            for( int i = 0 ;i < ySize ;i++ )
                delete [] m_array[ i ];
            delete [] m_array;  
            m_array = 0; 
            xSize = 0;
            ySize = 0;
        }
    }

    inline void allot( int cols,int rows )
    {
        release();
        xSize = cols;
        ySize = rows;

        m_array = new Tarray[ rows ];
        for( int i = 0 ;i < rows ;i++ )
            m_array[ i ] = new _Type[ cols ];
    }

    inline void reallot( int x,int y )
    {
    }

    inline _Type& operator()( int col,int row )
    {
        return m_array[ row ][ col ];
    }

    void operator=( const  TArray<_Type>& array )
    {
        for( int j = 0;j < ySize;j++ )
            for( int i = 0;i < xSize;i++ )
                m_array[ i ][ j ] = array.m_array[ i ][ j ];
    }


    inline int col_size( void )
    {
        return xSize;
    }

    inline int row_size( void )
    {
        return ySize;
    }

    inline bool empty( void )
    {
        if( ( xSize == 0 ) && ( ySize == 0 ) )
            return true;
        return false;
    }
};

#endif