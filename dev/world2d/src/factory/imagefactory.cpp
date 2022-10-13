//Image_Editer.cpp
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
//
//
//                                                         Pin
//
#include "ImageFactory.h"
#include <PfFileOperator.h>
#include <HPIN.h> /*source*/
//#include <SavePin.h>


/////////////////////////////////////////////////////////////////////////////////////////////////
//
CPIconFactory::CPIconFactory()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CPIconFactory::~CPIconFactory()
{
}


int CPIconFactory::SaveAll( CPIcon* icon )
{
    SaveRect( icon );
    SaveZLine( icon );
    SaveOffset( icon );
    SaveHitPoint( icon );
    return true;
}

/*
int CPIconFactory::Change_Offset(CPIcon* icon, short RX, short RY)
{
       icon->GetOffsetX() = RX - icon->GetSourceOffsetX();
       icon->GetOffsetY() = RY - icon->GetSourceOffsetY();
       return 1;
}
*/


int CPIconFactory::SaveZLine( CPIcon* icon )
{
    IO::PfFileSystem pf;
    
    if ( pf.open( icon->GetFilename(), "r+b" ) == NULL )
    {
        MessageBox( NULL, "檔案排序軸入失敗!!!", " 寫檔失敗", 48 );
        return pf.ferrno();
    }
    
    PINHEADER f_header;
    //fseek(pf(), m_file_offset, SEEK_SET);
    pf.seek( 0 );
    pf.read( &f_header, 1, sizeof( f_header ) );
    
    if ( strcmp( f_header.ApName, "PIN" ) == 0 )
    {
        f_header.ApZLineX0 = ( word_t )icon->ZLine[0].x;
        f_header.ApZLineY0 = ( word_t )icon->ZLine[0].y;
        f_header.ApZLineX1 = ( word_t )icon->ZLine[1].x;
        f_header.ApZLineY1 = ( word_t )icon->ZLine[1].y;
        pf.seek( 0 );
        pf.write( &f_header, 1, sizeof( PINHEADER ) );
        //寫入磁碟圖檔
        //fseek(f, m_file_offset + 17, SEEK_SET); fwrite(&GetOffsetX(), 1, sizeof(short), f);
        //fseek(f, m_file_offset + 19, SEEK_SET); fwrite(&GetOffsetY(), 1, sizeof(short), f);
        //fclose(pf());
        pf.close();
    }
    else
    {
        //fclose(pf());
        pf.close();
        return _ERR_TITLE_UN_CONFORM;
    }
    
    return 1;
}


int CPIconFactory::SaveOffset( CPIcon* icon )
{
    //if(GetFilename() == 0)
    //	return false;
    /*
    FILE *f;
    if((f=fopen(GetFilename() , "r+b")) == NULL)//開檔如果-1則失敗，傳回-1
    {
        MessageBox(NULL,"檔案基準寫入失敗!!!"," 寫檔失敗", 48);
      	return false;
    }
    */
    IO::PfFileSystem pf;
    
    if ( pf.open( icon->GetFilename(), "r+b" ) == NULL )
    {
        MessageBox( NULL, "檔案基準寫入失敗!!!", " 寫檔失敗", 48 );
        return pf.ferrno();
    }
    
    PINHEADER f_header;
    //fseek(pf(), m_file_offset, SEEK_SET);
    pf.seek( 0 );
    pf.read( &f_header, 1, sizeof( f_header ) );
    
    if ( strcmp( f_header.ApName, "PIN" ) == 0 )
    {
        f_header.ApRelativeX = icon->GetClipOffsetX();
        f_header.ApRelativeY = icon->GetClipOffsetY();
        pf.seek( 0 );
        pf.write( &f_header, 1, sizeof( PINHEADER ) );
        //寫入磁碟圖檔
        //fseek(f, m_file_offset + 17, SEEK_SET); fwrite(&GetOffsetX(), 1, sizeof(short), f);
        //fseek(f, m_file_offset + 19, SEEK_SET); fwrite(&GetOffsetY(), 1, sizeof(short), f);
        //fclose(pf());
        pf.close();
    }
    else
    {
        //fclose(pf());
        pf.close();
        return _ERR_TITLE_UN_CONFORM;
    }
    
    return 1;
}

int CPIconFactory::SaveHitPoint( CPIcon* icon )
{
    //if(GetFilename() == 0)
    //	return false;
    /*
    FILE *f;
    if((f=fopen(GetFilename() , "r+b")) == NULL)//開檔如果-1則失敗，傳回-1
    {
        MessageBox(NULL,"檔案命中點入失敗!!!"," 寫檔失敗", 48);
        return false;
    }
    */
    IO::PfFileSystem pf;
    
    if ( pf.open( icon->GetFilename(), "r+b" ) == NULL )
    {
        MessageBox( NULL, "檔案命中點入失敗!!!", " 寫檔失敗", 48 );
        return pf.ferrno();
    }
    
    PINHEADER f_header;
    //fseek(f, m_file_offset, SEEK_SET);
    pf.seek( 0 );
    //fread(&f_header, 1, sizeof(f_header), f);
    pf.read( &f_header, 1, sizeof( f_header ) );
    
    if ( strcmp( f_header.ApName, "PIN" ) == 0 )
    {
        f_header.APHitPointX0 = ( word_t )icon->HitPoint[0].x;
        f_header.APHitPointY0 = ( word_t )icon->HitPoint[0].y;
        f_header.APHitPointX1 = ( word_t )icon->HitPoint[1].x;
        f_header.APHitPointY1 = ( word_t )icon->HitPoint[1].y;
        f_header.APHitPointX2 = ( word_t )icon->HitPoint[2].x;
        f_header.APHitPointY2 = ( word_t )icon->HitPoint[2].y;
        f_header.APHitPointX3 = ( word_t )icon->HitPoint[3].x;
        f_header.APHitPointY3 = ( word_t )icon->HitPoint[3].y;
        //fseek(f, m_file_offset, SEEK_SET);
        pf.seek( 0 );
        //fwrite(&f_header, 1, sizeof(PINHEADER), f);
        pf.write( &f_header, 1, sizeof( PINHEADER ) );
        //寫入磁碟圖檔
        //fseek(f, m_file_offset + 124, SEEK_SET); fwrite(&(HitPoint.x), 1, sizeof(WORD), f);
        //fseek(f, m_file_offset + 126, SEEK_SET); fwrite(&(HitPoint.y), 1, sizeof(WORD), f);
        //fclose(f);
        pf.close();
    }
    else
    {
        //fclose(f);
        pf.close();
        return _ERR_TITLE_UN_CONFORM;
    }
    
    return 1;
}


int CPIconFactory::SaveRect( CPIcon* icon )
{
    //if(GetFilename() == 0)
    //	return false;
    /*
    FILE *f;
    if((f=fopen(GetFilename() , "r+b")) == NULL)//開檔如果-1則失敗，傳回-1
    {
        MessageBox(NULL,"檔案命中點入失敗!!!"," 寫檔失敗", 48);
        return false;
    }
    */
    IO::PfFileSystem pf;
    
    if ( pf.open( icon->GetFilename(), "r+b" ) == NULL )
    {
        MessageBox( NULL, "檔案影像範圍寫入失敗!!!", " 寫檔失敗", 48 );
        return pf.ferrno();
    }
    
    PINHEADER f_header;
    //fseek(f, m_file_offset, SEEK_SET);
    pf.seek( 0 );
    //fread(&f_header, 1, sizeof(f_header), f);
    pf.read( &f_header, 1, sizeof( f_header ) );
    
    if ( strcmp( f_header.ApName, "PIN" ) == 0 )
    {
        f_header.ApAmbitShape  = icon->Bounding_Shape;
        f_header.ApAmbitTall   = icon->Bounding_Tall;
        f_header.ApRectleft0   = ( word_t )icon->Bounding_Rect.left;
        f_header.ApRecttop0    = ( word_t )icon->Bounding_Rect.top;
        f_header.ApRectright0  = ( word_t )icon->Bounding_Rect.right;
        f_header.ApRectbottom0 = ( word_t )icon->Bounding_Rect.bottom;
        //fseek(f, m_file_offset, SEEK_SET);
        pf.seek( 0 );
        //fwrite(&f_header, 1, sizeof(PINHEADER), f);
        pf.write( &f_header, 1, sizeof( PINHEADER ) );
        //寫入磁碟圖檔
        //fseek(f, m_file_offset + 33, SEEK_SET);  fwrite(&Ambit_Shape,         1, sizeof(BYTE), f);
        //fseek(f, m_file_offset + 34, SEEK_SET);  fwrite(&Ambit_Tall,         1, sizeof(BYTE), f);
        //fseek(f, m_file_offset + 116, SEEK_SET); fwrite(&(Ambit_Rect.left),   1, sizeof(WORD), f);
        //fseek(f, m_file_offset + 118, SEEK_SET); fwrite(&(Ambit_Rect.top),    1, sizeof(WORD), f);
        //fseek(f, m_file_offset + 120, SEEK_SET); fwrite(&(Ambit_Rect.right),  1, sizeof(WORD), f);
        //fseek(f, m_file_offset + 122, SEEK_SET); fwrite(&(Ambit_Rect.bottom), 1, sizeof(WORD), f);
        //fclose(f);
        pf.close();
    }
    else
    {
        //fclose(f);
        pf.close();
        return _ERR_TITLE_UN_CONFORM;
    }
    
    return 1;
}
