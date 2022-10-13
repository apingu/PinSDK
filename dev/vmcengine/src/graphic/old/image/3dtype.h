#ifndef __3D_TYPE__H
#define __3D_TYPE__H

//----------------------------------------------
//			DirectX 8 Lib & Include
//==============================================
#ifndef _D3D8_H_
#include <d3d8.h>

#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"d3d8.lib")
#pragma comment (lib,"d3dx8.lib")
#endif

#ifndef __D3DX8MATH_H__
#include <d3dx8math.h>
#endif

#ifndef __DINPUT_INCLUDED__
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment (lib,"dinput8.lib")
#endif

#ifndef _DXERR8_H_
#include <dxerr8.h>

#pragma comment (lib,"dxerr8.lib")
#endif
//################################################
//				3D object Type ReDefine 
//------------------------------------------------
#define PDEVICE			LPDIRECT3DDEVICE8			//	 Direct3D Device 
#define PTEXTURE		LPDIRECT3DTEXTURE8			//	 Direct3D Texture
#define PSPRITEX		LPD3DXSPRITE				//	 Direct3D SPrite

#endif