/*==================================================================================
 * 改變檔案格式
 *---------------------------------------------------------------------------
 *  傳入256色Alpha圖為遮罩，將24bits BMP檔案轉換為PIN格式
 *
 *  arguments 1 : Alpha圖形的路徑(0 -> 沒有Alpha)
 *  arguments 2 : 來源圖形的路徑
 *  arguments 3 : 目的圖形的路徑
 *  arguments 4 : 基準點x
 *  arguments 5 : 基準點y
 *  arguments 6 : 動作
 *  arguments 7 : 方向
 *  arguments 8 : 順序
 *  arguments 9 : 圖素(16, 24, 32)
 *
 *  action value 0   ----> copy
 *  action value 0   ----> jump
 *  action value 0   ----> changeline
 *
 *
 *                mask      : | 動作 | byte數 |
 *                image     :   填入檔案中
 *
 *
 *  本以open系列寫 但解果會發生讀檔錯誤 ex bmp width set 489 時
 *  所讀的檔案數將與sizeof(f)不符 
 *
 *===========================================================================*/

#include "..\Define\PGF.h"



typedef struct tagRect{
    unsigned int Top;
    unsigned int Botton;
    unsigned int Right;
    unsigned int Left;
}Rect;

//int main(int argc, char *argv[])
bool Translate(char *alphafilepath, char *srcfilepath, char *destfilepath,WORD AdtumMarkX, WORD AdtumMarkY, BYTE Act, BYTE Direction, BYTE Step, WORD mode)
{

	BITMAPFILEHEADER bif, abif;
	BITMAPINFOHEADER bih, abih;
	PINHEADER myheader;

																	//char *srcfilepath, *alphafilepath, *destfilepath;
																	//WORD AdtumMarkX, AdtumMarkY;
																	//BYTE Act, Direction, Step;
																	//WORD mode;
	char infomessage[50];
	int BitMapWidth;
	int AlphaWidth;
	FILE *asrcfptr, *srcfptr, *destfptr;

	unsigned int  rowscount = 0, colscount = 0;
	unsigned short int colorcount = 0 , pointscount = 0;


	BYTE AlphaPoint;
	BYTE putinfo;
	Point24 BmpPoint;
	Rect MapRect   = {0, 0, 0, 0};
	Rect AlphaRect = {0, 0, 0, 0};
	unsigned int pTop = 0, pBottom = 0, pLeft = 0, pRight = 0, i, j;

																	//alphafilepath	=      argv[1] ;
																	//srcfilepath	=      argv[2] ;
																	//destfilepath	=      argv[3] ;
																	//AdtumMarkX	= atoi(argv[4]);
																	//AdtumMarkY	= atoi(argv[5]);
																	//Act     		= atoi(argv[6]);
																	//Direction     = atoi(argv[7]);
																	//Step          = atoi(argv[8]);
																	//mode		    = atoi(argv[9]);

                                                                    //----Check----------------------------------------------
																	sprintf(infomessage, "alphafilepath %s\n",alphafilepath);
																	TRACE(infomessage);
																	sprintf(infomessage, "srcfilepath %s\n",srcfilepath);
																	TRACE(infomessage);
																	sprintf(infomessage, "destfilepath %s\n",destfilepath);
																	TRACE(infomessage);
																	sprintf(infomessage, " x %d\n",AdtumMarkX);
																	TRACE(infomessage);
																	sprintf(infomessage, " y %d\n",AdtumMarkY);
																	TRACE(infomessage);
																	sprintf(infomessage, "mode %d\n",mode);
																	TRACE(infomessage);
																	//----End Check------------------------------------------


//===============================================================================================
	/*open writed file*/
	if((destfptr = fopen(destfilepath, "wb")) == NULL)
	{
		sprintf(infomessage,"open dest file failure!");
		goto OnError;
	}
//===============================================================================================
	/*open source bmp file*/
	if((srcfptr = fopen(srcfilepath, "rb")) == NULL)
	{
		sprintf(infomessage,"open source file failure !!!");
		goto OnError;
	}
	/*read source bmp file header*/
	if(fread(&bif,1,sizeof(bif),srcfptr) != sizeof(bif))
	{
		sprintf(infomessage,"read source file file header failure !!!");
		fclose(srcfptr);
		goto OnError;
	}
	                                                                    //----Check----------------------------------------------
																		sprintf(infomessage, "type %d\n",bif.bfType);				TRACE(infomessage);
																		sprintf(infomessage, "bfSize %d\n",bif.bfSize);				TRACE(infomessage);
																		sprintf(infomessage, "bfReserved1 %d\n",bif.bfReserved1);	TRACE(infomessage);
																		sprintf(infomessage, "bfReserved2 %d\n",bif.bfReserved2);	TRACE(infomessage);
																		sprintf(infomessage, "bfOffBits %d\n",bif.bfOffBits);		TRACE(infomessage);
																		//----END Check------------------------------------------
	if(fread(&bih,1,sizeof(bih),srcfptr) != sizeof(bih))
	{
		sprintf(infomessage,"read soure file header failure !!!");
		fclose(srcfptr);
		goto OnError;
	}
	                                                                    //----Check----------------------------------------------
																		sprintf(infomessage, "biSize %d\n",bih.biSize);				TRACE(infomessage);
																		sprintf(infomessage, "biWidth %d\n",bih.biWidth);			TRACE(infomessage);
																		sprintf(infomessage, "biHeight %d\n",bih.biHeight);			TRACE(infomessage);
																		sprintf(infomessage, "biPlanes %d\n",bih.biPlanes);			TRACE(infomessage);
																		sprintf(infomessage, "biBitCount %d\n",bih.biBitCount);		TRACE(infomessage);
																		//----END Check------------------------------------------

	/* check for source bmp file pixel format */
	if((bih.biWidth > 65535) || (bih.biHeight > 65535))																		
	{
		sprintf(infomessage,"picture width or height must less than 65535 !!!");
		goto OnError;
	}

	if(bih.biBitCount != 24)
	{
		sprintf(infomessage,"Source file only support for 24 bits color now !!!");
		goto OnError;
	}

	
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================
	if(strcmp(alphafilepath,"0") != 0)//==========================================================
	{
		/*open source alpha file*/
		if((asrcfptr = fopen(alphafilepath, "rb")) == NULL)
		{
			sprintf(infomessage,"open alpha file failure!");
            strcpy(alphafilepath, "0");
			goto NOAlpha;	
			//goto OnError;
		}
			/*read alpha bmp file header*/
			if(fread(&abif,1,sizeof(abif),asrcfptr) != sizeof(abif))
			{
				sprintf(infomessage,"read alpha file header failure !!!");	
				fclose(asrcfptr);	
				goto OnError;
			}
			                                                            //----Check----------------------------------------------	
																		sprintf(infomessage, "atype %d\n",abif.bfType);				TRACE(infomessage);
																		sprintf(infomessage, "abfSize %d\n",abif.bfSize);			TRACE(infomessage);
																		sprintf(infomessage, "abfReserved1 %d\n",abif.bfReserved1);	TRACE(infomessage);
																		sprintf(infomessage, "abfReserved2 %d\n",abif.bfReserved2);	TRACE(infomessage);
																		sprintf(infomessage, "abfOffBits %d\n",abif.bfOffBits);		TRACE(infomessage);
																		//----END Check------------------------------------------


			if(fread(&abih,1,sizeof(abih),asrcfptr) != sizeof(abih))
			{
				sprintf(infomessage,"read alpha header failure !!!");	
				fclose(asrcfptr);	
				goto OnError;
			}
                                                                        //----Check----------------------------------------------
																		sprintf(infomessage, "abiSize %d\n",abih.biSize);			TRACE(infomessage);
																		sprintf(infomessage, "abiWidth %d\n",abih.biWidth);			TRACE(infomessage);
																		sprintf(infomessage, "abiHeight %d\n",abih.biHeight);		TRACE(infomessage);
																		sprintf(infomessage, "abiPlanes %d\n",abih.biPlanes);		TRACE(infomessage);
																		sprintf(infomessage, "abiBitCount %d\n",abih.biBitCount);	TRACE(infomessage);
																		//----END Check------------------------------------------

			if((abih.biWidth > 65535) || (abih.biHeight > 65535))																		
			{
				sprintf(infomessage,"picture width or height must less than 65535 !!!");
				goto OnError;
			}


			if(abih.biBitCount != 8)
			{
				sprintf(infomessage,"Alpha file only support for 256 color now !!!");
				goto OnError;
			}

//===============================================================================================
		fseek(destfptr,sizeof(myheader),SEEK_SET);

	
		//exit(1);
//===============================================================================================

        AlphaWidth  = ((((abih.biWidth) * (abih.biBitCount  / 8)) + 3)/4) * 4;


		pRight = AlphaWidth;
		pBottom = abih.biHeight;

/*----------------------------------------------------------------------------*/
/*                         分析讀入的Alpha檔                                  */
/*----------------------------------------------------------------------------*/

		Point256 AddaPoint;
		FLAG	afirst;

		rowscount	= abih.biHeight;
		colscount	= 0;
		afirst		= 0;

		fseek(asrcfptr,abif.bfOffBits + ((rowscount - 1) * abih.biWidth),SEEK_SET);
		fread(&AlphaPoint,sizeof(BYTE),1,asrcfptr);
		AddaPoint = AlphaPoint;


		while(rowscount > 0)
		{
			fseek(asrcfptr,abif.bfOffBits + ((rowscount - 1) * abih.biWidth),SEEK_SET);
			colscount = 0;

			while(colscount < (unsigned int)bih.biWidth)
			{
				fread(&AlphaPoint,sizeof(BYTE),1,asrcfptr);
				if(AlphaPoint != AddaPoint)
				{
					if(((bih.biHeight - rowscount) < AlphaRect.Top) || (afirst == 0))
					{
						AlphaRect.Top    = (bih.biHeight - rowscount);
					}
					if(((bih.biHeight - rowscount) > AlphaRect.Botton) || (afirst == 0))
					{
						AlphaRect.Botton = (bih.biHeight - rowscount);
					}
					if((colscount < AlphaRect.Left) || (afirst == 0))
					{
						AlphaRect.Left   = colscount;
					}
					if((colscount > AlphaRect.Right) || (afirst == 0))
					{
						AlphaRect.Right  = colscount;
					}
					afirst = 1;
				}
				colscount ++;

			}
			rowscount --;
		}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//      write Alpha mask information to dest
//
        unsigned char forword;

        rowscount = abih.biHeight;

        for( i = AlphaRect.Top ; i < AlphaRect.Botton ; i++)
        {
            fseek(asrcfptr,abif.bfOffBits + (((abih.biHeight - i) - 1) * AlphaWidth) + AlphaRect.Left, SEEK_SET);
            fread(&forword,sizeof(BYTE),1,asrcfptr);
            fseek(asrcfptr,abif.bfOffBits + (((abih.biHeight - i) - 1) * AlphaWidth) + AlphaRect.Left, SEEK_SET);
			
            colorcount		= 0;
                                                                    //----Check-------------------------------------------------------------------------
                                                                    sprintf(infomessage, "=================================\n");	TRACE(infomessage);
                                                                    sprintf(infomessage, "rowscount : %d\n" , i               );	TRACE(infomessage);
                                                                    sprintf(infomessage, "=================================\n\n");	TRACE(infomessage);
                                                                    //----END Check---------------------------------------------------------------------

			for( j = AlphaRect.Left ; j < AlphaRect.Right ; j++)
			{

				fread(&AlphaPoint,sizeof(BYTE),1,asrcfptr);
                                                                    //sprintf(infomessage, "colscount %d\n", colscount);				TRACE(infomessage);
				if(AlphaPoint == forword)
				{
                                                                    //sprintf(infomessage, "colorkey ==> colorkey\n");				TRACE(infomessage);
                    colorcount++; 
                    forword = AlphaPoint;
                    continue;
                                                                    //sprintf(infomessage, "-------------------\n");					TRACE(infomessage);
				}

				else
				{
                                                                    //sprintf(infomessage, "colorkey ==> color\n");					TRACE(infomessage);
                    putinfo = forword;
                    fwrite(&putinfo,sizeof(putinfo),1,destfptr);

                    if(mode == 16)
                    {
                        colorcount = colorcount * 2;
                    }
                    else if(mode == 24)
                    {
                        colorcount = colorcount * 3;
                    }
                    else if(mode == 32)
                    {
                        colorcount = colorcount * 4;
                    }

                    pointscount = pointscount + colorcount ;//檢查總點轉的bytes數
                    fwrite(&colorcount,sizeof(colorcount),1,destfptr);

                    forword = AlphaPoint;
                    colorcount = 1;
                                                                    //sprintf(infomessage, "-------------------\n");						TRACE(infomessage);
				}
			}
	
            if(colorcount != 0)
            {
                putinfo = forword;
                fwrite(&putinfo,sizeof(putinfo),1,destfptr);
                
                if(mode == 16)
                {
                    colorcount = colorcount * 2;
                }
                else if(mode == 24)
                {
                    colorcount = colorcount * 3;
                }
                else if(mode == 32)
                {
                     colorcount = colorcount * 4;
                }
                
                pointscount = pointscount + colorcount ;//檢查總點轉的bytes數
                fwrite(&colorcount,sizeof(colorcount),1,destfptr);
            }



                                                                        if(pointscount != (unsigned int)(AlphaRect.Right - AlphaRect.Left) * abih.biBitCount/4)
                                                                        {
                                                                            sprintf(infomessage, " I'm sorry , I have bug for change file failure in colscount %d   rowscount %d \n",colscount, rowscount);						TRACE(infomessage);
                                                                            goto OnError;
                                                                        }
                                                                        sprintf(infomessage, "pointscount %d \n",pointscount);						TRACE(infomessage);
                                                                        pointscount = 0 ;

        }
        fclose(asrcfptr);
   }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
NOAlpha:
/*----------------------------------------------------------------------------*/
/*                         分析讀入的bmp檔                                    */
/*----------------------------------------------------------------------------*/
//if(strcmp(alphafilepath,"0") == 0)
{
		Point24 AddPoint;
		FLAG	first;

		rowscount	= bih.biHeight;
		colscount	= 0;
		first		= 0;

		fseek(srcfptr,bif.bfOffBits + ((rowscount - 1) * (bih.biWidth * (bih.biBitCount / 8))), SEEK_SET);
		fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);

		AddPoint.R = BmpPoint.R;
		AddPoint.G = BmpPoint.G;
		AddPoint.B = BmpPoint.B;

		while(rowscount > 0)
		{
			fseek(srcfptr,bif.bfOffBits + ((rowscount - 1) * (bih.biWidth * (bih.biBitCount / 8))), SEEK_SET);

			colscount = 0;

			while(colscount < (unsigned int)bih.biWidth)
			{
				fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);
				if((BmpPoint.R != AddPoint.R) && (BmpPoint.G != AddPoint.G) && (BmpPoint.B != AddPoint.B))
				{
					if(((bih.biHeight - rowscount) < MapRect.Top) || (first == 0))
					{
						MapRect.Top    = (bih.biHeight - rowscount);
					}
					if(((bih.biHeight - rowscount) > MapRect.Botton) || (first == 0))
					{
						MapRect.Botton = (bih.biHeight - rowscount);
					}
					if((colscount < MapRect.Left) || (first == 0))
					{
						MapRect.Left   = colscount;
					}
					if((colscount > MapRect.Right) || (first == 0))
					{
						MapRect.Right  = colscount;
					}
					first = 1;
				}
				//read(srcfptr, &BmpPoint, sizeof(BmpPoint));
				colscount ++;

			}
			rowscount --;
		}


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------------*/
/*                         寫入自己的檔頭                                     */
/*----------------------------------------------------------------------------*/
   	/* move to destination file start address */
	fseek(destfptr,0,SEEK_SET);

	strcpy(myheader.ApName, "PIN");

	myheader.ApWidth            =   bih.biWidth;
	myheader.ApHeight           =   bih.biHeight;

	if(strcmp(alphafilepath,"0") != 0) //含有Alpha檔
	{
		myheader.ApOffset       =   filelength(fileno(destfptr));
		myheader.ApMisregistrationX	= AlphaRect.Left;				//左上角X
	    myheader.ApMisregistrationY	= AlphaRect.Top;				//左上角Y
		pLeft   = AlphaRect.Left;
		pTop    = AlphaRect.Top;
		pRight  = AlphaRect.Right;
		pBottom = AlphaRect.Botton;
	}
	else                               //無Alpha檔
	{
        myheader.ApOffset	    =   sizeof(myheader);
        myheader.ApMisregistrationX	= MapRect.Left;				//左上角X
        myheader.ApMisregistrationY	= MapRect.Top;				//左上角Y
		myheader.ApMisregistrationX	= MapRect.Left;				//左上角X
	    myheader.ApMisregistrationY	= MapRect.Top;				//左上角Y
		pLeft   = MapRect.Left;
		pTop    = MapRect.Top;
		pRight  = MapRect.Right;
		pBottom = MapRect.Botton;
	}

	myheader.ApPixelMode	    =   mode;

   
	
  	myheader.ApRelativeX	    =   AdtumMarkX - pLeft;
   	myheader.ApRelativeY	    =   AdtumMarkY - pTop;


   	myheader.ApAct              =   Act;
	myheader.ApDirection        =   Direction;
	myheader.ApStep             =   Step;

	myheader.ApPinWidth         =   pRight  - pLeft;
  	myheader.ApPinHeight        =   pBottom - pTop;
    myheader.ApRectWidth        =   MapRect.Right  - MapRect.Left;
    myheader.ApRectHeight       =   MapRect.Botton - MapRect.Top;
    	
	memset(myheader.ApReserved, 204, sizeof(myheader.ApReserved));

	fwrite(&myheader	,sizeof(myheader)		,1,destfptr);

	//exit(0);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////   write to image

	/* move to destination file image start address */
    BitMapWidth = ((((bih.biWidth) * (bih.biBitCount / 8)) + 3)/4) * 4;
	fseek(destfptr,myheader.ApOffset,SEEK_SET);

	//lseek(destfptr,myheader.ApOffset,SEEK_SET);
	
	/*將資料檔寫入*/

	
		for( i = pTop ; i < pBottom ; i++)
		{
																		sprintf(infomessage, "=================================\n");		TRACE(infomessage);
																		sprintf(infomessage, "rowscount : %d\n", (bih.biHeight - i) );		TRACE(infomessage);
																		sprintf(infomessage, "=================================\n");		TRACE(infomessage);
			fseek(srcfptr,bif.bfOffBits + (((bih.biHeight - i) - 1) * BitMapWidth) + (pLeft * (bih.biBitCount / 8)), SEEK_SET);
			//lseek(srcfptr,bif.bfOffBits + ((rowscount - 1) * (bih.biWidth * (bih.biBitCount / 8))), SEEK_SET);


			for( j = pLeft ; j < pRight ; j++)
			{
				/*--------------16 bits Pin file---------------------*/
				if (myheader.ApPixelMode == 16)
				{
					PINPOINT16 Ppoint;

					fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);
					//read(srcfptr, &BmpPoint, sizeof(BmpPoint));
					Ppoint = ((BmpPoint.R >> 3) << 11) |  ((BmpPoint.G >> 2) << 5)  | (BmpPoint.B >> 3);
                    if(Ppoint != 0)
                    {
                        Ppoint = Ppoint;
                    }
					//Ppoint = ((Ppoint << 8) | (Ppoint >> 8));
					fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
					//write(destfptr,&Ppoint,sizeof(Ppoint));
				}
				/*--------------24 bits Pin file---------------------*/
				else if(myheader.ApPixelMode == 24)
				{

					PINPOINT24 Ppoint;

					fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);
					//read(srcfptr, &BmpPoint, sizeof(BmpPoint));
					Ppoint.R = BmpPoint.R ;
					Ppoint.G = BmpPoint.G ;
					Ppoint.B = BmpPoint.B ;
					fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
					//write(destfptr,&Ppoint,sizeof(Ppoint));
				}
				/*--------------32 bits Pin file---------------------*/
				else if(myheader.ApPixelMode == 32)
				{
					PINPOINT32 Ppoint;

					fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);
					//read(srcfptr, &BmpPoint, sizeof(BmpPoint));
					Ppoint.Reserved = 0   ;
					Ppoint.R        = BmpPoint.R ;
					Ppoint.G        = BmpPoint.G ;
					Ppoint.B        = BmpPoint.B ;
					fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
					//write(destfptr,&Ppoint,sizeof(Ppoint));

				}
				else
				{
					sprintf(infomessage,"Pin file dont't support this mode !!!!\n");
					goto OnError;
				}

			}

		}


//////////////////////////////////////////////////////////////////////////////////////////////////


	fclose(destfptr);
	fclose(srcfptr);
									sprintf(infomessage, "Change file successful !!!!\n");


																TRACE(infomessage);
	//MessageBox(NULL,infomessage,"Successful", MB_OK);
    return true;

OnError:
																TRACE(infomessage);
	//MessageBox(NULL,infomessage,"Failed", MB_OK);
	return false;


}

