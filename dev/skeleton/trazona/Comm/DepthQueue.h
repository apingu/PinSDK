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
#ifndef DEPTHQUEUE_H
#define DEPTHQUEUE_H

#include <vector>


template<class _Type> class DepthQueue : public std::vector<_Type>
{
	int    m_depth;

public:

	DepthQueue ( void )  { m_depth = 0; }     
	//DepthQueue ( int d ) { m_depth = depth; } 
	~DepthQueue( void )  { clear(); }

	void depth( int d ) { m_depth = d; } 


	_Type push( const _Type& x )
	{
		push_back( x );
		if( size() > m_depth )
		{
			return pop();
		}
		return NULL;
	}


	_Type pop( void )
	{
		_Type e = operator[]( 0 );
		erase( begin() );
		return e;
	}

};





#endif