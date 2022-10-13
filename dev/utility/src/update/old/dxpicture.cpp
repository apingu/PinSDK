/*================================================================================================
 *
 *  Pin Map
 *
 * 
    char    ApName[4];	        //4
	WORD    ApAlphaOffset;      //2  4  read headle length and alpha start
	DWORD   ApImageOffset;      //4  6  image array start address
    WORD    ApPinWidth;         //2  10
    WORD    ApPinHeight;        //2  12
    BYTE    ApPixelMode;	    //1  14
    short   ApRelativeX;	    //2  15
    short   ApRelativeY;	    //2  17
    WORD    ApRectleft;         //2  19
    WORD    ApRecttop;          //2  21
    WORD    ApRectright;        //2  23
    WORD    ApRectbottom;       //2  25
	WORD    APHitPointX;        //2  27
    WORD    APHitPointY;        //2  29
	SrcInfo ApSrcImage;         //8  31 Source image infomation  
	ExtInfo ApExtInfo;          //3  39 extern information
    BYTE    ApReserved[22];     //22 42 64
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
    RelativeX     = 0;
    RelativeY     = 0;
    PicHeaderPtr  = 0;
	PicAlphaPtr   = 0;
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
    PicHeaderPtr  = src.PicHeaderPtr;
    PicImagePtr   = src.PicImagePtr;
	PicAlphaPtr   = src.PicAlphaPtr;
	PicPalette    = src.PicPalette;
	DatumLine     = src.DatumLine;
	HitPoint      = src.HitPoint;
	ObjRect       = src.ObjRect;
	timer         = src.timer;
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
	HitPoint(0, 0);
	ObjRect(0, 0, 0, 0);
    DatumLine(0, 0, 0, 0);
	if(PicHeaderPtr != 0)
	{
	     delete [] PicHeaderPtr;
	}
    PicHeaderPtr = 0;
	PicAlphaPtr  = 0;
	PicImagePtr  = 0;
    PicPalette   = 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將影像檔案讀入記憶體
//
int tagHPINMAP::Load(const char *pathname)
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
	
	int AlphaOffSet     = *(WORD*) (pic + 4 );
	int ImageOffSet     = *(DWORD*)(pic + 6 );
	
	if(AlphaOffSet == ImageOffSet)  //no alpha
	{
		MapType = 0;
	}
	else              //have alpha
	{
		MapType = 1;
	}

	Width          = *(WORD*) (pic + 10);
	RealWidthByte  =  Width * (Pixel /8);
	Height         = *(WORD*) (pic + 12);
	Pixel          = *(BYTE *)(pic + 14);

	RelativeX      = *(short*)(pic + 15);
	RelativeY      = *(short*)(pic + 17);

    ObjRect.left   = *(WORD*) (pic + 19);
    ObjRect.top    = *(WORD*) (pic + 21);
    ObjRect.right  = *(WORD*) (pic + 23);
    ObjRect.bottom = *(WORD*) (pic + 25);

	HitPoint.x     = *(WORD*) (pic + 27);
	HitPoint.y     = *(WORD*) (pic + 29);

	PicHeaderPtr   = pic;
	PicAlphaPtr       = pic + AlphaOffSet;
	PicImagePtr	   = pic + ImageOffSet;
	Size           = size;
    

	close(f);
	
    return 1;											//傳回記憶體的指標

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將影像檔案讀入記憶體
//
void tagHPINMAP::Load(BYTE *pic)
{

	int AlphaOffSet     = *(WORD*) (pic + 4 );
	int ImageOffSet     = *(DWORD*)(pic + 6 );
	
	if(AlphaOffSet == ImageOffSet)  //no alpha
	{
		MapType = 0;
	}
	else              //have alpha
	{
		MapType = 1;
	}

	Width          = *(WORD*) (pic + 10);
	RealWidthByte  =  Width * (Pixel /8);
	Height         = *(WORD*) (pic + 12);
	Pixel          = *(BYTE *)(pic + 14);

	RelativeX      = *(short*)(pic + 15);
	RelativeY      = *(short*)(pic + 17);

    ObjRect.left   = *(WORD*) (pic + 19);
    ObjRect.top    = *(WORD*) (pic + 21);
    ObjRect.right  = *(WORD*) (pic + 23);
    ObjRect.bottom = *(WORD*) (pic + 25);

	HitPoint.x     = *(WORD*) (pic + 27);
	HitPoint.y     = *(WORD*) (pic + 29);

	PicHeaderPtr   = pic;
	PicAlphaPtr    = pic + AlphaOffSet;
	PicImagePtr	   = pic + ImageOffSet;
    Size           = (RealWidthByte * Height) + ImageOffSet;
	
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