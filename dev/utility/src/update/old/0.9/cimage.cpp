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

#include "CImage.H"
#include "..\ppzip\ppk.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP::tagHPINMAP()
{
	memset(this, 0, sizeof(tagHPINMAP));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP::~tagHPINMAP()
{
	if(PicHeaderPtr != 0) 
        delete [] PicHeaderPtr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
tagHPINMAP& tagHPINMAP::operator=(const tagHPINMAP &src)
{
	memcpy(this, &src, sizeof(tagHPINMAP));
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  計算範圍
//
PCRECT  tagHPINMAP::Rect(PCPOINT Situs)
{
	/*
	int left = Situs.x - RelativeX + ObjRect.left;
	int top  = Situs.y - RelativeY + ObjRect.top;
	PCRECT ImgRect(left, 
		           top, 
		           left + (ObjRect.right  - ObjRect.left),
				   top  + (ObjRect.bottom - ObjRect.top ));  
				   */
	int left = Situs.x - Offset.x;
	int top  = Situs.y - Offset.y;
	PCRECT ImgRect(left + ObjRect.left, 
		           top  + ObjRect.top, 
		           left + ObjRect.right,
				   top  + ObjRect.bottom);  


	return ImgRect;
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
	if(PicHeaderPtr != 0)
        delete [] PicHeaderPtr;
	memset(this, 0, sizeof(tagHPINMAP));

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將壓縮影像檔案讀入記憶體
//
int tagHPINMAP::Load(const char *pathname, int VedioMask)   //讀入Pin圖形檔
{
	int  f;
	char msg[30];
 	
	if((f=open(pathname,O_RDONLY | O_BINARY)) == -1)//開檔如果-1則失敗，傳回-1
	{
		sprintf(msg,"Load file %s failure !!!", pathname);
		MessageBox(NULL,msg, "Load Failed", MB_OK);
		return -1;
 	}

	char  filetype[4];
	read(f, filetype, 4);
	lseek(f,0,SEEK_SET);								//將檔案指標移至與檔案起始位置相距0的位置
    long  size    = filelength(f);
    PicHeaderPtr  = new BYTE[size];
    read(f,PicHeaderPtr,size);							//將檔案 (f) (一次) 讀入此(檔案長度) 到buffer (Pic)

	int ret = Load(PicHeaderPtr, VedioMask);
	if(ret < 0)
	{
	    delete [] PicHeaderPtr;
	    PicHeaderPtr = 0;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  將影像檔案讀入記憶體
//
int tagHPINMAP::Load(BYTE *pic, int VedioMask)
{
 	
	//檢查檔案格式====================================================================
	if(memcmp(pic, "PIN", 3) == 0)
	{

	}
	else if(memcmp(pic, "PZ", 2) == 0)
	{
		CPpk pk;
		
		BYTE* buf;
		pk.unCompress_PZ_BToB(pic, &buf);
		delete [] pic;
		pic = buf;
	}
	else
	{
		MessageBox(NULL,"file format error !!!", "Format error", MB_OK);
		return -1;
	}
	//END 檢查檔案格式================================================================

    m_VedioMask = VedioMask;

    int AlphaOffSet     = *(WORD*) (pic + 4 );
	int ImageOffSet     = *(DWORD*)(pic + 6 );
	
	if(AlphaOffSet == ImageOffSet)  //no alpha
	{
		MapType = 0;
		PicAlphaPtr    = 0;
	}
	else                            //have alpha
	{
		MapType = 1;
		PicAlphaPtr    = pic + AlphaOffSet;
	}

	Pixel          = (*(BYTE*)  (pic + 14) / 8);
	Width          = *(WORD*)   (pic + 10);
	RealWidthByte  = ((Width) * (Pixel));
	Height         = *(WORD*)   (pic + 12);

    RelativeX      = *(short*)  (pic + 15);
	RelativeY      = *(short*)  (pic + 17);
	Offset.x       = *(short*)  (pic + 23) + RelativeX;
	Offset.y       = *(short*)  (pic + 25) + RelativeY;


    ObjRect.left   = *(short*)  (pic + 30);
    ObjRect.top    = *(short*)  (pic + 32);
    ObjRect.right  = *(short*)  (pic + 34);
    ObjRect.bottom = *(short*)  (pic + 36);

	HitPoint.x     = *(WORD*)   (pic + 38);
	HitPoint.y     = *(WORD*)   (pic + 40);

	PicHeaderPtr   = pic;
	PicImagePtr	   = pic + ImageOffSet;
    Size           = (RealWidthByte * Height) + ImageOffSet;

	switch(m_VedioMask)
	{
	case 0:

		 break;

	case 1:
		 P565ToP555();
	     break;

	case 2:
         P565ToP556();
	     break;

	case 3:
	     P565ToP655();
	     break;

	case 4:
	     P565ToP888();
	     break;
	}
	
	return 1;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  重新載入
//
void tagHPINMAP::ReLoad(void)
{
	BYTE* pic = PicHeaderPtr;
    
	int AlphaOffSet     = *(WORD*) (pic + 4 );
	int ImageOffSet     = *(DWORD*)(pic + 6 );
	
	if(AlphaOffSet == ImageOffSet)  //no alpha
	{
		MapType = 0;
		PicAlphaPtr    = 0;
	}
	else                            //have alpha
	{
		MapType = 1;
		PicAlphaPtr    = pic + AlphaOffSet;
	}

	Pixel          = (*(BYTE*)  (pic + 14) / 8);
	Width          = *(WORD*)   (pic + 10);
	RealWidthByte  = ((Width) * (Pixel));
	Height         = *(WORD*)   (pic + 12);

    RelativeX      = *(short*)  (pic + 15);
	RelativeY      = *(short*)  (pic + 17);
	Offset.x        = *(short*) (pic + 23) + RelativeX;
	Offset.y        = *(short*) (pic + 25) + RelativeY;

    ObjRect.left   = *(short*)  (pic + 30);
    ObjRect.top    = *(short*)  (pic + 32);
    ObjRect.right  = *(short*)  (pic + 34);
    ObjRect.bottom = *(short*)  (pic + 36);

	HitPoint.x     = *(WORD*)   (pic + 38);
	HitPoint.y     = *(WORD*)   (pic + 40);
	PicHeaderPtr   = pic;
	PicImagePtr	   = pic + ImageOffSet;
    Size           = (RealWidthByte * Height) + ImageOffSet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// change 555 mode to 565 mode
//
void tagHPINMAP::P555ToP565(void)
{
	BYTE* pic565ptr = PicImagePtr;
				 
	for(int j = 0 ; j < Height; j++)
	{
    	for( int i = 0; i < Width; i++)
    	{
			register BYTE SR = (*(WORD *)pic565ptr >> 10) & 0x1F;           // must clear saved bit
            register BYTE SG = ((*(WORD *)pic565ptr >> 5) & 0x1F) << 1;     // 5 bits to 6 bits
            register BYTE SB = *(WORD *)pic565ptr & 0x1F;
            register BYTE save = (*(WORD *)pic565ptr >> 10) & 0x20;
            *(WORD*)pic565ptr = (SR << 11) | (SG << 5) | SB | save;
            pic565ptr    = pic565ptr + 2;
		}
	}
	return ;
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
			register BYTE SR = *(WORD *)pic555ptr >> 11;        
            register BYTE SG = (*(WORD *)pic555ptr >> 6) & 0x1F;            // 6 bits to 5 bits
            register BYTE SB = (*(WORD *)pic555ptr) & 0x1F;
            register BYTE save = ((*(WORD *)pic555ptr) & 0x20) << 10;        // save r's lowest bit
        
            *(WORD*)pic555ptr = (SR << 10) | (SG << 5) | SB | save;

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


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Is in area if in scopt return the situs in scopt
//             else return -1, -1
bool tagHPINMAP::InArea(PCPOINT situs, PCPOINT point)
{
	PCRECT rect = Rect(situs);
	if ((point.x < rect.right) && (point.x > rect.left) 
	    && (point.y < rect.bottom) && (point.y > rect.top)) 
		return true;

	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Is in scope if in scopt return the situs in scopt
//             else return -1, -1
PCPOINT tagHPINMAP::InScope(PCPOINT point, int NormX, int NormY)
{
    PCPOINT RetPoint(-1, -1);

	if ((point.x < (NormX + ObjRect.right))  && 
	    (point.x > (NormX + ObjRect.left))    && 
		(point.y < (NormY + ObjRect.bottom)) && 
		(point.y > (NormY + ObjRect.top)))
	{
         RetPoint((point.x - NormX), (point.y - NormY));
	}
	return RetPoint;
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
