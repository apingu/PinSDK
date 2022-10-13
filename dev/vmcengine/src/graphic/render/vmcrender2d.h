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
//                   if use dx fswindow define _FSWINDOW_
//
//
//
//
//
//
//
#ifndef RENDER2D_H
#define RENDER2D_H

#pragma warning(disable: 4786)

#include "IVMCRender.h"

#include <queue>
#include <vector>


//typedef void (*DrawStateFunc)(LGB::DrawNode* node, LGB::VMC* render, CPIcon* canvas);




namespace LGB  //game entity render
{
	class IVMCDevice2d;
	class VMCText;

	class DrawMethodFunctor
	{
	public:
		virtual void operator()(LGB::DrawNode* node, LGB::IVMCDevice2d* pDrawDevice) = 0;
	};


	class FontMethodFunctor
	{
	public:
		virtual void operator()(cText_Node* node, LGB::VMCText* pTextDevice) = 0;
	};


	// Forward Declarations
    class VMCRender2d : public IVMCRender
    {

    public:
		/** 建構子。 */
        VMCRender2d();
		/** 解構子。 */
        virtual ~VMCRender2d();

		// 建立的模式 1:DX 2:GDI 3:   4:Virtual
		virtual bool  CreateDevice          (DeviceTypeId type, void* hWnd,
                                             int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
			                                 bool fullscreen=true);

		// Definations of set in CreateFont() of Text
        // set #define FONT_SET_SBCS   0
        //     #define FONT_SET_DBCS   1
		// fmt 0
    	virtual int   CreateFont            (RENDER_FONTSET set, const tchar_t* filename, int w, int h, RENDER_FONTFMT fmt, int cache);

		virtual void  Destroy();

		virtual bool  BeginFrame            (DWORD argv=0);
		virtual void  EndFrame              (void);

		virtual void  BlitAll               ();


		virtual void  Flip                  ();                               //將一記憶體貼至螢幕


		bool          GetFullScreen();

	    void          SetFontColor(int fg, int bg, int opacity = 16);
		void          SetFontDropShadow(int color, int dist);   // will EnableDropShadow(1)
		void          SetFontSpace(int h, int v);
		LG::Size      GetFontSize(RENDER_FONTSET set);


	
		/**  取得繪圖器。
		 @return 繪圖的物件。 */
        //LGB::VMC*    GetRender(void)	{ return m_pRender; }

		/** 取得繪字用的物件。
		 @return 繪字用的物件。 */
		//LGB::VMCText* GetFont(void)	{ return m_pFontDevice; }

		/** 設定focus的物件，這會讓蓋著它的物件以半透明的方式繪製。
		 @param focusNode 該物件的指標。 */
		void          SetFocus(DrawNode* focusNode);

		/** 繪製。其實這裡會回呼DrawNode的RenderTo()。 */
		void          DoRender(DrawNode* pDrawNode);




		int           AlphaBlit(const Icon *Src, LG::Point point);
        int           Iconcpy(const Icon *Src, LG::Point point, Icon *Dest);
		void          PutsXY(int x, int y, const tchar_t* str);


		// DX 模式底下的 特製程式
		long          ChangeCoopLevel(bool bfullscreen);

	protected:
		

        void		  BlitTerrain();
		void		  BlitSurface();

		void		  BlitEntity();

		void		  BlitFX();
		void          BlitOnAir();

		void		  BlitUI();

		void	      BlitTop();
		

		void          ProcessTransparency();


		// 加入新的繪製方法  如果已經存在會回傳false 如果要替代方案 請先移除舊的
		bool          AddnewRenderFunc(DrawNode::DrawMethod id, LGB::DrawMethodFunctor* functor);
		void          RemoveRenderFunc(DrawNode::DrawMethod id);
		// 繪製影像
		void	      BlitImage(DrawNode* node);
		// 繪製文字
		void		  BlitText(cText_Node* node);


	public:

		//
		void          InsertTerrain(DrawNode* node);
		void          InsertSurface(DrawNode* node);

		void          InsertEntity(DrawNode* node);
		void          InsertFx(DrawNode* node);
		void          InsertUI(DrawNode* node);

		void          InsertCursor(DrawNode* node);

		void          InsertNode(DrawNode* node, DrawNode::DrawLayer layer);
		void          RemoveNode(DrawNode* node, DrawNode::DrawLayer layer);
		void          RemoveNode(DrawNode* node);



    protected:

		LGB::DrawNode::rendercontainer_t m_BlitContainer[DrawNode::MAX_DrawLayerS];


		DrawNode*                        m_Cursor;
		DrawNode*						 m_pFocusNode;

    private:

		//LGB::DrawNode::rendercontainer_t&  GetContainer(LGB::DrawNode::DrawLayer layer);
		std::vector< DrawMethodFunctor* > m_DrawMethodFuncList;
		std::vector< FontMethodFunctor* > m_TextMethodFuncList;
		short                             m_DeivceType;
        LGB::IVMCDevice2d*                m_pDrawDevice;     //image
        LGB::VMCText*                     m_pFontDevice;     //font

	protected:

		CPIcon*            GetCanvas(void);
        
		short              GetDeviceType() { return m_DeivceType; }
        LGB::IVMCDevice2d* GetDrawDevice() { return m_pDrawDevice; }
        LGB::VMCText*      GetpFontDevice(){ return m_pFontDevice; }

	public:

		static int	 	   PushText(LGB::VMCText *pTextDevice, cText_Node::tNode* node, LG::Point* pos, bool bmask=false);
		static void	       DrawText(LGB::VMCText *pTextDevice, cText_Node* node, LG::Point* pos);
		static void  	   DrawText(LGB::VMCText *pTextDevice, cText_Node* node, int start_line, LG::Rect rect);


    };

};//namespace LGB


#endif