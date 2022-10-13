/////////////////////////////////////////////////////////////////////////////
// WindowProc - message processing

LRESULT Proc(POINT point, UINT uMsg, UINT wParam)
{
    static HDC hdc;
    static PAINTSTRUCT ps;

    // Remember user input right now for later use!
    // This is for test, normally use Set/GetUserInput(..) instead
    // Deal with system messages of windoze

    switch (uMsg)
    {
#ifndef _USE_DDRAW32
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
		// RenderDeviceContext(hdc);
		EndPaint(hWnd, &ps);
        return 0;        
#endif  // _USE_DDRAW_32

    case WM_CREATE:
        hMainWnd  = hWnd;
        break;

    case WM_ACTIVATEAPP:
        bIsActive = (BOOL) wParam;
        break;

	case WM_MOUSEMOVE:

		
		break;

	case WM_LBUTTONDOWN:


		break;
	case WM_RBUTTONDOWN:
	
		
	    break;
	case WM_SETCURSOR:
        SetCursor(NULL);    // Hide the mouse cursor
        return TRUE;

    case WM_KEYDOWN:
        switch (wParam)
        {

		case VK_RIGHT:
	
			break;
		case VK_LEFT:



			break;
		case VK_DOWN:


			break;
		case VK_UP:

			break;

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
