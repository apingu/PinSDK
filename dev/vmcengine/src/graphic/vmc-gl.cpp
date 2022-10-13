
//#define _IRR_WINDOWS_

#ifdef _WINDOWS
#include <windows.h>
#endif

#include "vmc-gl.h"
#include "vmc-global.h"
#include "amath.inl"

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")

#pragma comment(lib, "Icon2d.lib")

//extern PixelFormat _VGA_PixelFormat;

void OnGLLoadImageFunc( CPIcon* image )
{
    // Build A Texture From The Data
    glGenTextures( 1, &image->Id );                                        // Generate OpenGL texture IDs

    glBindTexture( GL_TEXTURE_2D, image->Id );                             // Bind Our Texture
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );    // Linear Filtered // 似乎會影像畫線?不能拿掉
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );    // Linear Filtered 
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels );

    //glEnable(GL_TEXTURE_2D);
    //glIsEnabled 

    /*
    1.  GL_TEXTURE_MIN_FILTER  縮小時的濾鏡方式.
    param:
        GL_NEAREST 最近點取樣,速度最快.
        GL_LINEAR  線性過濾.

    2. GL_TEXTURE_MAG_FILTER  放大時的濾鏡方式.
    param:
        GL_NEAREST 最近點取樣,材質圖素不作濾鏡運算,速度最快,但是會產生鋸齒邊.
        GL_LINEAR  線性過濾,將材質圖素及相鄰的四個圖素加權平均得到平滑的效果,可柔和鋸齒邊,內定值,

        　
    3. GL_TEXTURE_WRAP_S  材質S軸的纏繞方式(水平軸向).

    4. GL_TEXTURE_WRAP_T  材質T軸的纏繞方式(垂直軸向).
    param:
        GL_CLAMP 材質的重複樣式,當材質座標固定於0.0~1.0的範圍時,超過的部分座標恆為0.0 or 1.0

        GL_REPEAT 當材質座標超過0.0~1.0的範圍時,會自動重複材質的內容;也就是說會展開並複製材質的內容去符合材質座標,請看下面圖示^^.
    */
}

void OnGLFreeImageFunc( CPIcon* image )
{
    glDeleteTextures( 1, &image->Id );                        // Generate OpenGL texture IDs
}



namespace LGB
{
    enum FUNC_MODE
    {
        FM_LOOKAT = 0,   // 第一個bit設為0時 Camera為朝一個向量觀看的模式  1為 Camera盯住一個點的模式
        FM_DRAWBACK = 1,   // 
        FM_DEPTHTEST = 2,   // 會不會依照Z排序 還是依照畫的先後順序
        FM_CULLFACE = 3,   // 刪去背面
        FM_BLEND = 4,   // 半透明

        FM_DRAWBEGIN = 18,  // 每次開始繪製的時候被設定
        FM_DRAWMODE3D = 20,  // 第一個bit設為1時 為3D的座標模式   0為 2D的座標模式
    };


    // 設定投影模式 螢幕的  @長 @寬 @如果是3D的時候 一個距離單位 是幾個 pixels @3D投影 或 2D投影 傳入設定的flag
    //void SetProjectMode(float width, float height, float ppu, DRAW_MODE dmode, DWORD &flag);

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    VMCGL::VMCGL()
    {
        wglSwapIntervalEXT = 0;
        pixels_per_unit_ = 100.0f;
        utflag_ = 0;

        SET_BIT_FLAG( utflag_, FM_DRAWBACK );
        SET_BIT_FLAG( utflag_, FM_DEPTHTEST );
        SET_BIT_FLAG( utflag_, FM_BLEND );

        // 初始化camera的位置
        // 頭朝上 往-Z的地方看
        camera_axis_.up = 0.0f;
        //camera_axis_.up.y =  1.0f;
        //camera_axis_.up.z =  0.0f;
        camera_axis_.fw.x = 0.0f;
        camera_axis_.fw.y = 0.0f;
        camera_axis_.fw.z = -1.0f;
        camera_axis_.at.x = 0.0f;
        camera_axis_.at.y = 0.0f;
        camera_axis_.at.z = 0.0f;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //! win32 destructor
    VMCGL::~VMCGL()
    {

        // I get a blue screen on my laptop, when I do not delete the
        // textures manually before releasing the dc. Oh how I love this.
        Destroy();
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool VMCGL::Create( void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen )
    {
        if( hWnd == 0 )
            return false;

        CPIcon::SetPixelFmt( PixelFormat::A8B8G8R8 );
        CPIcon::RegisterLoadImageFunc( OnGLLoadImageFunc );
        CPIcon::RegisterFreeImageFunc( OnGLFreeImageFunc );

        vobj_hwidget_ = ( HWND )hWnd;

        static    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof( PIXELFORMATDESCRIPTOR ),    // Size Of This Pixel Format Descriptor
            1,                // Version Number
            PFD_DRAW_TO_WINDOW |        // Format Must Support vobj_hwidget_
            PFD_SUPPORT_OPENGL |        // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,            // Must Support Double Buffering
            PFD_TYPE_RGBA,                // Request An RGBA Format
            depth << 3,                    // Select Our Color Depth
            0, 0, 0, 0, 0, 0,            // Color Bits Ignored
            0,                            // No Alpha Buffer
            0,                            // Shift Bit Ignored
            0,                              // No Accumulation Buffer
            0, 0, 0, 0,                    // Accumulation Bits Ignored
            24,                            // Z-Buffer (Depth Buffer)
            StencilBuffer ? 1 : 0,        // Stencil Buffer Depth
            0,                // No Auxiliary Buffer
            PFD_MAIN_PLANE,            // Main Drawing Layer
            0,                // Reserved
            0, 0, 0                // Layer Masks Ignored
        };

        for( int i = 0; i < 5; ++i )
        {
            if( i == 1 )
            {
                if( StencilBuffer )
                {
                    //os::Printer::log("Cannot create a GL device with stencil buffer, disabling stencil shadows.", ELL_WARNING);
                }
                StencilBuffer = false;
                pfd.cStencilBits = 0;
            }
            else if( i == 2 )
            {
                pfd.cDepthBits = 24;
            }
            if( i == 3 ) // might be checked twice, but shouldn't matter
            {
                pfd.cDepthBits = 16;
            }
            else if( i == 4 )
            {
                //os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
                return false;
            }

            //                 // get hdc
            if( !( hDC = GetDC( ( HWND )vobj_hwidget_ ) ) )
            {
                //os::Printer::log("Cannot create a GL device context.", ELL_ERROR);
                continue;
            }

            GLuint PixelFormat;
            // choose pixelformat
            if( !( PixelFormat = ChoosePixelFormat( hDC, &pfd ) ) )
            {
                //os::Printer::log("Cannot find a suitable pixelformat.", ELL_ERROR);
                continue;
            }

            // set pixel format
            if( !SetPixelFormat( hDC, PixelFormat, &pfd ) )
            {
                //os::Printer::log("Cannot set the pixel format.", ELL_ERROR);
                continue;
            }

            // create rendering context
            if( !( hRC = wglCreateContext( hDC ) ) )
            {
                //os::Printer::log("Cannot create a GL rendering context.", ELL_ERROR);
                continue;
            }

            // activate rendering context
            if( !wglMakeCurrent( hDC, hRC ) )
            {
                //os::Printer::log("Cannot activate GL rendering context", ELL_ERROR);
                continue;
            }

            break;
        }

        //GLfloat  ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
        //glEnable(GL_LIGHTING);
        //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
        //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

        ChangeScreenSize( x, y, width, height, fullscreen );

        glShadeModel( GL_SMOOTH );                                    // Enable Smooth Shading
        glClearDepth( 1.0f );
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
        glDepthFunc( GL_LEQUAL );
        glFrontFace( GL_CW );                                         // 以順時鐘的方向 當成是正面 所以 如果 當設定GL_CULL_FACE 逆時鐘畫的圖就會不見

        glEnable( GL_TEXTURE_2D );                                    // Enable Texture Mapping  (不設 就會是一片白 可是打開後就不能畫線)

        if( IF_BIT_FLAG( utflag_, FM_CULLFACE ) )                        //要不要取消畫背面 (看順時針是背面 還是 逆時針)
            glEnable( GL_CULL_FACE );
        else
            glDisable( GL_CULL_FACE );

        if( IF_BIT_FLAG( utflag_, FM_DEPTHTEST ) )                       // Enable Depth Testing 前後排序
            glEnable( GL_DEPTH_TEST );
        else
            glDisable( GL_DEPTH_TEST );

        if( IF_BIT_FLAG( utflag_, FM_DRAWBACK ) )
            glAlphaFunc( GL_GREATER, 0.0f );                            // 上面的意義就是:當像點的alpha值大於0時,才寫入色彩及深度緩衝頁,所以本來透空區的alpha值就為0,故不被運算,所以Zbuffer值便不會改變.

        if( IF_BIT_FLAG( utflag_, FM_BLEND ) )
        {
            //glEnable(GL_ALPHA_TEST);                              // Enable Alpha Testing  (disable blending) 去背  沒有半透
            glEnable( GL_BLEND );                                     // Enable Alpha Testing  (disable blending) 去背+半透明
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );      //
        }
        else
        {
            glDisable( GL_BLEND );
        }


        // 這個要打開
        wglSwapIntervalEXT = ( WGLSWAPINTERVALEXT )wglGetProcAddress( "wglSwapIntervalEXT" );

        // set vsync
        if( wglSwapIntervalEXT )
            wglSwapIntervalEXT( 0 );

        return true;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::Destroy( void )
    {

        if( hRC )
        {
            if( !wglMakeCurrent( 0, 0 ) )
            {
                //os::Printer::log("Release of dc and rc failed.", ELL_WARNING);
            }

            if( !wglDeleteContext( hRC ) )
            {
                //os::Printer::log("Release of rendering context failed.", ELL_WARNING);
            }

            hRC = 0;
        }

        if( hDC )
            ReleaseDC( ( HWND )vobj_hwidget_, hDC );
        hDC = 0;
    };


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool VMCGL::ChangeScreenSize( int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen )    // Resize And Initialize The GL vobj_hwidget_
    {
        if( height == 0 )                                                // Prevent A Divide By Zero By
        {
            height = 1;                                                // Making Height Equal One
        }

        fullscreen_ = fullscreen;
        SetPixelMode( x, y, width, height, PixelFormat::A8B8G8R8 );
        // 設定預設的投影模式
        glViewport( x, y, width, height );                               // Reset The Current Viewport

        //SetProjectMode(GetWidth(), GetHeight(), pixels_per_unit_, DRAWMODE_3D, utflag_);
        SetDrawMode( DRAWMODE_3D );

        return true;
    }

    //=============================================================================
    // NAME: 
    // DESC: 初始化座標矩陣
    //=============================================================================
    void VMCGL::LoadIdentity( void )
    {
        glLoadIdentity();
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::LoadMatrix( LGB::MATRIX_TYPE t, LG::Matrix4f& mt )
    {
        switch( t )
        {
            case MT_MODELVIEW_MATRIX:
                {
                    glGetFloatv( GL_MODELVIEW_MATRIX, mt.m );
                }
                break;
            case MT_PROJECTION_MATRIX:
                {
                    glGetFloatv( GL_PROJECTION_MATRIX, mt.m );
                }
                break;
        }

    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::ClearCanvas( float r, float g, float b, float a )
    {
        glClearColor( r, g, b, a );                        // Black Background
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool VMCGL::BeginFrame( DWORD argv )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );    // Clear The Screen And The Depth Buffer
        glLoadIdentity();                                      // Reset The View
        SET_BIT_FLAG( utflag_, FM_DRAWBEGIN );
        return true;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::EndFrame( void )
    {
        SET_BIT_FLAG( utflag_, FM_DRAWBEGIN );
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    ///////////////////////////////////////////////////////////////////////////////
    // set a perspective frustum with 6 params similar to glFrustum()
    // (left, right, bottom, top, near, far)
    // Note: this is for row-major notation. OpenGL needs transpose it
    ///////////////////////////////////////////////////////////////////////////////
    void ModelGL::setFrustum(float l, float r, float b, float t, float n, float f)
    {
        matrixProjection.identity();
        matrixProjection[0]  = 2 * n / (r - l);
        matrixProjection[2]  = (r + l) / (r - l);
        matrixProjection[5]  = 2 * n / (t - b);
        matrixProjection[6]  = (t + b) / (t - b);
        matrixProjection[10] = -(f + n) / (f - n);
        matrixProjection[11] = -(2 * f * n) / (f - n);
        matrixProjection[14] = -1;
        matrixProjection[15] = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // set a orthographic frustum with 6 params similar to glOrtho()
    // (left, right, bottom, top, near, far)
    // Note: this is for row-major notation. OpenGL needs transpose it
    ///////////////////////////////////////////////////////////////////////////////
    void ModelGL::setOrthoFrustum(float l, float r, float b, float t, float n,
        float f)
    {
        matrixProjection.identity();
        matrixProjection[0]  = 2 / (r - l);
        matrixProjection[3]  = -(r + l) / (r - l);
        matrixProjection[5]  = 2 / (t - b);
        matrixProjection[7]  = -(t + b) / (t - b);
        matrixProjection[10] = -2 / (f - n);
        matrixProjection[11] = -(f + n) / (f - n);
    }
    */

    bool VMCGL::SetDrawMode( DRAW_MODE dmode )
    {
        // 3D的投影法
        // 要設為3D投影 可是目前設定的是2D
        if( ( dmode == DRAWMODE_3D ) )
        {
            // 目前已經是3D的鏡頭了
            if( IF_BIT_FLAG( utflag_, FM_DRAWMODE3D ) )
            {
                // 是不是第一次呼叫 就算模式沒有切換還是要呼叫camera投射
                if( !IF_BIT_FLAG( utflag_, FM_DRAWBEGIN ) )
                    return false;
                // 設定為被呼叫過了
                SET_BIT_FLAG( utflag_, FM_DRAWBEGIN );
            }

            //glViewport (0, 0, (GLsizei)(GetWidth()), (GLsizei)(GetHeight()));               // Reset The Current Viewport
            glMatrixMode( GL_PROJECTION );                                                     // Select The Projection Matrix
            glLoadIdentity();                                                                // Reset The Projection Matrix

            float rh = ( ( float )GetWidth() / ( float )GetHeight() );
            const GLdouble  zNear = 1.0, zFar = 500, fieldOfView = 45.0;
            GLdouble        size = ( zNear * tanf( _DEGREE_TO_RADIAN( fieldOfView ) / 2.0 ) );
            glFrustum( -size, size, -size / rh, size / rh, zNear, zFar );
            //gluPerspective (45.0f, rh, 1.0f, 100.0f);    // Calculate The Aspect Ratio Of The Window

            glMatrixMode( GL_MODELVIEW );                                        // Select The Modelview Matrix
            glLoadIdentity();

            /*
            GLfloat xmin, xmax, ymin, ymax;

            ymax = zNear * (GLfloat)tanf(fovy * (float)M_PI / 360);
            ymin = -ymax;
            xmin = ymin * aspect;
            xmax = ymax * aspect;

            glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
            *

            /*
            //glViewport(0,0,width,height);                                 // Reset The Current Viewport
            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            //glOrtho(-width/2, width/2, -height/2, height/2, -1000.0f, 1000.0f);
            // Calculate The Aspect Ratio Of The Window

            float rh=((float)GetWidth()/(float)GetHeight());
            //float rd=(height/2)/((float)ppu);
            //gluPerspective(45.0f,rh,0.0f,100.0f);  //==> OK
            //glFrustum(-rd*rh, rd*rh, -rd, rd, 1.0f, rd+1.0f);

            const GLdouble  zNear = 0.000, zFar = 1000, fieldOfView = 45.0;
            GLdouble        size  = (zNear * tanf(_DEGREE_TO_RADIAN(fieldOfView) / 2.0));
            glFrustum(-size, size, -size / rh, size / rh, zNear, zFar);
            //gluPerspective(45.0f, rh, 0.01f, 100.0f);

            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            */

            SET_BIT_FLAG( utflag_, FM_DRAWMODE3D );

            {
                //
                float radiiansz = _DEGREE_TO_RADIAN( camera_axis_.up );
                LG::Vector3f upv( sin( radiiansz ), cos( radiiansz ), 0 );

                double atx = camera_axis_.at.x;
                double aty = camera_axis_.at.y;
                double atz = camera_axis_.at.z;

                double fwx = camera_axis_.fw.x;
                double fwy = camera_axis_.fw.y;
                double fwz = camera_axis_.fw.z;


                if( IF_BIT_FLAG( utflag_, FM_LOOKAT ) )
                {
                    // 盯住一個點的模式
                    gluLookAt( atx, aty, atz,
                               fwx, fwy, fwz,
                               // 看的地方是一個點
                               upv.x, upv.y, upv.z );  // 向上是一個向量
                }
                else
                {
                    // 看著一個方向的模式
                    gluLookAt( atx, aty, atz,
                               fwx + atx, fwy + aty, fwz + atz,
                               // 看的是一個方向
                               upv.x, upv.y, upv.z );  // 向上是一個向量
                }
                //glTranslatef(-camera_axis_.m[6],-camera_axis_.m[7],-camera_axis_.m[8]);

            }


        }
        else if( dmode == DRAWMODE_2D )
        {
            // 目前已經是2D的鏡頭了
            if( !IF_BIT_FLAG( utflag_, FM_DRAWMODE3D ) )
            {
                // 是不是第一次呼叫 就算模式沒有切換還是要呼叫camera投射
                if( !IF_BIT_FLAG( utflag_, FM_DRAWBEGIN ) )
                    return false;
                // 設定為被呼叫過了
                SET_BIT_FLAG( utflag_, FM_DRAWBEGIN );
            }

            //glViewport(0,0,width,height);                                // Reset The Current Viewport

            glMatrixMode( GL_PROJECTION );                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            glOrtho( 0, GetWidth(), GetHeight(), 0, 0.0f, 1000.0f );

            glMatrixMode( GL_MODELVIEW );                                    // Select The Modelview Matrix
            glLoadIdentity();
            CANCEL_BIT_FLAG( utflag_, FM_DRAWMODE3D );
        }
        return true;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    bool VMCGL::BeginDraw(DRAW_MODE m)
    {
        SetProjectMode(GetWidth(), GetHeight(), pixels_per_unit_, m, utflag_);

        // 3D的投影法
        // 要設為3D投影 可是目前設定的是2D
        if((dmode==DRAWMODE_3D) && (!IF_BIT_FLAG(flag, FM_DRAWMODE3D)))
        {
            //glViewport(0,0,width,height);                                 // Reset The Current Viewport

            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            //glOrtho(-width/2, width/2, -height/2, height/2, -1000.0f, 1000.0f);
            // Calculate The Aspect Ratio Of The Window

            float rh=((float)width/(float)height);
            //float rd=(height/2)/((float)ppu);
            //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);  //==> OK
            //glFrustum(-rd*rh, rd*rh, -rd, rd, 1.0f, rd+1.0f);

            const GLdouble  zNear = 0.01, zFar = 1000, fieldOfView = 45.0;
            GLdouble        size  = (zNear * tanf(_DEGREE_TO_RADIAN(fieldOfView) / 2.0));
            glFrustum(-size, size, -size / rh, size / rh, zNear, zFar);


            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            SET_BIT_FLAG(flag, FM_DRAWMODE3D);
        }

        if((dmode==DRAWMODE_2D) && (IF_BIT_FLAG(flag, FM_DRAWMODE3D)))
        {
            //glViewport(0,0,width,height);                                // Reset The Current Viewport

            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            glOrtho(0, width, height, 0, 0.0f, 1000.0f);

            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            CANCEL_BIT_FLAG(flag, FM_DRAWMODE3D);
        }


        if(IF_BIT_FLAG(utflag_, FM_DRAWMODE3D))
        {
            float radiiansz = _DEGREE_TO_RADIAN(camera_axis_.up);
            LG::Vector3f upv(sin(radiiansz), cos(radiiansz), 0);

            double atx = camera_axis_.at.x/pixels_per_unit_;
            double aty = camera_axis_.at.y/pixels_per_unit_;
            double atz = camera_axis_.at.z/pixels_per_unit_;

            if(IF_BIT_FLAG(utflag_, FM_LOOKAT))
            {
                // 盯住一個點的模式
                gluLookAt(atx, aty, atz,
                          camera_axis_.fw.x, camera_axis_.fw.y, camera_axis_.fw.z,
                          // 看的地方是一個點
                          upv.x, upv.y, upv.z);  // 向上是一個向量
            }
            else
            {
                // 看著一個方向的模式
                gluLookAt(atx, aty, atz,
                          camera_axis_.fw.x+atx, camera_axis_.fw.y+aty, camera_axis_.fw.z+atz,
                          // 看的是一個方向
                          upv.x, upv.y, upv.z);  // 向上是一個向量
            }
            //glTranslatef(-camera_axis_.m[6],-camera_axis_.m[7],-camera_axis_.m[8]);
        }
        else
        {
            // 2D模式下 部用移動
            //glTranslatef(0,0,1);
            //glTranslatef(-camera_axis_.m[6],-camera_axis_.m[7],-camera_axis_.m[8]);
        }

        return true;

    }
    */




    //=============================================================================
    // NAME:
    // DESC: 改變Camera的位置
    //=============================================================================
    void VMCGL::SetCameraPosition( const LG::Vertex3f& p )
    {
        camera_axis_.at.x = p.x;
        camera_axis_.at.y = p.y;
        camera_axis_.at.z = p.z;
    }


    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCGL::SetCameraLookFw( const LG::Vector3f& p )
    {
        // 將Camera設定為直視的模式
        camera_axis_.fw.x = p.x;
        camera_axis_.fw.y = p.y;
        camera_axis_.fw.z = p.z;
        NormalizeFunc( &camera_axis_.fw );
        CANCEL_BIT_FLAG( utflag_, FM_LOOKAT );
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void VMCGL::SetCameraLookAt( const LG::Vertex3f& p )
    {
        // 將Camera設定為盯看的模式
        // 這邊將 to 當成一的點
        camera_axis_.fw.x = p.x;
        camera_axis_.fw.y = p.y;
        camera_axis_.fw.z = p.z;
        SET_BIT_FLAG( utflag_, FM_LOOKAT );
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    const LG::Axis3f* VMCGL::GetCameraAxis( void )
    {
        return &camera_axis_;
    }




    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::DrawLine2i( const LG::Point& pt1, const LG::Point& pt2, const LC::RGBAb& color, float width )
    {
        SetDrawMode( DRAWMODE_2D );

        glDisable( GL_TEXTURE_2D );

        glLineWidth( width );

        glColor4ub( color.R, color.G, color.B, color.A );

        glBegin( GL_LINES );
        glVertex2i( pt1.x, pt1.y );
        glVertex2i( pt2.x, pt2.y );
        glEnd();

        glColor3f( 1.0f, 1.0f, 1.0f );

        glEnable( GL_TEXTURE_2D );
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::DrawLine3f( const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, float width )
    {
        SetDrawMode( DRAWMODE_3D );

        glDisable( GL_TEXTURE_2D );

        glLineWidth( width );

        glColor4ub( color.R, color.G, color.B, color.A );

        glBegin( GL_LINES );
        glVertex3f( pt1.x, pt1.y, pt1.z );
        glVertex3f( pt2.x, pt2.y, pt2.z );
        glEnd();

        glColor3f( 1.0f, 1.0f, 1.0f );

        glEnable( GL_TEXTURE_2D );
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    void  VMCGL::DrawQuads(float w, float h, const LC::RGBAb &color, const LG::Axis3f& m)
    {
        SetDrawMode(DRAWMODE_3D);

        glPushMatrix();

        float px = 0;
        float py = 0;
        w = w/pixels_per_unit_;
        h = h/pixels_per_unit_;



        glDisable(GL_TEXTURE_2D);

        //
        //
        glTranslatef(m.at.x, m.at.y, m.at.z);

        LG::Vector3f nv; //法向量
        LG::Vector3f zv(0, 0, 1); //z軸

        if(m.fw!=zv)
        {
            // 先取出與Z軸的法相量
            _Cross_product(nv, zv, m.fw);
            float degree0 = _RADIAN_TO_DEGREE(_VectorIncludedRadian(zv, m.fw));
            glRotatef(degree0, nv.x, nv.y, nv.z);                    // Rotate On The Y-Axis (Left - Right)
        }

        if(m.up !=0)
        {
            glRotatef(m.up, 0, 0, 1);                            // 轉動圖片的up
        }

        float zp=0;
        glBegin(GL_QUADS);                                        // Draw Our Quad
            glColor4b(color.R, color.G, color.B, color.A);                        // Set Top Point Of Triangle To Red
            //glColor4f(((float)color.R)/255.0f, ((float)color.G)/255.0f, ((float)color.B)/255.0f, ((float)color.A)/255.0f);                        // Set Top Point Of Triangle To Red
            //glColor4f(1.0f, 0.0f, 0.0f, 0.5f);                        // Set Top Point Of Triangle To Red
            glVertex3f(px,   py+h, 0);
            glVertex3f(px+w, py+h, 0);
            glVertex3f(px+w, py,   0);
            glVertex3f(px,   py,   0);
        glEnd();                                                // Done Drawing The Quad

        glColor3f(1.0f,1.0f,1.0f);                              //清空混色

        glEnable(GL_TEXTURE_2D);

        glPopMatrix();
        return;
    }
    */

    void VMCGL::DrawQuads( const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LG::Vertex3f& pt3, const LG::Vertex3f& pt4, const LC::RGBAb& color )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        glDisable( GL_TEXTURE_2D );

        //glColor4f(1.0f, 0.0f, 0.0f, 0.5f);                        // Set Top Point Of Triangle To Red
        glColor4f( ( ( float )color.R ) / 255.0f, ( ( float )color.G ) / 255.0f, ( ( float )color.B ) / 255.0f, ( ( float )color.A ) / 255.0f );                        // Set Top Point Of Triangle To Red

        glBegin( GL_QUADS );                                        // Draw Our Quad
            //glColor4b(color.R, color.G, color.B, color.A);                        // Set Top Point Of Triangle To Red
            //glColor4f(((float)color.R)/255.0f, ((float)color.G)/255.0f, ((float)color.B)/255.0f, ((float)color.A)/255.0f);                        // Set Top Point Of Triangle To Red
        glVertex3f( pt1.x, pt1.y, pt1.z );
        glVertex3f( pt2.x, pt2.y, pt2.z );
        glVertex3f( pt3.x, pt3.y, pt3.z );
        glVertex3f( pt4.x, pt4.y, pt4.z );
        glEnd();                                                // Done Drawing The Quad

        glColor3f( 1.0f, 1.0f, 1.0f );                              //清空混色

        glEnable( GL_TEXTURE_2D );

        glPopMatrix();
        return;


    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::DrawSphere( const LG::Vertex3f& pfstl, const LC::RGBAb& color, double radius, int slices, int stacks )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        GLUquadricObj* quadratic = gluNewQuadric();                        // Create A Pointer To The Quadric Object
        gluQuadricNormals( quadratic, GLU_SMOOTH );                        // Create Smooth Normals
        //gluQuadricTexture(quadratic, GL_FALSE);                            // Create Texture Coords


        LG::Vertex3f vt( ( pfstl.x ), ( pfstl.y ), ( pfstl.z ) );

        glTranslatef( vt.x, vt.y, vt.z );
        glColor4ub( color.R, color.G, color.B, color.A );
        //gluSphere(quadratic,1.3f,20,20);
        gluSphere( quadratic, ( radius ), slices, stacks );

        gluDeleteQuadric( quadratic );

        glColor3f( 1.0f, 1.0f, 1.0f );                              //清空混色

        glPopMatrix();
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::DrawCylinder( const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, double baseRadius, double topRadius, int slices, int stacks )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        GLUquadricObj* quadratic = gluNewQuadric();                        // Create A Pointer To The Quadric Object
        gluQuadricNormals( quadratic, GLU_SMOOTH );                        // Create Smooth Normals
        gluQuadricTexture( quadratic, GL_TRUE );                            // Create Texture Coords

        LG::Vertex3f vt1( ( pt1.x ), ( pt1.y ), ( pt1.z ) );
        LG::Vertex3f vt2( ( pt2.x ), ( pt2.y ), ( pt2.z ) );

        glTranslatef( vt1.x, vt1.y, vt1.z );
        glColor4ub( color.R, color.G, color.B, color.A );

        LG::Vector3f v1( vt2.x - vt1.x, vt2.y - vt1.y, vt2.z - vt1.z );
        LG::Vector3f v2( 0, 0, 1 );
        LG::Vector3f cp;

        // 先找出和Z軸的法向量
        // 法向量就是我要轉得軸
        Cross_productFunc( &cp, &v2, &v1 );

        // 要轉幾度
        float degree = _RADIAN_TO_DEGREE( VectorIncludedRadianFunc( &v1, &v2 ) );

        // 要畫多長
        float height = VertexToVertexFunc( &vt1, &vt2 );

        glRotatef( degree, cp.x, cp.y, cp.z );

        gluCylinder( quadratic, ( baseRadius ), ( topRadius ), height, slices, stacks );

        gluDeleteQuadric( quadratic );

        glColor3f( 1.0f, 1.0f, 1.0f );                              //清空混色

        glPopMatrix();
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int VMCGL::Blit( const Icon* Src, const LG::Point3& m )
    {
        SetDrawMode( DRAWMODE_2D );

        glPushMatrix();
        glLoadIdentity();

        float px = m.x - ( ( Icon* )Src )->GetClipOffsetX();
        float py = m.y - ( ( Icon* )Src )->GetClipOffsetY();
        float w = ( ( float )Src->width );
        float h = ( ( float )Src->height );

        glBindTexture( GL_TEXTURE_2D, Src->Id );                // (CHANGE)

        glBegin( GL_QUADS );
        // Front Face
        //glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f( 0.0f, 0.0f ); glVertex3f( px, py, 0.0f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex3f( px + w, py, 0.0f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex3f( px + w, py + h, 0.0f );
        glTexCoord2f( 0.0f, 1.0f ); glVertex3f( px, py + h, 0.0f );
        glEnd();

        glPopMatrix();
        return 1;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int VMCGL::BlitXY( const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        float px = -( ( float )( ( Icon* )Src )->GetClipOffsetX() ) / pixels_per_unit_;
        float py = -( ( float )( ( Icon* )Src )->GetClipOffsetY() ) / pixels_per_unit_;
        float w = ( ( float )Src->width ) / pixels_per_unit_;
        float h = ( ( float )Src->height ) / pixels_per_unit_;

        glBindTexture( GL_TEXTURE_2D, Src->Id );                // (CHANGE)

        //
        //
        glTranslatef( v.x / pixels_per_unit_, v.y / pixels_per_unit_, v.z / pixels_per_unit_ );

        if( a.roll != 0 )
            glRotatef( a.roll, 1.0f, 0.0f, 0.0f );                  // Rotate On The X-Axis (Roll)
        if( a.pitch != 0 )
            glRotatef( a.pitch, 0.0f, 1.0f, 0.0f );                   // Rotate On The Y-Axis (Pitch)
        if( a.yaw != 0 )
            glRotatef( a.yaw, 0.0f, 0.0f, 1.0f );                      // Rotate On The Z-Axis (Yaw)

        glBegin( GL_QUADS );                                        // Draw Our Quad
        glTexCoord2f( 0.0f, 0.0f );    glVertex3f( px, py + h, 0 );
        glTexCoord2f( 1.0f, 0.0f );    glVertex3f( px + w, py + h, 0 );
        glTexCoord2f( 1.0f, 1.0f );    glVertex3f( px + w, py, 0 );
        glTexCoord2f( 0.0f, 1.0f );    glVertex3f( px, py, 0 );
        glEnd();                                                // Done Drawing The Quad

        glPopMatrix();
        return 1;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int VMCGL::BlitXY( const Icon* Src, const LG::Axis3f& m )
    {
        /*
        glBegin(GL_QUADS);                                        // Draw Our Quad
        //glTexCoord2f(0.0f, 0.0f);    glVertex3f(px,   py+h, zp);
        //glTexCoord2f(1.0f, 0.0f);    glVertex3f(px+w, py+h, zp);
        //glTexCoord2f(1.0f, 1.0f);    glVertex3f(px+w, py,   zp);
        //glTexCoord2f(0.0f, 1.0f);    glVertex3f(px,   py,   zp);
        glVertex3f(0,   0, 0);
        glVertex3f(1.0,   0, 0);
        glVertex3f(1.0,  -1.0, 0);
        glVertex3f(0,  -1.0, 0);
        glEnd();                                                // Done Drawing The Quad
        return 1;
        */

        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        float px = -( ( float )( ( Icon* )Src )->GetClipOffsetX() ) / pixels_per_unit_;
        float py = +( ( float )( ( Icon* )Src )->GetClipOffsetY() ) / pixels_per_unit_;
        float w = ( ( float )Src->width ) / pixels_per_unit_;
        float h = ( ( float )Src->height ) / pixels_per_unit_;

        glBindTexture( GL_TEXTURE_2D, Src->Id );                // (CHANGE)

        //
        //
        glTranslatef( m.at.x / pixels_per_unit_, m.at.y / pixels_per_unit_, m.at.z / pixels_per_unit_ );


        LG::Vector3f nv; //法向量
        //LG::Vector3f yv(0, 1, 0); //y軸
        LG::Vector3f zv( 0, 0, 1 ); //z軸

        if( m.fw != zv )
        {
            // 先取出與Z軸的法相量
            Cross_productFunc( &nv, &zv, &m.fw );
            float degree0 = _RADIAN_TO_DEGREE( VectorIncludedRadianFunc( &zv, &m.fw ) );
            glRotatef( degree0, nv.x, nv.y, nv.z );                    // Rotate On The Y-Axis (Left - Right)
        }

        //if(m.up!=yv)
        if( m.up != 0 )
        {
            //float degree1 = _RADIAN_TO_DEGREE(_VectorIncludedRadian(yv, m.up));
            //glRotatef(degree1, 0, 0, 1);                        // 轉動圖片的up
            glRotatef( m.up, 0, 0, 1 );                            // 轉動圖片的up
        }

        float zp = 0;
        glBegin( GL_QUADS );                                        // Draw Our Quad
            //glTexCoord2f(0.0f, 0.0f);    glVertex3f(px,   py+h, zp);
            //glTexCoord2f(1.0f, 0.0f);    glVertex3f(px+w, py+h, zp);
            //glTexCoord2f(1.0f, 1.0f);    glVertex3f(px+w, py,   zp);
            //glTexCoord2f(0.0f, 1.0f);    glVertex3f(px,   py,   zp);
        glTexCoord2f( 0.0f, 0.0f );    glVertex3f( px, py, zp );
        glTexCoord2f( 1.0f, 0.0f );    glVertex3f( px + w, py, zp );
        glTexCoord2f( 1.0f, 1.0f );    glVertex3f( px + w, py - h, zp );
        glTexCoord2f( 0.0f, 1.0f );    glVertex3f( px, py - h, zp );
        glEnd();                                                // Done Drawing The Quad

        glPopMatrix();
        return 1;


        /*
        glLoadIdentity();                                    // Reset The View
        //glLoadMatrixf(m);
        glTranslatef(m.m[12], m.m[13], m.m[14]);

        float px = -((Icon*)Src)->GetClipOffsetX();
        float py =  ((Icon*)Src)->GetClipOffsetY();
        float w =((float)Src->width)/((float)GetWidth());
        float h =((float)Src->height)/((float)GetHeight());


        glBegin(GL_QUADS);
        // Front Face
        //glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(px,    py,  0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(px+w,  py,  0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(px+w,  py+h,  0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(px,    py+h,  0.0f);
        glEnd();


        //glTranslatef(0.0f,0.0f,-15);
        */

    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int VMCGL::BlitXZ( const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        float px = -( ( float )( ( Icon* )Src )->GetClipOffsetX() ) / pixels_per_unit_;
        float py = +( ( float )( ( Icon* )Src )->GetClipOffsetY() ) / pixels_per_unit_;
        float w = ( ( float )Src->width ) / pixels_per_unit_;
        float h = ( ( float )Src->height ) / pixels_per_unit_;


        glBindTexture( GL_TEXTURE_2D, Src->Id );                // (CHANGE)
        //
        //
        glTranslatef( v.x / pixels_per_unit_, v.y / pixels_per_unit_, v.z / pixels_per_unit_ );

        if( a.roll != 0 )
            glRotatef( a.roll, 1.0f, 0.0f, 0.0f );                      // Rotate On The X-Axis (Roll)
        if( a.pitch != 0 )
            glRotatef( a.pitch, 0.0f, 1.0f, 0.0f );                      // Rotate On The Y-Axis (Pitch)
        if( a.yaw != 0 )
            glRotatef( a.yaw, 0.0f, 0.0f, 1.0f );                  // Rotate On The Z-Axis (Yaw)


        float yp = 0;
        glBegin( GL_QUADS );                                        // Draw Our Quad
        glTexCoord2f( 0.0f, 0.0f );    glVertex3f( px, yp, py );
        glTexCoord2f( 1.0f, 0.0f );    glVertex3f( px + w, yp, py );
        glTexCoord2f( 1.0f, 1.0f );    glVertex3f( px + w, yp, py + h );
        glTexCoord2f( 0.0f, 1.0f );    glVertex3f( px, yp, py + h );
        glEnd();                                                // Done Drawing The Quad

        glPopMatrix();
        return 1;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int VMCGL::BlitXZ( const Icon* Src, const LG::Axis3f& m )
    {
        SetDrawMode( DRAWMODE_3D );

        glPushMatrix();

        float px = -( ( float )( ( Icon* )Src )->GetClipOffsetX() ) / pixels_per_unit_;
        float py = +( ( float )( ( Icon* )Src )->GetClipOffsetY() ) / pixels_per_unit_;
        float w = ( ( float )Src->width ) / pixels_per_unit_;
        float h = ( ( float )Src->height ) / pixels_per_unit_;

        glBindTexture( GL_TEXTURE_2D, Src->Id );                // (CHANGE)

        //
        glTranslatef( m.at.x / pixels_per_unit_, m.at.y / pixels_per_unit_, m.at.z / pixels_per_unit_ );


        LG::Vector3f nv; //法向量
        //LG::Vector3f yv(0, 1, 0); //y軸
        LG::Vector3f zv( 0, 0, 1 ); //z軸

        if( m.fw != zv )
        {
            // 先取出與Z軸的法相量
            Cross_productFunc( &nv, &zv, &m.fw );
            float degree0 = _RADIAN_TO_DEGREE( VectorIncludedRadianFunc( &zv, &m.fw ) );
            glRotatef( degree0, nv.x, nv.y, nv.z );                    // Rotate On The Y-Axis (Left - Right)
        }

        //if(m.up!=yv)
        if( m.up != 0 )
        {
            //float degree1 = _RADIAN_TO_DEGREE(_VectorIncludedRadian(yv, m.up));
            //glRotatef(degree1, 0, 0, 1);                        // 轉動圖片的up
            glRotatef( m.up, 0, 0, 1 );                            // 轉動圖片的up
        }


        float yp = 0;
        glBegin( GL_QUADS );                                        // Draw Our Quad
        glTexCoord2f( 0.0f, 0.0f );    glVertex3f( px, yp, py );
        glTexCoord2f( 1.0f, 0.0f );    glVertex3f( px + w, yp, py );
        glTexCoord2f( 1.0f, 1.0f );    glVertex3f( px + w, yp, py + h );
        glTexCoord2f( 0.0f, 1.0f );    glVertex3f( px, yp, py + h );
        glEnd();                                                // Done Drawing The Quad

        glPopMatrix();
        return 1;
    }



    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void VMCGL::Flip( void )
    {
        SwapBuffers( hDC );
    }



    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    /*
    void SetProjectMode(float width, float height, float ppu, DRAW_MODE dmode, DWORD &flag)
    {
        // 3D的投影法
        // 要設為3D投影 可是目前設定的是2D
        if((dmode==DRAWMODE_3D) && (!IF_BIT_FLAG(flag, FM_DRAWMODE3D)))
        {
            //glViewport(0,0,width,height);                                 // Reset The Current Viewport

            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            //glOrtho(-width/2, width/2, -height/2, height/2, -1000.0f, 1000.0f);
            // Calculate The Aspect Ratio Of The Window

            float rh=((float)width/(float)height);
            //float rd=(height/2)/((float)ppu);
            //gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);  //==> OK
            //glFrustum(-rd*rh, rd*rh, -rd, rd, 1.0f, rd+1.0f);

            const GLdouble  zNear = 0.01, zFar = 1000, fieldOfView = 45.0;
            GLdouble        size  = (zNear * tanf(_DEGREE_TO_RADIAN(fieldOfView) / 2.0));
            glFrustum(-size, size, -size / rh, size / rh, zNear, zFar);


            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            SET_BIT_FLAG(flag, FM_DRAWMODE3D);
        }

        if((dmode==DRAWMODE_2D) && (IF_BIT_FLAG(flag, FM_DRAWMODE3D)))
        {
            //glViewport(0,0,width,height);                                // Reset The Current Viewport

            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            glOrtho(0, width, height, 0, 0.0f, 1000.0f);

            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            CANCEL_BIT_FLAG(flag, FM_DRAWMODE3D);
        }

    }
    */






} // end namespace

/*

void Enable2D()
{
int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;

glViewport      (0, 0, width, height);

glMatrixMode    (GL_PROJECTION);
glLoadIdentity  ();
glOrthof(0, (float)width, 0, (float)height, 0, 100);
glMatrixMode    (GL_MODELVIEW);
glLoadIdentity();

glEnable                (GL_BLEND);
glEnable                (GL_TEXTURE_2D);
glDisable                (GL_CULL_FACE);
glDisable                (GL_DEPTH_TEST);
glDisable                (GL_LIGHTING);
//glDisableClientState    (GL_NORMAL_ARRAY);

}


void Enable3D()
{
int width = SCREEN_WIDTH;
int height = SCREEN_HEIGHT;
float aspect = (float)width/(float)height;

glViewport          (0, 0, width, height);
glScissor           (0, 0, width, height);

//glMatrixMode        (GL_MODELVIEW);
//glLoadIdentity      ();

glMatrixMode        (GL_PROJECTION);
glLoadIdentity      ();
InitPerspective     (60.f, aspect, 0.1f, 1000.f);

glMatrixMode        (GL_MODELVIEW);
glLoadIdentity      ();


glEnable           (GL_CULL_FACE);
//glDisable            (GL_BLEND);
glEnable            (GL_DEPTH_TEST);
//glEnable            (GL_LIGHTING);
//glEnableClientState (GL_NORMAL_ARRAY);

//glEnable(GL_NORMALIZE);
}
*/



