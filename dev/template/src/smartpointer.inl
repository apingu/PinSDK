//Smart_Pointer.h
//
//
//  use for new pointer and it will delete itself
//  must use as pointer
//
//
//
//
//
//
//
//               Apingu
//                  2002
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <stdio.h>
#include <stdlib.h>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


#define sp(p) smartptr<p>;

template <class _Type> class smartptr
{
    _Type*  pointer_;

public:

    smartptr()
    {
        pointer_ = 0;
    }
    smartptr( _Type* ptr )
    {
        pointer_ = ptr;
    }

    ~smartptr()
    {
        if( pointer_ != 0 )
        {
            delete pointer_;
        }
    }

    inline _Type* operator->()
    {
        return pointer_;
    }
    inline _Type& operator*()
    {
        return *pointer_;
    }

    inline operator void* ( )
    {
        return pointer_;
    }
    inline operator _Type* ( )
    {
        return pointer_;
    }

    inline bool operator==( _Type* ptr )
    {
        return ( pointer_ == ptr );
    }
    inline bool operator!=( _Type* ptr )
    {
        return ( pointer_ != ptr );
    }

    inline operator=( _Type& rhs )
    {
        if( this != &rhs )
        {
            reset( rhs.release() );
        }
        return *this;
    }

    inline _Type* release()
    {
        _Type*  oldPointee = pointer_;
        pointer_ = 0;
        return oldPointee;
        //delete pointer_; pointer_ = 0
    }

    inline void reset( _Type* ptr )
    {
        if( pointer_ != ptr )
        {
            delete pointer_;
        }
        pointer_ = ptr;
    }

    //void* operator new   ( size_t size)       { return pointer_ = new _Type;}
    //inline void operator delete( void *DeadObject, size_t size) { delete pointer_;}
};