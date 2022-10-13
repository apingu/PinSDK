#include <stdio.h>
#include <windows.h>
#include <vector>
//#include <al.h>
//#include <alc.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <assert.h>
#include "SoundCodec.h"



//#ifdef _DEBUG
// #pragma comment ( lib, "vorbisenc_static_d.lib" )
// #pragma comment ( lib, "vorbisfile_static_d.lib" )
// #pragma comment ( lib, "vorbis_static_d.lib" )
// #pragma comment ( lib, "ogg_static_d.lib" )
//#else
//#pragma comment ( lib, "vorbisenc_static.lib" )
//#pragma comment ( lib, "vorbisfile_static.lib" )
//#pragma comment ( lib, "vorbis_static.lib" )
//#pragma comment ( lib, "ogg_static.lib" )
//#endif




bool LoadWavePlain(const char *wavfile,char **dptr,int *flen,char *fmt24);

static int find_tag(FILE *fptr,const char *tag4,int *len)
{
	char fourcc[4];
	int i;
	bool b;

	for(b=false;!b;)
	{
		// search for "fmt "
		fread(fourcc,4,1,fptr);	// tag
		fread(len,4,1,fptr);	// length
		b=!memcmp(fourcc,tag4,4);

		if(b)
		{
			break;			// The data we need
		}
		if(*len>0)
		{
			i=fseek(fptr,*len,SEEK_CUR);
		}
		if(i)
		{
			break;
		}
	}
	return b;
}

// Get wave format and sample length for a wave file.
static int get_wave_format(FILE *fptr,WAVEFORMATEX *wft,long *datalen)
{
	char fourcc[4];
	int ivar,hl,sk;

	fread(fourcc,4,1,fptr);
	if(memcmp(fourcc,"RIFF",4))
		return 0;
	fread(&ivar,4,1,fptr);	// file length
	fread(fourcc,4,1,fptr);
	if(memcmp(fourcc,"WAVE",4))
		return 0;
	if(!find_tag(fptr,"fmt ",&ivar))
		false;
	// since many structures are bigger than WAVEFORMATEX, we need
	// to allocate buffer ourselves.
	sk=0;
	if(ivar>24){	// we can only support max 24 bytes of headers
		sk=ivar-24;	ivar=24;
	}
	memset(wft,0,sizeof(WAVEFORMATEX));	// need to clear data not used.
	fread(wft,ivar,1,fptr);	hl=ivar;
	if(sk)	fseek(fptr,sk,SEEK_CUR);
	// skip all other useless chunks
	if(!find_tag(fptr,"data",&ivar))
		return 0;
	*datalen=ivar;	// 但還不讀資料(此時應該指標放在資料開始位置)
	return hl;
}

// 只載入, 不放 DSB...(可用 XBox 的 SetBufferData 設?)
bool LoadWavePlain(const char *wavfile,char **dptr,int *flen,char *fmt24)
{
	WAVEFORMATEX *wftp;	// 標準 WAVEFORMAT* 應該是低於 24 bytes
	FILE *wavfp;
	long dlen,hl;
	bool b=false;
	char *buf;

	wavfp=fopen(wavfile,"rb");
	if(!wavfp)	goto _e_ncf;
	wftp=(WAVEFORMATEX *)fmt24;
	hl=get_wave_format(wavfp,wftp,&dlen);
	if(!hl)	goto _e_cf;
	if(wftp->wFormatTag!=WAVE_FORMAT_PCM)	// Windows only support this
		goto _e_cf;

	if(flen)
		*flen=dlen;
	*dptr=buf=new char[dlen];
	if(!buf)	goto _e_cf;
	fread(buf,dlen,1,wavfp);
	b=true;
_e_cf:
	fclose(wavfp);
_e_ncf:
//	if(!b)	log_printf(DSL_ERROR,"CreateDSBfromWav: file %s error\n",wavfile);	// error logging
	return b;
}

bool alutLoadWavFile(const char *filename, ALvoid **buffer,ALenum *fmt,ALsizei *len,ALsizei *freq)
{

	/*
	char *dptr;
	int dlen;
	char buf24[24];
	WAVEFORMATEX *wft;
	if(!LoadWavePlain(filename,&dptr,&dlen,buf24))
		return false;
	// 接著開始填入上面欄位
	wft = (WAVEFORMATEX *)buf24;
	*len = dlen;
	*freq = wft->nSamplesPerSec;
	if(wft->nChannels == 1)
{
		if(wft->wBitsPerSample == 8)
			*fmt = AL_FORMAT_MONO8;
		else
			*fmt = AL_FORMAT_MONO16;
		return true;
	}
	else if(wft->nChannels == 2)
	{
		if(wft->wBitsPerSample == 8)
			*fmt = AL_FORMAT_STEREO8;
		else
			*fmt = AL_FORMAT_STEREO16;
		return true;
	} else
		return false;
		*/
	return false;
}

int ogg_decode_to_new_memory(const char *oggfile,char **memptr,vorbis_info *info)
{
	int size,err,eof,ret,pos,csection;
	char *buffer;
	OggVorbis_File vf;
	vorbis_info *vi;
	FILE *fptr;
	
	fptr=fopen(oggfile,"rb");
	if(!fptr)	return -1001;
	err=ov_open(fptr,&vf,NULL,0);
	if(err<0)	return err;
	vi=ov_info(&vf,-1);
	
	size= vi->channels * 2 * ov_pcm_total(&vf,-1);
	buffer=(char *)malloc(size);
	if(!buffer)
	{
		err=-1002;	goto _err_over;
	}
	eof=pos=0;
	while(!eof)
	{
		ret=ov_read(&vf,buffer+pos,1024,0,2,1,&csection);
		if(ret==0)	eof=1;	// EOF reached.
		else if(ret<0){	err=ret;	break;	}
		else pos+=ret;
	}
	ret=pos;
_err_over:
	if(ret<0 && buffer)	free(buffer);	else	*memptr=buffer;
	if(ret>=0 && info)
		memcpy(info,vi,sizeof(vorbis_info));
	ov_clear(&vf);	// finished!
	fclose(fptr);
	return ret;
}


/*

bool alutLoadOggFile(const char *filename, ALvoid **buffer,ALenum *fmt,ALsizei *len,ALsizei *freq)
{
	int l;
	vorbis_info vinfo;
	char *dptr;

	l=ogg_decode_to_new_memory(filename,&dptr,&vinfo);
	if(l>0)
	{
		*buffer = dptr;
		*len = l;
		*freq = vinfo.rate;
		if(vinfo.channels == 1)
		{
			*fmt = AL_FORMAT_MONO16;
			return true;
		}
		else if(vinfo.channels == 2)
		{
			*fmt = AL_FORMAT_STEREO16;
			return true;
		}
		else
			return false;
	} else
		return false;
}
*/

// ===========================================================================
// 之後將 OpenAL 的實做放這...
// 實做部份
#define MAX_SOUND_SOURCES	32

// 改成最少和最多的需求
/*
#define MIN_SOURCE_REQUIRED	8
#define MAX_SOURCE_REQUIRED	32
*/
#define MAX_SOUND_BUFFERS	MAX_SOUND_SOURCES

// This is necessary
//#pragma comment(lib,"OpenAL32.lib")

ALCdevice	*g_pDevice = NULL;
ALCcontext	*g_pContext = NULL;

// Buffer & Sources
ALuint	   g_PS_ErrorCode;
char       g_PS_ErrorMsg[_MAX_MSG];

// 不同格式音效檔載入器
bool alutLoadWavFile(const char *filename, ALvoid **buffer,ALenum *fmt,ALsizei *len,ALsizei *freq);
bool alutLoadOggFile(const char *filename, ALvoid **buffer,ALenum *fmt,ALsizei *len,ALsizei *freq);

ALuint GetALErrorCode()
{
	return g_PS_ErrorCode;
}
const char *GetALErrorMessage()
{
	return g_PS_ErrorMsg;
}

// buffer object

class PSBuffer : public ISoundBuffer
{
	friend class PSSource;

public:

	PSBuffer()
	{
		m_UseCount=0;
		m_SndData=NULL;
		m_BuffNum=0xffffffff;
		strcpy( m_Filename, "" );
	}
	~PSBuffer()
	{
		if(m_BuffNum == 0xffffffff)
			return;
		alDeleteBuffers(1,&m_BuffNum);
		g_PS_ErrorCode = alGetError();
		assert(g_PS_ErrorCode == AL_NO_ERROR);
	}
	bool Init()
	{
		alGenBuffers(1,&m_BuffNum);
		g_PS_ErrorCode = alGetError();
		if(g_PS_ErrorCode != AL_NO_ERROR)
        {
			strcpy( g_PS_ErrorMsg, "Buffer init failed!" );
			return false;
		} else
			return true;
	}
	bool LoadFile(const char *filename)
	{
		const char *ext;
		ALenum fmt;
		ALsizei len,freq;
		ALvoid *ptr;
		ext = filename+(strlen(filename)-4);
		if(0==_stricmp(ext,".wav"))
		{
			if(!alutLoadWavFile(filename,&ptr,&fmt,&len,&freq))
				return false;
		}
		else if(0==_stricmp(ext,".ogg"))
		{
			if(!alutLoadOggFile(filename,&ptr,&fmt,&len,&freq))
				return false;
		}
		else 
		{
			strcpy( g_PS_ErrorMsg, "Unsupported Sound file format." );
			return false;
		}
		if(m_SndData)
		{
			delete m_SndData;
		}
		strcpy( m_Filename, filename );
		m_SndData = (unsigned char *)ptr;
		alBufferData(m_BuffNum,fmt,ptr,len,freq);
		g_PS_ErrorCode = alGetError();
		if((g_PS_ErrorCode != AL_NO_ERROR))
		{
			strcpy( g_PS_ErrorMsg, "Error Loading file" );
			return false;
		}
		return true;
	}
	void Clear()	// 內容清空
	{
		SAFE_DELETE(m_SndData);
		strcpy( m_Filename, "" );
		//m_Filename.clear();
	}
	bool IsFileLoaded(const char *filename)	// 目前是否使用此檔名(和輸入檔名一致?)
	{
		return (0==strcmp(m_Filename, filename));
	}
	bool Empty()	// 使用未使用中
	{
		return (0==strcmp(m_Filename, ""));
	}
	int UseCount()
	{
		return m_UseCount;
	}
private:
	ALuint		   m_BuffNum;
	char           m_Filename[_MAX_PATH];
	unsigned char* m_SndData;
	int			   m_UseCount;
};

class PSSource : public ISoundSource
{
public:
	PSSource()
		
	{
		m_AttachedBuffer=NULL;
		m_SourceNum=0xffffffff;
		SetUsable(true);
	}

	~PSSource()
	{
		if(m_SourceNum == 0xffffffff)
			return;
		alDeleteSources(1,&m_SourceNum);
		g_PS_ErrorCode = alGetError();
		assert(g_PS_ErrorCode == AL_NO_ERROR);
	}
	bool Init()
	{
		alGenSources(1,&m_SourceNum);
		g_PS_ErrorCode = alGetError();
		if(g_PS_ErrorCode != AL_NO_ERROR)
		{
			strcpy( g_PS_ErrorMsg, "Source init failed! Maybe run out of resources");
			return false;
		} else
			return true;
	}
	void AttachBuffer(ISoundBuffer *b)
	{
		PSBuffer *buf = (PSBuffer *)b;
		if(buf == m_AttachedBuffer)
			return;
		if(IsPlaying())
			Stop();
		if(m_AttachedBuffer)
		{
			m_AttachedBuffer->m_UseCount--;	// detach old
		}
		// 改成可以 attach NULL buffer, 如此會造成停止播放
		m_AttachedBuffer = buf;
		if(m_AttachedBuffer)
		{
			alSourcei(m_SourceNum, AL_BUFFER, m_AttachedBuffer->m_BuffNum);
			g_PS_ErrorCode = alGetError();
			if(g_PS_ErrorCode != AL_NO_ERROR)
			{
				strcpy( g_PS_ErrorMsg, "AttachBuffer Failed!" );
				return;
			}
			m_AttachedBuffer->m_UseCount++;
		} 
		else
		{
			alSourcei(m_SourceNum, AL_BUFFER, 0);
			g_PS_ErrorCode = alGetError();
			if(g_PS_ErrorCode != AL_NO_ERROR)
			{
				strcpy( g_PS_ErrorMsg, "DetachBuffer Failed!");
				return;
			}
		}
	}
	bool IsAvailable()	// 是否可以當作未使用加以配置?
	{
		if(m_SourceNum < 0)
			return false;
		return !IsPlaying();
	}
	bool IsPlaying()
	{
		ALint s;
		alGetSourcei(m_SourceNum, AL_SOURCE_STATE, &s);
		g_PS_ErrorCode = alGetError();
		if(g_PS_ErrorCode != AL_NO_ERROR)
		{
			strcpy( g_PS_ErrorMsg, "Chech source state Failed!" );
			return false;
		}
		if(s == AL_PLAYING)
			return true;
		else
			return false;
	}
	void Play(bool looped)
	{
		ALint l;
		ALuint bufn;

		if(m_AttachedBuffer)	bufn = m_AttachedBuffer->m_BuffNum;
		else					bufn = 0;
		(looped)? l=1 : l=0;
		alSourcei(m_SourceNum, AL_LOOPING, l);
		alSourcePlay(m_SourceNum);
		g_PS_ErrorCode = alGetError();
		if(g_PS_ErrorCode != AL_NO_ERROR)
		{
			strcpy( g_PS_ErrorMsg, "Play operation Failed!" );
			return;
		}
	}
	void Stop()
	{
		alSourceStop(m_SourceNum);
	}

	void SetVolume(float val)
	{
		alSourcef(m_SourceNum, AL_GAIN, val);
		g_PS_ErrorCode = alGetError();
		if(g_PS_ErrorCode != AL_NO_ERROR)
		{
			strcpy( g_PS_ErrorMsg, "Volume set Failed!" );
			return;
		}
	}
private:
	ALuint		m_SourceNum;
	PSBuffer	*m_AttachedBuffer;
};

std::vector<PSBuffer*>  g_PS_Buffers;
std::vector<PSSource*>  g_PS_Sources;
//PSBuffer g_PS_Buffers[MAX_SOUND_BUFFERS];
//PSSource g_PS_Sources[MAX_SOUND_SOURCES];
// 其中第0號 source 保留給 BGM, 其他可以任意給 SE 用

void list_al_device_and_cap()
{
	// list all devices
	const ALCchar *sstr,*p,*q;
	std::vector<const char *> devlist;
	ALCdevice *pdev;
	ALCint v1,v2;
	int i;
	char buf[128];

	p = sstr = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	while(*p){
		devlist.push_back((const char *)p);
		p += (1+strlen(p));
	}
	::OutputDebugStringA("Enumerated devices:\n");
	for(i=0;i<devlist.size();i++)
	{
		::OutputDebugStringA(devlist[i]);
		::OutputDebugStringA("\n");
		pdev = alcOpenDevice(devlist[i]);
		if(pdev)
		{
			alcGetIntegerv(pdev,ALC_MAJOR_VERSION,sizeof(ALCint),&v1);
			alcGetIntegerv(pdev,ALC_MINOR_VERSION,sizeof(ALCint),&v2);
			q = alcGetString(pdev, ALC_EXTENSIONS);
			sprintf(buf,"Ver: %d.%d Caps: %s\n",v1,v2,q);
			::OutputDebugStringA(buf);
			alcCloseDevice(pdev);
		} 
		else
		{
			::OutputDebugStringA("Error: Cannot open device\n");
		}
	}
}

// 使用 OpenAL 實做的播放設備
bool PSL_Init()
{
	const char *default_dev = "Generic Hardware";
	
	list_al_device_and_cap();	// 很不幸, 測試以後看來還是找不出想要的規則...
	g_pDevice = alcOpenDevice(default_dev);	// open desired device
	if(!g_pDevice)
	{
		::OutputDebugStringA("Cannot open desired device. Try default one...\n");
		g_pDevice = alcOpenDevice(NULL);	// open default device
	}
	if(g_pDevice)
	{
		g_pContext = alcCreateContext(g_pDevice, NULL);
		if(g_pContext)
		{
			alcMakeContextCurrent(g_pContext);
			g_PS_ErrorCode = alGetError();


			// 開始初始化 source & buffer
// 			for(i=0;i<MAX_SOUND_BUFFERS;i++)
// 			{
// 				g_PS_Buffers[i].Init();
// 			}
// 			for(i=0;i<MAX_SOUND_SOURCES;i++)
// 			{
// 				g_PS_Sources[i].Init();
// 				g_PS_Sources[i].SetVolume(1.0f);
// 			}
			return true;
		}
	}
	if(g_pDevice)
		alcCloseDevice(g_pDevice);
	::MessageBox(NULL,"Error opening OpenAL device!!","Error",MB_OK);
	return false;
}

void PSL_Free()
{
	for( int i=0; i<g_PS_Buffers.size(); i++ )
	{
		SAFE_DELETE(g_PS_Buffers[i]);
	}

	for( int i=0; i<g_PS_Sources.size(); i++ )
	{
		SAFE_DELETE(g_PS_Sources[i]);
	}


	alcMakeContextCurrent(NULL);
	alcDestroyContext(g_pContext);
	alcCloseDevice(g_pDevice);
}

/*
幾個要實做的函式
1. 找可用(未使用中)的 source
2. 找某個檔名所在的 buffer
3. 管理 buffer 記憶體的函式
4. 檢查 source 狀態, 以及進行必須的處理
*/
ISoundSource *PSL_GetAvailableSource()
{
	// 1 是BGM用
	for(int i=1;i<g_PS_Sources.size();i++)
	{
		if( ( g_PS_Sources[i]->IsAvailable() ) && ( g_PS_Sources[i]->GetUsable() ) )
			return g_PS_Sources[i];
	}

	PSSource* psource = new PSSource;
	psource->Init();
	psource->SetVolume(1.0f);
	g_PS_Sources.push_back(psource);

	return (ISoundSource*)psource;
}



ISoundBuffer *PSL_GetBufferWithName(const char *filename)
{
	for( int i=0;i<g_PS_Buffers.size();i++)
	{
		if(g_PS_Buffers[i]->IsFileLoaded(filename))
			return g_PS_Buffers[i];
	}
	// 沒找到就 load buffer
	PSBuffer* psound = new PSBuffer;
	psound->LoadFile(filename);
// 	g_PS_Buffers.push_back(psound);
// 	if(n>=0 && n< MAX_SOUND_BUFFERS)
// 	{
// 		if(g_PS_Buffers[n].LoadFile(filename))
// 			return g_PS_Buffers+n;
// 	}
	return psound;	// 一點辦法也沒有
}

void PSL_ClearBuffer()
{
	int i;
	for(i=0;i<g_PS_Buffers.size();i++)
	{
		if(( !g_PS_Buffers[i]->Empty()) && (g_PS_Buffers[i]->UseCount()<=0) )
		{
			g_PS_Buffers[i]->Clear();
		}
	}
}

void PSL_StopSource( ISoundSource* ssound )
{
	ssound->Stop();
}


void PSL_StopAllSource()
{
	for(int i=0;i<g_PS_Sources.size();i++)
		g_PS_Sources[i]->Stop();
}
