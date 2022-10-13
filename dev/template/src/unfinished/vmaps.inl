#pragma once

#pragma warning(disable:4786)
#include <map>
#include <vector>
#include <stdlib.h>
#include <string>
#include <T_Lock.h>

#if  defined(_WINDOWS)
    #pragma comment ( lib, "Algorithmic.lib" )
#endif

namespace ptl
{
    typedef tstring   vmaps_index;
    
#define _vmaps_t(_t)               std::map<vmaps_index, _t>
#define _vmaps_iterator(_t)        ptl::vmaps<_t>::iterator
#define _vmaps_const_iterator(_t)  ptl::vmaps<_t>::const_iterator
    
                                                                                        
    template <typename  _Type> class _VMX_sectorS
    {
        template<typename _T> friend class vmaps;
        typedef typename std::map<vmaps_index, _Type>::iterator       _xmap_iterator;
        typedef typename std::map<vmaps_index, _Type>::const_iterator _xmap_const_iterator;
        
        T_Lock                       lmap;
        std::map<vmaps_index, _Type> tmap;
        
        _VMX_sectorS() {}
        ~_VMX_sectorS() {}
        
        inline bool     empty() { return tmap.empty(); }
        inline unsigned size() { return tmap.size(); }
        inline void     insert( vmaps_index idx, _Type obj )
        {
            typedef typename std::map<tstring, _Type>::value_type value_type;
            value_type  data( ( ( vmaps_index ) idx ), ( ( _Type ) obj ) );
            tmap.insert( data );
        }
        inline unsigned int   erase( vmaps_index idx ) { return tmap.erase( ( vmaps_index ) idx ); }
        inline void           clear() { tmap.clear(); }
        inline _xmap_iterator find( vmaps_index idx ) { return tmap.find( idx ); }
        inline _xmap_iterator begin() { return tmap.begin(); }
        inline _xmap_iterator end() { return tmap.end(); }
        inline T_Lock&        lock() { return lmap; }
        //inline _xmap_iterator erase(_xmap_iterator iter)   { return tmap.erase(iter); }
        inline void           erase( _xmap_iterator iter ) { tmap.erase( iter ); }
    };
    
    extern unsigned int Crc32( const char* buffer, unsigned int buflen );
    
    template <typename _Type> class vmaps
    {
    
#ifndef _veclpos
#define _veclpos(c)          (unsigned int)abs(((int)(c%m_size)))
#endif
    
#ifndef _vecspos
#define _vecspos(s)          _veclpos( Crc32((const byte_t*)s.c_str(), s.size()*sizeof(tchar_t))%m_size )
#endif
    
        //#define _xmap_iterator    _Tree_iterator<std::_Tree_val<std::_Tmap_traits<long long,_Type ,std::less<long long>,std::allocator<std::pair<const long long,_Type >>,false>>>
        typedef typename std::map<vmaps_index, _Type>::iterator       _xmap_iterator;
		typedef typename std::map<vmaps_index, _Type>::const_iterator _xmap_const_iterator;
        
        unsigned int           m_size;
        _VMX_sectorS<_Type>*    m_sector;
        
    public:
    
        vmaps(): m_size( 0 ), m_sector( NULL ) {}
        vmaps( int i ) { init( i ); }
        ~vmaps() { destroy(); }
        
        typedef _xmap_iterator         iterator;
        typedef _xmap_const_iterator   const_iterator;
        
        inline void init( unsigned int i )
        {
            //clear();
            m_size = i;
            
            if ( m_sector )
            {
                delete[] m_sector;
            }
            
            m_sector = new _VMX_sectorS<_Type>[ i ];
        }
        
        inline void destroy()
        {
            m_size = 0;
            
            if ( m_sector )
            {
                delete[] m_sector;
            }
            
            m_sector = NULL;
        }
        
        inline void            clear() { for ( unsigned int i = 0; i < m_size; i++ ) { m_sector[ i ].clear(); } }
        inline void            clear( int i ) { m_sector[ i ].clear(); }
        
        inline iterator        begin() { return m_sector[ 0 ].begin(); }
        inline iterator        begin( vmaps_index idx ) { return m_sector[ _vecspos( idx ) ].begin(); }
        inline iterator        begin( int idx ) { return m_sector[ _veclpos( idx ) ].begin(); }
        
        inline iterator        end() { return m_sector[ m_size - 1 ].end(); }
        inline iterator        end( vmaps_index idx ) { return m_sector[ _vecspos( idx ) ].end(); }
        inline iterator        end( int idx ) { return m_sector[ _veclpos( idx ) ].end(); }
        
        //inline bool     insert(long long idx, _Type obj) { return m_sector[_vecspos(idx)].insert(idx, obj); }
        inline void            insert( vmaps_index idx, _Type obj ) { m_sector[ _vecspos( idx ) ].insert( idx, obj ); }
        //inline void            insert( int idx, _Type obj ) { m_sector[_veclpos( idx )].insert( idx, obj ); }
        
        inline int             erase( vmaps_index idx ) { return m_sector[ _vecspos( idx ) ].erase( idx ); }
        inline void            erase( iterator iter ) { m_sector[ _vecspos( iter->first ) ].erase( iter ); }
        inline _Type           throw_out( vmaps_index idx ) 
        {
            iterator iter = find( idx );
            if ( erase( iter ) == 1 ) 
            {
                return iter->second;
            }
            return 0;
        }

        inline _Type           throw_out( iterator iter )
        {
            if ( erase( iter ) == 1 )
            {
                return iter->second;
            }
            return 0;            
        }
        //inline void            erase( int idx ) { m_sector[_veclpos( idx )].erase( idx ); }
        
        inline iterator        find( vmaps_index idx ) { return m_sector[ _vecspos( idx ) ].find( idx ); }
        //inline iterator        find( int idx ) { return m_sector[_veclpos( idx )].find( idx ); }
        
        // 一定得這樣做 因為找不到的時候 first 是沒有東西的 所以miss會當掉
        // 不同容器造出來的也不能比較 會當掉
        inline bool            miss( vmaps_index idx, iterator iter ) { return ( iter == m_sector[ _vecspos( idx ) ].end() ); }
        inline bool            miss( int idx, iterator iter ) { return ( iter == m_sector[ _veclpos( idx ) ].end() ); }
        
        inline unsigned int    size() { unsigned int c = 0; for ( unsigned int i = 0; i < m_size; i++ ) { c += m_sector[ i ].size(); } return c; }
        inline unsigned int    vecsize() { return m_size; }
        inline unsigned int    mapsize( int i ) { return m_sector[ i ].size(); }
        inline T_Lock&         lock( vmaps_index idx ) { return m_sector[ _vecspos( idx ) ].lock(); }
        inline T_Lock&         lock( int idx ) { return m_sector[ ( unsigned int ) abs( ( ( int ) ( idx % m_size ) ) ) ].lock(); }
    };
}