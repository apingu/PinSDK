//Scene_Editer.cpp  scene edit class
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
//                                                                  Pin
//
//#include "../tools/stdafx.h"
#include "App2dFactory.h"
#include "VMCRender2d.h"

#include <assert.h>


PfTimer map_scroll_clock( 200 );
int _MAP_SCROLL_STEP_  = 0;
UINT g_ScrollSpeed = 1;

namespace App2dFactory
{
    Terrain2d::Terrain2d()
    {
        m_Rdir = 0;
        m_Askfileoverwrite = false;
        m_Save = true;
    }
    
    Terrain2d::~Terrain2d()
    {
        m_Observers.clear();
    }
    
    
    void Terrain2d::SetRotateDir( int dir )
    {
        m_RotateDir.push_back( dir );
    }
    
    void Terrain2d::GetRotateDir( std::vector<int>& dirlist )
    {
        for ( UINT i = 0; i < m_RotateDir.size(); i++ )
        {
            dirlist.push_back( m_RotateDir[i] );
        }
    }
    
    void CALLBACK Terrain2d::Scroll_Query( POINT point, UINT uMsg, UINT wParam )
    {
        // TODO: Alex, bad
        static int preDefAngle[3] = {0, 45, 315};
        //	if((m_pRender == NULL) ||
        //		(m_pRender->GetWidget() == 0) ||
        //		(m_pRender->GetWidget() != GetFocus()))
        /*
            if((m_pRenderer == NULL) ||
        	    (m_pRenderer->GetRender()->GetWidget() == 0) ||
        	    (m_pRenderer->GetRender()->GetWidget() != GetFocus()))
        	    return;
        */
        GameVertex pos = point;
        
        switch ( uMsg )
        {
        case WM_MOUSEMOVE:
            //==deviec situs=================
            m_Mouse_Map_Site  = ConvertScreenToMap( pos ); //ScreenToMapSite(point);
            //格子座標
            m_Mouse_Chessboard_Site = ConvertMapToGrid( m_Mouse_Map_Site ); //MapToChessboardSite(m_Mouse_Map_Site);
            //===============================
            break;
            
        case WM_KEYDOWN:
            if ( g_ScrollSpeed < 50 )
            { g_ScrollSpeed++; }
            
            switch ( wParam )
            {
            case VK_LEFT:  //65: A --> Left
                if ( ::GetKeyState( VK_LCONTROL ) < 0 )
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.x = GSO::World::IEntity2d::GetRefOrigin().x + GetTileWidth();
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                }
                else
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.x = GSO::World::IEntity2d::GetRefOrigin().x + g_ScrollSpeed;
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().x = GSO::World::IEntity2d::GetRefOrigin().x + g_ScrollSpeed;
                }
                
                break;
                
            case VK_UP:	//87: W --> Up
                if ( ::GetKeyState( VK_LCONTROL ) < 0 )
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.y = GSO::World::IEntity2d::GetRefOrigin().y + GetTileHeight();
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().y = GSO::World::IEntity2d::GetRefOrigin().y + GetTileHeight();
                }
                else
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.y = GSO::World::IEntity2d::GetRefOrigin().y + g_ScrollSpeed;
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().y = GSO::World::IEntity2d::GetRefOrigin().y + g_ScrollSpeed;
                }
                
                break;
                
            case VK_RIGHT:	//68: D --> Right
                if ( ::GetKeyState( VK_LCONTROL ) < 0 )
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.x = GSO::World::IEntity2d::GetRefOrigin().x - GetTileWidth();
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().x = GSO::World::IEntity2d::GetRefOrigin().x - GetTileWidth();
                }
                else
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.x = GSO::World::IEntity2d::GetRefOrigin().x - g_ScrollSpeed;
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().x = GSO::World::IEntity2d::GetRefOrigin().x - g_ScrollSpeed;
                }
                
                break;
                
            case VK_DOWN:	//83: S --> Down
                if ( ::GetKeyState( VK_LCONTROL ) < 0 )
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.y = GSO::World::IEntity2d::GetRefOrigin().y - GetTileHeight();
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().y = GSO::World::IEntity2d::GetRefOrigin().y - GetTileHeight();
                }
                else
                {
                    GameVertex opos = GSO::World::IEntity2d::GetRefOrigin();
                    opos.y = GSO::World::IEntity2d::GetRefOrigin().y - g_ScrollSpeed;
                    GSO::World::IEntity2d::SetRefOrigin( opos );
                    //GSO::World::IEntity2d::GetRefOrigin().y = GSO::World::IEntity2d::GetRefOrigin().y - g_ScrollSpeed;
                }
                
                break;
                
            case VK_NUMPAD4://65:
                if ( GetFocusEntity() )
                {
                    GetFocusEntity()->GetPos().x -= g_ScrollSpeed;
                }
                
                break;
                
            case VK_NUMPAD8://87:
                if ( GetFocusEntity() )
                {
                    GetFocusEntity()->GetPos().y -= g_ScrollSpeed;
                }
                
                break;
                
            case VK_NUMPAD6://68:
                if ( GetFocusEntity() )
                {
                    GetFocusEntity()->GetPos().x += g_ScrollSpeed;
                }
                
                break;
                
            case VK_NUMPAD2://83:
                if ( GetFocusEntity() )
                {
                    GetFocusEntity()->GetPos().y += g_ScrollSpeed;
                }
                
                break;
                
            // Rotate The Entity
            case VK_NUMPAD1://90:
                {
                    GSO::World::IEntity2d* focused = GetFocusEntity();
                    
                    if ( focused )
                    {
                        int maxDir   = focused->m_Anim2d.GetMaxDirection();
                        int currDir  = focused->GetNowAngle();
                        
                        if ( !m_RotateDir.empty() )
                        {
                            m_Rdir++;
                            
                            if ( m_Rdir >= m_RotateDir.size() )
                            {
                                m_Rdir = 0;
                            }
                            
                            currDir = m_RotateDir[m_Rdir];
                        }
                        else
                        { currDir = 0; }
                        
                        /*
                        currDir += 45;
                        if(currDir>=360)
                            currDir-=360;
                            */
                        /*
                        if ((currDir % 45) != 0)
                        //					if(maxDir == 0)
                        {
                            maxDir = focused->m_pResource2d->GetResCount();
                            if(--currDir < 0)
                            {
                        	    currDir = (maxDir-1);
                        //							currDir = 315;
                            }
                             currDir *= 45;
                        }
                        else
                        {
                            //currDir = preDefAngle[currDir];
                            currDir -= 45;
                            if(currDir < 0)
                            {
                        	    currDir = 360 + currDir;
                            }
                        }
                        */
                        focused->FaceTo( currDir );
                    }
                }
                break;
                
            case VK_NUMPAD3://88:
                {
                    GSO::World::IEntity2d* focused = GetFocusEntity();
                    
                    if ( focused )
                    {
                        int maxDir   = focused->m_Anim2d.GetMaxDirection();
                        int currDir  = focused->GetNowAngle();
                        
                        if ( !m_RotateDir.empty() )
                        {
                            m_Rdir--;
                            
                            if ( m_Rdir < 0 )
                            {
                                m_Rdir = m_RotateDir.size() - 1;
                            }
                            
                            currDir = m_RotateDir[m_Rdir];
                        }
                        else
                        { currDir = 0; }
                        
                        /*
                        currDir -= 45;
                        if(currDir<0)
                            currDir+=360;
                            */
                        /*
                        if(maxDir == 0)
                        {
                            maxDir = focused->m_pResource2d->GetResCount();
                            if(++currDir >= maxDir)
                            {
                        	    currDir = 0;
                            }
                            currDir *= 45;
                        }
                        else
                        {
                            //currDir = preDefAngle[currDir];
                            currDir += 45;
                            if(currDir >= 360)
                            {
                        	    currDir = 0;
                            }
                        }
                        */
                        focused->FaceTo( currDir );
                    }
                }
                break;
                
            //case 37: { GVertex spt(Norm().x + 10, Norm().y); Norm() = spt; break; }
            //case 38: { GVertex spt(Norm().x, Norm().y + 10); Norm() = spt; break; }
            //case 39: { GVertex spt(Norm().x - 10, Norm().y); Norm() = spt; break; }
            //case 40: { GVertex spt(Norm().x, Norm().y - 10); Norm() = spt; break; }
            default:
                break;
            }
            
            break;
            
        case WM_KEYUP:
            g_ScrollSpeed = 1;
            break;
        }
        
        /*
        GVertex  mouse_pt;
        GetCursorPos(&mouse_pt);
        ScreenToClient(_WidGet.hWnd, &mouse_pt);
        
        CPRect client_rect;
        GetClientRect(_WidGet.hWnd, &client_rect);
        client_rect.top = client_rect.top + 25;
        
        int count = 0;
        int clock = map_scroll_clock.Interval();
        if((client_rect.is_in_area(mouse_pt)) && (clock == -1))
        {
            if(_MAP_SCROLL_STEP_ <= 50)
        	    _MAP_SCROLL_STEP_ = _MAP_SCROLL_STEP_ + 10;
        
            if(point.x < _MAP_RECT_LIMIT_)
            {
          		    GVertex spt(Norm().x + _MAP_SCROLL_STEP_, Norm().y);
                    Scroll_To(spt);
        		    count++;
            }
            else if(point.x > _WidGet.Width - _MAP_RECT_LIMIT_)
            {
                    GVertex spt(Norm().x - _MAP_SCROLL_STEP_, Norm().y);
                    Scroll_To(spt);
        		    count++;
            }
            if(point.y < _MAP_RECT_LIMIT_)
            {
                    GVertex spt(Norm().x, Norm().y + _MAP_SCROLL_STEP_);
                    Scroll_To(spt);
        		    count++;
            }
            else if(point.y > (_WidGet.Height - _MAP_RECT_LIMIT_))
            {
                    GVertex spt(Norm().x, Norm().y - _MAP_SCROLL_STEP_);
                    Scroll_To(spt);
        		    count++;
            }
        
        }
        */
        //if((count == 0) && (clock == -1))
        //_MAP_SCROLL_STEP_ = 0;
        //else
        //Scroll_speed = Scroll_speed + 2;
    }
    
    
    int CALLBACK Terrain2d::Drag_Role_Query( POINT point, UINT uMsg, UINT wParam )
    {
        Scroll_Query( point, uMsg, wParam );
        
        if ( ( IsEntityEmpty() ) || ( GetFocusEntity() == 0 ) )
        { return PE_NULL; }
        
        BOOL dragable = GetFocusEntity()->GetDragable();
        BOOL enable   = GetFocusEntity()->GetEnable();
        GSO::World::IEntity2d* entity = GetFocusEntity();
        
        if ( entity == NULL )
        { return PE_NULL; }
        
        int cellID = GetZoneId( entity->GetPos() );
        
        if ( cellID != -1 )
        {
            GSO::World::Zone2d* zone = reinterpret_cast<GSO::World::Zone2d*>( GetCellData( cellID ) );
            
            if ( zone )
            {
                zone->ExitBuilding( ( ( GSO::World::Building2d* )entity ) );
            }
        }
        
        entity->SetDragable( true );
        entity->SetEnable  ( true );
        int result = entity->Proc( point, uMsg, wParam );
        entity->SetDragable( dragable );
        entity->SetEnable  ( enable );
        //GSO::World::IEntity2d* entity = GetFocusEntity();
        cellID = GetZoneId( entity->GetPos() );
        
        if ( cellID != -1 )
        {
            GSO::World::Zone2d* zone = reinterpret_cast<GSO::World::Zone2d*>( GetCellData( cellID ) );
            
            if ( zone )
            {
                zone->JoinBuilding( ( ( GSO::World::Building2d* )entity ) );
            }
        }
        
        return result;
    }
    
    
    void CALLBACK Terrain2d::Move_Role_Query( POINT point, UINT uMsg, UINT wParam )  //各角色的事件
    {
        if ( ( IsEntityEmpty() ) || ( GetFocusEntity() == 0 ) )
        { return; }
        
        //((CPMap*)(&_scene_obj))->Query(_Mouse_Situs, message, wParam);
        GameVertex pos = point;
        GSO::World::IEntity2d* entity = GetFocusEntity();
        
        if ( strcmp( entity->ObjectName(), "GSO::World::Role2d" ) == 0 )
        {
            switch ( uMsg )
            {
            case WM_LBUTTONDOWN:
                break;
                
            case WM_RBUTTONDOWN:
                break;
                
            case WM_LBUTTONUP:
                //((GSO::World::Role2d*)entity)->DoTo(ConvertScreenToMap(pos));
                break;
                
            case WM_RBUTTONUP:
                //((GSO::World::Role2d*)entity)->GoTo(ConvertScreenToMap(pos));
                break;
                
            default:
                break;
            }
        }
    }
    
    
    bool SaveAllZone( cellid_t id, GSO::StreamCellDescriptor desp, UINT order, void* pDefData )
    {
        App2dFactory::Terrain2d* pTerrain = ( App2dFactory::Terrain2d* )pDefData;
        GSO::World::Zone2d* zone = ( GSO::World::Zone2d* )pTerrain->GetCellData( id );
        
        if ( zone != NULL )
        {
            zone->SaveZone( pTerrain->GetCellDescriptor( id ) );
        }
        
        //else//要改資料實用的
        //	{
        //		GSO::World::Zone2d* zone=NULL;
        //		void* z = (void*)zone;
        //	    gScene.OnLoadCell(id, desp, &z, NULL);
        //		zone = (GSO::World::Zone2d*)z;
        //		UINT iii = zone->GetBuildings().size();
        //		zone->SaveZone(desp);
        //		gScene.OnReleaseCell(id, desp, (void*)zone, NULL);
        //	}
        return true;
    }
    
    
    //=============================================================================
    //
    //
    //=============================================================================
    int Terrain2d::SaveAllEntity( void )
    {
        if ( GetEntityCount() == 0 )
        { return 0; }
        
        assert( NULL && "not yet" );
        int ii = 0;
        
        for ( uint_t i = 0; i < GetEntityCount(); i++ )
        {
            if ( strcmp( Object( i )->GetResfname(), "" ) != 0 )
            {
                if ( Object( i )->SaveRes( Object( i )->GetResfname() ) )
                { ii++; }
            }
        }
        
        //儲存zone
        //int id = m_pTerrain->GetFocusedCellID();
        ForAllCell( SaveAllZone, this );
        return ii;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void Terrain2d::OnReleaseCell( int Id,
                                   GSO::StreamCellDescriptor CellDescriptor,
                                   void* pData,
                                   void* pUserdata )
    {
        if ( pData == NULL )
        { return; }
        
        if ( true )
        {
            GSO::World::Zone2d* zone = reinterpret_cast<GSO::World::Zone2d*>( pData );
            
            if ( zone == NULL )
            {
                MessageBox( NULL, "Zone can't be NULL", "Warning", MB_OK );
                return;
            }
            
            if ( m_Askfileoverwrite )
            {
                FILE* f0 = NULL;
                FILE* f1 = NULL;
                FILE* f2 = NULL;
                
                if ( !zone->GetWaklablefname().empty() )
                {
                    f0 = fopen( zone->GetWaklablefname().c_str(), "rw" );
                }
                
                if ( !zone->GetEventfname().empty() )
                {
                    f1 = fopen( zone->GetEventfname().c_str(), "rw" );
                }
                
                if ( strcmp( zone->GetResfname(), "" ) != 0 )
                {
                    f2 = fopen( zone->GetResfname(), "rw" );
                }
                
                if ( ( f0 != NULL ) || ( f1 != NULL ) || ( f2 != NULL ) )
                {
                    char buff[255];
                    sprintf( buff, " file\n %s\n %s\n %s\n exist, overwrite it? cancel for save and no ask anymore", zone->GetWaklablefname().c_str(),
                             zone->GetEventfname().c_str(),
                             zone->GetResfname() );
                    int ret = MessageBox( NULL, buff, "Warning", MB_YESNOCANCEL );
                    
                    if ( ret == IDCANCEL )
                    { m_Askfileoverwrite = false; }
                    else if ( ret == IDNO )
                    { goto EXITSAVE; }
                }
                
                if ( f0 != NULL )
                { fclose( f0 ); }
                
                if ( f1 != NULL )
                { fclose( f1 ); }
                
                if ( f2 != NULL )
                { fclose( f2 ); }
                
                //zone->SaveZone(CellDescriptor);
            }
            
            if ( m_Save )
            { zone->SaveZone( CellDescriptor ); }
        }
        
EXITSAVE:
        Terrain2d::OnReleaseCell( Id, CellDescriptor, pData, pUserdata );
        std::vector<CPEWorldObserver*>::iterator iObserver = m_Observers.begin();
        
        for ( ; iObserver != m_Observers.end(); ++iObserver )
        {
            ( *iObserver )->OnTargetChanged( 0 );
        }
    }
    
    GSO::World::IEntity2d* Terrain2d::GetTriggerEntity( LG::Point mousepos )
    {
        return GetEntityBySel( NULL, ConvertScreenToMap( mousepos ) );
    }
}


