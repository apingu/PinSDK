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
//
//
//
#ifndef ANIM2D_H
#define ANIM2D_H

#pragma warning (disable : 4786)


#include <string>
#include <map>
//#include <pf-def.h>
#include <pf-timer.h>

#define PLAY_INTERRUPT           1
#define PLAY_ON_INTERVAL_TIMES   0
#define PLAY_STOP               -1  // no anim or over play count
#define PLAY_ONE_TIMES          -2
#define PLAY_N_TIMES            -3

typedef  int              direction_t;

namespace GSO
{
    class sequence_t;
    class Anim2d
    {
    public:
    
        // 播放圖型的順序
        enum    IMAGE_PLAY_ORDER { PLAYORDER_FORWARD      = 0,    // 從第一張往後播放
                                   PLAYORDER_BACK         = 1
                                 };  // 從最後一張往前播放
                                 
                                 
        // 播放的方式
        enum    IMAGE_PLAY_MODE  { PLAYMODE_PLAYFOREVER   = 0,    // 連續播放
                                   PLAYMODE_STOP          = 1,    // 停止
                                   //PLAYMODE_STOP_ON_FIRST = 2,
                                   //PLAYMODE_STOP_ON_LAST  = 3,
                                   PLAYMODE_INTERRUPT     = 4
                                 };   // 撥放至第N張後停止
                                 
    private:
    
        pftml::Timer        m_FlipRate;        //animation refresh rate
        short               m_Counter;
        short               m_NowState;        //目前動作撥放的狀態(停止, 1次, n次, 延遲區間內, 圖形編號)
        direction_t         m_NowAngle;        //真正指向的動作圖形
        
        IMAGE_PLAY_MODE     m_PlayMode;
        short               m_PlayInterruptFrame;// -1 == last one, 0 = first one
        IMAGE_PLAY_ORDER    m_PlayOrder;
        
        
    public:
    
        std::map<direction_t, sequence_t*>   m_Sequences;
        _tstring                           m_Filename;
        
    public:
    
        Anim2d();
        ~Anim2d();
        
        //2 is read same file
        error_t             Load( const tchar_t* filename );
        // ftype 1 as xml  0 as old binary
        error_t             Save( const tchar_t* filename );
        const tchar_t*      GetFilename( void ) const;
        
        int                 Empty( void );
        void                Clear( void );
        
        bool                IsNowAnimInFrame( short order );
        
        void                operator++( int );
        void                operator--( int );
        
        // angle 0< as first angle
        bool                ChangeDirection( direction_t angle );
        bool                ChangeFrame( int fid );
        
        // a==1 to all seq else only current seq
        void                SetFlipRate( dword_t rate, int a = 1 );
        //重置filp rate的時間
        void                ResetFilpRate();
        
        mstimen_t            GetFlipRate( void ) const;
        bool                GetFilpInterval( void );
        
        sequence_t*         AddAnimSeq( direction_t angle );
        sequence_t*         GetAnimSeq( direction_t angle );
        static void         PushSeq( sequence_t* seq, uint_t e );
        static uint_t       GetSeqSize( sequence_t* seq );
        static int          GetSeqFrameId( sequence_t* seq, uint_t step );
        
        // 設定撥放的方式
        // -1 == last one, 0 = first one
        void                SetPlayMode( IMAGE_PLAY_MODE mode, short frame = -1 );
        // 設定撥放的順序
        void                SetPlayOrder( IMAGE_PLAY_ORDER order );
        
        IMAGE_PLAY_MODE     GetPlayMode();
        short               GetPlayInterrupt();
        IMAGE_PLAY_ORDER    GetPlayOrder();
        
        //傳回act 號動作-順序的圖形編號
        int                 SetFrameId( direction_t angle, unsigned int step, unsigned int no );
        int                 GetFrameId( direction_t angle, unsigned int step );
        
        //
        unsigned short      GetMaxDirection( void );
        direction_t         GetNowAngle( void );
        int                 GetNowFrame( void );
        short               GetNowState( void );
        unsigned int        GetNowAnimSum( void );
        short               GetNowPlayCount( void );
        
        
        //傳回目前的動作是在動畫中的第幾張
        int                 GetNowFrameStep( void );
        void                SetNowFrameStep( int step );
        
        //目前這個動畫中一共有幾張
        size32_t            GetNowFrameCount( void );
        //傳回傳入的動作有多少張數
        int                 GetAngleFrameCount( direction_t angle );
        
        //upate animation state and index
        short               Update( int count = -1 );
        
        static void         SetSaveFileType( int t );
    };
}


#endif
