#ifndef _SOUNDCODEC_H_
#define _SOUNDCODEC_H_

#include <basedef.h>

/**
 *  PicnoSound 底層播放部份
 *  這邊和使用的底層子系統無關, 純粹讓高階的部份直接呼叫用
 *  另外定義兩個基礎類別供高階(音效資料庫系統)部份直接利用
 */

class ISoundBuffer
{
public:

	virtual bool LoadFile(const char *filename) = 0;
	virtual void Clear() = 0;	// 內容清空
};

class ISoundSource
{
public:
	/// 注意, AttachBuffer 每個實做都只要考慮自己同步實做的 SoundBuffer 延伸類別就好了
	/// 反而 PicnoSound 介面不能混用兩個不同實做

	virtual void AttachBuffer(ISoundBuffer *buf) = 0;
	virtual bool IsPlaying() = 0;
	virtual void Play(bool looped) = 0;
	virtual void Stop() = 0;
	virtual void SetVolume(float val) = 0;

	virtual void SetUsable( bool b ) { m_bUsable=false; }
	virtual bool GetUsable()         { return m_bUsable; }

private:
	bool m_bUsable;
};

// Global functions
extern bool          PSL_Init();
extern void          PSL_Free();

extern ISoundSource *PSL_GetAvailableSource();	// 其他是所有音效共用

extern ISoundBuffer *PSL_GetBufferWithName(const char *filename);	// 找出一個已經載入 filename 的 buffer(如果沒有就自動載入)

extern void          PSL_StopSource( ISoundSource* ssound );
extern void          PSL_StopAllSource();	// 停止所有音效播放

extern void          PSL_ClearBuffer();



#endif	// _PICNOSOUND_LOW_H_
