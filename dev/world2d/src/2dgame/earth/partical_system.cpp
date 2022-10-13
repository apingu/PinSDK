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
#include <assert.h>
#include <math.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//
#include <graphic\vmc-draw.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "partical_system.h"


ParticalSnowEmitter::initial_t ParticalSnowEmitter::g_Default =
{
    50,            // tickcnt
    256,        // particles
    600,        // bottom
    -50,-700,    // x0,y0
    900,600,    // xr,yr
    500,        // time_min
    2000,        // time_range
    5,-3,        // wr,w0
    3,            // yspeed
    3000,        // layereff
};

ParticalSnowEmitter::ParticalSnowEmitter( initial_t *tmpl )
    : m_WindEffect( 0 )
{
    if( !tmpl )
        tmpl = &g_Default;
    memcpy( &option, tmpl, sizeof( initial_t ) );
}

ParticalSnowEmitter::~ParticalSnowEmitter()
{
}

void ParticalSnowEmitter::BlitImage( CPIcon* canvas )
{
    if( !m_IsActivate )
        return;
    for( int i = 0; i < option.particles; ++i )
    {
        if( m_ParticalSet[ i ].IsAlive() )
        {
            LGB::VMCDraw::AlphaBlit( gfx.GetImage( m_ParticalSet[ i ].GetLayer() - 1 ), m_ParticalSet[ i ].GetPos(), canvas );
        }
    }
}


void ParticalSnowEmitter::Process()
{
    static dword_t enter_count = GetTickCount();
    int layers = gfx.GetImageCount();
    int ymov;
    float ff, rate;

    if( ( GetTickCount() - enter_count ) < option.tickcnt )
        return;
    enter_count = GetTickCount();

    assert( option.particles <= PARTICAL_COUNT );
    rate = ( float )option.layereff / 10000.0;

    for( int i = 0; i < option.particles; ++i )
    {
        if( m_ParticalSet[ i ].IsAlive() )
        {
            if( m_ParticalSet[ i ].GetPos().y > option.bottom )
            {
                m_ParticalSet[ i ].SetAlive( false );
            }
            else
            {
                if( rand() % 50000 < m_ParticalSet[ i ].GetPos().y )
                    m_ParticalSet[ i ].SetAlive( false );
                m_ParticalSet[ i ].GetPos().x += m_WindEffect / m_ParticalSet[ i ].GetLayer() + rand() % 3 - 1;
                ff = 1.0 - ( float )m_ParticalSet[ i ].GetLayer() / ( float )layers;
                ff *= rate;
                ymov = ( int )( floor( ( ff + 1.0 )*option.yspeed ) );
                m_ParticalSet[ i ].GetPos().y += ymov;
            }
        }
        else
        {
            if( !m_IsActivate )
                continue;
            m_ParticalSet[ i ].Emit( GameVertex( rand() % option.xr + option.x0,
                rand() % option.yr + option.y0, 0 ) );
            m_ParticalSet[ i ].SetLayer( rand() % layers + 1 );
        }
    }
}

void ParticalSnowEmitter::OnTimer()
{
    if( ( GetTickCount() - m_LastTime ) < m_TimeInterval )
        return;
    m_LastTime = GetTickCount();
    m_TimeInterval = ( rand() % option.time_range ) + option.time_min;
    m_WindEffect = rand() % option.wr + option.w0;
}



//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------
ParticalRainEmitter::initial_t ParticalRainEmitter::g_Default =
{
        50,        // tickcnt
        256,    // particles
        600,    // bottom
        3,        // xdiff
        30,10,    // ydiff,yvar
        -50,-700,    // x0,y0
        900,600        // xr,yr
};

ParticalRainEmitter::ParticalRainEmitter( const initial_t *p )
{
    if( !p )    p = &g_Default;
    memcpy( &option, p, sizeof( initial_t ) );
}

ParticalRainEmitter::~ParticalRainEmitter()
{
}


void ParticalRainEmitter::BlitImage( CPIcon* canvas )
{
    if( !m_IsActivate )
        return;
    LG::Point pt1( 3, -30 );
    LG::Point pt2( -3, 30 );

    assert( option.particles <= PARTICAL_COUNT );

    for( int i = 0; i < option.particles; ++i )
    {
        if( m_ParticalSet[ i ].IsAlive() )
        {
            if( m_ParticalSet[ i ].GetPos().y + pt2.y >= 0 )
                LGB::VMCDraw::Line( m_ParticalSet[ i ].GetPos() + pt1, m_ParticalSet[ i ].GetPos() + pt2, 0xFFFFFFFF, canvas );
        }
    }
}

void ParticalRainEmitter::Process()
{
    static dword_t enter_count = GetTickCount();
    if( ( GetTickCount() - enter_count ) < option.tickcnt )
        return;
    enter_count = GetTickCount();
    for( int i = 0; i < PARTICAL_COUNT; ++i )
    {
        if( m_ParticalSet[ i ].IsAlive() )
        {
            if( m_ParticalSet[ i ].GetPos().y > option.bottom )
            {
                m_ParticalSet[ i ].SetAlive( false );
            }
            else
            {
                if( rand() % 10000 < m_ParticalSet[ i ].GetPos().y )
                    m_ParticalSet[ i ].SetAlive( false );
                m_ParticalSet[ i ].GetPos().x += option.xdiff + rand() % 3 - 1;
                m_ParticalSet[ i ].GetPos().y += option.ydiff + rand() % option.yvar;
            }
        }
        else
        {
            if( !m_IsActivate )
                continue;
            m_ParticalSet[ i ].Emit( GameVertex( rand() % option.xr + option.x0,
                rand() % option.yr + option.y0, 0 ) );
            m_ParticalSet[ i ].SetLayer( 1 );
        }
    }
}

void ParticalRainEmitter::OnTimer()
{
}

