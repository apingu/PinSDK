// windoze.cpp : a skeleton of Micro$oft Windoze application
//
//  Developer   Date    Comments
//  yann        990406  little modified
//              991031  little modified

#define APP_NAME    "ASK"
#define APP_TITLE   "ASK for Windoze 95/98/NT4/2000"

#include <windows.h>
#include <mmsystem.h>


// Define the window style of application
#if   defined _USE_DDRAW32

    #define CLS_STYLE      CS_DBLCLKS    // specifies the class style
    #define EX_WND_STYLE   WS_EX_TOPMOST // specifies the extended style of the window
    #define WND_STYLE      WS_POPUP      // specifies the window style

#elif defined _USE_GDI32

    #define CLS_STYLE      (CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW)
    #define EX_WND_STYLE   NULL
    #define WND_STYLE      (WS_CAPTION | WS_DLGFRAME)

#else

    #define CLS_STYLE      (CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW)
    #define EX_WND_STYLE   NULL
    #define WND_STYLE      WS_OVERLAPPEDWINDOW

#endif  // _USE_DDRAW32 or _USE_GDI32


HWND  hMainWnd;             // the handle of main window
POINT MouseSite;

BOOL  bIsActive   = FALSE;  // is window activated?
BOOL  bIsAppReady = FALSE;  // is application ready?

char  szCurrentDir[_MAX_PATH];


BOOL  InitWinApp(HINSTANCE hInstance, int nCmdShow);
BOOL  OnAppCreate(HWND hWnd);
void  OnAppDestroy(void);
void  OnAppIdle(void);

// static int iMessage, iX, iY, iKey = 0;


/////////////////////////////////////////////////////////////////////////////
// WinMain - initialization, message loop

int  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex;
    MSG    msg;

    // Create a named mutex object
    hMutex = CreateMutex(NULL, TRUE, APP_NAME);
    switch (GetLastError())
    {
    case NO_ERROR:
        break;
    case ERROR_ALREADY_EXISTS:
        return 0;
    }

    // Remember where are we started
    GetCurrentDirectory(_MAX_PATH, szCurrentDir);

    if (szCurrentDir[lstrlen(szCurrentDir) - 1] != '\\')
        lstrcat(szCurrentDir, "\\");

    // Prepare the window
    if (!InitWinApp(hInstance, nCmdShow))
        return 0;

    // Prepare the application
    if (!OnAppCreate(hMainWnd))
        return 0;

    // The message loop
    while (1)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))  // if a message is available
        {
            if (!GetMessage(&msg, NULL, 0, 0))  // if retrieves the WM_QUIT message
            {               
                CloseHandle(hMutex);
                return msg.wParam;
            }

            TranslateMessage(&msg); // translates virtual-key message into character message
            DispatchMessage(&msg);  // dispatches a message to a window procedure
        }
        else if (bIsActive)         // do critical game stuff
        {
            OnAppIdle();    
        }
/*      else
            // make sure we go to sleep if we have nothing else to do            
            WaitMessage(); 
*/
    }

    return msg.wParam;
}   /* WinMain */


/////////////////////////////////////////////////////////////////////////////
// WindowProc - message processing

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    static PAINTSTRUCT ps;
  
    // Remember user input right now for later use!
    // This is for test, normally use Set/GetUserInput(..) instead
/*  iKey = iMessage = 0;

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
		MouseSite.x = LOWORD(lParam);
		MouseSite.y = HIWORD(lParam);
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        iMessage = uMsg;
        iX = LOWORD(lParam);
        iY = HIWORD(lParam);
        break;

    case WM_KEYDOWN:
        iKey = wParam;
        break;
    } */

    // Deal with system messages of windoze
    switch (uMsg)
    {
#ifndef _USE_DDRAW32
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
//      RenderDeviceContext(hdc);
        EndPaint(hWnd, &ps);
        return 0;        
#endif  // _USE_DDRAW_32
        
    case WM_CREATE:
        hMainWnd  = hWnd;
        break;

    case WM_ACTIVATEAPP:
        bIsActive = (BOOL) wParam;
        break;

/*  case WM_SETCURSOR:
        SetCursor(NULL);    // Hide the mouse cursor
        return TRUE;
*/
    case WM_KEYDOWN:
        switch (wParam)
        {         
        case VK_ESCAPE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;    
        }
        break;        

    case WM_DESTROY:
        OnAppDestroy();
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}   /* WindowProc */


/////////////////////////////////////////////////////////////////////////////
// InitWinApp - create the window and do initialization

BOOL InitWinApp(HINSTANCE hInstance, int nCmdShow)
{
    HWND       hWnd;
    WNDCLASSEX wcex;
    char       szAppName[] = APP_NAME;

    // Set up and register a window class 
    wcex.cbSize        = sizeof(wcex);
    wcex.style         = CLS_STYLE;         // class style                   
    wcex.lpfnWndProc   = WindowProc;        // window function
    wcex.cbClsExtra    = 0;                 // no extra count of bytes
    wcex.cbWndExtra    = 0;                 // no extra count of bytes
    wcex.hInstance     = hInstance;
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION); 
                      // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)); 
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
                      // NULL;
    wcex.hbrBackground = (HBRUSH) GetStockObject(NULL_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = szAppName;         // window class name
    wcex.hIconSm       = NULL;                                     

    if (RegisterClassEx(&wcex) == 0)
        return FALSE;  
    
    // Want a 640x480 window in screen center
    RECT rect;
    int  x, y, width, height;

    rect.left   = GetSystemMetrics(SM_CXSCREEN) / 2 - 320;
    rect.top    = GetSystemMetrics(SM_CYSCREEN) / 2 - 240;
    rect.right  = rect.left + 640;
    rect.bottom = rect.top  + 480;

    AdjustWindowRect(&rect, WND_STYLE, FALSE);

    x      = rect.left;
    y      = rect.top;
    width  = rect.right - rect.left;
    height = rect.bottom - rect.top;

    // or want a full screen size window
/*  x      = CW_USEDEFAULT,               
    y      = CW_USEDEFAULT,               
    width  = GetSystemMetrics(SM_CXSCREEN)
    height = GetSystemMetrics(SM_CXSCREEN)
*/
    // Create an overlapped, pop-up, or child window with an extended style
    hWnd = CreateWindowEx(
                EX_WND_STYLE,       // extended window style
                szAppName,          // window class name
                APP_TITLE,          // window caption
                WND_STYLE,          // window style
                x,                  // initial x position
                y,                  // initial y position
                width,              // initial x size
                height,             // initial y size
                NULL,               // parent window handle
                NULL,               // window menu handle
                hInstance,          // program instance handle
                NULL);              // creation parameters

    if (hWnd == NULL)
        return FALSE;

    // Show this window
    SetFocus(hWnd);    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}   /* InitWinApp */


/////////////////////////////////////////////////////////////////////////////
// OnAppCreate - create the video device and initialize data

BOOL OnAppCreate(HWND hWnd)
{
    // Create the video device
//  OpenVideo(hWnd);
   
    // Create and initialize the game data
//  bIsAppReady = OnGameCreate();

    return bIsAppReady;
}  

void OnAppDestroy(void)
{
    // Destroy the game data
//  OnGameDestroy();

    // Shutdown the video device
//  CloseVideo();
}  

void OnAppIdle(void)
{
    // Extra process routine
//  TestProc(iMessage, iKey, iX, iY);

    // Idle routine
//  OnGameIdle();

    // Update video frame
//  UpdateVideo();   
}
