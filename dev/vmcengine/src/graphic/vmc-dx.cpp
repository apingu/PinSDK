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
//                                               Copyright    (C)    1999    Pin
//
#if defined(_WINDOWS)

#include "vmc-dx.h"
#include "vmc-clipper.h"
#include <memio.inl>


#pragma comment (lib, "ddraw.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "D3d9.lib")



namespace LGB
{

    extern  word_t  _PIXEL_WIDTH_SHIFT;

    //=============================================================================
    //
    //
    //=============================================================================
    VMCDX::VMCDX()
    {
        pDD = 0;
        pDD2 = 0;

        ZeroMemory( &pDD, sizeof( LPDIRECTDRAW ) );
        ZeroMemory( &pDD2, sizeof( LPDIRECTDRAW7 ) );
        ZeroMemory( &pPri, sizeof( LPDIRECTDRAWSURFACE7 ) );
        ZeroMemory( &pBack, sizeof( LPDIRECTDRAWSURFACE7 ) );
        ZeroMemory( &Desc, sizeof( DDSURFACEDESC2 ) );
        ZeroMemory( &pClip, sizeof( LPDIRECTDRAWCLIPPER ) );


        p_canvas_ = NULL;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    VMCDX::VMCDX( HWND hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool screenflag )
    {
        if( !Create( hWnd, x, y, width, height, depth, screenflag ) )// Set Direct X Mode
            exit( 1 );
    }

    //=============================================================================
    //
    //
    //=============================================================================
    VMCDX::~VMCDX()
    {
        ReleaseSurface();
        Destroy();
    }


    //=============================================================================
    // NAME: DXInit()
    // DESC: function to create directX
    //
    //    GDX.SetScreenMode(WINDOWS);
    //    if(!GDX.DXInit(hMainWnd))// Set Direct X Mode
    //        exit(1);
    //=============================================================================
    bool VMCDX::Create( void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool screenflag )
    {
        //IVMCDevice::Create(hWnd, width, height, depth, screenflag);
        if( ( width <= 0 ) || ( height <= 0 ) || ( depth < 0 ) )
        {
            OUTTRACE( "Canvas size error!!. \n" );
            return false;
        }

        vobj_hwidget_ = hWnd;

        if( DirectDrawCreate( NULL, &pDD, NULL ) != DD_OK ) //建立一個DirectDraw物件
        {
            OUTTRACE( "DirectDrawCreate() failed. \n" );
            return false;
        }

        if( pDD->QueryInterface( IID_IDirectDraw7, ( LPVOID* )&pDD2 ) != DD_OK )//建立一個DirectDraw7物件
        {
            OUTTRACE( "QueryInterface() for IDirectDraw7 failed. \n" );
            return false;
        }
        // 先取第一次的WINDOWS style
        m_dwStyle = GetWindowLong( ( HWND )vobj_hwidget_, GWL_STYLE );

        ChangeScreenSize( x, y, width, height, depth, screenflag );

        if( ( p_canvas_ ) != 0 )
            SAFE_DELETE( p_canvas_ );

        width = GetWidth();
        height = GetHeight();
        depth = GetDepthb();

        p_canvas_ = ::new CMemCanvas;
        p_canvas_->Allot( width, height, depth );
        //(*canvas)->Allot(width, height, depth);
        //(*canvas)->width        = width;
        //(*canvas)->height       = height;
        //(*canvas)->GetWidthSize() = width * (*canvas)->depth;
        //(*canvas)->pixels  = new ubyte_t[(*canvas)->height * (*canvas)->GetWidthSize()];
        ZeroMemory( p_canvas_->GetPixel(), p_canvas_->GetClipHeight() * p_canvas_->GetWidthSize() );

        return true;
    }



    //=============================================================================
    // NAME: SetFullScreen
    // DESC: 
    //=============================================================================
    bool VMCDX::ChangeScreenSize( int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen )
    {
        GetClientRect( ( HWND )vobj_hwidget_, &m_rcScreen );
        ClientToScreen( ( HWND )vobj_hwidget_, ( POINT* )&m_rcScreen.left );
        ClientToScreen( ( HWND )vobj_hwidget_, ( POINT* )&m_rcScreen.right );

        if( ( width == 0 ) || ( height == 0 ) || ( depth == 0 ) )
        {
            OUTTRACE( "Canvas size error!!. \n" );
            return false;
        }

        fullscreen_ = fullscreen;

        ReleaseSurface();

        // Get the dimensions of the viewport and screen bounds
        //GetClientRect((HWND)vobj_hwidget_, &m_rcViewport);
        m_rcViewport.setxywh( x, y, width, height );

        if( fullscreen )
        {
            //close system bar
            if( !GetFullScreen() )
                m_dwStyle = GetWindowLong( ( HWND )vobj_hwidget_, GWL_STYLE );

            DWORD dwStyle = WS_CLIPCHILDREN;
            //long dwStyle = GetWindowLong((HWND)vobj_hwidget_, GWL_STYLE);
            SetWindowLong( ( HWND )vobj_hwidget_, GWL_STYLE, dwStyle );

            //GetClientRect((HWND)vobj_hwidget_, &m_rcScreen);
            //ClientToScreen((HWND)vobj_hwidget_, (POINT*)&m_rcScreen.left);
            //ClientToScreen((HWND)vobj_hwidget_, (POINT*)&m_rcScreen.right);


            // Get exclusive mode
            if( pDD2->SetCooperativeLevel( ( HWND )vobj_hwidget_, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) != DD_OK )
            {
                OUTTRACE( "SetCooperativeLevel() failed. \n" );
                return false;
            }

            // Set the video mode to 640x480x16
            if( pDD2->SetDisplayMode( width, height, ( depth * 8 ), 0, 0 ) != DD_OK )
            {
                OUTTRACE( "SetDisplayMode FAILED. \n" );
                return false;
            }

            // Create the primary surface with 1 back buffer
            ZeroMemory( &Desc, sizeof( Desc ) );
            Desc.dwSize = sizeof( Desc );
            Desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
            Desc.dwBackBufferCount = 1;
            if( pDD2->CreateSurface( &Desc, &pPri, NULL ) != DD_OK )
            {
                MessageBoxA( NULL, "Create Surface failed", " Surface Failed", MB_OK );
                return false;
            }

            DDSCAPS2 Scaps;
            ZeroMemory( &Scaps, sizeof( DDSCAPS2 ) );
            Scaps.dwCaps = DDSCAPS_BACKBUFFER;

            if( ( pPri->GetAttachedSurface( &Scaps, &pBack ) ) != DD_OK )
            {
                MessageBoxA( NULL, "BackBuffer attached failed", " Surface Failed", MB_OK );
                return false;
            }

            /*
            ///////////////////////////////
            //
            pDD2->SetDisplayMode(width, height, (depth * 8), 0, 0); // 改模式
            if(pDD2->SetCooperativeLevel((HWND)vobj_hwidget_, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX) != DD_OK) //設定程式的模式 windows
            {
                OUTTRACE("SetCooperativeLevel() failed. \n");
                return false;
            }
            if(!CreateFullScreenPrimary(backc))
                return false;
            pClip=NULL;
            */
        }
        else
        {
            //open system bar
            //DWORD dwStyle=WS_OVERLAPPEDWINDOW;
            //long dwStyle = GetWindowLong((HWND)vobj_hwidget_, GWL_STYLE);
            SetWindowLong( ( HWND )vobj_hwidget_, GWL_STYLE, m_dwStyle );
            // Get normal windowed mode
            if( pDD2->SetCooperativeLevel( ( HWND )vobj_hwidget_, DDSCL_NORMAL ) != DD_OK )
            {
                OUTTRACE( "SetCooperativeLevel() failed. \n" );
                return false;
            }

            //change your windows size first
            m_rcScreen.right = m_rcScreen.left + width - 1;
            m_rcScreen.bottom = m_rcScreen.top + height - 1;
            AdjustWindowRectEx( &m_rcScreen, m_dwStyle, FALSE, NULL ); // Adjust Window To True Requested Size
            SetWindowPos( ( HWND )vobj_hwidget_, HWND_NOTOPMOST,
                m_rcScreen.left, m_rcScreen.top,
                m_rcScreen.width(), m_rcScreen.height(),
                0 );
            // Create the primary surface
            ZeroMemory( &Desc, sizeof( Desc ) );
            Desc.dwSize = sizeof( Desc );
            Desc.dwFlags = DDSD_CAPS;
            Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            if( FAILED( pDD2->CreateSurface( &Desc, &pPri, NULL ) ) )
            {
                MessageBoxA( NULL, "CreatePrimarySurface failed", " Surface Failed", MB_OK );
                return false;
            }

            // Create a clipper object since this is for a Windowed render
            LPDIRECTDRAWCLIPPER pClipper;
            if( pDD2->CreateClipper( 0, &pClipper, NULL ) != DD_OK )
            {
                MessageBoxA( NULL, "CreateClipper FAILED", " Clipper Failed", MB_OK );
                return false;
            }

            // Associate the clipper with the window
            pClipper->SetHWnd( 0, ( HWND )vobj_hwidget_ );
            pPri->SetClipper( pClipper );
            pClipper->Release();
            pClipper = NULL;

            // Get the backbuffer. For fullscreen mode, the backbuffer was created
            // along with the primary, but windowed mode still needs to create one.
            ZeroMemory( &Desc, sizeof( Desc ) );
            Desc.dwSize = sizeof( Desc );
            Desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
            Desc.dwWidth = width;
            Desc.dwHeight = height;
            Desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
            if( FAILED( pDD2->CreateSurface( &Desc, &pBack, NULL ) ) )
            {
                MessageBoxA( NULL, "CreateBackBufferSurface failed", " Surface Failed", MB_OK );
                return false;
            }

            ///////////////////////////////////////////////////////////
            //
            /*
            if(pDD2->SetCooperativeLevel((HWND)vobj_hwidget_, DDSCL_NORMAL) != DD_OK)
            {
                OUTTRACE("SetCooperativeLevel() failed. \n");
                return false;
            }
            if(!CreateWindowsPrimary())
                return false;
            // By Wood: Clipper 不是建不起來就得死, 但是總是建好比較好...
            if(pDD->CreateClipper(0,&pClip,NULL) == DD_OK){
                hrst=pClip->SetHWnd(0,(HWND)vobj_hwidget_);
                hrst=pPri->SetClipper(pClip);
            }
            */
        }

        // 以實際調整之後的為主
        //m_rcViewport.setxywh(x, y, m_rcScreen.width(), m_rcScreen.height());

        //IVMCDevice::ChangeScreenSize(width, height, fullscreen);
        PixelFormat pxfmt = JudgeVideoMask();
        SetPixelMode( x, y, width, height, pxfmt );
        ShowCursor( 0 );

        InvalidateRect( ( HWND )vobj_hwidget_, NULL, false );
        return true;
    }

    //=============================================================================
    // NAME: SetFullScreen
    // DESC: 
    //=============================================================================
    long VMCDX::ChangeCoopLevel( bool bfullscreen )
    {
        ReleaseSurface();

        return ChangeScreenSize( GetPosX(), GetPosY(), GetWidth(), GetHeight(), GetDepthb(), bfullscreen );
    }
    /*
    //=============================================================================
    // NAME: SetFullScreen
    // DESC:
    //=============================================================================
    bool VMCDX::SetFullScreen(int width, int height, int depth, int bkcount)
    {
        if((width <=0) || (height <=0) || (depth < 0))
        {
            OUTTRACE("Canvas size error!!. \n");
            return false;
        }

        m_WidgetSize.width = width;
        m_WidgetSize.height = height;

        pDD2->SetDisplayMode(width, height, (depth * 8), 0, 0); // 改模式
        if(pDD2->SetCooperativeLevel(vobj_hwidget_, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX) != DD_OK) //設定程式的模式 windows
        {
            OUTTRACE("SetCooperativeLevel() failed. \n");
            return false;
        }
        if(!CreateFullScreenPrimary(bkcount))
            return false;

        JudgeVideoMask();
        ShowCursor(0);
        return true;
    }


    //=============================================================================
    // NAME: SetWindow
    // DESC:
    //=============================================================================
    bool VMCDX::SetWindow(int width, int height, int depth)
    {
        m_WidgetSize.width = width;
        m_WidgetSize.height = height;

        if(pDD2->SetCooperativeLevel(vobj_hwidget_, DDSCL_NORMAL) != DD_OK)
        {
            OUTTRACE("SetCooperativeLevel() failed. \n");
            return false;
        }
        if(!CreateWindowsPrimary())
            return false;

        JudgeVideoMask();
        ShowCursor(0);

        return true;
    }
    */

    //=============================================================================
    // NAME: Destroy()
    // DESC: Release all surface
    //=============================================================================-
    void VMCDX::ReleaseSurface( void )
    {
        if( pDD2 != NULL )
        {
            pDD2->SetCooperativeLevel( ( HWND )vobj_hwidget_, DDSCL_NORMAL );
            if( pBack != NULL )
            {
                pBack->Release();
                pBack = NULL;
            }
            if( pPri != NULL )
            {
                pPri->Release();
                pPri = NULL;
            }
        }
    }

    //=============================================================================
    // NAME: Destroy()
    // DESC: Desrtoy all directX object
    //=============================================================================-
    void VMCDX::Destroy()
    {
        if( pDD2 != 0 )
        {
            pDD2->Release();
            pDD2 = 0;
            //SAFE_RELEASE(pDD2);
        }
        if( pDD != 0 )
        {
            pDD->Release();
            pDD = 0;
            //SAFE_RELEASE(pDD);
        }

        SAFE_DELETE( p_canvas_ );

        return;
    }


    /*
    //=============================================================================
    // NAME: CreateWindowsPrimary
    // DESC: create a windows screen surface of directX
    //=============================================================================
    bool VMCDX::CreateWindowsPrimary(void)
    {
        //DWORD dwFlags;

        if(pDD2->SetCooperativeLevel((HWND)vobj_hwidget_, DDSCL_NORMAL) != DD_OK)
        {
            OUTTRACE("SetCooperativeLevel() failed. \n");
            return false;
        }

        //dwFlags = DDSCL_NORMAL;
        //pDD2->SetCooperativeLevel(vobj_hwidget_, dwFlags);

        ZeroMemory(&Desc, sizeof(Desc));
        Desc.dwSize   = sizeof(Desc);
        Desc.dwFlags  = DDSD_CAPS ;
        Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        if(FAILED(pDD2->CreateSurface(&Desc,&pPri,NULL)))
        {
            MessageBoxA(NULL,"CreatePrimarySurface failed"," Surface Failed", MB_OK);
            return false;
        }


        ZeroMemory(&Desc, sizeof(Desc));
        Desc.dwSize   = sizeof(Desc);
        Desc.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        Desc.dwWidth  = m_WidgetSize.width;
        Desc.dwHeight = m_WidgetSize.height;
        Desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
        if(FAILED(pDD2->CreateSurface(&Desc,&pBack,NULL)))
        {
            MessageBoxA(NULL,"CreateBackBufferSurface failed"," Surface Failed", MB_OK);
            return false;
        }



        return true;
    }


    //=============================================================================
    // NAME: CreateFullScreenPrimary
    // DESC: create a full screen surface of directX
    //                              parameters : backbuffer count
    //=============================================================================
    bool VMCDX::CreateFullScreenPrimary(int BackBufferCount)
    {
        ZeroMemory(&Desc, sizeof(Desc));
        Desc.dwSize = sizeof(Desc);

        Desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

        while (BackBufferCount >= 0)
        {
            Desc.dwBackBufferCount = BackBufferCount;

            if (BackBufferCount != 0)
            {
                Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY ;
                if(FAILED(pDD2->CreateSurface(&Desc,&pPri,NULL)))
                    BackBufferCount--;
                else
                    break;

            }
            else if (BackBufferCount == 0)
            {
                Desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_SYSTEMMEMORY;
                if(FAILED(pDD2->CreateSurface(&Desc,&pPri,NULL)))
                {
                    MessageBoxA(NULL,"Create Surface failed"," Surface Failed", MB_OK);
                    return false;
                }
            }
        }

        DDSCAPS2 Scaps;
        ZeroMemory(&Scaps, sizeof(DDSCAPS2));
        Scaps.dwCaps = DDSCAPS_BACKBUFFER;

        //if(FAILED(pPri->GetAttachedSurface(&Desc.ddsCaps,&pBack)))
        if(FAILED(pPri->GetAttachedSurface(&Scaps,&pBack)))
        {
            MessageBoxA(NULL,"BackBuffer attached failed"," Surface Failed", MB_OK);
            return false;
        }

        ZeroMemory(&Desc, sizeof(Desc));
        Desc.dwSize   = sizeof(Desc);
        Desc.dwFlags  = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        Desc.dwWidth  = m_WidgetSize.width;
        Desc.dwHeight = m_WidgetSize.height;
        Desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
        if(FAILED(pDD2->CreateSurface(&Desc,&pBack,NULL)))
        {
            MessageBoxA(NULL,"CreateBackBufferSurface failed"," Surface Failed", MB_OK);
            return false;
        }

        return true;
    }


    //=============================================================================
    // NAME: CreateVisualMemCanvas()
    // DESC: create a visual memory canvas
    //=============================================================================
    bool VMCDX::CanvasCreate(CMemCanvas** canvas)
    {
        UINT width = GetWidth();
        UINT height = GetHeight();
        UINT depth = GetDepthb();

        if((*canvas) != 0)
            CanvasDestroy((*canvas));

        if((width <=0) || (height <=0) || (depth < 0))
        {
            OUTTRACE("Canvas size error!!. \n");
            return false;
        }


        (*canvas) = ::new CMemCanvas(width, height, depth, new ubyte_t[width*height*depth]);
        //(*canvas)->Allot(width, height, depth);
        //(*canvas)->width        = width;
        //(*canvas)->height       = height;
        //(*canvas)->GetWidthSize() = width * (*canvas)->depth;
        //(*canvas)->pixels  = new ubyte_t[(*canvas)->height * (*canvas)->GetWidthSize()];
        ZeroMemory((*canvas)->GetPixel(), (*canvas)->GetClipHeight() * (*canvas)->GetWidthSize());
        return true;
    }


    //=============================================================================
    // NAME: DestroyCanvas()
    // DESC: Destroy a visual memory canvas
    //=============================================================================
    bool VMCDX::CanvasDestroy(CMemCanvas *canvas)
    {
        SAFE_DELETE(canvas);
        return true;
    }
    */


    //=============================================================================
    // NAME: CreateSur()
    // DESC: Create    Surface    Use system memory
    //=============================================================================
    LpSurface Create_SYS_Sur( LpDraw pdd, DWORD w, DWORD h )
    {
        SurfaceDesc    desc;
        ZeroMemory( &desc, sizeof( desc ) );
        desc.dwSize = sizeof( desc );

        desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        desc.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
        desc.dwWidth = w;
        desc.dwHeight = h;

        LpSurface surf;
        HRESULT    r = pdd->CreateSurface( &desc, &surf, 0 );
        if( r != DD_OK )
            return 0;

        return surf;
    }


    //=============================================================================
    // NAME: ClearSurface()
    // DESC: Clear    Surface
    //=============================================================================
    bool ClearSurface( LpSurface surf, DWORD clr )
    {
        DDBLTFX bltfx;
        memset( &bltfx, 0, sizeof( bltfx ) );
        bltfx.dwSize = sizeof( bltfx );
        bltfx.dwFillColor = clr;
        surf->Blt( 0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx );
        return true;
    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    bool VMCDX::BeginFrame( DWORD argv )
    {
        return true;
    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    void VMCDX::EndFrame( void )
    {
        if( FAILED( pBack->Lock( 0, &Desc, DDLOCK_SURFACEMEMORYPTR, NULL ) ) )
        {
            //MessageBoxA(NULL,"Surface Lock failure","Lock Failed", MB_OK);
            //exit(false);
            pBack->Restore();
            return;
        }

        LONG length = p_canvas_->GetWidthSize();
        UINT tell = p_canvas_->GetHeight();
        register ubyte_t* dptr = ( ubyte_t* )Desc.lpSurface;// + (point.y * Desc.lPitch + (point.x << _PIXEL_WIDTH_SHIFT));
        register ubyte_t* sptr = p_canvas_->GetPixel();// + (clipper_rect.top*length + (clipper_rect.left<<_PIXEL_WIDTH_SHIFT));

        for( register UINT row = 0; row < tell; row++ )
        {
            __pfmemcpy( dptr, sptr, length );
            //將目的記憶體的控制指標移至下一行開始的位置
            dptr = dptr + Desc.lPitch;
            //將來源記憶體的控制指標移至下一行開始的位置
            sptr = sptr + length;
        }

        /*
    IOMAPCPY((ubyte_t*)Desc.lpSurface + (point.y * Desc.lPitch + (point.x << _PIXEL_WIDTH_SHIFT)),
              Desc.lPitch,
              src->GetPixel() + (clipper_rect.top*length + (clipper_rect.left<<_PIXEL_WIDTH_SHIFT)),
              length, tell,
              cp_line);*/

        pBack->Unlock( 0 ); // 處理完 surface 之後 unlock

        //return;


    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in x:0 Y:0
    //=============================================================================
    void VMCDX::Flip( void )
    {
        HRESULT hRet;
        if( GetFullScreen() )
        {
            // Else we are in fullscreen mode, so perform a flip.
            //ddFrontBuffer->Flip(NULL, DDFLIP_WAIT);
            if( ( hRet = pPri->Flip( NULL, 0 ) ) == DD_OK )
                return;
        }
        else
        {
            GetClientRect( ( HWND )vobj_hwidget_, &m_rcScreen );
            ClientToScreen( ( HWND )vobj_hwidget_, ( POINT* )&m_rcScreen.left );
            ClientToScreen( ( HWND )vobj_hwidget_, ( POINT* )&m_rcScreen.right );
            LG::Rect outrt;
            outrt.setxywh( m_rcViewport.left, m_rcViewport.top, m_rcScreen.width(), m_rcScreen.height() );
            if( ( hRet = pPri->Blt( &m_rcScreen, pBack,
                &outrt, DDBLT_WAIT, NULL ) ) == DD_OK )
                return;
        }



        if( hRet == DDERR_SURFACELOST )
        {
            hRet = pPri->Restore();
            if( hRet != DD_OK )
                return;
        }

        return;
    }


    //=============================================================================
    // NAME: Flip()
    // DESC: Blt memory map to screen, start in point
    //=============================================================================
    void VMCDX::Flip( CMemCanvas *src, POINT point )
    {
        if( ( HWND )vobj_hwidget_ != GetFocus() )
            return;

        /*
        LG::Rect windowrect;
        GetWindowRect((HWND)vobj_hwidget_, &windowrect);

        if(windowrect.left < 0)
            windowrect.left=0;

        if(windowrect.top < 0)
            windowrect.top=0;

        if(windowrect.right >= m_ScreenSize.cx)
            windowrect.right=m_ScreenSize.cx;

        if(windowrect.bottom >= m_ScreenSize.cy)
            windowrect.bottom=m_ScreenSize.cy;


        LG::Rect clipper_rect(0, 0, ((Icon*)src)->GetClipWidth() - 1, ((Icon*)src)->GetClipHeight() - 1);
        if(VMC_Clipper(&clipper_rect, &point, windowrect.width(), windowrect.height()))
            return;


        int cp_line = clipper_rect.width()<<_PIXEL_WIDTH_SHIFT;
        int tell = clipper_rect.height();

        if(src->GetWidthSize() > (clipper_rect.width() << _PIXEL_WIDTH_SHIFT))
            cp_line = ((clipper_rect.width() << _PIXEL_WIDTH_SHIFT));

        if(src->GetClipHeight() > (unsigned int)clipper_rect.height())
            tell = clipper_rect.height();


        if(FAILED(pPri->Lock(0, &Desc, DDLOCK_WAIT, NULL)))
        {
            MessageBoxA(NULL,"Surface Lock failure","Lock Failed", MB_OK);
            //exit(false);
            pPri->Restore();
            return;
        }

        //ubyte_t *pDest = (ubyte_t *)Desc.lpSurface + (point.y * Desc.lPitch + (point.x << 1));
        //ubyte_t *pSrc = src->pixels;

        long length = src->GetWidthSize();

        IOMAPCPY((ubyte_t*)Desc.lpSurface + (point.y * Desc.lPitch + (point.x << _PIXEL_WIDTH_SHIFT)),
                  Desc.lPitch,
                  src->GetPixel(),
                  length, tell,
                  cp_line);

        pPri->Unlock(0); // 處理完 surface 之後 unlock
        */

        return;
    }


    //=============================================================================
    // NAME: ClippedFlip()
    // DESC: Blt memory map to screen, start in x:0 Y:0 and Clip with rect
    //=============================================================================
    void VMCDX::ClippedFlip( CMemCanvas *src, POINT point, RECT rect )
    {
        if( ( HWND )vobj_hwidget_ != GetFocus() )
            return;

        DWORD LineSize = 0;
        int width = ( src->GetWidthSize() );

        rect.right = ( rect.right << _PIXEL_WIDTH_SHIFT );

        if( ( rect.right - rect.left ) > src->GetWidthSize() )
        {
            rect.left = 0;
            rect.right = src->GetWidthSize();
        }
        if( ( UINT )( rect.bottom - rect.top ) > src->GetClipHeight() )
        {
            rect.top = 0;
            rect.bottom = src->GetClipHeight();
        }

        LineSize = rect.right - rect.left;

        if( FAILED( pPri->Lock( 0, &Desc, DDLOCK_WAIT, NULL ) ) )
        {
            MessageBoxA( NULL, "Surface Lock failure", "Lock Failed", MB_OK );
            //exit(false);
            pPri->Restore();
            return;

        }
        ubyte_t* pDest = ( ubyte_t * )Desc.lpSurface + ( point.y * Desc.lPitch + ( point.x << _PIXEL_WIDTH_SHIFT ) );
        ubyte_t* pSrc = src->GetPixel() + ( rect.top * width ) + rect.left;

        int row = rect.top;
        while( row < rect.bottom )
        {
            __pfmemcpy( pDest, pSrc, LineSize );
            pDest += Desc.lPitch; // 指向下一列 pixel 起始位址
            pSrc = pSrc + width;
            row++;
        }

        pPri->Unlock( 0 ); // 處理完 surface 之後 unlock


        return;

    }


    //=============================================================================
    // NAME: ClearScreen()
    // DESC: set screen color with backcolor
    //=============================================================================
    bool VMCDX::ClearScreen( DWORD color )
    {
        DWORD ret;
        DWORD size = 0;

        size = GetWidth() * GetDepthb();

        if( ret = pPri->Lock( 0, &Desc, DDLOCK_WAIT, NULL ) )
        {

            MessageBoxA( NULL, "Surface Lock failure", "Lock Failed", MB_OK );
            exit( false );
        }

        ubyte_t *pDest = ( ubyte_t * )Desc.lpSurface;

        for( UINT row = 0; row < GetHeight(); row++ )
        {
            memset( pDest, color, size );
            pDest += Desc.lPitch; // 指向下一列 pixel 起始位址
        }

        pPri->Unlock( 0 ); // 處理完 surface 之後 unlock

        return true;
    }


    //=============================================================================
    // NAME: JudgeVideoMask()
    // DESC: judgment Vedio mode
    //=============================================================================
    PixelFormat VMCDX::JudgeVideoMask( void )
    {
        DDPIXELFORMAT pixel_format;

        ZeroMemory( &pixel_format, sizeof( pixel_format ) );
        pixel_format.dwSize = sizeof( pixel_format );
        pPri->GetPixelFormat( &pixel_format );

        PixelFormat P_M = PixelFormat::UNKNOW;
        if( pixel_format.dwRGBBitCount == 16 )
        {
            //CMemCanvas image_mask;
            // RRRRRGGGGGGBBBBB 565
            if( ( pixel_format.dwGBitMask == 0x07E0 ) && ( pixel_format.dwRBitMask = 0xF800 ) )
            {
                P_M = PixelFormat::R5G6B5;
            }
            // XRRRRRGGGGGBBBBB 555
            else if( ( pixel_format.dwGBitMask == 0x03E0 ) && ( pixel_format.dwRBitMask = 0x7C00 ) )
            {
                P_M = PixelFormat::R5G5B5;
            }
            // RRRRRRGGGGGBBBBB 655
            else if( ( pixel_format.dwGBitMask == 0x03E0 ) && ( pixel_format.dwRBitMask == 0xFC00 ) )
            {
                P_M = PixelFormat::R6G5B5;
            }
            // RRRRRGGGGGBBBBBB 556
            else if( ( pixel_format.dwGBitMask == 0x03E0 ) && ( pixel_format.dwBBitMask = 0x003F ) )
            {
                P_M = PixelFormat::R5G5B6;
            }
        }
        else if( pixel_format.dwRGBBitCount == 32 )
        {
            P_M = PixelFormat::R8G8B8A8;
            if( ( pixel_format.dwRBitMask == 0x00FF0000 ) &&
                ( pixel_format.dwGBitMask = 0x0000FF00 ) )
            {
                P_M = PixelFormat::A8R8G8B8;
            }
            else if( ( pixel_format.dwRBitMask == 0x000000FF ) &&
                ( pixel_format.dwBBitMask = 0x00FF0000 ) )
            {
                P_M = PixelFormat::A8B8G8R8;
            }

        }
        return P_M;
    }

    //=============================================================================
    // NAME: INIT()
    // DESC: Init directX judge vedio mask and set it
    //=============================================================================
    bool VMCDX::Show_Text( LPCSTR str, int x, int y, COLORREF color )
    {
        /*
        HDC hDC;

        DDBLTFX Fx;
        ZeroMemory(&Fx, sizeof(Fx)); Fx.dwSize = sizeof(Fx);
        Fx.dwFillColor = 0;

        if(FAILED(pText->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &Fx)))
            return false;
        if(FAILED(pText->GetDC(&hDC)))
            return false;

        HFONT hFont = (HFONT)SelectObject(hDC, m_font_type);
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, color);
        TextOut(hDC, x, y, str, strlen(str));
        SelectObject(hDC, hFont);
        pText->ReleaseDC(hDC);

        RECT rt;
        rt.left = 0;
        rt.right = 200;
        rt.top = 0;
        rt.bottom = 50;
        pPri->BltFast(0, 0, pText, &rt, DDBLTFAST_WAIT);
        */

        return TRUE;
    }

    //=============================================================================
    // NAME: INIT()
    // DESC: Init directX judge vedio mask and set it
    //=============================================================================
    /*

    void VMCDX::INIT(int Pixel_mode)
    {
        switch(Pixel_mode)
        {
            case PIXEL_565:
            {
                PixelConvet = P565;
                break;
            }
            case PIXEL_555:
            {
                PixelConvet = P555;
                break;
            }
            case PIXEL_556:
            {
                PixelConvet = P556;
                break;
            }
            case PIXEL_655:
            {
                PixelConvet = P655;
                break;
            }
        }
    }


    //=============================================================================
    // 565轉換888
    //
    // 11111 000000 00000  0XF800
    // 00000 111111 00000  0X7E0
    // 00000 000000 11111  0X1F
    //=============================================================================

    void VMCDX::P565(word_t Pixel, register ubyte_t *R, register ubyte_t *G, register ubyte_t *B)
    {
        *R = (Pixel >> 11);
        *G = ((Pixel >> 5)  &0X3F);
        *B = (Pixel  &0X1F);
    }

    //=============================================================================
    //
    // 555轉換888
    //
    // X 11111 00000 00000  0X7C00
    // X 00000 11111 00000  0X3E0
    // X 00000 00000 11111  0X1F
    //=============================================================================

    void VMCDX::P555(word_t Pixel, register ubyte_t *R, register ubyte_t *G, register ubyte_t *B)
    {
        *R = (Pixel >> 11);
        *G = ((Pixel >> 6)  &0X1F);
        *B = (Pixel  &0X1F);
    }

    //=============================================================================
    //
    // 556轉換888
    //
    // 11111 00000 000000  0XF800
    // 00000 11111 000000  0X7C0
    // 00000 00000 111111  0X3F
    //=============================================================================

    void VMCDX::P556(word_t Pixel, register ubyte_t *R, register ubyte_t *G, register ubyte_t *B)
    {
        *R = (Pixel >> 11);
        *G = ((Pixel >> 6)  &0X1F);
        *B = (Pixel  &0X3F);
    }

    //=============================================================================
    //
    // 655轉換888
    //
    // 111111 00000 00000  0XFC00
    // 000000 11111 00000  0X3E0
    // 000000 00000 11111  0X1F
    //=============================================================================

    void VMCDX::P655(word_t Pixel, register ubyte_t *R, register ubyte_t *G, register ubyte_t *B)
    {
        *R = (Pixel >> 10);
        *G = ((Pixel >> 6)  &0X1F);
        *B = (Pixel  &0X1F);
    }
    */

};

#endif
