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
#include "VMCD3D.h"
#include "assert.h"
unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header

 
void DxIcon::
//
//
DumpPixelsToTexture(LPDIRECT3DDEVICE8 Device )
{
	unsigned char *Mem;
	int MemSize;
	// Create New Memory
	MemSize = width * height * 4;
	if ( MemSize == 0 )
		return ;
	Mem = new unsigned char[ MemSize + 18];		// Add 18 for File Header 
//		char buf[256];
//		sprintf( buf, "Memory Size:%d ", MemSize);
//		MessageBox( NULL, buf , " Q____Q " , MB_OK );
//		exit(1);
    Mem[12]	=	(int)width%256;
	Mem[13]	=	(int)width/256;
	Mem[14]	=	(int)height%256;
	Mem[15]	=	(int)height/256;
	Mem[16]	=	(int)depth<<3;
	Mem[17]	=	0;
	memcpy( Mem, uTGAcompare,12 );
	memcpy( Mem+18,pixels,MemSize );

	for(int cswap = 18; cswap < MemSize; cswap += 4)
		Mem[cswap] ^= Mem[cswap+2] ^= Mem[cswap] ^= Mem[cswap+2];
	
	D3DXCreateTextureFromFileInMemoryEx( 
				Device,
				Mem,
				MemSize+18,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				1,
				0,
				D3DFMT_UNKNOWN,
				POOL_MANAGED?D3DPOOL_MANAGED:D3DPOOL_DEFAULT,			// 記憶體管理模式
				D3DX_FILTER_NONE,
				D3DX_FILTER_NONE, 
				D3DCOLOR_ARGB(255,0,0,0),		// 去背色.. ( 純綠 )
				NULL,
				NULL, 
				&TexPtr);	
	
	delete [] Mem;
	return ;
}

// --------------------------------------------------------
//			iFont Class Code Block
// ========================================================
// 構造函數
iFont::
/*-------------------------------------------------------------
void
-------------------------------------------------------------*/
iFont() : _hDc(NULL), _hBmp(NULL), _hFont(NULL), _Max(0), _RowNum(0), _pBits(NULL), 
	_TextSize(0), _TextureSize(0),
	_pd3dDevice(NULL), _pVB(NULL), _pTexture(NULL)
{
}

// 析構函數
iFont::
/*-------------------------------------------------------------
void
-------------------------------------------------------------*/
~iFont()
{
	Release();
}

// 初始化
bool iFont::
/*-------------------------------------------------------------
LPDIRECT3DDEVICE8 pd3dDevice  ---  D3DDevice設備
char szFontName[]			  ---  字體名(如: 細明體)
int nSize					  ---  字體大小, 只支持16和24
int nLevel					  ---  紋理的大小級別
-------------------------------------------------------------*/
Init( VAS_DEVICE pd3dDevice, char szFontName[], int nSize, int nLevel )
{
	Assert( pd3dDevice!=NULL );
	Assert ( szFontName!=NULL );
	Assert( nSize==16 || nSize==24  );	// 只支持16和24點陣
	Assert( nLevel>=1 && nLevel<=6 );	// 關系到紋理的尺寸 1=64 2=128 3=256 4=512 5=1024

	Release();

	_TextSize	 = nSize;				// 文字大小
	_TextureSize = 32 << nLevel;		// 紋理大小
	
	_RowNum = _TextureSize / _TextSize;	// 計算一行可以容納多少個文字
	_Max = _RowNum * _RowNum;			// 計算緩沖最大值
	
	_pd3dDevice = pd3dDevice;

	// 建立hdc
	_hDc = CreateCompatibleDC(NULL);
	
	// 建立字體
	LOGFONT LogFont;
	ZeroMemory( &LogFont, sizeof(LogFont) );
	LogFont.lfHeight			= -_TextSize;
	LogFont.lfWidth				= 0;
	LogFont.lfEscapement		= 0;
	LogFont.lfOrientation		= 0;
	LogFont.lfWeight			= FW_BOLD;
	LogFont.lfItalic			= FALSE;
	LogFont.lfUnderline			= FALSE;
	LogFont.lfStrikeOut			= FALSE;
	LogFont.lfCharSet			= DEFAULT_CHARSET;
	LogFont.lfOutPrecision		= OUT_DEFAULT_PRECIS; 
	LogFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS; 
	LogFont.lfQuality			= DEFAULT_QUALITY;
	LogFont.lfPitchAndFamily	= DEFAULT_PITCH;
	lstrcpy( LogFont.lfFaceName, szFontName );
	
	_hFont = CreateFontIndirect( &LogFont );
	if ( NULL == _hFont )
	{
		DeleteDC( _hDc );
		return false;
	}
	
	// 創建bitmap
	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth		= _TextSize;
	bmi.bmiHeader.biHeight		= -_TextSize;
	bmi.bmiHeader.biPlanes		= 1;
	bmi.bmiHeader.biBitCount	= 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	_hBmp = CreateDIBSection( _hDc, &bmi, DIB_RGB_COLORS,
		(void **) &_pBits, NULL, 0 );
	if ( NULL == _hBmp || NULL == _pBits )
	{
		DeleteObject( _hFont );
		DeleteDC( _hDc );
		return false;
	}
	
	// 將hBmp和hFont加入到hDc
	SelectObject( _hDc, _hBmp );
	SelectObject( _hDc, _hFont );
	
	// 設置文字屬性
	SetTextColor( _hDc, RGB(255,255,255) );
	SetBkColor( _hDc, 0 );
	SetTextAlign( _hDc, TA_TOP );
	
	// 建立文字需要的頂點緩沖
#ifdef VANISH_DX9
	if ( FAILED( _pd3dDevice->CreateVertexBuffer( _Max * 6 * sizeof(FONT2DVERTEX),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
		D3DPOOL_DEFAULT, &_pVB
		, NULL		// pHandle
		) ) )
#else
	if ( FAILED( _pd3dDevice->CreateVertexBuffer( _Max * 6 * sizeof(FONT2DVERTEX),
		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
		D3DPOOL_DEFAULT, &_pVB
		) ) )
#endif
	{
		DeleteObject( _hFont );
		DeleteObject( _hBmp );
		DeleteDC( _hDc );
		return false;
	}
	
	// 建立文字的紋理
#ifdef VANISH_DX9
	if ( FAILED( _pd3dDevice->CreateTexture( _TextureSize, _TextureSize, 1, 0, 
		D3DFMT_A4R4G4B4, D3DPOOL_MANAGED,
		&_pTexture,
		NULL				// Stand by 
		) ) )
#else
	if ( FAILED( _pd3dDevice->CreateTexture( _TextureSize, _TextureSize, 1, 0, 
		D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &_pTexture ) ) )
#endif
	{
		DeleteObject( _hFont );
		DeleteObject( _hBmp );
		DeleteDC( _hDc );
		SAFE_RELEASE(_pVB);
 		return false;
	}
	_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,		FALSE );
	_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x08 );
	_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL );
	_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		
	_pd3dDevice->SetTexture( 0, _pTexture );
	#ifdef VANISH_DX9
	_pd3dDevice->SetVertexShader( NULL );
	#else
	_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
	#endif
	
	#ifdef VANISH_DX9
	_pd3dDevice->SetStreamSource( 0, _pVB, sizeof(FONT2DVERTEX) , 0);
	#else
	_pd3dDevice->SetStreamSource( 0, _pVB, sizeof(FONT2DVERTEX) );
	#endif
	// 設置緩沖的最大容量
	_vBuf.resize( _Max );

	return true;
}

// 得到文字在紋理中的位置
void iFont::
/*-------------------------------------------------------------
char c1   ---  文字的第1個字節
char c2   ---  文字的第2個字節
int & tX  ---  寫入紋理中的坐標x
int & tY  ---  寫入紋理中的坐標y
-------------------------------------------------------------*/
Char2Texture( char c1, char c2, int & tX, int & tY )
{
	WORD w = MAKEWORD(c1, c2);		// 把此字變為WORD
	vector<Char>::iterator it = find( _vBuf.begin(), _vBuf.end(), w );
	if ( it == _vBuf.end() )		// 如果沒找到
	{
		it = find( _vBuf.begin(), _vBuf.end(), 0 ); // 查找空閑位置
		if ( it == _vBuf.end() )	// 緩沖已滿
		{
			for(; it!=_vBuf.begin(); it-- )
			{
				it->hz = 0;
			}
//			Log.Output( "字體緩沖已滿, 清空!" );
		}

		// 計算當前空閑的Char在緩沖中是第几個
		int at = (int)(it-_vBuf.begin());

		// 得到空閑位置的坐標
		tX = (at % _RowNum) * _TextSize;
		tY = (at / _RowNum) * _TextSize;

		// 設置這個Char為使用中
		(*it).hz = w;

		RECT rect = {0, 0, _TextSize, _TextSize};
		char sz[3] = {c1, c2, '\0'};
		// 填充背景為黑色(透明色)
		FillRect( _hDc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
		// 往hBitmap上寫字
		::TextOut( _hDc, 0, 0, sz, c1 & 0x80 ? 2 : 1 );
		
		// 鎖定表面, 把漢字寫入紋理, 白色的是字(可見), 黑色為背景(透明)
		D3DLOCKED_RECT d3dlr;
		_pTexture->LockRect(0, &d3dlr, NULL, D3DLOCK_NOSYSLOCK);
		BYTE * pDstRow = (BYTE*)( (WORD *)d3dlr.pBits + tY * _TextureSize + tX );
		
		for (DWORD y=0; y<_TextSize; y++)
		{
			WORD * pDst16 = (WORD*)pDstRow;
			for (DWORD x=0; x<_TextSize; x++)
			{
				BYTE bAlpha = (BYTE)((_pBits[_TextSize * y + x] & 0xff) >> 4);
				if (bAlpha > 0)
					*pDst16++ = (bAlpha << 12) | 0x0fff;
				else
					*pDst16++ = 0x0000;
			}
			pDstRow += d3dlr.Pitch;
		}
		
		_pTexture->UnlockRect( NULL );
		
/* 用來查看文字使用情況
		Log.Output( "size = -----------", _vBuf.size() );
		for ( int m=0; m<_vBuf.size(); m++ )
		{
			Char * p = &(_vBuf[m]);
			char hz[3];
			hz[0] = LOBYTE(p->hz); hz[1] = HIBYTE(p->hz); hz[2]='\0';
			Log.Output( "%s - %d", hz, p->use );
		}
		Log.Output( "------------------" );
*/
	}
	else
	{
		// 計算當前空閑的Char在緩沖中是第几個
		int at = it-_vBuf.begin();

		// 得到這個字的坐標
		tX = (at % _RowNum) * _TextSize;
		tY = (at / _RowNum) * _TextSize;
	}
}

// 得到文字在紋理中的位置
bool iFont::
/*-------------------------------------------------------------
char szText[]  ---  顯示的字符串
int x		   ---  屏幕坐標x
int y		   ---  屏幕坐標y
D3DCOLOR	   ---  顏色及alpha值
int nLen	   ---  字符串長度
float fScale   ---  放大比例
-------------------------------------------------------------*/
TextOut( char szText[], int x, int y, D3DCOLOR color, int nLen, float fScale )
{
	Assert( szText!=NULL );

	float sx = x, sy = y,  offset=0, w=0, h=0, tx1=0, ty1=0, tx2=0, ty2=0;
	
	w = h = (float)_TextSize *fScale;

	char ch[3] = {0,0,0};
	FONT2DVERTEX * pVertices = NULL;
	UINT wNumTriangles = 0;
	#ifdef VANISH_DX9
	_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);
	#else
	_pVB->Lock(0, 0, (BYTE**)&pVertices, D3DLOCK_DISCARD);
	#endif
	if ( -1 == nLen ||				// 默認值-1
		 nLen > lstrlen( szText ) ) // 如果nLen大于字符串實際長度, 則nLen=實際長度
		nLen = lstrlen( szText );
	for (int n=0; n<nLen; n++ )
	{
		ch[0] = szText[n];

		if ( ch[0]=='\n' )
		{
			sy+=h;
			sx=x;
			continue;
		}

		if ( ch[0] & 0x80 )
		{
			n++;
			ch[1] = szText[n];
			offset = w;
		}
		else
		{
			ch[1] = '\0';
			offset = w / 2 ;
		}

		int a, b;
		Char2Texture( ch[0], ch[1], a, b );
	
		// 計算紋理左上角 0.0-1.0
		tx1 = (float)(a) / _TextureSize;
	    ty1 = (float)(b) / _TextureSize;
		// 計算紋理右上角 0.0-1.0
		tx2 = tx1 + (float)_TextSize / _TextureSize;
	    ty2 = ty1 + (float)_TextSize / _TextureSize;

		// 填充頂點緩沖區
		*pVertices++ = FONT2DVERTEX(sx,		sy + h, 0.9f, color, tx1, ty2);
		*pVertices++ = FONT2DVERTEX(sx,		sy,		0.9f, color, tx1, ty1);
		*pVertices++ = FONT2DVERTEX(sx + w, sy + h, 0.9f, color, tx2, ty2);
		*pVertices++ = FONT2DVERTEX(sx + w, sy,		0.9f, color, tx2, ty1);
		*pVertices++ = FONT2DVERTEX(sx + w, sy + h,	0.9f, color, tx2, ty2);
		*pVertices++ = FONT2DVERTEX(sx,		sy,		0.9f, color, tx1, ty1);

		wNumTriangles+=2;

		sx+=offset;	// 坐標x增量
	}
/*
//測試當前紋理的使用情況
		sx = sy = 50;
		w = h = _TextureSize;
		tx1 = ty1 = 0;
		tx2 = ty2 = 1;
		*pVertices++ = FONT2DVERTEX(sx,		sy + h, 0.1f, color, tx1, ty2);
		*pVertices++ = FONT2DVERTEX(sx,		sy,		0.1f, color, tx1, ty1);
		*pVertices++ = FONT2DVERTEX(sx + w, sy + h, 0.1f, color, tx2, ty2);
		*pVertices++ = FONT2DVERTEX(sx + w, sy,		0.1f, color, tx2, ty1);
		*pVertices++ = FONT2DVERTEX(sx + w, sy + h,	0.1f, color, tx2, ty2);
		*pVertices++ = FONT2DVERTEX(sx,		sy,		0.1f, color, tx1, ty1);
*/
		wNumTriangles+=2;

	_pVB->Unlock();
	_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles );

	return true;
}

// 清除對象
void iFont::
/*-------------------------------------------------------------
void
-------------------------------------------------------------*/
Release()
{
	DeleteObject( _hFont );
	DeleteObject( _hBmp );
	DeleteDC( _hDc );
 	
 	SAFE_RELEASE(_pVB);
	SAFE_RELEASE(_pTexture);

	_vBuf.clear();
}


namespace LGB
{
    //=============================================================================
    //
    //
    //=============================================================================
    VMCD3D::VMCD3D()
	{
	    pD3d = NULL;				// LPDIRECT3DDEVICE8       pD3dDevice;					    	// 
	    pD3dDevice = NULL;		   	// 
		pSprite = NULL;		        // Sprite object
		ZeroMemory( &D3dpp, sizeof(D3dpp) );
	    ZeroMemory( &D3dcaps, sizeof(D3dcaps) );
	}
	
	//=============================================================================
    //
    //
    //=============================================================================
	VMCD3D::~VMCD3D()
	{
	    if( pD3dDevice )
		    pD3dDevice->SetTexture(0, NULL);
	    SAFE_RELEASE(pD3dDevice);
	    SAFE_RELEASE(pD3d);
		pSprite?pSprite->Release():0;
	}


    //=============================================================================
    //
    //
    //=============================================================================
    bool VMCD3D::Create( HWND hWnd )
	{
		if( NULL == ( pD3d = Direct3DCreate8( D3D_SDK_VERSION ) ) )
		{
			OUTTRACE("Direct3DCreate() failed. \n");
			return false;
		}

		m_hWidget = hWnd;
	
		// 斐膘馺扢掘
		JudgeVideoMask();
		//D3DDISPLAYMODE d3ddm;
		//ZeroMemory( &d3ddm, sizeof(d3ddm) );
		//pD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );
		//DxTest( pD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) );
		
	#ifdef _DEBUG // 敦諳
		D3dpp.Windowed = TRUE;
		D3dpp.BackBufferWidth  = 0;
		D3dpp.BackBufferHeight = 0;
		//D3dpp.BackBufferFormat = d3ddm.Format;
	#else         // 
		D3dpp.Windowed = FALSE;
		D3dpp.BackBufferWidth  = 800;
		D3dpp.BackBufferHeight = 600;
		//D3dpp.BackBufferFormat = D3DFMT_R5G6B5;
		D3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	#endif
		
		D3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		D3dpp.hDeviceWindow    = hWnd;
		D3dpp.BackBufferCount  = 1;
		D3dpp.EnableAutoDepthStencil = TRUE;
		D3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		// 斐膘D3DDevice
		if( FAILED( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
										  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										  &D3dpp, &pD3dDevice ) ) )
		{
			// 彆祥盓厥565耀宒, 憩郭彸555耀宒
			D3dpp.BackBufferFormat = D3DFMT_X1R5G5B5;
			pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
											 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											 &D3dpp, &pD3dDevice );
			//DxTest( pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			//								 D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			//								 &D3dpp, &pD3dDevice ) );
		}

		if ( pD3dDevice == NULL )
		{
			OUTTRACE("NULL Device" );
			exit(1);
		}
		if ( pSprite != NULL )
		{
			pSprite->Release();
			pSprite = NULL;
		}
		if ( pSprite == NULL )
    		D3DXCreateSprite( pD3dDevice, &pSprite );
	
		pD3dDevice->GetDeviceCaps( &D3dcaps );

		ClearScreen( D3DCOLOR_ARGB(0,0,0,0), true );

		return true;
	}

    //=============================================================================
    //
    //
    //=============================================================================
	int VMCD3D::JudgeVideoMask( void )
	{
		// 斐膘馺扢掘
		D3DDISPLAYMODE d3ddm;
		ZeroMemory( &d3ddm, sizeof(d3ddm) );
		pD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

		#ifdef _DEBUG // 敦諳
		D3dpp.BackBufferFormat = d3ddm.Format;
		#else         //
		D3dpp.BackBufferFormat = D3DFMT_R5G6B5;
        #endif
		//DxTest( pD3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) );

		PIXELFMT P_M = PIXEL_A8R8G8B8;
		switch( d3ddm.Format )
		{
		case D3DFMT_R5G6B5:
			P_M = PIXEL_R5G6B5; break;
		case D3DFMT_A1R5G5B5:
			P_M = PIXEL_R5G5B5; break;
		case D3DFMT_A8R8G8B8:
			P_M = PIXEL_A8R8G8B8; break;
		case D3DFMT_X8R8G8B8:
			P_M = PIXEL_A8R8G8B8; break;
		}
		Initiate( P_M );
		return P_M;

	}

    //=============================================================================
    //
    //
    //=============================================================================
	bool VMCD3D::SetScreenMode( bool fullscreen, int width, int height, int depth, int backc )
	{
		m_WidgetSize.width = width;
		m_WidgetSize.height = height;
		m_WidgetSize.depth = depth;
		m_fullscreen = fullscreen;

		D3dpp.Windowed = !m_fullscreen;

	    if ( D3dpp.Windowed )	// 敦諳耀宒
		{
		    D3dpp.BackBufferWidth  = 0;
		    D3dpp.BackBufferHeight = 0;
		}
	    else					// 耀宒
		{
		    D3dpp.BackBufferWidth  = width;
		    D3dpp.BackBufferHeight = height;
		    D3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		pD3dDevice->Reset(&D3dpp);

	    ClearScreen( D3DCOLOR_ARGB(0,0,0,0), true );
		return true;
	}


    //=============================================================================
    //
    //
    //=============================================================================
	bool VMCD3D::CreateEX( HWND hWnd, int width, int height, int depth, bool screenflag )
	{
		if( !Create( hWnd ) )
		{
			OUTTRACE("Create() failed. \n");
			return false;
		}

		SetScreenMode( screenflag, width, height, depth ); 
		return true;
	}


    //=============================================================================
    //
    //
    //=============================================================================
	bool VMCD3D::ClearScreen( COLORREF color, bool bZBuffer )    //清除螢幕
	{

		assert( pD3dDevice!=NULL ); // D3D

        //
 	    pD3dDevice->Clear( 0, NULL, 
		                   bZBuffer ? D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER : D3DCLEAR_TARGET,
		                   color, 1.0f, 0L );
		return true;
	}

    //=============================================================================
    //
    //                                //消滅DX物/
    //=============================================================================
	void VMCD3D::Destroy( void )
	{
		return;
	}


    //=============================================================================
    //
    //
    //=============================================================================
	bool VMCD3D::CanvasCreate( CMemCanvas** canvas, int width, int height, int depth )
	{
		return true;
	}


    //=============================================================================
    //
    //
    //=============================================================================
	bool VMCD3D::CanvasDestroy( CMemCanvas  *canvas )
	{
		return false;
	}
/*
struct tImage
{
    Puint   width;              // The width of the image in pixels
    Puint   height;             // The height of the image in pixels
    Puint   depth;

    Pbyte*  pixels;             // The image pixel data
    Pdword* palette;

    Pint    format;             // The channels in the image (3 = RGB : 4 = RGBA)
};

*/

unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
int VMCD3D::AlphaBlit( const DxIcon* Src, LG::Point point, CMemCanvas *Dest)
{
	if ( Src->TexPtr == NULL )
		return -1;
	RECT rc[] = { point.x, point.y , Src->width, Src->height };
	Draw( Src->TexPtr, rc );
	return 0;
}

int VMCD3D::AlphaBlit( const Icon* Src, LG::Point point, CMemCanvas *Dest)
{

	unsigned char *Mem;
		int MemSize;
		// Create New Memory
		MemSize = Src->width * Src->height * 4;
		if ( MemSize == 0 )
			return false;
		MemSize+= 18;
		Mem = new unsigned char[ MemSize ];
//		char buf[256];
//		sprintf( buf, "Memory Size:%d ", MemSize);
//		MessageBox( NULL, buf , " Q____Q " , MB_OK );
//		exit(1);
        Mem[12]	=	(int)Src->width%256;
		Mem[13]	=	(int)Src->width/256;
		Mem[14]	=	(int)Src->height%256;
		Mem[15]	=	(int)Src->height/256;
		Mem[16]	=	(int)Src->depth<<3;
		Mem[17]	=	0;
		memcpy( Mem, uTGAcompare,12 );
		memcpy( Mem+18,Src->pixels,MemSize-18 );


		VAS_TEXTURE newTex = NULL;
		CreateTexture(Mem,MemSize,D3DCOLOR_ARGB(0,0,255,0),&newTex);//Src->width ,Src->height, &newTex ,true);

		if ( newTex != NULL )
		{
			RECT rc[] = {0 , 0, Src->width, Src->height};
			Draw( newTex, rc );
			newTex->Release();
		}
		delete [] Mem;
		return 1;
}

	bool VMCD3D::Draw(LPDIRECT3DTEXTURE8 _Tex, RECT *rc,int Width, int Height, COLORREF color, int FlipFlags, int Rotation)
	{
		#ifdef SAFE_MODE
		
		if ( pDevice == NULL )
			return false;
		
		if ( _Tex == NULL )
			return false;

		if ( rc == NULL )
			return false;

		#endif

		D3DXVECTOR2 scaling(1,1), rcenter(0,0), trans(0,0);
		// 設定來源矩形
		// 目標矩形
		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = rc->right;
		rect.bottom = rc->bottom;

		Width  = Width==0	? rect.right : Width;
		Height = Height==0	? rect.bottom : Height;

		RECT DestRect  = {rc->left, rc->top, rc->left+ Width, rc->top+ Height},
			 SrcRect   = {rect.left, rect.top, rect.left+rect.right, rect.top+rect.bottom},
			*pSrcRect  = &SrcRect,
			*pDestRect = &DestRect;
		
		// 轉換為屏幕坐標
		trans.x = (float) pDestRect->left;
		trans.y = (float) pDestRect->top;
		
		// 如果源矩形和目標矩形大小不一, 則計算縮放比例
		if ( pDestRect && pSrcRect )
		{
			scaling.x = ((float) (pDestRect->right - pDestRect->left)) / 
				((float) (pSrcRect->right - pSrcRect->left));
			scaling.y = ((float) (pDestRect->bottom - pDestRect->top)) / 
				((float) (pSrcRect->bottom - pSrcRect->top));
		}

		// 圖, 支持上下 或 左右
		if ( pDestRect && pSrcRect && FlipFlags)
		{
			if ( FlipFlags == 1 )
			{
				scaling.x = -scaling.x; 
				trans.x += (float) (pDestRect->right - pDestRect->left);
			}
			if ( FlipFlags == 2 )
			{
				scaling.y = -scaling.y;
				trans.y += (float) (pDestRect->bottom - pDestRect->top);
			}
		}

		rcenter.x = (float)Width  / 2;
		rcenter.y = (float)Height / 2;

		// 繪制
		pSprite->Begin();
		pSprite->Draw( _Tex, pSrcRect, &scaling, &rcenter,D3DXToRadian(360-Rotation), &trans, color );
		pSprite->End();

		return true;
	}

	bool VMCD3D::DrawEx(LPDIRECT3DTEXTURE8 _Tex, RECT *rc,int x,int y,int Width, int Height, COLORREF color, int FlipFlags, int Rotation)
	{
		#ifdef SAFE_MODE
		
		if ( pDevice == NULL )
			return false;
		
		if ( _Tex == NULL )
			return false;

		if ( rc == NULL )
			return false;

		#endif

		D3DXVECTOR2 scaling(1,1), rcenter(0,0), trans(0,0);
		// 設定來源矩形
		// 目標矩形
		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = rc->right;
		rect.bottom = rc->bottom;

		Width  = Width==0	? rect.right : Width;
		Height = Height==0	? rect.bottom : Height;

		RECT DestRect  = {x, y,  x + Width, y + Height},
			 SrcRect   = {rect.left, rect.top, rect.left+rect.right, rect.top+rect.bottom},
			*pSrcRect  = &SrcRect,
			*pDestRect = &DestRect;
		
		// 轉換為屏幕坐標
		trans.x = (float) pDestRect->left;
		trans.y = (float) pDestRect->top;
		
		// 如果源矩形和目標矩形大小不一, 則計算縮放比例
		if ( pDestRect && pSrcRect )
		{
			scaling.x = ((float) (pDestRect->right - pDestRect->left)) / 
				((float) (pSrcRect->right - pSrcRect->left));
			scaling.y = ((float) (pDestRect->bottom - pDestRect->top)) / 
				((float) (pSrcRect->bottom - pSrcRect->top));
		}

		// 圖, 支持上下 或 左右
		if ( pDestRect && pSrcRect && FlipFlags)
		{
			if ( FlipFlags == 1 )
			{
				scaling.x = -scaling.x; 
				trans.x += (float) (pDestRect->right - pDestRect->left);
			}
			if ( FlipFlags == 2 )
			{
				scaling.y = -scaling.y;
				trans.y += (float) (pDestRect->bottom - pDestRect->top);
			}
		}

		// 繪制
		pSprite->Begin();
		pSprite->Draw( _Tex, pSrcRect, &scaling, &rcenter, D3DXToRadian(360-Rotation), &trans, color );
		pSprite->End();

		return true;
	}

	//	3D Engine Function
	long VMCD3D::CreateTexture(char *file,DWORD ColorKey, VAS_TEXTURE *pTex)
	{
		if ( pTex == 0 )
			pTex = new VAS_TEXTURE;

		HRESULT hr = D3DXCreateTextureFromFileEx(
			pD3dDevice,
			file,
			0,
			0,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			D3DX_FILTER_NONE,
			D3DX_FILTER_NONE,
			ColorKey,
			NULL,
			NULL,
			pTex);

		if ( hr != D3D_OK )
		{
			switch(hr)
			{
				case D3DERR_NOTAVAILABLE:
					VnMsg("This device does not support the queried technique.","CreateTexture Error");
					break;
				case D3DERR_OUTOFVIDEOMEMORY:
					VnMsg("顯示卡記憶體已經滿了","CreateTexture Error");
					break;
				case D3DXERR_INVALIDDATA:
					VnMsg(file,"CreateTexture Error 材質來源錯誤 ");
					break;
				case D3DERR_INVALIDCALL:
					VnMsg("D3DERR_INVALIDCALL","CreateTexture Error");
					break;
				case E_OUTOFMEMORY:
					VnMsg("記憶體不足無法完成材質建立","CreateTexture Error");
					break;
				default:
					VnMsg("未知問題~~無法建立材質","CreateTexture Error");
					break;
			}
			return -1;
		}
		return 1;
	}

	long VMCD3D::CreateTexture(BYTE *Mem,int length,DWORD ColorKey,VAS_TEXTURE *pTex)
	{
		if ( pTex == 0 )
			pTex = new VAS_TEXTURE;

		HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( 
				pD3dDevice,
				Mem,
				length,
				D3DX_DEFAULT,
				D3DX_DEFAULT,
				1,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,			// 記憶體管理模式
				D3DX_FILTER_NONE,
				D3DX_FILTER_NONE, 
				ColorKey,
				NULL,
				NULL, 
				pTex);	
	
		if ( hr != D3D_OK )
		{
			switch(hr)
			{
				case D3DERR_NOTAVAILABLE:
					VnMsg("This device does not support the queried technique.","CreateTexture Error");
					break;
				case D3DERR_OUTOFVIDEOMEMORY:
					VnMsg("顯示卡記憶體已經滿了","CreateTexture Error");
					break;
				case D3DXERR_INVALIDDATA:
					VnMsg("Form Memory","CreateTexture Error 材質來源錯誤 ");
						break;
				case E_OUTOFMEMORY:
					VnMsg("記憶體不足無法完成材質建立","CreateTexture Error");
					break;
				default:
					VnMsg("無法建立材質","CreateTexture Error");
					break;
			}
			return -1;
		}
		return 1;
}

	long VMCD3D::CreateCostomTexture(int width, int height ,VAS_TEXTURE *Tex, bool D3DManaged)
	{
		if ( Tex == NULL )
			Tex = new VAS_TEXTURE;
		
			HRESULT hr;
			hr = D3DXCreateTexture(pD3dDevice,
			width, height,
			1, 0,
			D3DFMT_X8R8G8B8, D3DManaged?D3DPOOL_MANAGED:D3DPOOL_DEFAULT, Tex );
	
			if ( FAILED( hr ))
			{
				char msg[256];
				sprintf(msg, " Could not create the D3DX texture!  hr=0x%x", hr);
				VnMsg(msg, "Texture Can't Create Error");
				return hr;
			}

			// D3DXCreateTexture can silently change the parameters on us
			D3DSURFACE_DESC ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));

		    if ( FAILED( hr = (*Tex)->GetLevelDesc( 0, &ddsd ) ) ) {
				char msg[256];
				sprintf(msg, "Could not get level Description of D3DX texture! hr = 0x%x", hr);
				VnMsg(msg, "Texture Can't Create Error");
		        return hr;
			}

		    // Save format info
		    d_TextureFormat = ddsd.Format;
		    if (d_TextureFormat != D3DFMT_X8R8G8B8 &&
			       d_TextureFormat != D3DFMT_A1R5G5B5) {
				char msg[256];
				sprintf(msg, "Texture is format we can't handle! Format = 0x%x", (*Tex));
				VnMsg(msg, "Texture Can't Create Error");
				return -1;
			}

		return 1;
	}


};//namespace LGB