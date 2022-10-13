#ifndef __TGA_H__
#define __TGA_H__
#include <windows.h>
#include <stdio.h>									// Standard I/O header

#ifndef uint32
#define uint32 uint_32
#endif
#ifndef int32
#define int32 int_32
#endif
#ifndef int16
#define int16 int_16
#endif
#ifndef uint16
#define uint16 uint_16
#endif
#ifndef ubyte
#define ubyte unsigned char
#endif
#ifndef uchar
#define uchar char
#endif

#define TGA_TRUECOLOR_32      (4)
#define TGA_TRUECOLOR_24      (3)
#define ARRAY_DELETE(a) { if (a) { delete [] a; a=NULL; } }
typedef struct
{
	BYTE	header[6];								// First 6 Useful Bytes From The Header
	int		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
	int		imageSize;								// Used To Store The Image Size When Setting Aside Ram
	int		temp;									// Temporary Variable
	int		type;
	int		Height;									//Height of Image
	int		Width;									//Width ofImage
	int		Bpp;									// Bits Per Pixel
	int		b_size;
} TGA;


typedef struct
{
int 	        right;
int 	        bottom;
int		left;
int		top;
int		CenterX;
int		CenterY;
int		nouse1;
int		nouse2;
}CenterData;//檔頭資料,總共32 Byts

#define BLOCK_SIZE sizeof(CenterData)
// #define BLOCK_SIZE 0
class TGA_OBJ
{
	public:
	// 資料成員		Data Members
	TGA_OBJ()
	{
		data = NULL;
		color_channel = NULL;
		alpha_buf = NULL;
		alpha_channel = NULL;
		color_buf = NULL;
		alpha_unit = 0;
		color_unit = 0;
		run_step = 0;
	};
	TGA info;
	BYTE *data;
	BYTE *color_channel;
	BYTE *alpha_channel;
	BYTE *alpha_buf;
	BYTE *color_buf;
	unsigned char block_data[BLOCK_SIZE];
	int alpha_unit, color_unit;
	int run_step;
	// 函式成員		Function Members
	bool alpha_to_bmp(char *file = NULL );									// 將 alpha 轉成 BMP 格式
	bool color_to_bmp(char *file = NULL );									// 將 color 轉成 BMP 格式

// 記憶體讀取
//==========================================================================================
	bool load_tga_FormMem(unsigned char *buf, DWORD length);				// 讀取記憶體指標
	// 未壓縮模式
	int mem_uncompressed(unsigned char *buf, DWORD length);
	// 壓縮模式
	int mem_compressed(unsigned char *buf, DWORD length);		// Load COMPRESSED TGAs
//==========================================================================================
// 檔案直讀
//==========================================================================================
	bool load_tga(char *file);												// 讀取檔案
	// 未壓縮模式
	int uncompressed(char *file, FILE *res);
	// 壓縮模式
	int compressed(char *file, FILE *res);
//==========================================================================================
	// 存檔
        //Q_Q;
	bool save_tga(char filename[], RECT *rc,bool compresse = true);  // 未壓縮 32bit TGA 圖檔
	int tga_write_rle( const char * file, int width, int height, unsigned char * dat, unsigned int format );
        void GetRect( BYTE *data, RECT oldRc, char nearcolor, RECT &rc  );
	~TGA_OBJ()
	{
		ARRAY_DELETE(data);
		ARRAY_DELETE(alpha_channel);
		ARRAY_DELETE(color_channel);
		ARRAY_DELETE(alpha_buf);
		ARRAY_DELETE(color_buf);
		ZeroMemory(&info,sizeof(TGA));
	};
};
bool save_to_file(int format, bool compare);	// 儲存檔案
#endif
