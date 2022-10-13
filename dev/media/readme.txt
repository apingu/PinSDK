
//=============================================================================
//  init
    AMovie.SetDirectDraw((IDirectDraw *)GDX.pDD, (LPDIRECTDRAWSURFACE)GDX.pPri);




//=============================================================================
//  play



    if (AMovie.LoadFile(".\\InterFace\\H01") == FALSE)
        return ;
    AMovie.SetDestRect(100, 50, 900, 650);
    PCPOINT pt;
    HPINMAP* aviCanvas = GDX.CreateVisualMemCanvas(800, 600);

    PCPOINT point(0, 0);
    ClientToScreen(hMainWnd, &point);
    
    int ret = -1;
    while(AMovie.RenderFrameToPrimary() != FALSE)
    {
    if(GetAsyncKeyState(VK_LBUTTON))
            goto EXIT;
     AMovie.Play();
    }

EXIT:

    GDX.VMCDestroy(aviCanvas);
    AMovie.Release();
    GameProcess = OnMainMenuIdle;

