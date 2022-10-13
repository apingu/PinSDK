#ifndef IVMCREDNER_H
#define IVMCREDNER_H


#include <VMCDef.h>
#include "VMCRenderDef.h"
#include "DrawNode.h"
#include "TextNode.h"
#include <IVMCDevice.h>

namespace LGB  //game entity render
{
	class IVMCRender
	{
	public:
		
		IVMCRender(){};
		~IVMCRender(){};


		// 建立的模式 1:DX 2:GDI 3:   4:Virtual
		virtual bool  CreateDevice          (DeviceTypeId type, void* hWnd, 
                                             int x=0, int y=0, uint_t width=800, uint_t height=600, uint_t depth=2,
			                                 bool fullscreen=true)=0;

		virtual void  Destroy               ()=0;

		virtual bool  BeginFrame            (DWORD argv=0)=0;
		virtual void  EndFrame              (void)=0;

		virtual void  BlitAll               ()=0;

		virtual void  Flip                  ()=0;                               //將一記憶體貼至螢幕

		/** 繪製。其實這裡會回呼DrawNode的RenderTo()。 */
		virtual void  DoRender              (DrawNode* pDrawNode)=0;


	};


};

#endif