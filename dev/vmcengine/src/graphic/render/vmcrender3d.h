#ifndef _VMCRENDER_H
#define _VMCRENDER_H


#include "IVMCRender.h"
#include <vector>


namespace LGB  //game entity render
{
	class IVMCDevice;

	class VMCRender3d : public IVMCRender
	{
		// 繪圖元件的分類
		LGB::DrawNode::rendercontainer_t m_BlitContainer[DrawNode::MAX_DrawLayerS];
		DrawNode*						 m_pFocusNode;
		LGB::IVMCDevice*                 m_pDrawDevice;  //image;
		short                            m_DeivceType;

	public:

		VMCRender3d();
		~VMCRender3d();

		// 建立的模式 1:DX 2:GDI 3:OpenGL   4:Virtual
		virtual bool  CreateDevice          (DeviceTypeId type, void* hWnd,
                                             int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
			                                 bool fullscreen = true);
		virtual void  Destroy();

		virtual bool  BeginFrame            (DWORD argv=0);
		virtual void  EndFrame              (void);
		virtual void  BlitAll               ();
		virtual void  Flip                  ();                               //將一記憶體貼至螢幕
				

		/** 設定focus的物件，這會讓蓋著它的物件以半透明的方式繪製。
		 @param focusNode 該物件的指標。 */
		void          SetFocus(DrawNode* focusNode);

		/** 繪製。其實這裡會回呼DrawNode的RenderTo()。 */
		void          DoRender(DrawNode* pDrawNode);



		//=======================================================================
		// 繪圖的組合function


		//
		//=======================================================================
		// 改變Camera的位置
		void                  SetCameraPosition(const LG::Vertex3f& p);
		// 改變Camera的視角固定朝一個方向看 傳入"單位"向量  ex (0, 0, -1)
		void                  SetCameraForward (const LG::Vector3f& p);
		// 設定Camera盯住某一個點           傳入點  ex (100, 50, 20)
		void                  SetCameraLookAt  (const LG::Vertex3f& p);
		// 取出camera的軸資料
		const LG::Axis3f*     GetCameraAxis    (void);

    protected:

		LGB::IVMCDevice*      GetDrawDevice(){return m_pDrawDevice;}

		//  判斷角色有沒有被建築物遮蔽
		void          ProcessTransparency();


		void          InsertNode(DrawNode* node, DrawNode::DrawLayer layer);
		void          RemoveNode(DrawNode* node, DrawNode::DrawLayer layer);

	};
};


#endif  //_RENDER3D_H