#ifndef APP3DFACTORY_H
#define APP3DFACTORY_H

#include <VMCRender3d.h>

namespace App3dFactory
{
	// 繪圖用的控制物件
	class Render3d : public LGB::VMCRender3d
	{
	public:

		Render3d();
		~Render3d();

		void  DrawAxis(float l, const LC::RGBAb& c1, const LC::RGBAb& c2,const  LC::RGBAb& c3, const LG::Axis3f& m);

		// 3D模式下的繪圖方式 (畫線) 
		void  DrawLine3f(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LC::RGBAb& color, float width=1.0f);

		//(平面) 
		void  DrawQuads(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, const LG::Vertex3f& pt3, const LG::Vertex3f& pt4, const LC::RGBAb& color);
		// 3D模式下的繪圖方式 (畫球) 
		void  DrawSphere(const LG::Vertex3f& pt, const LC::RGBAb& color, double radius, int slices=20, int stacks=20);

		void  DrawCylinder(const LG::Vertex3f& pt1, const LG::Vertex3f& pt2, 
			               const LC::RGBAb& color,
			               double baseRadius, double topRadius, int slices=20, int stacks=20);

		// 2D模式下的繪圖方式
		int   Blit(const Icon *Src, const LG::Point3& m);
		// 3D模式下的繪圖方式 (XY平面) 
		int   BlitXY(const Icon *Src, const LG::Axis3f& m);

        void  LoadMatrix(LGB::MATRIX_TYPE t, LG::Matrix4f& mt);

	};

   
    class Terrain3d// : public GSO::World::Terrain2d
    {

    public:

        Terrain3d(){};
        ~Terrain3d(){};
    };
};



#endif APPUTILITY_H