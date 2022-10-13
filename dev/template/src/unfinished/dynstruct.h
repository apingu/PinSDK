#ifndef _DYNSTRUCT_H_
#define _DYNSTRUCT_H_

#include <stdio.h>
#include <stdlib.h>
#include "DString.h"
#include "DynArray.h"


class CDynStruct
{
    ARRAY<BYTE>             m_dataSpace;

    DynArray<BYTE*>         m_dataOffset;
    DynArray<PCC::String>   m_datatype;
    DynArray<PCC::String>   m_dataField;

public:

    CDynStruct()
    {
    };

    void    ReadStruct( const char* path );
    void    newField( const char* type,void* ptr );
};

#endif

