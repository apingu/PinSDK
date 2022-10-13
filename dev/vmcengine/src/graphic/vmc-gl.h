// Copyright (C) 2002-2007
//
// For conditions of distribution and use, see copyright notice in Irrlicht.h
//

#ifndef VMCGL_H
#define VMCGL_H

//01. == C 系統文件/C++ 系統文件
#ifdef _WINDOWS
// include windows headers for HWND

#include <GL/gl.h>
#include <GL/glu.h>
#endif

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <color.h>
#include <graphic3d.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "icon.h"
#include "ivmc-device3d.h"


#define _VMC_DEVICE_ID_3DGL   11

namespace LGB
{
    class VMCGL : public IVMCDevice3d
    {
#ifdef _WINDOWS
        HDC   hDC; // Private GDI Device Context
        HGLRC hRC; // Permanent Rendering Context

#elif defined(_LINUX)

        GLXDrawable XWindow;
        Display* XDisplay;
#endif

        bool                 fullscreen_;
        void*                vobj_hwidget_;

        bool StencilBuffer;

        typedef void (APIENTRY *WGLSWAPINTERVALEXT)(int);
        WGLSWAPINTERVALEXT   wglSwapIntervalEXT;

        LG::Axis3f           camera_axis_;
        int                  pixels_per_unit_;  //每一個單位裏面 比對到幾個像素 (預設 0.01 代表1個像素) 
        DWORD                utflag_;
 
        //CPIcon* CurrentTexture[4];

        bool                 SetDrawMode       (DRAW_MODE dmode);

    public:
            
        VMCGL();
        //! destructor
        virtual ~VMCGL();

        DWORD                 DeviceType(){ return _VMC_DEVICE_ID_3DGL;}

        void*                 GetWidget             (void) { return vobj_hwidget_; }
        bool                  GetFullScreen         (void) { return fullscreen_; }


        HDC                   GetMemGC(){ return hDC; }
        
        virtual bool          Create          (void* hWnd, int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
                                               bool fullscreen=false);
        virtual void          Destroy         (void);

        // depth 沒有用處
        virtual bool          ChangeScreenSize(int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
                                               bool fullscreen=false);

        void                  ClearCanvas     (float r=0.0f, float g=0.0f, float b=0.0f, float a=0.0f);

        // BeginFrame{  BeginDraw{      EndDraw}   EndFrame}
        bool                  BeginFrame      (DWORD argv=0);
        void                  EndFrame        (void);

        //bool                  BeginDraw       (DRAW_MODE m);
        //void                  EndDraw         (void);
        
        virtual void          Flip            (void);                      // 將一記憶體貼至螢幕



        //=======================================================================
        //GL function
        // 初始化座標矩陣
        void                    LoadIdentity(void);
        // 取得繪圖矩陣
        void                  LoadMatrix(LGB::MATRIX_TYPE t, LG::Matrix4f &mt);

        //=======================================================================
        // 改變Camera的位置
        void                  SetCameraPosition(const LG::Vertex3f& p);
        // 改變Camera的視角固定朝一個方向看 傳入"單位"向量  ex (0, 0, -1)
        void                  SetCameraLookFw  (const LG::Vector3f &p);
        // 設定Camera盯住某一個點           傳入點  ex (100, 50, 20)
        void                  SetCameraLookAt  (const LG::Vertex3f& p);
        // 取出camera的軸資料
        const LG::Axis3f*     GetCameraAxis    (void);


        //=======================================================================
        void                  DrawLine2i(const LG::Point &pt1,    const LG::Point &pt2,    const LC::RGBAb &color, float width=1.0f);
        void                  DrawLine3f(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb &color, float width=1.0f);


        // 畫一個平面 (A要填1 才會出現)
        void                  DrawQuads(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, 
                                        const LG::Vertex3f& pt3, const LG::Vertex3f& pt4, 
                                        const LC::RGBAb &color);

        // 畫球
        void                  DrawSphere(const LG::Vertex3f& pfstl, const LC::RGBAb &color, double radius, int slices=20, int stacks=20);

        // 畫圓柱
        void                  DrawCylinder(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, 
                                           const LC::RGBAb &color,
                                           double baseRadius, double topRadius, int slices=20, int stacks=20);


        //

        // 2D模式下的繪圖方式
        int                   Blit(const Icon* Src, const LG::Point3& m);

        // 3D模式下的繪圖方式 (XY平面) 位置+跟3軸的 roll pitch yaw的角度
        int                   BlitXY(const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a);
        // 3D模式下的繪圖方式 (XY平面) 
        int                   BlitXY(const Icon* Src, const LG::Axis3f& m);

        // 3D模式下的繪圖方式 (XZ平面) 
        int                   BlitXZ(const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a);
        // 3D模式下的繪圖方式 (XZ平面) 
        int                   BlitXZ(const Icon* Src, const LG::Axis3f& m);

    };


} // VMCGL



#endif


