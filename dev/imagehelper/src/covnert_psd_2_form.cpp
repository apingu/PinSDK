#include "convert_image.h"
#include "convert_func.h"
#include "Load_psd.h"
#include "save_pin.h"
#include "save_elem.h"
#include <pf-def.h>
#include <pf-cstring.h>
#include <pf-debug.h>
#include <Direct.h>
#include <string>
#include <pf-string.h>

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
typedef struct __ButtonInfo
{
    char Name[_MAX_LFNAME];
    int  Id;
    int  GId;
    int  posx;
    int  posy;
} ButtonInfo;

std::vector<ButtonInfo> g_Buttons;

int  SortImageName( const void* name0, const void* name1 );
void SaveLayer2Elem( const char* LayerSet, int posx, int posy, std::vector<const char*>* Images );
void SaveAllButtonToForm( const char* formname );


int PsdToForm( const char*          srcfilename,
               const char*          destfilename,
               PixelFormat             mode,
               bool                 isTrim,
               int                  gradation,
               bool                 isCompress,
               bool                 isDelImage,
               ConvertImageFunctor* pFunctor )
{
    //typedef char FilenameWord[ _MAX_LFNAME ];
    FILE* psdf = fopen( srcfilename, "rb" );
    if( psdf == NULL )
    {
        //char msg[256];
        //sprintf( msg, "Open source psd file %s failure !", srcfilename );
        //AddResultMessage(msg);
        Pf_ERROR( "PsdToForm: Open source psd file %s failure !", srcfilename );
        return -1;
    }
    //取出要儲存的檔名
    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    char rootfolder[ _MAX_LPATH ];
    char infofile[ _MAX_LPATH ];
    _splitpath( srcfilename, drive, dir, fname, ext );
    _makepath( rootfolder, drive, dir, "", "" );
    //psd file infomation
    _makepath( infofile, drive, dir, fname, "psdinfo" );
    FILE* psdinfo = fopen( infofile, "wb" );
    if( psdinfo == NULL )
    {
        //char msg[256];
        //sprintf( msg, "Open psd info file %s failure !", infofile );
        Pf_ERROR( "PsdToForm: Open psd info file %s failure !", infofile );
        return -1;
    }
    char psdinfobuf[_MAX_MSG];
    memset( psdinfobuf, ' ', _MAX_MSG );
    sprintf( psdinfobuf, "<PSDXML>\n" );
    fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
    int w, h;
    int layersum = GetPSDLayerNum( psdf, &w, &h );
    int tgaMode  = Switch_Pixel_Format( mode );
    char elementname[_MAX_LFNAME] = {""};
    char copyfoldername[_MAX_LFNAME] = {""};
    //std::vector< const char* > elemimagelist;
    std::vector<std::string> iconimagelist;
    //放置同依個群組的檔名
    std::vector<std::string> groupfilelist;
    //紀錄建立過的 物件名稱
    // 可以用來比對 重複編號
    std::vector<std::string> objecthistory;
    // 物件裡的所有圖檔
    std::vector<std::string> objectfilelist;
    int posx = 0;
    int posy = 0;
    int bmarkfolder = 0;
    std::vector<std::string> markfilelist;
    for( unsigned int li = layersum; li > 0; li-- )
        //for( unsigned int li=1; li<=layersum; li++ )
    {
        char  layername[_MAX_LFNAME];
        PSDLAYERTYPE layertype;
        fseek( psdf, 0, SEEK_SET );
        if( GetPSDLayerName( psdf, li, layername, _MAX_LFNAME, layertype ) != PSDERR_OK )
        { continue; }
        if( bmarkfolder > 0 )
        {
            if( ( strcmp( layername, "</Layer set>" ) == 0 ) ||
                    ( strcmp( layername, "</Layer group>" ) == 0 ) )
            {
                bmarkfolder--;
            }
            else
            {
                markfilelist.push_back( layername );
            }
            continue;
        }
        if( layertype == PSDLAYER_FOLDER )
        {
            if( ( layername[0] == '/' ) && ( layername[1] == '/' ) )
            {
                bmarkfolder++;
                markfilelist.push_back( layername );
                continue;
            }
        }
        if( ( strcmp( layername, "</Layer set>" ) == 0 ) ||
                ( strcmp( layername, "</Layer group>" ) == 0 ) ||
                ( strcmp( layername, " copy" ) == 0 ) ||
                ( strcmp( layername, " 拷貝" ) == 0 ) )
        {
            if( !groupfilelist.empty() )
            {
                // 寫入PSDINFO資料
                // 空格
                char currlayername[_MAX_LFNAME];
                strreplacec( currlayername, _MAX_LFNAME, groupfilelist.back().c_str(), ' ', '_' );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() ) * 4 ), "</%s>\n", currlayername );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                groupfilelist.erase( groupfilelist.end() - 1 );
            }
            if( groupfilelist.size() == 1 )
            {
                objecthistory.push_back( groupfilelist[0] );
            }
            continue;
        }
        // 2011 03 26 多加 () 註解功能
        // 比方說  UI040001SC(開始)  ()不列入檔名中
        char* pl = strtok( layername, "()" );
        if( pl != NULL )
        { strcpy( layername, pl ); }
        char layer_desp[_MAX_LFNAME] = {""};
        char* pm = strtok( NULL, "()" );
        if( pm )
        {
            strcpy( layer_desp, pm );
        }
        // 要存放位置的資料夾
        std::string dirstr;
        dirstr = drive;
        dirstr += "\\";
        dirstr += dir;
        dirstr += "\\";
        for( uint_t i = 0; i < groupfilelist.size(); i++ )
        {
            dirstr += groupfilelist[i];
            dirstr += "\\";
        }
        if( layertype == PSDLAYER_FOLDER )
        {
            char newlayername[_MAX_LFNAME];
            strcpy( newlayername, layername );
            // layer 前後可能有空白字元
            char* bp = newlayername;
            char* ep = &( newlayername[strlen( newlayername )] );
            for( int i = 0; i < strlen( newlayername ); i++ )
            {
                if( newlayername[i] != ' ' )
                {
                    bp = &( newlayername[i] );
                    break;
                }
            }
            for( int i = strlen( newlayername ) - 1; i >= 0; i-- )
            {
                if( newlayername[i] != ' ' )
                {
                    ep = &( newlayername[i] );
                    break;
                }
            }
            memset( layername, 0, _MAX_LFNAME );
            memcpy( layername, bp, ( ep - bp ) + 1 );
            // 寫入PSDINFO資料
            // 空格
            // XML 裡面不能有 " " like <Layer 1>
            char currlayername[_MAX_LFNAME];
            strreplacec( currlayername, _MAX_LFNAME, layername, ' ', '_' );
            memset( psdinfobuf, ' ', _MAX_MSG );
            if( strcmp( layer_desp, "" ) == 0 )
            {
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "<%s>\n", currlayername );
            }
            else
            {
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "<%s Description=\"%s\">\n", currlayername, layer_desp );
            }
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            groupfilelist.push_back( layername );
            dirstr += layername;
            mkdir( dirstr.c_str() );
            continue;
        }
        else
        {
            BYTE* lpImage = NULL;
            BYTE* lpAlpha = NULL;
            fseek( psdf, 0, SEEK_SET );
            if(  LoadPSD( psdf, &lpImage, &lpAlpha, li, &posx, &posy, tgaMode ) != PSDERR_OK )
            { continue; }
            // 寫入PSDINFO資料
            // 空格
            if( ( lpImage == NULL ) || ( lpAlpha == NULL ) )
            {
                // 寫入PSDINFO資料
                // 空格
                char currlayername[_MAX_LFNAME];
                strreplacec( currlayername, _MAX_LFNAME, layername, ' ', '_' );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "<Name_%s>\n", currlayername );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<LayerNo>%d</LayerNo>\n", li );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<X>%d</X>\n", posx );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<Y>%d</Y>\n", posx );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<W>0</W>\n" );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<H>0</H>\n" );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<RESULT>Read .psd layer %s failure !</RESULT>\n" );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "</Name_%s>\n", currlayername );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                continue;
            }
            //寫入PSD的資料
            int iw = ( *( word_t* )( lpImage ) );
            int ih = ( *( word_t* )( lpImage + sizeof( word_t ) ) );
            std::string Pinfilename = dirstr;
            Pinfilename += layername;
            iconimagelist.push_back( Pinfilename.c_str() );
            if( isCompress )
            {
                Pinfilename += ".pinz";
            }
            else
            {
                Pinfilename += ".pin";
            }
            //找一下layer曾有沒有重複命名，算我可憐你
            UINT cl = 0;
            char drive1[ _MAX_DRIVE ];
            char dir1[ _MAX_LDIR ];
            char fname1[ _MAX_LFNAME ];
            char ext1[ _MAX_LEXT ];
            _splitpath( Pinfilename.c_str(), drive1, dir1, fname1, ext1 );
            while ( fopen( Pinfilename.c_str(), "rb" ) != NULL )
            {
                char numbuf[10];
                cl++;
                Pinfilename = drive1;
                Pinfilename += dir1;
                Pinfilename += fname1;
                Pinfilename += "_";
                Pinfilename += itoa( cl, numbuf, 10 );
                Pinfilename += ext1;
            }
            FILE* pinf = fopen( Pinfilename.c_str(), "wb" );
            if( pinf == NULL )
            {
                // 寫入PSDINFO資料
                // 空格
                memset( psdinfobuf, ' ', _MAX_MSG );
                sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "<RESULT>Write on %s failure !</RESULT>\n", Pinfilename.c_str() );
                fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                continue;
            }
            SavePIN( pinf, lpImage, lpAlpha, mode, isTrim, gradation, isCompress );
            fclose( pinf );
            SAFE_DELETE_ARRAY( lpImage );
            SAFE_DELETE_ARRAY( lpAlpha );
            objectfilelist.push_back( Pinfilename );
            if( pFunctor )
            { ( *pFunctor )( Pinfilename.c_str() ); }
            // 寫入PSDINFO資料
            // 空格
            char currlayername[_MAX_LFNAME];
            strreplacec( currlayername, _MAX_LFNAME, layername, ' ', '_' );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "<Name_%s>\n", currlayername );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<LayerNo>%d</LayerNo>\n", li );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<Target>%s</Target>\n", Pinfilename.c_str() );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<X>%d</X>\n", posx );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<Y>%d</Y>\n", posy );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<W>%d</W>\n", iw );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<H>%d</H>\n", ih );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 2 ) * 4 ), "<RESULT>Write successful !</RESULT>\n" );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
            memset( psdinfobuf, ' ', _MAX_MSG );
            sprintf( psdinfobuf + ( ( groupfilelist.size() + 1 ) * 4 ), "</Name_%s>\n", currlayername );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        }
        /*
        
        if( ( strcmp( layername, "</Layer set>") != 0 ) &&
            ( strcmp( layername, "</Layer group>") != 0 ) )
        {
            BYTE* lpImage=NULL;
            BYTE* lpAlpha=NULL;
        
            fseek( psdf, 0, SEEK_SET );
            if(  LoadPSD( psdf, &lpImage, &lpAlpha, li, &posx, &posy, tgaMode ) != PSDERR_OK )
                continue;
        
            //寫入PSD的資料
            char psdinfobuf[255];
            sprintf( psdinfobuf, "Layer%d %s  X=%d, Y=%d\n", li, layername, posx, posy );
            fwrite( psdinfobuf, strlen( psdinfobuf ),1, psdinfo );
            if( ( lpImage==NULL ) || ( lpAlpha==NULL ) )
            {
                char msg[256];
                sprintf( msg, "Read .psd layer %s failure !", layername );
                AddResultMessage(msg);
                continue;
            }
        
            char Pinfilename[ _MAX_LFNAME ];
                _makepath( Pinfilename, drive, dir, layername, "" );
        
            iconimagelist.push_back( Pinfilename );
        
        
            if( isCompress )
                _makepath( Pinfilename, drive, dir, layername, "pinz" );
            else
                _makepath( Pinfilename, drive, dir, layername, "pin" );
        
            //找一下layer曾有沒有重複命名，算我可憐你
            UINT cl=0;
            _splitpath( Pinfilename, drive, dir, fname, ext );
            while (fopen( Pinfilename, "rb" )!=NULL)
            {
                cl++;
                sprintf( Pinfilename, "%s%s%s_%d%s", drive, dir, fname, cl, ext );
            }
        
        
            FILE* pinf = fopen( Pinfilename, "wb" );
            if( pinf == NULL )
            {
                char msg[256];
                sprintf( msg, "Open dest pin file %s failure !", Pinfilename );
                AddResultMessage(msg);
                continue;
            }
        
            SavePIN( pinf, lpImage, lpAlpha, mode, isTrim, gradation, isCompress );
            fclose( pinf );
            SAFE_DELETE_ARRAY( lpImage );
            SAFE_DELETE_ARRAY( lpAlpha );
        
            groupfilelist.push_back( Pinfilename );
        
            //char* pfname = new char[ strlen(Pinfilename)+1 ];
            //strcpy( pfname, Pinfilename );
        
            //iconimagelist.push_back( pfname );
            //elemimagelist.push_back( pfname );
        }
        else
        {
            if( groupfilelist.empty() )
                continue;
            _splitpath( groupfilelist[0].c_str(), drive, dir, fname, ext );
            sprintf( copyfoldername, "%s\\%s", rootfolder, fname );
            mkdir( copyfoldername );
            unlink( groupfilelist[0].c_str() );
            for( UINT i=1; i<groupfilelist.size(); i++ )
            {
                _splitpath( groupfilelist[i].c_str(), drive, dir, fname, ext );
                char newpath[_MAX_LPATH];
                _makepath( newpath, "", copyfoldername, fname, ext );
                rename( groupfilelist[i].c_str(), newpath );
            }
            groupfilelist.clear();
        }
        */
        //unlink(Pinfilename );
        //pinfiles.push_back( Pinzfilename );
    }
    {
        char psdinfobuf[255];
        sprintf( psdinfobuf, "    <ALL_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        // 比對
        for( int i = 0; i < objecthistory.size(); i++ )
        {
            sprintf( psdinfobuf, "        %s\n", objecthistory[i].c_str() );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        }
        sprintf( psdinfobuf, "    </ALL_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        sprintf( psdinfobuf, "    <Double_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        for( int i = 0; i < objecthistory.size(); i++ )
        {
            for( int j = 0; j < objecthistory.size(); j++ )
            {
                if( i == j )
                { continue; }
                if( objecthistory[i] == objecthistory[j] )
                {
                    sprintf( psdinfobuf, "        %s\n", objecthistory[j].c_str() );
                    fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
                }
            }
        }
        sprintf( psdinfobuf, "    </Double_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        sprintf( psdinfobuf, "    <Vain_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        // 比對
        for( int i = 0; i < markfilelist.size(); i++ )
        {
            sprintf( psdinfobuf, "        %s\n", markfilelist[i].c_str() );
            fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
        }
        sprintf( psdinfobuf, "    </Vain_Object>\n" );
        fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
    }
    memset( psdinfobuf, ' ', _MAX_MSG );
    sprintf( psdinfobuf, "</PSDXML>\n" );
    fwrite( psdinfobuf, strlen( psdinfobuf ), 1, psdinfo );
    fclose( psdf );
    fclose( psdinfo );
    /*
    std::vector< ButtonInfo > g_Layers;
    ButtonInfo Info;
    for( unsigned int li=1; li< layersum+1; li++ )
    {
        fseek( psdf, 0, SEEK_SET );
        char layername[ _MAX_LFNAME ];
        GetPSDLayerName( psdf, li, layername, _MAX_LFNAME );
        ButtonInfo Info;
        char* cn = strtok( (char*)layername, "_" );
        for( unsigned int i=0; i<g_Layers.size(); i++ )
        {
            char* sn =  strtok( g_Layers[i].Name, "_" );
            if( strcmp( sn, cn )==0 )
            {
                g_Layers[i].LayerIndex.push_back(li);
                continue;
            }
        }
        strcpy( Info.Name, cn );
        Info.LayerIndex.push_back(li);
        g_Layers.push_back( Info );
    }
    */
    Pf_INFO( "Conversion successful !!!" );
    Pf_INFO( "=========================" );
    iconimagelist.clear();
    return 1;
}

int SortImageName( const void* name0, const void* name1 )
{
    char* fname0 = NULL;
    char* fname1 = NULL;
    while( ( fname0 != NULL ) || ( fname1 != NULL ) )
    {
        fname0 = strtok( ( char* )name0, "_" );
        fname1 = strtok( ( char* )name0, "_" );
        int ret = strcmp( fname0, fname1 );
        if( ret == 0 )
        {
            continue;
        }
        else
        { return ret; }
    }
    return 0;
}


void SaveLayer2Elem( const char* LayerSet, int posx, int posy, std::vector<const char*>* Images )
{
    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    ButtonInfo btinfo;
    btinfo.posx = posx;
    btinfo.posy = posy;
    _makepath( btinfo.Name, drive, dir, strtok( ( char* )LayerSet, "_" ), ".elem" );
    btinfo.Id = atoi( strtok( ( char* )LayerSet, "_" ) );
    btinfo.GId = atoi( strtok( ( char* )LayerSet, "_" ) );
    qsort( &( ( *Images )[0] ), Images->size(), sizeof( int ), SortImageName );
    HElem2d helem;
    SaveElem( btinfo.Name, &helem, *Images );
    g_Buttons.push_back( btinfo );
}

void SaveAllButtonToForm( const char* formname )
{
}
