
class ElmData
{
public:
    char     Title[4]      ;
	DWORD    RouteOffset   ;
	DWORD    TableOffset   ;
	BYTE     ImageCount    ;
	DWORD    *ImageOffset  ;
	int      ID            ;         //element id
	int      GID           ;         //element group id
    PCPOINT  Situs         ;         //出現時位於畫面的位置
	int      DirectionCount;
	int      MoveDistance  ;         //移動點數
	int      colckcycle    ;         //動作的週期時間
	bool     Visible       ;         //可見
	bool     Dragable      ;         //可拖曳
	bool     Enable        ;         //可作用
	bool     Moveable      ;         //可移動
	BYTE     Role          ;         //角色
	char     ApReserved[127];

	ElmData()
	{
	    Title[0]      = 'P';
		Title[1]      = 'E';
		Title[2]      = 'L';
		Title[3]      = 'M';
	    RouteOffset   =  Size();
		TableOffset   =  0 ;
	    ImageCount    =  0 ;
	    ImageOffset   =  0 ;
	    ID            =  0 ;         //element id
	    GID           = -1 ;         //element group id
	    DirectionCount=  1 ;
	    MoveDistance  =  2 ;         //移動點數
	    colckcycle    = 60 ;         //動作的週期時間
	    Visible       = true;        //可見
	    Dragable      = false;       //可拖曳
	    Enable        = true;        //可作用
	    Moveable      = true;        //可移動
		Role          = 0;
		memset(ApReserved, 204, 128);
	}

	~ElmData()
	{
		if(ImageCount != 0)
		   delete [] ImageOffset;
	}

	int Write(FILE *f)
	{
	    fseek(f, 0 , SEEK_SET);
        fwrite(Title, sizeof(Title), 1, f);
	    fwrite(&RouteOffset, sizeof(RouteOffset), 1, f);
	    fwrite(&TableOffset, sizeof(TableOffset), 1, f);
	    fwrite(&ImageCount,  sizeof(ImageCount) , 1, f);
		for(int i = 0 ; i < ImageCount ; i++)
		{
		    fwrite(&ImageOffset[i], sizeof(DWORD), 1, f);
		}
	    fwrite(&ID            , sizeof(ID)            , 1, f);         //element id
	    fwrite(&GID           , sizeof(GID)           , 1, f);         //element group id
	    fwrite(&Situs         , sizeof(Situs)         , 1, f);         //出現時位於畫面的位置
	    fwrite(&DirectionCount, sizeof(DirectionCount), 1, f);
	    fwrite(&MoveDistance  , sizeof(MoveDistance)  , 1, f);	
	    fwrite(&colckcycle    , sizeof(colckcycle)    , 1, f);         //動作的週期時間
	    fwrite(&Visible       , sizeof(Visible)       , 1, f);         //可見
	    fwrite(&Dragable      , sizeof(Dragable)      , 1, f);         //可拖曳
	    fwrite(&Enable        , sizeof(Enable)        , 1, f);         //可作用
		fwrite(&Moveable      , sizeof(Moveable)      , 1, f);         //可移動
		fwrite(&Role          , sizeof(Role)          , 1, f);         //角色
		fwrite(&ApReserved    , sizeof(ApReserved)    , 1, f);         //保留

		return 1;
	}


	int Read(BYTE* Ptr)
	{
	    if((Ptr[0] != 'P') && (Ptr[1] != 'E') && (Ptr[2] != 'L') && (Ptr[3] != 'M'))
		{ 
		    return -1;
		}
		RouteOffset = *(DWORD *)(Ptr + 4);
	    TableOffset = *(DWORD *)(Ptr + 8);
		ImageCount  = *(BYTE  *)(Ptr + 12);
	    

		ImageOffset = new DWORD[ImageCount];
	    for(int i = 0; i < ImageCount; i++)
		{
			ImageOffset[i] = *(DWORD *)(Ptr + 13 + (i * sizeof(DWORD)));
		}
		int ArraySize = ImageCount * sizeof(DWORD);
		ID             = *(int *)    (Ptr + 13 + ArraySize);
		GID            = *(int *)    (Ptr + 17 + ArraySize);
		Situs          = *(PCPOINT *)(Ptr + 21 + ArraySize);
	    DirectionCount = *(int *)    (Ptr + 29 + ArraySize);
		MoveDistance   = *(int *)    (Ptr + 33 + ArraySize);
		colckcycle     = *(int *)    (Ptr + 37 + ArraySize);
		Visible        = *(bool*)    (Ptr + 41 + ArraySize);
		Dragable       = *(bool*)    (Ptr + 42 + ArraySize);
		Enable         = *(bool*)    (Ptr + 43 + ArraySize);
		Moveable       = *(bool*)    (Ptr + 44 + ArraySize);
		Role           = *(BYTE*)    (Ptr + 45 + ArraySize);
		return 1;
	}


	int Size()
	{
		//int esize = 45 + (ImageCount * sizeof(DWORD));
		int esize = 173 + (ImageCount * sizeof(DWORD));

		return esize;
	}

	int Add(long fsize)
	{
		if((ImageCount) != 0 )
		{
			DWORD* TempBuffer = new DWORD[ImageCount];
		    memcpy(TempBuffer, ImageOffset, sizeof(DWORD) * ImageCount);    //位移資訊搬移
			ImageOffset = new DWORD[ImageCount + 1];
	        memcpy(ImageOffset, TempBuffer, sizeof(DWORD) * ImageCount);    //位移資訊搬移
	        delete [] TempBuffer;
			ImageCount ++;
		}
		else
		{
			ImageOffset = new DWORD[ImageCount + 1];
		}
	    RouteOffset = RouteOffset + sizeof(DWORD);                          //路徑的位移往後移4
	    TableOffset = TableOffset + sizeof(DWORD);                          //動作表的位移往後移4
	    ImageOffset[ImageCount] = fsize + sizeof(DWORD);                    //新增加圖形的位移
		ImageCount ++;

		return ImageCount;
	}
};
