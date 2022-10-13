// Body.cpp game element data store class
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
//                                               Copyright    (C)    1999    Pin
//
#include <pf-debug.h>
//#include <pf-string.h>
#include <pf-stdlib.h>
#include <math.h>
#include <gadget\file_operator.h>

#include "sprite2d.h"

namespace GSO
{
    //#include <crtdbg.h>
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    //  Sprite2d class
    _tstring     Sprite2d::m_CurrentDirectory;
    
    
    const tchar_t* _makefullCurrentDirectoryfilename( tchar_t* outbuf, const tchar_t* inbuf )
    {
        //沒有路徑不完整
        if ( strstr( outbuf, inbuf ) == 0 )
        {
            _makepath( outbuf, NULL, Sprite2d::GetCurrentDirectory(), inbuf, NULL );
        }
        
        //else
        //    strcpy(outbuf, inbuf);
        return mcurrentpath( outbuf, inbuf );
    }
    
    //=============================================================================
    // NAME:
    // DESC: constructor
    //=============================================================================
    Sprite2d::Sprite2d()
    {
        Initiate();
    }
    
    //=============================================================================
    // NAME:
    // DESC: distructor
    //=============================================================================
    Sprite2d::~Sprite2d()
    {
        Clear();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::Initiate( void )
    {
        m_InitDistance = 0;
        m_InitFlipRate = 0;
        m_Collision = 0;              //碰撞產生Flag
        m_Distance = 20;
        m_Step = 0;
        m_Descriptor = NULL;
        m_tmp_distance = 0;           //存放計算移動距離
        Id = GSO_STATIC_ID;
        GId = GSO_STATIC_ID;
        m_bEnable = true;
        m_bVisible = true;
        m_bMoveable = true;
        m_bDragable = false;
        m_bSelected = false;
        m_Position( 0, 0, 0 );       //地圖位置
        m_StepRate.SetCycle( 50 );
        m_Route.clear();
        m_Expression.clear();
        m_ResoureName.clear();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::Clear( void )
    {
        m_Route.clear();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Sprite2d::GetEnable( void ) const
    {
        return m_bEnable;
    }
    
    bool Sprite2d::GetVisible( void ) const
    {
        return m_bVisible;
    }
    
    bool Sprite2d::GetMoveable( void ) const
    {
        return m_bMoveable;
    }
    
    bool Sprite2d::GetDragable( void ) const
    {
        return m_bDragable;
    }
    
    bool Sprite2d::GetSelected( void ) const
    {
        return m_bSelected;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::SetEnable( bool f )
    {
        m_bEnable = f;
    }
    
    void Sprite2d::SetMoveable( bool f )
    {
        m_bMoveable = f;
    }
    
    void Sprite2d::SetVisible( bool f )
    {
        if ( GetVisible() != f )
        {
            m_bVisible = f;
            OnShow( f );
        }
        
        return;
    }
    
    void Sprite2d::SetDragable( bool f )
    {
        m_bDragable = f;
    }
    
    void Sprite2d::SetSelected( bool f )
    {
        m_bSelected = f;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    const tchar_t* Sprite2d::GetCurrentDirectory( void )
    {
        //return gResource2dMgr.GetCurrentDirectory();
        return m_CurrentDirectory.c_str();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::SetCurrentDirectory( const tchar_t* wdir )
    {
        tchar_t currfname[_MAX_LFNAME];
        m_CurrentDirectory = mcurrentpath( currfname, wdir );
        //gResource2dMgr.SetCurrentDirectory(wdir);
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GameVertex&    Sprite2d::GetPos( int index )
    {
        return m_Position;
    }
    
    void Sprite2d::SetPos( GameVertex pos )
    {
        m_Position = pos;
        OnMoving( m_Position );
    }
    
    GameVertex Sprite2d::GetScreenPos( void )
    {
        return m_Position + OnGetOrigin();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GameVertex Sprite2d::GetGoal( void )
    {
        if ( m_Route.empty() )
        { return GetPos(); }
        
        return m_Route.back();
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    GameVertex Sprite2d::GetNextRoute( void )
    {
        // Sean: 我偷改了加一的動作
        if ( m_Step < m_Route.size() )
        { return m_Route[m_Step]; }
        
        return m_Position;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::ClearRoute( void )
    {
        //if(!m_Route.empty())
        //m_Position = m_Route.back();
        m_StepRate.Reset();
        m_Route.clear();
        //m_StepRate.Reset();
        m_Step = 0;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Sprite2d::PushRoute( const GameVertex& naviPos )
    {
        m_Route.push_back( naviPos );
    }
    
    //=============================================================================
    // NAME: Go_Path()
    //
    //=============================================================================
    int Sprite2d::UpdateRoute( void )
    {
        float muntime = m_StepRate.Interval();
        
        if ( ( !GetMoveable() ) || ( m_Route.empty() ) )
        {
            return -1;
        }
        
        if ( muntime >= TIMER_INTERRUPT )
        {
            if ( m_StepRate.GetCycle() == 0 )
            {
                return 0;
            }
            
            // Sean: 這裡先借我改一下....
            size_t step = ( m_Step + muntime );
            m_Step = ( step >= m_Route.size() ) ? m_Route.size() : step;
            
            if ( ( m_Step >= m_Route.size() ) || ( m_Route[m_Step] == GetGoal() ) )
            {
                SetPos( m_Route.back() );
                OnArrival( m_Route.back() );
                ClearRoute();
                return 1;
            }
            else
            {
                SetPos( m_Route[m_Step] );
                return 0;
            }
            
            //muntime=abs(muntime);
            /*
                        if(m_Step >= m_Route.size())
                        {
                            //GameVertex destpos = m_Route.back();
                            SetPos(m_Route.back());
                            OnArrival(m_Route.back());
                            ClearRoute();
                            return 1;
                        }
                        else
                        {
                            SetPos(m_Route[m_Step]);
                            //int cycle = m_StepRate.GetCycle();
                            //printf("XXXXXX%d\n", (abs(muntime)/cycle));
                            //m_Step+=(abs(muntime)/cycle);
                            m_Step+=1;
                            return 0;
                        }
            */
            /*
            m_Position = m_Route[m_Step];
            //if(m_Route.back() != Pos())
            if(m_Route.back() != m_Position)
            {
                m_Step++;
                return 0;
            }
            else//走到終點
            {
                ClearRoute();
                return 1;
            }
            */
        }
        
        return -1;
    }
    
    GameVertex Sprite2d::GetPredictPosition( void )
    {
        if ( m_Step + 8 < m_Route.size() )
        {
            return m_Route[m_Step + 8];
        }
        
        return m_Route.back();
    }
    
    //=============================================================================
    // NAME: search_path()
    //
    //=============================================================================
    uint_t& Sprite2d::GetMoveSpeed( void )
    {
        return *( &m_Distance );
    }
    
    //=============================================================================
    // NAME:
    //
    //=============================================================================
    void Sprite2d::SetMoveSpeed( uint_t dist )
    {
        if ( dist == 0 )
        { return; }
        
        m_Distance = dist;
        m_StepRate.SetCycle( 1000 / ( m_Distance * 10 ) );
    }
    
    //=============================================================================
    // NAME: search_path()
    //
    //=============================================================================
    int Sprite2d::FindRoute( GameVertex to )
    {
        if ( ( !GetMoveable() ) || ( m_Distance == 0 ) )
        {
            ClearRoute();
            return -1;
        }
        
        /*
        if(m_Route.empty())
        {
            m_Route.push_back(m_Position);
        }
        */
        if ( to == m_Position )
        { return 0; }
        
        //m_Route.clear();
        //確定向量的分量
        GameVertex from = m_Route.empty() ? m_Position : m_Route.back();
        int Dx          = to.x - from.x;
        int Dy          = to.y - from.y;
        //計算增量數
        int steps       = ( int )( ( ( double ) max( abs( Dx ), abs( Dy ) ) + 0.5f ) + 1 );
        double dx_step  = ( ( double ) Dx / ( double ) steps );
        double dy_step  = ( ( double ) Dy / ( double ) steps );
        // Sean: 先借我偷改一下
        m_StepRate.SetCycle( 1000 / ( m_Distance * 10 ) );
        float x         = from.x + ( ( float ) dx_step ); // first must can't now situs;
        float y         = from.y + ( ( float ) dy_step );
        // Sean: 再借我偷改一下，這裡先mark
        // steps = (steps / m_Distance);// protect for walk over
        int lastposx = 0;
        int lastposy = 0;
        
        for ( int u = 0; u < steps; u++ )
        {
            int currposx = ( int ) ( x  + 0.5f );
            int currposy = ( int ) ( y  + 0.5f );
            
            if ( ( currposx != lastposx ) || ( currposy != lastposy ) )
            {
                PushRoute( GameVertex( currposx, currposy, 0 ) );
                //m_Route.push_back(GameVertex(currposx,
                //                             currposy));
            }
            
            lastposx = currposx;
            lastposy = currposy;
            x = x + ( ( float ) ( dx_step ) );
            y = y + ( ( float ) ( dy_step ) );
        }
        
        if ( ( to.x != lastposx ) && ( to.y != lastposy ) )
        {
            PushRoute( to );
            //m_Route.push_back(to);
        }
        
        m_StepRate.Reset();
        return m_Route.size();
    }
    
    /*
        int Sprite2d::FindRoute(GameVertex to)
        {
            if((!GetMoveable()) || (m_Distance == 0))
            {
                ClearRoute();
                return -1;
            }
    
            if(m_Route.empty())
            {
                m_Route.push_back(m_Position);
            }
    
            //m_Route.clear();
            //確定向量的分量
            GameVertex from = m_Route.back();
    
            int Dx          = to.x - from.x;
            int Dy          = to.y - from.y;
    
            //計算增量數
            int steps       = (int)
                              (floor((double) max(abs(Dx), abs(Dy))) +
                                1) ;
    
            double dx_step  = ((double) Dx / (double) steps)  ;
            double dy_step  = ((double) Dy / (double) steps)  ;
    
            float x         = from.x + ((float) dx_step* m_Distance);// first must can't now situs;
            float y         = from.y + ((float) dy_step* m_Distance);
    
            steps = (steps / m_Distance);// protect for walk over
            for(int u = 0;u < steps;u++)
            {
                m_Route.push_back(GameVertex((int) floor(x),
                                               (int) floor(y)));
                x = x + ((float) (dx_step) * (float) (m_Distance));
                y = y + ((float) (dy_step) * (float) (m_Distance));
    
                //x = x + ((float) (dx_step) * (float) (1));
                //y = y + ((float) (dy_step) * (float) (1));
    
            }
            m_Route.push_back(to);
            //m_StepRate.Reset();
    
            //m_Route.push_back(to);
            //m_Step=0;
            //m_Route.GoBegin();
            return steps+1;
        }
    
    */
    //=============================================================================
    // NAME: search_path()
    //
    //=============================================================================
    /*
    int Sprite2d::FindRoute(GameVertex from, GameVertex to)
    {
        if((!GetMoveable()) || (m_Distance == 0))
        {
            ClearRoute();
            return -1;
        }
    
    
        //m_Route.clear();
        //確定向量的分量
        int Dx          = to.x - from.x;
        int Dy          = to.y - from.y;
    
        //計算增量數
        int steps       = (int)
                          (floor((double) max(abs(Dx), abs(Dy))) +
                            1) ;
    
        double dx_step  = ((double) Dx / (double) steps)  ;
        double dy_step  = ((double) Dy / (double) steps)  ;
    
        float x         = from.x + ((float) dx_step* m_Distance);// first must can't now situs;
        float y         = from.y + ((float) dy_step* m_Distance);
    
        steps = (steps / m_Distance);// protect for walk over
        for(int u = 0;u < steps;u++)
        {
            m_Route.push_back(GameVertex((int) floor(x),
                                           (int) floor(y)));
            x = x + ((float) (dx_step) * (float) (m_Distance));
            y = y + ((float) (dy_step) * (float) (m_Distance));
        }
    
        //m_Route.push_back(to);
        m_Step=0;
        //m_Route.GoBegin();
        return steps;
    }
    */
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    /*
    int Sprite2d::FindAndUpdateRoute()
    {
        if((!GetMoveable()) || (m_Distance == 0) || m_Route.empty())
        {
            return -1;
        }
    
        if(m_StepRate.Interval() < 0)
        {
            if(m_Route.back() == m_Position)
            {
                m_Step=0;
                m_Route.clear();
                return 1;
            }
    
            if(m_Position != m_Route[m_Step])
            {
                GameVertex delta = m_Route[m_Step] - m_Position;
    
                float dx = (float)delta.x;
                float dy = (float)delta.y;
    
                float length = sqrt(dx*dx + dy*dy);
                if(length == 0 || length <= m_Distance)
                {
                    m_Position = m_Route[m_Step];
                    return 0;
                }
    
                m_Position.x = m_Position.x + m_Distance*(dx/length);
                m_Position.y = m_Position.y + m_Distance*(dy/length);
    
            }
            else
            {
                ++m_Step;
                return FindAndUpdateRoute();
            }
            return 0;
        }
        return-1;
    }
    */
    
    /*
    //=============================================================================
    // NAME: Action_ID()
    // DESC:
    //=============================================================================
    int Sprite2d::Action_ID(int act, int dir) //計算動作的ID
    {
        return ((act * m_MaxAspect) + dir);
    }
    */
    
    
    /*
    //=============================================================================
    // NAME: Set_Step()
    // DESC:
    //=============================================================================
    void Sprite2d::Set_Step(int step)
    {
        m_Distance = step;
        return;
    }
    */
    
    //=============================================================================
    
    
    
    
    
};//namespace GE












