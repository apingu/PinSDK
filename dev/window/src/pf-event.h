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
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
#include <Graphic2d.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件


typedef struct MyEvent
{
    void*     hWnd;
    uint_t    uMsg;
    uint_t    wParam;
    uint_t    lParam;
    long32_t  x;
    long32_t  y;
} PfEvent;


class IPFEventDelegate
{

public:

    IPFEventDelegate(){};
    ~IPFEventDelegate(){};

    //事件
    virtual void   OnNull( LG::Point point ) {};
    virtual void   OnMouseWheel( int key, LG::Point point ) {};
    virtual void   OnMouseOver( int key, LG::Point point ) {};
    virtual void   OnLMouseDown( int key, LG::Point point ) {};
    virtual void   OnRMouseDown( int key, LG::Point point ) {};
    virtual void   OnLMouseUp( int key, LG::Point point ) {};
    virtual void   OnRMouseUp( int key, LG::Point point ) {};
    virtual void   OnLDBClick( int key, LG::Point point ) {};
    virtual void   OnRDBClick( int key, LG::Point point ) {};
    virtual void   OnDrag( LG::Point point ){};
    virtual void   OnDragDrop( LG::Point point ) {};
    virtual void   OnKeyDown( int key ) {};
    virtual void   OnKeyUp( int key ) {};
    virtual void   OnDock( DWORD docktime, LG::Point point ) {};
    //virtual void   OnMoving(LG::Point point) {};
    virtual void   OnChar( unsigned nChar ) {};
    virtual void   OnUserNotify( uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point ) {};  // 使用者自定訊息
};