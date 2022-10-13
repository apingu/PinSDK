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
//
//
//
#ifndef DXICON_H
#define DXICON_H
#define POOL_MANAGED 1

#include "Icon.h"
#include <d3d8.h>
#ifndef __D3DX8MATH_H__
#include <d3dx8math.h>
#endif

#ifndef _DXERR8_H_
#include <dxerr8.h>
#pragma comment (lib,"dxerr8.lib")
#endif

#define D3DDEVICE LPDIRECT3DDEVICE8
class DxIcon : public CPIcon
{
	public:
		DxIcon(){	TexPtr = NULL; };
		LPDIRECT3DTEXTURE8 TexPtr;
		void DumpPixelsToTexture(D3DDEVICE Device );
		~DxIcon()
	{
		if ( TexPtr != NULL )
			TexPtr->Release();
	};
};

#endif DXICON_H


