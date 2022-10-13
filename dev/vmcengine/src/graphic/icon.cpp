/*================================================================================================
*
*  Pin Map
*
*
*
*
*                                                       2000 1 20
*
*================================================================================================*/

//01. == C 系統文件/C++ 系統文件
#include <assert.h>
#include <map>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-debug.h>
#include <pf-ziplib.h>
#include <aintersect.inl>
#include <gadget\file_operator.h>
//
#include <hpin.h>
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "icon.h"
//#include <vcm_global.h>

//#if defined(_WINDOWS)
//#pragma comment (lib, "Utility.lib")
//#ifdef _UNICODE
//#pragma comment (lib, "PZLibU.lib")
//#else
//#pragma comment (lib, "PZLib.lib")
//#endif
//#pragma comment (lib, "FileOperator.lib")
//#endif


#ifdef _PSDK
#include <PfStdlib.h>
//#include <PfString.h>
#else
extern const char* mcurrentpath( char* outs, const char* ins, const char* repchar = "/" );
#endif


namespace CPIcon_Singleton
{
    std::map<CPIcon*, int>* g_CPIconPool;
    //std::vector<CPIcon*>*    g_CPIconPool;
    short     g_Alpha_CollisionValue = 128;
    const int g_Flags_PixelsFromNew = 5;  // pixels is set from new by myself(1) or other(0)
};

PixelFormat CPIcon::_VGA_PixelFormat = PixelFormat::R5G6B5; //set by engine
//extern    PixelFormat            _VGA_PixelFormat;
//extern std::set<CPIcon*>*   CPIcon_Singleton::g_CPIconPool=NULL;

CPIcon::OnLoadImage g_LoadImageFunc = NULL;
CPIcon::OnFreeImage g_FreeImageFunc = NULL;



#define   _SAFE_BYTE_TO_BYTET(b)     ((ubyte_t*)(b))

//=============================================================================
//
//
//=============================================================================
CPIcon::CPIcon()
{
    //memset(this, 0, sizeof(tImage));
    width = 0;              // The width of the image in pixels
    height = 0;             // The height of the image in pixels
    depth = 0;
    pixels = 0;             // The image pixel data
    palette = 0;
    format = 0;             // The channels in the image (3 = RGB : 4 = RGBA)
    m_FileType = NONE_TYPE;
    m_FileSize = 0;             //file size
    BytesOfWidth = 0;       //picture width bytes
    ClipOffsetX = 0;        //picture offset x view for paste, it will be used when paste this picture
    ClipOffsetY = 0;        //picture offset y veiw for paste, it will be used when paste this picture
    ClipPosX = 0;
    ClipPosY = 0;
    SrcWidth = 0;
    SrcHeight = 0;
    PicAlphaPtr = NULL;
    Id = 0XFFFFFFFF;
    format = ( int )PixelFormat::R5G6B5;
    Flags = 0;
    SET_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew );
    m_Filename = "";

    if( CPIcon_Singleton::g_CPIconPool == NULL )
    {
        CPIcon_Singleton::g_CPIconPool = new std::map<CPIcon*, int>;
    }

    CPIcon_Singleton::g_CPIconPool->insert( std::map<CPIcon*, int>::value_type( this, 0 ) );
    //CPIcon_Singleton::g_CPIconPool->push_back(this);
}


//=============================================================================
//
//
//=============================================================================

CPIcon::CPIcon( uint_t w, uint_t h, uint_t d, ubyte_t* p )
{
    width = 0;              // The width of the image in pixels
    height = 0;             // The height of the image in pixels
    depth = 0;
    pixels = 0;             // The image pixel data
    palette = 0;
    format = 0;             // The channels in the image (3 = RGB : 4 = RGBA)
    m_FileType = NONE_TYPE;
    m_FileSize = 0;             //file size
    BytesOfWidth = 0;       //picture width bytes
    ClipOffsetX = 0;        //picture offset x view for paste, it will be used when paste this picture
    ClipOffsetY = 0;        //picture offset y veiw for paste, it will be used when paste this picture
    ClipPosX = 0;
    ClipPosY = 0;
    SrcWidth = 0;
    SrcHeight = 0;
    PicAlphaPtr = NULL;
    Id = 0XFFFFFFFF;
    format = ( int )PixelFormat::R5G6B5;
    Flags = 0;
    SET_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew );
    m_Filename = "";

    if( CPIcon_Singleton::g_CPIconPool == NULL )
    {
        CPIcon_Singleton::g_CPIconPool = new std::map<CPIcon*, int>;
    }

    CPIcon_Singleton::g_CPIconPool->insert( std::map<CPIcon*, int>::value_type( this, 0 ) );
    //CPIcon_Singleton::g_CPIconPool->push_back(this);
    Allot( w, h, d, p );
}


//=============================================================================
//
//
//=============================================================================
CPIcon::~CPIcon()
{
    Clear();

    if( CPIcon_Singleton::g_CPIconPool != NULL )
    {
        CPIcon_Singleton::g_CPIconPool->erase( this );

        /*
        for(int i = 0; i<CPIcon_Singleton::g_CPIconPool->size(); i++)
        {
            if((*CPIcon_Singleton::g_CPIconPool)[i]==this)
            {
                CPIcon_Singleton::g_CPIconPool->erase(CPIcon_Singleton::g_CPIconPool->begin()+i);
                break;
            }
        }
        */
        if( CPIcon_Singleton::g_CPIconPool->empty() )
            SAFE_DELETE( CPIcon_Singleton::g_CPIconPool )
    }
}


//=============================================================================
//
//
//=============================================================================
void CPIcon::RegisterLoadImageFunc( CPIcon::OnLoadImage func )
{
    g_LoadImageFunc = func;
}


//=============================================================================
//
//
//=============================================================================
void CPIcon::RegisterFreeImageFunc( CPIcon::OnFreeImage func )
{
    g_FreeImageFunc = func;
}

//=============================================================================
// NAME: Offset()
// DESC: computer real offset value
//=============================================================================
LG::Point CPIcon::Offset( void ) const
{
    return LG::Point( ClipPosX + ClipOffsetX, ClipPosY + ClipOffsetY );
}

//=============================================================================
//
//
//=============================================================================
void CPIcon::SetOffsetX( short x )
{
    ClipOffsetX = x - ClipPosX;
}


//=============================================================================
//
//
//=============================================================================
void CPIcon::SetOffsetY( short y )
{
    ClipOffsetY = y - ClipPosY;
}

//=============================================================================
// NAME: Image_Rect()
// DESC: compute image rect
//=============================================================================
LG::Rect CPIcon::GetClipRect( LG::Point Situs ) const
{
    int left = Situs.x - ClipOffsetX;
    int top = Situs.y - ClipOffsetY;
    return LG::Rect( left, top, left + width, top + height );
}


//=============================================================================
//
//
//=============================================================================
uint_t CPIcon::GetWidth( void ) const
{
    return SrcWidth;
}


//=============================================================================
//
//
//=============================================================================
uint_t CPIcon::GetHeight( void ) const
{
    return SrcHeight;
}

//=============================================================================
//
//
//=============================================================================
LG::Rect CPIcon::GetRect( LG::Point Situs ) const
{
    int left = Situs.x - Offset().x;
    int top = Situs.y - Offset().y;
    return LG::Rect( left, top, left + GetWidth(), top + GetHeight() );
}


//=============================================================================
//
//
//=============================================================================
CPIcon& CPIcon::operator=( const CPIcon& src )
{
    SAFE_DELETE_ARRAY( pixels );
    SAFE_DELETE_ARRAY( PicAlphaPtr );
    //memcpy(this, &src, sizeof(CPIcon))
    width = src.width;
    height = src.height;
    depth = src.depth;
    pixels = NULL;
    palette = NULL;
    format = src.format;
    m_Filename = src.m_Filename;
    m_FileType = src.m_FileType;
    m_FileSize = src.m_FileSize;
    BytesOfWidth = src.BytesOfWidth;
    ClipOffsetX = src.ClipOffsetX;
    ClipOffsetY = src.ClipOffsetY;
    ClipPosX = src.ClipPosX;
    ClipPosY = src.ClipPosY;
    SrcWidth = src.SrcWidth;
    SrcHeight = src.SrcHeight;
    Id = 0XFFFFFFFF;
    PicAlphaPtr = NULL;
    HitPoint[ 0 ] = src.HitPoint[ 0 ];
    HitPoint[ 1 ] = src.HitPoint[ 1 ];
    HitPoint[ 2 ] = src.HitPoint[ 2 ];
    HitPoint[ 3 ] = src.HitPoint[ 3 ];
    Bounding_Shape = src.Bounding_Shape;
    Bounding_Tall = src.Bounding_Tall;
    Bounding_Rect = src.Bounding_Rect;
    ZLine = src.ZLine;           //順序的基準線
    uint_t Image_size = src.width * src.height * src.depth;
    uint_t Alpha_size = src.m_FileSize - Image_size - sizeof( PINHEADER );

    //if(Allot(width, height, depth))
    //{
    //if(PicAlphaPtr != 0)
    //{
    if( src.PicAlphaPtr != NULL )
    {
        // 因為目前的alpha格式 是這樣的 如果alpha 有採用RLE壓縮之類的 就無法這樣做
        //Alpha_size=width*height;
        PicAlphaPtr = new ubyte_t[ Alpha_size ];
        __pfmemcpy( PicAlphaPtr, src.PicAlphaPtr, Alpha_size );
    }
    else
    {
        PicAlphaPtr = NULL;
    }

    if( src.pixels != NULL )
    {
        pixels = new ubyte_t[ Image_size ];
        __pfmemcpy( pixels, src.pixels, width * depth * height );
        SET_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew );
    }
    else
    {
        width = 0;
        height = 0;
        depth = 0;
    }

    //}
    return *this;
}


////=============================================================================
//// NAME: new
//// DESC:
////=============================================================================
////void CPIcon::operator new(uint_t w, uint_t h, uint_t d)
/*
bool CPIcon::Allot(const CPIcon* refer)
{
    bool ret = Allot(refer->width, refer->height, refer->depth);
    SrcWidth = refer->SrcWidth;
    SrcHeight = refer->SrcHeight;
    return ret;

}
*/


bool CPIcon::Allot( uint_t w, uint_t h, uint_t d )
{
    if( ( w <= 0 ) || ( h <= 0 ) || ( d < 0 ) )
    {
        //OUTTRACE("Canvas size error!!. \n");
        return false;
    }

    if( ( w == width ) && ( h == height ) && ( d == depth ) && ( IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) ) )
    {
        m_Filename.clear();
    }
    else
    {
        Clear();
        width = w;
        height = h;
        depth = d;
        BytesOfWidth = w * d;
        PicAlphaPtr = new ubyte_t[ height * width ];
        pixels = new ubyte_t[ height * BytesOfWidth ];
        SrcWidth = w;
        SrcHeight = h;
        SET_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew );
        //DWMEMSET(this, 0, sizeof(CPIcon));
    }

    memset( PicAlphaPtr, 0, ( height * width ) );
    memset( pixels, 0, ( height * BytesOfWidth ) );
    return true;
}


////=============================================================================
//// NAME: new
//// DESC:
////=============================================================================
bool CPIcon::Allot( uint_t w, uint_t h, uint_t d, ubyte_t* p )
{
    Clear();
    width = w;
    BytesOfWidth = w * d;
    height = h;
    depth = d;
    pixels = p;
    SrcWidth = w;
    SrcHeight = h;
    //CPIcon_Singleton::g_CPIconPool->push_back(this);
    m_Filename = "";
    CANCEL_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew );
    return true;
}

//=============================================================================
// NAME: GetBoundingShape()
// DESC: compute define rect
//=============================================================================
word_t CPIcon::GetBoundingShape( void ) const
{
    return Bounding_Shape;
}



//=============================================================================
// NAME: GetBoundingRect()
// DESC: compute define rect
//=============================================================================
LG::Rect CPIcon::GetBoundingRect( LG::Point Situs ) const
{
    int left = Situs.x - Offset().x;
    int top = Situs.y - Offset().y;
    return  LG::Rect( left + Bounding_Rect.left,
                      top + Bounding_Rect.top,
                      left + Bounding_Rect.right,
                      top + Bounding_Rect.bottom );
}


//=============================================================================
// NAME: Release()
// DESC: release image memory
//=============================================================================
void CPIcon::Clear( void )
{
    if( g_FreeImageFunc )
    {
        g_FreeImageFunc( this );
    }

    SAFE_DELETE_ARRAY( PicAlphaPtr );

    if( IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        SAFE_DELETE_ARRAY( pixels );
    }
    else
    {
        pixels = NULL;
    }

    width = 0;
    height = 0;
    depth = 0;
    m_Filename.clear();
    palette = NULL;
    Id = 0XFFFFFFFF;
    format = ( int )PixelFormat::R5G6B5;
}

//=============================================================================
// NAME: Clear()
// DESC: set image color
//=============================================================================
void CPIcon::FullColor( dword_t Color )
{
    //memset(pixels, Color, (size32_t)(height * BytesOfWidth));
    if( PicAlphaPtr )
    {
        memset( PicAlphaPtr, Color, ( height * width ) );
    }

    memset( pixels, Color, ( height * BytesOfWidth ) );
    //ZeroMemory(pixels, (height* BytesOfWidth));
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
uint_t CPIcon::GetMemorySize( void ) const
{
    return m_FileSize - sizeof( PINHEADER ) + sizeof( CPIcon );
}


//=============================================================================
// NAME: CheckVaildAlphaBuffer()
// DESC: check if alpha buffer is all invaild value1
//=============================================================================
bool CheckVaildAlphaBuffer( const ubyte_t* pa, size32_t la )
{
    if( pa == NULL )
    {
        return false;
    }

    div_t alpha_mod_64 = div( la, 8 );
    ubyte_t* pCheck = ( ubyte_t* )pa;
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

//=============================================================================
// NAME: Load()
// DESC: loading image file
//=============================================================================
int CPIcon::Load( const tchar_t* infname ) //,int VedioMask)   //讀入Pin圖形檔
{
    //int pf;
    //char msg[30];
    /*
    if((pf=open(fname,O_RDONLY | O_BINARY)) == -1)//開檔如果-1則失敗，傳回-1
    {
        sprintf(msg,"Load file %s failure !!!", fname);
        close(pf);
        MessageBox(NULL,msg, "Load Failed", MB_OK);
        return -1;
        }
    */
    tchar_t fname[ _MAX_LFNAME ];
    mcurrentpath( fname, infname );
    FileOperator pf;

    if( pf.open( fname, _T( "rb" ) ) == NULL )
    {
        //tchar_t msg[_MAX_MSG];
        //sprintf(msg, "Load file %s failure !!!", infname);
        Pf_ERROR( "CPIcon::Load: Load file %s failure !!!", infname );
        return pf.ferrno();
    }

    //check file format
    byte_t Title[ 4 ];
    memset( Title, 0, 4 );
    pf.seek( 0 );
    pf.read( Title, 3, 1 );

    if( ( strcmp( Title, "PIN" ) != 0 ) )
    {
        return -1;
    }

    //clear buffer
    Clear();
    m_Filename = fname;
    int AlphaOffset = 0;
    int ImageOffset = 0;
    PINHEADER f_header;
    pf.seek( 0 );
    pf.read( &f_header, sizeof( f_header ), 1 );
    Id                   = f_header.ApId;
    AlphaOffset          = f_header.ApAlphaOffset;
    ImageOffset          = f_header.ApImageOffset;
    width                = f_header.ApPinWidth;
    height               = f_header.ApPinHeight;
    depth                = LOBYTE( f_header.ApPixelMode );
    BytesOfWidth         = ( width * depth );
    format               = f_header.ApPixelMode;
    uint_t Alpha_size = ImageOffset - AlphaOffset;
    uint_t Image_size = width * depth * height;
    m_FileSize           = Image_size + ImageOffset;
    ubyte_t c_mode       = f_header.ApCompressMode;
    ClipOffsetX          = f_header.ApRelativeX;
    ClipOffsetY          = f_header.ApRelativeY;
    SrcWidth             = f_header.ApSrcImage.SrcWidth;
    SrcHeight            = f_header.ApSrcImage.SrcHeight;
    ClipPosX             = f_header.ApSrcImage.SrcOffsetX;
    ClipPosY             = f_header.ApSrcImage.SrcOffsetY;
    Bounding_Shape       = f_header.ApAmbitShape;
    Bounding_Tall        = f_header.ApAmbitTall;
    Bounding_Rect.top    = f_header.ApRecttop0;
    Bounding_Rect.left   = f_header.ApRectleft0;
    Bounding_Rect.right  = f_header.ApRectright0;
    Bounding_Rect.bottom = f_header.ApRectbottom0;
    HitPoint[ 0 ].x      = f_header.APHitPointX0;
    HitPoint[ 0 ].y      = f_header.APHitPointY0;
    HitPoint[ 1 ].x      = f_header.APHitPointX1;
    HitPoint[ 1 ].y      = f_header.APHitPointY1;
    HitPoint[ 2 ].x      = f_header.APHitPointX2;
    HitPoint[ 2 ].y      = f_header.APHitPointY2;
    HitPoint[ 3 ].x      = f_header.APHitPointX3;
    HitPoint[ 3 ].y      = f_header.APHitPointY3;
    ZLine[ 0 ].x         = f_header.ApZLineX0;
    ZLine[ 0 ].y         = f_header.ApZLineY0;
    ZLine[ 1 ].x         = f_header.ApZLineX1;
    ZLine[ 1 ].y         = f_header.ApZLineY1;
    //檢查檔案compress格式====================================================================
    int Map_size = Alpha_size + Image_size;

    if( Image_size > 0 )
    {
        switch( c_mode )
        {
            case '0':
                {
                    m_FileType = PIN_TYPE;

                    if( Alpha_size <= 1 ) //no alpha
                    {
                        PicAlphaPtr = 0;
                    }
                    else
                    {
                        PicAlphaPtr = new ubyte_t[ Alpha_size ];
                        pf.seek( AlphaOffset );
                        pf.read( PicAlphaPtr, Alpha_size, 1 );

                        // if alpha value is all 0XFF.......
                        if( !CheckVaildAlphaBuffer( ( ubyte_t* )PicAlphaPtr, Alpha_size ) )
                        {
                            SAFE_DELETE_ARRAY( PicAlphaPtr );
                        }
                    }

                    pixels = new ubyte_t[ Image_size ];
                    pf.seek( ImageOffset );
                    pf.read( pixels, Image_size, 1 );
                    //lseek(pf, m_file_offset + ImageOffset, SEEK_SET); read(pf, pixels, Image_size);
                    break;
                }

            case 'Z':
                {
                    m_FileType = PINZ_TYPE;
                    FileOperator _z_pk;
                    ubyte_t* CMbuf = new ubyte_t[ Map_size ];
                    pf.seek( AlphaOffset );
                    pf.read( CMbuf, Map_size, 1 );
                    //lseek(pf, m_file_offset + AlphaOffset, SEEK_SET); read(pf, CMbuf, Map_size);
                    int     UCLen = pfziplib::pzip::GetMaxUncompressedLen( _SAFE_BYTE_TO_BYTET( CMbuf ), Map_size );
                    ubyte_t* UCbuf = new ubyte_t[ UCLen ];
                    int uncompress_size = pfziplib::pzip::Uncompress_BToB( _SAFE_BYTE_TO_BYTET( CMbuf ), Map_size, UCbuf, UCLen );
                    delete[] CMbuf;

                    if( Alpha_size <= 1 ) //no alpha
                    {
                        PicAlphaPtr = NULL;
                    }
                    else
                    {
                        if( CheckVaildAlphaBuffer( UCbuf, Alpha_size ) )
                        {
                            PicAlphaPtr = new ubyte_t[ Alpha_size ];
                            memcpy( PicAlphaPtr, UCbuf, Alpha_size );
                        }
                        // if alpha value is all 0XFF.......
                        else
                        {
                            PicAlphaPtr = NULL;
                        }
                    }

                    pixels = new ubyte_t[ Image_size ];
                    memcpy( pixels, ( UCbuf + Alpha_size ), Image_size );
                    //IO::FreeZipBuffer(UCbuf);
                    SAFE_DELETE_ARRAY( UCbuf );
                    break;
                }

            default:
                {
                    Pf_ERROR( "CPIcon::Load: file format error !!!" );
                    PicAlphaPtr = NULL;   //File Alpha   offset
                    pixels = NULL;        //File picture offset
                    palette = NULL;
                    pf.close();
                    return -1;
                }
        }

        //END 檢查檔案格式================================================================
        switch( GetPixelFmt() )
        {
            case PixelFormat::R5G6B5:
                break;

            case PixelFormat::R5G5B5:
                ToR5G5B5();
                break;

            case PixelFormat::R5G5B6:
                ToR5G5B6();
                break;

            case PixelFormat::R6G5B5:
                ToR6G5B5();
                break;

            case PixelFormat::R8G8B8:
                ToR8G8B8();
                break;

            case PixelFormat::A8R8G8B8:
                ToA8R8G8B8();
                break;

            case PixelFormat::A8B8G8R8:
                ToA8B8G8R8();
                break;

            case PixelFormat::R8G8B8A8:
                ToR8G8B8A8();
                break;

            default:
                {
                    Pf_ERROR( "CPIcon::Load: %s : No define 565 file to no define VGA yet !!!", fname );
                    break;
                }
        }
    }
    else
    {
        PicAlphaPtr = NULL;
        pixels = NULL;
    }

    pf.close();

    if( g_LoadImageFunc )
    {
        g_LoadImageFunc( this );
    }

    return 1;
}


//=============================================================================
// NAME: Is_In_Area
// DESC: Is in area b_x b_x is image situs if in scopt return the situs in scopt
//         else return -1, -1
//=============================================================================
bool CPIcon::IsCollisionByBound( LG::Point point, int b_x, int b_y ) const
{
    //LG::Point s_pt(b_x, b_y);
    switch( Bounding_Shape )
    {
        case _P_RHOMBUS_:
            return PA::Intersect::Test( &( GetBoundingRect( LG::Point( b_x, b_y ) ).change_Quadr() ),
                                        &point );

        case _P_OCTAGON_:
            return PA::Intersect::Test( &( GetBoundingRect( LG::Point( b_x, b_y ) ).change_Octagon() ),
                                        &point );

        default:
            //_p_rect_
            return PA::Intersect::Test( &GetBoundingRect( LG::Point( b_x, b_y ) ), &point );
    }
}


//=============================================================================
// NAME: Is_In_Image_Area()
// DESC: Is in image area if in scopt return the situs in scopt
//         else return -1, -1
//=============================================================================
bool CPIcon::IsCollisionByImage( LG::Point point, int b_x, int b_y ) const
{
    if( PicAlphaPtr == 0 )
        return PA::Intersect::Test( &GetClipRect( LG::Point( b_x, b_y ) ),
                                    &point );
    else
    {
        if( PA::Intersect::Test( &GetClipRect( LG::Point( b_x, b_y ) ),
            &point ) )
        {
            uint_t offset = width * ( point.y - ( b_y - ClipOffsetY ) ) + ( point.x - ( b_x - ClipOffsetX ) );

            if( offset > ( GetClipHeight() * GetClipWidth() ) )
            {
                return false;
            }

            ubyte_t* testPoint = PicAlphaPtr + offset;

            if( *testPoint >= CPIcon_Singleton::g_Alpha_CollisionValue )
            {
                return true;
            }
        }
    }

    return false;
}


//=============================================================================
// NAME: GetCollisionByAlpha()
// DESC: Is in image area if in scopt return the situs in scopt
//         else return -1, -1
//=============================================================================
bool CPIcon::IsCollisionByAlpha( LG::Point point, int b_x, int b_y ) const
{
    return false;
}

//=============================================================================
// NAME: IsCollision
// DESC: Is in scope if in scopt return the situs in scopt
//         else return -1, -1
//=============================================================================
/*
bool CPIcon::Is_Collision(LG::Point point, int b_x, int b_y)
{
    return Is_In_Area(point, b_x, b_y);
}

bool CPIcon::Is_Collision(CPIcon *image, int b_x, int b_y)
{
    //LG::Rect rt = image->Rect(
    //LG::Point RetPoint(-1, -1);
    switch(Ambit_Shape)
    {
    case _p_rhombus_:
        return false;
    case _p_octagon_:
        return false;
    default: //_p_rect_
        return false;//Rect(LG::Point(b_x, b_y)).is_collision(rect);
    }
    return false;
}
*/




//=============================================================================
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
//        > 0: large (closer to the user, last paint!) point is under then line
//=============================================================================
#define IS_CLOSE_THEN(p1,p2,p3)  ((p2.y-p1.y)*(p3.x-p1.x)-(p3.y-p1.y)*(p2.x-p1.x))


int CPIcon::ZOrder( CPIcon& Hpinmap )
{
    //speedup
    //if (nZOrder &&obj->GetZOrder())
    //  return nZOrder - obj->GetZOrder();
    // Is my baseline closer to the user than obj's?
    if( ZLine[ 0 ].x <= Hpinmap.ZLine[ 0 ].x )
    {
        return IS_CLOSE_THEN( ZLine[ 0 ], ZLine[ 1 ], Hpinmap.ZLine[ 0 ] );
    }
    else
    {
        return -IS_CLOSE_THEN( Hpinmap.ZLine[ 0 ], Hpinmap.ZLine[ 1 ], ZLine[ 0 ] );
    }
}



//=============================================================================
// NAME: ReverseImage
// DESC: 上下翻轉影像
//=============================================================================
void CPIcon::ReverseImage( void )
{
    if( PicAlphaPtr != NULL )
    {
        ubyte_t* newalpha = new ubyte_t[ width * height ];
        ubyte_t* newalphaptr = newalpha;
        ubyte_t* alphaptr = NULL;

        for( int j = height - 1; j >= 0; j-- )
        {
            alphaptr = PicAlphaPtr + ( j * width );
            memcpy( newalphaptr, alphaptr, width );
            newalphaptr += width;
        }

        delete[] PicAlphaPtr;
        PicAlphaPtr = newalpha;
    }

    ubyte_t* newimage = new ubyte_t[ width * height * depth ];
    ubyte_t* newpixelptr = newimage;
    ubyte_t* pixelptr = NULL;

    for( int j = height - 1; j >= 0; j-- )
    {
        pixelptr = pixels + ( j * BytesOfWidth );
        memcpy( newpixelptr, pixelptr, BytesOfWidth );
        newpixelptr += BytesOfWidth;
    }

    delete[] pixels;
    pixels = newimage;
    return;
}


/*
void CPIcon::SetVGA_PixelFormat(PixelFormat pfmt)
{



}
*/


///////////////////////////////////////////////////////////////////////////////
//  convert pixel format

typedef word_t( _CALLBACK* On16bspixelTo16bpixel )( word_t src );
typedef dword_t( _CALLBACK* On16bspixelTo32bpixel )( word_t src, ubyte_t alpha );
typedef word_t( _CALLBACK* On32bspixelTo16bpixel )( dword_t src, ubyte_t& alpha );
typedef dword_t( _CALLBACK* On32bspixelTo32bpixel )( dword_t src );

void Icon16bitsTo16bits( uint_t w,
                         uint_t h,
                         const word_t* src,
                         word_t* dest,
                         On16bspixelTo16bpixel func );
void Icon16bitsTo32bits( uint_t w,
                         uint_t h,
                         const ubyte_t* alpha,
                         const word_t* src,
                         dword_t* dest,
                         On16bspixelTo32bpixel func );
void Icon32bitsTo16bits( uint_t w,
                         uint_t h,
                         const dword_t* src,
                         ubyte_t* desta,
                         word_t* destp,
                         On32bspixelTo16bpixel func );
void Icon32bitsTo32bits( uint_t w,
                         uint_t h,
                         const dword_t* src,
                         dword_t* dest,
                         On32bspixelTo32bpixel func );
void Icon16bitsTo24bits( uint_t w,
                         uint_t h,
                         const word_t* src,
                         dword_t* dest,
                         On16bspixelTo32bpixel func );

word_t  R5G6B5toR5G5B5( word_t src );
word_t  R5G6B5toR5G5B6( word_t src );
word_t  R5G6B5toR6G5B5( word_t src );
dword_t R5G6B5toA8R8G8B8( word_t src, ubyte_t alpha );

dword_t R5G6B5toR8G8B8A8( word_t src, ubyte_t alpha );
dword_t R5G6B5toA8B8G8R8( word_t src, ubyte_t alpha );

word_t  A8R8G8B8toR5G6B5( dword_t src, ubyte_t& alpha );
word_t  A8R8G8B8toR5G5B5( dword_t src, ubyte_t& alpha );

word_t  R8G8B8A8toR5G6B5( dword_t src, ubyte_t& alpha );
word_t  R8G8B8A8toR5G5B5( dword_t src, ubyte_t& alpha );
dword_t R8G8B8A8toA8R8G8B8( dword_t src );
dword_t R8G8B8A8toA8B8G8R8( dword_t src );
word_t  R5G5B5toR5G6B5( word_t src );


inline dword_t _ARGBto565( dword_t p )
{
    ubyte_t alpha;
    return A8R8G8B8toR5G6B5( p, alpha );
}
inline dword_t _ARGBto555( dword_t p )
{
    ubyte_t alpha;
    return A8R8G8B8toR5G5B5( p, alpha );
}
inline dword_t _ARGBto556( dword_t p )
{
    return p;
} // 未完成
inline dword_t _ARGBto655( dword_t p )
{
    return p;
} // 未完成
inline dword_t _ARGBtoARGB( dword_t p )
{
    return p;
}
inline dword_t _ARGBtoABGR( dword_t p )
{
    DWORD pixel32;
    ubyte_t* pixel = ( ubyte_t* )&pixel32;
    pixel[ 0 ] = p;
    pixel[ 1 ] = p >> 24;
    pixel[ 2 ] = p >> 16;
    pixel[ 3 ] = p >> 8;
    return pixel32;
}

inline dword_t _ARGBtoRGBA( dword_t p )
{
    DWORD pixel32;
    ubyte_t* pixel = ( ubyte_t* )&pixel32;
    pixel[ 0 ] = p;
    pixel[ 1 ] = p >> 8;
    pixel[ 2 ] = p >> 16;
    pixel[ 3 ] = p >> 24;
    return pixel32;
}

//     case PixelFormat::R8G8B8A8:
//         {
//             DWORD pixel32;
//             ubyte_t* pixel = (ubyte_t*)&pixel32;
//             pixel[0] = r;
//             pixel[1] = g;
//             pixel[2] = b;
//             pixel[3] = a;
//             return pixel32;
//         }
//     case PixelFormat::A8R8G8B8:
//         {
//             DWORD pixel32;
//             ubyte_t* pixel = (ubyte_t*)&pixel32;
//             pixel[0] = a;
//             pixel[1] = r;
//             pixel[2] = g;
//             pixel[3] = b;
//             return pixel32;
//         }



typedef dword_t( _CALLBACK* OnIconPixelFmtTransform )( dword_t );
OnIconPixelFmtTransform g_IconPixelFmtFuncTransform = _ARGBto565;


//=============================================================================
// NAME: P565ToP555
// DESC: change 565 mode to 555 mode
//=============================================================================
void CPIcon::ToR5G6B5( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                break;

            case PixelFormat::R5G6B5:
                break;

            case PixelFormat::R5G5B5:
                Icon16bitsTo16bits( width,
                                    height,
                                    ( const word_t* )pixels,
                                    ( word_t* )pixels,
                                    R5G5B5toR5G6B5 );
                break;

            case PixelFormat::R5G5B6:
                break;

            case PixelFormat::R6G5B5:
                break;

            case PixelFormat::R8G8B8A8:
                {
                    ubyte_t* pic565ptr = new ubyte_t[ width * height * 2 ];

                    if( PicAlphaPtr != NULL )
                    {
                        delete[] PicAlphaPtr;
                    }

                    PicAlphaPtr = new ubyte_t[ width * height ];
                    Icon32bitsTo16bits( width,
                                        height,
                                        ( const dword_t* )pixels,
                                        PicAlphaPtr,
                                        ( word_t* )pic565ptr,
                                        R8G8B8A8toR5G6B5 );
                    delete[] pixels;
                    pixels = pic565ptr;
                    depth = 2;
                    BytesOfWidth = ( width * depth );
                }
                break;

            case PixelFormat::A8R8G8B8:
                {
                    ubyte_t* pic565ptr = new ubyte_t[ width * height * 2 ];

                    if( PicAlphaPtr != NULL )
                    {
                        delete[] PicAlphaPtr;
                    }

                    PicAlphaPtr = new ubyte_t[ width * height ];
                    Icon32bitsTo16bits( width,
                                        height,
                                        ( const dword_t* )pixels,
                                        PicAlphaPtr,
                                        ( word_t* )pic565ptr,
                                        A8R8G8B8toR5G6B5 );
                    delete[] pixels;
                    pixels = pic565ptr;
                    depth = 2;
                    BytesOfWidth = ( width * depth );
                }
                break;
        }
    }

    format = ( int )PixelFormat::R5G6B5;
    return;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPIcon::ToR5G5B5( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                {
                    Icon16bitsTo16bits( width,
                                        height,
                                        ( const word_t* )pixels,
                                        ( word_t* )pixels,
                                        R5G6B5toR5G5B5 );
                }
                break;

            case PixelFormat::R5G5B5:
                //already
                break;

            case PixelFormat::R5G5B6:
                return;

            case PixelFormat::R6G5B5:
                return;

            case PixelFormat::R8G8B8A8:
                {
                    ubyte_t* pic555ptr = new ubyte_t[ width * height * 2 ];

                    if( PicAlphaPtr != NULL )
                    {
                        delete[] PicAlphaPtr;
                    }

                    PicAlphaPtr = new ubyte_t[ width * height ];
                    Icon32bitsTo16bits( width,
                                        height,
                                        ( const dword_t* )pixels,
                                        PicAlphaPtr,
                                        ( word_t* )pic555ptr,
                                        R8G8B8A8toR5G5B5 );
                    delete[] pixels;
                    pixels = pic555ptr;
                    depth = 2;
                    BytesOfWidth = ( width * depth );
                }
                break;

            case PixelFormat::A8R8G8B8:
                return;
        }
    }

    format = ( int )PixelFormat::R5G5B5;
    return;
}


void CPIcon::ToR5G5B6( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                Icon16bitsTo16bits( width,
                                    height,
                                    ( const word_t* )pixels,
                                    ( word_t* )pixels,
                                    R5G6B5toR5G5B6 );
                break;

            case PixelFormat::R5G5B5:
                //no yet
                return;

            case PixelFormat::R5G5B6:
                //no yet
                return;

            case PixelFormat::R6G5B5:
                //no yet
                return;

            case PixelFormat::R8G8B8A8:
                {
                    //
                    return;
                }

            case PixelFormat::A8R8G8B8:
                {
                    //
                    return;
                }
        }
    }

    format = ( int )PixelFormat::R5G5B6;
    return;
}

void CPIcon::ToR6G5B5( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                Icon16bitsTo16bits( width,
                                    height,
                                    ( const word_t* )pixels,
                                    ( word_t* )pixels,
                                    R5G6B5toR6G5B5 );
                break;

            case PixelFormat::R5G5B5:
                return;

            case PixelFormat::R5G5B6:
                return;

            case PixelFormat::R6G5B5:
                return;

            case PixelFormat::R8G8B8A8:
                {
                    //not yet
                    return;
                }

            case PixelFormat::A8R8G8B8:
                {
                    //not yet
                    return;
                }
        }
    }

    format = ( int )PixelFormat::R6G5B5;
    return;
}


void CPIcon::ToA8R8G8B8( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                {
                    ubyte_t* picARGBptr = new ubyte_t[ width * height * 4 ];
                    Icon16bitsTo32bits( width,
                                        height,
                                        PicAlphaPtr,
                                        ( const word_t* )pixels,
                                        ( dword_t* )picARGBptr,
                                        R5G6B5toA8R8G8B8 );
                    delete[] pixels;
                    pixels = picARGBptr;
                }
                break;

            case PixelFormat::R5G5B5:
                //no yet
                return;

            case PixelFormat::R5G5B6:
                //no yet
                return;

            case PixelFormat::R6G5B5:
                //no yet
                return;

            case PixelFormat::R8G8B8A8:
                {
                    //no yet
                    return;
                }

            case PixelFormat::A8R8G8B8:
                {
                    //already
                    return;
                }
        }
    }

    depth = 4;
    BytesOfWidth = ( width * depth );
    format = ( int )PixelFormat::A8R8G8B8;
    return;
}

void CPIcon::ToR8G8B8A8( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                {
                    ubyte_t* picRGBAptr = new ubyte_t[ width * height * 4 ];
                    Icon16bitsTo32bits( width,
                                        height,
                                        PicAlphaPtr,
                                        ( const word_t* )pixels,
                                        ( dword_t* )picRGBAptr,
                                        R5G6B5toR8G8B8A8 );
                    delete[] pixels;
                    pixels = picRGBAptr;
                }
                break;

            case PixelFormat::R5G5B5:
                return;

            case PixelFormat::R5G5B6:
                return;

            case PixelFormat::R6G5B5:
                return;

            case PixelFormat::R8G8B8A8:
                {
                    //already
                    break;
                }

            case PixelFormat::A8R8G8B8:
                {
                    //not yet
                    return;
                }
        }
    }

    depth = 4;
    BytesOfWidth = ( width * depth );
    format = ( int )PixelFormat::R8G8B8A8;
    return;
}

void CPIcon::ToA8B8G8R8( void )
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                {
                    ubyte_t* picARGBptr = new ubyte_t[ width * height * 4 ];
                    Icon16bitsTo32bits( width,
                                        height,
                                        PicAlphaPtr,
                                        ( const word_t* )pixels,
                                        ( dword_t* )picARGBptr,
                                        R5G6B5toA8B8G8R8 );
                    delete[] pixels;
                    pixels = picARGBptr;
                }
                break;

            case PixelFormat::R5G5B5:
                return;

            case PixelFormat::R5G5B6:
                return;

            case PixelFormat::R6G5B5:
                return;

            case PixelFormat::R8G8B8A8:
                {
                    Icon32bitsTo32bits( width,
                                        height,
                                        ( dword_t* )pixels,
                                        ( dword_t* )pixels,
                                        R8G8B8A8toA8B8G8R8 );
                    break;
                }

            case PixelFormat::A8R8G8B8:
                {
                    //already
                    break;
                }
        }
    }

    depth = 4;
    BytesOfWidth = ( width * depth );
    format = ( int )PixelFormat::A8B8G8R8;
    return;
}

void CPIcon::ToR8G8B8( void )  //轉變為RGB
{
    // 因為不是你自己建立的 不要去亂轉
    if( !IF_BIT_FLAG( Flags, CPIcon_Singleton::g_Flags_PixelsFromNew ) )
    {
        return;
    }

    if( pixels )
    {
        switch( format )
        {
            case PixelFormat::UNKNOW:
                return;

            case PixelFormat::R5G6B5:
                {
                    ubyte_t* picARGBptr = new ubyte_t[ width * height * 3 ];
                    Icon16bitsTo24bits( width,
                                        height,
                                        ( word_t* )pixels,
                                        ( dword_t* )picARGBptr,
                                        R5G6B5toA8B8G8R8 );    // Reality: RGBA
                    delete[] pixels;
                    pixels = picARGBptr;
                }
                break;

            case PixelFormat::R5G5B5:
                return;

            case PixelFormat::R5G5B6:
                return;

            case PixelFormat::R6G5B5:
                return;

            case PixelFormat::R8G8B8A8:
                return;

            case PixelFormat::A8R8G8B8:
                {
                    //already
                    break;
                }
        }
    }

    depth = 3;
    BytesOfWidth = ( width * depth );
    format = ( int )PixelFormat::R8G8B8;
    return;
}

///////////////////////////////////////////////////////////////////////////////////////
//
word_t R5G6B5toR5G5B5( word_t src )
{
    register ubyte_t SR = src >> 11;
    register ubyte_t SG = ( src >> 6 ) & 0X1F;         // 6 bits to 5 bits
    register ubyte_t SB = src & 0x1F;
    register ubyte_t save = ( ( src ) & 0x20 ) << 10; // save r's lowest bit
    return ( SR << 10 ) | ( SG << 5 ) | SB | save;
}

word_t R5G6B5toR5G5B6( word_t src )
{
    register ubyte_t SR = src >> 11;
    register ubyte_t SG = ( src >> 6 ) & 0X1F;
    register ubyte_t SB = src & 0x1F;
    return ( SR << 11 ) | ( SG << 6 ) | SB;
}

word_t R5G6B5toR6G5B5( word_t src )
{
    register ubyte_t SR = src >> 11;
    register ubyte_t SG = ( src >> 6 ) & 0X1F;
    register ubyte_t SB = src & 0x1f;
    return ( SR << 10 ) | ( SG << 5 ) | SB;
}

dword_t R5G6B5toA8R8G8B8( word_t src, ubyte_t alpha )
{
    register ubyte_t SR;
    register ubyte_t SG;            // 6 bits to 5 bits
    register ubyte_t SB;
    _SPLITP565( src, SR, SG, SB );
    return ( alpha << 24 ) | ( SR << 16 ) | ( SG << 8 ) | SB;
}

dword_t R5G6B5toR8G8B8A8( word_t src, ubyte_t alpha )
{
    register ubyte_t SR;
    register ubyte_t SG;            // 6 bits to 5 bits
    register ubyte_t SB;
    _SPLITP565( src, SR, SG, SB );
    return ( SR << 24 ) | ( SG << 16 ) | ( SB << 8 ) | alpha;
}

dword_t R5G6B5toA8B8G8R8( word_t src, ubyte_t alpha )
{
    register ubyte_t SR;
    register ubyte_t SG;            // 6 bits to 5 bits
    register ubyte_t SB;
    _SPLITP565( src, SR, SG, SB );
    return ( alpha << 24 ) | ( SB << 16 ) | ( SG << 8 ) | SR;
}

word_t A8R8G8B8toR5G6B5( dword_t src, ubyte_t& alpha )
{
    //ubyte_t* srcptr = (ubyte_t*) &src;
    //alpha = srcptr[3];
    //return _MAKEP565(srcptr[2], srcptr[1], srcptr[0]);
    unsigned r, g, b;
    r = ( src & 0xf80000 ) >> 8;
    g = ( src & 0xfc00 ) >> 5;
    b = ( src & 0xf8 ) >> 3;
    return ( r | g | b );
}


word_t A8R8G8B8toR5G5B5( dword_t src, ubyte_t& alpha )
{
    unsigned r, g, b;
    r = ( src & 0xf80000 ) >> 9;
    g = ( src & 0xf800 ) >> 6;
    b = ( src & 0xf8 ) >> 3;
    return ( r | g | b );
}


word_t R8G8B8A8toR5G6B5( dword_t src, ubyte_t& alpha )
{
    ubyte_t* srcptr = ( ubyte_t* )&src;
    alpha = srcptr[ 3 ];
    return _MAKEP565( srcptr[ 0 ], srcptr[ 1 ], srcptr[ 2 ] );
}


word_t R8G8B8A8toR5G5B5( dword_t src, ubyte_t& alpha )
{
    ubyte_t* srcptr = ( ubyte_t* )&src;
    alpha = srcptr[ 3 ];
    return _MAKEP555( srcptr[ 0 ], srcptr[ 1 ], srcptr[ 2 ] );
}

dword_t R8G8B8A8toA8R8G8B8( dword_t src )
{
    ubyte_t dest[ 4 ];
    ubyte_t* srcptr = ( ubyte_t* )&src;
    dest[ 0 ] = srcptr[ 3 ];
    dest[ 1 ] = srcptr[ 0 ];
    dest[ 2 ] = srcptr[ 1 ];
    dest[ 3 ] = srcptr[ 2 ];
    return *( dword_t* )dest;
}

dword_t R8G8B8A8toA8B8G8R8( dword_t src )
{
    ubyte_t dest[ 4 ];
    ubyte_t* srcptr = ( ubyte_t* )&src;
    dest[ 0 ] = srcptr[ 3 ];
    dest[ 1 ] = srcptr[ 2 ];
    dest[ 2 ] = srcptr[ 1 ];
    dest[ 3 ] = srcptr[ 0 ];
    return *( dword_t* )dest;
}

word_t R5G5B5toR5G6B5( word_t src )
{
    register ubyte_t SR = ( src >> 10 ) & 0x1F;        // must clear saved bit
    register ubyte_t SG = ( ( src >> 5 ) & 0x1F ) << 1; // 5 bits to 6 bits
    register ubyte_t SB = src & 0x1F;
    register ubyte_t save = ( src >> 10 ) & 0x20;
    return ( SR << 11 ) | ( SG << 5 ) | SB | save;
}

///////////////////////////////////////////////////////////////////////////////////////
//

void Icon16bitsTo16bits( uint_t w,
                         uint_t h,
                         const word_t* src,
                         word_t* dest,
                         On16bspixelTo16bpixel func )
{
    const word_t* srcbptr = src;
    word_t* destptr = dest;

    for( uint_t j = 0; j < h; j++ )
    {
        for( uint_t i = 0; i < w; i++ )
        {
            *destptr = func( *srcbptr );
            srcbptr++;// = src16bptr + 2;
            destptr++;
        }
    }
}


void Icon16bitsTo32bits( uint_t w,
                         uint_t h,
                         const ubyte_t* alpha,
                         const word_t* src,
                         dword_t* dest,
                         On16bspixelTo32bpixel func )
{
    const word_t* srcptr = src;
    dword_t* destptr = dest;
    const ubyte_t* alphaptr = alpha;

    for( uint_t j = 0; j < h; j++ )
    {
        for( uint_t i = 0; i < w; i++ )
        {
            if( alphaptr == NULL )
            {
                *destptr = func( *srcptr, 0XFF );
            }
            else
            {
                *destptr = func( *srcptr, *alphaptr );
                alphaptr++;
            }

            srcptr++;// += 2;
            destptr++;// += 4;
        }
    }
}

void Icon32bitsTo16bits( uint_t w,
                         uint_t h,
                         const dword_t* src,
                         ubyte_t* desta,
                         word_t* destp,
                         On32bspixelTo16bpixel func )
{
    const dword_t* srcptr = src;
    ubyte_t* destaptr = desta;
    word_t* destpptr = destp;

    for( UINT j = 0; j < h; j++ )
    {
        for( UINT i = 0; i < w; i++ )
        {
            *destpptr = func( *srcptr, *destaptr );
            srcptr++;
            destaptr++;
            destpptr++;
        }
    }

    return;
}

void Icon32bitsTo32bits( uint_t w,
                         uint_t h,
                         const dword_t* src,
                         dword_t* dest,
                         On32bspixelTo32bpixel func )
{
    const dword_t* srcptr = src;
    dword_t* destptr = dest;

    for( uint_t j = 0; j < h; j++ )
    {
        for( uint_t i = 0; i < w; i++ )
        {
            *destptr = func( *srcptr );
            srcptr++;
            destptr++;
        }
    }
}

void Icon16bitsTo24bits( uint_t w,
                         uint_t h,
                         const word_t* src,
                         dword_t* dest,
                         On16bspixelTo32bpixel func )
{
    const word_t* srcptr = src;
    ubyte_t* destptr = ( ubyte_t* )dest;
    ubyte_t* valptr;
    dword_t cval;
    valptr = ( ubyte_t* )&cval;

    //valptr++;
    for( uint_t j = 0; j < h; j++ )
    {
        for( uint_t i = 0; i < w; i++ )
        {
            cval = func( *srcptr, 0xff );
            *destptr++ = valptr[ 0 ];
            *destptr++ = valptr[ 1 ];
            *destptr++ = valptr[ 2 ];
            srcptr++;// += 2;
        }
    }
}


//=====================================================================================
//  icon cpy
//
//=====================================================================================
void CPIcon::SetPixelFmt( PixelFormat fmt )
{
    if( CPIcon_Singleton::g_CPIconPool != NULL )
    {
        if( !CPIcon_Singleton::g_CPIconPool->empty() )
        {
            std::map<CPIcon*, int>::iterator iter = CPIcon_Singleton::g_CPIconPool->begin();

            //for(int i = 0; i<CPIcon_Singleton::g_CPIconPool->size(); i++)
            while( iter != CPIcon_Singleton::g_CPIconPool->end() )
            {
                //CPIcon* icon = (*CPIcon_Singleton::g_CPIconPool)[i];
                CPIcon* icon = iter->first;

                switch( fmt )
                {
                    case PixelFormat::R5G6B5:
                        icon->ToR5G6B5();
                        break;

                    case PixelFormat::R5G5B5:
                        icon->ToR5G5B5();
                        break;

                    case PixelFormat::R5G5B6:
                        icon->ToR5G5B6();
                        break;

                    case PixelFormat::R6G5B5:
                        icon->ToR6G5B5();
                        break;

                    case PixelFormat::R8G8B8:
                        icon->ToR8G8B8();
                        break;

                    case PixelFormat::A8R8G8B8:
                        icon->ToA8R8G8B8();
                        break;

                    case PixelFormat::A8B8G8R8:
                        icon->ToA8B8G8R8();
                        break;

                    default:
                        {
                            Pf_ERROR( "CPIcon::SetPixelFmt: %s : file can't change to define VGA format yet !!!", icon->GetFilename() );
                            break;
                        }
                }

                iter++;
            }
        }
    }

    _VGA_PixelFormat = fmt;

    switch( fmt )
    {
        case PixelFormat::R5G6B5:
            g_IconPixelFmtFuncTransform = _ARGBto565;
            break;

        case PixelFormat::R5G5B5:
            g_IconPixelFmtFuncTransform = _ARGBto555;
            break;

        case PixelFormat::R5G5B6:
            g_IconPixelFmtFuncTransform = _ARGBto556;
            break;

        case PixelFormat::R6G5B5:
            g_IconPixelFmtFuncTransform = _ARGBto655;
            break;

        case PixelFormat::R8G8B8:
            g_IconPixelFmtFuncTransform = _ARGBtoARGB;
            break;

        case PixelFormat::A8R8G8B8:
            g_IconPixelFmtFuncTransform = _ARGBtoARGB;
            break;

        case PixelFormat::A8B8G8R8:
            g_IconPixelFmtFuncTransform = _ARGBtoABGR;
            break;

        default:
            {
                //char msg[256];
                //sprintf(msg, _T("no support pixel translate "));
                Pf_ERROR( "CPIcon::SetPixelFmt: no support pixel translate" );
                break;
            }
    }

    return;
}


//=============================================================================
//
//
//=============================================================================
PixelFormat CPIcon::GetPixelFmt( void )
{
    return _VGA_PixelFormat;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
dword_t CPIcon::MakePixel( dword_t pix )
{
    assert( g_IconPixelFmtFuncTransform != NULL || _T( "No support makepixel format" ) );
    return g_IconPixelFmtFuncTransform( pix );
    //     switch(CPIcon::GetPixelFmt())
    //     {
    //     case PixelFormat::R5G6B5:
    //         return _MAKEP565(r, g, b);
    //     case PixelFormat::R5G5B6:
    //         return _MAKEP556(r, g, b);
    //     case PixelFormat::R6G5B5:
    //         return _MAKEP655(r, g, b);
    //     case PixelFormat::R5G5B5:
    //         return _MAKEP555(r, g, b);
    //     case PixelFormat::R8G8B8A8:
    //         {
    //             DWORD pixel32;
    //             ubyte_t* pixel = (ubyte_t*)&pixel32;
    //             pixel[0] = r;
    //             pixel[1] = g;
    //             pixel[2] = b;
    //             pixel[3] = a;
    //             return pixel32;
    //         }
    //     case PixelFormat::A8R8G8B8:
    //         {
    //             DWORD pixel32;
    //             ubyte_t* pixel = (ubyte_t*)&pixel32;
    //             pixel[0] = a;
    //             pixel[1] = r;
    //             pixel[2] = g;
    //             pixel[3] = b;
    //             return pixel32;
    //         }
    //     }
    return 0;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPIcon::SplitPixel( int p, ubyte_t& r, ubyte_t& g, ubyte_t& b, ubyte_t& a )
{
    switch( CPIcon::GetPixelFmt() )
    {
        case PixelFormat::R5G6B5:
            a = 0;
            _SPLITP565( p, r, g, b );
            break;

        case PixelFormat::R5G5B6:
            a = 0;
            _SPLITP556( p, r, g, b );
            break;

        case PixelFormat::R6G5B5:
            a = 0;
            _SPLITP655( p, r, g, b );
            break;

        case PixelFormat::R5G5B5:
            a = 0;
            _SPLITP555( p, r, g, b );
            break;

        case PixelFormat::R8G8B8A8:
            _SPLITPRGBA( p, r, g, b, a );
            break;
    }

    return;
}

short CPIcon::GetAlpha_CollisionValue()
{
    return CPIcon_Singleton::g_Alpha_CollisionValue;
}

void  CPIcon::SetAlpha_CollisionValue( short v )
{
    CPIcon_Singleton::g_Alpha_CollisionValue = v;
}

