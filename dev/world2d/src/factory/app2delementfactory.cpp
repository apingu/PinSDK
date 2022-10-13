//Scene_Editer.cpp  scene edit class
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
//                                                                  Pin
//

#include "App2dFactory.h"
#include <Debug.h>
#include <PfFileOperator.h>

const int _MAP_RECT_LIMIT_ = 50;


namespace GSO
{
    namespace Global
    {
        extern bool g_Element2dEffectChildren;
    };
};

namespace App2dFactory
{

    Element2d::Element2d()
    {
    }
    
    Element2d::~Element2d()
    {
    }
    
    bool Element2d::CheckFormat( const char* filename )
    {
        IO::PfFileSystem pf;
        
        if ( pf.open( filename ) == NULL )
        {
            return false;
        }
        
        /* is a element file? */
        WORD fmtt;
        char* fmt = ( char* )( &fmtt );
        fmt[0] = 'E';
        fmt[1] = 'M';
        
        if ( pf.checkFormat( fmtt ) != _PF_OK )
        {
            return false;
        }
        
        return true;
    }
    
    int Element2d::GetAllImage( GSO::Element2d* pElement, std::vector<CPIcon*>& Image )
    {
        Image.clear();
        
        for ( int i = 0; i < pElement->GetImageCount(); i++ )
        {
            Image.push_back( ( CPIcon* )pElement->GetImage( i ) );
        }
        
        return 1;
    }
    
    int Element2d::GetAllImage( const char* filename, std::vector<CPIcon*>& Image )
    {
        char message[ _MAX_MSG ];
        char is[ _MAX_FNAME ];
        sprintf( is, "%s%s%s", filename, "?", "IMAGES.RES" );
        IO::PfFileSystem elemf;
        
        if ( elemf.open( is, "rb" ) == NULL )
        {
            sprintf( message, "open elem file %s failure !!!", filename );
            Pf_DEBUG( message, "Element factory" );
            elemf.close();
            return 0;
        }
        
        char iconfname[ _MAX_PATH ];
        char nobuf[ 10 ];
        UINT sum = elemf.FileCount();
        
        for ( UINT i = 0; i < sum; i++ )
        {
            CPIcon* icon = new CPIcon;
            sprintf( iconfname,
                     "%s|%s",
                     is,
                     //"I.L",
                     itoa( i, nobuf, 10 ) );
                     
            if ( icon->Load( iconfname ) != 1 )
            {
                sprintf( message, "open icon file %s failure !!!", iconfname );
                Pf_DEBUG( message, "Element factory" );
            }
            
            Image.push_back( icon );
        }
        
        return 1;
    }
    
    
}
