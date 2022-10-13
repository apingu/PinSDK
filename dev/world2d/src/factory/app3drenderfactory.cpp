
#include "App3dFactory.h"
#include <IVMCDevice3d.h>
#include <aMath.inl>

// 繪圖部份
#pragma comment ( lib, "Icon2d.lib" )
#pragma comment ( lib, "VMCEngine.lib" )
#pragma comment ( lib, "VMCRender.lib" )


namespace App3dFactory
{
	Render3d::Render3d()
	{

	}

	Render3d::~Render3d()
	{

	}

	void Render3d::DrawAxis(float l, const LC::RGBAb& c1, const LC::RGBAb& c2, const LC::RGBAb& c3, const LG::Axis3f& m)
	{
		LG::Vector3f fw;
		LG::Vector3f up;
		LG::Vector3f lf;

		//((LGB::IVMCDevice3d*)GetDrawDevice())->DrawLine3f(m.at,fwp, c1, l);
		//((LGB::IVMCDevice3d*)GetDrawDevice())->DrawLine3f(m.at, m.fw, c1, l);
		//((LGB::IVMCDevice3d*)GetDrawDevice())->DrawLine3f(m.at, m.fw, c1, l);

		AxisToVertex(fw, up, lf, &m);
		DrawCylinder(m.at, (m.at+up)*l, LC::RGBAb(0, 255, 0, 128), 0.05f, 0.05f);   // y 
	    DrawCylinder(m.at, (m.at+fw)*l, LC::RGBAb(0, 0, 255, 128), 0.05f, 0.05f);   // x
		DrawCylinder(m.at, (m.at+lf)*l, LC::RGBAb(255, 0, 0, 128), 0.05f, 0.05f);   // z


		//DrawCylinder(m.at, LG::Vertex3f(0, 1, 0), LC::RGBAb(0, 255, 0, 128), 0.05f, 0.05f);
		//DrawCylinder(m.at, LG::Vertex3f(1, 0, 0), LC::RGBAb(0, 0, 255, 128), 0.05f, 0.05f);
		//DrawCylinder(m.at, fwz, LC::RGBAb(255, 0, 0, 128), 0.05f, 0.05f);
		DrawSphere(m.at, LC::RGBAb(255, 255, 0, 128), 0.1f);
	}

	void Render3d::DrawQuads(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LG::Vertex3f& pt3, const LG::Vertex3f& pt4, const LC::RGBAb& color)
	{
		((LGB::IVMCDevice3d*)GetDrawDevice())->DrawQuads(pt1, pt2, pt3, pt4, color);
	}

	void Render3d::DrawSphere(const LG::Vertex3f& pt, const LC::RGBAb& color, double radius, int slices, int stacks)
	{
		((LGB::IVMCDevice3d*)GetDrawDevice())->DrawSphere(pt, color, radius, slices, stacks);
	}

	void Render3d::DrawCylinder( const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, double baseRadius, double topRadius, int slices, int stacks)
	{
		((LGB::IVMCDevice3d*)GetDrawDevice())->DrawCylinder(pt1, pt2, color, baseRadius, topRadius, slices, stacks);
	}

	// 3D模式下的繪圖方式 (畫線) 
	void Render3d::DrawLine3f(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, float width)
	{
		((LGB::IVMCDevice3d*)GetDrawDevice())->DrawLine3f(pt1, pt2, color, width);
		return;
	}

	int Render3d::Blit(const Icon *Src, const LG::Point3& m)
	{
		return ((LGB::IVMCDevice3d*)GetDrawDevice())->Blit(Src, m);
	}

	// 3D模式下的繪圖方式 (XY平面) 
	int Render3d::BlitXY(const Icon *Src, const LG::Axis3f& m)
	{
		return ((LGB::IVMCDevice3d*)GetDrawDevice())->BlitXY(Src, m);
	}

    void Render3d::LoadMatrix(LGB::MATRIX_TYPE t, LG::Matrix4f& mt)
    {
        return ((LGB::IVMCDevice3d*)GetDrawDevice())->LoadMatrix(t, mt);
    }

};

