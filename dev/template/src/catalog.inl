//
//
//
//
//
//
//
//    [ I ][ I ] => T
//    2 維索引
//
#pragma once
//01.== C 系統文件/C++ 系統文件
#include <map>
#include <string>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <pf-def.h>


namespace pfstl
{

    template <class _C, class _R, class _T> class _catalog: public std::map< _C, std::map< _R, _T > >
    {
        typename std::map< _C, std::map< _R, _T > >::iterator iter_;
        std::map< _R, _T >                                    emap_;

    public:

        typedef typename std::map< _R, _T >::iterator         iterator;

        _catalog(){};
        ~_catalog(){}

        void insert( _C c, _R r, _T t )
        {
            typename std::map< _C, std::map< _R, _T > >::iterator iter00 = std::map< _C, std::map< _R, _T > >::find( c );
            if( iter00 == std::map< _C, std::map< _R, _T > >::end() )
            {
                std::map< _R, _T > nt;
                nt.insert( typename std::map< _R, _T >::value_type( r, t ) );
                std::map< _C, std::map< _R, _T > >::insert( typename std::map< _C, std::map< _R, _T > >::value_type( c, nt ) );
            }
            else
            {
                typename std::map< _R, _T >::iterator iter01 = iter00->second.find( r );
                if( iter01 == iter00->second.end() )
                {
                    iter00->second.insert( typename  std::map< _R, _T >::value_type( r, t ) );
                }
                else
                {
                    iter01->second = t;
                }
            }
        }

        void erase( _C c, _R r )
        {
            typename std::map< _C, std::map< _R, _T > >::iterator iter00 = std::map< _C, std::map< _R, _T > >::find( c );
            if( iter00 != std::map< _C, std::map< _R, _T > >::end() )
            {
                iter00->erase( r );
            }
        }

        typename _catalog::iterator find( _C c, _R r )
        {
            iter_ = std::map< _C, std::map< _R, _T > >::find( c );
            if( iter_ != std::map< _C, std::map< _R, _T > >::end() )
            {
                //iter_ = iter;
                return iter_->second.find( r );
            }
            return end();
        }

        typename _catalog::iterator begin( _C c )
        {
            iter_ = std::map< _C, std::map< _R, _T > >::find( c );
            if( iter_ != std::map< _C, std::map< _R, _T > >::end() )
            {
                return iter_->second.begin();
            }
            return end();
        }

        typename _catalog::iterator end()
        {
            if( iter_ != std::map< _C, std::map< _R, _T > >::end() )
            {
                //iter_ = iter;
                return iter_->second.end();
            }
            //iter_->seocnd.end();
            //return iter_.end();
            return emap_.end();
        }

        //typename _catalog::iterator end( _C c )
        //{
        //    typename std::map< _C, std::map< _R, _T > >::iterator iter = std::map< _C, std::map< _R, _T > >::find( c );
        //    if( iter != std::map< _C, std::map< _R, _T > >::end() )
        //    {
        //        return iter->second.end();
        //    }
        //    return end();
        //}
    };

    #ifndef _mcatalog
        template <class T> using _mstring_catalog = _catalog<_mstring, _mstring, T>;
        #define _mcatalog        _mstring_catalog
    #endif

    #ifndef _wcscatalog
        template <class T> using _wstring_catalog = _catalog<_wstring, _wstring, T>;
        #define _wcatalog        _wstring_catalog
    #endif
    
    #ifndef _tcscatalog
        template <class T> using _tstring_catalog = _catalog<_tstring, _tstring, T>;
        #define _tcatalog        _tstring_catalog
    #endif

}

