#include "convert_image.h"
#include "convert_func.h"
#include "load_bmp.h"
#include "save_pin.h"
#include <pf-debug.h>

//std::vector<const char*> outdegmessage;



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




///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
int BMPToPin( const char*    alphafilepath,
              const char*    srcfilepath,
              const char*    destfilepath,
              PixelFormat       mode,
              bool           isTrim,
              int            gradation,
              bool           isCompress )
{
    int tgaMode  = Switch_Pixel_Format( mode );
    int Result = 0;
    //BOOL no_alpha = FALSE;
    FILE* fh = 0;
    FILE* fa = 0;
    FILE* fd = 0;
    unsigned char* lpImage = NULL;
    unsigned char* lpAlpha = NULL;
    if( ( fd = fopen( destfilepath, "wb" ) ) == NULL )
    {
        Pf_ERROR( "BMPToPin: Open write file fail ...." );
        return -2;
    }
    if( ( fh = fopen( srcfilepath, "rb" ) ) == NULL )
    {
        Pf_ERROR( "BMPToPin: Open source file fail ...." );
        return -4;
    }
    else
    {
        Result = LoadBMP( fh, &lpImage, tgaMode );
        if( Result != BMPERR_OK ) ///////////////////////
        { return CONV_LOAD_MAP_FAIL; }
        if( fh != 0 ) { fclose( fh ); }
        if( lpImage == NULL )
        {
            Pf_ERROR( "BMPToPin: BMP file error ...." );
            return -10;
        }
    }
    if( ( fa = fopen( alphafilepath, "rb" ) ) == NULL )
    {
        Pf_WARNING( "BMPToPin: open alpha file failure, continue will be no alpha image!!" );
    }
    else
    {
        Result  = LoadBMP( fa, &lpAlpha, PIXELFMT_8 );
        if( fa != 0 ) { fclose( fa ); }
    }
    if( SavePIN( fd,
                 lpImage,
                 lpAlpha,
                 mode,
                 isTrim,
                 gradation,
                 isCompress ) != 1 )
    {
        Pf_ERROR( "BMPToPin: Bmp convert to pin file failure ...." );
    }
    if( fd != 0 ) { fclose( fd ); }
    delete [] lpAlpha;
    delete [] lpImage;
    Pf_INFO( "Conversion successful !!!" );
    Pf_INFO( "=========================" );
    return 1;
}


/*


BITMAPFILEHEADER bif, abif;
BITMAPINFOHEADER bih, abih;

//int  //messageID;

int BitMapWidth;
int AlphaWidth;

unsigned int  rowscount = 0, colscount = 0;
unsigned short int colorcount = 0 , pointscount = 0;


byte_t AlphaPoint;
byte_t putinfo;

FILE*        OpenWritefile     (const char *destfilepath);
FILE*        OpenSourceBmp     (const char *srcfilepath);
FILE*        OpenSourceAlpha   (const char *alphafilepath);

RECT         AnalyzeAlpha      (FILE *alphaf);//(byte_t *ptrAlpha);
unsigned int WriteAlphaMask    (RECT AlphaRect, int PixelMode, FILE *alphaf, FILE *destfptr, int gradation = 256);

RECT         AnalyzeBmp        (FILE *bmpf);//(byte_t *ptrBMP);
int          WriteImage        (RECT MapRect, int PixelMode, FILE *bmpf, FILE *destfptr);



FILE* srcfptr  = OpenSourceBmp(srcfilepath);
if( srcfptr == 0 )
    return -1;
FILE* destfptr = OpenWritefile(destfilepath);
if( destfptr == 0)
    return -1;
FILE* asrcfptr = OpenSourceAlpha(alphafilepath);


RECT MapRect  ;
RECT AlphaRect;
RECT ObjectRect;

MapRect.top    = 0;
MapRect.left   = 0;
MapRect.right  = bih.biWidth;
MapRect.bottom = bih.biHeight;


ObjectRect = AnalyzeBmp(srcfptr);

if((isTrim) && (asrcfptr == 0))//要裁切 沒有Alpha
{
    MapRect.top = ObjectRect.top;
    MapRect.left = ObjectRect.left;
    MapRect.right = ObjectRect.right;
    MapRect.bottom = ObjectRect.bottom;
}

unsigned int AlphaSize = 0;

if((isTrim) && (asrcfptr != 0))//要裁切 有Alpha
{
   AlphaRect = AnalyzeAlpha(asrcfptr);
   MapRect.top = AlphaRect.top;
   MapRect.left = AlphaRect.left;
   MapRect.right = AlphaRect.right;
   MapRect.bottom = AlphaRect.bottom;

   fseek(destfptr, sizeof(PINHEADER), SEEK_SET);
   AlphaSize = WriteAlphaMask(AlphaRect, mode, asrcfptr, destfptr, gradation);
   fclose(asrcfptr);
}

ObjectRect.top = ObjectRect.top - MapRect.top;
ObjectRect.left = ObjectRect.left - MapRect.left;
ObjectRect.right = ObjectRect.right - MapRect.left;
ObjectRect.bottom = ObjectRect.bottom - MapRect.top;


fseek(destfptr, 0, SEEK_SET);
WritePinHeader(destfptr,
               (unsigned short)bih.biWidth,  //原來的寬
               (unsigned short)bih.biHeight,
                AlphaSize,
                MapRect,                     //壓縮後的長寬
                ObjectRect,                  //圖形的範圍
                mode,
                AdtumMarkX, AdtumMarkY,
                Act, Direction, Step);

fseek(destfptr, sizeof(PINHEADER) + AlphaSize, SEEK_SET);
WriteImage(MapRect, mode, srcfptr, destfptr);

fclose(destfptr);
fclose(srcfptr);
                                sprintf( OutDebugString, "Change file successful !!!!" );
                                AddResultMessage( "Change file successful !!!!" );
                                AddResultMessage( "==============================" );

return 1;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
FILE* OpenWritefile(const char *destfilepath)
{
FILE* destfptr;
//===============================================================================================
//open writed file
if((destfptr = fopen(destfilepath, "wb")) == NULL)
{
    sprintf(OutDebugString,"open dest file [%s] failure!", destfilepath);
    ERRORMSG(OutDebugString);
    AddResultMessage( "open dest file failure!" );
    ////messageID = 2;
    END_PROGRAM(0);
}

return destfptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
FILE* OpenSourceBmp(const char *srcfilepath)
{
//===============================================================================================
//open source bmp file
FILE* srcfptr;
if((srcfptr = fopen(srcfilepath, "rb")) == NULL)
{
    sprintf( OutDebugString,"open source file [%s] failure !!!", srcfilepath );
    ERRORMSG( OutDebugString );
    AddResultMessage( "open source file failure !!!" );     ////messageID = 4;
    END_PROGRAM(0);
    //goto OnError;
}
//read source bmp file header
if(fread(&bif,1,sizeof(bif),srcfptr) != sizeof(bif))
{
    sprintf( OutDebugString,"read source file [%s] file header failure !!!", srcfilepath );
    fclose( srcfptr );
    ERRORMSG(OutDebugString);
    AddResultMessage( "read source file file header failure !!!" );
    END_PROGRAM(0);
    //goto OnError;
}
if(fread(&bih,1,sizeof(bih),srcfptr) != sizeof(bih))
{
    sprintf( OutDebugString,"read soure file [%s] header failure !!!", srcfilepath );
    fclose( srcfptr );
    ERRORMSG(OutDebugString);
    AddResultMessage( "read soure file header failure !!!" );
    END_PROGRAM(0);
    //goto OnError;
}

// check for source bmp file pixel format
if((bih.biWidth > 65535) || (bih.biHeight > 65535))
{
    sprintf( OutDebugString,"picture width or height must less than 65535 !!!" );
    ERRORMSG( OutDebugString );
    AddResultMessage( "picture width or height must less than 65535 !!!" );
    END_PROGRAM(0);
    //goto OnError;
}
if(bih.biBitCount != 24)
{
    sprintf(OutDebugString,"Source file [%s] only support for 24 bits color now !!!", srcfilepath);
    ERRORMSG(OutDebugString);
    AddResultMessage( "Source file only support for 24 bits color now !!!" );
    ////messageID = 7;
    END_PROGRAM(0);
    //goto OnError;
}

BitMapWidth = ((((bih.biWidth) * (bih.biBitCount  / 8)) + 3)/4) * 4;
return srcfptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
FILE* OpenSourceAlpha(const char *alphafilepath)
{
    //open source alpha file
    FILE* asrcfptr;
    if((asrcfptr = fopen(alphafilepath, "rb")) == NULL)
    {
        sprintf(OutDebugString,"open alpha [%s] file failure, continue will be no alpha image!!", alphafilepath);
        AddResultMessage( "Warning : open alpha file failure, continue will be no alpha image!!" );
        //messageID = 3;
        //int result = MessageBox(NULL, OutDebugString, "Open Alpha error !!!", MB_OKCANCEL);
        //if(result == IDCANCEL)
        //  return -2;
        //else
        return 0;

    }
        //read alpha bmp file header
        if(fread(&abif,1,sizeof(abif),asrcfptr) != sizeof(abif))
        {
            sprintf(OutDebugString, "read alpha file header failure !!!");
            AddResultMessage( "Warning : read alpha file header failure, continue will be no alpha image !!!" );
            fclose(asrcfptr);
            return 0;
            //goto OnError;
        }

        if(fread(&abih,1,sizeof(abih),asrcfptr) != sizeof(abih))
        {
            sprintf(OutDebugString,"read alpha file [%s] header failure, continue will be no alpha image!!!", alphafilepath);
            AddResultMessage( "Warning : read alpha file header failure, continue will be no alpha image!!!" );
            fclose(asrcfptr);
            return 0;
            //goto OnError;
        }

        if((abih.biWidth > 65535) || (abih.biHeight > 65535))
        {
            sprintf(OutDebugString, "picture width or height must less than 65535 !!!");
            AddResultMessage( "Warning : picture width or height must less than 65535, continue will be no alpha image !!!" );
            return 0;
            //goto OnError;
        }


        if(abih.biBitCount != 8)
        {
            sprintf(OutDebugString, "Alpha file %s only support for 256 color now, continue will be no alpha image !!!", alphafilepath);
            AddResultMessage( "Warning : Alpha file only support for 256 color now, continue will be no alpha image !!!" );
            //messageID = 6;
            //int result = MessageBox(NULL, OutDebugString, "Alpha format error !!!", MB_OKCANCEL);
            //if(result == IDCANCEL)
            //    return -2;
            //else
                return 0;
            //goto OnError;
        }

    //===============================================================================================


    //END_PROGRAM(1);
    //===============================================================================================
    AlphaWidth  = ((((abih.biWidth) * (abih.biBitCount  / 8)) + 3)/4) * 4;
    return asrcfptr;
}


//==============================================================================
//                         分析讀入的bmp檔
//
//==============================================================================
RECT AnalyzeBmp(FILE* srcfptr)
{
    Point24 AddPoint;
    Point24 BmpPoint;
    RECT    ImageRect;
    ImageRect.top = bih.biHeight;
    ImageRect.left = bih.biWidth;
    ImageRect.right  = 0;
    ImageRect.bottom = 0;

    FLAG    first;

    rowscount   = bih.biHeight;
    colscount   = 0;
    first       = 0;

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
                if(((bih.biHeight - rowscount) < (unsigned int)ImageRect.top) || (first == 0))
                    ImageRect.top    = (bih.biHeight - rowscount);

                if(((bih.biHeight - rowscount) > (unsigned int)ImageRect.bottom) || (first == 0))
                    ImageRect.bottom = (bih.biHeight - rowscount);

                if((colscount < (unsigned int)ImageRect.left) || (first == 0))
                    ImageRect.left   = colscount;

                if((colscount > (unsigned int)ImageRect.right) || (first == 0))
                    ImageRect.right  = colscount;

                first = 1;
            }
            colscount ++;

        }
        rowscount --;
    }
return ImageRect;
}

/////////////////////////////////////////////////////////////////////////////////
//                         分析讀入的Alpha檔
RECT AnalyzeAlpha(FILE* asrcfptr)
{
    Point256 AddaPoint;
    FLAG    afirst;

    RECT AlphaImageRect;
    AlphaImageRect.top = abih.biHeight;
    AlphaImageRect.left = abih.biWidth;
    AlphaImageRect.bottom = 0;
    AlphaImageRect.right  = 0;

    rowscount   = abih.biHeight;
    colscount   = 0;
    afirst      = 0;

    fseek(asrcfptr,abif.bfOffBits + ((rowscount - 1) * abih.biWidth),SEEK_SET);
    fread(&AlphaPoint,sizeof(byte_t),1,asrcfptr);
    AddaPoint = AlphaPoint;

    while(rowscount > 0)
    {
        fseek(asrcfptr,abif.bfOffBits + ((rowscount - 1) * abih.biWidth),SEEK_SET);
        colscount = 0;

        while(colscount < (unsigned int)bih.biWidth)
        {
            fread(&AlphaPoint,sizeof(byte_t),1,asrcfptr);
            if(AlphaPoint != AddaPoint)
            {
                if(((abih.biHeight - rowscount) < (unsigned int)AlphaImageRect.top) || (afirst == 0))
                    AlphaImageRect.top    = (abih.biHeight - rowscount);

                if(((abih.biHeight - rowscount) > (unsigned int)AlphaImageRect.bottom) || (afirst == 0))
                    AlphaImageRect.bottom = (abih.biHeight - rowscount);

                if((colscount < (unsigned int)AlphaImageRect.left) || (afirst == 0))
                    AlphaImageRect.left   = colscount;

                if((colscount > (unsigned int)AlphaImageRect.right) || (afirst == 0))
                    AlphaImageRect.right  = colscount;

                afirst = 1;
            }
            colscount ++;

        }
        rowscount --;
    }
return AlphaImageRect;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//      write Alpha mask information to dest
//
unsigned int WriteAlphaMask(RECT AlphaRect, int PixelMode, FILE *asrcfptr, FILE *destfptr, int gradation)
{
unsigned char forword;
unsigned int  Alphasize =0;

    rowscount = abih.biHeight;

    for(unsigned int i = (unsigned int)AlphaRect.top ; i <= (unsigned int)AlphaRect.bottom ; i++)
    {
        fseek(asrcfptr,abif.bfOffBits + (((abih.biHeight - i) - 1) * AlphaWidth) + AlphaRect.left, SEEK_SET);
        fread(&forword,sizeof(byte_t),1,asrcfptr);
            ///////////////////////////////////////////////////////////////////////////
            if(((int)forword <= 255) && ((int)forword >= 248))
                forword = 255;

            else if(((int)forword <= 7) && ((int)forword >= 0))
                forword = 0;

            forword = Alpha_Gradation(forword, gradation);

            ////////////////////////////////////////////////////////////////////////////

        fseek(asrcfptr,abif.bfOffBits + (((abih.biHeight - i) - 1) * AlphaWidth) + AlphaRect.left, SEEK_SET);
        colorcount      = 0;
                                                                //----Check-------------------------------------------------------------------------
                                                                sprintf(OutDebugString, "=================================\n");     OUTDEBUG(OutDebugString);
                                                                sprintf(OutDebugString, "rowscount : %d\n" , i               );     OUTDEBUG(OutDebugString);
                                                                sprintf(OutDebugString, "=================================\n\n");   OUTDEBUG(OutDebugString);
                                                                //----END Check---------------------------------------------------------------------

        for(unsigned int j = (unsigned int)AlphaRect.left ; j <= (unsigned int)AlphaRect.right ; j++)
        {

            fread(&AlphaPoint,sizeof(byte_t),1,asrcfptr);

            ///////////////////////////////////////////////////////////////////////////
            if(((int)AlphaPoint <= 255) && ((int)AlphaPoint >= 248))
                AlphaPoint = 255;

            else if(((int)AlphaPoint <= 7) && ((int)AlphaPoint >= 0))
                AlphaPoint = 0;

            AlphaPoint = Alpha_Gradation(AlphaPoint, gradation);

            ////////////////////////////////////////////////////////////////////////////
                                                               //sprintf(OutDebugString, "colscount %d\n", colscount);              OUTDEBUG(OutDebugString);
            if(AlphaPoint == forword)
            {
                                                                //sprintf(OutDebugString, "colorkey ==> colorkey\n");               OUTDEBUG(OutDebugString);
                colorcount++;
                forword = AlphaPoint;
                continue;
                                                                //sprintf(OutDebugString, "-------------------\n");                 OUTDEBUG(OutDebugString);
            }

            else
            {
                                                                //sprintf(OutDebugString, "colorkey ==> color\n");                  OUTDEBUG(OutDebugString);
                putinfo = forword;
                fwrite(&putinfo,sizeof(putinfo),1,destfptr);

                if(PixelMode == 16)
                    colorcount = colorcount * 2;

                else if(PixelMode == 24)
                    colorcount = colorcount * 3;

                else if(PixelMode == 32)
                    colorcount = colorcount * 4;

                pointscount = pointscount + colorcount ;//檢查總點轉的bytes數
                fwrite(&colorcount,sizeof(colorcount),1,destfptr);

                forword = AlphaPoint;
                colorcount = 1;
                Alphasize = Alphasize + 3;
                                                                //sprintf(OutDebugString, "-------------------\n");                     OUTDEBUG(OutDebugString);
            }
        }

        if(colorcount != 0)
        {
            putinfo = forword;
            fwrite(&putinfo,sizeof(putinfo),1,destfptr);

            if(PixelMode == 16)
                colorcount = colorcount * 2;

            else if(PixelMode == 24)
                colorcount = colorcount * 3;

            else if(PixelMode == 32)
                 colorcount = colorcount * 4;

            pointscount = pointscount + colorcount ;//檢查總點轉的bytes數
            fwrite(&colorcount,sizeof(colorcount),1,destfptr);
            Alphasize = Alphasize + 3;
        }


                                                                    if(pointscount != (unsigned short)((AlphaRect.right - AlphaRect.left) + 1) * abih.biBitCount/4)
                                                                    {
                                                                        sprintf(OutDebugString, " I'm sorry , I have bug for change file failure in colscount %d   rowscount %d \n",colscount, rowscount);                      ERRORMSG(OutDebugString);
                                                                        AddResultMessage( " I'm sorry , I have bug for change file failure in colscount ? rowscount ?" ); ///////////////////////
                                                                        END_PROGRAM(0);
                                                                        //goto OnError;
                                                                    }
                                                                    sprintf(OutDebugString, "pointscount %d \n",pointscount);                       OUTDEBUG(OutDebugString);
                                                                    pointscount = 0 ;

    }
    fclose(asrcfptr);
    return Alphasize;

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
int WriteImage(RECT MapRect, int PixelMode, FILE *srcfptr, FILE *destfptr)
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/////   write to image
Point24 BmpPoint;
// move to destination file image start address
//fseek(destfptr,64,SEEK_SET);
//fwrite(&MapRect,sizeof(Rect),1,destfptr);

//將資料檔寫入

    for(unsigned int i = (unsigned int)MapRect.top ; i <= (unsigned int)MapRect.bottom ; i++)
    {
                                                                    sprintf(OutDebugString, "=================================\n");     OUTDEBUG(OutDebugString);
                                                                    sprintf(OutDebugString, "rowscount : %d\n", (bih.biHeight - i) );   OUTDEBUG(OutDebugString);
                                                                    sprintf(OutDebugString, "=================================\n");     OUTDEBUG(OutDebugString);

        fseek(srcfptr,bif.bfOffBits + (((bih.biHeight - i) - 1) * BitMapWidth) + (MapRect.left * (bih.biBitCount / 8)), SEEK_SET);
        //lseek(srcfptr,bif.bfOffBits + ((rowscount - 1) * (bih.biWidth * (bih.biBitCount / 8))), SEEK_SET);

        for(unsigned int j = (unsigned int)MapRect.left ; j <= (unsigned int)MapRect.right ; j++)
        {
            fread(&BmpPoint,sizeof(BmpPoint),1,srcfptr);
            //--------------16 bits Pin file---------------------
            switch(PixelMode)
            {
            case 16:
            {
                PINPOINT16 Ppoint;

                //read(srcfptr, &BmpPoint, sizeof(BmpPoint));
                Ppoint = ((BmpPoint.R >> 3) << 11) |  ((BmpPoint.G >> 2) << 5)  | (BmpPoint.B >> 3);
                //Ppoint = ((Ppoint << 8) | (Ppoint >> 8));
                fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
                //write(destfptr,&Ppoint,sizeof(Ppoint));
                break;
            }
            //--------------24 bits Pin file---------------------
            case 24:
            {
                PINPOINT24 Ppoint;

                //read(srcfptr, &BmpPoint, sizeof(BmpPoint));
                Ppoint.R = BmpPoint.R ;
                Ppoint.G = BmpPoint.G ;
                Ppoint.B = BmpPoint.B ;
                fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
                //write(destfptr,&Ppoint,sizeof(Ppoint));
                break;
            }
            //--------------32 bits Pin file---------------------
            case 32:
            {
                PINPOINT32 Ppoint;

                //read(srcfptr, &BmpPoint, sizeof(BmpPoint));
                Ppoint.Reserved = 0   ;
                Ppoint.R        = BmpPoint.R ;
                Ppoint.G        = BmpPoint.G ;
                Ppoint.B        = BmpPoint.B ;
                fwrite(&Ppoint,sizeof(Ppoint),1,destfptr);
                //write(destfptr,&Ppoint,sizeof(Ppoint));
                break;
            }
            default:
            {
                sprintf(OutDebugString,"Pin file dont't support this mode !!!!\n");
                ERRORMSG(OutDebugString);
                AddResultMessage( "Pin file dont't support this mode !!!!" );
                END_PROGRAM(0);
                //goto OnError;
            }
            }

        }

    }
    END_PROGRAM(1);
//////////////////////////////////////////////////////////////////////////////////////////////////
}
*/

