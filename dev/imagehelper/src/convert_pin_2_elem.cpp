//
//
//
//
//   HEADLE.RES   headle data
//   .anm         action table
//   IMAGE.RES    image  list
//   E.S   elm    string
//
//
//
//
//
//
//
#include <vector>
#include <pf-string.h>
#include <pf-debug.h>
#include <game\HELEM.h>
#include <gadget\file_operator.h>
#include "convert_image.h"


int PinToElem( const char* Pinpath, const char* Pelmpath )
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_LDIR];
    char fname[_MAX_LFNAME];
    char ext[_MAX_LEXT];
    _splitpath(Pelmpath, drive, dir, fname, ext);

    HElem2d elmdata;
    elmdata.Visible        = true;
    elmdata.Enable         = true;
    elmdata.Moveable       = true;
    elmdata.Dragable       = false;

    FILE    *f;
   
    //wtire data
    char HDPath[_MAX_LPATH];
    _makepath(HDPath, drive, dir, "HEADLE", "RES");

    if( ( f=fopen( HDPath, "wb" ) ) == NULL )//開檔如果-1則失敗，傳回-1
    {
        //char msg[256];
        //sprintf( msg, "Convert pin to elem write header file %s failure !!!", HDPath );
        Pf_ERROR( "PinToElem: Convert pin to elem write header file %s failure !!!", HDPath );
        return -1;
    }
    
    fwrite(&elmdata, 1, sizeof(elmdata), f);
    fclose(f);

    //write route List
    /*
    fseek(f, elmdata.RouteOffset, SEEK_SET);
    int rs = RouteList.size();
    fwrite(&rs, sizeof(int), 1, f);  //write point count
    

    for(int pointcount = 0 ; pointcount < RouteList.size() ; pointcount++)
        fwrite(&RouteList[pointcount], sizeof(PCPOINT), 1, f);

    */

    //write action table
    char ATPath[_MAX_LPATH];
    _makepath(ATPath, drive, dir, fname, "anm");

    FILE* atf;

    if((atf=fopen(ATPath,"wb")) == NULL)//開檔如果-1則失敗，傳回-1
    {
        //char msg[256];
        //sprintf( msg, "Convert pin to elem write anim file %s failure !!!", ATPath );
        Pf_ERROR( "PinToElem: Convert pin to elem write anim file %s failure !!!", ATPath );
        return -1;
    }
    WORD maxdir = 1;
    fwrite(&maxdir, sizeof(WORD), 1, atf);  //write act count

    dword_t angle = 0;
    fwrite(&angle, sizeof(dword_t), 1, atf);  //write step count

    WORD len = 1;
    fwrite(&len, sizeof(WORD), 1, atf);

    dword_t index = 0;
    fwrite(&index, sizeof(dword_t), 1, atf);

    fclose(atf);


    //write image
    char ILPath[_MAX_LPATH];
    _makepath(ILPath, drive, dir, "IMAGES", "RES");
    
    std::vector<const char*> ImageFilePath;
    ImageFilePath.push_back( Pinpath );
    

    FileSystem pk(ILPath, "wb");
    pk.Packagefiles( ImageFilePath );
    pk.close();


    //to combine file
    FileSystem elm_combine( Pelmpath, "wb" );
    
    std::vector<const char*> paths;
    paths.push_back(HDPath);
    //paths.push_back(ATPath);
    paths.push_back(ILPath);

    WORD fmtt;
    char* fmt = (char*)(&fmtt);
    fmt[0] = 'E';
    fmt[1] = 'M';
    elm_combine.Packagefiles(paths, fmtt, _DELETE_SORUCE_FILE);
    elm_combine.close();

    return 1;

}