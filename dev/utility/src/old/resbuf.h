#ifndef RESBUF_H
#define RESBUF_H

#include <basedef.h>

class _tagResBuf
{
    size32_t m_bufflen;
    char*    m_buffptr;

public:

    _tagResBuf();
    _tagResBuf( const _tagResBuf& buff );
    ~_tagResBuf();

    operator char*();

    char&        operator[] ( unsigned int pos );
    void         operator = ( const _tagResBuf& buff );

    void         resize     ( unsigned int len );
    unsigned int size       ( void );
    void         clear      ( void );
    bool         empty      ( void );

};

#endif
