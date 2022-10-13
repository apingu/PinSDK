//
//
//  Base Image Class
//
//
//
//
//
//
//
//
//
//                                                         Pin
//

#ifndef ICON_H
#define ICON_H

//#include <stdlib.h>
#include <Color.h>
#include <graphic2D.h>
#include <tImage.h>
#include <string>

#define Error_Condition(s)      (((CPIcon*)s) == NULL) || (((CPIcon*)s)->GetPixel() == NULL)
#define No_Alpha_Condition(a)   (a->PicAlphaPtr == NULL)


enum ICON_TYPE{
    NONE_TYPE = 0,
    PIN_TYPE,
    PINZ_TYPE,
};

//enum VedioMask { p565, p555, p556, p655, pRGB, pRGBA, pBGRA };



class CPIcon;

typedef class CPIcon: public tImage
{
    _tstring            m_Filename;
    ICON_TYPE           m_FileType;
    dword_t             m_FileSize;         //file size
    word_t              BytesOfWidth;       //picture width bytes
    short               ClipOffsetX;        //picture offset x view for paste, it will be used when paste this picture
    short               ClipOffsetY;        //picture offset y veiw for paste, it will be used when paste this picture
    word_t              ClipPosX;
    word_t              ClipPosY;
    uint_t              SrcWidth;
    uint_t              SrcHeight;
    long                Flags;

    static PixelFormat  _VGA_PixelFormat;

public:

    CPIcon();
    CPIcon( uint_t w, uint_t h, uint_t d, ubyte_t* p );
    ~CPIcon();

    uint_t              Id;
    ubyte_t*            PicAlphaPtr;        //File Alpha   offset
    LG::Point           HitPoint[ 4 ];
    word_t              Bounding_Shape;
    word_t              Bounding_Tall;
    LG::Rect            Bounding_Rect;
    LG::Line2d          ZLine;           //順序的基準線

    typedef void ( _CALLBACK *OnLoadImage )( CPIcon* );
    typedef void ( _CALLBACK *OnFreeImage )( CPIcon* );


    void                Clear( void );
    void                FullColor( dword_t Color );
                        
    int                 Load( const tchar_t* pathname );    //讀入Pin圖形檔
    CPIcon &            operator=( const CPIcon &src );
                        
                        
    const tchar_t*      GetFilename( void )   const { return m_Filename.c_str(); }
    uint_t              GetFileSize( void )   const { return m_FileSize; }
    uint_t              GetMemorySize( void ) const;
    ushort_t            GetWidthSize( void )  const { return BytesOfWidth; }
    ushort_t            GetDepth( void )      const { return depth; }
                        
                        
    static void         RegisterLoadImageFunc( CPIcon::OnLoadImage func );
    static void         RegisterFreeImageFunc( CPIcon::OnFreeImage func );
    static void         SetPixelFmt( PixelFormat fmt );
    static PixelFormat  GetPixelFmt( void );
    static dword_t      MakePixel( dword_t pix );// a r g b
    static void         SplitPixel( int p, ubyte_t &r, ubyte_t &g, ubyte_t &b, ubyte_t &a );





    ///////////////////////////////////////////////////////////////////////////////
    //轉移後資訊
    //貼圖使用的偏移位置*貼圖底層用的
    inline ubyte_t*     GetPixel( void )                { return pixels; }
    inline uint_t       GetClipWidth( void )    const    { return width; }
    inline uint_t       GetClipHeight( void )    const    { return height; }
    inline short        GetClipOffsetX( void )    const    { return ClipOffsetX; }
    inline short        GetClipOffsetY( void )    const    { return ClipOffsetY; }
    LG::Rect            GetClipRect( LG::Point Situs ) const;  //最小範圍

public:

    ///////////////////////////////////////////////////////////////////////////////
    //原始檔未經轉檔裁切資訊
    LG::Point           Offset( void ) const;        //圖形的基準點
    void                SetOffsetX( short x );
    void                SetOffsetY( short y );
    uint_t              GetWidth( void ) const;
    uint_t              GetHeight( void ) const;
    LG::Rect            GetRect( LG::Point Situs ) const;  //最小範圍

    ///////////////////////////////////////////////////////////////////////////////
    //編輯資訊
    word_t              GetBoundingShape( void ) const;
    LG::Rect            GetBoundingRect( LG::Point Situs ) const;


    //bool                Allot(const CPIcon* refer);
    bool                Allot( uint_t w, uint_t h, uint_t d );
    bool                Allot( uint_t w, uint_t h, uint_t d, ubyte_t* p );
    bool                IsCollisionByBound( LG::Point point, int b_x = 0, int b_y = 0 ) const;
    bool                IsCollisionByImage( LG::Point point, int b_x = 0, int b_y = 0 )    const;
    bool                IsCollisionByAlpha( LG::Point point, int b_x = 0, int b_y = 0 ) const;
    int                 ZOrder( CPIcon &Hpinmap );

public:

    void                ReverseImage( void );    //上下翻轉影像

    // pixel format convert--------------------------------------------------
    void                ToR5G6B5( void );    //將轉變為565
    void                ToR5G5B5( void );    //將轉變為555
    void                ToR5G5B6( void );    //轉變為556
    void                ToR6G5B5( void );    //轉變為655
    void                ToR8G8B8( void );    //轉變為RGB 
    void                ToA8R8G8B8( void );    //轉變為ABGR
    void                ToA8B8G8R8( void );    //轉變為BGRA
    void                ToR8G8B8A8( void );    //轉變為RGBA
    //-----------------------------------------------------------------------

    static short        GetAlpha_CollisionValue();
    static void         SetAlpha_CollisionValue( short v );

}CPImage, Icon, HPINMAP, *LPHPINMAP;




#endif
/////////////////////////////////////////////////////////////////////////////
