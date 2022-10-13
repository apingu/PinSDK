#include "DynStruct.h"
#include "..\Tools\str.h"
#include <io.h>

void CDynStruct::ReadStruct( const char* path )
{
    FILE* f;

    if( ( f = fopen( path, "rb" ) ) == NULL )
        return ;

    int filesize    = filelength( fileno( f ) );
    char* fileptr   = new char[ filesize ];
    fread( fileptr, filesize, 1, f );

    char String[ _MAX_STRING_SIZE ][ _MAX_STRING_LEN ];
    memset( String, NULL, _MAX_STRING_SIZE * _MAX_STRING_LEN );
    int size        = DivStr( fileptr, filesize, String );

    bool StartStruct= false;
    int structsize  = 0;

    for( int StrIndex = 0;StrIndex < size + 1;StrIndex++ )
    {
        if( strcmp( String[ StrIndex ], "{" ) == 0 )
        {
            StrIndex++;
            StartStruct = true;
        }
        if( StartStruct )
        {
            //PCC::String fieldtype;
            //fieldtype = String[StrIndex];
            //m_datatype.push_back(fieldtype);
            structsize = structsize + SelectTypeSize( String[ StrIndex ] );

            StrIndex++;
            //DivChar(String[StrIndex], ';');
            //PCC::String fieldstring;
            //fieldstring = String[StrIndex];
            //m_dataField.push_back(fieldstring);


            if( strcmp( String[ StrIndex + 1 ], "//" ) == 0 )
                StrIndex++;
        }
        if( String[ StrIndex + 1 ][ 0 ] == '}' )
            break;
    }
    m_dataSpace.Allot( structsize );
    delete [] fileptr;
}


void CDynStruct::newField( const char* type,void* ptr )
{
    if( strcmp( type, "BYTE" ) == 0 )
        ptr = new unsigned char;
    else if( strcmp( type, "WORD" ) == 0 )
        ptr = new unsigned short;
    else if( strcmp( type, "short" ) == 0 )
        ptr = new short;
    else if( strcmp( type, "DWORD" ) == 0 )
        ptr = new unsigned int;
    else if( strcmp( type, "char" ) == 0 )
        ptr = new char;
    else if( strcmp( type, "unsigned char" ) == 0 )
        ptr = new unsigned char;
    else if( strcmp( type, "int" ) == 0 )
        ptr = new int;
    else if( strcmp( type, "unsigned int" ) == 0 )
        ptr = new unsigned int;
    else if( strcmp( type, "long" ) == 0 )
        ptr = new long;
    else if( strcmp( type, "unsigned long" ) == 0 )
        ptr = new unsigned long;
    else if( strcmp( type, "float" ) == 0 )
        ptr = new float;
    else if( strcmp( type, "double" ) == 0 )
        ptr = new double;
    else if( strcmp( type, "long double" ) == 0 )
        ptr = new long double;
}