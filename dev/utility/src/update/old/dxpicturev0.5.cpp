/*================================================================================================
 *
 *  Pin Map
 *
 * 
 *  PIN header       ver0.5
 *
 *            typedef struct tagPinHEADER{
 *                    char  ApName[4];	        //4
 *	                  WORD  ApWidth;	        //2  +4
 *	                  WORD  ApHeight;		    //2  +6
 *                    DWORD ApOffset;		    //4  +8
 *                    WORD  ApPixelMode;        //2  +12
 *                    WORD  ApMisregistrationX; //2  +14
 *                    WORD  ApMisregistrationY; //2  +16
 *                    short ApRelativeX;	    //2  +18
 *                    short ApRelativeY;	    //2  +20
 *                    WORD  ApPinWidth;         //2  +22
 *                    WORD  ApPinHeight;        //2  +24
 *                    WORD  ApRectWidth;        //2  +26
 *                    WORD  ApRectHidth;        //2  +28
 *                    BYTE  ApAct;              //1  +30
 *                    BYTE  ApDirection;        //1  +31
 *                    BYTE  ApStep;             //1  +32
 *                    BYTE  ApReserved[31];	    //31
 *            }PINHEADER;
 *
 *
 *                                                       2000 1 20
 *================================================================================================*/

#include "DXPicture.H"

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP::tagHPINMAP()
{
    MapType       = 0;
    Width         = 800;
    RealWidthByte = 1600;
    Height        = 600;
    Pixel         = 2;
	Size          = 0;
	RectWidth     = 0;
	RectHeight    = 0;
    Norm.x        = 0;
    Norm.y        = 0;
    RelativeX     = 0;
    RelativeY     = 0;
    PicHeaderPtr  = 0;
    PicImagePtr   = 0;
	PicPalette    = 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP::~tagHPINMAP()
{
	if(PicHeaderPtr != 0) 
	{
		delete [] PicHeaderPtr;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP& tagHPINMAP::operator=(const tagHPINMAP &src)
{
    Size          = src.Size;
	MapType       = src.MapType;
	Width         = src.Width;
    RealWidthByte = src.RealWidthByte;
    Height        = src.Height;
    Pixel         = src.Pixel;
    RelativeX     = src.RelativeX;
    RelativeY     = src.RelativeY;
    Norm          = src.Norm;
    PicHeaderPtr  = src.PicHeaderPtr;
    PicImagePtr   = src.PicImagePtr;
	PicPalette    = src.PicPalette;
	DatumLine     = src.DatumLine;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  清除影像
//
void tagHPINMAP::Clear(int Color)
{
	memset(PicImagePtr, Color, (Height* RealWidthByte));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  釋放
//
void tagHPINMAP::Release(void)
{
    Size          = 0;
	MapType       = 0;
	Width         = 0;
    RealWidthByte = 0;
    Height        = 0;
    Pixel         = 0;
    RelativeX     = 0;
    RelativeY     = 0;
    Norm(0, 0);
    DatumLine(0, 0, 0, 0);
	if(PicHeaderPtr != 0)
	{
	     delete [] PicHeaderPtr;
	}
    PicHeaderPtr = 0;

    if(PicImagePtr != 0)
	{
	     delete [] PicImagePtr;
    }
	PicImagePtr = 0;

	if(PicPalette != 0)
	{
	     delete [] PicPalette;
    }
	PicPalette    = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將影像檔案讀入記憶體
//
int tagHPINMAP::Load(char *pathname)
{
	int  f;

	char msg[30];
 	
	if((f=open(pathname,O_RDONLY | O_BINARY)) == -1)//開檔如果-1則失敗，傳回-1
	{
		sprintf(msg,"Load file %s failure !!!", pathname);
		MessageBox(NULL,msg, "Load Failed", MB_OK);
		return -1;
 	}
	
    long  size = filelength(f);
    BYTE* pic  = new BYTE[size];
    lseek(f,0,SEEK_SET);								//將檔案指標移至與檔案起始位置相距0的位置
	read(f,pic,size);									//將檔案 (f) (一次) 讀入此(檔案長度) 到buffer (Pic)
	
	//檢查檔案格式====================================================================
	char Title[4];               
	Title[0] = *(char*)(pic);
	Title[1] = *(char*)(pic + 1);
	Title[2] = *(char*)(pic + 2);
	if((Title[0] != 'P') && (Title[0] != 'I') && (Title[0] != 'N'))
	{
		sprintf(msg,"Load file %s format error !!!", pathname);
		MessageBox(NULL,msg, "Format error", MB_OK);
		return -1;
	}
	//END 檢查檔案格式================================================================
	
	int OffSet     = *(DWORD*)(pic + 8 );
	if(OffSet == 64)
	{
		MapType = 0;
	}
	else
	{
		MapType = 1;
	}

	Size           = size;

    Pixel          = *(WORD *)(pic + 12);
	RelativeX      = *(short*)(pic + 18);
	RelativeY      = *(short*)(pic + 20);

	Width          = *(WORD*) (pic + 22);
	RealWidthByte  =  Width * (Pixel /8);
	Height         = *(WORD*) (pic + 24);
	RectWidth      = *(WORD*) (pic + 26);
	RectHeight     = *(WORD*) (pic + 28);
	Norm.x         = *(WORD*) (pic + 14) + RelativeX;
	Norm.y         = *(WORD*) (pic + 16) + RelativeY;
	
	PicHeaderPtr   = pic;
	PicImagePtr	   = pic + OffSet;

	close(f);
	
    return 1;											//傳回記憶體的指標

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將影像檔案讀入記憶體
//
void tagHPINMAP::Load(BYTE *pic)
{

	int OffSet     = *(DWORD*)(pic + 8 );
	if(OffSet == 64)
	{
		MapType = 0;
	}
	else
	{
		MapType = 1;
	}

    Pixel          = *(WORD *)(pic + 12);
	RelativeX      = *(short*)(pic + 18);
	RelativeY      = *(short*)(pic + 20);

	Width          = *(WORD*) (pic + 22);
	RealWidthByte  =  Width * (Pixel /8);
	Height         = *(WORD*) (pic + 24);

	Norm.x         = *(WORD*) (pic + 14) + RelativeX;
	Norm.y         = *(WORD*) (pic + 16) + RelativeY;
	
	PicHeaderPtr   = pic;
	PicImagePtr	   = pic + OffSet;
	Size           = (RealWidthByte * Height) + OffSet;


    return ;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  重新載入
//
void tagHPINMAP::ReLoad(void)
{
    this->Load(PicHeaderPtr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// change 565 mode to 555 mode
//
void tagHPINMAP::P565ToP555(void)
{

	BYTE* pic555ptr = PicImagePtr;
				 
	for(int j = 0 ; j < Height; j++)
	{
    	for( int i = 0; i < Width; i++)
    	{
			register BYTE SR = ( *(WORD *)pic555ptr >> 11 );
        	register BYTE SG = ((*(WORD *)pic555ptr >> 6  ) & 0X1F);
	        register BYTE SB = ((*(WORD *)pic555ptr) & 0x1F);

            *(WORD*)pic555ptr = (SR << 10) |  (SG << 5)  | SB;
            pic555ptr    = pic555ptr + 2;
          
		}
	}

	return ;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// change 565 mode to 556 mode
//
void tagHPINMAP::P565ToP556(void)
{

	BYTE* pic556ptr = PicImagePtr;
				 
	for(int j = 0 ; j < Height; j++)
	{
    	for( int i = 0; i < Width; i++)
    	{
			register BYTE SR = ( *(WORD *)pic556ptr >> 11 );
        	register BYTE SG = ((*(WORD *)pic556ptr >> 6  ) & 0X1F);
	        register BYTE SB = ((*(WORD *)pic556ptr) & 0x1F);

            *(WORD*)pic556ptr = (SR << 11) |  (SG << 6)  | SB;
            pic556ptr    = pic556ptr + 2;
          
		}
	}

	return ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// change 565 mode to 655 mode
//
void tagHPINMAP::P565ToP655(void)
{

	BYTE* pic655ptr = PicImagePtr;
				 
	for(int j = 0 ; j < Height; j++)
	{
    	for( int i = 0; i < Width; i++)
    	{
			register BYTE SR = ( *(WORD *)pic655ptr >> 11 );
        	register BYTE SG = ((*(WORD *)pic655ptr >> 6  ) & 0X1F);
	        register BYTE SB = ((*(WORD *)pic655ptr) & 0x1f);

            *(WORD*)pic655ptr = (SR << 10) |  (SG << 5)  | SB;
            pic655ptr    = pic655ptr + 2;
		}
	}

	return ;

}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// 判斷遠近
//
// Screen coordinate system
// Is line(p1,p2) closer to user's eyes then point(p3)?
// Return < 0 for no, = 0 for equal, > 0 for yes
//
//        p3.y - p1.y   p2.y - p1.y
// return ----------- < -----------
//        p3.x - p1.x   p2.x - p1.x
//
// return < 0: my zorder is small then obj
//        = 0: equal
//        > 0: large (closer to the user, last paint!)

#define IS_CLOSE_THEN(p1,p2,p3)  ((p2.y-p1.y)*(p3.x-p1.x)-(p3.y-p1.y)*(p2.x-p1.x))


int tagHPINMAP::ZOrder(tagHPINMAP &Hpinmap)
{
    //speedup
	//if (nZOrder && obj->GetZOrder())
    //  return nZOrder - obj->GetZOrder();
    // Is my baseline closer to the user than obj's?
    if (DatumLine.LP.x <= Hpinmap.DatumLine.LP.x)
    {
	    return IS_CLOSE_THEN(DatumLine.LP, DatumLine.RP, Hpinmap.DatumLine.LP);
    }
    else
    {
	    return -IS_CLOSE_THEN(Hpinmap.DatumLine.LP, Hpinmap.DatumLine.RP, DatumLine.LP);
    }
}