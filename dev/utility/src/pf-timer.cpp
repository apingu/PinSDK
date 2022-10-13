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
//#include <set>
#include <vector>
//#include <sys/timeb.h>
//#include <time.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "pf-time.h"
#include "pf-timer.h"


namespace pftml
{
    namespace
    {
        //std::set<Timer*>            g_cp_timer_cntr;
        std::vector<pftml::Timer*>* g_cp_timer_cntr = NULL;
        mstimen_t                   g_cp_tImer_stop_time = 0;
    }

#define  _AutoTimerGetTimerFunc(t)  (t == 0)? pftml::Time::GetTickMSTime():t;

    //=============================================================================
    // NAME:
    // DESC: constructor
    //=============================================================================
    Timer::Timer( _IN mstimen_t time )
    {
        mstimen_t curr_time = pftml::Time::GetTickMSTime();
        minus_time_ = 0;
        frames_ = 0;
        count_ = 0;
        cycle_time_ = time;
        pause_time_ = 0;
        minus_per_second_ = 0;
        birth_time_ = curr_time;
        is_pursue_cycle = true;
        /////Now_Time = curr_time;
        last_time_ = curr_time;
        pause_time_ = 0;

        if( g_cp_timer_cntr == NULL )
        {
            g_cp_timer_cntr = new std::vector<Timer*>;
        }

        //g_cp_timer_cntr.insert(std::set<Timer*>::value_type(this));
        g_cp_timer_cntr->push_back( this );
    }

    //=============================================================================
    // NAME:
    // DESC: distructor
    //=============================================================================
    Timer::~Timer()
    {
        if( g_cp_timer_cntr != NULL )
        {
            for( unsigned i = 0; i < g_cp_timer_cntr->size(); i++ )
            {
                if( ( *g_cp_timer_cntr )[ i ] == this )
                {
                    g_cp_timer_cntr->erase( g_cp_timer_cntr->begin() + i );
                    break;
                }
            }
        }

        //std::set<Timer*>::iterator iter=g_cp_timer_cntr.find(this);
        //if(iter!=g_cp_timer_cntr.end())
        // g_cp_timer_cntr.erase(this);

        if( g_cp_timer_cntr->empty() )
        {
            SAFE_DELETE( g_cp_timer_cntr );
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Timer& Timer::operator=( _IN const Timer& ptimer )
    {
        count_ = ptimer.count_;
        frames_ = ptimer.frames_;
        birth_time_ = ptimer.birth_time_;
        //////Now_Time = ptimer.Now_Time;
        last_time_ = ptimer.last_time_;
        minus_time_ = ptimer.minus_time_;
        cycle_time_ = ptimer.cycle_time_;
        minus_per_second_ = ptimer.minus_per_second_;
        pause_time_ = ptimer.pause_time_;
        return *this;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::Play( _IN mstimen_t time )
    {
        if( pause_time_ == 0 )
        {
            last_time_ = time;
        }
        else
        {
            mstimen_t mt = time - pause_time_;
            last_time_ += mt;
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::Pause( _IN mstimen_t time )
    {
        pause_time_ = time;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::PlayAll( _IN mstimen_t time )
    {
        if( g_cp_timer_cntr != NULL )
        {
            for( unsigned i = 0; i < g_cp_timer_cntr->size(); i++ )
            {
                if( ( *g_cp_timer_cntr )[ i ] )
                {
                    ( *g_cp_timer_cntr )[ i ]->Play( time );
                }
            }
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::PauseAll( _IN mstimen_t time )
    {
        if( g_cp_timer_cntr != NULL )
        {
            for( unsigned i = 0; i < g_cp_timer_cntr->size(); i++ )
            {
                if( ( *g_cp_timer_cntr )[ i ] )
                {
                    ( *g_cp_timer_cntr )[ i ]->Pause( time );
                }
            }
        }
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::SetBirthTime( _IN mstimen_t time )
    {
        birth_time_ = _AutoTimerGetTimerFunc( time );
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t Timer::GetBirthTime( void )
    {
        return birth_time_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Timer::SetCycle( _IN mstimen_t time )
    {
        cycle_time_ = time;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    mstimen_t Timer::GetCycle( void )  const
    {
        return cycle_time_;
    }

    //=============================================================================
    // NAME:
    // DESC: restart to reckon by time
    //=============================================================================
    void Timer::Reset( _IN mstimen_t time )
    {
        last_time_ = _AutoTimerGetTimerFunc( time );
        return;
    }

    //=============================================================================
    // NAME:
    // DESC: restart to reckon by time
    //=============================================================================
    mstimen_t Timer::MinuTime( _IN mstimen_t time )
    {
        mstimen_t curr_time = _AutoTimerGetTimerFunc( time );

        if( curr_time < last_time_ )
        {
            last_time_ = curr_time;
            return minus_time_ = 0;
        }

        return minus_time_ = ( curr_time - last_time_ );
    }

    //=============================================================================
    // NAME:
    // DESC: frame per second
    //=============================================================================
    float Timer::Fps( _IN mstimen_t time )
    {
        mstimen_t curr_time = _AutoTimerGetTimerFunc( time );
        mstimen_t Minus = curr_time - last_time_;
        frames_++;

        if( Minus > 1000 )
        {
            last_time_ = curr_time;
            minus_per_second_ = ( float )( ( 1000.0 * frames_ ) / Minus );
            frames_ = 0;
        }

        return minus_per_second_;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    float Timer::Interval( _IN mstimen_t time )
    {
        mstimen_t curr_time = _AutoTimerGetTimerFunc( time );

        //Now_Time = GetTickMSTime();

        //minus_time_ = Now_Time - last_time_;

        if( curr_time < last_time_ )
        {
            last_time_ = curr_time;
            minus_time_ = 0;
        }
        else
        {
            // 離上一次時間 相差多少
            minus_time_ = ( curr_time - last_time_ );
        }

        float minus_progress = ( ( float )( minus_time_ ) / ( ( float )cycle_time_ ) );
        count_++;

        // 超過週期了
        // 開始下一個週期
        if( ( TIMER_STOP != cycle_time_ ) &&
            ( minus_time_ >= cycle_time_ ) )
        {
            if( !is_pursue_cycle )
            {
                last_time_ = curr_time;
            }
            else // 下一個週期 要把這次超過的時間 回補
            {
                mstimen_t mt = 0;
                if( minus_time_ >= cycle_time_ )
                {
                    // 離一個週期德時間 還剩下多久
                    mt = minus_time_ - cycle_time_;
                }
                last_time_ = curr_time - mt;
            }

            count_ = 0;
            minus_time_ = 0;
            //return minu;
            //return TIMER_INTERRUPT;
        }

        return minus_progress;
    }

    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    bool Timer::IsInterval( void )
    {
        if( minus_time_ == 0 )
        {
            return true;
        }

        return false;
    }

    //=============================================================================
    // NAME: IsDuringTime()
    // DESC:
    //=============================================================================
    //bool Timer::IsTimeOut( void )
    //{
    //    // 永遠不會超時
    //    if( TIMER_STOP == cycle_time_ )
    //    {
    //        return false;
    //    }
    //
    //    return ( minus_time_ >= cycle_time_ ) ? true : false;
    //}

    //=============================================================================
    // NAME:
    // DESC: 計算週期計算次數
    //=============================================================================
    unsigned int Timer::ComputeCount( void )
    {
        return count_;
    }

    //=============================================================================
    // NAME: SetBreakCycle()
    // DESC:
    //=============================================================================
    void Timer::SetPrusueCycle( bool b )
    {
        is_pursue_cycle = b;
    }
}