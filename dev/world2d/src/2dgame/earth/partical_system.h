/*
 *    File:        particalsystem.h
 *    Desc:        hko partical system
 *    Creater:    Sean
 *    Create:        04/07/31
 */

#ifndef PARTICAL_SYSTEM_H
#define PARTICAL_SYSTEM_H
#include <element2d.h>
#include <time.h>

#define PARTICAL_COUNT    256
#define LAYER_BACK        1
#define LAYER_MID        2
#define LAYER_FRONT        3
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
class Partical
{
public:
    Partical() : m_Pos(-50, -50, 0), m_IsAlive(false), m_Layer(LAYER_BACK)    {}
    ~Partical() {}

    inline void            SetAlive(bool alive)    {    m_IsAlive = alive;    }
    inline bool            IsAlive()                {    return m_IsAlive;    }
    inline void            Emit(GameVertex& pos)    {    m_IsAlive = true;    m_Pos = pos;    }
    inline GameVertex&    GetPos()                {    return m_Pos;    }
    inline void            SetPos(GameVertex& pos)    {    m_Pos = pos;    }
    inline int            GetLayer()                {    return m_Layer;    }
    inline void            SetLayer(int layer)        {    m_Layer = layer;}

private:
    GameVertex m_Pos;
    bool       m_IsAlive;
    int        m_Layer;
};

//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
//**************************經常當在這邊 請檢查
class IParticalEmitter
{
public:
    IParticalEmitter() : m_IsActivate(false), m_TimeInterval(10), m_LastTime(0)    {srand((int)time(0));}
    virtual ~IParticalEmitter() {}
    
    inline int            Load(const char* filename)    {    return gfx.LoadRes(filename);}
    inline void            Enable(bool enable = true)    {    m_IsActivate = enable;         }
    inline void            Excute()
    {
        if (!m_IsActivate)
            return;
        Process();
        OnTimer();
    }
    
    virtual void    BlitImage(CPIcon* canvas) = 0;

protected:
    virtual void    Process() = 0;
    virtual void    OnTimer() = 0;
    dword_t           m_TimeInterval;
    dword_t           m_LastTime;
    bool            m_IsActivate;
    Partical        m_ParticalSet[PARTICAL_COUNT];
    GSO::Element2d  gfx;
};

//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
class ParticalSnowEmitter : public IParticalEmitter
{
public:
    struct initial_t
    {
        short tickcnt;    // GetTickCount() - enter_count
        short particles;    // 使用到的粒子數
        short bottom;        // 畫面底部, 超過則消失
        short x0,y0;        // Emit 用
        short xr,yr;        // Range(Emit用)
        short time_min;    // OnTimer 用計時器
        short time_range;
        short wr,w0;        // 風力影響數值
        short yspeed;        // Y 方向最低掉落速度
        short layereff;        // Layer 對於速度的影響(*1/10000)
    };
public:

    ParticalSnowEmitter(initial_t *tmpl);
    virtual ~ParticalSnowEmitter();
    virtual void BlitImage(CPIcon* canvas);
    
protected:
    virtual void Process();

    virtual void OnTimer();

    static initial_t g_Default;
    initial_t option;
    int m_WindEffect;
};

//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
class ParticalRainEmitter : public IParticalEmitter
{
public:
    struct initial_t
    {
        short tickcnt;    // GetTickCount() - enter_count
        short particles;    // 使用到的粒子數
        short bottom;        // 畫面底部, 超過則消失
        short xdiff;        // x位移
        short ydiff,yvar;    // y位移及差異
        short x0,y0;        // Emit 用
        short xr,yr;        // Range(Emit用)
    };
public:
    ParticalRainEmitter(const initial_t *);
    virtual ~ParticalRainEmitter();
    virtual void BlitImage(CPIcon* canvas);
    
protected:    
    virtual void Process();
    virtual void OnTimer();

    static initial_t g_Default;
    initial_t option;
};

#endif