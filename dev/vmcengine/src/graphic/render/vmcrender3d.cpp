#include "VMCRender3d.h"

#include <VMCDX.h>
#include <VMCGDI.h>
#include <VMCGL.h>
#include <VMCVL.h>
#include <VMCDraw.h>


#ifdef _FSWINDOW_
#include <fswindow.h>
#endif

#include <aIntersect.inl>
#include <assert.h>

#define _NOLICENSE_

#ifndef _NOLICENSE_
CPIcon*   g_pLicense=NULL;
#endif



namespace LGB
{
	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	VMCRender3d::VMCRender3d()
	{
		m_pDrawDevice = NULL;
		m_pFocusNode = NULL;


#ifndef _NOLICENSE_
		g_pLicense=new CPIcon;
		if(!g_pLicense->Load(_T("license")))
			SAFE_DELETE(g_pLicense);
#endif
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	VMCRender3d::~VMCRender3d()
	{
#ifndef _NOLICENSE_
		SAFE_DELETE(g_pLicense);
#endif
		Destroy();
	}


	//=============================================================================
	//  Name : 
	//  Desc : 建立的模式 1:DX 2:GDI
	//=============================================================================
	bool VMCRender3d::CreateDevice(DeviceTypeId type, void* hWnd, int x, int y, uint_t width, uint_t height, uint_t depth, bool fullscreen)
	{
		if(m_pDrawDevice!=NULL)
		{
			m_pDrawDevice->Destroy();
			SAFE_DELETE(m_pDrawDevice);
		}

		switch(type)
		{
			/*
		case _VMCTYPEID_2DDX:
			m_pDrawDevice=new LGB::VMCDX;
			break;
		case _VMCTYPEID_2DGDI:
			m_pDrawDevice=new LGB::VPage;
			break;
			case _VMCTYPEID_2DVL:
			m_pDrawDevice=new LGB::VMCVL;
			break;
			*/
		case _VMCTYPEID_3DGL:
			m_pDrawDevice=new LGB::VMCGL;
			break;
		default:
			assert(0&&"create device type not exist!");
			break;
		}

		m_DeivceType=type;
		if(m_pDrawDevice)
		{
			m_pDrawDevice->Create(hWnd, x, y, width, height, depth, fullscreen);
#ifdef _FSWINDOW_
			if(type==_VMCTYPEID_2DDX)
			{
				FSWindow_Init((Pwnd)hWnd,
					(LPDIRECTDRAW7)(((LGB::VMCDX*)m_pDrawDevice)->GetDirectDraw()),
					(LPDIRECTDRAWSURFACE7)(((LGB::VMCDX*)m_pDrawDevice)->GetPrimSurface()),
					(LPDIRECTDRAWSURFACE7)(((LGB::VMCDX*)m_pDrawDevice)->GetBackSurface()));
			}
#endif

			return true;
		}
		return false;
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::Destroy()
	{
		SAFE_DELETE(m_pDrawDevice);
	}



	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::DoRender(DrawNode* pDrawNode)
	{
		//pDrawNode->RenderTo(*this); 
		pDrawNode->OnRenderTo((m_BlitContainer[ pDrawNode->RenderLayer ])); 
	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	bool VMCRender3d::BeginFrame(DWORD argv)
	{
		if(!m_pDrawDevice)
			return false;


		// 先判斷透空被遮住物
		ProcessTransparency();

		// 把建築物 跟角色放在一起
		for (UINT i = 0; i < m_BlitContainer[DrawNode::kLayerBld].size(); i++)
			m_BlitContainer[DrawNode::kLayerRole].push_back(m_BlitContainer[DrawNode::kLayerBld][i]);


		/* 3D不用排序
		if (!m_BlitContainer[DrawNode::kLayerRole].empty())
		{
			std::stable_sort(m_BlitContainer[DrawNode::kLayerRole].begin(),
				m_BlitContainer[DrawNode::kLayerRole].end(),
				SoryByPosition);
		}
		*/

		return m_pDrawDevice->BeginFrame(argv);
	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::EndFrame(void)
	{
		m_pDrawDevice->EndFrame();

		for (int i = 0; i < LGB::DrawNode::MAX_DrawLayerS; ++i)
		{
			m_BlitContainer[i].clear();
		}
	}



	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::BlitAll()
	{
		if(m_pDrawDevice==NULL)
			return;


		LGB::IVMCDevice3d* pDevice = (LGB::IVMCDevice3d*)GetDrawDevice();
		if(pDevice)
		{
			//if(pDevice->BeginDraw(DRAWMODE_3D))
			{
				// 先畫地表
				for (UINT j = 0; j < m_BlitContainer[DrawNode::kLayerBG].size(); j++)
				{
					DrawNode* pNode = m_BlitContainer[DrawNode::kLayerBG][j];
					pDevice->BlitXZ(pNode->OnGetRenderImage(),
						(pNode->OnGetRenderAxis()));
				}

				// 地表上層
				for (UINT j = 0; j < m_BlitContainer[DrawNode::kLayerSurface].size(); j++)
				{
					DrawNode* pNode = m_BlitContainer[DrawNode::kLayerSurface][j];
					pDevice->BlitXZ(pNode->OnGetRenderImage(),
						(pNode->OnGetRenderAxis()));
				}

				// 地表上物件
				for (UINT j = 0; j < m_BlitContainer[DrawNode::kLayerRole].size(); j++)
				{
					DrawNode* pNode = m_BlitContainer[DrawNode::kLayerRole][j];
					if(IF_BIT_FLAG(pNode->RenderType, DrawNode::NtBillBoard))
					{
						LG::Axis3f Axis = (pNode->OnGetRenderAxis());
						//Axis.fw = pDevice->GetCameraAxis()->at - Axis.at;
						Axis.fw = (pDevice->GetCameraAxis()->at) - Axis.at;
						pDevice->BlitXY(pNode->OnGetRenderImage(),
							Axis);
					}
					else
					{
						pDevice->BlitXY(pNode->OnGetRenderImage(),
							(pNode->OnGetRenderAxis()));
					}
					//pNode->OnBlitText(m_pFont);
				}

				for (UINT j = 0; j < m_BlitContainer[DrawNode::kLayerFX].size(); j++)
				{
					DrawNode* pNode = m_BlitContainer[DrawNode::kLayerFX][j];
					pDevice->BlitXY(pNode->OnGetRenderImage(),
						(pNode->OnGetRenderAxis()));
				}

				//pDevice->EndDraw();
			}


			//if(pDevice->BeginDraw(DRAWMODE_2D))
			{
				// UI層
				for (UINT j = 0; j<m_BlitContainer[DrawNode::kLayerUI].size(); ++j)
				{
					DrawNode* pNode = m_BlitContainer[DrawNode::kLayerUI][j];
					pDevice->Blit(pNode->OnGetRenderImage(),
						pNode->OnGetRenderPos());
					//pNode->OnBlitText(m_pFont);
				}
				//pDevice->EndDraw();
			}
		}

	}

	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::Flip()
	{
		//LGB::VMCDraw::AlphaBlit(g_pLicense, m_pFocusNode->OnGetRenderPos(), GetCanvas());
		if(m_pDrawDevice)
		{
#ifdef _FSWINDOW_
			if (FSWindow_IsActive())
			{
				FSWindow_Update();
			}
			else
			{
				m_pDrawDevice->Flip();
			}
#else
			m_pDrawDevice->Flip();
#endif
		}
	}


	//=======================================================================
	// 改變Camera的位置
	void  VMCRender3d::SetCameraPosition(const LG::Vertex3f& p)
	{
		LGB::IVMCDevice3d* pDevice = (LGB::IVMCDevice3d*)GetDrawDevice();
		if(pDevice)
			pDevice->SetCameraPosition(p);
	}

	// 改變Camera的視角固定朝一個方向看 傳入"單位"向量  ex (0, 0, -1)
	void  VMCRender3d::SetCameraForward(const LG::Vector3f& p)
	{
		LGB::IVMCDevice3d* pDevice = (LGB::IVMCDevice3d*)GetDrawDevice();
		if(pDevice)
		    pDevice->SetCameraLookFw(p);
	}

	// 設定Camera盯住某一個點           傳入點  ex (100, 50, 20)
	void VMCRender3d::SetCameraLookAt(const LG::Vertex3f& p)
	{
		LGB::IVMCDevice3d* pDevice = (LGB::IVMCDevice3d*)GetDrawDevice();
		if(pDevice)
			pDevice->SetCameraLookAt(p);
	}

	// 取出camera的軸資料
	const LG::Axis3f* VMCRender3d::GetCameraAxis(void)
	{
		LGB::IVMCDevice3d* pDevice = (LGB::IVMCDevice3d*)GetDrawDevice();
		if(pDevice)
			return pDevice->GetCameraAxis();
		return NULL;
	}


	/*
	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	LGB::IVMCDevice2d* VMCRender3d::GetDraw2dDevice()
	{
		if((m_pDrawDevice->DeviceType()==_VMCTYPEID_2DDX)||
			(m_pDrawDevice->DeviceType()==_VMCTYPEID_2DGDI)||
			(m_pDrawDevice->DeviceType()==_VMCTYPEID_2DVL))
		    return (LGB::IVMCDevice2d*)GetDraw2dDevice();
		return NULL;
	}


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	LGB::IVMCDevice3d* VMCRender3d::GetDraw3dDevice()
	{
		if((m_pDrawDevice->DeviceType()==_VMCTYPEID_3DGL))
			return (LGB::IVMCDevice3d*)GetDraw2dDevice();
		return NULL;
	}
	*/


	//=============================================================================
	//  Name : 
	//  Desc :
	//=============================================================================
	void VMCRender3d::ProcessTransparency()
	{
		if (m_pFocusNode != NULL)
		{
			for (UINT i = 0; i < m_BlitContainer[DrawNode::kLayerBld].size(); i++)
			{
				if (m_pFocusNode != m_BlitContainer[DrawNode::kLayerBld][i] &&
					m_pFocusNode->OnGetComparisonPos().z < m_BlitContainer[DrawNode::kLayerBld][i]->OnGetComparisonPos().z &&
					m_BlitContainer[DrawNode::kLayerBld][i]->OnIsOverlap(m_pFocusNode->OnGetComparisonPos()))
				{
					m_BlitContainer[DrawNode::kLayerBld][i]->m_Transparent = true;
				}
			}
		}
	}

	
    //=============================================================================
    //  Name : 
    //  Desc :
    //=============================================================================
	void VMCRender3d::InsertNode(DrawNode* node, DrawNode::DrawLayer layer)
	{
		if (layer < DrawNode::MAX_DrawLayerS)
		{
			assert (node != NULL && "VMCRender3d::InsertNode: A NULL node Pushed!!!");
			m_BlitContainer[layer].push_back(node);
		}
	}



    //=============================================================================
    //  Name :
    //  Desc :
    //=============================================================================
	void VMCRender3d::RemoveNode(DrawNode* node, DrawNode::DrawLayer layer)
	{
		if(layer < DrawNode::MAX_DrawLayerS)
		{
			for(UINT j=0; j<m_BlitContainer[layer].size(); j++)
			{
				if(m_BlitContainer[layer][j]==node)
				{
					m_BlitContainer[layer].erase(m_BlitContainer[layer].begin()+j);
					return;
				}
			}
		}
		return;
	}

}
