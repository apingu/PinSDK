//=============================================================================
//
// Timer
//
//
//
//      Time compute Tool
//
//
//
//
//
//
//               Apingu
//=============================================================================
#pragma once

#include <pf-def.h>

#define TIMER_INTERRUPT 1.0
#define TIMER_STOP   0XFFFFFFFF

namespace pftml  // time library
{
    class Timer
    {
    private:

        mstimen_t          cycle_time_;       // clock interval time
        mstimen_t          minus_time_;
        mstimen_t          birth_time_;       // build class system time
        mstimen_t          last_time_;
        unsigned int       frames_;           // counter for fps
        unsigned int       count_;            // counter for average
        float              minus_per_second_; // FPS
        mstimen_t          pause_time_;
        bool               is_pursue_cycle;   // 下一個週期 要不要 追回 上一個週期超過的時間

    public:

        Timer( mstimen_t interval = 0 );
        ~Timer();


        void               Play( _IN mstimen_t time );
        void               Pause( _IN mstimen_t time = 0 );

        // 重新啟動所有計時器
        static   void      PlayAll( _IN mstimen_t time );
        // 暫停所有計時器
        static   void      PauseAll( _IN mstimen_t time );

        Timer&             operator=( _IN const Timer& ptimer );

        void               SetBirthTime( _IN mstimen_t time );
        mstimen_t          GetBirthTime( void );
        void               SetCycle( _IN mstimen_t time );
        mstimen_t          GetCycle( void ) const;


        bool               IsInterval( void );
        // 還在 計時的時間區段內
        //bool               IsTimeOut( void );
        // 離上一次 到期後 詢問了幾次
        unsigned int       ComputeCount( void );

        // 重設目前時間  傳入 現在的時間 或 0 系統抓取現在的時間
        void               Reset( _IN mstimen_t time = 0 );

        // 從上次到現在的時間差
        mstimen_t          MinuTime( _IN mstimen_t time = 0 );


        // 有沒有超過時間
        // 傳入 要計算到期的時間 0 = now time 當下的時間
        // e.g. if ( Interval() >= TIMER_INTERRUPT ) => 剛好或超過
        //         ( Interval() < TIMER_INTERRUPT )  => 未超過
        // 0.1 ~ 0.3 ~ ... 到 1等於時間到 超過 後 就 1.1 ~ 1.2 可以想成 時間條的百分比
        float              Interval( _IN mstimen_t time = 0 );
        float              Fps( _IN mstimen_t time = 0 );

        // 時間會不會往前追朔?
        // 抓到時間後 若已經超過 下一次 會把這次超過的時間 補扣回來
        void               SetPrusueCycle( _IN bool b );
    };
}

/*
    pftml::Timer timer;
    timer.SetCycle();

    ...on update

    float muntime = m_StepRate.Interval();
    if ( muntime >= TIMER_INTERRUPT )
        .... time on
*/