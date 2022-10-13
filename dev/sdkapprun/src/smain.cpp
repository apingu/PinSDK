
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <string.h>

#include <vector>
#include <string>

#include <pf-def.h>
#include <pf-file.h>
#include <pf-string.h>

#include <pf-stdlib.h>

#include <pf-ziplib.h>
#include <algorithmic/crypt-crc32.h>
#include <algorithmic/crypt-crc64.h>

#include "template/catalog.inl"

#include <algorithmic/crypt-md5.h>
#include <template/vmapl.inl>


#include "sdkbundle.h"


#pragma comment (lib, "utility.lib")
#pragma comment (lib, "ziplib.lib")
#pragma comment (lib, "xdatabase.lib")
#pragma comment (lib, "template.lib")
#pragma comment (lib, "algorithmic.lib")
#pragma comment (lib, "parser.lib")
#pragma comment (lib, "netsocket.lib")
#pragma comment (lib, "cyberspace.lib")

#ifdef _WINDOWS
    #ifdef _UNICODE
        #pragma comment (lib, "helper_u.lib")
        #pragma comment (lib, "gadget_u.lib")
    #else
        #pragma comment (lib, "helper.lib")
        #pragma comment (lib, "gadget.lib")
    #endif
#endif

#pragma comment (lib, "vmcengine.lib")
#pragma comment (lib, "world2d.lib")
#pragma comment (lib, "imagehelper.lib")
#pragma comment (lib, "media.lib")
#pragma comment (lib, "painter.lib")
#pragma comment (lib, "imagehelper.lib")






int main()
{
    printf( "hello \n" );


    office::XlFile xlfile;
    xlfile.Load( _T("password_test.xls"), _T( "ab50903783" ) );
    xlfile.SetPassword( _T( "ab50903783" ) );
    xlfile.Save();
    
    _tstring cella1 = xlfile.SelectCell( _T( "A1" ) )->GetCell2CStr();



    net_helper::sftp::ToolKit uploader;
    //bool ret = uploader.DownloadFile( "root", "apinguap1nGu@l1z0", remote_path.c_str(), "file.txt" );
    bool ret = uploader.UploadFile( "root", "ap1nguaapinap1nGu@1l2o", "upload.txt", "sftp://123.193.213.74/~/update01.txt" );


    //_tstring cmpa1 = _T( "期初庫存(05/26)期初庫存(06/09)" );
    //_tstring cmpa2 = _T( "*期*庫存(*)期*存(*06)" );

    //_tstring cmpa1 = _T( "AABEFG(05/26)" );
    //_tstring cmpa2 = _T( "AABEFG(*)" );

    pfstl::vmapl< _tstring > testmap;
    testmap.init( 20 );

    testmap.insert( 25009, _T( "AAABBBB" ) );
    _tstring vv = testmap[ 25009 ];


    pfstl::_catalog< _tstring, _tstring, int > test;

    test.insert( _T( "AA" ), _T( "BB" ), 3 );

    pfstl::_catalog< _tstring, _tstring, int >::iterator iter = test.find( _T( "AA" ), _T( "BB" ) );
    if( iter != test.end() )
    {
        int hhh = 0;
    }




    return 1;
}