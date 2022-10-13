typedef struct tagPINHEADER
{
    char    ApName[4];	        //4
	WORD    ApAlphaOffset;      //2  4  read headle length and alpha start
	DWORD   ApImageOffset;      //4  6  image array start address
    WORD    ApPinWidth;         //2  10
    WORD    ApPinHeight;        //2  12
    BYTE    ApPixelMode;	    //1  14
    short   ApRelativeX;	    //2  15
    short   ApRelativeY;	    //2  17
    int     ApRectleft;         //2  19
    int     ApRecttop;          //2  23
    int     ApRectright;        //2  27
    int     ApRectbottom;       //2  31
	short   APHitPointX;        //2  35
    short   APHitPointY;        //2  37
	SrcInfo ApSrcImage;         //8  39 Source image infomation  
	ExtInfo ApExtInfo;          //3  47 extern information
    BYTE    ApReserved[78];     //78 50 64

}PINHEADER;
