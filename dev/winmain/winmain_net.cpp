#define	ARGB(a,	r, g, b)       ((D3DCOLOR) ( ((a) << 24)	| ((r) << 16) | ((g) << 8) | (b)))
//-----------------------------------------------------------------------------
#define	CLASS_NAME "G1"
#define	TITLE_NAME  "G1"
//-----------------------------------------------------------------------------

#define	D3D_OVERLOADS
#define	STRICT

#include <AfxWin.h>
#include <AfxExt.h>
#include <mmsystem.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "..\ALL\cdaudio.h"
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <process.h>
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#include "WINSOCK.H"

#include "resource.h"

#include "..\COM\Global.h"
#include "..\COM\All_App.h"

#pragma	comment	(lib,"amstrmid.lib")
#pragma	comment	(lib,"dxguid.lib")
#pragma	comment	(lib,"ddraw.lib")
#pragma	comment	(lib,"dsound.lib")
//#pragma comment (lib,"d3dim.lib")
//#pragma comment (lib,"d3drm.lib")
//#pragma comment (lib,"d3dx.lib")
#pragma	comment	(lib,"winmm.lib")
#pragma	comment	(lib,"imm32.lib")



//-----------------------------------------------------------------------------

MSG  msg;
HWND h_wnd;
HINSTANCE hInst;

//-- Debug Use ----------------------------------------------------------------

int	Bug_count1 = 0;
int	Bug_count2 = 0;
char	Bug_str[80];

//-----------------------------------------------------------------------------

#include "..\COM\PIC_2D.CPP"
#include "..\COM\MENU_2D.CPP"
#include "..\COM\MAN_LINK.CPP"
#include "..\COM\ANI_LINK.CPP"
#include "SAV_LINK.CPP"

//-----------------------------------------------------------------------------

#include "..\COM\WSOCK.CPP"

//-----------------------------------------------------------------------------

CRect	winrect;
static	int  mousex;
static	int  mousey;

//--- CD Use ------------------------------------------------------------------

BYTE	Now_track;
BYTE	Now_Minute;
BYTE	Now_Second;
BYTE	All_Tracks;
BYTE	CD_OK =	0;

//--- Timer Use	---------------------------------------------------------------

static	DWORD	lastTickCount[20] =  {
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0
				    };
static	DWORD	More_Time[20] =	 {
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0,
				       0, 0, 0,	0, 0
				    };
DWORD		thisTickCount;

char	Timer_Str[20][20]= {
			    "1/100 sec",
			    "1/60 sec",
			    "1/56 sec",
			    "1/48 sec",
			    "1/40 sec",
			    "1/30 sec",
			    "1/24 sec",
			    "1/16 sec",
			    "1/12 sec",
			    "1/8 sec",

			    "1/6 sec",
			    "1/4 sec",
			    "1/3 sec",
			    "1/2 sec",
			    "1 sec",
			    "2 sec",
			    "3 sec",
			    "4 sec",
			    "5 sec",
			    "8 sec"
			   };

//--- Animation	Use -----------------------------------------------------------

int	Make_Ani_num=0;	 //
int	Make_x	    =0;	 //
int	Make_y	    =0;	 //
int	Make_frame_count=0;  //
char	Ani_speed=7;
BYTE	Ani_Play_Flag=0;
BYTE	Ani_Set_Flag=0;
BYTE	Ani_Set_trace_fg = 0;
int	Bk_Icon_count=0;

//-----	Animation Switch ------------------------------------------------------

char	Animation_SW = 1;

//-----	Mouse Use -------------------------------------------------------------

BYTE	L_mouse_down_now  = 0;
BYTE	R_mouse_down_now  = 0;
BYTE	L_mouse_down = 0;
BYTE	L_mouse_up   = 0;
BYTE	R_mouse_down = 0;
BYTE	R_mouse_up   = 0;

//-----	Wave Use --------------------------------------------------------------

BYTE	Set_wav_flag = 0;
BYTE	Load_wav_flag =	0;

//-----	Set frame use ---------------------------------------------------------

BYTE	Set_frame_Flag =  0;

//-----	Ani File  use ---------------------------------------------------------

BYTE	Ani_File_Flag  =  0;

//-----------------------------------------------------------------------------

BYTE	T_flag = 0;  //	Show Text flag
BYTE	FPS_flag = 0;  // Only Show FPS	 flag

//-----	Att Use	---------------------------------------------------------------

WORD	Att_buf[40*30];
int	Att_w=1;
int	Att_h=1;
int	BK_Att_x;
int	BK_Att_y;

BYTE	GetAtt_Flag = 0;
BYTE	Input_pic_att_fg = 0;
BYTE	Input_num_count	= 0;
WORD	Input_num	= 0;

//--- Map Use -----------------------------------------------------------------

ICON_DATA Map_Up;

WORD	Map_data_buf[9*7];  // Render ground map use
WORD	Map_buf[48];	    // mouse use
int	Map_buf_w;
int	Map_buf_h;

//-- Get Pic Use --------------------------------------------------------------

WORD Getpic_number = 0;
int  Getpic_x	   = 0;
int  Getpic_y	   = 0;
int  Getpic_w	   = 0;
int  Getpic_h	   = 0;
int  Start_mousex = 0;
int  Start_mousey = 0;

//-- Render Use	---------------------------------------------------------------

static int	mouse_Count=0;
static BYTE	Time_move[20] =	{
				  0, 0,	0, 0, 0,
				  0, 0,	0, 0, 0,
				  0, 0,	0, 0, 0
				 };
static BYTE	Flip_Count=0;

//-- Key in Dialog use	-------------------------------------------------------

HWND	Key_hDlg;
BYTE	KeyDown_FG;

//-----------------------------------------------------------------------------

BYTE	Pass_flag = 0;
char	Game_Count_page	= 0;

//-----	Play Wav Use ----------------------------------------------------------
static BYTE	wav_ever_play_fg[200] =	 {
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0,
				       0, 0, 0,	0, 0, 0, 0, 0, 0, 0
				    };

#include "..\COM\M_DLG.CPP"

#include "..\COM\a_star.cpp"
#include "..\COM\TEXT.CPP"
#include "..\COM\LoadTga.CPP"
#include "..\COM\AMovie.CPP"
#include "..\COM\China.CPP"
#include "..\ALL\CDAudio.CPP"
#include "..\COM\Psound.CPP"
#include "..\COM\Alpha.CPP"
#include "..\COM\Tool.CPP"
#include "ATT.CPP"
#include "..\COM\Walk.CPP"
#include "Key_Do.CPP"
#include "Mouse_Do.CPP"
#include "..\COM\LoadSave.CPP"
#include "..\COM\GameTool.CPP"
//#include "M_sw.CPP"
#include "M_show.CPP"
#include "..\COM\Render2.CPP"
#include "..\COM\M_Render.CPP"
#include "Render.CPP"
#include "..\COM\Init.cpp"
#include "Command.cpp"

//-----------------------------------------------------------------------------
// Name: WndProc()
//-----------------------------------------------------------------------------

#define	SEC_TIMER	1

LRESULT	CALLBACK WndProc( HWND hWnd, UINT uMsg,	WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
	case WM_CREATE:
	     SetTimer(hWnd,SEC_TIMER , 1000 , NULL);
	     return 0;



	case WSA_READ:
	     if( On_Line_fg == TRUE )
	       Read_Data(hWnd, wParam, lParam );
	     break;

	case WM_CHAR:
	     ASC_fg = 1;
	     KeyAscii =	(BYTE)wParam;
	     return 0;

	case WM_KEYDOWN:
	     KeyDown_FG	= 1;
	     KeyDown(wParam);
	     return 0;

	case WM_KEYUP:
	     KeyDown_FG	= 0;
	     KeyUp(wParam);
	     return 0;

	case WM_MOUSEMOVE:
	     mousex = LOWORD(lParam);
	     mousey = HIWORD(lParam);
	     return 0;

	case WM_LBUTTONDOWN:
	     L_mouse_down = 1;
	     L_mouse_down_now= 1;
	     return 0;

	case WM_LBUTTONUP:
	     L_mouse_down_now= 0;
	     L_mouse_up	= 1;
	     return 0;

	case WM_RBUTTONDOWN:
	     R_mouse_down = 1;
	     R_mouse_down_now =	1;
	     return 0;

	case WM_RBUTTONUP:
	     R_mouse_up	  = 1;
	     R_mouse_down_now =	0;
	     return 0;

	case WM_TIMER:
#ifndef	 USE_MAP_EDIT
	     if( wParam	== SEC_TIMER )
	       S_D.Game_Time++;
#endif	//USE_MAP_EDIT
	     return 0;

	case WM_CLOSE:
	     DestroyWindow( hWnd );
	     return 0;

	case WM_DESTROY:
	     Free_All();
	     PostQuitMessage(0);
	     return 0L;

    }

    return DefWindowProc( hWnd,	uMsg, wParam, lParam );
}



//-----------------------------------------------------------------------------
// Name: WinMain()
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR strCmdLine, INT )
{
    // Check have other	programming
    BOOL	bFound = FALSE;
    HANDLE	hMutex = NULL;

    #ifdef	_WIN32
	hMutex = CreateMutex(NULL,TRUE,CLASS_NAME);
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	  bFound = TRUE;
    #else
	if( hPrevInstance != NULL)
	  bFound = TRUE;
    #endif

	if( bFound )
	{
		HWND pFirstWnd = NULL;
		if( pFirstWnd==	FindWindow(NULL,CLASS_NAME))
		{
			if(IsIconic(pFirstWnd))
			  ShowWindow(pFirstWnd,SW_SHOWNORMAL);

			BringWindowToTop(pFirstWnd);

			BringWindowToTop(GetLastActivePopup(pFirstWnd));
		}
		return FALSE;
	}

    #ifdef	_WIN32
	ReleaseMutex(hMutex);
    #endif


    // Register	the window class
    WNDCLASS wndClass =	{
			  CS_HREDRAW | CS_VREDRAW,
			  WndProc,
			  0,
			  0,
			  hInstance,
			  NULL,
			  LoadCursor(NULL, IDC_ARROW),
			  (HBRUSH)GetStockObject(BLACK_BRUSH),
			  NULL,
			  CLASS_NAME
			};
    RegisterClass( &wndClass );

    // Create our main window

    int	W_X = (GetSystemMetrics(SM_CXSCREEN) - 640 )/2;
    int	W_Y = (GetSystemMetrics(SM_CYSCREEN) - 480 )/2;

    h_wnd= CreateWindowEx(WS_EX_APPWINDOW	,
			  CLASS_NAME,
			  TITLE_NAME,
                          WS_POPUP,
			  W_X,
			  W_Y,
			  640,480,
                          NULL,
                          NULL,
			  hInstance,
			  NULL);
    hInst = hInstance;

    ShowWindow( h_wnd, SW_SHOWNORMAL );
    UpdateWindow( h_wnd );

    Init_All();


    while(1)
    {

	while( PeekMessage( &msg, NULL,	0U, 0U,	PM_REMOVE ) )
        {
	    if(	msg.message == WM_CLOSE) return	msg.wParam;
	    if(WM_QUIT == msg.message) return msg.wParam;
	    TranslateMessage( &msg );
	    DispatchMessage( &msg );
	}
	Render();
	if( Quit_Fg == 1 )
	  PostMessage( h_wnd,WM_CLOSE ,0,0);
    }

    return msg.wParam;
}
