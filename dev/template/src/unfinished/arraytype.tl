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
//
#ifndef ARRAYTYPE_H
#define ARRAYTYPE_H


namespace pt
{
	template <class _Type, unsigned int _Size >
	class arraytype
	{
	    _Type m_buff[_Size];
	
	public:
	
	    arraytype( const arraytype& st ){ memcpy( m_buff, st.m_buff, _Size*sizeof(_Type) ); }
		arraytype(){}
		~arraytype(){}
		
		
		_Type&     operator[] ( unsigned int id ){ return m_buff[id]; }
		bool       operator== ( arraytype st ){ return memcmp( m_buff, st.m_buff, _Size*sizeof(_Type) ); }
	    arraytype& operator= ( arraytype st ){ memcpy( m_buff, st.m_buff, _Size*sizeof(_Type) ); return (*this); }
		arraytype& operator= ( _Type* st ){ memcpy( m_buff, st, _Size*sizeof(_Type) ); return (*this); }
	
	};
};



#endif