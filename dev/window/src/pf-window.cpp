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
//                                               Copyright    (C)    2001    Pin
//////////////////////////////////////////////////////////////////////////

//01.== C 系統文件/C++ 系統文件
#include <list>

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "pf-window.h"
//Base//

//void     CPEmptyWndProc   (PfEvent event){ return ; }

bool    ( *UserCallBackProc )   ( PfEvent event ) = NULL;
void    ( *UserPollProc )       ( PfEvent event ) = NULL;
void    ( *UserDestroy )        ( void );
bool    CPEmptyIdle             ( int ) { return true; }

#ifdef _WINDOWS
    typedef uint_t                      Pwparam;
    typedef long32_t                    Plparam;
    typedef long32_t                    Plresult;
    #ifdef UNICODE
        #define CLASSNAME                 L"PfWindow App"
    #else
        #define CLASSNAME                 "PfWindow App"
    #endif
    Plresult CPDefWndProc     ( Pwnd m_window, uint_t uMsg, Pwparam wParam, Plparam lParam );
    
    bool w32DefineProperty( WNDCLASSEX* property );
    bool w32Create( LG::Boxi size, void* param );
    
    //bool                  g_bInitialize=false;
    WNDCLASSEX*             g_property = NULL;
#endif
#ifdef _X11
    bool x11Create( LG::Boxi size, void* param );
#endif
#ifdef _XBOX
    bool xboxCreate( LG::Boxi size, void* param );
#endif
#ifdef _PS2
    bool ps2Create( LG::Boxi size, void* param );
#endif






//Plresult (*UserCallBackProc)(HWND m_window, uint_t uMsg, Pwparam wParam, Plparam lParam) = CPDefWndProc;

int         _bIsActive;                          // is window activated?
bool        _pckeys[256];                        // Array Used For The Keyboard Routine
//std::list< PfEvent > g_Event;
UINT        g_WindowCount = 0;
bool        g_bProc = false;
//PfEvent     g_Event;
LG::Point   g_EventPoint;

//=============================================================================
// NAME:
// DESC: Constructor func
//=============================================================================
PfWindow::PfWindow()
{
    //memset(&m_window, 0, sizeof(Pwnd));// Holds Our Window Handle
    //m_window = NULL;
    m_display       = NULL;
    m_window        = NULL;
    _bIsActive      = true;
    m_bfullscreen   = false;
    m_AppTitle      = NULL;
    UserDestroy     = NULL;
    OnIdle          = CPEmptyIdle;
    m_LoopBreakTime = 1;
    //strcpy(szClass_name,);
    //Initialize();
    // Remember where are we started
#if defined _WINDOWS
    GetCurrentDirectoryA( _MAX_LPATH, m_szCurrentDir );
#endif
#ifdef _LINUX
    getcwd( m_szCurrentDir, _MAX_LPATH );
#endif
    if( ( m_szCurrentDir[strlen( m_szCurrentDir ) - 1] != '\\' ) || ( m_szCurrentDir[strlen( m_szCurrentDir ) - 1] != '/' ) )
    {
        strcat( m_szCurrentDir, "/" );
    }
}


//=============================================================================
// NAME:
// DESC: distructor
//=============================================================================
PfWindow::~PfWindow()
{
    if( UserDestroy != NULL )
    { UserDestroy(); }
    Destroy();
}


//=============================================================================
// NAME: Initialize()
// DESC:
//=============================================================================
bool PfWindow::Initialize( void* property )
{
#ifdef _WINDOWS
    if( g_WindowCount == 0 )
    {
        // create a new property
        if( g_property != NULL )
        {
            delete g_property; 
        }
        g_property = new WNDCLASSEX;
        if( property == NULL )
        { 
            w32DefineProperty( ( WNDCLASSEX* )g_property ); 
        }
        else
        { 
            memcpy( g_property, property, sizeof( WNDCLASSEX ) ); 
        }
        //very inportant function
        CoInitialize( NULL );
        if ( !RegisterClassEx( ( WNDCLASSEX* )g_property ) )                  // Attempt To Register The Window Class
        {
            MessageBoxA( NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                        // Exit And Return FALSE
        }
    }
    /*
    #ifdef _WINDOWS
    //return w32DefineProperty(g_property);
    #endif
    
    #ifdef _X11
    
    return TRUE;
    #endif
    DebugMessage("Not a support system", "PfWindow");
    */
#endif
    return TRUE;
}


//=============================================================================
// NAME:
// DESC: Distructor
//=============================================================================
void PfWindow::Destroy( void )
{
#if defined (_WINDOWS) &&!defined(_XBOX)
    if ( IsFullScreen() )                           // Are We In Fullscreen Mode?
    {
        ChangeDisplaySettings( NULL, 0 );       // If So Switch Back To The Desktop
        ShowCursor( TRUE );                     // Show Mouse Pointer
    }
    if ( m_window )
    {
        int r =  DestroyWindow( m_window );
        if( r == NULL )                          // Are We Able To Destroy The Window?
        {
            Pf_ERROR( "PfWindow: Could Not Release window" );
            int ret = GetLastError();
            //m_window=NULL;                     // Set m_window To NULL
        }
        else
        { 
            m_window = NULL; 
        }
        g_WindowCount--;
        SAFE_DELETE_ARRAY( m_AppTitle );
    }
    if( g_WindowCount == 0 )
    {
        if( g_property != NULL )
        {
            if( ( ( WNDCLASSEX* )g_property )->hInstance != NULL )
            {
                if ( !UnregisterClass( ( ( WNDCLASSEX* )g_property )->lpszClassName, ( ( WNDCLASSEX* )g_property )->hInstance ) ) // Are We Able To Unregister Class
                {
                    Pf_ERROR( "PfWindow: Could Not Unregister Class" );
                    //m_hInstance=NULL;                                    // Set m_hInstance To NULL
                }
                delete ( WNDCLASSEX* )g_property;
                g_property = NULL;
                //memset(((WNDCLASSEX*)g_property), 0, sizeof(WNDCLASSEX));
            }
        }
    }
#endif
#ifdef _X11
    printf( "Destory Window\n" );
    XDestroyWindow( m_display, m_window );
    g_WindowCount--;
    SAFE_DELETE_ARRAY( m_AppTitle );
    //getchar();
    printf( "XFlush\n" );
    XFlush( m_display );
    //getchar();
    printf( "close display\n" );
    XCloseDisplay( m_display );
    //getchar();
    m_window  = NULL;
    m_display = NULL;
#endif
    //DebugMessage("Not a support system", "PfWindow");
    return;
}


//=============================================================================
// NAME: Create
// DESC: create a window
//=============================================================================
bool PfWindow::Create( cstr_t title, LG::Point pos, LG::Boxi size, bool fullscreenflag, const PWstyle* wstyle, int background )
{
    if( title != NULL )
    {
        m_AppTitle = new tchar_t[strlen( title ) + 1];
        strcpy( m_AppTitle, title );
    }
    m_area     = size;
    /*
    WNDCLASSEX                    wc;
    
    //if (!hPrevInstance)
    {
        // Register the Window Class
        wc.lpszClassName = "PfWindow App";
        wc.lpfnWndProc = (WNDPROC) CPDefWndProc;
        wc.style = CS_VREDRAW | CS_HREDRAW;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);        // Load The Default Icon
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);        // Load The Arrow Pointer
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        if (!RegisterClass(&wc))
            return FALSE;
    }
    
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    // Create and Show the Main Window
    m_window = CreateWindowEx(0,
                          "1234",
                          wc.lpszClassName,
                          WS_CLIPCHILDREN | WS_POPUP,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,                         // Calculate Adjusted Window Width
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    if (m_window == NULL)
        return FALSE;
    //ShowWindow(m_window, nCmdShow);
    UpdateWindow(m_window);
    return 1;
    */
    if( g_property == NULL )
    { Initialize( NULL ); }
#ifdef _WINDOWS
    //WNDCLASSEX    g_property;                                // Windows Class Structure
    dword_t      dwExStyle;                                  // Window Extended Style
    dword_t      dwStyle;                                    // Window Style
    LG::Rect     WndRect( pos.x, pos.y, pos.x + ( size.width - 1 ), pos.y + ( size.height - 1 ) );
    if ( IsFullScreen() )                                        // Attempt Fullscreen Mode?
    {
        if( ChangeToFullScreen() )
        { 
            ShowCursor( FALSE ); 
        }                             // Hide Mouse Pointer
    }
    //define style
    if( wstyle != NULL )
    {
        dwStyle = wstyle->dwStyle;
        dwExStyle = wstyle->dwExStyle;
    }
    else
    {
        if ( IsFullScreen() )                                        // Are We Still In Fullscreen Mode?
        {
            dwExStyle = WS_EX_APPWINDOW;                         // Window Extended Style
            //dwStyle=WS_POPUP;                                  // Windows Style
            dwStyle = WS_CLIPCHILDREN | WS_POPUP;                // Windows Style
        }
        else
        {
            //dwStyle=WS_POPUP;
            //dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;        // Window Extended Style
            //dwStyle=WS_CLIPCHILDREN | WS_POPUP;
            dwStyle = WS_OVERLAPPEDWINDOW;
            dwExStyle = WS_EX_TOPMOST;      // Window Extended Style
            /*
            if(title != NULL)
            {
                dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;//WS_OVERLAPPEDWINDOW;                         // Windows Style
            }
            else
            {
                dwStyle=WS_POPUP;                         // Windows Style
                //dwStyle=CW_USEDEFAULT;
            }
            */
            //dwStyle = dwStyle ^ (WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX);
        }
        //dwStyle=WS_CLIPCHILDREN | WS_POPUP;
    }
    //AdjustWindowRectEx(&WndRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size
    // Calculate the proper size for the window given a client of 640x480
    //UINT cx = size.width+GetSystemMetrics(SM_CXSIZEFRAME)*2;
    //UINT cy = size.height+GetSystemMetrics(SM_CYSIZEFRAME)*2+GetSystemMetrics(SM_CYMENU);
#ifdef UNICODE
    WCHAR real_title[128];
    int len;
    len = strlen( title );
    if( len >= 128 )    { len = 127; }
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, title, len, real_title, 128 );
    real_title[len] = 0;
#else
    const tchar_t* real_title = title;
#endif
    HINSTANCE hInstance = GetModuleHandle( NULL );
    // Create and Show the Main Window
    if ( !( m_window = CreateWindowEx( dwExStyle,           // Extended Style For The Window
                                       ( ( WNDCLASSEX* )g_property )->lpszClassName, // Class Name
                                       real_title,                              // Window Title
                                       //WS_POPUP |                             // Required Window Style //WS_OVERLAPPEDWINDOW
                                       //WS_CLIPCHILDREN |                      // Required Window Style
                                       dwStyle,                                 // Selected Window Style //CW_USEDEFAULT
                                       WndRect.left, WndRect.top,               // Window Position
                                       WndRect.width(),                         // Calculate Adjusted Window Width
                                       WndRect.height(),                        // Calculate Adjusted Window Height
                                       NULL,                                    // No Parent Window
                                       NULL,                                    // No Menu
                                       hInstance,                   // Instance
                                       NULL ) ) )                               // Don't Pass Anything To WM_CREATE
    {
        MessageBoxA( NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION );
        Destroy();
        return FALSE;                                        // Return FALSE
    }
    //ShowWindow(m_window,SW_SHOW);                               // Show The Window
    //SetForegroundWindow(m_window);                            // Slightly Higher Priority
    //SetFocus(m_window);                                       // Sets Keyboard Focus To The Window
    g_WindowCount++;
    return TRUE;
#endif
#ifdef _X11
    XSetWindowAttributes attribute;
    XSizeHints* sz;
    // 建立一個 display 的 connection
    m_display = XOpenDisplay( "0:0" );
    // 建立和設定 window 的屬性
    m_window = XCreateWindow( m_display, XDefaultRootWindow( m_display ),
                              WndRect.left, WndRect.top, WndRect.width(), WndRect.height(), m_Depth, XDefaultDepth( m_display, 0 ),
                              InputOutput, CopyFromParent, 0, &attribute );
    // 和 Window Manager 進行溝通
    XStoreName( m_display, m_window, title );
    sz         = XAllocSizeHints();
    sz->x      = WndRect.left;
    sz->y      = WndRect.top;
    sz->width  = WndRect.width();
    sz->height = WndRect.height();
    sz->flags  = USPosition | USSize;
    XSetNormalHints( m_display, m_window, sz );
    // Mapping Window  正式影射到顯示器畫面
    //printf("Map window\n");
    XMapWindow( m_display, m_window );
    //getchar();    // 至此，視窗已執行 Map 的動作了，但顯示器上，卻可能看不到。
    //printf("XFlush\n");
    XFlush( m_display );
    //getchar();    // 這裏，你應該就看到顯示器上的變化了
    /*
       .................
       .... 程式處理部分 ..
       ....................
    */
    return TRUE;
#endif
#ifdef _PS2
    // Initialize communication with the IOP
#endif
#ifdef _XBOX
    XDEVICE_PREALLOC_TYPE _device_t[] =
    {
        { XDEVICE_TYPE_GAMEPAD, 4},
        {XDEVICE_TYPE_MEMORY_UNIT, 2}
    };
    // Initialize devices
    XInitDevices( sizeof( _device_t ) / sizeof( XDEVICE_PREALLOC_TYPE ), _device_t );
    // Variables for multi-platform compile
    //m_window = NULL;
    //int device   = 0;
    //int window   = 0;
    //int instance = 0;
    // Set display size for application
    //int displaywidth  = w;
    //int displayheight = h;
    // Fall through to common entry point
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return FALSE;
}


//=============================================================================
// NAME: SetIcon()
// DESC:
//=============================================================================
void PfWindow::SetIcon( const char* IconResName, bool bBig )
{
#ifdef _WINDOWS
    HICON hicon = LoadIcon( GetModuleHandle( 0 ), IconResName );
    SendMessage( m_window, WM_SETICON, FALSE, ( LPARAM )hicon );
#endif
    return;
}

//=============================================================================
// NAME: ChangeToFullScreen()
// DESC:
//=============================================================================
bool PfWindow::ChangeToFullScreen( bool fullscreen )
{
    if( IsFullScreen() == fullscreen )
    { return TRUE; }
#ifdef _WINDOWS
    if( fullscreen )
    {
        DEVMODE dmScreenSettings;                             // Device Mode
        memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) ); // Makes Sure Memory's Cleared
        dmScreenSettings.dmSize = sizeof( dmScreenSettings ); // Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth  = m_area.width;         // Selected Screen Width
        dmScreenSettings.dmPelsHeight = m_area.height;        // Selected Screen Height
        dmScreenSettings.dmBitsPerPel = m_area.depth << 3;    // Selected Bits Per Pixel
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
        {
            // If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
            if( MessageBoxA( NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "CPWindows",
                             MB_YESNO | MB_ICONEXCLAMATION ) == IDYES )
            {
                return FALSE;                                  // Select Windowed Mode (Fullscreen=FALSE)
            }
            else
            {
                // Pop Up A Message Boxi Letting User Know The Program Is Closing.
                MessageBoxA( NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP );
                PostQuitMessage( 0 );
                return FALSE;                                // Exit And Return FALSE
            }
        }
    }
    else
    {
        if( ChangeDisplaySettings( NULL, 0 ) != DISP_CHANGE_SUCCESSFUL )
        {
            return FALSE;
        }
    }
#else
#endif
    m_bfullscreen = fullscreen;
    return TRUE;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool PfWindow::IsFullScreen( void )
{
    return m_bfullscreen;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void PfWindow::SetStyle( long style )
{
#ifdef _WINDOWS
    SetWindowLong( ( HWND )m_window, GWL_STYLE, style );
#endif
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
long PfWindow::GetStyle( void )
{
#ifdef _WINDOWS
    return GetWindowLong( ( HWND )m_window, GWL_STYLE );
#endif
}



//=============================================================================
// NAME:
// DESC:
//=============================================================================
void PfWindow::SetLoopBreakTime( dword_t time )
{
    m_LoopBreakTime = time;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool PfWindow::PeekEvent()
{
#ifdef _WINDOWS
    MSG msg;
    if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) // if a message is available
    {
        //DWORD curmsg = msg.message;
        if ( !GetMessage( &msg, NULL, 0, 0 ) ) // if retrieves the WM_QUIT message
        {
            return false;
        }
        //      if(msg.message == WM_QUIT)
        //      {
        //          goto ON_EXIT;
        //      }
        TranslateMessage( &msg ); // translates virtual-key message into character message
        DispatchMessage( &msg ); // dispatches a message to a window procedure
    }
    else
    {
        //保證每次Idle 至少會收到一次
        //PfEvent event={ m_window, 0, 0, 0, g_EventPoint.x, g_EventPoint.y };
        //UserCallBackProc(event);
        if( !OnIdle( _bIsActive ) )
        { return false; }
        if( m_LoopBreakTime > 0 )
        { Sleep( m_LoopBreakTime ); }
    }
    //      char buff[255];
    //      sprintf(buff, "%Idel %s\n", i);
    //      OutputDebugString(_T(buff));
    //      i++;
    //         //else                       // do critical game stuff
    //         {
    //             if(!OnIdle(_bIsActive))
    //                 goto ON_EXIT;
    //         }
    /*      else
                // make sure we go to sleep if we have nothing else to do
                WaitMessage();
    */
    /*
    if(!g_bProc)
    {
        //保證每次Idle 至少會收到一次
        PfEvent event={ m_window, 0, 0, g_EventPoint.x, g_EventPoint.y };
        UserCallBackProc(event);
    }
    */
#endif
#ifdef _X11
    XEvent xe;
    XFlush( m_display );
    XNextEvent( m_display, &xe );  /* 取得下一個 event */
    //if(main_loop(&xe))        /* 處理 event */
    //    return 0;
    //if(XCheckWindowEvent(m_display, m_window,
    if( !OnIdle( true ) )
    { return false; }
#endif
    return true;
}

//=============================================================================
// NAME: MainLoop
// DESC:
//=============================================================================
void PfWindow::MainLoop( void )
{
    while ( 1 )
    {
        if( !PeekEvent() )
        {
            break;
        }
    }
    return;
}



//=============================================================================
// NAME:  IsKeyPress()
// DESC:  return key press status
//=============================================================================
bool PfWindow::IsKeyPress( uint_t key )
{
    return _pckeys[key];
}


//=============================================================================
// NAME:  SetIdleFunc()
// DESC:
//=============================================================================
void PfWindow::SetIdleFunc( bool ( *AppIdle )( int ) )
{
    OnIdle = AppIdle;
    return;
}

//=============================================================================
// NAME:  SetProcFunc()
// DESC:
//=============================================================================
void PfWindow::SetProcFunc( bool ( *Procfunc )  ( PfEvent event ) )
{
    UserCallBackProc = Procfunc;
    return;
}


//=============================================================================
// NAME:  SetDestroyFunc()
// DESC:
//=============================================================================
void PfWindow::SetDestroyFunc  ( void ( *AppDestroy )( void ) )
{
    UserDestroy = AppDestroy;
    return;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
LG::Point PfWindow::GetMousePosition( void )
{
    return g_EventPoint;
}


/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
PfEvent PfWindow::GetEventState(void)
{
    PfEvent event = g_Event;
    g_Event.key=0;
    g_Event.uMsg=0;
    return event;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void PfWindow::ClearEventState(void)
{
    //g_Event.clear();
    g_Event.uMsg=0;
    g_Event.key =0;
}
*/


//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool PfWindow::IsExist( void )
{
    if( m_window != NULL )
    { return true; }
    return false;
}



//=============================================================================
// NAME:  Position()
// DESC:  get windows position
//=============================================================================
LG::Point PfWindow::Position( void )
{
    LG::Rect rt;
#ifdef _WINDOWS
    GetWindowRect( m_window, &rt );
#endif
#ifdef _X11
#endif
    //return rt.p_r_t();
    return rt.p_l_t();  // 為什麼會傳那個? 太扯了
}



//=============================================================================
// NAME:
// DESC:  changes the dimensions of the specified window
//=============================================================================
int PfWindow::Resize( LG::Rect WindowRect )
{
#ifdef _WINDOWS
    m_area.width  = WindowRect.width();
    m_area.height = WindowRect.height();
    return MoveWindow( m_window, WindowRect.left, WindowRect.top, WindowRect.right, WindowRect.bottom, true );
#endif
#ifdef _X11
    XWindowChanges values;
    values.x = WindowRect.left;
    values.y = WindowRect.top;
    values.width  = WindowRect.width();
    values.height = WindowRect.height();
    dword_t vmask  = CWX | CWY | CWWidth | CWHeight;
    XConfigureWindow( m_display, m_window, vmask, &values );
    XFlush( m_display );
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return 0;
}

//=============================================================================
// NAME:
// DESC:  changes the position and dimensions of the specified window
//=============================================================================
int PfWindow::Move( int x, int y )
{
#ifdef _WINDOWS
    return MoveWindow( m_window, x, y, m_area.width, m_area.height, true );
#endif
#ifdef _X11
    XWindowChanges values;
    values.x = x;
    values.y = y;
    values.width  = m_area.width;
    values.height = m_area.height;
    dword_t vmask  = CWX | CWY;
    XConfigureWindow( m_display, m_window, vmask, &values );
    XFlush( m_display );
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return 0;
}


//=============================================================================
// NAME:
// DESC:  Activates the window and displays it in its current size and position.
//          Hides the window and activates another window.
//=============================================================================
int PfWindow::Show( bool flag )
{
#ifdef _WINDOWS
    if( flag )
    { 
        return ShowWindow( m_window, SW_SHOW ); 
    }
    return ShowWindow( m_window, SW_HIDE );
#endif
#ifdef _X11
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return 0;
}


//=============================================================================

// NAME:
// DESC:  Maximizes the specified window
//=============================================================================
int PfWindow::Maximize( void )
{
#ifdef _WINDOWS
    return ShowWindow( m_window, SW_MAXIMIZE );
#endif
#ifdef _X11
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return 0;
}



//=============================================================================
// NAME:
// DESC:  Minimizes the specified window and activates the next top-level window in the Z order.
//=============================================================================
int PfWindow::Minimize( void )
{
#ifdef _WINDOWS
    return ShowWindow( m_window, SW_MINIMIZE );
#endif
#ifdef _X11
#endif
    Pf_ERROR( "PfWindow: Not a support system" );
    return 0;
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int PfWindow::Focus( void )
{
#ifdef _WINDOWS
    if( SetFocus( m_window ) )
    { return 1; }
#endif
    return -1;
}

//=============================================================================
// NAME: MsgBox
// DESC: Print Message box if necessary
//=============================================================================
void PfWindow::MsgBox( const tchar_t* message, const tchar_t* title )
{
#ifdef _WINDOWS
    MessageBoxA( ( HWND )m_window, message, title, MB_OK );
#endif
}

///////////////////////////////////////////////////////////////////////////////
//  windows func

//=============================================================================
// NAME:
// DESC:
//=============================================================================
int PfWindow::w32DragAcceptFiles ( bool fAccept )
{
#ifdef _WINDOWS
    DragAcceptFiles( m_window, fAccept );
#endif
    Pf_ERROR( "PfWindow: Not a win32 application" );
    return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
#ifdef _WINDOWS
Plresult CPDefWndProc( Pwnd m_window, uint_t uMsg, Pwparam wParam, Plparam lParam )
{
    g_bProc = true;
    bool do_default = true;
    if( UserCallBackProc != NULL )
    {
        /*
        if(uMsg != g_Event.back().uMsg)
        {
            g_Event.push_back(event);
        }
        g_Event.uMsg = uMsg;
        g_Event.key  = wParam;
        g_Event.x    = LOWORD(lParam);
        g_Event.y    = HIWORD(lParam);
        */
        switch ( uMsg )                          // Check For Windows Messages
        {
        case WM_MOUSEMOVE:
            {
                g_EventPoint.x = LOWORD( lParam );
                g_EventPoint.y = HIWORD( lParam );
            }
            break;
        case WM_MOVE:
            {
                g_EventPoint.x = LOWORD( lParam );
                g_EventPoint.y = HIWORD( lParam );
            }
            break;
        case WM_ACTIVATEAPP:
            {
                _bIsActive = ( bool )wParam;
            }
            break;
        case WM_SYSCOMMAND:                     // Intercept System Commands
            {
                switch ( wParam )               // Check System Calls
                {
                case SC_SCREENSAVE:         // Screensaver Trying To Start?
                    break;
                case SC_MONITORPOWER:       // Monitor Trying To Enter Powersave?
                    break;                 // Jump Back
                }
            }
            break;                          // Exit
        case WM_LBUTTONDOWN:
            _pckeys[VK_LBUTTON] = TRUE;
            break;
        case WM_LBUTTONUP:
            _pckeys[VK_LBUTTON] = FALSE;
            break;
        case WM_RBUTTONDOWN:
            _pckeys[VK_RBUTTON] = TRUE;
            break;
        case WM_RBUTTONUP:
            _pckeys[VK_RBUTTON] = FALSE;
            break;
        case WM_MBUTTONDOWN:
            _pckeys[VK_MBUTTON] = TRUE;
            break;
        case WM_MBUTTONUP:
            _pckeys[VK_MBUTTON] = FALSE;
            break;
        case WM_KEYDOWN:                    // Is A Key Being Held Down?
            {
                _pckeys[wParam] = TRUE;     // If So, Mark It As TRUE
            }
            break;                          // Jump Back
        case WM_KEYUP:                      // Has A Key Been Released?
            {
                _pckeys[wParam] = FALSE;    // If So, Mark It As FALSE
            }
            break;                          // Jump Back
        case WM_CLOSE:                      // Did We Receive A Close Message?
            {
                PostQuitMessage( 0 );       // Send A Quit Message
            }
            break;                          // Jump Back
        case WM_DESTROY:
            {
                PostQuitMessage( 0 );       // Send A Quit Message
            }
            break;
            /*
            case WM_GETMINMAXINFO:
            {
                MINMAXINFO      *pMinMax;
                // Fix the size of the window to 640x480 (client size)
                pMinMax = (MINMAXINFO *)lParam;
                pMinMax->ptMinTrackSize.x = 800+GetSystemMetrics(SM_CXSIZEFRAME)*2;
                pMinMax->ptMinTrackSize.y = 600+GetSystemMetrics(SM_CYSIZEFRAME)*2
                                               +GetSystemMetrics(SM_CYMENU);
                pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
                pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
                break;
            }
            */
        }
        PfEvent event = { m_window, uMsg, wParam, lParam, g_EventPoint.x, g_EventPoint.y };
        if( UserCallBackProc( event ) )
        { do_default = false; }
    }
    if( uMsg != WM_IME_CHAR )
    {
        if( do_default )
        { return DefWindowProc( m_window, uMsg, wParam, lParam ); }
    }
    else
    { return 0L; }
    return FALSE;
}   /* WindowProc */
#endif


//=============================================================================
// NAME:
// DESC:
//=============================================================================
Pmgc PfWindow::CreateCanvas( void )
{
#ifdef _WINDOWS
    if( m_window != NULL )
    { return GetDC( m_window ); }
#endif
#ifdef _X11
    XGCValues gcvalue;
    if( ( m_display != NULL ) && ( m_window != NULL ) )
    { return gc = XCreateGC( m_display, m_window, 0, &gcvalue ); }
#endif
    return NULL;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int_t PfWindow::ReleaseCanvas( Pmgc gc )
{
#ifdef _WINDOWS
    if( m_window != 0 )
    { return ReleaseDC( m_window, gc ); }
#endif
#ifdef _x11
    if( m_window != 0 )
    {
        XFreeGC( m_display, gc );
        return 1;
    }
#endif
    return 0;
}




#ifdef _WINDOWS
bool w32DefineProperty( WNDCLASSEX* property )
{
    if( property == 0 )
    { return FALSE; }
    HINSTANCE hInstance = GetModuleHandle( NULL );
    property->cbSize = sizeof( WNDCLASSEX );
    property->style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; // Redraw On Move, And Own DC For Window
    //property->style            = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // Redraw On Move, And Own DC For Window
    property->lpfnWndProc      = ( WNDPROC ) CPDefWndProc;           // WndProc Handles Messages
    property->cbClsExtra       = 0;                                  // No Extra Window Data
    property->cbWndExtra       = 0;                                  // No Extra Window Data
    property->hInstance        = hInstance;                          // Set The Instance
    property->hIcon            = LoadIcon( hInstance, IDI_WINLOGO ); // Load The Default Icon
    property->hIconSm          = LoadIcon( hInstance, IDI_WINLOGO ); // Load The Default Icon
    property->hCursor          = LoadCursor( NULL, IDC_ARROW );      // Load The Arrow Pointer
    property->hbrBackground    = ( HBRUSH ) GetStockObject( NULL_BRUSH ); // No Background Required For GL
    property->lpszMenuName     = NULL;                               // We Don't Want A Menu
    property->lpszClassName    = CLASSNAME;                     // Set The Class Name
    return TRUE;
}

bool w32Create( LG::Boxi size, void* param )
{
    return TRUE;
}
#endif
#ifdef _X11
bool x11Create( LG::Boxi size, void* param )
{
    return TRUE;
}
#endif
#ifdef _XBOX
bool xboxCreate( LG::Boxi size, void* param )
{
    return TRUE;
}

#endif
#ifdef _PS2
void InitGraphics( void )
{
    // init GS board
    sceGsResetGraph( 0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME );
    // set the pixel mode, screen size, depth test, zbuffer, draw clear
    sceGsSetDefDBuff( &db, SCE_GS_PSMCT32, SCRN_W, SCRN_H,
                      SCE_GS_ZGREATER, SCE_GS_PSMZ24, SCE_GS_CLEAR );
}

void InitSystem( void )
{
    // resets the entire VIF0 including VIF0-FIF0
    sceDevVif0Reset();
    // resets the entire VIF1 including VIF1-FIF0
    sceDevVif1Reset();
    // reset Vu0
    sceDevVu0Reset();
    // reset Vu1
    sceDevVu1Reset();
    // resets devices on the data path to GS (VIF1,Vu0,GIF)
    sceGsResetPath();
    // resets the DMAC
    sceDmaReset( 1 );
    return;
}

bool ps2Create( LG::Boxi size, void* param )
{
    sceSifInitRpc( 0 );
    // Reboot IOP and load IOP modules
    if ( DEVICE == 0 )
    {
        // Reboot IOP with image from host
        while ( !sceSifRebootIop( "host0:modules/ioprp243.img" ) );
        //while(!sceSifRebootIop(IOPIMG));
        while ( !sceSifSyncIop() );
        sceSifInitRpc( 0 );
        sceFsReset();
        // Load modules from host
        while ( sceSifLoadModule( "host0:modules/sio2man.irx", 0, NULL ) < 0 );
        while ( sceSifLoadModule( "host0:modules/padman.irx",  0, NULL ) < 0 );
        while ( sceSifLoadModule( "host0:modules/libsd.irx",   0, NULL ) < 0 );
        while ( sceSifLoadModule( "host0:modules/sdrdrv.irx",  0, NULL ) < 0 );
        while ( sceSifLoadModule( "host0:modules/usbd.irx",    0, NULL ) < 0 );
        while ( sceSifLoadModule( "host0:modules/usbkb.irx",   0, NULL ) < 0 );
    }
    else
    {
        // Initialize CD/DVD drive
        sceCdInit ( SCECdINIT );
        sceCdMmode( MEDIA );
        // Reboot IOP with image from CD/DVD-ROM
        while ( !sceSifRebootIop( "cdrom0:\\MODULES\\IOPRP243.IMG;1" ) );
        while ( !sceSifSyncIop() );
        sceSifInitRpc( 0 );
        sceFsReset();
        // Re-initialize CD/DVD drive
        sceCdInit ( SCECdINIT );
        sceCdMmode( MEDIA );
        // Load modules from CD/DVD-ROM
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\SIO2MAN.IRX;1", 0, NULL ) < 0 );
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\PADMAN.IRX;1",  0, NULL ) < 0 );
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\LIBSD.IRX;1",   0, NULL ) < 0 );
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\SDRDRV.IRX;1",  0, NULL ) < 0 );
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\USBD.IRX;1",    0, NULL ) < 0 );
        while ( sceSifLoadModule( "cdrom0:\\MODULES\\USBKB.IRX;1",   0, NULL ) < 0 );
    }
    // Initialize IOP memory allocation
    sceSifInitIopHeap();
    // Declare variables for compatibility
    //m_window = NULL;
    //int window = 0;
    // Set display size for application
    //int displaywidth  = w;
    //int displayheight = h;
    // Fall through to common entry point
    return TRUE;
}

#endif
