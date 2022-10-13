/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "DsCanvas.h"


#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <math.h>
#include <aMath.inl>


/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/HelloJni/HelloJni.java
 *
 */


namespace DrawSprit
{
    enum DRAW_MODE
    {
        DRAWMODE_2D      = 2,
        DRAWMODE_3D      = 3
    };

    enum SETTING_FLAG
    {
        SF_LOOKAT        = 0,   // 第一個bit設為0時 Camera為朝一個向量觀看的模式  1為 Camera盯住一個點的模式
        SF_DRAWBACK      = 1,   //
        SF_DEPTHTEST     = 2,   // 會不會依照Z排序 還是依照畫的先後順序
        SF_CULLFACE      = 3,   // 刪去背面
        SF_BLEND         = 4,   // 半透明

        SF_DRAWBEGIN     = 18,  // 每次開始繪製的時候被設定
        SF_DRAWMODE3D    = 20,  // 第一個bit設為1時 為3D的座標模式   0為 2D的座標模式
    };

    bool SetDrawMode(DRAW_MODE dmode, int w, int h, LG::Axis3f& CameraAxis, DWORD& SetFlag);

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Canvas::Canvas()
    {
        m_Top = 0;
        m_Left = 0;
        m_Width = 0;
        m_Height = 0;
        m_SetFlag = 0;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    Canvas::~Canvas()
    {

    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    static void _gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
    {
        GLfloat top = zNear * ((GLfloat) tan(fovy * _PI_ / 360.0));
        GLfloat bottom = -top;
        GLfloat left = bottom * aspect;
        GLfloat right = top * aspect;
        glFrustumf(left, right, bottom, top, zNear, zFar);
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    static void gluLookAt(float eyeX, float eyeY, float eyeZ,
            float centerX, float centerY, float centerZ, float upX, float upY,
            float upZ)
    {
        // See the OpenGL GLUT documentation for gluLookAt for a description
        // of the algorithm. We implement it in a straightforward way:

        float fx = centerX - eyeX;
        float fy = centerY - eyeY;
        float fz = centerZ - eyeZ;

        // Normalize f
        float rlf = 1.0f / sqrtf(fx*fx + fy*fy + fz*fz);
        fx *= rlf;
        fy *= rlf;
        fz *= rlf;

        // Normalize up
        float rlup = 1.0f / sqrtf(upX*upX + upY*upY + upZ*upZ);
        upX *= rlup;
        upY *= rlup;
        upZ *= rlup;

        // compute s = f x up (x means "cross product")

        float sx = fy * upZ - fz * upY;
        float sy = fz * upX - fx * upZ;
        float sz = fx * upY - fy * upX;

        // compute u = s x f
        float ux = sy * fz - sz * fy;
        float uy = sz * fx - sx * fz;
        float uz = sx * fy - sy * fx;

        float m[16] ;
        m[0] = sx;
        m[1] = ux;
        m[2] = -fx;
        m[3] = 0.0f;

        m[4] = sy;
        m[5] = uy;
        m[6] = -fy;
        m[7] = 0.0f;

        m[8] = sz;
        m[9] = uz;
        m[10] = -fz;
        m[11] = 0.0f;

        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;

        glMultMatrixf(m);
        glTranslatef(-eyeX, -eyeY, -eyeZ);
    }


    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool Canvas::Create(int top, int left, uint_t width, uint_t height)
    {
        Resize(left, top, width, height);

        glShadeModel(GL_SMOOTH);                                    // Enable Smooth Shading
        glClearDepthf(1.0f);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CW);                                         // 以順時鐘的方向 當成是正面 所以 如果 當設定GL_CULL_FACE 逆時鐘畫的圖就會不見

        glEnable(GL_TEXTURE_2D);                                    // Enable Texture Mapping  (不設 就會是一片白 可是打開後就不能畫線)

        if(IF_BIT_FLAG(m_SetFlag, SF_CULLFACE))                        //要不要取消畫背面 (看順時針是背面 還是 逆時針)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        if(IF_BIT_FLAG(m_SetFlag, SF_DEPTHTEST))                       // Enable Depth Testing 前後排序
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);

        if(IF_BIT_FLAG(m_SetFlag, SF_DRAWBACK))
            glAlphaFunc(GL_GREATER,0.0f);                            // 上面的意義就是:當像點的alpha值大於0時,才寫入色彩及深度緩衝頁,所以本來透空區的alpha值就為0,故不被運算,所以Zbuffer值便不會改變.

        if(IF_BIT_FLAG(m_SetFlag, SF_BLEND))
        {
            //glEnable(GL_ALPHA_TEST);                              // Enable Alpha Testing  (disable blending) 去背  沒有半透
            glEnable(GL_BLEND);                                     // Enable Alpha Testing  (disable blending) 去背+半透明
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      //
        }
        else
        {
            glDisable(GL_BLEND);
        }

        eglSwapInterval(EGL_DEFAULT_DISPLAY, 0);

        //SetDrawMode(DRAWMODE_2D, width, height, m_CameraAxis, m_SetFlag);
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Canvas::Resize(int top, int left, uint_t width, uint_t height)
    {
        glViewport(left, top, width, height);                            // Reset The Current Viewport
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CW);                                         // 以順時鐘的方向 當成是正面 所以 如果 當設定GL_CULL_FACE 逆時鐘畫的圖就會不見
        glEnable(GL_TEXTURE_2D);                                    // Enable Texture Mapping  (不設 就會是一片白 可是打開後就不能畫線)
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t Canvas::GetWidth()
    {
        return m_Width;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t Canvas::GetHeight()
    {
    	return m_Height;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool SetDrawMode(DRAW_MODE dmode, int w, int h, LG::Axis3f& CameraAxis, DWORD& SetFlag)
    {
        // 3D的投影法
        // 要設為3D投影 可是目前設定的是2D
        if((dmode==DRAWMODE_3D))
        {
            // 目前已經是3D的鏡頭了
            if(IF_BIT_FLAG(SetFlag, SF_DRAWMODE3D))
            {
                // 是不是第一次呼叫 就算模式沒有切換還是要呼叫camera投射
                if(!IF_BIT_FLAG(SetFlag, SF_DRAWBEGIN))
                    return false;
                // 設定為被呼叫過了
                SET_BIT_FLAG(SetFlag, SF_DRAWBEGIN);
            }

            //glViewport (0, 0, (GLsizei)(GetWidth()), (GLsizei)(GetHeight()));               // Reset The Current Viewport
            glMatrixMode (GL_PROJECTION);                                                     // Select The Projection Matrix
            glLoadIdentity ();                                                                // Reset The Projection Matrix

            float rh=((float)w/(float)h);
            const double  zNear = 1.0, zFar = 500, fieldOfView = 45.0;
            double        size  = (zNear * tanf(_DEGREE_TO_RADIAN(fieldOfView) / 2.0));
            glFrustumf(-size, size, -size / rh, size / rh, zNear, zFar);
            //gluPerspective (45.0f, rh, 1.0f, 100.0f);    // Calculate The Aspect Ratio Of The Window

            glMatrixMode (GL_MODELVIEW);                                        // Select The Modelview Matrix
            glLoadIdentity ();

            /*
            GLfloat xmin, xmax, ymin, ymax;

            ymax = zNear * (GLfloat)tanf(fovy * (float)M_PI / 360);
            ymin = -ymax;
            xmin = ymin * aspect;
            xmax = ymax * aspect;

            glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
            */

            SET_BIT_FLAG(SetFlag, SF_DRAWMODE3D);

            {
                //
                float radiiansz = _DEGREE_TO_RADIAN(CameraAxis.up);
                LG::Vector3f upv(sin(radiiansz), cos(radiiansz), 0);

                double atx = CameraAxis.at.x;
                double aty = CameraAxis.at.y;
                double atz = CameraAxis.at.z;

                double fwx = CameraAxis.fw.x;
                double fwy = CameraAxis.fw.y;
                double fwz = CameraAxis.fw.z;


                if(IF_BIT_FLAG(SetFlag, SF_LOOKAT))
                {
                    // 盯住一個點的模式
                    gluLookAt(atx, aty, atz,
                              fwx, fwy, fwz,
                              // 看的地方是一個點
                              upv.x, upv.y, upv.z);  // 向上是一個向量
                }
                else
                {
                    // 看著一個方向的模式
                    gluLookAt(atx, aty, atz,
                              fwx+atx, fwy+aty, fwz+atz,
                              // 看的是一個方向
                              upv.x, upv.y, upv.z);  // 向上是一個向量
                }
                //glTranslatef(-m_CameraAxis.m[6],-m_CameraAxis.m[7],-m_CameraAxis.m[8]);
            }

        }
        else if(dmode==DRAWMODE_2D)
        {
            // 目前已經是2D的鏡頭了
            if(!IF_BIT_FLAG(SetFlag, SF_DRAWMODE3D))
            {
                // 是不是第一次呼叫 就算模式沒有切換還是要呼叫camera投射
                if(!IF_BIT_FLAG(SetFlag, SF_DRAWBEGIN))
                    return false;
                // 設定為被呼叫過了
                SET_BIT_FLAG(SetFlag, SF_DRAWBEGIN);
            }

            //glViewport(0,0,width,height);                                // Reset The Current Viewport

            glMatrixMode(GL_PROJECTION);                                // Select The Projection Matrix
            glLoadIdentity();                                            // Reset The Projection Matrix

            glOrthof(0, w, h, 0, 0.0f, 1000.0f);

            glMatrixMode(GL_MODELVIEW);                                    // Select The Modelview Matrix
            glLoadIdentity();
            CANCEL_BIT_FLAG(SetFlag, SF_DRAWMODE3D);
        }
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    bool Canvas::BeginFrame(DWORD argv)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear The Screen And The Depth Buffer
        glLoadIdentity();                                      // Reset The View
        SET_BIT_FLAG(m_SetFlag, SF_DRAWBEGIN);
        return true;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Canvas::EndFrame()
    {
        SET_BIT_FLAG(m_SetFlag, SF_DRAWBEGIN);
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    uint_t Canvas::LoadImage(const void* pixels, int width, int height)
    {
        GLuint tid=0;
        glGenTextures(1, &tid);

        glBindTexture(GL_TEXTURE_2D, tid);                                   // Bind Our Texture
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);    // Linear Filtered // 似乎會影像畫線?不能拿掉
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    // Linear Filtered
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        return tid;
    }

    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
    void Canvas::FreeImage(uint_t textureid)
    {
        glDeleteTextures(1, &textureid);
    }

}
