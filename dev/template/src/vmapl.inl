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

#pragma warning(disable:4786)

//01.== C 系統文件/C++ 系統文件
#include <map>
#include <vector>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <t_lock.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


namespace pfstl
{
    typedef ulong64_t   vmapl_index;

#define _vmapl_t(_t)               std::map<vmapl_index, _t>
#define _vmapl_iterator(_t)        pfstl::vmapl<_t>::iterator
#define _vmapl_const_iterator(_t)  pfstl::vmapl<_t>::const_iterator

    // 容器
    template <typename _Type> class vmapl
    {
        // 區段物件
        template <typename _Ty> class sectorl
        {
        private:

            template<typename _T> friend class vmapl;
            typedef typename std::map<vmapl_index, _Ty>::iterator       iterator;
            typedef typename std::map<vmapl_index, _Ty>::const_iterator const_iterator;

            T_Lock                     elock_; // 給外部使用的
            T_Lock                     ulock_; // 給內部自己使用的
            std::map<vmapl_index, _Ty> uarea_; // 資料存放區

            sectorl() {}
            ~sectorl() {}

            inline bool           empty() { return uarea_.empty(); }
            inline uint64_t       size() { return uarea_.size(); }
            inline void           insert( vmapl_index idx, _Ty obj ) { typedef typename std::map<vmapl_index, _Ty>::value_type value_type; value_type data( ( ( vmapl_index )idx ), ( ( _Ty )obj ) ); uarea_.insert( data ); }
            inline unsigned int   erase( vmapl_index idx ) { return uarea_.erase( ( vmapl_index )idx ); }
            inline void           clear() { uarea_.clear(); }
            inline iterator       find( vmapl_index idx ) { return uarea_.find( idx ); }
            inline iterator       begin() { return uarea_.begin(); }
            inline iterator       end() { return uarea_.end(); }
            inline void           erase( iterator iter ) { uarea_.erase( iter ); }
        };

#ifndef _veclpos
#define _veclpos(c)          (unsigned int)abs(((int)(c%length_)))
#endif

        //#define _xmap_iterator    _Tree_iterator<std::_Tree_val<std::_Tmap_traits<vmapl_index,_Type ,std::less<vmapl_index>,std::allocator<std::pair<const vmapl_index,_Type >>,false>>>
        unsigned int           length_;
        sectorl<_Type>* sector_;

    public:

        vmapl(): length_( 0 ), sector_( NULL ) {}
        vmapl( int i ) { init( i ); }
        ~vmapl() { destroy(); }

        typedef typename std::map<vmapl_index, _Type>::iterator        iterator;
        typedef typename std::map<vmapl_index, _Type>::const_iterator  const_iterator;

        inline void            init( unsigned int i ) { length_ = i; if( sector_ ) { delete[] sector_; } sector_ = new sectorl<_Type>[ i ]; }

        inline void            destroy() { length_ = 0; if( sector_ ) { delete[] sector_; } sector_ = NULL; }

        inline void            clear() { for( unsigned int i = 0; i < length_; i++ ) { TS_Lock lock( &sector_[ i ].ulock_ ); sector_[ i ].clear(); } }
        inline void            clear( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); sector_[ ui ].clear(); }

        inline iterator        begin() { TS_Lock lock( &sector_[ 0 ].ulock_ ); return sector_[ 0 ].begin(); }
        inline iterator        begin( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].begin(); }

        inline iterator        end() { int ui = length_ - 1;     TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].end(); }
        inline iterator        end( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].end(); }

        inline void            insert( vmapl_index idx, _Type obj ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); sector_[ ui ].insert( idx, obj ); }

        inline int             erase( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].erase( idx ); }
        inline void            erase( iterator iter ) { int ui = _veclpos( iter->first ); TS_Lock lock( &sector_[ ui ].ulock_ );  return sector_[ ui ].erase( iter ); }

        inline _Type           throw_out( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); iterator iter = sector_[ ui ].find( idx ); if( erase( iter ) == 1 ) { return iter->second; } return 0; }

        inline _Type           throw_out( iterator iter ) { if( erase( iter ) == 1 ) { return iter->second; } return 0; }


        inline iterator        find( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].find( idx ); }
        inline _Type& operator[]( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); iterator iter = sector_[ ui ].find( idx ); if( iter != sector_[ ui ].end() ) { return iter->second; } sector_[ ui ].insert( idx, _Type() ); return sector_[ ui ].find( idx )->second; }

        inline bool            miss( vmapl_index idx, iterator iter )
        {
            // 一定得這樣做 因為找不到的時候 first 是沒有東西的 所以miss會當掉
            // 不同容器造出來的也不能比較 會當掉
            int ui = _veclpos( idx );
            TS_Lock lock( &sector_[ ui ].ulock_ );
            return ( iter == sector_[ ui ].end() );
        }

        inline unsigned int    size() { unsigned int c = 0; for( unsigned int i = 0; i < length_; i++ ) { TS_Lock lock( &sector_[ i ].ulock_ ); c += sector_[ i ].size(); } return c; }
        inline unsigned int    vsize() { return length_; }
        inline unsigned int    ssize( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ].size(); }
        inline T_Lock& lock( vmapl_index idx ) { return sector_[ _veclpos( idx ) ].elock_; }
        inline std::map<vmapl_index, _Type>& sector( vmapl_index idx ) { int ui = _veclpos( idx ); TS_Lock lock( &sector_[ ui ].ulock_ ); return sector_[ ui ]; }
    };
}