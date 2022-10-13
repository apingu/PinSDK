// 1.採用一個音效一個獨立序的播放方式 因為會有 讀完檔案卻還沒有放完的狀況 而在等待放完時 會照成其他的音效 卡住
// 2.load建立序 play只是讓他播放 delete才會消滅序
//
//
//
//
//
//
#include "VorbisDriver.h"
#include <process.h>
#include <T_Lock.h>
/*
FILE *fp = NULL;

{
fp = fopen("debug.txt", "w+");
fprintf(fp, "Decode : 1");
fclose(fp);
fp = NULL;
}
*/
// ?????


#include <mmsystem.h>


// DLL???
extern "C" __declspec( dllexport ) Vox * __stdcall CreateVox()
{
    return new VorbisDriver();    // Vox
}

bool __stdcall VorbisDriver::Delete()
{
    delete this;

    return true;
}

__stdcall VorbisDriver::VorbisDriver()
    : fp( NULL ), pInfo( NULL ),
    hWaveOut( NULL ), hSemaphore( NULL ), hThread( NULL ), ThreadState( THREAD_STOP ),
    IsVorbisDriverInitialized( false ), DecodePage( 0 ), LoopCounter( -1 ), Volume( 1 ), SeekOffset( 0 ), IsFade( false ),
    pSJISBuffer( NULL )
{
    // ?
    hSemaphore = CreateSemaphore( NULL, MAXBUFFER, MAXBUFFER, NULL );

    // ?
    if( waveOutGetNumDevs() != 0 ) IsDeviceExist = true;
    else IsDeviceExist = false;
}

__stdcall VorbisDriver::~VorbisDriver()
{
    // 
    if( pSJISBuffer ) delete[] pSJISBuffer;

    Release();

    // 
    if( hSemaphore )
    {
        CloseHandle( hSemaphore );
        hSemaphore = NULL;
    }
}

bool __stdcall VorbisDriver::Load( char* pFileName )
{
    // ?
    if( !IsDeviceExist )
        return false;

    // ??
    if( !hSemaphore )
        return false;


    // ?
    fp = fopen( pFileName, "rb" );
    if( !fp )
        return false;


    // LOAD->PLAY->STOP->LOADLOAD->PLAY->LOAD
    if( ThreadState == THREAD_RUN )
        return false;

    strcpy( fname, pFileName );

    if( ov_open( fp, &vf, NULL, 0 ) < 0 )
        return false;

    pInfo = ov_info( &vf, -1 );
    vorbis_comment* pComment = ov_comment( &vf, -1 );
    Comment.user_comments = pComment->user_comments;
    Comment.comment_lengths = pComment->comment_lengths;
    Comment.comments = pComment->comments;
    Comment.vendor = pComment->vendor;

    IsVorbisDriverInitialized = true;

    //?
    WAVEFORMATEX wfex;
    ZeroMemory( &wfex, sizeof( wfex ) );
    wfex.wFormatTag = WAVE_FORMAT_PCM;
    wfex.nChannels = pInfo->channels;
    wfex.nSamplesPerSec = pInfo->rate;
    wfex.wBitsPerSample = 16;
    wfex.nBlockAlign = wfex.nChannels * wfex.wBitsPerSample / 8;
    wfex.nAvgBytesPerSec = wfex.nBlockAlign * wfex.nSamplesPerSec;
    wfex.cbSize = 0;


    if( waveOutOpen( &hWaveOut,
        WAVE_MAPPER,
        &wfex,
        ( DWORD_PTR )WaveProc,
        ( DWORD_PTR )this,
        //            (DWORD)WaveProc, 
        //            (unsigned long)this,
        WAVE_ALLOWSYNC | CALLBACK_FUNCTION ) != MMSYSERR_NOERROR )
        return false;


    // waveOut
    for( int i = 0; i < MAXBUFFER; i++ )
    {
        memset( &WaveHdr[ i ], 0, sizeof( WAVEHDR ) );
        WaveHdr[ i ].lpData = new char[ BUFFERSIZE ];
    }


    // 用update的方式 所以 不用thread
    if( !hThread )
    {
        DWORD ThreadID;
        ThreadState = THREAD_PAUSE;
        hThread = ( HANDLE )_beginthreadex( NULL, 0, ( THREADFUNC )ThreadProc, ( LPVOID )this, 0, ( unsigned int* )&ThreadID );
        if( hThread == 0 )
            return false;
    }

    return true;
}

bool __stdcall VorbisDriver::Release()
{
    // ?
    if( !IsDeviceExist ) return false;

    // ?
    if( hThread )
    {
        ThreadState = THREAD_STOP;
        WaitForSingleObject( hThread, 5000 ); // ???

        if( CloseHandle( hThread ) == 0 ) return false;
        hThread = NULL;
    }

    // waveOut
    if( hWaveOut )
    {
        waveOutReset( hWaveOut );
        for( int i = 0; i < MAXBUFFER; i++ )
        {
            while( waveOutUnprepareHeader( hWaveOut, &WaveHdr[ i ], sizeof( WAVEHDR ) ) == WAVERR_STILLPLAYING ) Sleep( 100 );
            delete[] WaveHdr[ i ].lpData;
        }
        waveOutClose( hWaveOut );
        hWaveOut = NULL;
    }

    // oggVorbis
    if( IsVorbisDriverInitialized == true )
    {
        ov_clear( &vf );
        IsVorbisDriverInitialized = false;
    }

    if( fp ) fclose( fp );
    fp = NULL;

    ThreadState = THREAD_STOP;
    DecodePage = 0;
    LoopCounter = 0;
    Volume = 1;
    SeekOffset = 0;
    IsFade = false;

    return true;
}

// : -1: ? 0:  1: ?
int __stdcall VorbisDriver::Decode()
{
    // ?
    if( !IsDeviceExist ) return -1;

    int eof = 1;
    int Counter = 0;

    // 
    while( eof )
    {
        int CurrentSection;
        // 0, 2, 1  little endian, 16bit, signed
        long Result = ov_read( &vf, ConvBuffer, sizeof( ConvBuffer ), 0, 2, 1, &CurrentSection );
        if( Result == 0 ) eof = 0;
        else if( Result < 0 );// ?? 
        else
        {
            // ????
            short* pBuffer = ( short* )ConvBuffer;
            int DataNum = Result / 2;
            for( int i = 0; i < DataNum; i++ )
            {
                pBuffer[ i ] = ( short )( ( float )pBuffer[ i ] * Volume );

                // 
                if( IsFade )
                {
                    Volume += ( EndVolume - StartVolume ) / ( ( ( float )FadeTime / 1000 ) * pInfo->rate * pInfo->channels );
                    if( StartVolume < EndVolume )
                    {
                        if( Volume > EndVolume ) Volume = EndVolume;
                    }
                    else
                    {
                        if( Volume < EndVolume ) Volume = EndVolume;
                    }
                }
            }

            memcpy( WaveHdr[ DecodePage ].lpData + Counter, ConvBuffer, Result );
            Counter += Result;
            if( Counter > BUFFERSIZE - ( 4096 + 10 ) ) break; // (10)
        }
    }

    //printf("Decodeing...%d\n",Counter);

        // waveOut?
    if( Counter != 0 )
    {
        if( waveOutUnprepareHeader( hWaveOut, &WaveHdr[ DecodePage ], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR )
        {
            return -1;
        }

        WaveHdr[ DecodePage ].dwBytesRecorded = 0;
        WaveHdr[ DecodePage ].dwFlags = 0;
        WaveHdr[ DecodePage ].dwLoops = 0;
        WaveHdr[ DecodePage ].dwUser = 0;
        WaveHdr[ DecodePage ].lpNext = 0;
        WaveHdr[ DecodePage ].reserved = 0;

        WaveHdr[ DecodePage ].dwBufferLength = Counter;


        if( waveOutPrepareHeader( hWaveOut, &WaveHdr[ DecodePage ], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;
        if( waveOutWrite( hWaveOut, &WaveHdr[ DecodePage ], sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) return -1;

        // 
        ++DecodePage;
        if( DecodePage >= MAXBUFFER ) DecodePage = 0;
    }

    if( eof == 0 )
    {
        // 
        DWORD MMTimeOld = 0;
        // 必須要等音效真的已經播完後 才可以移除
        // 否則會撥到一半就被移除
        // 如果回圈設 10 音效尾巴會被切到
        // 如果sleep(20) 一些短促音效 會切到
        for( int i = 0; i < 100; i++ )
        {
            // 30
            Sleep( 30 );
            MMTIME MMTime;
            MMTime.wType = TIME_BYTES;
            if( waveOutGetPosition( hWaveOut, &MMTime, sizeof( MMTIME ) ) != MMSYSERR_NOERROR )
            {
                break;
            }
            if( MMTimeOld == MMTime.u.sample )
            {
                break;
            }
            MMTimeOld = MMTime.u.sample;
        }

        waveOutReset( hWaveOut );
        SeekOffset = 0;
        ov_pcm_seek( &vf, 0 ); // 
    }

    if( Counter == 0 ) return -1;

    return eof;
}

//??
void __stdcall CALLBACK VorbisDriver::WaveProc( HWAVEOUT hWaveOut, UINT msg,
                                                DWORD instance, DWORD param1, DWORD param2 )
{
    switch( msg )
    {
        case WOM_DONE:
            VorbisDriver* pVorbisDriver = ( class VorbisDriver* )( ( DWORD_PTR )instance );
            //            VorbisDriver *pVorbisDriver = (class VorbisDriver *)instance;
            ReleaseSemaphore( pVorbisDriver->hSemaphore, 1, NULL );
            break;
    }
}


// ?

unsigned int __stdcall VorbisDriver::ThreadProc( void* pParam )
{
    // ?ThreadState?
    VorbisDriver* pVorbisDriver = ( VorbisDriver* )pParam;
    while( 1 )
    {
        bool icr = false;
        bool ww = false;
        // 
        while( 1 )
        {
            if( pVorbisDriver->ThreadState == THREAD_STOP ) goto EXIT;        // ?
            if( pVorbisDriver->ThreadState == THREAD_PAUSE ) goto CONTINUE;    // ?
            if( !pVorbisDriver->fp ) goto CONTINUE;                            // ?

            int Result = WaitForSingleObject( pVorbisDriver->hSemaphore, 100 );
            if( Result == WAIT_OBJECT_0 ) break;
            if( Result == WAIT_FAILED || Result == WAIT_ABANDONED ) goto EXIT;
            ww = true;
            Sleep( 50 ); // ?(^^;
        }
        //WaitForSingleObject(pVorbisDriver->hSemaphore, INFINITE);
        //int Result = pVorbisDriver->Decode();
        int Result = -1;
        // ?
        if( pVorbisDriver->LoopCounter != 0 )
        {
            Result = pVorbisDriver->Decode();
        }
        else
        {
            Result = -1;
        }

        //printf("%d\n", Result);
                /*
                if(Result == 0)
                {
                    if(pVorbisDriver->LoopCounter!=-1)
                        --pVorbisDriver->LoopCounter;
                }
                */

        if( Result <= 0 )
        {
            icr = true;
            ReleaseSemaphore( pVorbisDriver->hSemaphore, 1, NULL );
        }

        // 
        if( pVorbisDriver->IsFade )
        {
            DWORD Time = timeGetTime();
            if( pVorbisDriver->FadeStartTime + pVorbisDriver->FadeTime < Time )
            {
                pVorbisDriver->Volume = pVorbisDriver->EndVolume;
                pVorbisDriver->IsFade = false;
            }
        }

        if( icr )
        {
            if( pVorbisDriver->LoopCounter > 0 )
            {
                --pVorbisDriver->LoopCounter;
                if( pVorbisDriver->LoopCounter == 0 )
                {
                    goto EXIT;
                }
            }
        }

CONTINUE:
        //if(!ww)
        Sleep( 50 ); // ?(^^;
    }

EXIT:
    pVorbisDriver->ThreadState = THREAD_STOP;
    //_endthreadex(0);


    return 0;
}

bool __stdcall VorbisDriver::Play()
{
    if( ThreadState == THREAD_PAUSE ) waveOutRestart( hWaveOut ); // 
    ThreadState = THREAD_RUN;

    return true;
}

bool __stdcall VorbisDriver::Pause()
{
    waveOutPause( hWaveOut );
    ThreadState = THREAD_PAUSE;

    return true;
}

bool __stdcall VorbisDriver::SetLoop( int Count )
{
    //if(Count < 0) return false;
    LoopCounter = Count;

    return true;
}


bool __stdcall VorbisDriver::CheckDevice()
{
    if( waveOutGetNumDevs() == 0 ) return false;

    return true;
}

bool __stdcall VorbisDriver::SetVolume( float Volume )
{
    if( Volume > 1.0f ) return false;

    this->Volume = Volume;

    return true;
}

float __stdcall VorbisDriver::GetVolume()
{
    return Volume;
}

// FadeTime: ?
bool __stdcall VorbisDriver::Fade( float StartVolume, float EndVolume, int FadeTime )
{
    // ??
    if( StartVolume > 1.0f || StartVolume < 0.0f ) return false;
    if( EndVolume > 1.0f || EndVolume < 0.0f ) return false;
    if( FadeTime < 0 ) return false;

    this->StartVolume = StartVolume;
    this->EndVolume = EndVolume;
    this->FadeTime = FadeTime;
    FadeStartTime = timeGetTime();
    Volume = StartVolume;
    IsFade = true;

    return true;
}

// : ?
float __stdcall VorbisDriver::GetTotalTime()
{
    return ( ( float )ov_pcm_total( &vf, -1 ) / pInfo->rate ) * 1000;
}

// : ?
float __stdcall VorbisDriver::GetCurrentTime()
{
    MMTIME MMTime;
    MMTime.wType = TIME_BYTES;
    waveOutGetPosition( hWaveOut, &MMTime, sizeof( MMTIME ) );

    return ( float )MMTime.u.cb / pInfo->rate / pInfo->channels / 2 * 1000 + SeekOffset;
}

// Time: ?
bool __stdcall VorbisDriver::Seek( float Time )
{
    if( Time > GetTotalTime() ) return false;
    ov_pcm_seek( &vf, ( ogg_int64_t )( Time / 1000 * pInfo->rate ) );

    waveOutReset( hWaveOut );
    SeekOffset = Time;

    return true;
}

VOXSTATE __stdcall VorbisDriver::GetStatus()
{
    if( ThreadState == THREAD_RUN && LoopCounter > 0 ) return STATE_PLAYING;
    //if(LoopCounter == 0) return STATE_STOP;
    if( ThreadState == THREAD_STOP ) return STATE_STOP;

    return STATE_PAUSE;
}

VOXCOMMENT* __stdcall VorbisDriver::GetComment()
{
    return &Comment;
}

// UTF-8Windows95?
// http://support.microsoft.com/default.aspx?scid=kb;ja;175392
char* __stdcall VorbisDriver::ParseComment( char* pFieldName, CHARACTERCODE CharacterCode )
{
    for( int i = 0; i < Comment.comments; ++i )
    {
        if( strncmp( Comment.user_comments[ i ], pFieldName, strlen( pFieldName ) ) == 0 )
        {
            char* pComment = Comment.user_comments[ i ] + strlen( pFieldName ) + 1;
            int BufferSize = MultiByteToWideChar( CP_UTF8, 0, pComment, -1, NULL, 0 );
            WCHAR* pUnicodeBuffer = new WCHAR[ BufferSize + 16 ]; // ?
            ZeroMemory( pUnicodeBuffer, sizeof( pUnicodeBuffer ) );
            MultiByteToWideChar( CP_UTF8, 0, pComment, -1, pUnicodeBuffer, BufferSize );
            BufferSize = WideCharToMultiByte( CP_ACP, 0, pUnicodeBuffer, -1, NULL, 0, NULL, NULL );
            if( pSJISBuffer ) delete[] pSJISBuffer;
            pSJISBuffer = new CHAR[ BufferSize + 16 ]; // ?
            ZeroMemory( pSJISBuffer, sizeof( pSJISBuffer ) );
            WideCharToMultiByte( CP_ACP, 0, pUnicodeBuffer, -1, pSJISBuffer, BufferSize, NULL, NULL );

            delete[] pUnicodeBuffer;

            return pSJISBuffer;
        }
    }

    return NULL;
}
