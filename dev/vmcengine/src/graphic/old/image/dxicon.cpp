#include "DxIcon.h"
unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header

 
void DxIcon::
//
//
DumpPixelsToTexture(D3DDEVICE Device )
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
				D3DCOLOR_ARGB(255,0,255,0),		// 去背色.. ( 純綠 )
				NULL,
				NULL, 
				&TexPtr);	
	
	delete [] Mem;
	return ;
}
