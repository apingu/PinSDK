#ifndef __UTCRITICALSECTION_H
#define __UTCRITICALSECTION_H

#include <cassert>
#include <windows.h>

namespace Cyberspace
{
//-----------------------------------------------------------------------------
class UtCriticalSection 
{
public:
    UtCriticalSection() { InitializeCriticalSection( &m_CriticalSection ); }
    ~UtCriticalSection() { DeleteCriticalSection( &m_CriticalSection) ; }

    inline void    Enter() { EnterCriticalSection( &m_CriticalSection ); }
    inline void    Leave() { LeaveCriticalSection( &m_CriticalSection ); }

private:
    const UtCriticalSection & operator=( const UtCriticalSection & );
    UtCriticalSection( const UtCriticalSection & );

    CRITICAL_SECTION    m_CriticalSection;
};

//-----------------------------------------------------------------------------
class UtAutoCS
{
public:
    UtAutoCS( UtCriticalSection *pHost ): m_pHost( pHost ) 
    {
        assert( NULL != m_pHost );
        m_pHost->Enter();
    }

    ~UtAutoCS() 
    {
        m_pHost->Leave();
    }

private:
    UtCriticalSection *m_pHost;
};

//-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
#endif