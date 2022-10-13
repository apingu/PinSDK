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
#ifndef _PROPERTIES_H_
#define _PROPERTIES_H_

#include <fFile.h>

template< class _Type> 
class CProperties
{
public:

    CProperties();
    ~CProperties();
    
    Open( const char* filename );
    ReadData( _Type* get, UINT id );
    Close();
};



#endif
