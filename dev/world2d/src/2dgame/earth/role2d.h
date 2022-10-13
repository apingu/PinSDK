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

#include <pf-def.h>
#include "ientity2d.h"

#include <template\arraymap.inl>


namespace DOLL
{
    class CDoll;
    class AvatarNode;
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
        /// 9 remove from world
        /// judgment GetNowMotionStage()
        //=============================================================================
        enum MOTIONSTAGE { _STAGE_WATTING    = 0,    // 
                           _STAGE_PREPARE    = 1,
                           _STAGE_BEGIN_DO   = 2,
                           _STAGE_DOING      = 3,
                           _STAGE_DO_FINISH  = 4,
                           _STAGE_BEGIN_MOVE = 5,
                           _STAGE_MOVING     = 6,
                           _STAGE_ARRIVE     = 7,
                           _STAGE_REMOVE     = 9};

        enum MOTIONSTATE { DO_NOW_STAND,            // 設定為平時待機動作，並馬上執行
                           DO_NEXT_STAND,           // 設定為平時待機動作，但等候所有動作完成
                           DO_ATTACH_MOTION,        // 設定下一步動作
                           DO_REPLACE_MOTION,       // 取消目前所有動作，並馬上進行該動作
                           DO_MAX };

         //const int kTypeChar = 0;
        //const int kTypeBld    = 1;
        //const int kTypeNone = 2;

        
        /////////////////////////////////////////////////////////////////////////////////////////////
        // Role2d class 角色結構設定
        class Role2d : public IEntity2d//, public OBPathSearch
        {
            typedef IEntity2d BaseClass;


            /// 走地圖時需要的步數 
            uint_t                        m_Steps;
            uint_t                        m_SearchRange;
            uint_t                        m_RegionId;

            /// 目前動作狀態(0 - 等待, 1 - 準備, 2 - 開始動作, 3 - 動作進行中, 4 - 完成動作);
            MOTIONSTAGE                   m_NowMotionStage;
            static pfstl::arraymap<int>      m_WeightMap;  //加權值

            // new
            struct MationNode
            {
                _tstring  WantToDo;   //要做的動作
                GameVertex  TargetPos;  //目的點
                int8_t      type;       //要做的類型: 0 站立做動作 1 移動作動作
            };
            std::vector<MationNode>       m_MotionList;


        public:
            Role2d();
            virtual ~Role2d();

            const tchar_t*     ObjectName() const { return _T("Role2d"); }

            virtual void       Clear();
            virtual int        Update(int count = -1);                //direction actS

        public:
            //const tchar_t*     GetWantToDo(int i);
            MOTIONSTAGE        GetNowMotionStage(void);

            //設定路徑搜尋的範圍
            uint_t             GetWalkRange(void);
            void               SetWalkRange(uint_t range);

            
            // do something face to direction
            void               DoTo(const tchar_t* anm, direction_t  dirt,   MOTIONSTATE state = DO_REPLACE_MOTION);
            // do something face to someplace
            void               DoTo(const tchar_t* anm, GameVertex   MapPos, MOTIONSTATE state = DO_REPLACE_MOTION);
            // do something go to someplace
            void               GoTo(const tchar_t* anm, GameVertex   MapPos, MOTIONSTATE state = DO_REPLACE_MOTION);
            
           
        protected:



            virtual int        JudgCollision(void); 


        private:

            void               Motion(const tchar_t* anm, GameVertex MapPos, int type,  MOTIONSTATE state);


            int                Search_Path (const GameVertex& mapFrom,
                                            const GameVertex& mapTo);

            void               OnMakeDiffuse(const GameVertex& RoleGridPos,
                                             uint_t  range);// search road can walk
            
            int                OnSearchPath(std::vector<GameVertex>* path, 
                                             const GameVertex& localGridFrom,
                                             const GameVertex& localGridTo);
        };
        


        ///////////////////////////////////////////////////////////////////////////////
        // class avatarrole
        /*
        enum AVATAR_LAYER_INDEX
        {
            AVT_BODY,        ///< 0  身體
            AVT_HEAD,        ///< 1  頭
            AVT_LEFTHAND,    ///< 2  左手
            AVT_RIGHTHAND,    ///< 3  右手
            AVT_BOOTS,        ///< 4  鞋
            AVT_CLOTH,        ///< 5  上衣
            AVT_SLACKS,        ///< 6  褲子
            AVT_HAIR,        ///< 7  頭髮
            AVT_HAT,        ///< 8  帽子
            AVT_OUTCLOTH,    ///< 9  外衣(夾克)
            AVT_FACE1,        ///< 10 臉表情
            AVT_FACE2,        ///< 11 臉裝飾
            AVT_ITEM1,        ///< 12 紙娃娃裝備1
            AVT_ITEM2,        ///< 13 紙娃娃裝備2
            AVT_END            ///< 14 最後一項不使用
        };
        */

        
        class AvatarRole :public Role2d
        {
            typedef Role2d BaseClass;


        public:
            AvatarRole();
            ~AvatarRole();

            virtual const tchar_t*    ObjectName() { return _T("AvatarRole");}

        public:


            /// 設定裝備對映(那一個裝備對映到哪一個子節點)
            static void               SetLayerNameMapping(int layer, const tchar_t* name);
            static int                GetLayerNameMapping(const tchar_t* name);
            

            virtual const CPIcon*     Animation(int count = -1);                  //Animation + Imageplaying image
            virtual const CPIcon*     GetImage(uint_t id);

            /// 裝備
            int                       DressItem(uint_t layer, const tchar_t* filename, longlong_t itemid=-1);
            int                       DressItem(const tchar_t* layername, const tchar_t* filename, longlong_t itemid=-1);
            
            /// 卸下裝備
            int                       UndressItem(uint_t layer);
            int                       UndressItem(const tchar_t* layername);

            /// 換裝
            int                       ChangeItem(uint_t layer, const tchar_t* filename, longlong_t itemid=-1);
            int                       ChangeItem(const tchar_t* layername, const tchar_t* filename, longlong_t itemid=-1);

    
            /// 摧毀
            void                      Destroy(void);

            /// 載入動畫
            error_t                   LoadAnm(const tchar_t* filename);

            /// 取得layer
            uint_t                    GetLayers();
            bool                      GetLayerMask(uint_t layer);
            void                      SetLayerVisible(uint_t layer, bool v);


            /// 讀自己的圖層，如果是用紙娃娃，最好不要用
            int                       Load(const tchar_t* filename);

            // 讀入紙娃娃系統的
            // 排序表(也可不用 用於要改變 排序的順序時)
            // 道具資料表
            static int                LoadAvatarTable(const tchar_t*  ordertable, const tchar_t*  itemtable);


        protected:

            /// 取得子節點
            AvatarRole*               GetChildLayer(uint_t layer);


            virtual bool              IsCollisionByImage(GameVertex mpos);

        private:


            /// 取得排序後的image
            AvatarRole*               GetSortedItem(uint_t sequence);

            /// 取得item的資料
            DOLL::AvatarNode*         GetItemInfo()    { return m_pItemInfo; }

            /// 物件的資訊
            DOLL::AvatarNode*         m_pItemInfo;
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
            int                   Load(const tchar_t* filename);
            /// 載入動畫
            error_t               LoadAnm(const tchar_t* filename);

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
