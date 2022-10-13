// Base Mesh Class
//
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↓↑
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↓↑
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↓↑
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↓↑
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//      ↓↑
//      [  ]<-->[  ]<-->[  ]<-->[  ]<-->[  ]
//
//              Copyright (C) 1999 Pin
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
//#include "PLIST.H"

template <class _Type> class mesh: private PCC::RList<_Type>
{
public:

    typedef pfstl::RList<_Type> RING;
    pfstl::RList<RING*>         MAP;

    mesh()
    {
    };

    ~mesh()
    {
    };

public:

    inline _Type& operator()( int Col, int Row )
    {
        RING*    Rowlist = MAP( Row );
        return   Rowlist->Get( Col );
    }

    inline void operator()( int Col, int Row, _Type node )
    {
        RING*   Rowlist = MAP.Get( Row );
        if( Rowlist == 0 )
        {
            Rowlist = new RING;
        }
        Rowlist->Put( Col, node );
        MAP.Put( Row, Rowlist );
    }

    inline void operator()( int Row, PCC::RList<_Type>* rlist )
    {
        MAP.Put( Row, rlist );
    }

    inline PCC::RList<_Type>* operator()( int Row )
    {
        return MAP.Get( Row );
    }

    inline void Insert( int Col, int Row, _Type node )
    {
        RING*   Rowlist = new RING;
        Rowlist->Insert( Col, node );
        MAP.Insert( Rowlist, Row );
    }

    inline void Insert_Row( int Col, int Row, _Type node )
    {
        RING*   Rowlist = new RING;
        Rowlist->Put( Col, node );
        MAP.Insert( Rowlist, Row );
    }

    inline void Insert_Col( int Col, int Row, _Type node )
    {
        RING*   Rowlist = new RING;
        Rowlist->Insert( Col, node );
        MAP.Put( Rowlist, Row );
    }

    inline int Col_Size( int Row )
    {
        RING*   RowList = MAP( Row );
        if( RowList == 0 )
        {
            return 0;
        }
        return RowList->size();
    }

    inline int Row_Size()
    {
        return MAP.size();
    }
};