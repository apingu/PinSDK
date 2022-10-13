//Role.h header file of Role.cpp
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
//                                                         Pin
//
#ifndef ROLE2D_H
#define ROLE2D_H

#pragma warning (disable : 4786)

#include <vector>
#include <list>
#include <arraymap.inl>
#include <basedef.h>
#include "IEntity2d.h"
//#include "doll.h"


namespace DOLL
{
    class CDoll;
	class SItem;
};


//Active status
namespace GSO
{
    namespace World
    {

        //=============================================================================
        /// 0 __1___2_3_4 action end
        ///         5_6_7 move   end
        /// do one action must have action and direction
        /// 0 wait for action and direction | 1 only have action or direction | 
        /// 2 begin action | 3 actioning | 4 end act   |
        /// 5 begin moving | 6 moving    | 7 end moving|
        /// judgment GetNowMotionStage()
        //=============================================================================
        enum MOTIONSTAGE { _STAGE_WATTING    = 0,    // 
                           _STAGE_PREPARE    = 1,
                           _STAGE_BEGIN_DO   = 2,
                           _STAGE_DOING      = 3,
                           _STAGE_DO_FINISH  = 4,
                           _STAGE_BEGIN_MOVE = 5,
                           _STAGE_MOVING     = 6,
                           _STAGE_ARRIVE     = 7 };

        enum MOTIONSTATE { DO_NOW_STAND,            // 設定為平時待機動作，並馬上執行
                           DO_NEXT_STAND,            // 設定為平時待機動作，但等候所有動作完成
                           DO_ATTACH_MOTION,        // 設定下一步動作
                           DO_REPLACE_MOTION };        // 取消目前所有動作，並馬上進行該動作

         //const int kTypeChar = 0;
        //const int kTypeBld    = 1;
        //const int kTypeNone = 2;

        
        /////////////////////////////////////////////////////////////////////////////////////////////
        // Role2d class 角色結構設定
        class Role2d : public IEntity2d//, public OBPathSearch
        {
            typedef IEntity2d BaseClass;
        public:
            Role2d();
            virtual ~Role2d();

            const tchar_t*     ObjectName() const { return _T("Role2d"); }

            virtual void       Release();
            virtual int        Update(int count = -1);                //direction actS

        public:
            const tchar_t*     GetWantToDo( int i );
            MOTIONSTAGE        GetNowMotionStage( void );

            //設定路徑搜尋的範圍
            uint_t             GetWalkRange( void );
            void               SetWalkRange( uint_t range );

            // 如果 anm == NULL -->會從世界上被移除
            // 注意: 如果想要做到此行為，請務必保留該物件的指標!!以免造成memory leak
            void               Do( const tchar_t* anm, MOTIONSTATE state = DO_REPLACE_MOTION  );  //set act
            void               To( GameVertex MapPos );               //set object 
            void               GoTo( GameVertex MapPos );             //set object

        protected:
            //動作
            void               JudgMotion( void );
            int                JudgStatus( void );
            virtual int        JudgCollision( void ); 
            
        protected:

            basestring                    m_StandAnm;
            ptrdlist<basestring>          m_WantToDo;   //準備要做的動作  push to protected
            static pt::arraymap<int>      m_WeightMap;  //加權值

            void               EndMotion( void );

        private:
            int                Search_Path ( const GameVertex& mapFrom,
                                             const GameVertex& mapTo );

            void               OnMakeDiffuse( const GameVertex& RoleGridPos,
                                              uint_t  range );// search road can walk
            
            int                OnSearchPath( std::vector<GameVertex>* path, 
                                             const GameVertex& localGridFrom,
                                             const GameVertex& localGridTo );
            //void OnSearchPushResult( const std::vector<GameVertex>& path, const GameVertex& globalMapRef,
            //                         const GameVertex& gridFrom );//, const GameVertex& mapFrom );
            //bool GetNextGridMapPos ( int direction, const GameVertex& globalMapRef, 
            //                         GameVertex& gridStart, GameVertex& dest );

        private:
            /// 走地圖時需要的步數 
            uint_t               m_Steps;
            uint_t               m_SearchRange;
            uint_t               m_RegionId;
            
            /// 目前動作狀態(0 - 等待, 1 - 準備, 2 - 開始動作, 3 - 動作進行中, 4 - 完成動作);
            MOTIONSTAGE          m_NowMotionStage;            

        };
        


        ///////////////////////////////////////////////////////////////////////////////
        // class avatarrole
        
        class AvatarRole :public Role2d
        {
            typedef Role2d BaseClass;
        public:
            AvatarRole();
            ~AvatarRole();

            virtual const tchar_t*     ObjectName() { return _T("AvatarRole");}

            /// 啟始紙娃娃
            static int                Init(uint_t layers);

		public:


            /// 設定裝備對映(那一個裝備對映到哪一個子節點)
            static void               SetItemMapping(int layer, const tchar_t* name);
            static int                GetItemMapping(const tchar_t* name);
            
            /// 讀自己的圖層，如果是用紙娃娃，最好不要用
            int                       Load( const tchar_t* filename );
            /// 新的load函式，由外部呼叫
            int                       Load(uint_t layer, DOLL::SItem * item, const tchar_t* filename);


            virtual const CPIcon*     Animation( int count = -1 );                  //Animation + Imageplaying image
            virtual const CPIcon*     GetImage( uint_t id );

            /// 裝備
            int                       AttachItem(uint_t layer, DOLL::SItem* item, const tchar_t* filename);
            int                       AttachItem(const tchar_t *layername, DOLL::SItem* item, const tchar_t* filename);
            
            /// 卸下裝備
            int                       DetachItem(uint_t layer);
            int                       DetachItem(const tchar_t *layername);

            /// 換裝
            int                       ChangeItem(uint_t layer, DOLL::SItem* item, const tchar_t* filename);
            int                       ChangeItem(const tchar_t *layername, DOLL::SItem* item, const tchar_t* filename);

            /// 取得排序後的image
            CPIcon*                   GetSortedImage(uint_t sequence, int _tick = -1);
    
            /// 摧毀
            void                      Destroy(void);

            /// 把要求的子節點清掉，注意，0是代表父節點，沒意義
            int                       DetachChildren(uint_t layer);

            /// 取得子節點
            AvatarRole*               GetChild(uint_t layer);


            /// 載入動畫
            error_t                   LoadAnm(const tchar_t *filename);

            /// 取得layer
            int                       GetLayers();
            bool                      GetLayerMask(int layer);

            virtual bool              IsCollisionByImage( GameVertex mpos );

            static int                LoadAvatarInfo(const tchar_t * filename);

		protected:

			static DOLL::SItem*       GetAvatarTableItem(const tchar_t *filename, const tchar_t *group = 0);
			static DOLL::SItem*       GetAvatarTableItem(int id);
			static std::string        GetAvatarTableName(const DOLL::SItem* item);


        private:

			/// 取得item的資料
			DOLL::SItem*              GetItemInfo()    { return m_pItemInfo; }

            /// 物件的資訊
            DOLL::SItem*              m_pItemInfo;
            CPIcon*                   _CurrIcon;

            uint_t                    m_ImageSumSize;
            std::list< CPIcon* >      m_ImageCache;// new->old
                        
            /// 其它的圖層，配合紙娃娃系統使用
            std::vector<AvatarRole*>  m_Children;
        };

        //=====================================================================
        class Avatar: public Role2d
        {
            typedef Role2d BaseClass;
        public:
            Avatar();
            virtual ~Avatar();

        public:
            virtual const tchar_t* ObjectName() { return _T("Avatar");}
            virtual bool          IsCollisionByImage(GameVertex mpos);
            virtual const CPIcon* Animation(int count = -1);                  //Animation + Imageplaying image
            virtual const CPIcon* GetImage(uint_t id);

        public:
            /// 摧毀
            void                  Destroy();
            void                  FreeLayer(unsigned int layer);
            int                   Load(unsigned int layer, const tchar_t* filename);
            /// 讀自己的圖層，如果是用紙娃娃，最好不要用
            int                   Load( const tchar_t* filename );
            /// 載入動畫
            error_t               LoadAnm(const tchar_t *filename);

        private:

            CPIcon*                  _CurrIcon;
            uint_t                     m_ImageSumSize;
            std::list<CPIcon*>       m_ImageCache;// new->old
                        
            /// 其它的圖層，配合紙娃娃系統使用
            std::vector<Element2d*>  m_Children;
            
        };

    };//World

};//GE



#endif
