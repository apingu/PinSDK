#include "resbuf.h"
#include <basedef.h>
#include <stdio.h>

#if defined(__GNUC__)
#include <string.h>
#endif

_tagResBuf::_tagResBuf()
{
    m_bufflen = 0;
    m_buffptr = NULL;
}

_tagResBuf::_tagResBuf( const _tagResBuf& buff )
{
    m_bufflen = 0;
    m_buffptr = NULL;
    operator =  ( buff );
}

_tagResBuf::~_tagResBuf()
{
    SAFE_DELETE_ARRAY(m_buffptr);
}

_tagResBuf::operator char*()
{
    return m_buffptr;
}


char& _tagResBuf::operator[] ( unsigned int pos )
{
    return m_buffptr[pos];
}


void _tagResBuf::operator= ( const _tagResBuf& buff )
{
    if( m_bufflen == buff.m_bufflen )
    {
        memcpy( m_buffptr, buff.m_buffptr, buff.m_bufflen );
    }
    else
    {
        SAFE_DELETE_ARRAY(m_buffptr);
        m_buffptr = new char[buff.m_bufflen];
        m_bufflen = buff.m_bufflen;
        memcpy( m_buffptr, buff.m_buffptr, buff.m_bufflen );
    }

}

void _tagResBuf::resize( unsigned int len )
{
    if( m_bufflen != len )
    {
        SAFE_DELETE_ARRAY(m_buffptr);
        m_buffptr = new char[len];
        m_bufflen = len;
    }
}


void  _tagResBuf::clear( void )
{
    SAFE_DELETE_ARRAY(m_buffptr);
    m_bufflen=0;
}

unsigned int _tagResBuf::size( void )
{
    return m_bufflen;
}

bool _tagResBuf::empty( void )
{
    return (m_bufflen==0)?true:false;
}
