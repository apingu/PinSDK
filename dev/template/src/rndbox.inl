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
//
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>


namespace pfstl
{
    template <class _Type> class rndbox
    {
        struct elem
        {
            _Type type;
            int   count;
        };

        std::vector<elem>  elem_vec_;
        std::vector<elem>  elem_bak_;

    public:

        rndbox()   { srand( time( NULL ) ); }
        ~rndbox()  {}

        void push_back( _Type t, int c )
        {
            if( c <= 0 )
            {
                return;
            }
            elem e;
            e.type = t;
            e.count = c;
            elem_vec_.push_back( e );
            elem_bak_.push_back( e );
        }

        void clear()
        {
            elem_vec_.clear();
            elem_bak_.clear();
        }

        unsigned int size()
        {
            int c = 0;
            for( int i = 0; i < elem_vec_.size(); i++ )
            {
                c += elem_vec_[ i ].count;
            }
            return c;
        }

        bool empty()
        {
            return elem_vec_.empty();
        }

        bool pop_random( _Type& rt )
        {
            if( elem_vec_.empty() )
            {
                return false;
            }
            std::random_shuffle( elem_vec_.begin(), elem_vec_.end() );
            int p = rand() % size();
            int c = 0;
            for( unsigned int i = 0; i < elem_vec_.size(); i++ )
            {
                c += elem_vec_[ i ].count;
                if( p < c )
                {
                    _Type t = elem_vec_[ i ].type;
                    elem_vec_[ i ].count--;
                    if( elem_vec_[ i ].count == 0 )
                    {
                        elem_vec_.erase( elem_vec_.begin() + i );
                    }
                    if( elem_vec_.empty() )
                    {
                        elem_vec_ = elem_bak_;
                    }
                    rt = t;
                    return true;
                }
            }
            return false;
        }

        bool get_random( _Type& rt )
        {
            if( elem_vec_.empty() )
            {
                return false;
            }
            std::random_shuffle( elem_vec_.begin(), elem_vec_.end() );
            int p = rand() % size();
            int c = 0;
            for( int i = 0; i < elem_vec_.size(); i++ )
            {
                c += elem_vec_[ i ].count;
                if( p < c )
                {
                    rt = elem_vec_[ i ].type;
                    return true;
                }
            }
            return false;
        }

        bool operator==( const rndbox& t ) const
        {
            if( t.elem_bak_.size() != elem_bak_.size() )
            {
                return false;
            }

            int c = 0;
            for( int i = 0; i < t.elem_bak_.size(); i++ )
            {
                if( t.elem_bak_[ i ].type == elem_bak_[ i ].type )
                {
                    if( elem_bak_[ i ].count != elem_bak_[ i ].count )
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        bool operator!=( const rndbox& t )
        {
            return !operator==( t );
        }

        rndbox& operator=( const rndbox& t )
        {
            clear();
            for( int i = 0; i < t.elem_bak_.size(); i++ )
            {
                push_back( t.elem_bak_[ i ].type, t.elem_bak_[ i ].count );
            }
            return *this;
        }
    };

}