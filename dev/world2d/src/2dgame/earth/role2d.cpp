//Role.cpp game world role class
//
//
//
//
//
//
//
//                                                                  Pin
//

//01. == C 系統文件/C++ 系統文件
#include <stack>
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <algorithmic\ob_path_search.h>
//
#include <memio.inl>
#include <graphic\vmc-draw.h>


//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "role2d.h"
#include "terrain2d.h"
#include "doll.h"


//#pragma comment (lib, "VMCEngine.lib")


namespace GSO
{
    namespace World
    {
        /// by sean 有多少圖層
        namespace AvatarTable
        {
            DOLL::CDoll              g_Avatar;
            DOLL::CAvatarList        g_AvatarMgr;
            unsigned short           g_Layers = 16;        /// 有多少圖層
            std::vector<tchar_t*>   g_LayerNameMapping; /// 一個LAYER 對應到一個字串


            /// 這兩個有參數的GetItemInfo函式是用來取得doll裡的資料的，而不是本身的
            /*
            DOLL::AvatarNode* GetAvatarTable(const tchar_t* filename, const tchar_t* group)
            {
                return g_Avatar.m_pItemList->GetItemInfo(filename, group);
            }

            DOLL::AvatarNode* GetAvatarTable(int id)
            {
                return g_Avatar.m_pItemList->GetItemInfo(id);
            }
            */
        }

        pfstl::arraymap<int>  Role2d::m_WeightMap;  //加權值
        uint_t                g_MaxImageSize = 128;
        OBPathSearch          g_Pathsearch;
        const int             _MAX_SERACH_RANGE = 45;

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Role2d::Role2d(): m_Steps( 0 ), m_SearchRange( 0 )//, m_Type(kTypeNone)
        {
            int temp = 0;
            memcpy( &temp, "ROLE", sizeof( int ) );
            SetCId( temp );
            m_RegionId = 0;
            m_Collision = 0;
            m_NowMotionStage = _STAGE_WATTING;
            m_MotionList.clear();
            SetWalkRange( _MAX_SERACH_RANGE );
            RenderLayer = LGB::DrawNode::kLayerRole;
            return;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Role2d::~Role2d()
        {}


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Role2d::Clear()
        {
            BaseClass::Clear();
            m_Steps = 0;
            //m_obj = 0;    //使用道具
            m_Route.clear();
            m_TargetSeq.clear();
            //m_WantToDo.clear();
            m_MotionList.clear();
        }


        /*
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        const tchar_t*    Role2d::GetWantToDo(int i)
        {
            return (i<m_WantToDo.size()) ? NULL : m_WantToDo[i].c_str();
        }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        MOTIONSTAGE    Role2d::GetNowMotionStage( void )
        {
            return m_NowMotionStage;
        }


        //=============================================================================
        // NAME: to_my_self()
        // DESC: compute my self face to
        //=============================================================================
        GameVertex to_my_self( int now_dir, GameVertex now_situs )
        {
            GameVertex facepos;
            switch( now_dir )
            {
            case 0:
                facepos( now_situs.x, now_situs.y - 1, 0 );   break;
            case 45:
                facepos( now_situs.x + 1, now_situs.y - 1, 0 ); break;
            case 90:
                facepos( now_situs.x + 1, now_situs.y, 0 );   break;
            case 135:
                facepos( now_situs.x + 1, now_situs.y + 1, 0 ); break;
            case 180:
                facepos( now_situs.x, now_situs.y + 1, 0 );   break;
            case 225:
                facepos( now_situs.x - 1, now_situs.y + 1, 0 ); break;
            case 270:
                facepos( now_situs.x - 1, now_situs.y, 0 );   break;
            case 315:
                facepos( now_situs.x - 1, now_situs.y - 1, 0 ); break;
            }


            /*
                     switch(dir_count)
                     {
                     case 4:
                     {
                         switch(now_dir)
                         {
                         case 0:facepos(now_situs.x, now_situs.y + 1);
                         case 1:facepos(now_situs.x - 1, now_situs.y);
                         case 2:facepos(now_situs.x, now_situs.y - 1);
                         case 3:facepos(now_situs.x + 1, now_situs.y);
                         }
                     }
                     default:
                     {
                         switch(now_dir)
                         {
                         case 0:facepos(now_situs.x + 1, now_situs.y + 2);
                         case 1:facepos(now_situs.x, now_situs.y + 1);
                         case 2:facepos(now_situs.x - 1, now_situs.y + 1);
                         case 3:facepos(now_situs.x, now_situs.y - 1);
                         case 4:facepos(now_situs.x - 1, now_situs.y - 1);
                         case 5:facepos(now_situs.x, now_situs.y - 1);
                         case 6:facepos(now_situs.x + 1, now_situs.y - 1);
                         case 7:facepos(now_situs.x + 1, now_situs.y);
                         }
                     }
                    }
                    */
            return facepos;
        }



#define _ROLE_MOTION_TYPE_DO    1
#define _ROLE_MOTION_TYPE_GO    3

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Role2d::DoTo( const tchar_t* anm, direction_t dirt, MOTIONSTATE state )
        {
            Motion( anm, JudgPosition( dirt ), _ROLE_MOTION_TYPE_DO, state );
        }

        void Role2d::DoTo( const tchar_t* anm, GameVertex MapPos, MOTIONSTATE state )
        {
            Motion( anm, MapPos, _ROLE_MOTION_TYPE_DO, state );
        }

        void Role2d::GoTo( const tchar_t* anm, GameVertex   MapPos, MOTIONSTATE state )
        {
            Motion( anm, MapPos, _ROLE_MOTION_TYPE_GO, state );
        }


        void Role2d::Motion( const tchar_t* anm, GameVertex MapPos, int type, MOTIONSTATE state )
        {
            if( ( anm != NULL ) && ( strcmp( anm, "" ) != 0 ) )
            {
                tchar_t buf[ _MAX_LFNAME ];
                mcurrentpath( buf, anm );

                switch( state )
                {
                case DO_REPLACE_MOTION://只做此動作
                    {
                        // 沒有待機動作，所以設此動做為待機動作
                        m_NowMotionStage = _STAGE_WATTING;//中斷動作

                        if( strcmp( GetAnmfname(), "" ) == 0 )
                        {
                            Element2d::LoadAnm( buf );
                            FaceTo( MapPos );
                        }

                        MationNode standnode;
                        if( !m_MotionList.empty() )
                        {
                            // 目前在做其他的動作 先記下來原本的待機動作是甚麼
                            standnode.type = m_MotionList[ 0 ].type;
                            standnode.WantToDo = m_MotionList[ 0 ].WantToDo;
                            standnode.TargetPos = m_MotionList[ 0 ].TargetPos;
                        }
                        else
                        {
                            standnode.type = _STAGE_BEGIN_DO;
                            standnode.WantToDo = GetAnmfname();
                            standnode.TargetPos = MapPos;
                        }

                        m_MotionList.clear();
                        m_MotionList.push_back( standnode );

                        MationNode nextmotion;
                        nextmotion.type = type;
                        nextmotion.WantToDo = anm;
                        nextmotion.TargetPos = MapPos;
                        m_MotionList.push_back( nextmotion );

                        break;
                    }
                case DO_NOW_STAND://立刻改變動作
                    {
                        m_NowMotionStage = _STAGE_WATTING;//中斷動作
                        m_MotionList.clear();
                        Element2d::LoadAnm( buf );
                        FaceTo( MapPos );
                        break;
                    }
                case DO_NEXT_STAND://下一個動作狀態
                    {
                        MationNode nextstand;
                        nextstand.type = _STAGE_BEGIN_DO;
                        nextstand.WantToDo = anm;
                        nextstand.TargetPos = MapPos;

                        // 記錄下來 待機動作
                        if( m_MotionList.empty() )
                        {
                            m_MotionList.push_back( nextstand );
                        }
                        else
                        {
                            m_MotionList[ 0 ].type = _STAGE_BEGIN_DO;
                            m_MotionList[ 0 ].WantToDo = anm;
                            m_MotionList[ 0 ].TargetPos = MapPos;
                        }

                        break;
                    }
                    //case DO_ATTACH_MOTION:
                default://連續動作狀態
                    {
                        // 記錄下來 待機動作
                        if( m_MotionList.empty() )
                        {
                            MationNode nextstand;
                            nextstand.type = _STAGE_BEGIN_DO;
                            nextstand.WantToDo = GetAnmfname();
                            m_MotionList.push_back( nextstand );
                        }

                        MationNode nextmotion;
                        nextmotion.type = type;
                        nextmotion.WantToDo = anm;
                        nextmotion.TargetPos = MapPos;
                        m_MotionList.push_back( nextmotion );
                        break;
                    }
                }
            }
            else
            {

            }

        }

        /*
                //=============================================================================
                // NAME:
                // DESC:
                //=============================================================================
                bool Role2d::Go_To_NextGrid(int direction)
                {
                    GameVertex dest(0, 0);
                    if (!GetNextGridMapPos(direction, GetPos(), dest))
                    {
                        return false;
                    }

                    m_Route.clear();
                    FindRoute(GetPos(), dest);
                    //          m_Route.GoBegin();
                    return true;
                }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================


        //=============================================================================
        ///
        /// 0 __ ___2_3_4 action end
        ///         5_6_7 move   end
        /// do one action must have action and direction
        /// 0 wait for action and direction | no use | 
        /// 2 begin action | 3 actioning | 4 end act   |
        /// 5 begin moving | 6 moving    | 7 end moving|
        /// 9 remove
        /// judgment GetNowMotionStage()
        /// **if moving state Route size must> 1 for judg with doing state
        //=============================================================================

        int Role2d::Update( int count )
        {
            //if (!GetEnable())
            //    return 0;

            Element2d::Update();

            // 先做判斷階段
            {
                // 上一次是中斷的動作
                switch( m_NowMotionStage )
                {
                case _STAGE_WATTING:
                    {
                        if( m_MotionList.size() >= 2 )
                        {
                            // 假如是移動類型
                            if( m_MotionList[ 1 ].type == _ROLE_MOTION_TYPE_GO )
                            {
                                m_NowMotionStage = _STAGE_BEGIN_MOVE;
                            }
                            else if( m_MotionList[ 1 ].type == _ROLE_MOTION_TYPE_DO )
                            {
                                m_NowMotionStage = _STAGE_BEGIN_DO;
                            }
                            //否則都當他是要移除
                            else
                            {
                                m_NowMotionStage = _STAGE_REMOVE;
                            }

                        }
                    }
                    break;
                    // 上次是開始移動
                case _STAGE_BEGIN_MOVE:  { m_NowMotionStage = _STAGE_MOVING; } break;
                    // 上次是開始動作
                case _STAGE_BEGIN_DO:    { m_NowMotionStage = _STAGE_DOING;  } break;
                    // 移動中 到達目的地算完成動作
                case _STAGE_MOVING:
                    {
                        if( GetGoal() == GetPos() )
                            m_NowMotionStage = _STAGE_ARRIVE;
                    }
                    break;
                    // 動作中 做一次算完成動作
                case _STAGE_DOING:
                    {
                        if( m_Anim2d.IsNowAnimInFrame( -1 ) )
                            m_NowMotionStage = _STAGE_DO_FINISH;
                    }
                    break;
                    // 到達目的地
                case _STAGE_ARRIVE:     { m_NowMotionStage = _STAGE_WATTING; } break;
                    // 完成動作
                case _STAGE_DO_FINISH:  { m_NowMotionStage = _STAGE_WATTING; } break;
                }
            }

            // 判斷每個階段要做啥
            {
                // 上一次是中斷的動作
                switch( m_NowMotionStage )
                {
                    // 等待狀態
                case _STAGE_WATTING:
                    {
                    }
                    break;
                    // 開始移動
                case _STAGE_BEGIN_MOVE:
                    {
                        // compute work path
                        // TODO: Alex, Why should I control the inner state of sprite outside sprite
                        ClearRoute();
                        m_WeightMap.zero_value();

                        GameVertex MapPos = m_MotionList[ 1 ].TargetPos;
                        // 有在世界裡面 就做路徑搜尋
                        if( GetWorld() != NULL )
                        {
                            Search_Path( GetPos(), MapPos );
                        }
                        else// role not have world go line path
                        {
                            // TODO: Alex, This should not happen at this time.
                            assert( 0 && "Role2d::GoTo: Zone not exist!" );
                            FindRoute( MapPos );
                        }

                        if( m_Route.empty() )
                            PushRoute( GetPos() );
                        else
                            PushRoute( m_Route.back() );
                        //m_Route.push_back(m_Route.back());//讓最後一個點最少有兩個


                        // 改變動作
                        LoadAnm( m_MotionList[ 1 ].WantToDo.c_str() );
                        FaceTo( GetNextRoute() );

                    }
                    break;
                    // 上次是開始動作
                case _STAGE_BEGIN_DO:
                    {
                        GameVertex MapPos = m_MotionList[ 1 ].TargetPos;
                        // 維持原來方向不變
                        if( MapPos == GetPos() )
                            MapPos = to_my_self( m_Anim2d.GetNowAngle(), GetPos() );

                        ClearRoute();
                        // because when get next route we will add 1 to be next postion, so 
                        // we must push two times
                        PushRoute( MapPos );

                        //if this role put in world, judg if to situs have some one there
                        //            if((GetWorld() != NULL) && (!GetWorld()->IsBoardEmpty()))
                        // 假如有人在這個位置上 跟他建立關西
                        if( ( GetWorld() != NULL ) )
                        {
                            Zone2d* residentZone = GetWorld()->GetZone( GetPos() );
                            if( residentZone && !residentZone->IsBoardEmpty() )
                            {
                                for( unsigned int i = 0; i < GetWorld()->GetEntityCount(); i++ )
                                {
                                    IEntity2d* target = ( IEntity2d* )( *GetWorld() )[ i ];
                                    if( target->IsCollisionByBound( MapPos ) )
                                    {
                                        if( target != this )//that guy not myself
                                            BuildInteraction( target ); // i will do some thing to that guy
                                        break;
                                    }
                                }
                            }
                        }


                        // 改變動作
                        LoadAnm( m_MotionList[ 1 ].WantToDo.c_str() );
                        FaceTo( GetNextRoute() );
                    }
                    break;
                    // 移動中 到達目的地算完成動作
                case _STAGE_MOVING:
                    {
                        // 仍在移動中
                        if( UpdateRoute() == 0 )
                        {
                            if( GetWorld() != NULL )
                                FaceTo( GetPredictPosition() );
                            else
                                FaceTo( GetNextRoute() );
                        }
                    }
                    break;
                    // 動作中 做一次算完成動作
                case _STAGE_DOING:
                    {
                    }
                    break;
                    // 到達目的地
                case _STAGE_ARRIVE:
                    // 完成動作
                case _STAGE_DO_FINISH:
                    {
                        m_TargetSeq.clear();
                        ClearRoute();

                        // 移除掉剛剛做的那一個node
                        m_MotionList.erase( m_MotionList.begin() + 1 );
                        // 沒動作的 回復待機狀態
                        if( m_MotionList.size() == 1 )
                        {
                            LoadAnm( m_MotionList[ 0 ].WantToDo.c_str() );
                            FaceTo( GetPos() );
                            m_MotionList.clear();
                        }
                    }
                    break;
                default:
                    {
                        if( GetWorld() != NULL )
                            GetWorld()->PushToRecycleBin( this );
                    }
                    break;
                }
            }

            return 1;
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
/*
        void Role2d::WashBrain()
        {
            Element2d::m_Anim2d.Load(m_StandAnm);
            ResetSpeed();
            m_Route.clear();
            m_WeightMap.clear();
            m_WantToDo.clear();

            m_NowMotionStage = _STAGE_WATTING;
            m_Steps = 0;
        }
*/

/*
            // 排序
            m_ItemList.m_Doll.m_pSort->BeginSort(m_ItemList.m_NowDirect / 45);
            for (RUN_ANIM::iterator iter = m_ItemList.m_Anm.begin(); iter != m_ItemList.m_Anm.end(); ++iter)
                m_ItemList.m_Doll.m_pSort->Item((*iter)->item);
            ListType *list = m_ItemList.m_Doll.m_pSort->EndSort();

            if (m_ItemList.m_Anm.size()> 0)
            {
                elem.m_Anim2d.Update();
                uint_t tick = elem.m_Anim2d.GetNowFrame();
                for (ListType::iterator it = list->begin(); it != list->end(); ++it)
                    for (RUN_ANIM::iterator iter = m_ItemList.m_Anm.begin(); iter != m_ItemList.m_Anm.end(); ++iter)
                    {
                        if (strcmp((*it)->filename, (*iter)->item->filename) == 0)
                        {
                            (*iter)->elem.m_Anim2d.ChangeDirection(m_ItemList.m_NowDirect);
                            m_pVPage->AlphaBlit((*iter)->elem.m_pResource2d->GetRes(tick),
                                LG::Point(0, 0), VMCanvas);
                        }
                    }
            }
*/
/*
void Role2d::STANDBY(void)
{
    return ;//Image();
}
void Role2d::WALK(void)
{
    return ;//Image();
}
void Role2d::ATTACK(void)
{
    return ;//Image();
}
void Role2d::DEFENCE(void)
{
    return ;//Image();
}
void Role2d::MAGIC(void)
{
    if(GetNowMotionStage() == 2)
    {
        if(m_obj != 0)
        {
            m_obj->Do(AS_ATTACK);
            m_obj->Do(AS_DESTORYSELF);
            m_obj->GetPos()(m_Route.back().x, m_Route.back().y + 1);
            //m_obj->m_To = m_To;
            GetWorld()->Plant_Role(m_obj);
         }
     }
     return ;//Image();
}
void Role2d::HITBACK(void)
{
    return ;//Image();
}
void Role2d::MAGICATTACK(void)
{
    return ;//Image();
}
*/


/*

//=============================================================================
///
/// 0 __1___2_3_4 action end
///         5_6_7 move   end
/// do one action must have action and direction
/// 0 wait for action and direction | 1 only have action or direction |
/// 2 begin action | 3 actioning | 4 end act   |
/// 5 begin moving | 6 moving    | 7 end moving|
/// judgment GetNowMotionStage()
/// **if moving state Route size must> 1 for judg with doing state
//=============================================================================
int Role2d::JudgStatus(void)
{
    //0
    if ((m_Route.empty()) && (m_WantToDo.empty()))
        return m_NowMotionStage = _STAGE_WATTING;


    switch(GetNowMotionStage())
    {
      case _STAGE_BEGIN_MOVE:
        {
            //6have dest, have action, situs between from and dest
            //if((!m_Route.empty()) && (!m_WantToDo.empty()) && (GetPos() != m_Route.front()) && (GetPos() != m_Route.back()))//  (GetNowMotionStage()>= 2) && ((m_WantToDo() == m_Anim_Table.m_NowAct) || (GetNowMotionStage() == 3)))
            return m_NowMotionStage = _STAGE_MOVING;
        }
      case _STAGE_BEGIN_DO:
        {
            //if((!m_Route.empty()) && (!m_WantToDo.empty()) && (m_Anim_Table.m_NowAct == m_WantToDo()))//  (GetNowMotionStage()>= 2) && ((m_WantToDo() == m_Anim_Table.m_NowAct) || (GetNowMotionStage() == 3)))
            return m_NowMotionStage = _STAGE_DOING;
        }

      case _STAGE_MOVING:
        {
            //if (m_Route.size()<=1)
            if(GetGoal()==GetPos())
                return m_NowMotionStage = _STAGE_ARRIVE;
            return GetNowMotionStage();
        }
        //
      case _STAGE_DOING:
        {
            //if (m_Anim2d.GetNowState() <= -1)  //end to action
            if(    m_Anim2d.IsNowAnimInFrame(-1))
                return m_NowMotionStage = _STAGE_DO_FINISH;
            return GetNowMotionStage();
        }
    }

    //1
    if (((m_Route.empty()) + (m_WantToDo.empty())) == 1)
        return m_NowMotionStage = _STAGE_PREPARE;
    //2
    if ((!m_WantToDo.empty()) && (m_Route.size() == 1))//(m_Anim_Table.m_NowAct != m_WantToDo.front()))//& &((GetNowMotionStage() <2) || (GetNowMotionStage() == 4)))//begin to action
        return m_NowMotionStage = _STAGE_BEGIN_DO;
    //5 have dest, have action, situs in from
    if ((!m_WantToDo.empty()) && (m_Route.size()> 1))//(GetPos() != m_Route.front()))//& &((GetNowMotionStage() <2) || (GetNowMotionStage() == 4)))//begin to action
        return m_NowMotionStage = _STAGE_BEGIN_MOVE;
    return -1;
}


//=============================================================================
// NAME: JudgAct()
// DESC: judgment which value to be set in when status
//=============================================================================
void Role2d::JudgMotion(void)
{
    switch (GetNowMotionStage())
    {
      case _STAGE_ARRIVE:
        //7
        {
            EndMotion();
            return;
        }
      case _STAGE_DO_FINISH:
        //4
        {
            m_WantToDo++;
            if (m_WantToDo.isBegin())        //做完一次行為後，動作串列裏沒有下一個準備的行為
                EndMotion();                //清除所有動作
            //else                          //跳回狀態2，準備作動作串列裏的下一個的動作
            //{
            //m_Anim_Table.m_NowAct = -1;
            //}
            return;
        }
      case _STAGE_MOVING:
        {
            if (m_Anim2d.GetNowState() <-1)  //end to action
            {
                if(!m_WantToDo.empty())
                {
                    m_WantToDo++;
                    std::string str = m_WantToDo();
                    if (!str.empty())
                    {
                        Element2d::LoadAnm(str.c_str());
                    }
                }
            }
            // 仍在移動中
            if (UpdateRoute() == 0)
            {
                if (GetWorld() != NULL)
                    FaceTo(GetPredictPosition());
                else
                    FaceTo(GetNextRoute());
            }
            return;
        }
      //do same thing
      case _STAGE_BEGIN_DO://2
      case _STAGE_BEGIN_MOVE://5
        {
            if(!m_WantToDo.empty())
            {
                //m_WantToDo.GoBegin();
                Element2d::LoadAnm(m_WantToDo().c_str());
            }
            FaceTo(GetNextRoute());
            return;
        }
    }

    return ;//m_Anim_Table.m_NowAct;
}
*/

//=============================================================================
//
//
//=============================================================================
/*
void Role2d::OnResponse(Role2d* who)
{
    if (m_Target.empty())
        return;
    /*
             int actsize = m_Target()->m_Anim2d.GetNowAnimSum();
             switch(m_Target()->m_NowAct)
             {
             case AS_ATTACK:
             {
                 if(m_Target()->m_Anim2d.GetNowState() ==
                     ((actsize / 3) * 2))
                 {
                     Do("AS_DEFENCE");
                     To(m_Target()->GetScreenPos());
                     //m_WantToDo.GoBegin();
                 }
                 if(m_Target()->m_Anim2d.GetNowState() == actsize - 2)
                 {
                     Do("AS_HITBACK");
                     //To(m_Target()->GetScreenPos());
                     //m_WantToDo++;
                 }
                 break;
             }
             case AS_HITBACK:
             {
                 if(m_Target()->m_Anim2d.GetNowState() ==
                     ((actsize / 3) * 2))
                 {
                     Do("AS_DEFENCE");
                     To(m_Target()->GetScreenPos());
                     //m_Route.push_back(m_Target()->GetScreenPos());
                     //To(m_Target[0]->GetScreenPos());
                     //m_WantToDo.GoBegin();
                 }
                 break;
             }
             case AS_MAGIC:
             {
                 if(m_Target()->m_Anim2d.GetNowState() == actsize / 5 * 4)
                 {
                     Do("AS_DEFENCE");
                     To(m_Target()->GetScreenPos());
                     //m_WantToDo.GoBegin();
                 }
                 break;
             }
             case AS_MAGICATTACK:
             {
                 if(m_Target()->m_Anim2d.GetNowState() == actsize / 5 * 4)
                 {
                     Do("AS_DEFENCE");
                     To(GetPos());
                     //m_WantToDo.GoBegin();
                 }
                 break;
             }
             }
            */
            //}


            //=============================================================================
            // NAME: JudgCollision()
            // DESC: judgment which is collision
            //=============================================================================
        bool CollisionEntityCallBack( unsigned int order, void* pEntity, void* pData )
        {
            IEntity2d* entity0 = ( IEntity2d* )pEntity;
            IEntity2d* entity1 = ( IEntity2d* )pData;
            if( entity1->IsCollisionByBound( entity0->GetPos() ) )
            {
                entity1->m_Collision++;
            }
            return true;
        }

        int Role2d::JudgCollision( void )
        {
            m_Collision = 0;
            if( ( GetWorld() != 0 ) &&
                ( GetNowMotionStage() == _STAGE_MOVING ) )
            {
                GetWorld()->ForAllEntity( CollisionEntityCallBack, this );
            }
            return m_Collision;
        }



        //////////////////////////////////////////////////////////////////////////////////////////////
        //
        //
        //
        //=============================================================================
        // NAME: Search_Path()
        // DESC: If you need global map pos, convert grid(local) to map(local) and then add to globalMapRef
        //=============================================================================
        uint_t Role2d::GetWalkRange( void )
        {
            return m_SearchRange;
        }

        void Role2d::SetWalkRange( uint_t range )
        {
            m_SearchRange = range;
            //int r = (1 + (2 * range));
            //m_WeightMap.reallot(r, r);
            return;
        }


        int Role2d::Search_Path( const GameVertex& mapFrom, const GameVertex& mapTo )
        {

            Zone2d* residentZone = GetWorld()->GetZone( GetPos() );
            if( !residentZone || residentZone->IsBoardEmpty() )
            {
                //                return -1;                   
                                // 目前 non-Streaming 仍有問題, 唯一解決方法是沒資料就讓他無條件過去
                m_Route.clear();
                FindRoute( mapTo );
                //m_Route.push_back(mapTo);
                return 1;
            }

            //ClearRoute();
            //GameVertex globalMapRef = residentZone->GetPos();

            /*
            GameVertex gridFrom, gridTo;
            gridFrom.x = mapFrom.x - globalMapRef.x;
            gridFrom.y = mapFrom.y - globalMapRef.y;
            gridTo.x   = mapTo.x   - globalMapRef.x;
            gridTo.y   = mapTo.y   - globalMapRef.y;

            gridFrom = GetWorld()->ConvertMapToGrid(gridFrom);
            gridTo   = GetWorld()->ConvertMapToGrid(gridTo);
            */


            GameVertex gridFrom, gridTo;
            //gridFrom.x = mapFrom.x - globalMapRef.x;
            //gridFrom.y = mapFrom.y - globalMapRef.y;
            //gridTo.x   = mapTo.x   - globalMapRef.x;
            //gridTo.y   = mapTo.y   - globalMapRef.y;

            gridFrom = GetWorld()->ConvertMapToGrid( mapFrom );
            gridTo = GetWorld()->ConvertMapToGrid( mapTo );


            /*
            if(gridTo.x<0)
                gridTo.x=0;
            if(gridTo.y<0)
                gridTo.y=0;
                */

                //write weigh map
                //OnMakeDiffuse(globalMapRef, gridFrom, m_SearchRange);// search road can walk
            OnMakeDiffuse( gridFrom, m_SearchRange );// search road can walk
            std::vector<GameVertex> path;
            OnSearchPath( &path, gridFrom, gridTo );

            for( unsigned int i = 0; i < path.size(); i++ )
            {
                FindRoute( path[ i ] );
            }

            return 1;
        }

        //=============================================================================
        // NAME:
        // DESC: 
        //=============================================================================
        //void Role2d::OnMakeDiffuse(const GameVertex& globalMapRef, const GameVertex& RoleGridPos, uint_t range)// search road can walk
        void Role2d::OnMakeDiffuse( const GameVertex& RoleGridPos, uint_t range )// search road can walk
        {
            assert( range > 0 );
            if( GetWorld() == NULL )
                return;

            //            assert((GetWorld() != NULL) && "Role2d world was a NULL !!!");

                        // Allot the map of weight
            int cols = ( 1 + ( 2 * range ) );
            m_WeightMap.reallot( cols, cols );

            LG::Point n;
            // Search all his branchs
            for( int i = 0; i < cols; ++i )
            {
                for( int j = 0; j < cols; ++j )
                {
                    n.x = ( i - range ) + RoleGridPos.x;
                    n.y = ( j - range ) + RoleGridPos.y;

                    // convert weightMap back to chessbord grid
                    //if (GetWorld()->GetGridData(WALK_BOARD, 
                    //                              GetWorld()->ConvertGridToMap(n)+globalMapRef) == 0)


                    m_WeightMap( i, j ) = ( int )GetWorld()->GetGridData( WALK_BOARD,
                        GetWorld()->ConvertGridToMap( n ) );

                    //                    if ((int)GetWorld()->GetGridData(WALK_BOARD, 
                    //                                                       GetWorld()->ConvertGridToMap(n)) == 0)
                    //                    {
                    //                        m_WeightMap(i, j) = 10000;  // mark searched walkable
                    //                        continue;                     // block cell
                    //                    }
                    //            
                    //                    m_WeightMap(i, j) = 1;  // mark searched walkable
                }
            }

            //the first step of search
            g_Pathsearch.Init( m_WeightMap(), cols, cols );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        //
        //
        //

        int Role2d::OnSearchPath( std::vector<GameVertex>* path,
            const GameVertex& localGridFrom, const GameVertex& localGridTo )
        {
            // the first step of search
            // This Algorithm treat start as center grid, that is, left top is (0,0), center is (10,10) if it's 21x21
            int rng = ( m_SearchRange * 2 );
            int lenx = m_SearchRange + ( localGridTo.x - localGridFrom.x );
            if( lenx < 0 )
                lenx = 0;
            else if( lenx > rng )
                lenx = rng;

            int leny = m_SearchRange + ( localGridTo.y - localGridFrom.y );
            if( leny < 0 )
                leny = 0;
            else if( leny > rng )
                leny = rng;


            m_Steps = g_Pathsearch.SearchInMap( m_SearchRange, m_SearchRange,
                lenx, leny );


            Zone2d* residentZone = GetWorld()->GetZone( GetPos() );
            //GameVertex v;
            GameVertex mapGlobalTo;
            GameVertex gridStart = localGridFrom;
            for( unsigned int i = 0; i < m_Steps; ++i )
            {
                int direction = g_Pathsearch.PathDir[ i ];
                static int dirx[ 8 ] = { 0, 1, 1, 1, 0, -1, -1, -1 };
                static int diry[ 8 ] = { -1,-1, 0, 1, 1,  1,  0, -1 };

                if( direction < 8 )
                {
                    gridStart.x += dirx[ direction ];
                    gridStart.y += diry[ direction ];
                }
                else
                {
                    assert( ( direction >= 0 && direction < 8 ) &&
                        "Role2d::Go_To_NextGrid() -- invalid direction" );
                    continue;
                }

                GameVertex dest = GetWorld()->ConvertGridToMap( gridStart );
                path->push_back( GetWorld()->ConvertGridToMap( gridStart ) );
            }

            return m_Steps;
        }



        /*
        //=============================================================================
        // NAME: OnSearchPushResult()
        // DESC:
        //=============================================================================
        void Role2d::OnSearchPushResult(const std::vector<GameVertex>& path,
                                         const GameVertex& globalMapRef,
                                         const GameVertex& gridFrom)//,
                                         //const GameVertex& mapFrom)
        {
            //GameVertex mapGlobalFrom = mapFrom;
            GameVertex gridStart = gridFrom;
            GameVertex mapGlobalTo;

            for (unsigned int i = 0; i <m_Steps; ++i)
            {
                if (GetNextGridMapPos(g_Pathsearch.PathDir[i],
                                        globalMapRef,
                                        gridStart,
                                        mapGlobalTo))
                {
                    //SetPos(mapGlobalFrom);
                    FindRoute(mapGlobalTo);
                    //FindRoute(mapGlobalFrom, mapGlobalTo);
                    //mapGlobalFrom = mapGlobalTo;
                }
            }
        }
        */


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        bool Role2d::GetNextGridMapPos(int direction,
                                        const GameVertex& globalMapRef,
                                        GameVertex& gridStart,
                                        GameVertex& dest)

        {
            assert((direction>= 0 && direction <8) &&
                    "Role2d::Go_To_NextGrid() -- invalid direction");
            static dirx[8]={  0, 1, 1, 1, 0, -1, -1, -1 };
            static diry[8]={ -1,-1, 0, 1, 1,  1,  0, -1 };

            if(direction <8)
            {
                gridStart.x += dirx[direction];
                gridStart.y += diry[direction];
            }
            else
                return false;
            */
            /*
            switch (direction)
            {
              case 0:
                gridStart.y -= 1;
                break;
              case 1:
                gridStart.x += 1;
                gridStart.y -= 1;
                break;
              case 2:
                gridStart.x += 1;
                break;
              case 3:
                gridStart.x += 1;
                gridStart.y += 1;
                break;
              case 4:
                gridStart.y += 1;
                break;
              case 5:
                gridStart.x -= 1;
                gridStart.y += 1;
                break;
              case 6:
                gridStart.x -= 1;
                break;
              case 7:
                gridStart.x -= 1;
                gridStart.y -= 1;
                break;
              default:
                return false;
            }
            */
            //dest = GetWorld()->ConvertGridToMap(gridStart) + globalMapRef;
            //dest = GetWorld()->ConvertGridToMap(gridStart) + globalMapRef;
            //return true;
        //}


        //---------------------------------------------------------------------
        //似乎 最近拿的 擺再第一個 會比較容易命中

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        CPIcon* SearchAllImageChache( std::list<CPIcon*>& imagecache, uint_t id )
        {
            //uint_t i = 0;
            std::list<CPIcon*>::iterator iter = imagecache.begin();
            while( iter != imagecache.end() )
            {
                CPIcon* icon = ( *iter );
                if( icon == NULL )
                    return NULL;

                if( icon->Id == id )
                {
                    CPIcon* icon = ( *iter );
                    //tchar_t buff[50];
                    imagecache.erase( iter );
                    imagecache.push_front( icon );
                    //sprintf(buff, "count %d\n", i);
                    //OutputDebugString(buff);
                    return icon;
                }
                //i++;
                ++iter;
            }
            return NULL;
        }

        //=============================================================================
        // NAME:
        // DESC: to clear no avail image buffer
        //=============================================================================
        CPIcon* AdjustAllImageCache( std::list<CPIcon*>& imagecache )
        {
            CPIcon* buff = NULL;
            //if(imagecache.size()>= g_MaxImageSize)
            //{
            std::list<CPIcon*>::iterator iter = imagecache.end();
            --iter;
            //std::list<CPIcon*>::iterator iter = imagecache.begin();
            buff = ( *iter );
            //imagecache.erase(iter);
        //}
        //else
        //{
        //    buff = new CPIcon;
        //    imagecache.push_front(buff);
        //}

            return buff;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CreateAllImageCache( std::list<CPIcon*>& imagecache )
        {
            uint_t len = imagecache.size();
            for( uint_t i = len; i < g_MaxImageSize; i++ )
            {
                imagecache.push_back( new CPIcon );
            }
        }

        //=============================================================================
        // NAME:
        // DESC: to release all cache buffer
        //=============================================================================
        void ReleaseAllImageCache( std::list<CPIcon*>& imagecache )
        {
            std::list<CPIcon*>::iterator iter = imagecache.begin();
            while( iter != imagecache.end() )
            {
                SAFE_DELETE( ( *iter ) );
                iter++;
            }
            imagecache.clear();
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void ClearAllImageChache( std::list<CPIcon*>& imagecache )
        {
            std::list<CPIcon*>::iterator iter = imagecache.begin();
            while( iter != imagecache.end() )
            {
                ( *iter )->Clear();
                iter++;
            }
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        AvatarRole::AvatarRole()
        {
            int temp = 0;
            memcpy( &temp, "AVRO", sizeof( int ) );
            SetCId( temp );
            m_Children.clear();
            _CurrIcon = 0;
            m_pItemInfo = new DOLL::AvatarNode;
            m_ImageCache.clear();
            CreateAllImageCache( m_ImageCache );

        }

        AvatarRole::~AvatarRole()
        {
            Destroy();
        }


        //=============================================================================
        // NAME: 
        // DESC: 
        //=============================================================================
        void AvatarRole::Destroy( void )
        {
            if( m_Children.size() > 0 )
            {
                for( int i = 0; i < AvatarTable::g_Layers; ++i )
                {
                    if( m_Children[ i ] != 0 )
                    {
                        delete m_Children[ i ];
                        m_Children[ i ] = 0;
                    }
                }
            }
            m_Children.clear();
            SAFE_DELETE( m_pItemInfo );
            ReleaseAllImageCache( m_ImageCache );
            //m_ImageCache.clear();
            //SAFE_DELETE(_CurrIcon);
        }


        //=============================================================================
        // NAME: 
        // DESC: 
        //=============================================================================
        void AvatarRole::SetLayerNameMapping( int layer, const tchar_t* name )
        {
            tchar_t* buf = new tchar_t[ 32 ];
            strcpy( buf, name );

            if( AvatarTable::g_LayerNameMapping.size() != AvatarTable::g_Layers )
            {
                AvatarTable::g_LayerNameMapping.clear();
                for( int i = 0; i < AvatarTable::g_Layers; ++i )
                {
                    // 這是static 的變數
                    AvatarTable::g_LayerNameMapping.push_back( 0 );
                }
            }

            AvatarTable::g_LayerNameMapping[ layer ] = buf;
        }

        int AvatarRole::GetLayerNameMapping( const tchar_t* name )
        {
            for( int i = 0; i < AvatarTable::g_Layers; ++i )
                if( AvatarTable::g_LayerNameMapping[ i ] != 0 )
                    if( strcmp( name, AvatarTable::g_LayerNameMapping[ i ] ) == 0 )
                        return i;
            return -1;
        }

        const CPIcon* AvatarRole::Animation( int count )                  //Animation + Imageplaying image
        {
            /*
            const CPIcon* icon = GetImage(m_Anim2d.GetNowFrame());
            if(icon !=NULL )
                return icon;
            return BaseClass::Animation(count);
            */
            m_Anim2d.Update( count );
            return GetImage( m_Anim2d.GetNowFrame() );
        }

        const CPIcon* AvatarRole::GetImage( uint_t id )
        {
            _CurrIcon = SearchAllImageChache( m_ImageCache, id );
            if( _CurrIcon != NULL )
                return _CurrIcon;

            //tchar_t buff[255] = {"Create for new buffer!!!\n"};
            //OutputDebugString(buff);

            uint_t tick = 0;
            if( id == -1 )
                tick = m_Anim2d.GetNowFrame();
            else
                tick = id;

            if( GetLayers() != 0 )
            {
                //這是第一層對吧，以這個為主
                AvatarRole* avatar = GetSortedItem( 0 );
                assert( avatar != 0 && "Invalid Body Role value. See m_CharacterName!" );
                CPIcon* icon = ( CPIcon* )avatar->BaseClass::GetImage( tick );
                assert( icon != 0 && "Invalid Body Image value. See m_CharacterName!" );

                //取出無效的buffer
                _CurrIcon = AdjustAllImageCache( m_ImageCache );
                assert( _CurrIcon != 0 && "AvatarRole::Animation: _CurrIcon cant be zero" );

                _CurrIcon->Id = id;
                _CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );

                LG::Point pt = icon->Offset();
                _CurrIcon->SetOffsetX( pt.x );
                _CurrIcon->SetOffsetY( pt.y );

                LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );

                for( uint_t i = 1; i < GetLayers(); ++i )
                {
                    avatar = GetSortedItem( i );
                    if( avatar != NULL )
                    {
                        if( avatar->GetVisible() )
                        {
                            icon = ( CPIcon* )avatar->BaseClass::GetImage( tick );
                            if( icon != NULL )
                                LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );
                        }
                    }
                }
            }
            else
            {
                //這是第一層對吧，以這個為主
                CPIcon* icon = ( CPIcon* )BaseClass::GetImage( id );
                if( icon != NULL )
                {
                    _CurrIcon = AdjustAllImageCache( m_ImageCache );
                    _CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );
                    LG::Point pt = icon->Offset();
                    _CurrIcon->SetOffsetX( pt.x );
                    _CurrIcon->SetOffsetY( pt.y );
                    LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );
                    _CurrIcon->Id = id;
                }
                //return (CPIcon*)BaseClass::GetImage(id);
            }

            return _CurrIcon;
        }

        bool AvatarRole::IsCollisionByImage( GameVertex mpos )
        {
            if( ( GetLayers() != 0 ) && ( _CurrIcon != NULL ) )
            {
                return _CurrIcon->IsCollisionByImage( mpos + GSO::World::IEntity2d::GetRefOrigin(), GetPos().x, GetPos().y );
            }
            //return _CurrIcon->IsCollisionByImage(mpos);
            return BaseClass::IsCollisionByImage( mpos );
        }



        //=============================================================================
        // NAME: 
        // DESC: 強制建立一個畫布，如果已建立且大小相同，就不動
        //=============================================================================
        /*
        CPIcon* AvatarRole::CreateImage(unsigned int x, unsigned int y, unsigned int colorbit, uint_t id)
        {
            if(_CurrIcon == NULL)
            {
                _CurrIcon = new CPIcon;
                _CurrIcon->Allot(x, y, colorbit);
                _CurrIcon->Id = id;
            }
            //else if((_CurrIcon->GetClipWidth() != x) &&
            //       (_CurrIcon->GetClipHeight() != y) &&
            //         (_CurrIcon->GetDepth() != colorbit))
            //{
            //    _CurrIcon->Allot(x, y, colorbit);
                //_icon_height = y;
                //_icon_weight = x;
                //_icon_colot_bit = colorbit;
            //}

            _CurrIcon->Allot(x, y, colorbit);
            return _CurrIcon;
        }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        DOLL::AvatarNode* AvatarRole::GetAvatarTableItem(const tchar_t* filename, const tchar_t* group)
        {
            //return AvatarTable::g_Avatar.m_pItemList->GetItemInfo(filename, group);
            return NULL;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        DOLL::AvatarNode* AvatarRole::GetAvatarTableItem(int id)
        {
            //return AvatarTable::g_Avatar.m_pItemList->GetItemInfo(id);
            return NULL;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        std::string AvatarRole::GetAvatarTableName(const DOLL::AvatarNode* item)
        {
            return item->filename;
        }
        */


        //=============================================================================
        // NAME: 
        // DESC: 取得排序後的image
        //=============================================================================
        AvatarRole* AvatarRole::GetSortedItem( uint_t sequence )
        {
            //if(m_pResource2d == NULL)
                //return NULL;


            if( m_pItemInfo == NULL )
            {
                if( sequence == 0 )
                {
                    return this;
                }

                if( ( m_Children.size() > sequence ) && ( m_Children[ sequence ] != NULL ) )
                {
                    return m_Children[ sequence ];
                }

                return NULL;
            }
            if( sequence == 0 )
            {
                /// Avatar專用update
                if( GetLayers() == 0 )
                    return 0;

                DOLL::CSorting* sorting = AvatarTable::g_Avatar.m_pSort;

                AvatarTable::g_Avatar.m_pSort->BeginSort( m_Anim2d.GetNowAngle() / 45 );
                AvatarTable::g_Avatar.m_pSort->Item( m_pItemInfo );
                for( std::vector<AvatarRole*>::iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
                {
                    if( ( *iter ) != 0 )
                        AvatarTable::g_Avatar.m_pSort->Item( ( *iter )->m_pItemInfo );
                }
                AvatarTable::g_Avatar.m_pSort->EndSort();
            }

            // 如果是根
            if( m_pItemInfo->reserved == sequence )
            {
                return this;
            }

            // 其它的節點
            for( int i = 0; i < AvatarTable::g_Layers; ++i )
                if( ( m_Children.size() > 0 ) && ( m_Children[ i ] != 0 ) )
                    if( m_Children[ i ]->m_pItemInfo != 0 )
                        if( m_Children[ i ]->m_pItemInfo->reserved == sequence )
                            return m_Children[ i ];
            return NULL;
        }

        // by sean 
        //=============================================================================
        // NAME: 
        // DESC: Load Anm
        //=============================================================================
        error_t    AvatarRole::LoadAnm( const tchar_t* filename )
        {
            return m_Anim2d.Load( filename );
        }

        int AvatarRole::Load( const tchar_t* filename )
        {
            //ReleaseAllImageCache(m_ImageCache);
            //CreateAllImageCache(m_ImageCache);
            ClearAllImageChache( m_ImageCache );
            return BaseClass::LoadRes( filename );
        }

        // by sean 
        /*
        //=============================================================================
        // NAME:
        // DESC: 新的讀圖函式，讀多層圖用
        //=============================================================================
        int AvatarRole::Load(uint_t layer, const tchar_t* filename)
        {


            return result;
        }
        */


        // by sean
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        // Get the layer's mask, if the return value is true, it means "this layer
        // cant attach any item".
        bool AvatarRole::GetLayerMask( unsigned int layer )
        {
            long mask = 0x01 << layer;

            // for each layer
            for( unsigned int i = 0; i < m_Children.size(); ++i )
            {
                if( m_Children[ i ] != 0 )
                {
                    //if (m_Children[i]->GetItemInfo()->switcher  &mask)
                    //    return true;
                    return m_Children[ i ]->GetItemInfo()->switcher[ layer ];
                }
            }
            return false;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //============================================================================= 
        void AvatarRole::SetLayerVisible( uint_t layer, bool v )
        {
            if( m_Children[ layer ] )
            {
                if( m_Children[ layer ]->GetVisible() != v )
                {
                    // 要重新設定可不可見 所以得把BUFFER都先清除
                    ClearAllImageChache( m_ImageCache );
                    m_Children[ layer ]->SetVisible( v );
                }
            }
        }

        // by sean
        //=============================================================================
        // NAME: 
        // DESC: 裝備
        //=============================================================================
        int    AvatarRole::DressItem( uint_t layer, const tchar_t* filename, longlong_t itemid )
        {
            if( filename == NULL )
                return 0;

            ClearAllImageChache( m_ImageCache );

            if( m_Children.size() < AvatarTable::g_Layers )
            {
                for( int i = 0; i < AvatarTable::g_Layers; ++i )
                    m_Children.push_back( 0 );
            }


            if( layer == 0 )
            {
                m_pItemInfo->layer = layer;
                return Load( filename );
            }


            --layer;

            if( layer >= AvatarTable::g_Layers )
                return 0;

            if( m_Children[ layer ] == NULL )
                m_Children[ layer ] = new AvatarRole;

            DOLL::AvatarNode* avataritem = AvatarTable::g_AvatarMgr.GetItemInfo( itemid );
            if( avataritem )
            {
                memcpy( m_Children[ layer ]->m_pItemInfo, avataritem, sizeof( DOLL::AvatarNode ) );
            }

            m_Children[ layer ]->m_pItemInfo->layer = layer;
            int result = m_Children[ layer ]->Load( filename );

            if( result != 1 )
            {
                delete m_Children[ layer ];
                m_Children[ layer ] = NULL;
            }
            else
            {
                // 打開或是關閉相關的 互斥圖層
                for( uint_t i = 0; i < m_Children.size(); i++ )
                {
                    if( m_Children[ i ] )
                        m_Children[ i ]->SetVisible( m_Children[ layer ]->m_pItemInfo->switcher[ i ] );
                }
            }

            return result;
        }

        int    AvatarRole::DressItem( const tchar_t* layername, const tchar_t* filename, longlong_t itemid )
        {
            int layer = GetLayerNameMapping( layername );
            if( layer < 0 )
                return 0;
            return DressItem( layer, filename, itemid );
        }

        // by sean
        //=============================================================================
        // NAME: 
        // DESC: 卸下裝備
        //=============================================================================
        int    AvatarRole::UndressItem( uint_t layer )
        {
            ClearAllImageChache( m_ImageCache );
            if( ( layer < 0 ) || ( layer > GetLayers() ) )
                return 0;

            if( layer == 0 )
            {
                Clear();
                return 1;
            }

            --layer;

            if( m_Children[ layer ] == 0 )
                return 0;

            delete m_Children[ layer ];
            m_Children[ layer ] = 0;

            return 1;
        }

        int    AvatarRole::UndressItem( const tchar_t* layername )
        {
            int layer = GetLayerNameMapping( layername );
            if( layer <0 || layer > GetLayers() )
                return 0;
            return UndressItem( layer );
        }

        // by sean
        //=============================================================================
        // NAME: 
        // DESC: 換裝
        //=============================================================================
        int    AvatarRole::ChangeItem( uint_t layer, const tchar_t* filename, longlong_t itemid )
        {
            UndressItem( layer );
            return DressItem( layer, filename, itemid );
        }

        int    AvatarRole::ChangeItem( const tchar_t* layername, const tchar_t* filename, longlong_t itemid )
        {
            int layer = GetLayerNameMapping( layername );
            if( layer == -1 )
                return 0;
            return ChangeItem( layer, filename, itemid );
        }

        int    AvatarRole::LoadAvatarTable( const tchar_t*  ordertable, const tchar_t*  itemtable )
        {
            AvatarTable::g_Avatar.Init();

            if( itemtable )
                AvatarTable::g_AvatarMgr.Load( itemtable );

            if( ordertable )
                AvatarTable::g_Avatar.m_pSort->Load( ordertable );
            return 1;
            //return AvatarTable::g_Avatar.m_pItemList->Load(filename);
        }

        // by sean
        //=============================================================================
        // NAME: 
        // DESC: 
        //=============================================================================
        uint_t AvatarRole::GetLayers()
        {
            return m_Children.size();
        }


        //=============================================================================
        // NAME: 
        // DESC: 取得子節點
        //=============================================================================
        AvatarRole* AvatarRole::GetChildLayer( uint_t layer )
        {
            if( layer == 0 )
                return this;
            --layer;

            if( layer > AvatarTable::g_Layers )
                return NULL;

            return m_Children[ layer ];
        }





















        Avatar::Avatar()
        {
            int temp = 0;
            memcpy( &temp, "AVTR", sizeof( int ) );
            SetCId( temp );

            m_Children.clear();
            _CurrIcon = NULL;
            m_ImageCache.clear();
            CreateAllImageCache( m_ImageCache );
            m_Children.resize( 32 );

            for( unsigned i = 0; i < 32; ++i )
            {
                m_Children[ i ] = NULL;
            }
        }

        Avatar::~Avatar()
        {
            Destroy();
        }

        bool Avatar::IsCollisionByImage( GameVertex mpos )
        {
            if( ( m_Children.size() != 0 ) && ( _CurrIcon != NULL ) )
                return _CurrIcon->IsCollisionByImage( mpos - GetPos() );
            return BaseClass::IsCollisionByImage( mpos );
        }

        const CPIcon* Avatar::Animation( int count )                  //Animation + Imageplaying image
        {
            m_Anim2d.Update( count );
            return GetImage( m_Anim2d.GetNowFrame() );
        }
        const CPIcon* Avatar::GetImage( uint_t id )
        {
            _CurrIcon = SearchAllImageChache( m_ImageCache, id );
            if( _CurrIcon != NULL )
                return _CurrIcon;

            if( m_Children.size() != 0 )
            {
                //這是第一層對吧，以這個為主
                CPIcon *icon = ( CPIcon* )BaseClass::GetImage( id );
                if( icon != NULL )
                {

                    //                   assert(icon != NULL &&"Invalid Body value. See m_CharacterName!");

                                        //取出無效的buffer
                    _CurrIcon = AdjustAllImageCache( m_ImageCache );
                    assert( _CurrIcon != NULL && "AvatarRole::Animation: _CurrIcon cant be zero" );

                    _CurrIcon->Id = id;
                    _CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );
                    LG::Point pt = icon->Offset();
                    _CurrIcon->SetOffsetX( pt.x );
                    _CurrIcon->SetOffsetY( pt.y );
                    LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );

                    for( unsigned int i = 0; i < m_Children.size(); ++i )
                    {
                        if( m_Children[ i ] == NULL )
                            continue;

                        icon = ( CPIcon* )m_Children[ i ]->GetImage( id );

                        LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );
                    }
                }
            }
            else
            {
                //這是第一層對吧，以這個為主
                CPIcon* icon = ( CPIcon* )BaseClass::GetImage( id );
                if( icon != NULL )
                {
                    _CurrIcon = AdjustAllImageCache( m_ImageCache );
                    _CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );
                    LG::Point pt = icon->Offset();
                    _CurrIcon->SetOffsetX( pt.x );
                    _CurrIcon->SetOffsetY( pt.y );
                    LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );
                    _CurrIcon->Id = id;
                }
            }

            return _CurrIcon;
        }

        //        /// 摧毀
        void Avatar::Destroy()
        {
            if( m_Children.size() > 0 )
            {
                for( unsigned int i = 0; i < m_Children.size(); ++i )
                {
                    if( m_Children[ i ] != NULL )
                    {
                        delete m_Children[ i ];
                        m_Children[ i ] = NULL;
                    }
                }
            }
            m_Children.clear();
            ReleaseAllImageCache( m_ImageCache );
        }

        void Avatar::FreeLayer( unsigned int layer )
        {
            ClearAllImageChache( m_ImageCache );
            if( layer == 0 )
            {
                Clear();
                return;
            }

            --layer;
            if( m_Children[ layer ] == NULL )
                return;
            SAFE_DELETE( m_Children[ layer ] );
        }

        int    Avatar::Load( unsigned int layer, const tchar_t* filename )
        {
            ClearAllImageChache( m_ImageCache );
            if( layer == 0 )
            {
                return LoadRes( filename );
            }

            --layer;
            if( m_Children[ layer ] == NULL )
            {
                m_Children[ layer ] = new Element2d;
            }

            return m_Children[ layer ]->LoadRes( filename );
        }

        int    Avatar::Load( const tchar_t* filename )
        {
            ClearAllImageChache( m_ImageCache );
            return BaseClass::LoadRes( filename );
        }

        /// 載入動畫
        error_t    Avatar::LoadAnm( const tchar_t* filename )
        {
            return m_Anim2d.Load( filename );
        }



    };//World
};//GE




        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
              void Role2d::Do(int action, int times)
              {
                  for(int i = 0;i <times;i++)
                      m_WantToDo.push_back(action);
                m_WantToDo.GoBegin();
                  //m_WantActTimes = times;
                  //if(GetNowMotionStage()()> 2)
                  //  GetNowMotionStage() = 1;
                  return;
              }
        */

        /*
        void Role2d::Do(const tchar_t* anm, MOTIONSTATE state)
        {
            if((anm != NULL) && (strcmp(anm, "") != 0))
            {
                tchar_t buf[_MAX_LFNAME];
                mcurrentpath(buf, anm);
                switch(state)
                {
                case DO_REPLACE_MOTION://只做此動作
                    {
                        // 沒有待機動作，所以設此動做為待機動作
                        if(m_StandAnm.empty())
                        {
                            m_WantToDo.clear();
                            m_StandAnm = buf;
                            Element2d::LoadAnm(buf);
                            m_WantToDo.push_back(buf);    //擺一個動作進去 才會進入改變方向的階段
                            m_NowMotionStage=_STAGE_WATTING;//中斷動作
                        }
                        else
                        {
                            m_WantToDo.clear();
                            m_WantToDo.push_back(buf);
                            m_NowMotionStage=_STAGE_WATTING;//中斷動作
                        }
                        break;
                    }
                case DO_NOW_STAND://立刻改變動作
                    {
                        m_WantToDo.clear();
                        m_StandAnm = buf;
                        Element2d::LoadAnm(buf);
                        m_WantToDo.push_back(buf);    //擺一個動作進去 才會進入改變方向的階段
                        m_NowMotionStage=_STAGE_WATTING;//中斷動作
                        break;
                    }
                case DO_NEXT_STAND://下一個動作狀態
                    {
                        m_StandAnm = buf;
                        break;
                    }
                //case DO_ATTACH_MOTION:
                default://連續動作狀態
                    {
                        m_WantToDo.push_back(buf);
                        break;
                    }
                }
            }
            return;
        }
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Role2d::To(direction_t  dirt)
        {
            GameVertex pos=GetPos();

            if(BETWEEN(0, dirt, 90))
            {
                pos.x+=1;
                pos.y-=1;
            }
            else if(BETWEEN(90, dirt, 180))
            {
                pos.x+=1;
                pos.y+=1;
            }
            else if(BETWEEN(180, dirt, 270))
            {
                pos.x-=1;
                pos.y+=1;
            }
            else if(BETWEEN(180, dirt, 360))
            {
                pos.x-=1;
                pos.y-=1;
            }
            else if((dirt == 0) || (dirt==360))
            {
                pos.y-=1;
            }
            else if(dirt==90)
            {
                pos.x+=1;
            }
            else if(dirt==180)
            {
                pos.y+=1;
            }
            else if(dirt==270)
            {
                pos.x-=1;
            }
            To(pos);
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Role2d::To(GameVertex Mappos)
        {
            // if face to self situs
            //m_Route.push_back(GetPos());
            //GameVertex situs = GetWorld()->to_Screen_Site(Mappos);
            if (Mappos == GetPos())
                Mappos = to_my_self(m_Anim2d.GetNowAngle(), GetPos());

            ClearRoute();
            // because when get next route we will add 1 to be next postion, so
            // we must push two times
            PushRoute(Mappos);
            //m_Route.push_back(Mappos);//add goal to route
            //m_Route.push_back(Mappos);//add goal to route

            //if this role put in world, judg if to situs have some one there
            //            if((GetWorld() != NULL) && (!GetWorld()->IsBoardEmpty()))
            if ((GetWorld() != NULL))
            {
                Zone2d* residentZone = GetWorld()->GetZone(GetPos());
                if (residentZone && !residentZone->IsBoardEmpty())
                {
                    for (unsigned int i = 0; i <GetWorld()->GetEntityCount(); i++)
                    {
                        IEntity2d* target = (IEntity2d*) (*GetWorld())[i];
                        if (target->IsCollisionByBound(Mappos))
                        {
                            if (target != this)//that guy not myself
                                BuildInteraction(target); // i will do some thing to that guy
                            //else//that guy is myself
                                //Mappos = to_my_self(m_Anim2d.GetNowAngle(),
                                                     //GetPos());
                            break;
                        }
                    }



                }
            }
            //else
            //  m_Route.push_back(situs);

            //m_Step = 0;
            //m_Route.GoBegin();
            return;
        }



        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Role2d::GoTo(GameVertex MapSitus)
        {
            // compute work path
            // TODO: Alex, Why should I control the inner state of sprite outside sprite
            ClearRoute();
            m_WeightMap.zero_value();
            //m_Route.clear();
            //m_Step = 0;

            //            if((GetWorld() != 0) && (!GetWorld()->IsBoardEmpty()))

            if (GetWorld() != NULL)
            {
                Search_Path(GetPos(), MapSitus);
            }
            else// role not have world go line path
            {
                // TODO: Alex, This should not happen at this time.
                assert(0 && "Role2d::GoTo: Zone not exist!");
                FindRoute(MapSitus);
                //FindRoute(GetPos(), MapSitus);
            }

            if(m_Route.empty())
                To(MapSitus);
            else
                PushRoute(m_Route.back());
               //m_Route.push_back(m_Route.back());//讓最後一個點最少有兩個
        }

        */