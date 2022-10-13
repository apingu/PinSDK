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
//
//
//

//1

//2
#include <pf-string.h>
#include <pf-file.h>
#include <game\element2d.h>
#include <gadget\file_operator.h>

//3
#include <graphic\icon.h>
#include <game\helem.h> /*source*/

//4
#include "batch_convert.h"

#include "save_elem.h"


void BatchConvertElem( std::vector< const char*>* filenames )
{


}


bool GetBatchConvertFileCallBack( const tchar_t* path, int depth, void* pData )
{
    std::vector<std::string>* filenamelist = ( std::vector<std::string>* )pData;
    filenamelist->push_back( path );
    return true;
}

void SaveElemList( const std::vector<CPIcon*>& elemlist )
{
    char elemfoldername[ _MAX_LPATH ];
    pathrdivf( elemfoldername, _MAX_LPATH, elemlist[ 0 ]->GetFilename(), 2 );

    char outfname[ _MAX_LPATH ];
    strdumpstr( outfname, _MAX_LPATH, elemlist[ 0 ]->GetFilename(), elemfoldername );
    // 去掉最後一個"\"
    outfname[ strlen( outfname ) - 1 ] = 0;

    //step 1: find the anim
    std::map<std::string, GSO::Anim2d*> AnimList;

    for( UINT e = 0; e < elemlist.size(); e++ )
    {
        GSO::Anim2d* nowanim = NULL;
        char nowanimname[ _MAX_LPATH ];
        char nowanglename[ _MAX_LPATH ];

        elemlist[ e ]->Id = e;

        pathrdiv( nowanimname, _MAX_LPATH, elemlist[ e ]->GetFilename(), 2 );
        pathrdiv( nowanglename, _MAX_LPATH, elemlist[ e ]->GetFilename(), 1 );

        //step 1: find the anim
        std::map<std::string, GSO::Anim2d*>::iterator iter = AnimList.find( nowanimname );

        //沒有這個動作 要建立一個新的
        if( iter == AnimList.end() )
        {
            nowanim = new GSO::Anim2d;
            AnimList.insert( std::map<std::string, GSO::Anim2d*>::value_type( nowanimname, nowanim ) );
        }
        else
        {
            nowanim = iter->second;
        }


        //step 2: find the angle
        //沒有這個方向 要建立一個新的
        GSO::sequence_t* nowseq = nowanim->GetAnimSeq( atof( nowanglename ) );
        if( nowseq == NULL )
        {
            nowseq = nowanim->AddAnimSeq( atof( nowanglename ) );
        }
        nowanim->PushSeq( nowseq, e );
        //nowseq->push_back(e);
    }


    std::map<std::string, GSO::Anim2d*>::iterator iter = AnimList.begin();
    while( iter != AnimList.end() )
    {
        GSO::Anim2d* saveanm = iter->second;
        char saveanmpath[ _MAX_LPATH ];
        sprintf( saveanmpath, "%s_%s.anm", outfname, iter->first.c_str() );
        saveanm->Save( saveanmpath );
        SAFE_DELETE( saveanm );
        iter++;
    }

    //


    //
    std::vector<const char*>  saveimagefilelist;
    for( uint_t sf = 0; sf < elemlist.size(); sf++ )
    {
        saveimagefilelist.push_back( elemlist[ sf ]->GetFilename() );
    }

    char saveelempath[ _MAX_LPATH ];
    sprintf( saveelempath, "%s.elem", outfname );

    HElem2d HData;
    SaveElem( saveelempath, &HData, saveimagefilelist );
}


void SaveIconListAsElem( const char* ojbname, std::vector<const char*>& Iconflist, void* pData )
{
    std::vector<CPIcon*> icons;

    for( uint_t i = 0; i < Iconflist.size(); i++ )
    {
        CPIcon* icon = new CPIcon;
        if( icon->Load( Iconflist[ i ] ) == 1 )
        {
            icons.push_back( icon );
        }
        else
        {
            //可能不是一個格式檔
            SAFE_DELETE( icon );
        }
    }

    SaveElemList( icons );

    for( unsigned int i = 0; i < icons.size(); i++ )
    {
        SAFE_DELETE( icons[ i ] );
    }
}

int ScanStandFolderToElem( const char* folder, OnStandElemFolder func, void* pData )
{
    std::vector<std::string> filenamelist;

    FileSystem pfile;
    pfile.Scanfiles( folder, GetBatchConvertFileCallBack, &filenamelist );

    if( filenamelist.empty() )
        return 0;

    char elemname[ _MAX_LPATH ];

    // 先找出第一個基準元素的名稱
    for( uint_t i = 0; i < filenamelist.size(); i++ )
    {
        // 假如他不是一個格式檔 用下一個
        if( ( strcasestr( filenamelist[ i ].c_str(), ".pin" ) == 0 ) && ( strcasestr( filenamelist[ i ].c_str(), ".pinz" ) == 0 ) )
            continue;
        const char* filename = filenamelist[ i ].c_str();
        pathrdiv( elemname, _MAX_LPATH, filename, 3 );
        break;
    }

    // 擺同一個元件的圖檔
    std::vector<const char*> iconfiles;

    for( uint_t i = 0; i < filenamelist.size(); i++ )
    {
        char nowelemname[ _MAX_LPATH ];

        //get anim name        //(LPCTSTR

        if( ( strcasestr( filenamelist[ i ].c_str(), ".pin" ) == 0 ) && ( strcasestr( filenamelist[ i ].c_str(), ".pinz" ) == 0 ) )
            continue;

        pathrdiv( nowelemname, _MAX_LPATH, filenamelist[ i ].c_str(), 3 );
        if( strcmp( nowelemname, elemname ) == 0 )
        {
            iconfiles.push_back( filenamelist[ i ].c_str() );
        }
        else
        {
            std::string elemsrcfname = elemname;
            strcpy( elemname, nowelemname );
            if( iconfiles.empty() )
            {
                continue;
            }
            i--;

            func( elemsrcfname.c_str(), iconfiles, pData );

            //SaveElemList(elemlist);

            //for(uint_t el=0; el<elemlist.size(); el++)
            //    SAFE_DELETE(elemlist[el]);
            //elemlist.clear();
            iconfiles.clear();
        }
    }

    int cc = iconfiles.size();
    if( !iconfiles.empty() )
    {
        char nowelemname[ _MAX_LPATH ];
        pathrdiv( nowelemname, _MAX_LPATH, iconfiles[ 0 ], 3 );

        func( nowelemname, iconfiles, pData );
        //SaveElemList(elemlist);

        //for(uint_t el=0; el<elemlist.size(); el++)
        //    SAFE_DELETE(elemlist[el]);
        //elemlist.clear();
        iconfiles.clear();
    }
    return cc;
}


int BatchConvertFolderToElem( const char* folder )
{
    return ScanStandFolderToElem( folder );

    std::vector<std::string> filenamelist;

    FileSystem pfile;
    pfile.Scanfiles( folder, GetBatchConvertFileCallBack, &filenamelist );

    if( filenamelist.empty() )
        return 0;

    char elemname[ _MAX_LPATH ];

    // 先找出第一個基準元素的名稱+
    for( uint_t i = 0; i < filenamelist.size(); i++ )
    {
        // 假如他不是一個格式檔 用下一個
        if( ( strcasestr( filenamelist[ i ].c_str(), ".pin" ) == 0 ) && ( strcasestr( filenamelist[ i ].c_str(), ".pinz" ) == 0 ) )
            continue;
        const char* filename = filenamelist[ i ].c_str();
        pathrdiv( elemname, _MAX_LPATH, filename, 3 );
        break;
    }

    // 擺同一個元件的圖檔
    std::vector<CPIcon*> elemlist;

    for( uint_t i = 0; i < filenamelist.size(); i++ )
    {
        char nowelemname[ _MAX_LPATH ];

        //get anim name        //(LPCTSTR

        if( ( strcasestr( filenamelist[ i ].c_str(), ".pin" ) == 0 ) && ( strcasestr( filenamelist[ i ].c_str(), ".pinz" ) == 0 ) )
            continue;

        pathrdiv( nowelemname, _MAX_LPATH, filenamelist[ i ].c_str(), 3 );
        if( strcmp( nowelemname, elemname ) == 0 )
        {
            CPIcon* icon = new CPIcon;
            if( icon->Load( filenamelist[ i ].c_str() ) == 1 )
            {
                elemlist.push_back( icon );
            }
            else
            {
                //可能不是一個格式檔
                SAFE_DELETE( icon );
            }
        }
        else
        {
            strcpy( elemname, nowelemname );
            if( elemlist.empty() )
            {
                continue;
            }

            i--;

            SaveElemList( elemlist );

            for( uint_t el = 0; el < elemlist.size(); el++ )
                SAFE_DELETE( elemlist[ el ] );
            elemlist.clear();
        }
    }

    int cc = elemlist.size();
    if( !elemlist.empty() )
    {
        SaveElemList( elemlist );

        for( uint_t el = 0; el < elemlist.size(); el++ )
            SAFE_DELETE( elemlist[ el ] );
        elemlist.clear();
    }
    return cc;
}