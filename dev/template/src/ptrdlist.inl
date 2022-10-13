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

#include <list>

template <class _Type> class ptrdlist: public std::list<_Type>
{
    unsigned int                        index_;

    typedef std::list<_Type>             list_type;
    typedef typename list_type::iterator list_iterator_type;
    list_iterator_type                   Iter_;

public:

    ptrdlist()  { index_ = 0; }
    ~ptrdlist() { list_type::clear(); }

    //=========================================================================
    // group move node
    //
    //=========================================================================
    bool operator ++( int )
    {
        if( list_type::empty() )
        {
            return false;
        }

        Iter_++;

        if( Iter_ == list_type::end() )
        {
            index_ = 0;
            Iter_ = list_type::begin();
        }
        else
        {
            index_++;
        }
        return true;
    }

    bool operator --( int )
    {
        if( list_type::empty() )
        {
            return false;
        }
        if( Iter_ == list_type::begin() )
        {
            Iter_ = list_type::end();
            index_ = list_type::size() - 1;
        }
        else
        {
            index_--;
        }
        Iter_--;
        return true;
    }

    bool GoBegin( void )
    {
        if( list_type::empty() )
        {
            return false;
        }
        Iter_ = list_type::begin();
        index_ = 0;
        return true;
    }

    bool GoLast( void )
    {
        if( list_type::empty() )
        {
            return false;
        }
        Iter_ = list_type::end();
        Iter_--;
        index_ = list_type::size() - 1;
        return true;
    }

    bool GoTo( unsigned int Index )
    {
        if( Index >= list_type::size() )
        {
            return false;
        }
        Iter_ = list_type::begin();
        for( unsigned int i = 0; i < Index; i++ )
        {
            Iter_++;
        }
        index_ = Index;
        return true;
    }

    bool isBegin( void )
    {
        if( list_type::empty() )
        {
            return true;
        }

        return ( Iter_ == list_type::begin() ) ? true : false;
    }

    bool isLast( void )
    {
        if( list_type::empty() )
        {
            return true;
        }

        list_iterator_type iter = list_type::end();
        iter--;
        return ( Iter_ == iter ) ? true : false;
    }

    _Type& operator()( void )
    {
        if( !list_type::empty() )
        {
            //int ss=size();
            //if( Iter_!=end() )
            return ( *Iter_ );
        }
        Iter_ = list_type::begin();
        return ( *Iter_ );
    }

    _Type& operator[]( unsigned int Index )
    {
        if( Index >= list_type::size() )
        {
            return ( *list_type::begin() );
        }

        list_iterator_type iter = list_type::begin();
        for( unsigned int i = 0; i < Index; i++ )
        {
            iter++;
        }
        return ( *iter );
    }

    void push_back( const _Type& _Val )
    {
        list_type::push_back( _Val );
        if( list_type::size() == 1 )
        {
            Iter_ = list_type::begin();
            index_ = 0;
        }
    }

    int GetIndex( void )
    {
        return index_;
    }
};