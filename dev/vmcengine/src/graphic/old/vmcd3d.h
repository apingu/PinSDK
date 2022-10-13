//===========================================================================================
// Stick Picture Kernel Class
//
//
//
//
//
//
//
//
//
//                                               Use attention +ddraw.lib dxguid.lib D3d8.lib
//                                               Copyright    (C)    1999    Pin
//
// =========================================================================================
#ifndef VMCD3D_H
#define VMCD3D_H


#include <d3d8.h>
#include <windows.h>
#include <io.h>

#include "VMC.h"


#pragma comment (lib,"ddraw.lib" )
#pragma comment (lib,"dxguid.lib" )
#pragma comment (lib,"D3d8.lib" )
#pragma comment (lib,"d3dx8.lib")


#ifndef __D3DX8MATH_H__
#include <d3dx8math.h>
#endif

#ifndef _DXERR8_H_
#include <dxerr8.h>
#pragma comment (lib,"dxerr8.lib")
#endif


#define OUTTRACE(s)   OutputDebugString(s)

#define POOL_MANAGED 1

/*
#ifndef LPDIRECT3DDEVICE8
#define LPDIRECT3DDEVICE8 LPDIRECT3DDEVICE8;
#endif 
*/

#ifndef VAS_TEXTURE
#define VAS_TEXTURE LPDIRECT3DTEXTURE8
#endif

// 定義DX版本物件別名
#define VAS_D3D		LPDIRECT3D8
#define VAS_DEVICE	LPDIRECT3DDEVICE8
#define VAS_CAPS	D3DCAPS8
#define VAS_SPRITE	LPD3DXSPRITE
#define VAS_TEXTURE	LPDIRECT3DTEXTURE8
#define VAS_VERTEXBUFFER	LPDIRECT3DVERTEXBUFFER8
#define VAS_VERTEXSHADER	LPDIRECT3DVERTEXSHADER8
#define VAS_SURFACE		IDirect3DSurface8*
#define Direct3DCreate(a) Direct3DCreate8( a )

#ifndef VnMsg
#define VnMsg(a,b) {MessageBox(0,a,b,MB_OK);}
#endif

	class DxIcon : public CPIcon
	{

	public:

		LPDIRECT3DTEXTURE8 TexPtr;

		DxIcon(){	TexPtr = NULL; };
		void DumpPixelsToTexture( LPDIRECT3DDEVICE8 Device );
		~DxIcon()
		{
		if ( TexPtr != NULL )
			TexPtr->Release();
		};
		int Load( const char* pathname )
		{
			CPIcon::Load( pathname );
			//PRGBAToPABGR();
//		    P565ToPARGB();
			ReverseImage();


			return 1;
		}
	};

//-----------------------------------------------------------------------
//			vFont Code this Block
//=======================================================================

#include <vector>
#include <string>
#include <algorithm>

#include <cassert>

#ifdef _DEBUG
	#define Assert(expr) assert(expr)
	#define Verify(expr) assert(expr)
#else

	#define Assert(expr) ((void)0)
	#define Verify(expr) expr
#endif
struct FONT2DVERTEX
{
	float x, y, z, w;
	D3DCOLOR color;
	float u, v;
	
	FONT2DVERTEX(float fx, float fy, float fz, D3DCOLOR clr, float fu, float fv) : 
		x(fx), y(fy), z(fz), w(1.0f), color(clr), u(fu), v(fv) {}
};

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)


using namespace std;
	class iFont
	{
		public:
			iFont();
			~iFont();

		bool Init( VAS_DEVICE pd3dDevice, char szFontName[], int nSize = 16, int nLevel = 1 );

		bool TextOut( char szText[], int x, int y, D3DCOLOR color = 0xFFFFFFFF, int nLen = -1, float fScale = 1.0f );

		void Release();

		private:
			void Char2Texture( char c1, char c2, int & tX, int & tY );

		private:
			HDC			_hDc;			// 設備句柄
			HBITMAP		_hBmp;			// 位圖句柄
			HFONT		_hFont;			// 字體句柄
			UINT		_Max;			// 緩沖最大數
			int			_RowNum;		// 一行可以容納的文字個數
			DWORD *		_pBits;			// 位圖的數據指針

			int			_TextSize;		// 文字的大小
			int			_TextureSize;	// 緩沖紋理的大小

			struct Char{
				WORD	hz;				// 文字

				Char() : hz(0) {}

		// 查找文字
		inline bool operator == ( WORD h ) const
		{
			return hz==h ? true : false;
		}
	};
	vector <Char> _vBuf;					// 記錄緩沖中現有的文字情況

	VAS_DEVICE				_pd3dDevice;	// D3D device
	VAS_VERTEXBUFFER		_pVB;			// 文字的頂點緩沖區
	VAS_TEXTURE				_pTexture;		// 包含文字的紋理
};

//-----------------------------------------------------------------------
//			vFont  End this parse
//=======================================================================

//-----------------------------------------------------------------------------
// Name: class VMCDX
// Desc: 2D draw class use for directX draw mode
//------------------------------------------------------------------------------
namespace LGB
{

	class VMCD3D : public VMC
	{
	public:											// We Need Shared Device for another com init
	    LPDIRECT3DDEVICE8       pD3dDevice;		   	// 
	protected:
		LPDIRECT3D8             pD3d;				// LPDIRECT3DDEVICE8       pD3dDevice;					    	// 
		LPD3DXSPRITE		    pSprite;		    // Sprite object
	    D3DPRESENT_PARAMETERS   D3dpp;				// 
	    D3DCAPS8				D3dcaps;			// 
		D3DFORMAT				d_TextureFormat	;
		int  JudgeVideoMask( void );

	public:

		VMCD3D();
		~VMCD3D();
		//VanishSprite						_Sprite;
		bool        Create                 ( HWND hWnd );
		bool        SetScreenMode          ( bool fullscreen = true,
			                                 int width = 800, int height = 600, int depth = 2,
											 int backc = 2 );

		bool        CreateEX               ( HWND hWnd,
			                                 int width = 800, int height = 600,
											 int depth = 2,
											 bool screenflag = false
							    			);                         //DX Fullscreen or Windows

		bool        ClearScreen            ( COLORREF color, bool bZBuffer=false );    //清除螢幕


		void        Destroy                ( void );                                 //消滅DX物件

		bool        CanvasCreate           ( CMemCanvas** canvas, int width, int height, int depth = 0 );     //建立虛擬的畫布
		bool        CanvasDestroy          ( CMemCanvas  *canvas );			                                  //消滅虛擬的畫布

		// D3D 成像 Functions
		// Edit By abyss 2003-11-21 Change log Create 3D Fucntion
		inline bool BeginScene();									// (在任何繪圖前 call)
		inline bool EndScene();										// (在任何繪圖後 call )
		inline void Flip( void* src );

		long CreateTexture( char *file, DWORD ColorKey = 0, VAS_TEXTURE *pTex = 0 );
		long CreateTexture( BYTE *Mem,  int length, DWORD ColorKey = 0, VAS_TEXTURE *pTex = 0 );
		long CreateCostomTexture( int width, int height ,VAS_TEXTURE *Tex, bool D3DManaged);
	    bool Draw( LPDIRECT3DTEXTURE8 _Tex, RECT *rc,int Width=0, int Height=0, COLORREF color = 0xFFFFFFFF, int FlipFlags = 0, int Rotation = 0 );
	    bool DrawEx( LPDIRECT3DTEXTURE8 _Tex, RECT *rc,int x=0,int y=0,int Width=0, int Height=0, COLORREF color = 0xFFFFFFFF, int FlipFlags = 0, int Rotation = 0 );
	    int	 AlphaBlit( const Icon* Src, LG::Point point, CMemCanvas *Dest = NULL );
		int	 AlphaBlit( const DxIcon* Src, LG::Point point, CMemCanvas *Dest=NULL );
	};

	// 開始布景(在任何繪制前調用)
	inline bool VMCD3D::BeginScene( void )
	{
		if ( pD3dDevice==NULL )
			return false;// D3D設備沒有建立


		HRESULT hr;
		if( FAILED( hr = pD3dDevice->TestCooperativeLevel() ) )
		{
			// 當D3DDevice丟失時,要等到設備有效后方可繼續
			if( D3DERR_DEVICELOST==hr )
				return false;
			
			// Reset D3DDevice
			if( D3DERR_DEVICENOTRESET==hr )
				return false;

			return false;
		}
		
		pD3dDevice->BeginScene() ;

		return true;
	}

	// 結束布景(在任何繪制后調用)
	inline bool VMCD3D::EndScene( void )
	{
		if ( pD3dDevice==NULL )
			return false;// D3D設備沒有建立


	   // 渲染結束
		pD3dDevice->EndScene();

		return true;
	}

	inline void VMCD3D::Flip( void* src )
	{
		// 相當于Flip函數
		pD3dDevice->Present( NULL, NULL, NULL, NULL );
		return;

	}

	//------------------------ inline functions definition ----------------end

};//namespace LGB

#endif//VMCD3D_H