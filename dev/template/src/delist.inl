//
//
// P Container CLASS for template class
//
//
//
//     1.Ring List
//        [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↑_______________________________|
//
//
//             2000/11/01
//
//              Copyright (C) 1999 Pin
//=========================================================================================*/
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <memory>
#include <math.h>
#include <assert.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


namespace pfstl
{
    template <class _Type> class tagNODE
    {
    public:

        tagNODE()  { prev = NULL; next = NULL; }
        ~tagNODE() {};

        tagNODE* prev;
        _Type    value;
        tagNODE* next;
    };


    //////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  Ring List (環狀鏈結串列)
    //

    //static T_Lock g_delistlock;

    template <class _Type> class delist
    {
        //=============================================================================
        //
        template <class _Value> class NodeIter
        {
        public:
            tagNODE<_Value>* node_;
        public:

            NodeIter()                             { node_ = NULL; }
            NodeIter( tagNODE<_Value>* n )         { node_ = n; }
            ~NodeIter()                            { node_ = NULL; }
            bool empty( void )                     { return ( node_ == NULL ) ? true : false; }
            void clear( void )                     { node_ = NULL; }
            operator _Value()                      { node_->value; }
            bool operator==( NodeIter<_Value> i )  { return ( node_ == i.node_ ); }
            bool operator!=( NodeIter<_Value> i )  { return ( node_ != i.node_ ); }
            bool operator==( tagNODE<_Value>  i )  { return ( node_ == i ); }
            void operator++( int )
            {
                if( node_ != NULL )
                {
                    node_ = node_->next;
                }
            }
            void operator--( int )
            {
                if( node_ != NULL )
                {
                    node_ = node_->prev;
                }
            }
            void operator+( int i )
            {
                if( node_ == NULL )
                {
                    return;
                }
                for( int j = 0; j < i; j++ )
                {
                    node_ = node_->next;
                }
            }

            void operator-( int i )
            {
                if( node_ == NULL )
                {
                    return;
                }
                for( int j = 0; j < i; j++ )
                {
                    node_ = node_->prev;
                }
            }

            tagNODE<_Value>* operator=( NodeIter<_Value> i )
            {
                node_ = i.node_;
                return node_;
            }

            _Value operator()( void )
            {
                return ( node_ != NULL ) ? node_->value : NULL;
            }


            tagNODE<_Value>* prev( void )
            {
                return ( node_ != NULL ) ? node_->prev : NULL;
            }

            tagNODE<_Value>* next( void )
            {
                return ( node_ != NULL ) ? node_->next : NULL;
            }
        };
        //=============================================================================

    private:

        typedef tagNODE<_Type> NODE;

        unsigned int  m_size;
        NODE* pHeader, *pTail;


        NODE* NewNode( _Type v )
        {
            //TS_Lock lock(&g_delistlock);
            NODE* newNode = new NODE;
            if( newNode != NULL )
            {
                newNode->prev = newNode;
                newNode->value = v;
                newNode->next = newNode;
            }
            return newNode;
        }

#define ATTACHNODE(f, b)    f->next = b; b->prev = f
#define INSERTNODE(f, n, b) ATTACHNODE(f, n); ATTACHNODE(n, b)
#define DELETENODE(n)       SAFE_DELETE(n)//if(n!=NULL){ n->prev=NULL, n->next=NULL; delete n; } n=NULL
#define GOPREVNODE(n)       n = n->prev
#define GONEXTNODE(n)       n = n->next


        void push_empty( _Type value )
        {
            clear();
            //TS_Lock lock(&g_delistlock);
            NODE* pNewNode = NewNode( value );
            pHeader = pNewNode;
            pTail = pNewNode;
            m_size = 1;
        }

    public:

        typedef  NodeIter<_Type> iterator;

        delist()         { pHeader = NULL; pTail = NULL; m_size = 0; }
        ~delist()        { clear(); }

        //=========================================================================
        //
        //
        //=========================================================================
        bool clear( void )
        {
            if( empty() )
            {
                return true;
            }

            //TS_Lock lock(&g_delistlock);
            NODE* ptr = pTail;
            while( ptr != pHeader )
            {
                GOPREVNODE( pTail );
                DELETENODE( ptr );
                ptr = pTail;
            }
            DELETENODE( pHeader );
            pTail = NULL;
            m_size = 0;
            return true;
        }


        bool DeleteAll( void )
        {
            if( empty() )
            {
                return true;
            }
            //TS_Lock lock(&g_delistlock);
            for( int i = 0; i < m_size; i++ )
            {
                delete operator[]( i );
            }
            clear();
        }

        //=========================================================================
        //
        //
        //=========================================================================
        iterator begin( void )       { return pHeader; }
        iterator end( void )      { return pTail; }
        _Type front( void )       { return pHeader->value; }
        _Type back( void )        { return pTail->value; }

        void push( iterator iter )
        {
            //TS_Lock lock(&g_delistlock);
            INSERTNODE( pTail, iter.node_, pHeader->prev );
            pTail = iter.node_;
        }

        void drop( iterator iter )
        {
            //TS_Lock lock(&g_delistlock);
            ATTACHNODE( iter.prev(), iter.next() );
            if( iter.node_ == pTail )
            {
                pTail = iter.prev();
            }
            else if( iter.node_ == pHeader )
            {
                pHeader = iter.next();
            }
            ATTACHME( iter.node_ );
        }

        void swap( iterator i, iterator j )
        {
            //TS_Lock lock(&g_delistlock);
            if( i.node_ == pHeader )
            {
                pHeader = j.node_;
            }
            else if( j.node_ == pHeader )
            {
                pHeader = i.node_;
            }

            if( i.node_ == pTail )
            {
                pTail = j.node_;
            }
            else if( j.node_ == pTail )
            {
                pTail = i.node_;
            }


            iterator ip = i.node_->prev;
            iterator in = i.node_->next;

            iterator jp = j.node_->prev;
            iterator jn = j.node_->next;


            ATTACHNODE( ip.node_, j.node_ );
            ATTACHNODE( j.node_, in.node_ );

            ATTACHNODE( jp.node_, i.node_ );
            ATTACHNODE( i.node_, jp.node_ );
        }


        void move( iterator i, iterator j )
        {
            //TS_Lock lock(&g_delistlock);
            if( j.node_ == i.node_ )
            {
                return;
            }

            if( i.node_ == pHeader )
            {
                pHeader = i.node_->next;
            }
            else if( i.node_ == pTail )
            {
                pTail = i.node_->prev;
            }

            ATTACHNODE( i.node_->prev, i.node_->next );
            iterator nextins = j.next();

            ATTACHNODE( j.node_, i.node_ );
            ATTACHNODE( i.node_, nextins.node_ );
            if( j.node_ == pTail )
            {
                pTail = i.node_;
            }
        }


        iterator find( unsigned int i )
        {
            if( i >= m_size )
            {
                return NULL;
            }
            //TS_Lock lock(&g_delistlock);
            NODE* lpNode = pHeader;
            for( unsigned int j = 0; j < i; j++ )
            {
                GONEXTNODE( lpNode );
            }
            return lpNode;
        }

        //=========================================================================
        //
        //
        //=========================================================================
        bool empty( void )
        {
            return ( ( m_size == 0 ) && ( pHeader == NULL ) && ( pTail == NULL ) ) ? true : false;
        }

        //=========================================================================
        //
        //
        //=========================================================================        
        unsigned int size( void )   { return m_size; }

        //=========================================================================
        //
        //
        //=========================================================================
        bool is_loop( void )        { return ( ( pTail->next == 0 ) && ( pHeader->prev == 0 ) ) ? true : false; }
        void loop_link( void )      { pTail->next = pHeader; pHeader->prev = pTail; return; }
        void loop_break( void )     { pTail->next = 0; pHeader->prev = 0; return; }

        //=========================================================================
        // group move node
        //
        //=========================================================================
        int GetIndex( iterator iter )
        {
            //TS_Lock lock(&g_delistlock);
            NODE* lpNode = pHeader;
            for( int i = 0; i < m_size; i++ )
            {
                if( iter.node_->value == lpNode->value )
                {
                    return i;
                }
                else
                {
                    GONEXTNODE( lpNode );
                }
            }
            return -1;
        }

        int GetIndex( _Type value )
        {
            //TS_Lock lock(&g_delistlock);
            NODE* pNode = pHeader;
            for( int i = 0; i < size(); i++ )
            {
                if( value == pNode->value )
                {
                    return i;
                }
                GONEXTNODE( pNode );
            }
            return -1;
        }

        //ok not to do any check for over limit
        _Type& operator[]( int index )    { return find( index ).node_->value; }

        //=========================================================================
        //
        //
        //=========================================================================
        void push_back( _Type value ) //ok
        {
            if( empty() )
            {
                push_empty( value );
                return;
            }
            else
            {
                //TS_Lock lock(&g_delistlock);
                NODE* pNewNode = NewNode( value );
                ATTACHNODE( pTail, pNewNode );
                ATTACHNODE( pNewNode, pHeader );
                pTail = pNewNode;
                m_size++;
            }
        }


        //****************maybe memory leak if no delete before pop*******************
        _Type pop_front()
        {
            //TS_Lock lock(&g_delistlock);
            _Type value;
            if( !empty() )
            {
                value = pHeader->value;
                NODE* pH = pHeader;
                GONEXTNODE( pHeader );
                ATTACHNODE( pTail, pHeader );
                DELETENODE( pH );
                m_size--;
            }
            return value;
        }

        _Type pop_back()
        {
            //TS_Lock lock(&g_delistlock);
            _Type value;
            if( !empty() )
            {
                value = pTail->value;
                NODE* pT = pTail;
                GOPREVNODE( pTail );
                ATTACHNODE( pTail, pHeader );
                DELETENODE( pT );
                m_size--;
            }
            return value;
        }


        //=========================================================================
        //
        //
        //=========================================================================
        bool set( _Type value, int index )
        {
            if( empty() )
            {
                return false;
            }

            //TS_Lock lock(&g_delistlock);
            NODE* pNewNode = pHeader;
            for( int i = 0; i < index; i++ )
            {
                if( pNewNode == pTail )
                {
                    return false;
                }

                pNewNode = pNewNode->next;
            }
            pNewNode->value = value;
            return true;
        }

        bool insert( _Type value, int index ) //ok
        {
            if( empty() )
            {
                return false;
            }

            iterator iter = find( index - 1 );

            //TS_Lock lock(&g_delistlock);
            if( iter.empty() )
            {
                return false;
            }
            NODE* pNewNode = NewNode( value );
            NODE* nextnode = iter.node_->next;
            ATTACHNODE( iter.node_, pNewNode );
            ATTACHNODE( pNewNode, nextnode );
            m_size++;
            return true;
        }

        void erase( iterator& iter )
        {
            //TS_Lock lock(&g_delistlock);
            NODE* DelNode = iter.node_;
            if( DelNode == pHeader )
            {
                GONEXTNODE( pHeader );
            }
            if( DelNode == pTail )
            {
                GOPREVNODE( pTail );
            }

            ATTACHNODE( DelNode->prev, DelNode->next );
            m_size--;
            if( m_size == 0 )
            {
                iter.node_ = NULL;
                pHeader = NULL;
                pTail = NULL;
            }
            else
            {
                iter++;
            }
            delete DelNode;
        }

        void erase( int index )
        {
            if( empty() )
            {
                return;
            }
            erase( find( index - 1 ) );
        }


        _Type Get( int Index ) //ok
        {
            if( empty() )
            {
                push_back( 0 );
            }

            NODE* ptr = pHeader;
            for( int i = 0; i < Index; i++ )
            {
                if( ptr == pTail )
                {
                    push_back( 0 );
                    continue;
                }
                else
                {
                    GONEXTNODE( ptr );
                    continue;
                }
            }
            return operator[]( 0 );
        }
    };
    // End RLIST
    /////////////////////////////////////////////////////////////////////////////////////////////////
}