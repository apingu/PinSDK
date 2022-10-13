//---------------------------------------------------------------------------

#include "tga.h"

#define int_16    __int16
#define int_32    __int32
#define int_64    __int64
#define float_32  float
#define float_64  double
#define long_32   LONG32
// 0 width              寬尺寸
// 1 height             長尺寸
// 2 type byte color    顏色資訊
// 3 R.G.B or R.G.B.A   是否為三原色與有無 alpha channel 傳回值為 3 時為 RGB 傳回 4 是 RGBA
// 4 ImageSize          資料大小
//---------------------------------------------------------------------------

// Debug 輸出圖
// #define SHOW_FILE 1


// 未壓縮的 TGA 檔案..   header!
BYTE uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
// 壓縮過後的 TGA 檔案 Haader!
BYTE cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header

static int16 ttohs( int16 val );
static int16 htots( int16 val );
static int32 ttohl( int32 val );
static int32 htotl( int32 val );
void TGA_OBJ::GetRect( BYTE *AlphaData ,RECT oldRc, char nearcolor, RECT &rc   )
{


        for(int x=0;x<oldRc.right*oldRc.bottom;x++)
        {
                if ( AlphaData[x] < nearcolor )
                {
                        rc.top = x;
                        break;
                }
        }
        rc.top = rc.top / oldRc.bottom;
        for(int x=oldRc.right*oldRc.bottom;x>0;x--)
        {
                if ( AlphaData[x] < nearcolor )
                {
                        rc.bottom = x;
                        break;
                }
        }
        rc.bottom = rc.bottom / oldRc.bottom;

        for(int x=0;x<oldRc.right;x++)
        {
                for(int y=0;y<oldRc.bottom;y++)
                {
                        if ( AlphaData[y*oldRc.right + x]  < nearcolor )
                        {
                                rc.left = y;
                                break;
                        }
                }
        }

        for(int x=0;x<oldRc.right;x++)
        {
                for(int y=oldRc.bottom;y>0;y--)
                {
                        if ( AlphaData[y*oldRc.right + x]  < nearcolor )
                        {
                                rc.right = y;
                                break;
                        }
                }
        }

        return ;
}
bool TGA_OBJ::save_tga(char filename[], RECT *rc,bool compresse)
{

	int img_size;
	int left,top;
	int pre,data_run;
	unsigned char header[18];
	BYTE rgba[4];
	BYTE *data_buf;


	img_size = ( rc->right * rc->bottom * 4 );

	top = rc->top;
	left = rc->left;

	data_buf = new BYTE[img_size];

	memcpy(header,uTGAcompare,12);
        header[0] = BLOCK_SIZE;
        header[12] = (int)rc->right%256;
	header[13] = (int)rc->right/256;
	header[14] = (int)rc->bottom%256;
	header[15] = (int)rc->bottom/256;
	header[16] = 0x20;
	header[17] = 0;

	pre     = 0;
	data_run= 0;

	for(int x=info.Height;x>=1;x--)
	{
		for(int y=1;y<=info.Width;y++)
		{
			if ( x > top && x <= (top + rc->bottom) && y > left && y <= (left + rc->right) )
			{
				if ( compresse == false )
				{
					for(int z=0;z<4;z++)
					{
						rgba[z] = data[pre];
						pre++;
					}
				
					data_buf[data_run] = rgba[2];
							data_run++;
					data_buf[data_run] = rgba[1];
							data_run++;
					data_buf[data_run] = rgba[0];
							data_run++;
					data_buf[data_run] = rgba[3];
							data_run++;
				}
				else
				{
					for(int zz=0;zz<4;zz++)
					{
						data_buf[data_run] = data[pre];
						pre++;
						data_run++;
					}
				}
			}
			else
			pre+=4;
		}
	}
	if ( compresse == true )
	{
		if ( tga_write_rle(filename,rc->right,rc->bottom,data_buf,TGA_TRUECOLOR_32) == 0 )
                return false;
        }
	else
	{
		FILE *wfp;
		wfp=fopen(filename, "w+b");
		fwrite(header,18,1,wfp);
                fwrite( &block_data, BLOCK_SIZE, 1, wfp );
		fwrite(data_buf,img_size,1,wfp);
		fclose(wfp);
		delete [] data_buf;

	}
        delete [] data_buf;

	return true;
}

bool TGA_OBJ::color_to_bmp(char *file)
{
	int num;
	int data_num;
	int pitch_pix;

	if ( color_unit ==0 )
		return false;

	BITMAPFILEHEADER fhead;
	BITMAPINFOHEADER ihead;

	pitch_pix = info.Width%4;

	num = 54;
	num += info.Height* (info.Width + pitch_pix ) * 3;

	color_buf = new BYTE[num];
	
		// file 檔頭 
		fhead.bfOffBits = 0x36;
		fhead.bfReserved1 = 0;
		fhead.bfReserved2 = 0;
		fhead.bfType = 0x4d42;
		fhead.bfSize = (info.Width * info.Height * 3) + 0x36;
		// 寫入檔案檔頭
		memcpy(color_buf, &fhead, sizeof(BITMAPFILEHEADER) );

		// 影像檔頭
		ihead.biSize = sizeof(BITMAPINFOHEADER);
		ihead.biHeight = info.Height;
		ihead.biWidth = info.Width;
		ihead.biPlanes = 1;
		ihead.biBitCount = 24;
		ihead.biCompression = BI_RGB;
		ihead.biSizeImage = info.Width * info.Height * 3;
	
		ihead.biXPelsPerMeter = 0;
		ihead.biYPelsPerMeter = 0;
		ihead.biClrUsed = 0;
		ihead.biClrImportant = 0;
		// 寫入影像檔頭
		memcpy(color_buf+sizeof(BITMAPFILEHEADER), &ihead, sizeof(BITMAPINFOHEADER) );
	
		data_num = 54;

		unsigned char* pWriteMem = color_buf+54 ;
		unsigned char* pReadMem = color_channel ;

		int writePitch = ihead.biWidth*3 ;
		if (writePitch%4 != 0)
			writePitch += 4-writePitch%4 ;
		//writePitch += 4-writePitch%4 ;


		for (int i = 0; i<ihead.biHeight; i++)
		{
			memset (pWriteMem, 0, writePitch) ;
			memcpy (pWriteMem, pReadMem, ihead.biWidth*3) ;
			pReadMem += ihead.biWidth*3 ;
			pWriteMem += writePitch ;
		}
		/*
		for (int i = 0; i<ihead.biHeight; i++)
		{
			memcpy (pWriteMem, pReadMem, ihead.biWidth*3) ;
			pReadMem += ihead.biWidth*3 ;
			pWriteMem += writePitch ;
		}
		*/
		/*
		for(int x=0;x<color_unit;x++)
		{
			if ( x % (info.Width*3) == (info.Width*3-1) )
			{
				for(int z=0;z<pitch_pix*3;z++)
				{
					color_buf[data_num] = 255;
					data_num++;
				}
			}

			color_buf[data_num] = color_channel[x];
			data_num++;
		}
		*/
		
		if ( file != NULL )
		{
			FILE *fp;
			fp=fopen(file, "wb");
			fwrite(color_buf, num, 1, fp);
			fclose(fp);
		}


#ifdef SHOW_FILE
		//輸出測試 BMP
		FILE *fp;
		fp=fopen("test color.bmp", "wb");
		fwrite(color_buf, num, 1, fp);
		fclose(fp);
#endif

		return true;
}


bool TGA_OBJ::alpha_to_bmp(char *file)
{
	
	if ( alpha_unit ==0 )
		return false;

	int num;
	int data_num;
	int pitch_pix;

	BITMAPFILEHEADER fhead;
	BITMAPINFOHEADER ihead;


	pitch_pix = info.Width%4;

	num = 54;
	num += info.Height* (info.Width + pitch_pix ) * 3;

	alpha_buf = new BYTE[num];	

	
		// file 檔頭 
		fhead.bfOffBits = 0x36;
		fhead.bfReserved1 = 0;
		fhead.bfReserved2 = 0;
		fhead.bfType = 0x4d42;
		fhead.bfSize = info.Width * info.Height * 3 + 0x36;
		// 寫入檔案檔頭
		memcpy(alpha_buf, &fhead, sizeof(BITMAPFILEHEADER) );

		// 影像檔頭
		ihead.biSize = sizeof(BITMAPINFOHEADER);
		ihead.biHeight = info.Height;
		ihead.biWidth = info.Width;
		ihead.biPlanes = 1;
		ihead.biBitCount = 24;
		ihead.biCompression = BI_RGB;
		ihead.biSizeImage = info.Width * info.Height * 3;
	
		ihead.biXPelsPerMeter = 0;
		ihead.biYPelsPerMeter = 0;
		ihead.biClrUsed = 0;
		ihead.biClrImportant = 0;

		// 寫入影像檔頭
		memcpy(alpha_buf+sizeof(BITMAPFILEHEADER), &ihead, sizeof(BITMAPINFOHEADER) );
	
		data_num = 54;
		int run_buf=0;

		unsigned char *alpha_channel_buf = new unsigned char[info.Width*info.Height*3];
		
		for(int x=0;x<alpha_unit;x++)
		for(int z=0;z<3;z++)
		{
				alpha_channel_buf[run_buf] = alpha_channel[x];
				run_buf++;
		}

		unsigned char* pWriteMem = alpha_buf+54 ;
		unsigned char* pReadMem = alpha_channel_buf ;

		int writePitch = ihead.biWidth*3 ;
		if (writePitch%4 != 0)
			writePitch += 4-writePitch%4 ;
		//writePitch += 4-writePitch%4 ;

		for (int i = 0; i<ihead.biHeight; i++)
		{
			memset (pWriteMem, 0, writePitch) ;
			memcpy (pWriteMem, pReadMem, ihead.biWidth*3) ;
			pReadMem += ihead.biWidth*3 ;
			pWriteMem += writePitch ;
		}

/*
		for(int x=0;x<alpha_unit;x++)
		{
			alpha_buf[data_num] = alpha_channel[x];
			data_num++;
			alpha_buf[data_num] = alpha_channel[x];
			data_num++;
			alpha_buf[data_num] = alpha_channel[x];
			data_num++;
		}
*/

		//輸出測試 BMP

		if ( file != NULL )
		{
			FILE *fp;
			fp=fopen(file, "wb");
			fwrite(alpha_buf, num, 1, fp);
			fclose(fp);
		}

#ifdef SHOW_FILE
		FILE *fp;
		fp=fopen("test Alpha.bmp", "wb");
		fwrite(alpha_buf, num, 1, fp);
		fclose(fp);
#endif
		delete [] alpha_channel_buf;
	return true;
}

// =========================================================================================================
//												記憶體讀取介面
// =========================================================================================================

int TGA_OBJ::mem_uncompressed(unsigned char *buf, DWORD length)	// Load an uncompressed TGA (note, much of this code is based on NeHe's
{																			// TGA Loading code nehe.gamedev.net)
        byte tga_info[6];
        int PerPixel;

	memcpy(tga_info, buf+run_step,6);							// Read TGA header
	run_step += 6;

	if ( info.b_size != -1 )
	{
/*
		char*buf = new char[255];
		sprintf(buf,"%d",info.b_size );
		MessageBox(NULL,buf,"block_data size",MB_OK); 
		delete buf;
*/
		memcpy(block_data, buf+run_step,info.b_size);
		run_step += info.b_size;
	}



	info.Width = tga_info[1] * 256 + tga_info[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	info.Height = tga_info[3] * 256 + tga_info[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	info.Bpp	= tga_info[4];										// Determine the bits per pixel

	if( ( info.Width <= 0) || ( info.Width <= 0) || (( info.Bpp != 24) && ( info.Bpp !=32)))
	{
		MessageBox(NULL, "讀取錯誤的原因為顏色資訊不是 24 或 32 與長寬可能為 0", "TGA DLL", MB_OK);	// Display Error
		return 0;														// Return failed
	}

	if( info.Bpp == 24)													//If the BPP of the image is 24...
	{
		info.bytesPerPixel	= 3;											// Set Image type to GL_RGB
	}
	else																	// Else if its 32 BPP
	{
        info.bytesPerPixel = 4;											// Set image type to GL_RGBA
	}

	PerPixel = ( info.Bpp / 8 );									// Compute the number of BYTES per pixel
	info.imageSize	 = (PerPixel * info.Width * info.Height );
    data = new BYTE[info.imageSize];					// Allocate that much memory

	if( data == NULL)											// If no space was allocated
	{
		MessageBox(NULL, "無法配置記憶體給 Tga data 使用", "TGA DLL", MB_OK);	// Display Error
		return 0;														// Return failed
	}


	if( memcpy( data, buf+run_step , info.imageSize ) == NULL )	// Attempt to read image data
	{
		MessageBox(NULL, "無法讀取出圖形資料", "TGA DLL", MB_OK);		// Display Error
		if( data != NULL)										// If imagedata has data in it
		{
			delete [] data;										// Delete data from memory
		}
		return 0;														// Return failed
	}

	// Byte Swapping Optimized By Steve Thomas
	for(int cswap = 0; cswap < info.imageSize; cswap += PerPixel)
		data[cswap] ^= data[cswap+2] ^= data[cswap] ^= data[cswap+2];
	
	alpha_unit = 0;
	color_unit = 0;

	return 1;															// Return success
}


int TGA_OBJ::mem_compressed(unsigned char *buf, DWORD length)		// Load COMPRESSED TGAs
{
        byte tga_info[6];
		int PerPixel;

		memcpy(tga_info, buf+run_step,6);							// Read TGA header
		run_step += 6;

		if ( info.b_size != -1 )
		{
/*
		char*buf = new char[255];
		sprintf(buf,"%d",info.b_size );
		MessageBox(NULL,buf,"block_data size",MB_OK); 
		delete buf;
*/
			memcpy(block_data, buf+run_step,info.b_size);
			run_step += info.b_size;
		}

	info.Width = tga_info[1] * 256 + tga_info[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	info.Height = tga_info[3] * 256 + tga_info[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	info.Bpp= tga_info[4];										// Determine the bits per pixel

	if( (info.Width <= 0) || (info.Height <= 0) || (( info.Bpp != 24) && ( info.Bpp !=32)))
	{
		MessageBox(NULL, "讀取錯誤的原因為顏色資訊不是 24 或 32 與長寬可能為 0", "TGA DLL", MB_OK);	// Display Error
		return 0;														// Return failed
	}

	PerPixel	        = (info.Bpp / 8);									// Compute BYTES per pixel
	info.imageSize =		(PerPixel * info.Width * info.Height );

	data     = new BYTE[info.imageSize];					// Allocate that much memory
	color_channel	= new BYTE[ ((info.Width * info.Height)*3 )];
	color_unit = 0;

	if ( info.Bpp == 32 )
	{
		alpha_unit = 0;
		alpha_channel = new BYTE[info.Width*info.Height];		// 配置 Alpha_channel Byte Buffer
		if ( alpha_channel == NULL )
			return 0;
	}

	if( data == NULL)											// If no space was allocated
	{
		MessageBox(NULL, "無法配置記憶體給 Tga data 使用", "TGA DLL", MB_OK);	// Display Error
		return 0;														// Return failed
	}

	int pixelcount	        = info.Height * info.Width;							// Nuber of pixels in the image
	int currentpixel		= 0;												// Current pixel being read
	int currentbyte	        = 0;												// Current byte
	byte * colorbuffer = (byte *)malloc(PerPixel);

	do
	{
		byte chunkheader = 0;											// Storage for "chunk" header

		if( memcpy(&chunkheader, buf+run_step,sizeof(byte) ) == NULL )				// Read in the 1 byte header
		{
			MessageBox(NULL, "無法讀取 RLE 檔頭資訊", "ERROR", MB_OK);	// Display Error
			if( data != NULL)									// If there is stored image data
			{
				delete [] data;									// Delete image data
			}
			return 0;													// Return failed
		}

		run_step+=sizeof(byte);
		if( chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;													// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++ )       // Read RAW color values
			{
				if( memcpy(colorbuffer,buf+run_step, PerPixel ) == NULL )    // Try to read 1 pixel
				{
					MessageBox(NULL, "無法正確讀取圖型資料", "ERROR", MB_OK);		// IF we cant, display an error

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if( data != NULL )										// See if there is stored Image data
					{
						delete [] data;										// If so, delete it too
					}

					return 0;														// Return failed
				}
				run_step+=PerPixel;
																		// write to memory
				data[currentbyte	] = colorbuffer[2];				    // Flip R and B vcolor values around in the process
				data[currentbyte + 1	] = colorbuffer[1];
				data[currentbyte + 2	] = colorbuffer[0];
				
				color_channel[color_unit] = colorbuffer[0];				    // Flip R and B vcolor values around in the process
				color_unit++;
				color_channel[color_unit] = colorbuffer[1];
				color_unit++;
				color_channel[color_unit] = colorbuffer[2];
				color_unit++;

				if( PerPixel == 4)												// if its a 32 bpp image
				{
					data[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					alpha_channel[alpha_unit] = colorbuffer[3];
					alpha_unit++;
				}

				currentbyte += PerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if( currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					MessageBox(NULL, "讀取過多的像素", "TGA DLL", NULL);			// if there is too many... Display an error!


					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(data != NULL)										// If there is Image data
					{
						delete [] data;										// delete it
					}

					return 0;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if( memcpy(colorbuffer, buf+run_step, PerPixel) == NULL )		// Attempt to read following color values
			{
				MessageBox(NULL, "無法的取來源檔", "TGA DLL", MB_OK);			// If attempt fails.. Display error (again)


				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(data != NULL)											// If thereis image data
				{
					delete [] data;											// delete it
				}

				return false;															// return failed
			}
			run_step+= PerPixel;

			for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated
			{																			// by the header
				data[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				data[currentbyte + 1	] = colorbuffer[1];
				data[currentbyte + 2	] = colorbuffer[0];

				color_channel[color_unit] = colorbuffer[0];				    // Flip R and B vcolor values around in the process
				color_unit++;
				color_channel[color_unit] = colorbuffer[1];
				color_unit++;
				color_channel[color_unit] = colorbuffer[2];
				color_unit++;

				if( PerPixel == 4)												// If TGA images is 32 bpp
				{
					data[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					alpha_channel[alpha_unit] = colorbuffer[3];
					alpha_unit++;
				}

				currentbyte += PerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if( currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					MessageBox(NULL, "讀取過多的像素", "TGA DLL", NULL);			// if there is too many... Display an error!

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if( data != NULL)										// If there is Image data
					{
						delete [] data;										// delete it
					}

					return 0;														// Return failed
				}
			}
		}
	}

	while ( currentpixel < pixelcount);													// Loop while there are still pixels left




//	fclose(res);																		// Close the file
//	 char buf[255];
//	 sprintf(buf,"%d",color_unit);
//	 MessageBox(NULL,buf,"Alpha_value",MB_OK);
	return 1;																		// return success
}
//

bool TGA_OBJ::load_tga_FormMem(unsigned char *buf, DWORD length)
{
	ubyte tga_header[12];
	
	if ( buf == NULL )
	{
		MessageBox(NULL,"Load Tga Form Memory Buf Is NULL","error",MB_OK);
		return false;
	}
	
	memcpy(tga_header,buf+run_step,12);
	run_step += 12;
	
        // Memcmp 用來比對記憶體位置!!
	if(  tga_header[2] == 2 )		 // See if header matches the predefined header of
	{																		// an Uncompressed TGA image
				if ( tga_header[0] != 0 )
					info.b_size = tga_header[0];
				else
					info.b_size = -1;


				//  MessageBox(NULL,"uncompared","TGA DLL",MB_OK);
                if ( mem_uncompressed(buf, length) == 0 )
                return 0;
	}
	else if(  tga_header[2] == 10 )		// See if header matches the predefined header of
	{																		// an RLE compressed TGA image

		
				if ( tga_header[0] != 0 )
					info.b_size = tga_header[0];
				else
					info.b_size = -1;

				//  MessageBox(NULL,"compared","TGA DLL",MB_OK);
               if ( mem_compressed(buf, length) == 0 )
               return 0;
	}
	else																	// If header matches neither type
	{

		MessageBox(NULL, "無法判別的檔案格式", "TGA DLL", MB_OK);	// Display an error
		return 0;																// Exit function
	}
        return 1;			


}


// =========================================================================================================
//												檔案讀取介面
// =========================================================================================================

bool TGA_OBJ::load_tga(char *file)
{
        FILE *res;												// File pointer to texture file
        BYTE tga_header[12];
	
       	res = fopen(file, "rb");

	if( res == NULL )											// If it didn't open....
	{
		MessageBox(NULL, "無法讀取原始 TGA 檔", "TGA DLL", MB_OK);	// Display an error message
		return 0;														// Exit function
	}

	if(fread(&tga_header, sizeof(tga_header), 1, res) == 0)					// Attempt to read 12 byte header from file
	{
		MessageBox(NULL, "無法讀取 TGA 檔頭資料, 請檢查讀入的檔案是否為 TGA 檔", "TGA DLL", MB_OK);		// If it fails, display an error message
		if(res != NULL)													// Check to seeiffile is still open
		{
			fclose(res);													// If it is, close it
		}

		return 0;														// Exit function
	}

        // Memcmp 用來比對記憶體位置!!
	if(  tga_header[2] == 2 )		 // See if header matches the predefined header of
	{																		// an Uncompressed TGA image
				if ( tga_header[0] != 0 )
					info.b_size = tga_header[0];
				else
					info.b_size = -1;


				//  MessageBox(NULL,"uncompared","TGA DLL",MB_OK);
                if ( uncompressed(file, res) == 0 )
                return 0;
	}
	else if(  tga_header[2] == 10 )		// See if header matches the predefined header of
	{																		// an RLE compressed TGA image

		
				if ( tga_header[0] != 0 )
					info.b_size = tga_header[0];
				else
					info.b_size = -1;

				//  MessageBox(NULL,"compared","TGA DLL",MB_OK);
               if ( compressed(file, res) == 0 )
               return 0;
	}
	else																	// If header matches neither type
	{

		MessageBox(NULL, "無法判別的檔案格式", "TGA DLL", MB_OK);	// Display an error
		fclose(res);
		return 0;																// Exit function
	}
        return 1;															// All went well, continue on
}

int TGA_OBJ::uncompressed(char * file, FILE * res)	// Load an uncompressed TGA (note, much of this code is based on NeHe's
{																			// TGA Loading code nehe.gamedev.net)
        byte tga_info[6];
        int PerPixel;

	if( fread(tga_info, sizeof(tga_info), 1, res) == 0)					// Read TGA header
	{
		MessageBox(NULL, "無法讀取正確的檔頭資料", "TGA DLL", MB_OK);		// Display error
		if( res != NULL )													// if file is still open
		{
			fclose(res);													// Close it
		}
		return 0;														// Return failular
	}


	if ( info.b_size != -1 )
	{
/*
		char*buf = new char[255];
		sprintf(buf,"%d",info.b_size );
		MessageBox(NULL,buf,"block_data size",MB_OK); 
		delete buf;
*/
		if( fread(block_data, info.b_size, 1, res) == 0)					// Read TGA header
		{
			MessageBox(NULL, "無法讀取額外資訊", "Tga Funciton", MB_OK);		// Display error
			if( res != NULL )													// if file is still open
			{
				fclose(res);													// Close it
			}
			return 0;	
		}
	}



	info.Width = tga_info[1] * 256 + tga_info[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	info.Height = tga_info[3] * 256 + tga_info[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	info.Bpp	= tga_info[4];										// Determine the bits per pixel

	if( ( info.Width <= 0) || ( info.Width <= 0) || (( info.Bpp != 24) && ( info.Bpp !=32)))
	{
		MessageBox(NULL, "讀取錯誤的原因為顏色資訊不是 24 或 32 與長寬可能為 0", "TGA DLL", MB_OK);	// Display Error
		if(res != NULL)													// Check if file is still open
		{
			fclose(res);													// If so, close it
		}
		return 0;														// Return failed
	}

	if( info.Bpp == 24)													//If the BPP of the image is 24...
	{
		info.bytesPerPixel	= 3;											// Set Image type to GL_RGB
	}
	else																	// Else if its 32 BPP
	{
        info.bytesPerPixel = 4;											// Set image type to GL_RGBA
	}

	PerPixel = ( info.Bpp / 8 );									// Compute the number of BYTES per pixel
	info.imageSize	 = (PerPixel * info.Width * info.Height );
        data = new BYTE[info.imageSize];					// Allocate that much memory

	if( data == NULL)											// If no space was allocated
	{
		MessageBox(NULL, "無法配置記憶體給 Tga data 使用", "TGA DLL", MB_OK);	// Display Error
		fclose(res);														// Close the file
		return 0;														// Return failed
	}


	if( fread( data, 1, info.imageSize , res) != info.imageSize )	// Attempt to read image data
	{
		MessageBox(NULL, "無法讀取出圖形資料", "TGA DLL", MB_OK);		// Display Error
		if( data != NULL)										// If imagedata has data in it
		{
			delete [] data;										// Delete data from memory
		}
		fclose(res);														// Close file
		return 0;														// Return failed
	}
        char buf[256];
        char count=0;
	alpha_unit = 0;
	color_unit = 0;
        alpha_channel = new BYTE[info.Width*info.Height];
        color_channel = new BYTE[info.Width*info.Height*info.bytesPerPixel];

        //sprintf( buf, "color:%d\nalpha:%d",color_unit,alpha_unit);
        //MessageBox(NULL,buf,"q__q",MB_OK);

        for(int x=0;x<info.imageSize;x++)
        {
                count++;
                if ( count < 4 )
                {
                        color_channel[color_unit] = data[x];
                        color_unit++;

                        if ( count == 4 )
                        count = 0;
                }
                else
                {
                        alpha_channel[alpha_unit] = data[x];
                        alpha_unit++;
                        count=0;
                }
        }
        //sprintf( buf, "color:%d\nalpha:%d",color_unit,alpha_unit);
        //MessageBox(NULL,buf,"q__q",MB_OK);

	// Byte Swapping Optimized By Steve Thomas
	for(int cswap = 0; cswap < info.imageSize; cswap += PerPixel)
		data[cswap] ^= data[cswap+2] ^= data[cswap] ^= data[cswap+2];

        fclose(res);															// Close file
	return 1;															// Return success
}

int TGA_OBJ::compressed(char *file, FILE *res)		// Load COMPRESSED TGAs
{
        byte tga_info[6];
		int PerPixel;

	if( fread(tga_info, sizeof(tga_info), 1, res) == 0)					// Attempt to read header
	{
		MessageBox(NULL, "無法讀取正確的檔頭資料", "TGA DLL", MB_OK);		// Display error
		if(res != NULL)													// If file is open
		{
			fclose(res);													// Close it
		}
		return false;														// Return failed
	}


	if ( info.b_size != -1 )
	{
		/*
		char*buf = new char[255];
		sprintf(buf,"%d",sizeof(block_data) );
		MessageBox(NULL,buf,"block_data size",MB_OK); 
		delete buf;
		*/
		if( fread(block_data, info.b_size, 1, res) == 0)					// Read TGA header
		{
			MessageBox(NULL, "無法讀取額外資訊", "Tga Funciton", MB_OK);		// Display error
			if( res != NULL )													// if file is still open
			{
				fclose(res);													// Close it
			}
			return 0;	
		}
	}


	info.Width = tga_info[1] * 256 + tga_info[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	info.Height = tga_info[3] * 256 + tga_info[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	info.Bpp= tga_info[4];										// Determine the bits per pixel

	if( (info.Width <= 0) || (info.Height <= 0) || (( info.Bpp != 24) && ( info.Bpp !=32)))
	{
		MessageBox(NULL, "讀取錯誤的原因為顏色資訊不是 24 或 32 與長寬可能為 0", "TGA DLL", MB_OK);	// Display Error
		if(res != NULL)													// Check if file is still open
		{
			fclose(res);													// If so, close it
		}
		return 0;														// Return failed
	}

	PerPixel	        = (info.Bpp / 8);									// Compute BYTES per pixel
	info.imageSize =		(PerPixel * info.Width * info.Height );
	data     = new BYTE[info.imageSize];					// Allocate that much memory
	color_channel	= new BYTE[ ((info.Width * info.Height)*3 )];
	color_unit = 0;

	if ( info.Bpp == 32 )
	{
		alpha_unit = 0;
		alpha_channel = new BYTE[info.Width*info.Height];		// 配置 Alpha_channel Byte Buffer
		if ( alpha_channel == NULL )
			return 0;
	}

	if( data == NULL)											// If no space was allocated
	{
		MessageBox(NULL, "無法配置記憶體給 Tga data 使用", "TGA DLL", MB_OK);	// Display Error
		fclose(res);														// Close the file
		return 0;														// Return failed
	}

	int pixelcount	        = info.Height * info.Width;							// Nuber of pixels in the image
	int currentpixel		= 0;												// Current pixel being read
	int currentbyte	        = 0;												// Current byte
	unsigned char * colorbuffer = (unsigned char *)malloc(PerPixel);

	do
	{
		byte chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(unsigned char), 1, res) == 0)				// Read in the 1 byte header
		{
			MessageBox(NULL, "無法讀取 RLE 檔頭資訊", "ERROR", MB_OK);	// Display Error
			if( res != NULL )												// If file is open
			{
				fclose(res);												// Close file
			}
			if( data != NULL)									// If there is stored image data
			{
				delete [] data;									// Delete image data
			}
			return 0;													// Return failed
		}

		if( chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;													// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++ )       // Read RAW color values
			{
				if(fread(colorbuffer, 1, PerPixel, res) != PerPixel)    // Try to read 1 pixel
				{
                                         MessageBox(NULL, "無法正確讀取圖型資料", "ERROR", MB_OK);		// IF we cant, display an error

					if(res != NULL)													// See if file is open
					{
						fclose(res);													// If so, close file
					}

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if( data != NULL )										// See if there is stored Image data
					{
						delete [] data;
                                                data = NULL;									// If so, delete it too
					}
					return 0;														// Return failed
				}
																						// write to memory
				data[currentbyte	] = colorbuffer[2];				    // Flip R and B vcolor values around in the process
				data[currentbyte + 1	] = colorbuffer[1];
				data[currentbyte + 2	] = colorbuffer[0];

				color_channel[color_unit] = colorbuffer[0];				    // Flip R and B vcolor values around in the process
				color_unit++;
				color_channel[color_unit] = colorbuffer[1];
				color_unit++;
				color_channel[color_unit] = colorbuffer[2];
				color_unit++;

				if( PerPixel == 4)												// if its a 32 bpp image
				{
					data[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
					alpha_channel[alpha_unit] = colorbuffer[3];
					alpha_unit++;
				}

				currentbyte += PerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if( currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					MessageBox(NULL, "讀取過多的像素", "TGA DLL", NULL);			// if there is too many... Display an error!
                                        if(res != NULL)													// If there is a file open
					{
						fclose(res);													// Close file
					}

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(data != NULL)										// If there is Image data
					{
						delete [] data;
                                                data = NULL;										// delete it
					}
                                        return 0;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, PerPixel, res) != PerPixel)		// Attempt to read following color values
			{
				MessageBox(NULL, "無法的取來源檔", "TGA DLL", MB_OK);			// If attempt fails.. Display error (again)

				if(res != NULL)														// If thereis a file open
				{
					fclose(res);														// Close it
				}

				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(data != NULL)											// If thereis image data
				{
					delete [] data;											// delete it
				}

				return false;															// return failed
			}

			for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated
			{																			// by the header
				data[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				data[currentbyte + 1	] = colorbuffer[1];
				data[currentbyte + 2	] = colorbuffer[0];

				color_channel[color_unit] = colorbuffer[0];				    // Flip R and B vcolor values around in the process
				color_unit++;
				color_channel[color_unit] = colorbuffer[1];
				color_unit++;
				color_channel[color_unit] = colorbuffer[2];
				color_unit++;

				if( PerPixel == 4)												// If TGA images is 32 bpp
				{
					data[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
					alpha_channel[alpha_unit] = colorbuffer[3];
					alpha_unit++;
				}

				currentbyte += PerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if( currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					MessageBox(NULL, "讀取過多的像素", "TGA DLL", NULL);			// if there is too many... Display an error!

					if(res != NULL)													// If there is a file open
					{
						fclose(res);													// Close file
					}

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if( data != NULL)										// If there is Image data
					{
						delete [] data;										// delete it
					}

					return 0;														// Return failed
				}
			}
		}
	}
        while ( currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(res);																		// Close the file
//	 char buf[255];
//	 sprintf(buf,"%d",color_unit);
//	 MessageBox(NULL,buf,"Alpha_value",MB_OK);


	return 1;																		// return success
}


// =================================================================================================================
//													save to Rle Length Encoden
// ------------------------------------------------------------------------------------------------------------------
int TGA_OBJ::tga_write_rle( const char * file, int width, int height, unsigned char * dat, unsigned int format ) {

    FILE * tga;

    unsigned int i, j;
    unsigned int oc, nc;

    enum RLE_STATE { INIT, NONE, RLP, RAWP };

    int state = INIT;

    unsigned int size = width * height;

    unsigned short shortwidth = (unsigned short)width;
    unsigned short shortheight = (unsigned short)height;

    ubyte repcount;

    float red, green, blue, alpha;

    int idx, row, column;

    // have to buffer a whole line for raw packets.
    unsigned char * rawbuf = (unsigned char *)malloc( width * format );  

    ubyte idlen = BLOCK_SIZE;
    ubyte zeroes[5] = { 0, 0, 0, 0, 0 };
    unsigned int pixbuf;
    ubyte one = 1;
    ubyte cmap_type = 0;
    ubyte img_type  = 10;  // 2 - uncompressed truecolor  10 - RLE truecolor
    unsigned short xorigin  = 0;
    unsigned short yorigin  = 0;
    ubyte  pixdepth = format * 8;  // bpp
    ubyte img_desc  = format == TGA_TRUECOLOR_32 ? 8 : 0;
  

    switch( format ) {
    case TGA_TRUECOLOR_24:
    case TGA_TRUECOLOR_32:
        break;

    default:
        //TargaError = TGA_ERR_BAD_FORMAT;
        return( 0 );
    }


    tga = fopen( file, "wb" );

    if( tga == NULL ) {
        //TargaError = TGA_ERR_OPEN_FAILS;
        return( 0 );
    }

    // write id length
    fwrite( &idlen, 1, 1, tga );

    // write colormap type
    fwrite( &cmap_type, 1, 1, tga );

    // write image type
    fwrite( &img_type, 1, 1, tga );

    // write cmap spec.
    fwrite( &zeroes, 5, 1, tga );

    // write image spec.
    fwrite( &xorigin, 2, 1, tga );
    fwrite( &yorigin, 2, 1, tga );
    fwrite( &shortwidth, 2, 1, tga );
    fwrite( &shortheight, 2, 1, tga );
    fwrite( &pixdepth, 1, 1, tga );
    fwrite( &img_desc, 1, 1, tga );


    // write image id.
    fwrite( &block_data, idlen, 1, tga );

    // initial color values -- just to shut up the compiler.
    nc = 0;

    // color correction -- data is in RGB, need BGR.
    // also run-length-encoding.
    for( i = 0; i < size; i++ ) {

        idx = i * format;

        row = i / width;
        column = i % width;

        //printf( "row: %d, col: %d\n", row, column );
        pixbuf = 0;
        for( j = 0; j < format; j++ ) {
            pixbuf += dat[idx+j] << (8 * j);
        }

        switch( format ) {

        case TGA_TRUECOLOR_24:

            pixbuf = ((pixbuf & 0xFF) << 16) + 
                     (pixbuf & 0xFF00) + 
                     ((pixbuf & 0xFF0000) >> 16);

            pixbuf = htotl( pixbuf );
            break;

        case TGA_TRUECOLOR_32:

            /* need to un-premultiply alpha.. */

            red     = (pixbuf & 0xFF) / 255.0f;
            green   = ((pixbuf & 0xFF00) >> 8) / 255.0f;
            blue    = ((pixbuf & 0xFF0000) >> 16) / 255.0f;
            alpha   = ((pixbuf & 0xFF000000) >> 24) / 255.0f;
/*
            if( alpha > 0.0001 ) {
                red /= alpha;
                green /= alpha;
                blue /= alpha;
            }
*/
            /* clamp to 1.0f */

            red = red > 1.0f ? 255.0f : red * 255.0f;
            green = green > 1.0f ? 255.0f : green * 255.0f;
            blue = blue > 1.0f ? 255.0f : blue * 255.0f;
            alpha = alpha > 1.0f ? 255.0f : alpha * 255.0f;

            pixbuf = (ubyte)blue + (((ubyte)green) << 8) + 
                (((ubyte)red) << 16) + (((ubyte)alpha) << 24);
                
            pixbuf = htotl( pixbuf );
            break;

        }


        oc = nc;

        nc = pixbuf;


        switch( state ) {

        case INIT:
            // this is just used to make sure we have 2 pixel values to consider.
            state = NONE;
            break;


        case NONE:

            if( column == 0 ) {
                // write a 1 pixel raw packet for the old pixel, then go thru again.
                repcount = 0;
                fwrite( &repcount, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
                state = NONE;
                break;
            }

            if( nc == oc ) {
                repcount = 0;
                state = RLP;
            } else {
                repcount = 0;
                state = RAWP;
                for( j = 0; j < format; j++ ) {
#ifdef WORDS_BIGENDIAN
                    rawbuf[(repcount * format) + j] = (ubyte)(*((&oc)+format-j-1));
#else
                    rawbuf[(repcount * format) + j] = *(((ubyte *)(&oc)) + j);
#endif
                }
            }
            break;


        case RLP:
            repcount++;

            if( column == 0 ) {
                // finish off rlp.
                repcount |= 0x80;
                fwrite( &repcount, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
                state = NONE;
                break;
            }

            if( repcount == 127 ) {
                // finish off rlp.
                repcount |= 0x80;
                fwrite( &repcount, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
                state = NONE;
                break;
            }

            if( nc != oc ) {
                // finish off rlp
                repcount |= 0x80;
                fwrite( &repcount, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
                state = NONE;
            }
            break;


        case RAWP:
            repcount++;

            if( column == 0 ) {
                // finish off rawp.
                for( j = 0; j < format; j++ ) {
#ifdef WORDS_BIGENDIAN
                    rawbuf[(repcount * format) + j] = (ubyte)(*((&oc)+format-j-1));
#else
                    rawbuf[(repcount * format) + j] = *(((ubyte *)(&oc)) + j);
#endif
                }
                fwrite( &repcount, 1, 1, tga );
                fwrite( rawbuf, (repcount + 1) * format, 1, tga );
                state = NONE;
                break;
            }

            if( repcount == 127 ) {
                // finish off rawp.
                for( j = 0; j < format; j++ ) {
#ifdef WORDS_BIGENDIAN
                    rawbuf[(repcount * format) + j] = (ubyte)(*((&oc)+format-j-1));
#else
                    rawbuf[(repcount * format) + j] = *(((ubyte *)(&oc)) + j);
#endif
                }
                fwrite( &repcount, 1, 1, tga );
                fwrite( rawbuf, (repcount + 1) * format, 1, tga );
                state = NONE;
                break;
            }

            if( nc == oc ) {
                // finish off rawp
                repcount--;
                fwrite( &repcount, 1, 1, tga );
                fwrite( rawbuf, (repcount + 1) * format, 1, tga );
                
                // start new rlp
                repcount = 0;
                state = RLP;
                break;
            }

            // continue making rawp
            for( j = 0; j < format; j++ ) {
#ifdef WORDS_BIGENDIAN
                rawbuf[(repcount * format) + j] = (ubyte)(*((&oc)+format-j-1));
#else
                rawbuf[(repcount * format) + j] = *(((ubyte *)(&oc)) + j);
#endif
            }

            break;

        }
       

    }


    // clean up state.

    switch( state ) {

    case INIT:
        break;

    case NONE:
        // write the last 2 pixels in a raw packet.
        fwrite( &one, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
#ifdef WORDS_BIGENDIAN
                fwrite( (&nc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &nc, format, 1, tga );
#endif
        break;

    case RLP:
        repcount++;
        repcount |= 0x80;
        fwrite( &repcount, 1, 1, tga );
#ifdef WORDS_BIGENDIAN
                fwrite( (&oc)+4, format, 1, tga );  // byte order..
#else
                fwrite( &oc, format, 1, tga );
#endif
        break;

    case RAWP:
        repcount++;
        for( j = 0; j < format; j++ ) {
#ifdef WORDS_BIGENDIAN
            rawbuf[(repcount * format) + j] = (ubyte)(*((&oc)+format-j-1));
#else
            rawbuf[(repcount * format) + j] = *(((ubyte *)(&oc)) + j);
#endif
        }
        fwrite( &repcount, 1, 1, tga );
        fwrite( rawbuf, (repcount + 1) * 3, 1, tga );
        break;

    }


    // close the file.
    fclose( tga );
    free( rawbuf );

    return( 1 );

}



static __int16 ttohs( int16 val ) {

#ifdef WORDS_BIGENDIAN
    return( ((val & 0xFF) << 8) + (val >> 8) );
#else
    return( val );
#endif 

}


static int16 htots( int16 val ) {

#ifdef WORDS_BIGENDIAN
    return( ((val & 0xFF) << 8) + (val >> 8) );
#else
    return( val );
#endif

}


static int32 ttohl( int32 val ) {

#ifdef WORDS_BIGENDIAN
    return( ((val & 0x000000FF) << 24) +
            ((val & 0x0000FF00) << 8)  +
            ((val & 0x00FF0000) >> 8)  +
            ((val & 0xFF000000) >> 24) );
#else
    return( val );
#endif 

}


static int32 htotl( int32 val ) {

#ifdef WORDS_BIGENDIAN
    return( ((val & 0x000000FF) << 24) +
            ((val & 0x0000FF00) << 8)  +
            ((val & 0x00FF0000) >> 8)  +
            ((val & 0xFF000000) >> 24) );
#else
    return( val );
#endif 

}
