// Done by TheTutor -- 11/2/01

/*  This tutorial shows ONE way to do "particles" in OpenGL -- Like everything in
    programming, are a lot of ways you can make a "particle" -- This is one simple
    effective way

    This is how we're going to make our particles:

    1)  Each particle will be comprised of two textures -- Both are "gray scale" bitmaps
        that have a "cool particle look" (these can EASILY be made with PhotoShop)
    2)  Each texture will be drawn to an OpenGL stock object "the quad" (ie a flat square)
    3)  Then to both quads we will add an "alpha blended color" so they look cooooool :)
    4)  Finally we will spin one of the quads around the z-axis so the particle has a
        "shimmering look"

    That will do it.  So without any other fanfare, it's particle time...
*/

// Make sure we load all the libraries we need
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")

#define class_name "TheTutor_Particles_OGL"

#define WIN_WID 800
#define WIN_HGT 600

#include "particle.h"

// Globals ***

GL_OBJ gl_obj; // Our OpenGL object

// *** End of Globals

// Changes the screen resolution
bool ChangeDisplayMode( int pixel_width,
                        int pixel_height,
                        bool switch_back = false );

// This locks the frame rate to "frame_rate" passed in -- Here we overload the
// input parameter so by default the frame rate will be 60 frames per second
bool LockFrameRate( int frame_rate = 60 );

bool MasterInit( HWND hwnd ); // This initializes EVERYTING

// WinProc
LRESULT CALLBACK WinProc( HWND hwnd,
                          UINT message,
                          WPARAM wparam,
                          LPARAM lparam );

// WinMain
int WINAPI WinMain( HINSTANCE hinstance,
                    HINSTANCE hprev,
                    PSTR cmdline,
                    int ishow )
{
    MSG msg;
    HWND hwnd               = NULL;
    WNDCLASSEX wndclassex   ={0};

    // Fill the fields we care about
    wndclassex.cbSize = sizeof( WNDCLASSEX );
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;                   
    wndclassex.hInstance = hinstance;   
    wndclassex.lpszClassName = class_name;
    wndclassex.hbrBackground = ( HBRUSH ) GetStockObject( BLACK_BRUSH );

    RegisterClassEx( &wndclassex ); // Register the class

    // We know we want these attributes for creating the window
    // (We will set the other attributes depending on if we could switch to 
    // full screen or not)
    DWORD win_style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    bool full_screen= ChangeDisplayMode( WIN_WID, WIN_HGT ); // Try and change to full screen

    // Create our window -- Notice how we set use different "variables" when creating
    // the window based upon if we were able to switch to full screen or not
    hwnd = CreateWindowEx( WS_EX_TOPMOST,
                           class_name,
                           "OGL",
                           ( full_screen ) ?
                           ( win_style | WS_POPUP ) :
                           ( win_style | WS_SYSMENU ),
                           ( full_screen ) ?
                           0 :
                           CW_USEDEFAULT,
                           ( full_screen ) ?
                           0 :
                           CW_USEDEFAULT,
                           WIN_WID,
                           WIN_HGT,
                           NULL,
                           NULL,
                           hinstance,
                           NULL );

    // Error Check
    if( !hwnd )
    {
        ChangeDisplayMode( 0, 0, true );
        UnregisterClass( class_name, hinstance );
        return EXIT_FAILURE;
    }

    srand( GetTickCount() ); // Seed random timer

    // If anything we try to initialize fails, we better quit
    if( !MasterInit( hwnd ) )
    {
        ChangeDisplayMode( 0, 0, true ); // Set back display mode
        FreeTextures(); // Free textures (if they we're allocated)
        UnregisterClass( class_name, hinstance );
        return EXIT_FAILURE;
    }

    ShowCursor( false );

    // Show and update window
    ShowWindow( hwnd, ishow );
    UpdateWindow( hwnd );

    while( 1 )
    {
        if( PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
                break;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }else if( LockFrameRate() )
        {
            gl_obj.startDraw(); // Begin to draw

            RunApp();

            gl_obj.endDraw(); // Blit to screen
        }
    } // end of while(1)

    ShowCursor( true );

    FreeTextures(); // Free the textures
    UnregisterClass( class_name, hinstance );
    return msg.wParam;
}

// Simple WinProc
LRESULT CALLBACK WinProc( HWND hwnd,
                          UINT message,
                          WPARAM wparam,
                          LPARAM lparam )
{
    PAINTSTRUCT ps;

    switch( message )
    {
    case WM_PAINT:BeginPaint( hwnd, &ps );
    EndPaint( hwnd, &ps );
    return 0;

    case WM_KEYDOWN:if( wparam == VK_ESCAPE )
    {
                        ChangeDisplayMode( 0, 0, true ); // Always switch back to normal screen resolution
                        PostQuitMessage( 0 );
    }

    return 0;

    case WM_CLOSE:case WM_DESTROY:ChangeDisplayMode( 0, 0, true ); // Always switch back to normal screen resolution
    PostQuitMessage( 0 );
    return 0;
    } // end of switch(message)

    return DefWindowProc( hwnd, message, wparam, lparam );
}

// This is what actually changes the screen resolution
bool ChangeDisplayMode( int pixel_width, int pixel_height, bool switch_back )
{
    // If we're switching back to "our normal resolution"
    if( switch_back )
    {
        // By passing in NULL we're saying "Go back to screen resolution logged in the registry"
        // 0 says "do this dynamically (like now)"
        // **Note this should not fail, that's why we don't check (could if you wanted to)
        ChangeDisplaySettings( NULL, 0 );
        return true;
    }

    DEVMODE dev_mode={0};
    LONG result;

    // This call is going to get us the current display settings -- By parameter:
    // NULL -- This specifies which display device (NULL means current display device)
    // ENUM_CURRENT_SETTINGS -- Retrieve the current settings for the display device
    // dev_mode -- Address of DEVMODE structure to fill with the display settings
    if( !EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dev_mode ) )
        return false;

    // We have a DEVMODE filled with all the current settings
    // so lets change the ones we want to
    dev_mode.dmPelsWidth = pixel_width;
    dev_mode.dmPelsHeight = pixel_height;

    // Now we'll try to change to the new settings
    // CDS_FULLSCREEN -- flag says this change is "temporary" (there are other flags you can pass in)
    result = ChangeDisplaySettings( &dev_mode, CDS_FULLSCREEN );

    // ChangeDisplaySettings() returns DISP_CHANGE_SUCCESSFUL when it's successful
    if( result == DISP_CHANGE_SUCCESSFUL )
        return true;
    else
        return false; // No changing to full screen :(

}

// Locks the frame rate at "frame_rate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate( int frame_rate )
{
    static float lastTime   = 0.0f;

    // Get current time in seconds (milliseconds * .001 = seconds)
    float currentTime       = GetTickCount() * 0.001f; 

    // Get the elapsed time by subtracting the current time from the last time
    // If the desired frame rate amount of seconds has passed -- return true (ie Blit())
    if( ( currentTime - lastTime ) > ( 1.0f / frame_rate ) )
    {
        // Reset the last time
        lastTime = currentTime; 
        return true;
    }

    return false;
}

// Here we will attempt to initialize all our "objects"
bool MasterInit( HWND hwnd )
{
    // Initialize "OpenGL" using 16 bits per pixel for the color and depth bits
    if( gl_obj.Init( hwnd, 16 ) == false )
        return false;

    // Try and initialize the particles and textures
    if( Init() == false )
        return false;

    // Set up the view port
    gl_obj.setViewPort( WIN_WID, WIN_HGT, 45 );


    // Now set all necessary OpenGL flags
    glEnable( GL_TEXTURE_2D ); // So we can texture map
    glEnable( GL_BLEND ); // So we can "alpha blend"
    glShadeModel( GL_SMOOTH ); // We'll have "smooth shading"

    // Set our alpha blending -- Take a look at MSDN for the big technical definition
    // of what's happening.  For the watered down highly non-technical version,
    // we're just taking all the source pixels (our textures) and blending them with
    // what ever color(s) our underneath them.  Also the "blend" is going towards the color
    // SOLID white (255,255,255) -- So in our case our two textures per 
    // particle will get blended since they overlap AND if two or more particles overlap
    // their colors will get blended together as well.
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    return true; // The app has been initialized
}

/*

    This tutorial doesn't do anything revolutionary -- It's intent was to simply show you
    a way to program "particles" -- What you do with your particles is totally up to you :)

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  ?2001 GameTutorials        |
\*----------------------------*/