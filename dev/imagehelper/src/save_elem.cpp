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
#include <gadget\file_operator.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "save_elem.h"


int SaveElem( const char* filename, const HElem2d* elem, std::vector<const char*> Images )
{
    char drive[ _MAX_DRIVE ];
    char dir[ _MAX_LDIR ];
    char fname[ _MAX_LFNAME ];
    char ext[ _MAX_LEXT ];
    _splitpath( filename, drive, dir, fname, ext );

    //========wtire header
    char HDpath[ _MAX_LPATH ];
    _makepath( HDpath, drive, dir, "HEADLE", "RES" );

    FileOperator pf;
    if( ( pf.open( HDpath, "wb" ) ) == NULL )//開檔如果-1則失敗，傳回-1
    {
        //char message[ _MAX_MSG ];
        //sprintf(message, "save file %s failure !!!", HDpath);
        //DebugMessage(message, "Element2d");
        Pf_ERROR( "SaveElem: save file %s failure !!!", HDpath );
        return 0;
    }

    pf.write( elem, 1, sizeof( HElem2d ) );
    pf.close();

    //==========write action table==========
    /*
        char ATpath[ _MAX_LPATH ];
        _makepath(ATpath, drive, dir, "A", "T");
        m_AnimTable.Save(ATpath);
    */


    //==========write image==========
    char ILpath[ _MAX_LPATH ];
    _makepath( ILpath, drive, dir, "IMAGES", "RES" );

    FileSystem image;
    if( ( image.open( ILpath, "wb" ) ) == NULL )//開檔如果-1則失敗，傳回-1
    {
        //char msg[ _MAX_MSG ];
        //sprintf(msg, "save animation file %s failure -_-b", filename);
        Pf_ERROR( "SaveElem: save animation file %s failure -_-b", filename );
        return 0;
    }
    image.Packagefiles( Images );
    image.close();

    //==========to combine file==========
    FileSystem elm_combine( filename, "wb" );
    std::vector<const char*> paths;
    paths.push_back( HDpath );
    //paths.push_back(ATpath);
    paths.push_back( ILpath );

    word_t fmtt;
    char* fmt = ( char* )( &fmtt );
    fmt[ 0 ] = 'E';
    fmt[ 1 ] = 'M';

    elm_combine.Packagefiles( paths, fmtt, _DELETE_SORUCE_FILE );
    elm_combine.close();

    return 1;
}
