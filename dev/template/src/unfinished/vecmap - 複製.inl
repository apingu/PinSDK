#ifndef VECMAP_H
#define VECMAP_H

#pragma warning(disable:4786)
#include <map>
#include <vector>
#include <T_Lock.h>
#include <assert.h>


namespace pt
{

#define _vecmap_iterator(_t)  std::map< longlong, _t >::iterator
#define _vecmap_map(_t)       std::map<longlong, _t>

    template <class _Type> 
    class indexmap : public std::map<longlong, _Type>
    {
    public:

        inline bool insert(longlong where, _Type val)
        {
            pair< map<longlong, _Type>::iterator, bool > pr = std::map<longlong, _Type>::insert(std::map<longlong, _Type>::value_type(where, val));
            return pr.second;
        }
    };


    template <class _Type>
	class vecmap
	{

#define _vecpos(c)			(unsigned int)(c%m_size)

		unsigned int                m_size;

	public:

        template <class _Type>
        class sector
        {
            T_Lock                  lmap;
            indexmap<_Type>         tmap;
        };

        sector<_Type>*              m_sector;

		//T_Lock*                     m_lock;
		//indexmap<_Type>*            m_list;

		vecmap()
		{
			m_size = 0;
            m_sector = NULL;
		}

		~vecmap()
		{
			clear();
		}


		inline void init(unsigned int i)
		{
			m_size = i;
            if(m_sector)
                delete [] m_sector;
			if(m_lock!=NULL)
				delete [] m_lock;
			m_lock = new T_Lock[i];
			if(m_list!=NULL)
				delete [] m_list;
			m_list = new indexmap<_Type>[i];
		}

		inline void clear()
		{
			m_size = 0;
			delete [] m_lock;
			delete [] m_list;
		}


		inline bool insert(longlong idx, _Type obj)
		{
			unsigned int ix = _vecpos(idx);
			if(ix<m_size)
			{
                TS_Lock lock(&m_lock[ix]);
				return m_list[ix].insert(idx, obj);
			}
			return false;
		}

		inline _Type erase(longlong idx)
		{
			unsigned int ix = _vecpos(idx);
			if(ix<m_size)
			{
                TS_Lock lock(&m_lock[ix]);
				std::map<longlong, _Type>::iterator iter = m_list[ix].find(idx);
				if(iter!=m_list[ix].end())
				{
					_Type tp = iter->second;
					m_list[ix].erase(iter);
					return tp;
				}
			}

            _Type nulltype;
            memset(&nulltype, 0, sizeof(_Type));
			return nulltype;
		}

		inline _Type find(longlong idx)
		{
			unsigned int ix = _vecpos(idx);
			if(ix<m_size)
			{
				std::map<longlong, _Type>::iterator iter = m_list[ix].find(idx);
				if(iter==m_list[ix].end())
					return NULL;
				return iter->second;
			}

            _Type nulltype;
            memset(&nulltype, 0, sizeof(_Type));
            return nulltype;
		}

		inline size_t  size()
		{
			int c=0;
			for(int i=0; i<m_size; i++)
			{
				c+=m_list[i].size();
			}
			return c;
		}

		inline size_t  vecsize()           {return m_size;}
		inline size_t  mapsize(int i)      {return m_list[i].size();}

		inline unsigned int getpos(longlong c) 
		{
			unsigned int ret = _vecpos(c);
			if(ret<m_size)
				return ret;
			assert( NULL||"vecmap get pos error!");
			return -1;
		}

        inline T_Lock* getlockbyid(longlong c)
        {
            unsigned int ix=_vecpos(c);
            if(ix<m_size)
                return &m_lock[ix];
            return NULL;
        }

        inline std::map<longlong, _Type>* getmapbyid(longlong c)
        {
            unsigned int ix=_vecpos(c);
            if(ix<m_size)
                return &m_list[ix];
            return NULL;
        }

        inline T_Lock* getlockbypos(unsigned int pos)
        {
            if(ix<m_size)
                return &m_lock[pos];
            return NULL;
        }

        inline std::map<longlong, _Type>* getmapbypos(unsigned int pos)
        {
            if(ix<m_size)
                return &m_list[pos];
            return NULL;
        }
	};

}




#endif