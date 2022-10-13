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
#include "zone2d.h"
#include "load_zone.cpp"
#include "terrain2d.h"
#include <chessboard.h>
#include <pf-stdlib.h>
//#include <pf-string.h>

#include <string>

namespace GSO
{
    namespace World
    {
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Zone2d::Zone2d( Terrain2d* pTerrain, uint_t id )
            : p_terrain_( pTerrain )
        {
            SetId( id );
            RenderLayer = LGB::DrawNode::kLayerBG;
            p_chessboard_ = new CPChessboard;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Zone2d::~Zone2d()
        {
            ClearTriggers();
            ClearBuildings();
            SAFE_DELETE( p_chessboard_ );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        pfstd::CString Zone2d::GetWaklablefname( void )
        {
            return p_chessboard_->GetBroadName( board_list_[ WALK_BOARD ] );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        pfstd::CString Zone2d::GetEventfname( void )
        {
            return p_chessboard_->GetBroadName( board_list_[ EVENT_BOARD ] );
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        error_t Zone2d::LoadBoard( uint_t iBoard, const tchar_t* filename )
        {
            return p_chessboard_->Load( iBoard, filename );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Zone2d::IsBoardEmpty( void )
        {
            return p_chessboard_->IsBoardEmpty();
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::SetBoardData( BoardStyle board, uint_t col, uint_t row, void* value )
        {
            p_chessboard_->SetData( board_list_[ board ], col, row, value );
            return;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void* Zone2d::GetBoardData( BoardStyle board, uint_t col, uint_t row )
        {
            return  p_chessboard_->GetData( board_list_[ board ], col, row );
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::JoinBuilding( Building2d* pBuilding )
        {
            buildings_.push_back( pBuilding );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::ExitBuilding( Building2d* pBuilding )
        {
            for( uint_t i = 0; i < buildings_.size(); i++ )
            {
                if( buildings_[ i ] == pBuilding )
                {
                    buildings_.erase( buildings_.begin() + i );
                    return;
                }
            }
            assert( 0 && "Can't building in zone!" );
            //            std::vector<Building2d*>::iterator iBld = buildings_.begin();
            //            for(; iBld != buildings_.end(); ++iBld)
            //            {
            //                if(*iBld == pBuilding)
            //                {
            //                    buildings_.erase(iBld);
            //                    return;
            //                }
            //            }
                        //buildings_.clear();
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::ClearBuildings()
        {
            buildings_.clear();
        }



        /*
                //=============================================================================
                // NAME:
                // DESC: 建立菱形格座標2維陣列
                //=============================================================================
                bool Zone2d::CreateChessboard(int MapWidth,
                                               int MapHeight,
                                               int GridWidth,
                                               int GridHeight,
                                               shape_t GridType)
                {
                    SetMapValue(MapWidth, MapHeight, GridWidth, GridHeight, GridType);

                    if((MapWidth == 0) ||
                        (MapHeight == 0) ||
                        (GridWidth == 0) ||
                        (GridHeight == 0))
                        return false;


                    GVertex ScreenToMapSite;
                    GVertex GridSite;
                    //SetGridValue(GridWidth, GridHeight, GridType);

                    //Rhomb_Grid_WHScale(m_Grid.width, m_Grid.height); //Rhome格地形的長寬比例值
                    if(m_Grid.shape == _P_RHOMBUS_)   //菱形格資料
                    {
                        //Rhomb_Chessboard_Revise(m_Grid.width,
                        //                         m_Grid.height,
                        //                         MapHeight);

                        ScreenToMapSite.x = MapWidth;    //換算地圖右上角得出矩陣寬
                        ScreenToMapSite.y = 0;
                        GridSite = Rhomb_Map_to_Chessboard_Site(ScreenToMapSite);
                        m_Cols = GridSite.x ;

                        ScreenToMapSite.x = MapWidth;    //換算地圖右下角得出矩陣高
                        ScreenToMapSite.y = MapHeight;
                        GridSite = Rhomb_Map_to_Chessboard_Site(ScreenToMapSite);
                        m_Rows = GridSite.y ;
                    }
                    // 矩形..........................
                    else if(m_Grid.shape == _P_RECT_)   //菱形格資料
                    {
                        m_Cols = (int) ceil((double) (MapWidth / GridWidth));
                        m_Rows = (int)
                                 ceil((double) (MapHeight / GridHeight));
                    }
                    Board_Release_All();
                    Board_Allot_All(m_Cols, m_Rows);
                    InsertBoard("Walkable.board", m_Cols, m_Rows);
                    InsertBoard("Event.board", m_Cols, m_Rows);

                    return true;

                }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        error_t Zone2d::LoadZone( StreamCellDescriptor pCellDescriptor )
        {
            if( pCellDescriptor == NULL )
                return -1;

            error_t result = 0;

            p_chessboard_->Board_Release_All();
            board_list_[ WALK_BOARD ] = p_chessboard_->InsertBoard( "Walkable.board",
                p_terrain_->GetTileCols(),
                p_terrain_->GetTileRows(),
                sizeof( long ), ( void* )_UN_WALKABLE );
            board_list_[ EVENT_BOARD ] = p_chessboard_->InsertBoard( "Event.board",
                p_terrain_->GetTileCols(),
                p_terrain_->GetTileRows(),
                sizeof( long ), ( void* )0 );
            //CPGspParser Loader;
            //result = Loader.Load(path);
            GspNode* Loader = ( GspNode* )pCellDescriptor;

            GspZoneConfigure reader( this, p_terrain_ );
            Loader->DoPass( &reader );

            return result;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        error_t Zone2d::SaveZone( StreamCellDescriptor pCellDescriptor )
        {
            GspNode* zonenode = ( GspNode* )pCellDescriptor;

            zonenode->Clear();

            tchar_t buff[ 255 ];
            tchar_t drive[ _MAX_DRIVE ];
            tchar_t dir[ _MAX_LDIR ];
            tchar_t fname[ _MAX_LFNAME ];
            tchar_t ext[ _MAX_LEXT ];
            if( strcmp( GetResfname(), "" ) != 0 )
                _splitpath( GetResfname(), drive, dir, fname, ext );
            else
                strcpy( fname, "Def" );
            // to fix
            //            uint_t oldid = GetId();
            //
            //            uint_t ox=(oldid%25)*800;
            //            uint_t oy=(oldid/25)*600;
            //            
            //            
            //            int zid = ((ox/800)*10000)+(oy/600);
            //            zonenode->WriteCommand("ID", "=", _itoa(zid, buff, 10));
            zonenode->WriteCommand( "ID", "=", lltoa( GetId(), buff, 10 ) );
            pfstd::CString nn = GetResfname();
            pfstd::CString name = GetResfname();
            if( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name.c_str(), Sprite2d::GetCurrentDirectory() );
                if( currname != NULL )
                    name = currname;
                zonenode->WriteCommand( "LOADRES", "=", name.c_str() );
            }

            // to fix
            //sprintf(buff, "%d,%d", ox, oy);
            sprintf( buff, "%d,%d", GetPos().x, GetPos().y );
            zonenode->WriteCommand( "ORIGIN", "=", buff );

            std::string gdFile;
            gdFile = "/";
            gdFile += p_terrain_->GetWalkFolder();
            gdFile += "/";
            gdFile += p_terrain_->GetMapName();
            std::string evFile;
            evFile = "/";
            evFile += p_terrain_->GetEventFolder();
            evFile += "/";
            evFile += p_terrain_->GetMapName();

            sprintf( buff, "ZGD_%s_%lld.wlk", fname, GetId() );
            gdFile += buff;
            sprintf( buff, "ZGD_%s_%lld.evt", fname, GetId() );
            evFile += buff;

            char* goutbuf = new char[ ( __max( gdFile.size(), evFile.size() ) + 1 ) << 1 ];

            mcurrentpath( goutbuf, gdFile.c_str() );

            zonenode->WriteCommand( "GDWALK", "=", goutbuf );
            PBOARD pBoard = p_chessboard_->GetDescriptor( WALK_BOARD );
            if( pBoard )
            {
                std::string path;
                /*
                if(p_terrain_ !=NULL )
                {
                    tchar_t curdrive[_MAX_DRIVE];
                    tchar_t curdir[_MAX_LDIR];
                    tchar_t curfname[_MAX_LFNAME];
                    tchar_t curext[_MAX_LEXT];
                    _splitpath(p_terrain_->GetFilename(), curdrive, curdir, curfname, curext);
                    path=curdrive;
                    path+=curdir;

                }
                */
                //else
                {
                    path = Sprite2d::GetCurrentDirectory();
                }
                path += gdFile;

                p_chessboard_->Save( pBoard, path.c_str() );
            }

            mcurrentpath( goutbuf, evFile.c_str() );

            zonenode->WriteCommand( "GDEVENT", "=", goutbuf );
            pBoard = p_chessboard_->GetDescriptor( EVENT_BOARD );
            if( pBoard )
            {
                std::string path;
                /*
                if(p_terrain_ !=NULL )
                {
                    tchar_t curdrive[_MAX_DRIVE];
                    tchar_t curdir[_MAX_LDIR];
                    tchar_t curfname[_MAX_LFNAME];
                    tchar_t curext[_MAX_LEXT];
                    _splitpath(p_terrain_->GetFilename(), curdrive, curdir, curfname, curext);
                    path=curdrive;
                    path+=curdir;
                }
                */
                //else
                {
                    path = Sprite2d::GetCurrentDirectory();
                }
                path += evFile;
                p_chessboard_->Save( pBoard, path.c_str() );
            }
            SAFE_DELETE_ARRAY( goutbuf );

            char currpath[ _MAX_LPATH ];

            //std::vector<Building2d*>::iterator iBuilding = buildings_.begin();
            for( uint_t bi = 0; bi < buildings_.size(); bi++ )
            {
                GSO::World::Building2d* build = buildings_[ bi ];

                GspNode* bulnode = new GspNode( "BUILDING" );
                bulnode->WriteCommand( "ID", "=", lltoa( build->GetId(), buff, 10 ) );

                sprintf( buff, "%d,%d", build->GetPos().x, build->GetPos().y );
                bulnode->WriteCommand( "POSITION", "=", buff );

                //要拿掉
                //bulnode->WriteCommand("X",  "=", _itoa(build->GetPos().x, buff, 10));
                //bulnode->WriteCommand("Y",  "=", _itoa(build->GetPos().y, buff, 10));

                name = build->GetResfname();
                if( !name.empty() )
                {
                    const tchar_t* s = strstr( name.c_str(), build->GetCurrentDirectory() );
                    if( s != NULL )
                    {
                        //name = s;
                        //name+= (strlen(build->GetCurrentDirectory()));
                        name = s + ( strlen( build->GetCurrentDirectory() ) );
                    }
                    mcurrentpath( currpath, name.c_str() );
                    bulnode->WriteCommand( "LOADRES", "=", currpath );
                }


                name = build->GetAnmfname();
                if( !name.empty() )
                {
                    const tchar_t* s = strstr( name.c_str(), build->GetCurrentDirectory() );
                    if( s != NULL )
                    {
                        //name = s;
                        //name+= (strlen(build->GetCurrentDirectory()));
                        name = s + ( strlen( build->GetCurrentDirectory() ) );
                    }
                    mcurrentpath( currpath, name.c_str() );
                    bulnode->WriteCommand( "LOADANM", "=", currpath );
                }
                //                else //暫時資料
                //                {
                //                    bulnode->WriteCommand("LOADANM", "=", "Terrain/Bld/Standby.anm");
                //                }

                bulnode->WriteCommand( "DIR", "=", _itoa( build->GetNowAngle(), buff, 10 ) );

                if( build->GetDataBaseName() != NULL )
                {
                    sprintf( buff, "%s,%d", build->GetDataBaseName(), build->GetDataBaseEntry() );
                    bulnode->WriteCommand( "DATABASE", "=", buff );
                }
                zonenode->AttachChildNode( bulnode );
                bulnode->WriteCommand( "RENDERLAYER", "=", _itoa( build->RenderLayer, buff, 10 ) );
            }

            // Save Events
            std::vector<Trigger*>::iterator iEvent = triggers_.begin();
            for( ; iEvent != triggers_.end(); ++iEvent )
            {
                GspNode* eventnode = new GspNode( "TRIGGER" );

                eventnode->WriteCommand( "TYPE", "=", _itoa( ( *iEvent )->type, buff, 10 ) );
                eventnode->WriteCommand( "SHAPE", "=", _itoa( ( *iEvent )->shape, buff, 10 ) );
                eventnode->WriteCommand( "LTX", "=", _itoa( ( *iEvent )->range.left, buff, 10 ) );
                eventnode->WriteCommand( "LTY", "=", _itoa( ( *iEvent )->range.top, buff, 10 ) );
                eventnode->WriteCommand( "BRX", "=", _itoa( ( *iEvent )->range.right, buff, 10 ) );
                eventnode->WriteCommand( "BRY", "=", _itoa( ( *iEvent )->range.bottom, buff, 10 ) );
                eventnode->WriteCommand( "PX", "=", _itoa( ( *iEvent )->pos.x, buff, 10 ) );
                eventnode->WriteCommand( "PY", "=", _itoa( ( *iEvent )->pos.y, buff, 10 ) );
                eventnode->WriteCommand( "DATA", "=", ( *iEvent )->data );
                zonenode->AttachChildNode( eventnode );

            }

            return 1;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::GetTrigger( const GameVertex mapPos, std::vector<const Trigger*>& TriggerList )
        {
            std::vector<Trigger*>::iterator iTrigger = triggers_.begin();
            for( ; iTrigger != triggers_.end(); ++iTrigger )
            {
                Trigger* pTrigger = *iTrigger;
                if( ( mapPos.x > ( pTrigger->range.left > pTrigger->range.right ? pTrigger->range.right : pTrigger->range.left ) &&
                    mapPos.x < ( pTrigger->range.left > pTrigger->range.right ? pTrigger->range.left : pTrigger->range.right ) ) &&
                    ( mapPos.y > ( pTrigger->range.top > pTrigger->range.bottom ? pTrigger->range.bottom : pTrigger->range.top ) &&
                    mapPos.y < ( pTrigger->range.top > pTrigger->range.bottom ? pTrigger->range.top : pTrigger->range.bottom ) ) )
                {
                    TriggerList.push_back( pTrigger );
                }
            }
            return;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::GetTriggerList( std::vector<Trigger*>& events )
        {
            std::vector<Trigger*>::iterator iTrigger = triggers_.begin();
            for( ; iTrigger != triggers_.end(); ++iTrigger )
            {
                events.push_back( *iTrigger );
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::ClearTriggers()
        {
            std::vector<Trigger*>::iterator iEvent = triggers_.begin();
            for( ; iEvent != triggers_.end(); ++iEvent )
            {
                delete *iEvent;
            }
            triggers_.clear();
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Zone2d::InsertTrigger( Trigger* pTrigger )
        {
            triggers_.push_back( pTrigger );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Zone2d::RemoveTrigger( Trigger* pTrigger )
        {
            std::vector<Trigger*>::iterator iEvent = triggers_.begin();
            for( ; iEvent != triggers_.end(); ++iEvent )
            {
                if( pTrigger = *iEvent )
                {
                    triggers_.erase( iEvent );
                    return true;
                }
            }
            return false;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
//        void Zone2d::RenderTo(LGB::VMCRender2d &renderer)
//        {
//            renderer.InsertNode(this, LGB::DrawNode::kLayerBG);
//
//            /*
//            std::vector<Building2d*>::iterator iBld = buildings_.begin();
//            for(; iBld != buildings_.end(); ++iBld)
//            {
//                renderer.InsertNode(*iBld, LGB::VMCRender2d::kLayerBld);
//            }
//            */
//
//        }
    };
};
