//Role.cpp game world role class
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
//                                                                  Pin
//
#include "Role2d.h"
#include "Terrain2d.h"
#include <OBPathSearch.h>
#include <stack>
#include <fString.h>
#include <memio.inl>
#include <VMCDraw.h>
#include <assert.h>
#include "Doll.h"



namespace GSO
{
    namespace World
    {
		/// by sean 有多少圖層
		namespace AvatarTable
		{
			DOLL::CDoll           g_Avatar;
			unsigned short        g_Layers;      /// 有多少圖層
			std::vector<tchar_t *>   g_ItemMapping; /// 裝備對映表
			/// 這兩個有參數的GetItemInfo函式是用來取得doll裡的資料的，而不是本身的
			DOLL::SItem* GetAvatarTable(const tchar_t *filename, const tchar_t *group)
			{
				return g_Avatar.m_pItemList->GetItemInfo(filename, group);
			}

			DOLL::SItem* GetAvatarTable(int id)
			{
				return g_Avatar.m_pItemList->GetItemInfo(id);
			}
	    }


		pt::arraymap<int>     Role2d::m_WeightMap;  //加權值
		UINT                  g_MaxImageSize=32;
        OBPathSearch          g_Pathsearch;
        const int            _MAX_SERACH_RANGE = 45;

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        Role2d::Role2d() : m_Steps( 0 ), m_SearchRange(0)//, m_Type(kTypeNone)
        {
			int temp = 0;
            memcpy( &temp, "ROLE", sizeof( int ) );
			SetCId(temp);
			m_RegionId = 0;
			m_Collision=0;
			m_NowMotionStage = _STAGE_WATTING;
			m_StandAnm="";
            m_WantToDo.clear();
			SetWalkRange( _MAX_SERACH_RANGE );
			RenderLayer = LGB::DrawNode::kLayerRole;
			return;
        }


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        Role2d::~Role2d()
        {}

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		/*
		void Role2d::BlitImage(LGB::VMC *render, CPIcon* canvas )
		{
            if(!m_Transparent)
			{
				switch (RState)
				{
				case GSO::Sprite2d::kNormal:
					render->AlphaBlit(Animation(), LG::Point( GetScreenPos().x, GetScreenPos().y-GetScreenPos().z ), canvas);
					break;
				case GSO::Sprite2d::kAlpha:
					break;
				case GSO::Sprite2d::kLignten:
					render->Alpha16LightenBlit(Animation(), LG::Point( GetScreenPos().x, GetScreenPos().y-GetScreenPos().z ), canvas , 128);
					break;
				case GSO::Sprite2d::kDarken:
					render->Alpha16DarkenBlit(Animation(), LG::Point( GetScreenPos().x, GetScreenPos().y-GetScreenPos().z ), canvas );
					break;
				default:
					break;
				}
			}
			else // TODO: Alex, this should be transparent right now, we need more flags!
			{
				// Set true before calling AlphaBlit to let Animation function correctly
				m_Transparent = false;
				render->Alpha16DarkenBlit( Animation(), GetScreenPos(), canvas );
			}
		}
		*/


		// by sean 
        //=============================================================================
        //  Name : 
        //  Desc : 
        //=============================================================================
		/*
        void Role2d::Destroy(void)
        {
        }
		*/


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void Role2d::EndMotion( void )
        {
            //clear all releation
            /*
            if ( !m_Target.empty() )
            {
                UINT tars = 0;
                for ( unsigned int tarc = 0 ; tarc < m_Target.size() ; tarc++ )
                {
                    if ( ( m_Target[tarc]->GetNowMotionStage() ==
                           _STAGE_WATTING ) ||
                         ( m_Target[tarc]->GetNowMotionStage() ==
                           _STAGE_ARRIVE ) ||
                         ( m_Target[tarc]->GetNowMotionStage() ==
                           _STAGE_DO_FINISH ) )
                    {
                        tars++;
                    }
                }
                if ( tars == m_Target.size() ) //all role had finish their work then we can break all relative
                    BreakInteraction();
            }
            */


			//自殺
			/*
            if( m_StandAnm==NULL )
			{
			    //BreakInteraction();
                if ( GetWorld() != 0 )
				{
                    GetWorld()->ExitRole( this );
					//要不要delete自己勒?
				}
			}
			*/
			if( m_StandAnm.empty() )
			{
				Element2d::m_Anim2d.Release();
			}
			else
			{
				Element2d::LoadAnm( m_StandAnm.c_str() );
			}

            m_TargetSeq.clear();
            m_WantToDo.clear();
			ClearRoute();
            return;
        }

		//=============================================================================
		//  Name : 
		//  Desc :
		//=============================================================================
		const tchar_t*	Role2d::GetWantToDo( int i )
		{ 
			return ( i<m_WantToDo.size() ) ? NULL : m_WantToDo[i].c_str(); 
		}

		//=============================================================================
		//  Name : 
		//  Desc :
		//=============================================================================
		MOTIONSTAGE	Role2d::GetNowMotionStage( void )
		{
			return m_NowMotionStage; 
		}


 
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        /*
              void Role2d::Do( int action, int times )
              {
                  for( int i = 0;i < times;i++ )
                      m_WantToDo.push_back( action );
                m_WantToDo.GoBegin();
                  //m_WantActTimes = times;
                  //if(GetNowMotionStage()() > 2)
                  //  GetNowMotionStage() = 1;
                  return;
              }
        */

        void Role2d::Do( const tchar_t* anm, MOTIONSTATE state )
        {

			if( ( anm != NULL ) && (strcmp(anm, "") != 0) )
			{
				tchar_t buf[_MAX_FNAME];
				makecurrentpath( buf, anm );
				switch( state )
				{
				case DO_REPLACE_MOTION://只做此動作
					{
						m_WantToDo.clear();
						m_WantToDo.push_back( buf );
						m_WantToDo.GoBegin();
						m_NowMotionStage=_STAGE_WATTING;//中斷動作
						break;
					}
				case DO_NOW_STAND://立刻改變動作
					{
						m_WantToDo.clear();
						m_StandAnm = buf;
						Element2d::LoadAnm( buf );
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
						m_WantToDo.push_back( buf );
						//要將指標指向第一比動作
						if( m_WantToDo.size()==1 )
				            m_WantToDo.GoBegin();
						break;
					}
				}
			}
			else
			{				
				m_WantToDo.push_back( anm );
			}

            return;
        }
        
        //=============================================================================
        //  Name : to_my_self()
        //  Desc : compute my self face to
        //=============================================================================
        GameVertex to_my_self( int now_dir, GameVertex now_situs )
        {
            GameVertex facepos;
            switch ( now_dir )
            {
              case 0:
                facepos( now_situs.x, now_situs.y - 1 );     break;
              case 45:
                facepos( now_situs.x + 1, now_situs.y - 1 ); break;
              case 90:
                facepos( now_situs.x + 1, now_situs.y );     break;
              case 135:
                facepos( now_situs.x + 1, now_situs.y + 1 ); break;
              case 180:
                facepos( now_situs.x, now_situs.y + 1 );     break;
              case 225:
                facepos( now_situs.x - 1, now_situs.y + 1 ); break;
              case 270:
                facepos( now_situs.x - 1, now_situs.y );     break;
              case 315:
                facepos( now_situs.x - 1, now_situs.y - 1 ); break;
            }


            /*
                     switch( dir_count )
                     {
                     case 4:
                     {
                         switch( now_dir )
                         {
                         case 0:facepos( now_situs.x, now_situs.y + 1 );
                         case 1:facepos( now_situs.x - 1, now_situs.y );
                         case 2:facepos( now_situs.x, now_situs.y - 1 );
                         case 3:facepos( now_situs.x + 1, now_situs.y );
                         }
                     }
                     default:
                     {
                         switch( now_dir )
                         {
                         case 0:facepos( now_situs.x + 1, now_situs.y + 2 );
                         case 1:facepos( now_situs.x, now_situs.y + 1 );
                         case 2:facepos( now_situs.x - 1, now_situs.y + 1 );
                         case 3:facepos( now_situs.x, now_situs.y - 1 );
                         case 4:facepos( now_situs.x - 1, now_situs.y - 1 );
                         case 5:facepos( now_situs.x, now_situs.y - 1 );
                         case 6:facepos( now_situs.x + 1, now_situs.y - 1 );
                         case 7:facepos( now_situs.x + 1, now_situs.y );
                         }
                     }
                    }
                    */
            return facepos;
        }


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void Role2d::To( GameVertex Mappos )
        {
            // if face to self situs
            //m_Route.push_back( GetPos() );
            //GameVertex situs = GetWorld()->to_Screen_Site( Mappos );
            if ( Mappos == GetPos() )
                Mappos = to_my_self( m_Anim2d.GetNowAngle(), GetPos() );

			ClearRoute();
			// because when get next route we will add 1 to be next postion, so 
			// we must push two times
            m_Route.push_back( Mappos );//add goal to route
			//m_Route.push_back( Mappos );//add goal to route

            //if this role put in world, judg if to situs have some one there
            //            if( ( GetWorld() != NULL ) && ( !GetWorld()->IsBoardEmpty() ) )
            if ( ( GetWorld() != NULL ) )
            {
                Zone2d* residentZone = GetWorld()->GetZone(GetPos());
                if ( residentZone && !residentZone->IsBoardEmpty() )
                {
                    for ( unsigned int i = 0; i < GetWorld()->GetEntityCount(); i++ )
                    {
                        IEntity2d* target = ( IEntity2d* ) (*GetWorld())[ i ];
                        if ( target->IsCollisionByBound( Mappos ) )
                        {
                            if ( target != this )//that guy not myself
                                BuildInteraction( target ); // i will do some thing to that guy
                            //else//that guy is myself
                                //Mappos = to_my_self( m_Anim2d.GetNowAngle(),
                                                     //GetPos() );
                            break;
                        }
                    }


                    /*
                                    LG::Point GridPt = GetWorld()->Screen_to_Chessboard_Site(situs); //to judge where we want to do
                                    //if( m_WeightMap( GridPt.y, GridPt.x ) < 0 ) // is over our weight range
                                    //  return;
                                    if( ( GridPt.x > GetWorld()->Get_Cols() ) || ( GridPt.y > GetWorld()->Get_Rows() ) )// is over our weight range
                                        return;
                                    Role2d* target = (Role2d*)GetWorld()->Who_on_that_block( GridPt );
                                    if( target != 0 ) //have some guy on my goal
                                    {
                                        if( target != this )//that guy not myself
                                            Relative_Build_Target( target ); // i will do some thing to that guy
                                        else//that guy is myself
                                            situs = to_my_self( GetMaxAspect(), m_Anim_Table.m_Now_Aspect, GetPos());
                                        //Role2d* elm = (Role2d*)GetWorld()->m_MapUnit( GridPt.y, GridPt.x );
                                        //To(elm);
                                    }
                                   
                                    return;*/
                }
            }
            //else
            //  m_Route.push_back( situs );

            //m_Step = 0;
            //m_Route.GoBegin();
            return;
        }


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void Role2d::GoTo( GameVertex MapSitus )
        {
            // compute work path
			// TODO: Alex, Why should I control the inner state of sprite outside sprite
			ClearRoute();
            m_WeightMap.zero_value();
			//m_Route.clear();
			//m_Step = 0;

            //            if( ( GetWorld() != 0 ) && ( !GetWorld()->IsBoardEmpty() ) )

            if ( GetWorld() != NULL )
            {
				Search_Path( GetPos(), MapSitus );
            }
            else// role not have world go line path
            {
				// TODO: Alex, This should not happen at this time.
				assert(0 && "Role2d::GoTo: Zone not exist!");
				FindRoute( MapSitus );
                //FindRoute( GetPos(), MapSitus );
            }
			
			if( m_Route.empty() )
				To( MapSitus );
			else
			   m_Route.push_back( m_Route.back() );//讓最後一個點最少有兩個
        }
/*
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        bool Role2d::Go_To_NextGrid( int direction )
        {
            GameVertex dest( 0, 0 );
            if ( !GetNextGridMapPos( direction, GetPos(), dest ) )
            {
                return false;
            }

            m_Route.clear();
            FindRoute( GetPos(), dest );
            //          m_Route.GoBegin();
            return true;
        }
*/


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        int Role2d::Update(int count)
        {
            if ( !GetEnable() )
                return 0;

			Element2d::Update();

            //OnResponse();
            JudgStatus();  //set m_STAGE value;

			if ((!m_WantToDo.empty()) &&
				(m_WantToDo().empty()))
			{
				if ( GetWorld() != NULL )
			        GetWorld()->PushToRecycleBin( this );
				return 2;
			}

			JudgMotion();     //

			JudgCollision();
//			std::set< IEntity2d* >::iterator iter = m_TargetSeq.begin();
//			while (iter != m_TargetSeq.end())//        for( UINT i=0; i<m_TargetSeq.size(); i++ )
//			{
//                OnResponse((*iter));
//				++iter;
//			}
            return 1;
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void Role2d::Release()
		{
			BaseClass::Release();
	        m_Steps = 0;
            //m_obj = 0;    //使用道具
            m_Route.clear();
            m_TargetSeq.clear();
            m_WantToDo.clear();
		}


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
/*
		void Role2d::WashBrain()
		{
            Element2d::m_Anim2d.Load( m_StandAnm );
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
            
			if (m_ItemList.m_Anm.size() > 0)
			{
				elem.m_Anim2d.Update();
				UINT tick = elem.m_Anim2d.GetNowFrame();
				for (ListType::iterator it = list->begin(); it != list->end(); ++it)
					for (RUN_ANIM::iterator iter = m_ItemList.m_Anm.begin(); iter != m_ItemList.m_Anm.end(); ++iter)
					{
						if (strcmp((*it)->filename, (*iter)->item->filename) == 0)
						{
							(*iter)->elem.m_Anim2d.ChangeDirection(m_ItemList.m_NowDirect);
							m_pVPage->AlphaBlit((*iter)->elem.m_pResource2d->GetRes(tick), 
								LG::Point( 0, 0 ), VMCanvas );
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
                    m_obj->GetPos()( m_Route.back().x, m_Route.back().y + 1 );
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


        //=============================================================================
        ///
        /// 0 __1___2_3_4 action end
        ///         5_6_7 move   end
        /// do one action must have action and direction
        /// 0 wait for action and direction | 1 only have action or direction | 
        /// 2 begin action | 3 actioning | 4 end act   |
        /// 5 begin moving | 6 moving    | 7 end moving|
        /// judgment GetNowMotionStage()
		/// **if moving state Route size must > 1 for judg with doing state
        //=============================================================================
        int Role2d::JudgStatus( void )
        {
            //0
            if ( ( m_Route.empty() ) && ( m_WantToDo.empty() ) )
                return m_NowMotionStage = _STAGE_WATTING;


			switch( GetNowMotionStage() )
			{
              case _STAGE_BEGIN_MOVE:
				{
				    //6have dest, have action, situs between from and dest
                    //if( ( !m_Route.empty() ) && ( !m_WantToDo.empty() ) && ( GetPos() != m_Route.front() ) && ( GetPos() != m_Route.back() ) )//  (GetNowMotionStage() >= 2) && ( (m_WantToDo() == m_Anim_Table.m_NowAct) || (GetNowMotionStage() == 3) ) )
                    return m_NowMotionStage = _STAGE_MOVING;
                }
              case _STAGE_BEGIN_DO:
                {
                    //if( ( !m_Route.empty() ) && ( !m_WantToDo.empty() ) && ( m_Anim_Table.m_NowAct == m_WantToDo() ) )//  (GetNowMotionStage() >= 2) && ( (m_WantToDo() == m_Anim_Table.m_NowAct) || (GetNowMotionStage() == 3) ) )
                    return m_NowMotionStage = _STAGE_DOING;
                }

			  case _STAGE_MOVING:
                {
                    if ( m_Route.size()<=1 )
                        return m_NowMotionStage = _STAGE_ARRIVE;
                    return GetNowMotionStage();
                }
                //
              case _STAGE_DOING:
                {
                    if ( m_Anim2d.GetNowState() <= -1 )  //end to action
                        return m_NowMotionStage = _STAGE_DO_FINISH;
                    return GetNowMotionStage();
                }
			}
				
            //1
            if ( ( ( m_Route.empty() ) + ( m_WantToDo.empty() ) ) == 1 )
                return m_NowMotionStage = _STAGE_PREPARE;
            //2
            if ( ( !m_WantToDo.empty() ) && ( m_Route.size() == 1 ) )//( m_Anim_Table.m_NowAct != m_WantToDo.front() ) )//&& ( (GetNowMotionStage() < 2) || (GetNowMotionStage() == 4) ) )//begin to action
                return m_NowMotionStage = _STAGE_BEGIN_DO;
            //5 have dest, have action, situs in from
            if ( ( !m_WantToDo.empty() ) && ( m_Route.size() > 1 ) )//( GetPos() != m_Route.front() ) )//&& ( (GetNowMotionStage() < 2) || (GetNowMotionStage() == 4) ) )//begin to action
                return m_NowMotionStage = _STAGE_BEGIN_MOVE;
            return -1;
        }


        //=============================================================================
        //  Name : JudgAct()
        //  Desc : judgment which value to be set in when status 
        //=============================================================================
        void Role2d::JudgMotion( void )
        {
            switch ( GetNowMotionStage() )
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
                    if ( m_WantToDo.isBegin() )		//做完一次行為後，動作串列裏沒有下一個準備的行為
                        EndMotion();				//清除所有動作
                    //else                     //跳回狀態2，準備作動作串列裏的下一個的動作
                    //{
                    //m_Anim_Table.m_NowAct = -1;
                    //}
                    return;
                }
              case _STAGE_MOVING:
                {
                    if ( m_Anim2d.GetNowState() < -1 )  //end to action
                    {
					    m_WantToDo++;
						std::string str = m_WantToDo();
						if ( !str.empty() )
						{
							Element2d::LoadAnm( str.c_str() );
						}
                    }
					// 仍在移動中
                    if ( UpdateRoute() == 0 )
					{
						if (GetWorld() != NULL)
							FaceTo(GetPredictPosition());
						else
							FaceTo( GetNextRoute() );
					}
                    return;
                }
			  //do same thing
              case _STAGE_BEGIN_DO://2
              case _STAGE_BEGIN_MOVE://5
                {
					if( !m_WantToDo.empty() )
					{
						//m_WantToDo.GoBegin();
                        Element2d::LoadAnm( m_WantToDo().c_str() );
					}
                    FaceTo( GetNextRoute() );
                    return;
                }
            }

            return ;//m_Anim_Table.m_NowAct;
        }

        //=============================================================================
        //
        //
        //=============================================================================
		/*	
        void Role2d::OnResponse( Role2d* who )
        {
            if ( m_Target.empty() )
                return;
            /*
                     int actsize = m_Target()->m_Anim2d.GetNowAnimSum();
                     switch( m_Target()->m_NowAct )
                     {
                     case AS_ATTACK:
                     {
                         if( m_Target()->m_Anim2d.GetNowState() ==
                             ( ( actsize / 3 ) * 2 ) )
                         {
                             Do( "AS_DEFENCE" );
                             To( m_Target()->GetScreenPos() );
                             //m_WantToDo.GoBegin();
                         }
                         if( m_Target()->m_Anim2d.GetNowState() == actsize - 2 )
                         {
                             Do( "AS_HITBACK" );
                             //To(m_Target()->GetScreenPos() );
                             //m_WantToDo++;
                         }
                         break;
                     }
                     case AS_HITBACK:
                     {
                         if( m_Target()->m_Anim2d.GetNowState() ==
                             ( ( actsize / 3 ) * 2 ) )
                         {
                             Do( "AS_DEFENCE" );
                             To( m_Target()->GetScreenPos() );
                             //m_Route.push_back(m_Target()->GetScreenPos() );
                             //To(m_Target[0]->GetScreenPos());
                             //m_WantToDo.GoBegin();
                         }
                         break;
                     }
                     case AS_MAGIC:
                     {
                         if( m_Target()->m_Anim2d.GetNowState() == actsize / 5 * 4 )
                         {
                             Do( "AS_DEFENCE" );
                             To( m_Target()->GetScreenPos() );
                             //m_WantToDo.GoBegin();
                         }
                         break;
                     }
                     case AS_MAGICATTACK:
                     {
                         if( m_Target()->m_Anim2d.GetNowState() == actsize / 5 * 4 )
                         {
                             Do( "AS_DEFENCE" );
                             To( GetPos() );
                             //m_WantToDo.GoBegin();
                         }
                         break;
                     }
                     }
                    */
        //}


        //=============================================================================
        //  Name : JudgCollision()
        //  Desc : judgment which is collision
        //=============================================================================
		BOOL CollisionEntityCallBack( unsigned int order, void* pEntity, void* pData )
		{
			IEntity2d* entity0 = (IEntity2d*)pEntity;
			IEntity2d* entity1 = (IEntity2d*)pData;
            if ( entity1->IsCollisionByBound( entity0->GetPos() ) )
            {
                 entity1->m_Collision++;
            }
			return true;
		}

        int Role2d::JudgCollision( void )
        {
            m_Collision = 0;
            if ( ( GetWorld() != 0 ) &&
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
        //  Name : Search_Path()
        //  Desc : If you need global map pos, convert grid(local) to map(local) and then add to globalMapRef
        //=============================================================================
		UINT Role2d::GetWalkRange( void )
		{
			return m_SearchRange;
		}

		void Role2d::SetWalkRange( UINT range )
		{
			m_SearchRange = range;
			//int r = ( 1 + ( 2 * range ) );
			//m_WeightMap.reallot( r, r );
			return;
		}

		
        int Role2d::Search_Path( const GameVertex& mapFrom, const GameVertex& mapTo )
        {

			Zone2d* residentZone = GetWorld()->GetZone(GetPos());
            if ( !residentZone || residentZone->IsBoardEmpty() )
            {
//				return -1;                   
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
			
			gridFrom = GetWorld()->ConvertMapToGrid(mapFrom);
			gridTo   = GetWorld()->ConvertMapToGrid(mapTo);


			/*
			if( gridTo.x<0 )
				gridTo.x=0;
			if( gridTo.y<0 )
				gridTo.y=0;
				*/

			//write weigh map
			//OnMakeDiffuse( globalMapRef, gridFrom, m_SearchRange );// search road can walk
			OnMakeDiffuse( gridFrom, m_SearchRange );// search road can walk
			std::vector<GameVertex> path;
			OnSearchPath( &path, gridFrom, gridTo );
			//OnSearchPushResult( path, globalMapRef, gridFrom, mapFrom );

            for ( unsigned int i=0; i<path.size(); i++ )
            {
                FindRoute( path[i] );
            }

			return 1;
        }

		//=============================================================================
        //  Name :
        //  Desc : 
        //=============================================================================
        //void Role2d::OnMakeDiffuse( const GameVertex& globalMapRef, const GameVertex& RoleGridPos, UINT range )// search road can walk
		void Role2d::OnMakeDiffuse( const GameVertex& RoleGridPos, UINT range )// search road can walk
        {
            //assert( range > 0 );
			if (GetWorld() == NULL)
				return;

//			assert( ( GetWorld() != NULL ) && "Role2d world was a NULL !!!" );

            // Allot the map of weight
            int cols = ( 1 + ( 2 * range ) );
            m_WeightMap.reallot( cols, cols );

			LG::Point n;
            // Search all his branchs
            for ( int i = 0; i < cols; ++ i )
            {
                for ( int j = 0; j < cols; ++j )
                {
                    n.x = ( i - range ) + RoleGridPos.x;
					n.y = ( j - range ) + RoleGridPos.y;

                    // convert weightMap back to chessbord grid
                    //if ( GetWorld()->GetGridData( WALK_BOARD, 
					//	                          GetWorld()->ConvertGridToMap(n)+globalMapRef ) == 0 )


					m_WeightMap( i, j ) = (int)GetWorld()->GetGridData( WALK_BOARD,
				    	                                                GetWorld()->ConvertGridToMap(n) );

//					if ( (int)GetWorld()->GetGridData( WALK_BOARD, 
//						                               GetWorld()->ConvertGridToMap(n) ) == 0 )
//					{
//						m_WeightMap( i, j ) = 10000;  // mark searched walkable
//                        continue;                     // block cell
//					}
//			
//					m_WeightMap( i, j ) = 1;  // mark searched walkable
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
			int rng = (m_SearchRange*2);
			int lenx=m_SearchRange + (localGridTo.x - localGridFrom.x);
			if( lenx<0 )
				lenx=0;
			else if( lenx>rng)
				lenx=rng;

			int leny=m_SearchRange + (localGridTo.y - localGridFrom.y);
			if( leny<0 )
				leny=0;
			else if( leny>rng)
				leny=rng;


            m_Steps = g_Pathsearch.SearchInMap( m_SearchRange, m_SearchRange,
                                                lenx, leny);
			

			Zone2d* residentZone = GetWorld()->GetZone(GetPos());
			//GameVertex v;
			GameVertex mapGlobalTo;
			GameVertex gridStart = localGridFrom;
            for ( unsigned int i = 0; i<m_Steps; ++i )
            {
				int direction = g_Pathsearch.PathDir[i];
			    static int dirx[8]={  0, 1, 1, 1, 0, -1, -1, -1 };
			    static int diry[8]={ -1,-1, 0, 1, 1,  1,  0, -1 };

			    if( direction <8 )
				{
				    gridStart.x += dirx[direction];
				    gridStart.y += diry[direction];
				}
			    else
				{
			        assert( ( direction >= 0 && direction < 8 ) &&
                            "Role2d::Go_To_NextGrid() -- invalid direction" );
					continue;
				}

				GameVertex dest =GetWorld()->ConvertGridToMap( gridStart ); 
				path->push_back( GetWorld()->ConvertGridToMap( gridStart ) );
            }

			return m_Steps;
        }



		/*
        //=============================================================================
        //  Name : OnSearchPushResult()
        //  Desc : 
        //=============================================================================
        void Role2d::OnSearchPushResult( const std::vector<GameVertex>& path,
			                             const GameVertex& globalMapRef,
										 const GameVertex& gridFrom )//,
										 //const GameVertex& mapFrom )
        {
			//GameVertex mapGlobalFrom = mapFrom;
			GameVertex gridStart = gridFrom;
            GameVertex mapGlobalTo;

            for ( unsigned int i = 0; i < m_Steps; ++i )
            {
                if ( GetNextGridMapPos( g_Pathsearch.PathDir[i],
					                    globalMapRef,
										gridStart,
										mapGlobalTo ) )
                {
					//SetPos( mapGlobalFrom );
					FindRoute( mapGlobalTo );
                    //FindRoute( mapGlobalFrom, mapGlobalTo );
                    //mapGlobalFrom = mapGlobalTo;
                }
            }
        }
		*/


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		/*
        bool Role2d::GetNextGridMapPos( int direction,
			                            const GameVertex& globalMapRef,
			                            GameVertex& gridStart,
                                        GameVertex& dest)

        {
            assert( ( direction >= 0 && direction < 8 ) &&
                    "Role2d::Go_To_NextGrid() -- invalid direction" );
			static dirx[8]={  0, 1, 1, 1, 0, -1, -1, -1 };
			static diry[8]={ -1,-1, 0, 1, 1,  1,  0, -1 };

			if( direction <8 )
			{
				gridStart.x += dirx[direction];
				gridStart.y += diry[direction];
			}
			else
				return false;
			*/
			/*
            switch ( direction )
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
			//dest = GetWorld()->ConvertGridToMap( gridStart ) + globalMapRef;
			//dest = GetWorld()->ConvertGridToMap( gridStart ) + globalMapRef;
            //return true;
        //}


		//---------------------------------------------------------------------
		//似乎 最近拿的 擺再第一個 會比較容易命中

		//=============================================================================
		// Name :
		// Desc :
		//=============================================================================
		CPIcon* SearchAllImageChache( std::list< CPIcon* >& imagecache, UINT id )
		{
			//UINT i=0;
			std::list< CPIcon* >::iterator iter = imagecache.begin();
			while( iter != imagecache.end() )
			{
				CPIcon* icon = (*iter);
				if( icon == NULL )
					return NULL;

				if( icon->Id == id )
				{
					CPIcon* icon = (*iter);
					//tchar_t buff[50];
					imagecache.erase( iter );
					imagecache.push_front( icon );
					//sprintf( buff, "count %d\n", i );
					//OutputDebugString( buff );
					return icon;
				}
				//i++;
				++iter;
			}
			return NULL;
		}

		//=============================================================================
		// Name :
		// Desc : to clear no avail image buffer
		//=============================================================================
		CPIcon* AdjustAllImageCache( std::list< CPIcon* >& imagecache )
		{
			CPIcon* buff=NULL;
			//if( imagecache.size() >= g_MaxImageSize )
			//{
			    std::list< CPIcon* >::iterator iter = imagecache.end();
				--iter;
				//std::list< CPIcon* >::iterator iter = imagecache.begin();
				buff = (*iter);
			    //imagecache.erase( iter );
			//}
			//else
			//{
			//	buff = new CPIcon;
			//	imagecache.push_front( buff );
			//}
			
			return buff;
		}


		//=============================================================================
		// Name :
		// Desc :
		//=============================================================================
		void CreateAllImageCache( std::list< CPIcon* >& imagecache )
		{
			UINT len = imagecache.size();
			for( UINT i=len; i< g_MaxImageSize; i++ )
			{
				imagecache.push_back( new CPIcon );
			}
		}

		//=============================================================================
		// Name :
		// Desc : to release all cache buffer
		//=============================================================================
		void ReleaseAllImageCache( std::list< CPIcon* >& imagecache )
		{
			std::list< CPIcon* >::iterator iter = imagecache.begin();
			while( iter != imagecache.end() )
			{
				SAFE_DELETE( (*iter) );
				iter++;
			}
			imagecache.clear();
		}


		//=============================================================================
		// Name :
		// Desc :
		//=============================================================================
		void ClearAllImageChache( std::list< CPIcon* >& imagecache )
		{
			std::list< CPIcon* >::iterator iter = imagecache.begin();
			while( iter != imagecache.end() )
			{
				(*iter)->Release();
				iter++;
			}
		}


		//=============================================================================
		// Name :
		// Desc :
		//=============================================================================
		AvatarRole::AvatarRole()
		{
			int temp = 0;
            memcpy( &temp, "AVRO", sizeof( int ) );
			SetCId(temp);
            m_Children.clear();
			_CurrIcon = 0;
			m_pItemInfo = 0;
			m_ImageCache.clear();
			CreateAllImageCache( m_ImageCache );

		}

		AvatarRole::~AvatarRole()
		{
			Destroy();
		}

        //=============================================================================
        //  Name : 
        //  Desc : 
        //=============================================================================
        int AvatarRole::Init(UINT layers)
        {
			AvatarTable::g_Layers = layers;

			for (int i = 0; i < AvatarTable::g_Layers; ++i)
			{
				// 這是static 的變數
				AvatarTable::g_ItemMapping.push_back(0);
			}

            return 1;
        }

        //=============================================================================
        //  Name : 
        //  Desc : 
        //=============================================================================
        void AvatarRole::Destroy(void)
        {
			if (m_Children.size() > 0)
			{
				for (int i = 0; i < AvatarTable::g_Layers; ++i)
				{
					if (m_Children[i] != 0)
					{
						delete m_Children[i];
						m_Children[i] = 0;
					}
				}
			}
			m_Children.clear();
			m_pItemInfo = 0;
		    ReleaseAllImageCache( m_ImageCache );
			//m_ImageCache.clear();
			//SAFE_DELETE( _CurrIcon );
        }


        //=============================================================================
        //  Name : 
        //  Desc : 
        //=============================================================================
        void AvatarRole::SetItemMapping(int layer, const tchar_t* name)
		{
			tchar_t *buf = new tchar_t[32];
			strcpy(buf, name);
			AvatarTable::g_ItemMapping[layer] = buf;
		}

		int AvatarRole::GetItemMapping(const tchar_t* name)
		{
			for (int i = 0; i < AvatarTable::g_Layers; ++i)
				if (AvatarTable::g_ItemMapping[i] != 0)
					if (strcmp(name, AvatarTable::g_ItemMapping[i]) == 0)
						return i;
			return -1;
		}
				
		const CPIcon* AvatarRole::Animation( int count )                  //Animation + Imageplaying image
		{
			/*
			const CPIcon* icon = GetImage( m_Anim2d.GetNowFrame() );
			if( icon!=NULL )
				return icon;
			return BaseClass::Animation(count);
			*/
			m_Anim2d.Update(count);
			return GetImage( m_Anim2d.GetNowFrame() );
		}

		const CPIcon* AvatarRole::GetImage( UINT id )
		{
			_CurrIcon=SearchAllImageChache( m_ImageCache, id );
			if( _CurrIcon != NULL )
			    return _CurrIcon;

			//tchar_t buff[255] = {"Create for new buffer!!!\n"};
			//OutputDebugString( buff );

			if (GetLayers() != 0)
			{
				//這是第一層對吧，以這個為主
				CPIcon *icon = GetSortedImage(0, id );
				assert(icon != 0 && "Invalid Body value. See m_CharacterName!");

				//取出無效的buffer
				_CurrIcon=AdjustAllImageCache( m_ImageCache );
				assert(_CurrIcon != 0 && "AvatarRole::Animation: _CurrIcon cant be zero");

				_CurrIcon->Id = id;
			    _CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );
				
				LG::Point pt = icon->Offset();
				_CurrIcon->SetOffsetX(pt.x);
				_CurrIcon->SetOffsetY(pt.y);

				LGB::VMCDraw::Iconcpy(icon, pt, _CurrIcon);

				for (int i = 1; i < GetLayers(); ++i)
				{
					icon = GetSortedImage(i, id );
					if (icon == NULL)
						return _CurrIcon;
					//pt = icon->Offset();
					LGB::VMCDraw::Iconcpy(icon, pt, _CurrIcon);
				}
			}
			else
			{
				//這是第一層對吧，以這個為主
				CPIcon* icon = (CPIcon*)BaseClass::GetImage( id );
				if( icon!=NULL )
				{
				    _CurrIcon=AdjustAllImageCache( m_ImageCache );
            		_CurrIcon->Allot( icon->GetWidth(), icon->GetHeight(), icon->GetDepth() );
				    LG::Point pt = icon->Offset();
				    _CurrIcon->SetOffsetX(pt.x);
				    _CurrIcon->SetOffsetY(pt.y);
			        LGB::VMCDraw::Iconcpy( icon, pt, _CurrIcon );
					_CurrIcon->Id = id;
				}
				//return (CPIcon*)BaseClass::GetImage( id );
			}

			return _CurrIcon;
		}

		bool AvatarRole::IsCollisionByImage( GameVertex mpos )
		{
			if ((GetLayers() != 0) && (_CurrIcon != NULL))
				return _CurrIcon->IsCollisionByImage(mpos - GetPos());
//				return _CurrIcon->IsCollisionByBound(mpos, GetPos().x, GetPos().y);
			return BaseClass::IsCollisionByImage(mpos);
		}


		
        //=============================================================================
        //  Name : 
        //  Desc : 強制建立一個畫布，如果已建立且大小相同，就不動
        //=============================================================================
		/*
		CPIcon* AvatarRole::CreateImage(unsigned int x, unsigned int y, unsigned int colorbit, UINT id )
		{
			if(_CurrIcon == NULL)
			{
				_CurrIcon = new CPIcon;
				_CurrIcon->Allot(x, y, colorbit);
				_CurrIcon->Id = id;
			}
			//else if( ( _CurrIcon->GetClipWidth() != x ) &&
			//       ( _CurrIcon->GetClipHeight() != y ) &&
			//	     ( _CurrIcon->GetDepth() != colorbit ) )
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
		//  Name : 
		//  Desc :
		//=============================================================================
		DOLL::SItem* AvatarRole::GetAvatarTableItem(const tchar_t *filename, const tchar_t *group)
		{
			return AvatarTable::g_Avatar.m_pItemList->GetItemInfo(filename, group);
		}


		//=============================================================================
		//  Name : 
		//  Desc :
		//=============================================================================
		DOLL::SItem* AvatarRole::GetAvatarTableItem(int id)
		{
			return AvatarTable::g_Avatar.m_pItemList->GetItemInfo(id);
		}


		//=============================================================================
		//  Name : 
		//  Desc :
		//=============================================================================
		std::string AvatarRole::GetAvatarTableName(const DOLL::SItem* item)
		{
			return item->filename;
		}


        //=============================================================================
        //  Name : 
        //  Desc : 取得排序後的image
        //=============================================================================
		CPIcon* AvatarRole::GetSortedImage(UINT sequence, int _tick)
		{
			//if( m_pResource2d == NULL )
				//return NULL;
			
			UINT tick = 0;
			if (_tick == -1)
				tick = m_Anim2d.GetNowFrame();
			else
				tick = _tick;

			if (m_pItemInfo == NULL)
			{
				if (sequence == 0)
				{
					return (CPIcon*)BaseClass::GetImage(tick);
				}

				if ((m_Children.size() > sequence) && (m_Children[sequence] != NULL))
					return (CPIcon*)m_Children[sequence]->BaseClass::GetImage(tick);

				return NULL;
			}
			if (sequence == 0)
			{
				/// Avatar專用update
				if (GetLayers() == 0)
					return 0;

				DOLL::CSorting* sorting = AvatarTable::g_Avatar.m_pSort;

				AvatarTable::g_Avatar.m_pSort->BeginSort(m_Anim2d.GetNowAngle() / 45);
				AvatarTable::g_Avatar.m_pSort->Item(m_pItemInfo);
				for (std::vector<AvatarRole*>::iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter)
				{
					if ((*iter) != 0)
						AvatarTable::g_Avatar.m_pSort->Item((*iter)->m_pItemInfo);
				}
				AvatarTable::g_Avatar.m_pSort->EndSort();
			}
	
			// 如果是根
			if (m_pItemInfo->reserved == sequence)
			{
				return (CPIcon*)BaseClass::GetImage(tick);
			}

			// 其它的節點
			for (int i = 0; i < AvatarTable::g_Layers; ++i)
				if ((m_Children.size() > 0) && (m_Children[i] != 0))
					if (m_Children[i]->m_pItemInfo != 0)
						if (m_Children[i]->m_pItemInfo->reserved == sequence)
							return (CPIcon*)m_Children[i]->BaseClass::GetImage( tick );
			return NULL;
		}

		// by sean 
        //=============================================================================
        //  Name : 
        //  Desc : Load Anm
        //=============================================================================
		error_t	AvatarRole::LoadAnm(const tchar_t *filename)
		{
			return m_Anim2d.Load(filename);
		}

		int AvatarRole::Load( const tchar_t* filename )
		{
			//ReleaseAllImageCache( m_ImageCache );
			//CreateAllImageCache( m_ImageCache );
			ClearAllImageChache( m_ImageCache );
			return BaseClass::LoadRes(filename);
		}

		// by sean 
        //=============================================================================
        //  Name : 
        //  Desc : 新的讀圖函式，讀多層圖用
        //=============================================================================
        int AvatarRole::Load(UINT layer, DOLL::SItem* item, const tchar_t* filename)
        {
			--layer;

			if (layer >= AvatarTable::g_Layers)
				return 0;

			if (m_Children[layer] == NULL)
				m_Children[layer] = new AvatarRole;

			int result = m_Children[layer]->Load(filename);

			if (result == 1)
				m_Children[layer]->m_pItemInfo = item;
			else
			{
				delete m_Children[layer];
				m_Children[layer] = NULL;
			}
            return result;
        }


		// by sean
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		int AvatarRole::DetachChildren(UINT layer)
		{
			--layer;

			if (m_Children[layer] == 0)
				return 0;	

			delete m_Children[layer];
			m_Children[layer] = 0;
			
			return 1;
		}

		// Get the layer's mask, if the return value is true, it means "this layer
		// cant attach any item".
		bool AvatarRole::GetLayerMask(int layer)
		{
			long mask = 0x01 << layer;

			// for each layer
			for (unsigned int i = 0; i < m_Children.size(); ++i)
				if (m_Children[i] != 0)
					if (m_Children[i]->GetItemInfo()->switcher & mask)
						return true;
			return false;
		}
		// by sean
        //=============================================================================
        //  Name : 
        //  Desc : 裝備
        //=============================================================================
		int	AvatarRole::AttachItem(UINT layer, DOLL::SItem* item, const tchar_t* filename)
		{
			ClearAllImageChache( m_ImageCache );

			if (m_Children.size() < AvatarTable::g_Layers)
				for (int i = 0; i < AvatarTable::g_Layers; ++i)
					m_Children.push_back(0);

			// return if the layer is masked 
//			if (GetLayerMask(layer))
//				return 0;

			if (layer == 0)
			{
				m_pItemInfo = item;
				return Load(filename);
			}
			
			return Load(layer, item, filename);
		}

		int	AvatarRole::AttachItem(const tchar_t *layername, DOLL::SItem* item, const tchar_t* filename)
		{
			int layer = GetItemMapping(layername);
			if (layer < 0)
				return 0;
			return AttachItem(layer, item, filename);
		}
			
		// by sean
        //=============================================================================
        //  Name : 
        //  Desc : 卸下裝備
        //=============================================================================
		int	AvatarRole::DetachItem(UINT layer)
		{
			ClearAllImageChache( m_ImageCache );
			if ((layer < 0) || (layer > GetLayers()))
				return 0;

		    if (layer == 0)
			{
				Release();
				return 1;
			}

			return DetachChildren(layer);
		}

		int	AvatarRole::DetachItem(const tchar_t *layername)
		{
			int layer = GetItemMapping(layername);
			if (layer < 0 || layer > GetLayers())
				return 0;
			return DetachItem(layer);
		}

		// by sean
        //=============================================================================
        //  Name : 
        //  Desc : 換裝
        //=============================================================================
		int	AvatarRole::ChangeItem(UINT layer, DOLL::SItem* item, const tchar_t* filename)
		{
			DetachItem(layer);
			return AttachItem(layer, item, filename);
		}

		int	AvatarRole::ChangeItem(const tchar_t *layername, DOLL::SItem* item, const tchar_t* filename)
		{
			int layer = GetItemMapping(layername);
			if (layer == -1)
				return 0;
			DetachItem(layer);
			return ChangeItem(layer, item, filename);
		}

		int	AvatarRole::LoadAvatarInfo(const tchar_t * filename)
		{
				AvatarTable::g_Avatar.Init();

				tchar_t buf[300];
				sprintf(buf, "%s.sort", filename);
				AvatarTable::g_Avatar.m_pSort->Load(buf);
				return AvatarTable::g_Avatar.m_pItemList->Load(filename);
		}

		// by sean
        //=============================================================================
        //  Name : 
        //  Desc : 
        //=============================================================================
		int AvatarRole::GetLayers()
		{
			return m_Children.size();
		}


        //=============================================================================
        //  Name : 
        //  Desc : 取得子節點
        //=============================================================================
		AvatarRole* AvatarRole::GetChild(UINT layer)
		{
			if (layer == 0)
				return this;
			--layer;

			if (layer > AvatarTable::g_Layers)
				return NULL;

			return m_Children[layer];
		}

		



















		Avatar::Avatar()
		{
			int temp = 0;
            memcpy(&temp, "AVTR", sizeof(int));
			SetCId(temp);

            m_Children.clear();
			_CurrIcon = NULL;
			m_ImageCache.clear();
			CreateAllImageCache(m_ImageCache);
			m_Children.resize(32);

			for (unsigned i = 0; i < 32; ++i)
			{
				m_Children[i] = NULL;
			}
		}

		Avatar::~Avatar()
		{
			Destroy();
		}

		bool Avatar::IsCollisionByImage(GameVertex mpos)
		{
			if ((m_Children.size() != 0) && (_CurrIcon != NULL))
				return _CurrIcon->IsCollisionByImage(mpos - GetPos());
			return BaseClass::IsCollisionByImage(mpos);
		}

		const CPIcon* Avatar::Animation(int count)                  //Animation + Imageplaying image
		{
			m_Anim2d.Update(count);
			return GetImage(m_Anim2d.GetNowFrame());
		}
		const CPIcon* Avatar::GetImage(UINT id)
		{
			_CurrIcon = SearchAllImageChache(m_ImageCache, id);
			if (_CurrIcon != NULL)
			    return _CurrIcon;

			if (m_Children.size() != 0)
			{
				//這是第一層對吧，以這個為主
				CPIcon *icon = (CPIcon*)BaseClass::GetImage(id);
				if( icon!=NULL )
				{

//   				assert(icon != NULL && "Invalid Body value. See m_CharacterName!");

					//取出無效的buffer
					_CurrIcon = AdjustAllImageCache(m_ImageCache);
					assert(_CurrIcon != NULL && "AvatarRole::Animation: _CurrIcon cant be zero");

					_CurrIcon->Id = id;
					_CurrIcon->Allot(icon->GetWidth(), icon->GetHeight(), icon->GetDepth());
					
					LG::Point pt = icon->Offset();
					_CurrIcon->SetOffsetX(pt.x);
					_CurrIcon->SetOffsetY(pt.y);

					LGB::VMCDraw::Iconcpy(icon, pt, _CurrIcon);

					for (unsigned int i = 0; i < m_Children.size(); ++i)
					{
						if (m_Children[i] == NULL)
							continue;

						icon = (CPIcon*)m_Children[i]->GetImage(id);

						LGB::VMCDraw::Iconcpy(icon, pt, _CurrIcon);
					}
				}
			}
			else
			{
				//這是第一層對吧，以這個為主
				CPIcon* icon = (CPIcon*)BaseClass::GetImage(id);
				if (icon != NULL)
				{
				    _CurrIcon = AdjustAllImageCache(m_ImageCache);
            		_CurrIcon->Allot(icon->GetWidth(), icon->GetHeight(), icon->GetDepth());
				    LG::Point pt = icon->Offset();
				    _CurrIcon->SetOffsetX(pt.x);
				    _CurrIcon->SetOffsetY(pt.y);
			        LGB::VMCDraw::Iconcpy(icon, pt, _CurrIcon);
					_CurrIcon->Id = id;
				}
			}

			return _CurrIcon;
		}

//		/// 摧毀
		void Avatar::Destroy()
		{
			if (m_Children.size() > 0)
			{
				for (unsigned int i = 0; i < m_Children.size(); ++i)
				{
					if (m_Children[i] != NULL)
					{
						delete m_Children[i];
						m_Children[i] = NULL;
					}
				}
			}
			m_Children.clear();
		    ReleaseAllImageCache(m_ImageCache);
		}

		void Avatar::FreeLayer(unsigned int layer)
		{
			ClearAllImageChache(m_ImageCache);
			if (layer == 0)
			{
				Release();
				return;
			}
			
			--layer;
			if (m_Children[layer] == NULL)
				return;
			SAFE_DELETE(m_Children[layer]);
		}

		int	Avatar::Load(unsigned int layer, const tchar_t* filename)
		{
			ClearAllImageChache(m_ImageCache);
			if (layer == 0)
			{
				return LoadRes(filename);
			}

			--layer;
			if (m_Children[layer] == NULL)
			{
				m_Children[layer] = new Element2d;
			}

			return m_Children[layer]->LoadRes(filename);
		}

        int	Avatar::Load(const tchar_t* filename)
		{
			ClearAllImageChache(m_ImageCache);
			return BaseClass::LoadRes(filename);
		}

		/// 載入動畫
		error_t	Avatar::LoadAnm(const tchar_t *filename)
		{
			return m_Anim2d.Load(filename);
		}



    };//World
};//GE

