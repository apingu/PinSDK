//
//
//
//
//
//
//
//
//
//

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <graphic2d.h>
#include <pf-ziplib.h>
//
#include <game\HPIN.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "save_pin.h"


//#include "convert_func.h"

unsigned int Alpha_Gradation( int alpha, int gradation );


void WritePinHeader( FILE*           fh,
                     unsigned short  SrcWidth,
                     unsigned short  SrcHeight,
                     unsigned int    AlphaSize,
                     RECT            ImageRect,
                     //RECT            ObjectRect,
                     bool            isCompress,
                     PixelFormat mode = PixelFormat::R5G5B5,
                     unsigned short  AdtumMarkX = 0, unsigned short AdtumMarkY = 0,
                     byte_t Act = 0, byte_t Direction = 0, byte_t Step = 0 );


RECT  MinScope ( byte_t** SrcImage,
                 int Width, int Height,
                 int pixel_byte_size,
                 dword_t ColorKeys
               );                   //compute this min scope


void  TrimScope( byte_t** SrcImage,
                 int Width, int Height,
                 int pixel_byte_size,
                 RECT TrimRect
               );   //cut to min scope


void Reduce_Alpha( byte_t** SrcImage, int Width, int Height, int Pixel_Bit_size, int gradation = 256 );



unsigned int  Add_256_Scope( byte_t** SrcImage,
                             int Width, int Height,
                             int Pixel_Bit_Size,
                             int ImagePixel
                           );


UINT Switch_Pixel_Bit_Size( PixelFormat mode );

bool SaveCheckVaildAlphaBuffer( const byte_t* pa, size32_t la )
{
    if( pa == NULL )
    { return false; }
    div_t alpha_mod_64 = div( la, 8 );
    byte_t* pCheck = ( byte_t* )pa;
    bool balpha = false;
    for( int i = 0; i < alpha_mod_64.quot; i++ )
    {
        if( ( *( qword_t* )pCheck ) != 0XFFFFFFFFFFFFFFFF )
        {
            return true;
        }
        pCheck += 8;
    }
    for( int i = 0; i < alpha_mod_64.rem; i++ )
    {
        if( ( *pCheck ) != 0XFF )
        {
            return true;
        }
        pCheck += 1;
    }
    return false;
}


int SavePIN( FILE* fh,
             unsigned char* lpImage,
             unsigned char* lpAlpha,
             PixelFormat    pixelFmt,
             bool           isTrim,
             int            gradation,
             bool           isCompress )
{
    byte_t* Image = NULL;
    byte_t* Alpha = NULL;
    int pixel_byte_size = Switch_Pixel_Bit_Size( pixelFmt ) >> 3;
    //int tgaMode         = Switch_Pixel_Format( pixelFmt );
    if( lpImage == NULL )
    {
        return -10;
    }
    int IWidth  = *( word_t* )( lpImage );
    int IHeight = *( word_t* )( lpImage + 2 );
    Image = new byte_t[IWidth * IHeight];
    unsigned int alphasize = 0;
    //Image+=4;
    memcpy( Image, ( lpImage + 4 ), ( IWidth * IHeight ) );
    //if(Image != 0)
    //delete [] Image;
    /*
    dword_t NormPoint=0;
    switch( pixel_byte_size )
    {
    case 1: NormPoint = *(byte_t*)Image; break;
    case 2: NormPoint = *(WORD*)Image; break;
    case 3: //NormPoint = (POINT24*)ImagePtr; break;
    case 4: NormPoint = *(dword_t*)Image; break;
    }
    */
    //LG::Rect ObjectRect = MinScope(&Image, (IWidth / pixel_byte_size), IHeight, pixel_byte_size, NormPoint );
    //LG::Rect ObjectRect( 0, 0, IHeight, IWidth );
    LG::Rect ImageRect( 0, 0, ( IWidth / pixel_byte_size ) - 1, IHeight - 1 );
    //unsigned char* Alpha = 0;
    if( lpAlpha == NULL ) //沒有Alpha
    {
        //ADD_MSG("Warning : BMP file have no alpha!!!");
        if( isTrim ) //要裁切
        {
            dword_t NormPoint = 0;
            switch( pixel_byte_size )
            {
            case 1:
                NormPoint = *( byte_t* )Image;
                break;
            case 2:
                NormPoint = *( WORD* )Image;
                break;
            case 3: //NormPoint = (POINT24*)ImagePtr; break;
            case 4:
                NormPoint = *( dword_t* )Image;
                break;
            }
            ImageRect = MinScope( &Image, ( IWidth / pixel_byte_size ), IHeight, pixel_byte_size, NormPoint );
            //ImageRect = ObjectRect;
            TrimScope( &Image, ( IWidth / pixel_byte_size ), IHeight, pixel_byte_size, ImageRect ); //裁減影像
        }
    }
    else
    {
        //int Result  = LoadBMP( fa, &Alpha, PIXELFMT_8 );
        int AWidth  = *( word_t* )( lpAlpha );
        int AHeight = *( word_t* )( lpAlpha + 2 );
        Alpha = new byte_t[AWidth * AHeight];
        memcpy( Alpha, ( lpAlpha + 4 ), ( AWidth * AHeight ) );
        if( SaveCheckVaildAlphaBuffer( Alpha, AWidth * AHeight ) )
        {
            if( isTrim ) //要裁切
            {
                dword_t AlphaNormPoint = ( *( ( dword_t* )Alpha ) );
                ImageRect = MinScope( &Alpha, AWidth, AHeight, 1, AlphaNormPoint );
                TrimScope( &Alpha, AWidth, AHeight, 1, ImageRect ); //裁減Alpha
                TrimScope( &Image, ( IWidth / pixel_byte_size ), IHeight, pixel_byte_size, ImageRect ); //裁減影像
            }
            Reduce_Alpha( &Alpha,                             //Alpha降階
                          ( ( ImageRect.right - ImageRect.left ) + 1 ),
                          ( ( ImageRect.bottom - ImageRect.top ) + 1 ),
                          8, gradation );
            alphasize = ImageRect.width() * ImageRect.height();
        }
        else
        {
            SAFE_DELETE_ARRAY( Alpha );
        }
    }
    /*
    ObjectRect.top = ObjectRect.top - ImageRect.top;
    ObjectRect.left = ObjectRect.left - ImageRect.left;
    ObjectRect.right = ObjectRect.right - ImageRect.left;
    ObjectRect.bottom = ObjectRect.bottom - ImageRect.top;
    */
    //fclose( fd );
    int ImageSize = ( ( ImageRect.width() * pixel_byte_size ) * ImageRect.height() );
    int bufsize = ImageSize + alphasize;
    ubyte_t* buf = new ubyte_t[bufsize];
    if( alphasize > 0 )
    { memcpy( buf, Alpha, alphasize ); }
    if( ImageSize > 0 )
    { memcpy( ( buf + alphasize ), Image, ImageSize ); }
    SAFE_DELETE_ARRAY( Alpha );
    SAFE_DELETE_ARRAY( Image );
    if( isCompress )
    {
        //int bufsize = filelength(fileno(pf)) - sizeof(PINHEADER);
        //fseek(pf, sizeof(PINHEADER), SEEK_SET);
        //fread(buf, 1, bufsize, pf);
        size32_t pzBufLen = pfziplib::pzip::GetMaxCompressLen( bufsize );
        ubyte_t* pzBuf = new ubyte_t[pzBufLen];
        pfziplib::pzip::GetMaxCompressLen( bufsize );
        int pzBufsize = pfziplib::pzip::Compress_BToB( buf, bufsize, pzBuf, pzBufLen );
        if( pzBufsize == 0 )
        {
            //fclose(pf);
            //goto UNCOMPRESS;
            isCompress = false;
            //return 0;
        }
        else
        {
            //PinHeader.ApCompressMode = 'Z';
            //FILE *pzf;
            //if((pzf = fopen(Pinzpath, "wb")) == NULL)
            //return OPEN_FILE_FAIL;
            //fwrite(&PinHeader, 1, sizeof(PINHEADER), pzf);
            //fwrite(pzBuf     , 1, pzBufsize, pzf);
            SAFE_DELETE_ARRAY( buf );
            buf = pzBuf;
            bufsize = pzBufsize;
            //SAFE_DELETE_ARRAY( pzBuf );
            //fclose(pf);
            //fclose(pzf);
        }
    }
    WritePinHeader( fh,                        //寫入檔頭
                    ( ( IWidth ) / pixel_byte_size ), IHeight,
                    alphasize,
                    ImageRect,
                    //ObjectRect,
                    isCompress,
                    pixelFmt );
    fwrite( buf, bufsize, 1, fh );
    pfziplib::FreeZipBuffer( buf );
    //SAFE_DELETE_ARRAY(buf);
    /*
    if( fa != 0 ) fclose(fa);
    if( fd != 0 ) fclose(fd);
    if( fh != 0 ) fclose(fh);
    
    delete [] Alpha;
    delete [] Image;
    
    ADD_MSG("Conversion successful !!!");
    ADD_MSG("=========================");
    */
    return 1;
}


/*
//=============================================================================
// NAME:
// DESC: write pin image file header
//=============================================================================
int  WriteExtraData( FILE *fh,
                     unsigned short AdtumMarkX,
                     unsigned short AdtumMarkY,
                     unsigned char  Act,
                     unsigned char  Direction,
                     unsigned char  Step )
{
    int curpos = ftell(fh);

    PINHEADER myheader;
    memset(&myheader, 0, sizeof(myheader));

    fread( &myheader, sizeof(PINHEADER), 1, fh );
    if( strcpy( myheader.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    myheader.ApExtInfo.ExtAct        =  Act;
    myheader.ApExtInfo.ExtDirection  =  Direction;
    myheader.ApExtInfo.ExtStep       =  Step;

    ushort_t Left   = (ushort_t)myheader.ApSrcImage.SrcOffsetX;
    ushort_t Top    = (ushort_t)myheader.ApSrcImage.SrcOffsetY;

    myheader.ApRelativeX             = AdtumMarkX - Left;
    myheader.ApRelativeY             = AdtumMarkY - Top;

    fseek( fh, curpos, SEEK_SET );
    fwrite( &myheader, sizeof(PINHEADER), 1, fh );
    fseek( fh, curpos, SEEK_SET );
    return PINERR_OK;
}

int  WriteExtraZLine    ( FILE *fh,
                          unsigned short zx0,
                          unsigned short zy0,
                          unsigned short zx1,
                          unsigned short zy1 )
{
    int curpos = ftell(fh);
    PINHEADER f_header;
    fread( &f_header, sizeof(PINHEADER), 1, fh );

    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApZLineX0 = zx0;
    f_header.ApZLineY0 = zy0;
    f_header.ApZLineX1 = zx1;
    f_header.ApZLineY1 = zy1;

    fseek( fh, curpos, SEEK_SET );
    fwrite( &f_header, 1, sizeof(PINHEADER), fh );
    fseek( fh, curpos, SEEK_SET );
    //寫入磁碟圖檔
    //fseek(f, m_file_offset + 17, SEEK_SET); fwrite(&RelativeX, 1, sizeof(short), f);
    //fseek(f, m_file_offset + 19, SEEK_SET); fwrite(&RelativeY, 1, sizeof(short), f);
    //fclose( pf() );
    //pf.close();

    return PINERR_OK;
}

int  WriteExtraOffset( FILE *fh,
                       unsigned short x,
                       unsigned short y )
{
    int curpos = ftell(fh);
    PINHEADER f_header;
    fread( &f_header, sizeof(PINHEADER), 1, fh );

    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApRelativeX = x - f_header.ApSrcImage.SrcOffsetX;
    f_header.ApRelativeX = y - f_header.ApSrcImage.SrcOffsetY;

    fseek( fh, curpos, SEEK_SET );
    fwrite( &f_header, 1, sizeof(PINHEADER), fh );
    fseek( fh, curpos, SEEK_SET );

    return PINERR_OK;
}

int  WriteExtraHitPoint ( FILE *fh,
                          unsigned short x,
                          unsigned short y )
{
    int curpos = ftell(fh);
    PINHEADER f_header;
    fread( &f_header, sizeof(PINHEADER), 1, fh );

    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.APHitPointX0 = x;
    f_header.APHitPointY0 = y;


    fseek( fh, curpos, SEEK_SET );
    fwrite( &f_header, 1, sizeof(PINHEADER), fh );
    fseek( fh, curpos, SEEK_SET );


    return PINERR_OK;
}

int  WriteExtraRect     ( FILE *fh,
                          unsigned short shape,
                          unsigned short tall,
                          unsigned short top,
                          unsigned short left,
                          unsigned short right,
                          unsigned short bottom )
{
    int curpos = ftell(fh);
    PINHEADER f_header;
    fread( &f_header, sizeof(PINHEADER), 1, fh );

    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApAmbitShape  = shape;
    f_header.ApAmbitTall   = tall;
    f_header.ApRecttop0    = top;
    f_header.ApRectleft0   = left;
    f_header.ApRectright0  = right;
    f_header.ApRectbottom0 = bottom;

    fseek( fh, curpos, SEEK_SET );
    fwrite( &f_header, 1, sizeof(PINHEADER), fh );
    fseek( fh, curpos, SEEK_SET );

    return PINERR_OK;
}
*/

//=============================================================================
// NAME:
// DESC: write pin image file header
//=============================================================================
void WritePinHeader( FILE*    fh,
                     ushort_t  SrcWidth, ushort_t SrcHeight,
                     uint_t    AlphaSize,
                     RECT     ImageRect,
                     //RECT     ObjectRect,
                     bool     isCompress,
                     PixelFormat mode,
                     ushort_t  AdtumMarkX, ushort_t AdtumMarkY,
                     byte_t Act, byte_t Direction, byte_t Step )
{
    /* move to destination file start address */
    static PINHEADER myheader;
    ushort_t Top = 0, Bottom = 0, Left = 0, Right = 0;
    memset( &myheader, 0XFF, sizeof( myheader ) );
    fseek( fh, 0, SEEK_SET );
    strcpy( myheader.ApName, "PIN" );
    myheader.ApAlphaOffset           =  sizeof( myheader );
    myheader.ApSrcImage.SrcOffsetX   = ( ushort_t )ImageRect.left;          //左上角X
    myheader.ApSrcImage.SrcOffsetY   = ( ushort_t )ImageRect.top;           //左上角Y
    Left   = ( ushort_t )ImageRect.left;
    Top    = ( ushort_t )ImageRect.top;
    Right  = ( ushort_t )ImageRect.right;
    Bottom = ( ushort_t )ImageRect.bottom;
    myheader.ApExtInfo.ExtAct        =  Act;
    myheader.ApExtInfo.ExtDirection  =  Direction;
    myheader.ApExtInfo.ExtStep       =  Step;
    myheader.ApSrcImage.SrcWidth     = ( ushort_t )SrcWidth;
    myheader.ApSrcImage.SrcHeight    = ( ushort_t )SrcHeight;
    myheader.ApImageOffset           = sizeof( myheader ) +
                                       AlphaSize; //filelength(fileno(fh));//(sizeof(myheader) //+ AlphaSize);//(((AlphaRect.right - AlphaRect.left) * (mode/2)) * (AlphaRect.bottom - AlphaRect.top)));//
    myheader.ApPixelMode             = (word_t)mode;
    if( isCompress )
    { myheader.ApCompressMode      = 'Z'; }
    else
    { myheader.ApCompressMode      = '0'; }
    myheader.ApAmbitShape            = _P_RECT_;
    myheader.ApVar                   = 10;
    myheader.ApPinWidth              = ( Right  - Left ) + 1;
    myheader.ApPinHeight             = ( Bottom - Top ) + 1;
    myheader.ApAmbitTall             = 0;
    myheader.ApRelativeX             = AdtumMarkX - Left;
    myheader.ApRelativeY             = AdtumMarkY - Top;
    myheader.ApRecttop0              = myheader.ApSrcImage.SrcOffsetY;
    myheader.ApRectleft0             = myheader.ApSrcImage.SrcOffsetX;
    myheader.ApRectright0            = myheader.ApSrcImage.SrcOffsetX + myheader.ApPinWidth;
    myheader.ApRectbottom0           = myheader.ApSrcImage.SrcOffsetY + myheader.ApPinHeight;
    fwrite( &myheader, sizeof( myheader ), 1, fh );
}


//=============================================================================
//    Name : Alpha_Gradation()
//    Desc :
//=============================================================================
unsigned int Alpha_Gradation( int alpha, int gradation )
{
    int r   = 256 / gradation;
    int a_r = alpha / r;
    return r * a_r;
}


//=============================================================================
//    Name : Reduce_Alpha()
//    Desc :
//=============================================================================
void Reduce_Alpha( byte_t** SrcImage, int Width, int Height, int Pixel_Bit_size, int gradation )
{
    byte_t* Src_Ptr = ( *SrcImage );
    int mem_image_size = Width * Height;
    int alpha_value = 0;
    for( int i = 0 ; i < mem_image_size; i++ )
    {
        switch( Pixel_Bit_size )
        {
        case 8:
            alpha_value = *( byte_t* )Src_Ptr;
            break;
        case 16:
            alpha_value = *( word_t* )Src_Ptr;
            break;
        case 24:
            break;
        }
        if( ( alpha_value <= 255 ) && ( alpha_value >= 248 ) )
        { alpha_value = 255; }
        else if( ( alpha_value <= 7 ) && ( alpha_value >= 0 ) )
        { alpha_value = 0; }
        else
        { alpha_value = Alpha_Gradation( alpha_value, gradation ); }
        switch( Pixel_Bit_size )
        {
        case 8:
            *( byte_t* )Src_Ptr = ( byte_t )alpha_value;
            Src_Ptr = Src_Ptr + 1;
            break;
        case 16:
            *( word_t* )Src_Ptr = ( word_t )alpha_value;
            Src_Ptr = Src_Ptr + 2;
            break;
        case 24:
            break;
        }
    }
}


//=============================================================================
//
//  linear add 256 image
//=============================================================================
unsigned int Add_256_Scope( byte_t** SrcImage, int Width, int Height, int Pixel_Bit_Size, int ImagePixel )
{
    byte_t  PixelInfo;
    int   forword = 0 , Point = 0;
    int   pixel_byte_size = Pixel_Bit_Size / 8;
    unsigned int FileSize = 0;
    word_t ColorCount  = 0;
    word_t PointsCount = 0;
    byte_t* SrcPtr    = ( *SrcImage );
    byte_t* SwapImage = new byte_t[( ( Width * pixel_byte_size )  * Height ) * 2];
    memset( SwapImage, 0, ( Width * pixel_byte_size )  * Height );
    if( SwapImage == 0 )
    { return -1; }
    byte_t* SwapPtr   = SwapImage;
    int i = 0, j = 0;
    for( i = 0 ; i < Height; i++ )
    {
        SrcPtr = ( *SrcImage ) + ( ( Width * pixel_byte_size ) * i );
        forword = *( byte_t* )SrcPtr; ///////////////////////////////////
        ColorCount      = 0;
        PointsCount     = 0;
        for( j = 0 ; j < Width ; j++ )
        {
            Point  = *( byte_t* )SrcPtr; ///////////////////////////
            SrcPtr = SrcPtr + pixel_byte_size;
            if( Point == forword )
            {
                ColorCount++;
                forword = Point;
                continue;
            }
            else
            {
                PixelInfo = forword;
                *SwapPtr  = PixelInfo;//write
                SwapPtr   = SwapPtr + 1;
                if( ImagePixel == 16 )
                { ColorCount = ColorCount * 2; }
                else if( ImagePixel == 24 )
                { ColorCount = ColorCount * 3; }
                else if( ImagePixel == 32 )
                { ColorCount = ColorCount * 4; }
                PointsCount = PointsCount + ColorCount ;//檢查總點轉的byte_ts數
                *( word_t* )SwapPtr = ( word_t )ColorCount;
                SwapPtr  = SwapPtr + 2;                  //write pixel count 2 byte_ts
                FileSize = FileSize + 3;
                Pf_INFO( "Add_256_Scope: %d %d  i%d j%d\n", PixelInfo, ColorCount, i, j );
                forword = Point;
                ColorCount = 1;
                //sprintf(infomessage, "-------------------\n");                        OUTDEBUG(infomessage);
            }
        }
        if( ColorCount != 0 )
        {
            PixelInfo = forword;
            *SwapPtr  = PixelInfo;//write
            SwapPtr   = SwapPtr + 1;
            if( ImagePixel == 16 )
            { ColorCount = ColorCount * 2; }
            else if( ImagePixel == 24 )
            { ColorCount = ColorCount * 3; }
            else if( ImagePixel == 32 )
            { ColorCount = ColorCount * 4; }
            PointsCount = PointsCount + ColorCount ;//檢查總點轉的byte_ts數
            ( *( word_t* )SwapPtr ) = ( word_t )ColorCount;
            //memcpy(SwapPtr, &ColorCount, sizeof(word_t));
            SwapPtr  = SwapPtr  + 2;                  //write pixel count 2 byte_ts
            FileSize = FileSize + 3;
        }
        if( PointsCount != ( unsigned short )( Width ) * ( ImagePixel / 8 ) )
        {
            //char output[256];
            //sprintf( output, " I'm sorry , I have bug for change file failure in colscount %d   rowscount %d \n", j, i ); //                        ERRORMSG(OutDebugString);
            //Pf_ERROR( output, "Conversion" );
            Pf_ERROR( "Add_256_Scope: I'm sorry , I have bug for change file failure in colscount %d  rowscount %d \n", j, i );
            //ADD_MSG( " I'm sorry , I have bug for change file failure in colscount ? rowscount ?" ); ///////////////////////
            //END_PROGRAM(0);
            //goto OnError;
        }
    }
    delete [] *SrcImage;
    ( *SrcImage ) = new byte_t[FileSize];;
    memcpy( *SrcImage, SwapImage, FileSize );
    delete [] SwapImage;
    return FileSize;
}


//=============================================================================
//
//
//=============================================================================
RECT MinScope( byte_t** SrcImage, int Width, int Height, int pixel_byte_size, dword_t ColorKeys )
{
    RECT MinRect;
    if( ( *SrcImage ) == 0 )
    {
        MinRect.top    = 0;
        MinRect.bottom = 0;
        MinRect.right  = 0;
        MinRect.left   = 0;
        return  MinRect;
    }
    int  NormPoint = ColorKeys;
    int  ReadPoint;
    //int  pixel_byte_size  = Pixel_Bit_Size/8;
    //int  firstPoint = 1;
    byte_t* ImagePtr  = ( *SrcImage );
    MinRect.top    = Height - 1;
    MinRect.left   = Width  - 1;
    MinRect.right  = 0;
    MinRect.bottom = 0;
    for( int Row = 0; Row < Height; Row++ )
    {
        for( int Col = 0; Col < Width; Col++ )
        {
            switch( pixel_byte_size )
            {
            case 1:
                ReadPoint = *( byte_t* )ImagePtr;
                break;
            case 2:
                ReadPoint = *( word_t* )ImagePtr;
                break;
            case 3:
                //ReadPoint = (POINT24*)ImagePtr;
                break;
            case 4:
                ReadPoint = *( dword_t* )ImagePtr;
                break;
            }
            if( ReadPoint != NormPoint )
            {
                if( Row < MinRect.top )
                { MinRect.top    = Row; }
                if( Row > MinRect.bottom )
                { MinRect.bottom = Row; }
                if( Col < MinRect.left )
                { MinRect.left   = Col; }
                if( Col > MinRect.right )
                { MinRect.right  = Col; }
            }
            ImagePtr = ImagePtr + pixel_byte_size;
        }
    }
    if( ( MinRect.left >= MinRect.right ) ||
            ( MinRect.top >= MinRect.bottom ) )
    {
        MinRect.top    = 0;
        MinRect.left   = 0;
        MinRect.right  = Width  - 1;
        MinRect.bottom = Height - 1;
    }
    return MinRect;
}

//=============================================================================
//  cut image
//
//=============================================================================
void TrimScope( byte_t** SrcImage, int Width, int Height, int pixel_byte_size, RECT TrimRect )
{
    if( ( *SrcImage ) == 0 )
    { return; }
    //int pixel_byte_size = (Pixel_Bit_Size / 8);
    int RealWidth = Width * pixel_byte_size;
    int MinWidth  = ( ( ( TrimRect.right  - TrimRect.left ) + 1 ) * pixel_byte_size );
    int MinHeight = ( ( TrimRect.bottom - TrimRect.top ) + 1 );
    byte_t* ImagePtr;
    byte_t* SwapImage = new byte_t[MinWidth * MinHeight];
    byte_t* SwapImagePtr = SwapImage;
    ImagePtr  = ( *SrcImage ) + ( RealWidth * TrimRect.top ) + ( TrimRect.left * pixel_byte_size );
    for( int Row = TrimRect.top; Row <= TrimRect.bottom; Row++ )
    {
        memcpy( SwapImagePtr, ImagePtr, MinWidth );
        ImagePtr     = ImagePtr     + RealWidth;
        SwapImagePtr = SwapImagePtr + MinWidth;
    }
    delete [] *SrcImage;
    ( *SrcImage ) = SwapImage;
}


//=============================================================================
//  cut image
//
//=============================================================================

UINT Switch_Pixel_Bit_Size( PixelFormat mode )
{
    switch( mode )
    {
    case PixelFormat::R8G8B8:
        return 24;
    case PixelFormat::R8G8B8A8:
        return 32;
    case PixelFormat::A8R8G8B8:
        return 32;
    default:
        return 16;
    }
}

