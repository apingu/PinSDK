// Body.h header file of body.cpp
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
#ifndef SPRITE2D_H
#define SPRITE2D_H

#pragma warning (disable : 4786)

//01. == C 系統文件/C++ 系統文件
#include <vector>
#include <string>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-timer.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "game_def.h"


#define GSO_STATIC_ID   0
namespace GSO
{
    //-----------------------------------------------------------------------------
    // NAME: class Sprite2d
    // DESC: to store base value use for game element
    //------------------------------------------------------------------------------
    class Sprite2d// : private CPGene
    {
    private:
    
        entityid_t               Id              ;        //ID
        entityid_t               GId             ;        //group id
        entityid_t               CId             ;        //class id
        uint_t                     m_Step;                 //目前走到的第幾點
        bool                     m_bEnable;               //is element enable    反應
        bool                     m_bVisible;              //is element visible   秀圖
        bool                     m_bMoveable;             //is elelment moveable 移動
        bool                     m_bDragable;             //is element dragable  拖曳
        bool                     m_bSelected;
        
        static _tstring          m_CurrentDirectory;
        
    protected:
        uint_t                     m_InitDistance;
        uint_t                     m_InitFlipRate;
        int                         m_tmp_distance;         //存放計算移動距離
        pftml::Timer                     m_StepRate;
        GameVertex                 m_Position;             //地圖位置
        uint_t                   m_Distance;             //****move distance    移動距離
        // 這個.....好像沒有用到耶....
        std::vector<int>         m_Face;                 //****行走路徑  push to protected screen pos
        std::vector<GameVertex>  m_Route;                //****行走路徑  push to protected screen pos
        
    public:
    
        Sprite2d();
        virtual ~Sprite2d();
        
        virtual const tchar_t*  ObjectName() const = 0;
        
        virtual void            SetId( entityid_t id )  { Id = id; }
        virtual void            SetGId( entityid_t gid ) { GId = gid; }
        virtual void            SetCId( entityid_t cid ) { CId = cid; }
        virtual entityid_t      GetId( void ) const     { return Id; }
        virtual entityid_t      GetGId( void ) const    { return GId; }
        virtual entityid_t      GetCId( void ) const    { return CId; }
        
        _tstring              m_Expression;      //注解
        _tstring              m_ResoureName;     //資源名稱
        
        void                    Initiate( void );
        virtual void            Clear( void );
        // 這個....沒有實作耶...
//        void                    Restore_Speed(void);

        static const tchar_t*   GetCurrentDirectory( void );
        static void             SetCurrentDirectory( const tchar_t* wdir );
        
        ////////////////////////////////////////////////////////////////////////////////////
        //position information group
        virtual GameVertex&        GetPos( int index = 0 );
        virtual void            SetPos( GameVertex pos );
        virtual GameVertex      GetScreenPos( void );
        
    protected:
        virtual GameVertex      OnGetOrigin() {return GameVertex();}
        
    public:
        //路徑
        // 取得要去的目標的位置，若沒有任何目標，則傳回目前的位置
        GameVertex              GetGoal( void ); //return where will move to
        // 取得下一個點，若非移動中，則傳回目前的位置
        GameVertex              GetNextRoute( void );
        // 將目前路徑清空
        void                    ClearRoute( void );
        // 以路徑裡下一個點更新位置
        int                     UpdateRoute( void );
        // 給定一個點，計算每次更新所要移動的點
        int                     FindRoute( GameVertex to );
        // 由後方加入一個點，成為新的目標
        void                    PushRoute( const GameVertex& naviPos );
        // 取得路徑的數量
        size32_t                GetRouteSize( void ) { return ( size32_t )m_Route.size(); }
        
        GameVertex                GetPredictPosition( void );
        
    public:
    
        ////////////////////////////////////////////////////////////////////////////////////
        //
        // 設定、取得移動速度
        uint_t&                 GetMoveSpeed( void );
        void                    SetMoveSpeed( uint_t dist );
        
        
    public:
        virtual bool            GetEnable( void ) const ;
        virtual bool            GetVisible( void ) const ;
        virtual bool            GetMoveable( void ) const ;
        virtual bool            GetDragable( void ) const ;
        virtual bool            GetSelected( void ) const ;
        
        virtual void            SetEnable( bool f );
        virtual void            SetVisible( bool f );
        virtual void            SetMoveable( bool f );
        virtual void            SetDragable( bool f );
        virtual void            SetSelected( bool f );
        
        
    public:
        int                     m_Collision;                 //碰撞產生Flag
        void*                     m_Descriptor;
        
    public:
        virtual void            OnShow( bool f ) {};                // when show
        virtual void            OnMoving( const GameVertex& pos ) {}; // when show
        virtual void            OnArrival( GameVertex pos ) {};     // when go arrival
        
        
    };
};//namespace


#endif
