//Device3d.cpp
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
//                                               Copyright    (C)    2000    Pin
//
#include "Device3d.h"
#include <Debug.h>



namespace LGR
{
#ifdef _RWKERNEL_

    RwRGBA charsetforeground={208, 208, 0, 255};
    RwRGBA charsetbackground={0, 0, 0, 0};
    RtCharset* gCharset     = NULL;

#if (defined(SKY))
#define rwRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(_XBOX))
#define rwRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(DOLPHIN))
#define rwRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(D3D8_DRVMODEL_H))
#define rwRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(OPENGL_DRVMODEL_H))
#if (defined(macintosh))
#define rwRESOURCESDEFAULTARENASIZE (2 << 20)
#else
#define rwRESOURCESDEFAULTARENASIZE (16 << 20)
#endif
#else
#define rwRESOURCESDEFAULTARENASIZE (16 << 20)
#endif


#endif


    //=============================================================================
    //  Name : 
    //  Desc : Constructor func
    //=============================================================================
    Device3d::Device3d()
    {
        DebugDelf();

#if defined(_WGLKERNEL_) || defined(_XGLKERNEL_)
        m_hRC = NULL;                       // Permanent Rendering Context
#endif
#ifdef _WGLKERNEL_
        m_hDC = NULL;                        // Private GDI Device3d Context
        m_hFont = NULL;
#endif

        //fullscreen   = TRUE;                        // Fullscreen Flag Set To Fullscreen Mode By Default
        Create_Font( "新細明體", 12, 6 );
        //for( int i = 0; i < _VMC_MAX_LIGHT; i++ )
        //  Light[i].Assign( i );
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Device3d::~Device3d()
    {
        Destroy();
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void* Device3d::GetCanvas( void )
    {
#ifdef _WGLKERNEL_
        return &m_hDC;
#endif
        return 0;
    }


    /*
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
     void  Device3d::SetRenderFunc( int ( __cdecl *RenderFunc )( void ) )
    {
        //RenderScene = RenderFunc;
        SetIdleFunc( RenderFunc );
    }
    */


    //=============================================================================
    //  Name :
    //  Desc : initialing All OpenGL value
    //=============================================================================
    int Device3d::Initialize3D( void* displayID )
    {
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
        glEnable( GL_TEXTURE_2D );                          // Enable Texture Mapping
        glBlendFunc( GL_SRC_ALPHA, GL_ONE );                    // Set The Blending Function For Translucency
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );             // This Will Clear The Background Color To Black
        glClearDepth( 1.0f );                               // Enables Clearing Of The Depth Buffer
        glDepthFunc( GL_LESS );                             // The Type Of Depth Test To Do
        glEnable( GL_DEPTH_TEST );                          // Enables Depth Testing
        glShadeModel( GL_SMOOTH );                          // Enables Smooth Color Shading
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );    // Really Nice Perspective Calculations

        glLoadIdentity();                                   // Reset The View
#endif

#ifdef _RWKERNEL_
        if( !RwEngineInit( 0X0, 0X0, rwRESOURCESDEFAULTARENASIZE ) )
        {
            DebugMessage( "Initialise: RenderWare Engine failed to initialise",
                          "VFWRW" );
            return FALSE;
        }

        if( !RpWorldPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach world plugin",
                          "VFWRW" );
            return FALSE;
        }

        if( !RtAnimInitialize() )
        {
            DebugMessage( "Initialise: Failed to attach animation plugin",
                          "VFWRW" );
            return FALSE;
        }

        if( !RpHAnimPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach Hanimation plugin",
                          "VFWRW" );
            return FALSE;
        }

        if( !RpSkinPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach skin plugin", "VFWRW" );
            return FALSE;
        }

        if( !RpCollisionPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach collision plugin",
                          "VFWRW" );
            return FALSE;
        }

        if( !RpRandomPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach random plugin",
                          "VFWRW" );
            return FALSE;
        }

        if( !RpMatFXPluginAttach() )
        {
            DebugMessage( "Initialise: Failed to attach matfx plugin",
                          "VFWRW" );
            return FALSE;
        }


        RwEngineOpenParams parameters;
        parameters.displayID = ( void * ) displayID;
        if( !RwEngineOpen( &parameters ) )
        {
            DebugMessage( "RenderWare::Initialise: Failed to open library",
                          "VFWRW" );
            RwEngineTerm();
            return FALSE;
        }

        if( !RwEngineStart() )
        {
            DebugMessage( "RenderWare::Initialise: Failed to start engine",
                          "VFWRW" );
            RwEngineClose();
            RwEngineTerm();
            return FALSE;
        }

        //RwDebugSetHandler( _debughandler );

        RwImageRegisterImageFormat( "bmp", RtBMPImageRead, 0 );
        RwImageRegisterImageFormat( "png", RtPNGImageRead, 0 );
        RwImageRegisterImageFormat( "tif", RtTIFFImageRead, 0 );

#if defined(_PS2)
        RpSkySelectTrueTSClipper( 1 );
#endif

        // Create debug character set
        RtCharsetOpen();
        gCharset = RtCharsetCreate( &charsetforeground, &charsetbackground );

        /*
           //RpAlphaPluginAttach();
        */
#endif

        return TRUE;                                                 // Initialization Went OK
    }



    //=============================================================================
    //  Name :
    //  Desc :  Here's Where We Do All The Drawing
    //=============================================================================
    int DefRenderScene( void )
    {
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );          // Clear The Screen And The Depth Buffer
        glLoadIdentity();                                            // Reset The Current Modelview Matrix
#endif
        return TRUE;                                                 // Everything Went OK
    }


    //=============================================================================
    //  Name :
    //  Desc : Properly Kill The Window
    //=============================================================================
    void Device3d::Destroy( void )
    {
#ifdef _WGLKERNEL_

        if( m_hFont )
        {
            Destroy_Font();
        }

        if( m_hRC )                                            // Do We Have A Rendering Context?
        {
            if( !wglMakeCurrent( NULL, NULL ) )                 // Are We Able To Release The DC And RC Contexts?
            {
                DebugMessage( "Release Of DC And RC Failed", "V3D" );
            }

            if( !wglDeleteContext( m_hRC ) )                     // Are We Able To Delete The RC?
            {
                DebugMessage( "Release Rendering Context Failed", "V3D" );
            }else
                m_hRC = NULL;                                       // Set RC To NULL

        }

        if( m_hDC != NULL )
        {
            if( !ReleaseDC( ( HWND ) ( m_DisplayID ), m_hDC ) )                       // Are We Able To Release The DC
            {
                DebugMessage( "Release Device3d Context Failed", "V3D" );
            }else
                m_hDC = NULL;                                       // Set DC To NULL

        }
#endif
#ifdef _RWKERNEL_
        RtCharsetClose(); 
        RwEngineStop();
        RwEngineClose();
        RwEngineTerm();
#endif

        return;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    BOOL Device3d::SetPixelFormat( int bbp )
    {
        m_depth = bbp;

        if( m_DisplayID == 0 )
            return FALSE;

#ifdef _WGLKERNEL_
        GLuint PixelFormat;                            // Holds The Results After Searching For A Match
        //glViewport(  0, 0, m_Width, m_Height  );

        static  PIXELFORMATDESCRIPTOR pfd   =                  // pfd Tells Windows How We Want Things To Be
        {sizeof( PIXELFORMATDESCRIPTOR ),                // Size Of This Pixel Format Descriptor
         1,                                              // Version Number
         PFD_DRAW_TO_WINDOW |                             // Format Must Support Window
         PFD_SUPPORT_OPENGL |                             // Format Must Support OpenGL
         PFD_DOUBLEBUFFER,                               // Must Support Double Buffering
         PFD_TYPE_RGBA,                                  // Request An RGBA Format
         bbp,                                            // Select Our Color Depth
         0,
         0,
         0,
         0,
         0,
         0,                               // Color Bits Ignored
         0,                                              // No Alpha Buffer
         0,                                              // Shift Bit Ignored
         0,                                              // No Accumulation Buffer
         0,
         0,
         0,
         0,                                     // Accumulation Bits Ignored
         bbp,                                            // 16Bit Z-Buffer ( Depth Buffer )
         0,                                              // No Stencil Buffer
         0,                                              // No Auxiliary Buffer
         PFD_MAIN_PLANE,                                 // Main Drawing Layer
         0,                                              // Reserved
         0,
         0,
         0                                         // Layer Masks Ignored
        };

        //HWND* wnd = (HWND*)m_DisplayID;

        if( !( m_hDC = GetDC( ( HWND ) m_DisplayID ) ) )               // Did We Get A Device3d Context?
        {
            Destroy();                                      // Reset The Display
            MessageBox( NULL,
                        "Can't Create A GL Device3d Context",
                        "ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                   // Return FALSE
        }

        if( !( PixelFormat = ChoosePixelFormat( m_hDC, &pfd ) ) )   // Did Windows Find A Matching Pixel Format?
        {
            Destroy();                                      // Reset The Display
            MessageBox( NULL,
                        "Can't Find A Suitable PixelFormat",
                        "ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                   // Return FALSE
        }

        if( !SetPixelFormat( m_hDC, PixelFormat, &pfd ) )     // Are We Able To Set The Pixel Format?
        {
            Destroy();                                      // Reset The Display
            MessageBox( NULL,
                        "Can't Set The PixelFormat",
                        "ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                   // Return FALSE
        }
        return TRUE; // Success
#endif

#ifdef _RWKERNEL_
        return TRUE;
#endif

        return FALSE;
    }


    //=============================================================================
    //  Name :
    //  Desc : Creates render context and makes it current
    //=============================================================================
    BOOL Device3d::SetRenderContext( void )
    {
#ifdef _WGLKERNEL_
        if( m_hDC == 0 )
            return FALSE;

        // Create the OpenGL render context
        if( !( m_hRC = wglCreateContext( m_hDC ) ) )         // Are We Able To Get A Rendering Context?
        {
            Destroy();                                      // Reset The Display
            MessageBox( NULL,
                        "Can't Create A GL Rendering Context",
                        "ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                   // Return FALSE
        }

        if( !wglMakeCurrent( m_hDC, m_hRC ) )                // Try To Activate The Rendering Context
        {
            Destroy();                                      // Reset The Display
            MessageBox( NULL,
                        "Can't Activate The GL Rendering Context",
                        "ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
            return FALSE;                                   // Return FALSE
        }

        return TRUE; // Success
#endif

#ifdef _RWKERNEL_
        return TRUE;
#endif


        return FALSE;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    BOOL Device3d::Open( void* displayID, int bbp )
    {
        //displayID = m_MyWnd;
        if( displayID == 0 )
        {
            Destroy();
            DebugMessage( "No draw display", "VFW" );
            return FALSE;
        }

        m_DisplayID = displayID;

        if( !SetPixelFormat( bbp ) )
        {
            Destroy();
            DebugMessage( "set pixel format fail!!", "VFW" );
            return FALSE;
        }

        if( !SetRenderContext() )
        {
            Destroy();
            DebugMessage( "Set render context fail", "VFW" );
            return FALSE;
        }
        #ifdef _WGLKERNEL_

        //CPRect rt;
        //GetWindowRect( (HWND)(&displayID), &rt );
        //m_DisplaySize.cx = rt.width();
        //m_DisplaySize.cy = rt.height();
#endif

#ifdef _XGLKERNEL_

#endif

        //ReSizeScene(  m_DisplaySize.width,  m_DisplaySize.height );                   // must resize first ***************

        if( !Initialize3D( displayID ) )                   // Initialize Our Newly Created GL Window
        {
            Destroy();                                      // Reset The Display
            DebugMessage( "Initialization Failed", "VFW" );
            return FALSE;                                   // Return FALSE
        }
        return TRUE;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    int Device3d::Show( const Camera* camera )
    {
#ifdef _WGLKERNEL_
        glFlush();
        SwapBuffers( m_hDC );
#endif

#ifdef _XGLKERNEL_
        glFlush();
        glXSwapBuffers( ( ( Pwnd * ) m_DisplayID )->m_display,
                        ( ( Pwnd * ) m_DisplayID )->wnd );
#endif

#ifdef _RWKERNEL_
#if defined(_WIN32) && !defined(_XBOX)
        if( RwCameraShowRaster( camera->m_pCamera, ( HWND ) m_DisplayID, 0 ) )
            return 1;
#else
        if( RwCameraShowRaster( camera->m_pCamera, 0, rwRASTERFLIPWAITVSYNC ) )
            return 1;
#endif
#endif
        return NULL;
    }




    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    /*
     void  Device3d::Turn_On_Light( Pword num )
    {
        if( num <= _VMC_MAX_LIGHT ) // turn on one light
        {
            Light[num].Enable();
            return;
        }
        for( int l = 0; l < _VMC_MAX_LIGHT; l++ )// turn on all light
            Light[l].Enable();
    }
    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
     void  Device3d::Turn_Off_Light( Pword num )
    {
        if( num <= _VMC_MAX_LIGHT ) // turn on one light
        {
            Light[num].Disable();
            return;
        }
        for( int l = 0; l < _VMC_MAX_LIGHT; l++ )// turn on all light
            Light[l].Disable();
    }
    */


    //=============================================================================
    //  Name :
    //  Desc :
    //  Ver  :  rw
    //=============================================================================
    void Device3d::SetZTest( bool open )
    {
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
        return;
#endif

#ifdef _RWKERNEL_
        RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, ( void * ) open );
        RwRenderStateSet( rwRENDERSTATEZTESTENABLE, ( void * ) open );
#endif
        return;
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    VfMetrics* Device3d::GetMetrics( VfMetrics* ginfo )
    {
#ifdef _RWKERNEL_
        RwMetrics* metrics;
        if( NULL == ( metrics = RwEngineGetMetrics() ) )
            return NULL;
        memcpy( ginfo, metrics, sizeof( RwMetrics ) );
        return ginfo;
#endif
    }




























    ///////////////////////////////////////////////////////////////////////////////
    //  font

    //=============================================================================
    //  Name : Create_Font()
    //  Desc : create font type and width, height
    //=============================================================================
    void Device3d::Create_Font( const char* font_type, int s_w, int s_h )
    {
#if defined( _GLKERNEL_ ) && defined ( _WIN32 )
        m_hFont = CreateFont( s_w,
                              s_h,
                              0,
                              0,
                              FW_BOLD,
                              0,
                              0,
                              0,
                              DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS,
                              DEFAULT_QUALITY,
                              DEFAULT_PITCH,
                              font_type );
#endif
    }



    //=============================================================================
    //  Name : Text(  )
    //  Desc : draw text in screen
    //=============================================================================
    void Device3d::Text( int x, int y, COLORTYPE col, Pcstr string )
    {
#if defined( _GLKERNEL_ ) && defined ( _WIN32 )
        HFONT hFont = ( HFONT ) SelectObject( m_hDC, m_hFont );
        SetBkMode( m_hDC, TRANSPARENT );
        SetTextColor( m_hDC, col );
        TextOut( m_hDC, x, y, string, strlen( string ) );
        SelectObject( m_hDC, hFont );
#endif
    }

    //=============================================================================
    //  Name : Destroy_Font(  )
    //  Desc : Destroy font object
    //=============================================================================
    void Device3d::Destroy_Font( void )
    {
#if defined( _GLKERNEL_ ) && defined ( _WIN32 )
        DeleteObject( m_hFont );
#endif
        return;
    }
};//namespace L3DE
