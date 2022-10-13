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
//            system include $(PINSDK)/Base,$(PINSDK)/Platform,$(PINSDK)/Platform/XXXX
//            compile unix like system   -->   gcc -o Xtest Xtest.c -L/usr/X11R6/lib -lX11
//
//                                               Copyright    (C)    2001    Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "pf-event.h"
#include <platform.h>


class PfWindow
{
    //Pwproperty        m_property;
    tchar_t*        m_AppTitle;
    tchar_t         m_szCurrentDir[_MAX_LPATH];              // now work dir
    dword_t         m_LoopBreakTime;
                    
protected:          
                    
    Pdisplay        m_display;
    LG::Boxi        m_area;
    bool            m_bfullscreen;                           // is full screen mode?
                    
                    
    bool            ( *OnIdle )( int isWndActive );
public:             
                    
    Pwnd            m_window;                             // Holds Our Window Handle
    
    PfWindow();
    ~PfWindow();
    
    //operator  RwCamera* ()   { return m_camera; }
    
    //Pwnd &        GetWindow          (void){ return m_window; }
    operator        Pwnd&               ()      { return m_window; }
    str_t           GetWorkDir          ( void ) { return m_szCurrentDir; }

    static bool     Initialize          ( void* property = NULL );
    Pmgc            CreateCanvas        ( void );
    int_t           ReleaseCanvas       ( Pmgc gc );
    LG::Size        GetWindowSize       ( void ) { return LG::Size( m_area.width, m_area.height ); }

    bool            IsKeyPress          ( uint_t key );
    bool            Create              ( cstr_t title, LG::Point pos, LG::Boxi size,
                                          bool fullscreenflag = false,
                                          const PWstyle* style = NULL,
                                          int background = NULL );
                                          
    void            SetIcon             ( const char* IconResName, bool bBig );
    void            Destroy             ( void );
    bool            IsExist             ( void );

    void            SetStyle            ( long style );
    long            GetStyle            ( void );

    bool            ChangeToFullScreen  ( bool m_bfullscreen = true );
    bool            IsFullScreen        ( void );

    void            SetLoopBreakTime    ( dword_t time );
    bool            PeekEvent           ();
    void            MainLoop            ( void );
    int             Move                ( int x, int y );
    int             Resize              ( LG::Rect WindowRect );
    int             Show                ( bool flag );
    int             Maximize            ( void );
    int             Minimize            ( void );
    LG::Point       Position            ( void );
    int             Focus               ( void );

    LG::Point       GetMousePosition    ( void );
    /*                                  
    PfEvent         GetEventState       (void);
    void            ClearEventState     (void);
    */
    
    //==WIN32 system========================================================
    int             w32DragAcceptFiles  ( bool fAccept );

    void            SetProcFunc         ( bool  ( *Procfunc )  ( PfEvent event ) );
    void            SetIdleFunc         ( bool  ( *AppIdle )   ( int ) );
    void            SetDestroyFunc      ( void  ( *AppDestroy )( void ) );
    
    // For game to show message box if necessary
    void            MsgBox             ( const tchar_t* message, const tchar_t* title = NULL );
};
//typedef PLRESULT (PfWindow::*MPROC) (Pwnd hWnd, uint_t uMsg, PWPARAM wParam, PLPARAM lParam);