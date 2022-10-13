#include "VorbisDriver.h"
#include <vector>
using namespace std;

struct VOXDATA
{
    VorbisDriver* pVorbisDriver;
    int ID;
};

vector<VOXDATA> VoxDataVector;

extern "C" __declspec( dllexport ) int __stdcall VoxLoad( char* pFileName )
{
    static int IDCounter = 0;
    VOXDATA VoxData;
    VoxData.pVorbisDriver = new VorbisDriver();
    if( !VoxData.pVorbisDriver->Load( pFileName ) )
    {
        delete VoxData.pVorbisDriver;
        return -1; // ?
    }
    VoxData.ID = IDCounter;
    VoxDataVector.push_back( VoxData );
    ++IDCounter;

    return VoxData.ID;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxPlay( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->Play();
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxPause( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->Pause();
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxRelease( int ID )
{
    //    for(unsigned int i = 0; i < VoxDataVector.size(); i++){
    //        if(VoxDataVector[i].ID == ID){
    //            return VoxDataVector[i].pVorbisDriver->Release();
    //        }
    //    }

    vector<VOXDATA>::iterator it;
    for( it = VoxDataVector.begin(); it != VoxDataVector.end(); it++ )
    {
        if( ( *it ).ID == ID )
        {
            bool Result = ( *it ).pVorbisDriver->Release();
            if( !Result ) return false;
            Result = ( *it ).pVorbisDriver->Delete(); // VoxDeleteC++?????
            VoxDataVector.erase( it );
            if( !Result ) return false;

            return true;
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxSetLoop( int ID, int Count )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->SetLoop( Count );
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxCheckDevice( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->CheckDevice();
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxSetVolume( int ID, int Volume )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            // C++Volume:0.01.0?010000?
            return VoxDataVector[ i ].pVorbisDriver->SetVolume( ( float )Volume / 10000 );
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) int __stdcall VoxGetVolume( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            // C++Volume:0.01.0?010000?
            return ( int )( VoxDataVector[ i ].pVorbisDriver->GetVolume() * 10000 );
        }
    }

    return -1;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxFade( int ID, int StartVolume, int EndVolume, int FadeTime )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            // C++Volume:0.01.0?010000?
            return VoxDataVector[ i ].pVorbisDriver->Fade( ( float )StartVolume / 10000, ( float )EndVolume / 10000, FadeTime );
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxDelete( int ID )
{
    vector<VOXDATA>::iterator it;
    for( it = VoxDataVector.begin(); it != VoxDataVector.end(); it++ )
    {
        if( ( *it ).ID == ID )
        {
            // C++?????VoxRelease
//            bool Result = (*it).pVorbisDriver->Delete();
//            VoxDataVector.erase(it);
//            return Result;

            // 
            return true;
        }
    }

    return false;
}

extern "C" __declspec( dllexport ) int __stdcall VoxGetTotalTime( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            // C++floatint?
            return ( int )VoxDataVector[ i ].pVorbisDriver->GetTotalTime();
        }
    }

    return -1;
}

extern "C" __declspec( dllexport ) int __stdcall VoxGetCurrentTime( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            // C++floatint?
            return ( int )VoxDataVector[ i ].pVorbisDriver->GetCurrentTime();
        }
    }

    return -1;
}

extern "C" __declspec( dllexport ) bool __stdcall VoxSeek( int ID, int Time )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return ( int )VoxDataVector[ i ].pVorbisDriver->Seek( ( float )Time );
        }
    }

    return false;
}

//  0: PAUSE 1: ? -1: ?
extern "C" __declspec( dllexport ) int __stdcall VoxGetStatus( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            VOXSTATE State = VoxDataVector[ i ].pVorbisDriver->GetStatus();
            if( State == STATE_PAUSE ) return 0;
            if( State == STATE_PLAYING ) return 1;
        }
    }

    return -1;
}

extern "C" __declspec( dllexport ) VOXCOMMENT * __stdcall VoxGetComment( int ID )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->GetComment();
        }
    }

    return NULL;
}

extern "C" __declspec( dllexport ) char* __stdcall VoxParseComment( int ID, char* pFieldName, CHARACTERCODE CharacterCode )
{
    for( unsigned int i = 0; i < VoxDataVector.size(); i++ )
    {
        if( VoxDataVector[ i ].ID == ID )
        {
            return VoxDataVector[ i ].pVorbisDriver->ParseComment( pFieldName, CharacterCode );
        }
    }

    return NULL;
}
