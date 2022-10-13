#ifndef _IVMCDEVICE3D_H
#define _IVMCDEVICE3D_H

#include "ivmc-device.h"

namespace LGB
{
    class IVMCDevice3d : public IVMCDevice
    {
    public:
        IVMCDevice3d(){}
        ~IVMCDevice3d(){}

        // foundation device interface
        virtual DWORD  DeviceType()=0; 

        virtual void*  GetWidget             (void)=0;
        virtual bool   GetFullScreen         (void)=0;


        virtual bool   Create                (void* hWnd, int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2,
                                              bool fullscreen = true) = 0;
        virtual bool   ChangeScreenSize      (int x = 0, int y = 0, uint_t width = 800, uint_t height = 600, uint_t depth = 2,
                                              bool fullscreen = true) = 0;
        virtual void   Destroy               (void) = 0;

        virtual bool   BeginFrame            (DWORD argv = 0) = 0;
        virtual void   EndFrame              (void) = 0;

        virtual void   Flip                  (void) = 0;                               //將一記憶體貼至螢幕


        //=======================================================================
        // 初始化座標矩陣
        virtual void   LoadIdentity(void) = 0;
        // 取得繪圖矩陣
        virtual void   LoadMatrix(LGB::MATRIX_TYPE t, LG::Matrix4f& mt) = 0;


        //
        //=======================================================================
        // 改變Camera的位置
        virtual void  SetCameraPosition(const LG::Vertex3f& p) = 0;
        // 改變Camera的視角固定朝一個方向看 傳入"單位"向量  ex (0, 0, -1)
        virtual void  SetCameraLookFw  (const LG::Vector3f& p) = 0;
        // 設定Camera盯住某一個點           傳入點  ex (100, 50, 20)
        virtual void  SetCameraLookAt  (const LG::Vertex3f& p) = 0;

        virtual const LG::Axis3f*   GetCameraAxis    (void) = 0;

        //=======================================================================
        virtual void  DrawLine2i(const LG::Point& pt1, const LG::Point& pt2, const LC::RGBAb& color, float width = 1.0f) = 0;
        virtual void  DrawLine3f(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, float width = 1.0f) = 0;


        // 畫一個平面 (A要填1 才會出現)
        virtual void  DrawQuads(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, 
                                const LG::Vertex3f& pt3, const LG::Vertex3f& pt4, 
                                const LC::RGBAb& color){};

        // 畫球
        virtual void  DrawSphere(const LG::Vertex3f& pfstl, const LC::RGBAb& color, double radius, int slices, int stacks){};
        // 畫圓柱
        virtual void  DrawCylinder(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, 
                                    const LC::RGBAb& color,
                                    double baseRadius, double topRadius, int slice, int stacks){};


        // 2D模式下的繪圖方式
        virtual int   Blit(const Icon* Src, const LG::Point3& m)=0;

        // 3D模式下的繪圖方式 (XY平面) 位置+跟3軸的 roll pitch yaw的角度
        virtual int   BlitXY(const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a) = 0;
        // 3D模式下的繪圖方式 (XY平面)
        virtual int   BlitXY(const Icon* Src, const LG::Axis3f& m) = 0;

        // 3D模式下的繪圖方式 (XZ平面)
        virtual int   BlitXZ(const Icon* Src, const LG::Vertex3f& v, const LG::Angle3f& a) = 0;
        // 3D模式下的繪圖方式 (XZ平面) 
        virtual int   BlitXZ(const Icon* Src, const LG::Axis3f& m) = 0;
    };
}

#endif //_IVMCDEVICE2D_H