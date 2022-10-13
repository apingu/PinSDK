#include "Pep.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
PEP::PEP()
{
	m_WriteData   = 0;
	m_VedioMask   = 0;
	m_StartOffset = 0;
	strcpy(Title, "PEP");

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
PEP::~PEP()
{



}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::Save(const char *destfilepath, std::vector<HPINMAP*> PicList, int Group1Count, ...)
{

	const int TitleLen       = 4;
	const int FileCountLen   = 2;
	const int StatusCountLen = 2;
	const int StatusLen      = 2;
	const int FileOffsetLen  = 4;
    //////////////////////////////


	FILE *WriteFile;
	char WriteFileName[_MAX_PATH];  //dest file name
	
	strcpy(WriteFileName, destfilepath);

	unsigned short  FileCount    = 0;
	unsigned short  StatusCount  = 0;
	unsigned short  Status       = 0;
	unsigned int    FileOffset   = 0;
	int i = Group1Count;
    //////////////////////////////////////////////
    va_list GroupList;
    va_start( GroupList, Group1Count );     /* Initialize variable arguments. */
    while( i != -1 )
    {
	   FileCount = FileCount + (unsigned short)i;
       StatusCount++;
       i = va_arg( GroupList, int);
    }
    va_end( GroupList );              /* Reset variable arguments.      */
		
	//////////////////////////////


	//////////////////////////////////////////////
	//各狀態的數量陣列
	unsigned short* SCountArray = new unsigned short[StatusCount];   /////********** array 1
	int ListCount = 0;
	int AllStatusFileCount = 0;
	i = Group1Count;
	va_list GroupSList;
    va_start( GroupSList, Group1Count );     /* Initialize variable arguments. */
    while( i != -1 )
    {
	   SCountArray[ListCount] = i;
	   AllStatusFileCount = AllStatusFileCount + i;
       i = va_arg( GroupSList, int);
	   ListCount++;
    }
    va_end( GroupSList );              /* Reset variable arguments.      */
	///////////////////////////////////////////////


	if(AllStatusFileCount != PicList.size())
    {
		sprintf(Msg, "argument error!");
		return -1;
	}


	unsigned int* FileOffsetArray = new unsigned int[(PicList.size() + 1)];   /////********** array 2
    
	//open file to write pep file
	if((WriteFile = fopen(WriteFileName, "wb")) == NULL)
	{
		sprintf(Msg, "open dest file failure!");
		return -1;
	}
 
   //write file title
   fwrite(Title,sizeof(Title),1,WriteFile);

   if(m_WriteData != 0)
       fwrite(m_WriteData, m_StartOffset, 1, WriteFile);
   else
	   fseek(WriteFile, m_StartOffset, SEEK_CUR);
   
   //檔案總數
   fwrite(&FileCount,sizeof(FileCount),1,WriteFile);
   //狀態數
   fwrite(&StatusCount, StatusCountLen,1,WriteFile);
   //各狀態的數量
   fwrite(SCountArray, StatusLen, StatusCount, WriteFile);
   delete [] SCountArray;
   
   unsigned long OffsetStart = (sizeof(Title) + sizeof(FileCount) + sizeof(StatusCount))
	            + (StatusCount * sizeof(Status)) + m_StartOffset;//offset 的開始寫入位置
   unsigned long HeadleSize = OffsetStart
				+ ((FileCount + 1) * sizeof(FileOffset));

   //第一個檔案的位移
   FileOffsetArray[0] = HeadleSize;
  
   for(i = 0 ; i < PicList.size() ; i++)
   { 
		  DWORD Filesize = PicList[i]->Size;
		  FileOffsetArray[i + 1] = FileOffsetArray[i] + Filesize;
		  
		  fseek(WriteFile, FileOffsetArray[i], SEEK_SET); //move to dest write start
		  fwrite(PicList[i]->PicHeaderPtr ,PicList[i]->Size,1,WriteFile);    //write menery to dest file 
   }

   fseek(WriteFile, OffsetStart, SEEK_SET);
   fwrite(FileOffsetArray, FileOffsetLen, (FileCount + 1), WriteFile);
   fclose(WriteFile);
   delete [] FileOffsetArray;   

   return FileCount;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::Save(const char *destfilepath, std::vector<const char*> FilePath, int Group1Count, ...)
{
	const int TitleLen       = 4;
	const int FileCountLen   = 2;
	const int StatusCountLen = 2;
	const int StatusLen      = 2;
	const int FileOffsetLen  = 4;
    //////////////////////////////


	FILE *WriteFile, *OpenFile;
	char WriteFileName[_MAX_PATH];  //dest file name
	
	strcpy(WriteFileName, destfilepath);

	unsigned short  FileCount    = FilePath.size();
	unsigned short  StatusCount  = 0;
	unsigned short  Status       = 0;
	unsigned int    FileOffset   = 0;
	int i = Group1Count;
    //////////////////////////////////////////////
    va_list GroupList;
    va_start( GroupList, Group1Count );     /* Initialize variable arguments. */
    while( i != -1 )
    {
	   //FileCount = FileCount + (unsigned short)i;
       StatusCount++;
       i = va_arg( GroupList, int);
    }
    va_end( GroupList );              /* Reset variable arguments.      */
		
	//////////////////////////////


	//////////////////////////////////////////////
	//各狀態的數量陣列
	unsigned short* SCountArray = new unsigned short[StatusCount];   /////********** array 1
	int ListCount = 0;
	int AllStatusFileCount = 0;
	i = Group1Count;
	va_list GroupSList;
    va_start( GroupSList, Group1Count );     /* Initialize variable arguments. */
    while( i != -1 )
    {
	   SCountArray[ListCount] = i;
	   AllStatusFileCount = AllStatusFileCount + i;
       i = va_arg( GroupSList, int);
	   ListCount++;
    }
    va_end( GroupSList );              /* Reset variable arguments.      */
	///////////////////////////////////////////////
	int k = FilePath.size();
	if(AllStatusFileCount != FilePath.size())
    {
		sprintf(Msg, "argument error!");
		return -1;
	}


	unsigned int* FileOffsetArray = new unsigned int[(FilePath.size() + 1)];   /////********** array 2
    
	//open file to write pep file
    if((WriteFile = fopen(WriteFileName, "wb")) == NULL)
	{
		sprintf(Msg, "open dest file failure!");
		return -1;
	}
 
   //write file title
   fwrite(Title,sizeof(Title),1,WriteFile);

   if(m_WriteData != 0)
       fwrite(m_WriteData, m_StartOffset, 1, WriteFile);
   else
       fseek(WriteFile, m_StartOffset, SEEK_CUR);
   
   //檔案總數
   fwrite(&FileCount,sizeof(FileCount),1,WriteFile);
   //狀態數
   fwrite(&StatusCount, StatusCountLen,1,WriteFile);
   //各狀態的數量
   fwrite(SCountArray, StatusLen, StatusCount, WriteFile);
   delete [] SCountArray;
   
   unsigned long OffsetStart = (sizeof(Title) + sizeof(FileCount) + sizeof(StatusCount))
	            + (StatusCount * sizeof(Status)) + m_StartOffset;//offset 的開始寫入位置
   unsigned long HeadleSize = OffsetStart
				+ ((FileCount + 1) * sizeof(FileOffset));

   //第一個檔案的位移
   FileOffsetArray[0] = HeadleSize;

  
   for(i = 0 ; i < FilePath.size() ; i++)
   { 

	   char path[_MAX_PATH];
	   strcpy(path, FilePath[i]);
          if((OpenFile = fopen(FilePath[i], "rb")) == NULL)
		  {
			 sprintf(Msg, "open source combine file failure!");
			 return -3;
		  }
		  

		  int Filesize = filelength(fileno(OpenFile));
		  FileOffsetArray[i + 1] = FileOffsetArray[i] + Filesize;
		  
		  unsigned char *FileBuffer = new unsigned char[Filesize];


		  fseek(OpenFile,0,SEEK_SET);                     //move to combine file start
		  fread(FileBuffer,Filesize,1,OpenFile);          //read combine file to memery
		  fseek(WriteFile, FileOffsetArray[i], SEEK_SET); //move to dest write start
		  fwrite(FileBuffer,Filesize,1,WriteFile);    //write menery to dest file 
		  delete [] FileBuffer;
		  fclose(OpenFile);
   }

   fseek(WriteFile, OffsetStart, SEEK_SET);
   fwrite(FileOffsetArray, FileOffsetLen, (FileCount + 1), WriteFile);
   fclose(WriteFile);
   delete [] FileOffsetArray;   

   return FileCount;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
BYTE* PEP::loadUnit(const char *pathname, int number, int offset)
{
	FILE  *f;

	if((f=fopen(pathname,"rb")) == NULL)//開檔如果-1則失敗，傳回-1
	{
		sprintf(Msg,"Load file %s failure !!!", pathname);
		MessageBox(NULL,Msg, "Load Failed", MB_OK);
		return 0;
	}

	WORD StatusCount;

	fseek(f, offset + 6, SEEK_SET);
	fread(&StatusCount, sizeof(WORD), 1, f);


	int p1 = 0, p2 = 0, unitSize = 0;

	fseek(f, offset + 8 + (StatusCount * 2) + (number * 4), SEEK_SET);
	fread(&p1, sizeof(DWORD), 1, f);

	fseek(f, offset + 8 + (StatusCount * 2) + ((number + 1) * 4), SEEK_SET);
	fread(&p2, sizeof(DWORD), 1, f);

	unitSize = p2 - p1;
	BYTE *Addr = new BYTE[unitSize];

	fseek(f, offset + p1, SEEK_SET);
	fread(Addr, unitSize, 1, f);

	fclose(f);

	return Addr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
BYTE* PEP::loadUnit(FILE *f, int number, int offset)
{
	WORD StatusCount;

	fseek(f, 6, SEEK_SET);
	fread(&StatusCount, sizeof(WORD), 1, f);


	int p1 = 0, p2 = 0, unitSize = 0;

	fseek(f, offset + 8 + (StatusCount * 2) + (number * 4), SEEK_SET);
	fread(&p1, sizeof(DWORD), 1, f);

	fseek(f, offset + 8 + (StatusCount * 2) + ((number + 1) * 4), SEEK_SET);
	fread(&p2, sizeof(DWORD), 1, f);

	unitSize = p2 - p1;
	BYTE *Addr = new BYTE[unitSize];

	fseek(f, offset + p1, SEEK_SET);
	fread(Addr, unitSize, 1, f);

	return Addr;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::loadPep(const char *pathname, WORD **StatusList, std::vector<BYTE*> *PicList)
{
	FILE  *f;

	if((f=fopen(pathname,"rb")) == NULL)//開檔如果-1則失敗，傳回-1
	{
		sprintf(Msg,"Load file %s failure !!!", pathname);
		MessageBox(NULL,Msg, "Load Failed", MB_OK);
		return -1;
	}

    long  size=_filelength(fileno(f));
    BYTE* pic = new BYTE[size];
    fseek(f,0,SEEK_SET);								//將檔案指標移至與檔案起始位置相距0的位置
	fread(pic,size,1,f);								//將檔案 (f) (一次) 讀入此(檔案長度) 到buffer (Pic)

	int StatusCount = loadPep(pic, StatusList, PicList);

	delete [] pic;
	
	fclose(f);
	return StatusCount;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::loadPep(BYTE *pepfile, WORD **StatusList, std::vector<BYTE*> *AddrList)
{
	BYTE* picptr = pepfile;

	//檢查檔案格式====================================================================
	if((*(char*)(picptr) != Title[0]) && (*(char*)(picptr + 1) != Title[1]) && (*(char*)(picptr + 2) != Title[2]) && (*(char*)(picptr + 3) != Title[3]))
	{
		sprintf(Msg,"Load file format error !!!");
		MessageBox(NULL,Msg, "Format error", MB_OK);
		return -1;
	}
	//END 檢查檔案格式================================================================
	picptr = picptr + m_StartOffset;

	WORD FileCount    = *(WORD*)(picptr + 4) ;
	WORD StatusCount  = *(WORD*)(picptr + 6) ;

	(*StatusList) = new WORD[StatusCount];

	for(int i = 0 ; i < StatusCount ; i++)
		(*StatusList)[i] = *(WORD*)(picptr + 8 + (i * 2)) ;


	DWORD* OffSet    = new DWORD[FileCount + 1];
	DWORD* FileSize  = new DWORD[FileCount];


	//各圖形的位置
	for(i = 0 ; i <= FileCount ; i ++)
		OffSet[i] = *(DWORD*)(picptr + 8 + (StatusCount * 2) + (i * 4));

	//各圖形的大小
	for(i = 0 ; i < FileCount ; i ++)
		FileSize[i] = OffSet[i + 1] - OffSet[i];

	int OffSetIndex = 0;
	int FileSizeIndex = 0;

	for(int j = 0; j < StatusCount; j++)
	{
		for( i = 0; i < (*StatusList)[j]; i ++)
		{
			BYTE *Addr = new BYTE[FileSize[FileSizeIndex]];
     		if(Addr == NULL)
			    return -1;
			
			picptr = pepfile + OffSet[OffSetIndex];
			memcpy(Addr, picptr, FileSize[FileSizeIndex]);

			//BYTE *Addr = new BYTE;
			//BYTE* Addr = pepfile + OffSet[OffSetIndex];

			AddrList->push_back(Addr);
			OffSetIndex ++;
			FileSizeIndex ++;
		}
	}

	delete [] OffSet;
	delete [] FileSize;

	return StatusCount;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::loadPep(const char *pathname, WORD **StatusList, std::vector<HPINMAP*> *PicList)
{
	FILE  *f;

	if((f=fopen(pathname,"rb")) == NULL)//開檔如果-1則失敗，傳回-1
	{
		sprintf(Msg,"Load file %s failure !!!", pathname);
		MessageBox(NULL,Msg, "Load Failed", MB_OK);
		return -1;
	}

    long  size=_filelength(fileno(f));
    BYTE* pic = new BYTE[size];
    fseek(f,0,SEEK_SET);								//將檔案指標移至與檔案起始位置相距0的位置
	fread(pic,size,1,f);								//將檔案 (f) (一次) 讀入此(檔案長度) 到buffer (Pic)

	int StatusCount = loadPep(pic, StatusList, PicList);

	delete [] pic;
	
	fclose(f);
	return StatusCount;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
int PEP::loadPep(BYTE *pepfile, WORD **StatusList, std::vector<HPINMAP*> *PicList)                               //讀入Pop圖形包裝檔
{
	BYTE* picptr = pepfile;

	//檢查檔案格式====================================================================
	char title[4];               
	title[0] = *(char*)(picptr);
	title[1] = *(char*)(picptr + 1);
	title[2] = *(char*)(picptr + 2);
	if((title[0] != Title[0]) || (title[1] != Title[1]) || (title[2] != Title[2]))
	{
		sprintf(Msg,"Load file format error !!!");
		MessageBox(NULL,Msg, "Format error", MB_OK);
		return -1;
	}
	//END 檢查檔案格式================================================================

    picptr	= picptr + m_StartOffset;

	WORD FileCount    = *(WORD*)(picptr + 4) ;
	WORD StatusCount  = *(WORD*)(picptr + 6) ;

	(*StatusList) = new WORD[StatusCount];

	for(int i = 0 ; i < StatusCount ; i++)
	{
		(*StatusList)[i] = *(WORD*)(picptr + 8 + (i * 2)) ;
	}


	DWORD* OffSet    = new DWORD[FileCount + 1];
	DWORD* FileSize  = new DWORD[FileCount];


	//各圖形的位置
	for(i = 0 ; i <= FileCount ; i ++)
		OffSet[i] = *(DWORD*)(picptr + 8 + (StatusCount * 2) + (i * 4));

	//各圖形的大小
	for(i = 0 ; i < FileCount ; i ++)
		FileSize[i] = OffSet[i + 1] - OffSet[i];

	int OffSetIndex = 0;
	int FileSizeIndex = 0;

	for(int j = 0; j < StatusCount; j++)
	{
		for( i = 0; i < (*StatusList)[j]; i ++)
		{
			HPINMAP *Pin = new HPINMAP;
			//picptr = pepfile + OffSet[OffSetIndex];
			BYTE *PinPic = new BYTE[FileSize[FileSizeIndex]];
     		if(PinPic == NULL)
				return -1;
			
			picptr = pepfile + OffSet[OffSetIndex];
			memcpy(PinPic, picptr, FileSize[FileSizeIndex]);
			            
			Pin->Load(PinPic, m_VedioMask);
			PicList->push_back(Pin);

			OffSetIndex ++;
			FileSizeIndex ++;
		}
	}

	delete [] OffSet;
	delete [] FileSize;

	return StatusCount;
}