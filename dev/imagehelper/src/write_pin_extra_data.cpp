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
#include <gadget\file_operator.h>
//
#include <game\HPIN.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "write_pin_extra_data.h"

//#include "..\File\PZFile.h"

//=============================================================================
// NAME: 
// DESC: write pin image file header
//=============================================================================
int  WriteExtraData( uPfFile fh,
                     unsigned short AdtumMarkX, 
                     unsigned short AdtumMarkY, 
                     unsigned char  Act,
                     unsigned char  Direction,
                     unsigned char  Step )
{
    int curpos = _pftell(fh);

    PINHEADER myheader;
    memset(&myheader, 0, sizeof(myheader));

    _pfread( &myheader, sizeof(PINHEADER), 1, fh );
    if( strcpy( myheader.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    myheader.ApExtInfo.ExtAct        =  Act;
    myheader.ApExtInfo.ExtDirection  =  Direction;
    myheader.ApExtInfo.ExtStep       =  Step;

    ushort_t Left   = (ushort_t)myheader.ApSrcImage.SrcOffsetX;
    ushort_t Top    = (ushort_t)myheader.ApSrcImage.SrcOffsetY;

    myheader.ApRelativeX             = AdtumMarkX - Left;
    myheader.ApRelativeY             = AdtumMarkY - Top;

    _pfseek( fh, curpos, SEEK_SET );
    _pfwrite( &myheader, sizeof(PINHEADER), 1, fh );
    _pfseek( fh, curpos, SEEK_SET );
    return PINERR_OK;
}

int  WriteExtraZLine    ( uPfFile fh, 
                          unsigned short zx0,
                          unsigned short zy0, 
                          unsigned short zx1,
                          unsigned short zy1 )
{
    int curpos = _pftell(fh);
    PINHEADER f_header;
    _pfread( &f_header, sizeof(PINHEADER), 1, fh );
    
    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApZLineX0 = zx0;
    f_header.ApZLineY0 = zy0;
    f_header.ApZLineX1 = zx1;
    f_header.ApZLineY1 = zy1;

    _pfseek( fh, curpos, SEEK_SET );
    _pfwrite( &f_header, 1, sizeof(PINHEADER), fh );
    _pfseek( fh, curpos, SEEK_SET );
    //寫入磁碟圖檔
    //_pfseek(f, m_file_offset + 17, SEEK_SET); _pfwrite(&RelativeX, 1, sizeof(short), f);
    //_pfseek(f, m_file_offset + 19, SEEK_SET); _pfwrite(&RelativeY, 1, sizeof(short), f);
    //fclose( pf() );
    //pf.close();

    return PINERR_OK;
}

int  WriteExtraOffset( uPfFile fh,
                        unsigned short x,
                       unsigned short y )
{
    int curpos = _pftell(fh);
    PINHEADER f_header;
    _pfread( &f_header, sizeof(PINHEADER), 1, fh );
    
    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApRelativeX = x - f_header.ApSrcImage.SrcOffsetX;
    f_header.ApRelativeY = y - f_header.ApSrcImage.SrcOffsetY;

    _pfseek( fh, curpos, SEEK_SET );
    _pfwrite( &f_header, 1, sizeof(PINHEADER), fh );
    _pfseek( fh, curpos, SEEK_SET );

    return PINERR_OK;
}

int  WriteExtraHitPoint ( uPfFile fh,
                           unsigned short x,
                          unsigned short y )
{
    int curpos = _pftell(fh);
    PINHEADER f_header;
    _pfread( &f_header, sizeof(PINHEADER), 1, fh );
    
    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.APHitPointX0 = x;
    f_header.APHitPointY0 = y;


    _pfseek( fh, curpos, SEEK_SET );
    _pfwrite( &f_header, 1, sizeof(PINHEADER), fh );
    _pfseek( fh, curpos, SEEK_SET );


    return PINERR_OK;
}

int  WriteExtraRect     ( uPfFile fh,
                          unsigned short shape,
                          unsigned short tall,
                          unsigned short top,
                          unsigned short left,
                          unsigned short right,
                          unsigned short bottom )
{
    int curpos = _pftell(fh);
    PINHEADER f_header;
    _pfread( &f_header, sizeof(PINHEADER), 1, fh );
    
    if( strcmp( f_header.ApName, "PIN" )!=0 )
        return PINERR_NOSUPPORT;

    f_header.ApAmbitShape  = shape;
    f_header.ApAmbitTall   = tall;
    f_header.ApRecttop0    = top;
    f_header.ApRectleft0   = left;
    f_header.ApRectright0  = right;
    f_header.ApRectbottom0 = bottom;

    _pfseek( fh, curpos, SEEK_SET );
    _pfwrite( &f_header, 1, sizeof(PINHEADER), fh );
    _pfseek( fh, curpos, SEEK_SET );

    return PINERR_OK;
}
