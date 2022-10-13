//Map.cpp game map class
//
//                map have three king site
//                             screen site
//                             map    site
//                             grid   site
//
//
//                                                         Pin
//

//01. == C 系統文件/C++ 系統文件

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <parser/pgsp_file.h>
//
#include <aintersect.inl>

//03. == 其他 使用 函式庫的.h 文件


//04. == 此專案內.h 文件
#include "vmc-render2d.h"
#include "data_stream_def.h"
#include "scene2d.h"
#include "load_world.cpp"
#include "topog2d.h"
#include "terrain2d.h"



namespace GSO
{
    namespace World
    {
#define _G2ROLE(e)     ((Role2d*)e)
#define _G2ENTITY(e)   ((IEntity2d*)e)

        int compare( const void* elem1, const void* elem2 );

        class MyTopog2d: public Topog2d
        {
        public:

            MyTopog2d(){}
            ~MyTopog2d(){}

            virtual GameVertex  OnGetScreenDatumMark() { return GSO::World::IEntity2d::GetRefOrigin(); }
        };


        ////////////////////////////////////////////////////////////////////////////////////////////
        //  Terrain2d class
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Terrain2d::Terrain2d():// m_pRenderer(0),
            m_Streaming( true )
        {
            m_Camera.SetPosition( GameVertex( 0, 0, 0 ) );
            m_GridConverter = new MyTopog2d;
            m_ConfigureIO = new CPGspParser;
            m_Scene = new Scene2d;
            m_bLoadRes = true;
            m_ZoneDrawState = LGB::DrawNode::RSNormal;
            m_Name = "map";
            m_WalkFolder = "walk";
            m_EventFolder = "event";
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Terrain2d::~Terrain2d()
        {
            //m_ScrollSpeed = 0;
            Destroy();
            SAFE_DELETE( m_GridConverter );
            SAFE_DELETE( m_ConfigureIO );
            SAFE_DELETE( m_Scene );
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Terrain2d::SetMapName( const tchar_t* name )
        {
            m_Name = name;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        const tchar_t* Terrain2d::GetMapName()// const
        {
            return m_Name.c_str();
        }

        void Terrain2d::SetWalkFolder( const tchar_t* name )
        {
            m_WalkFolder = name;
        }


        const tchar_t* Terrain2d::GetWalkFolder()
        {
            return m_WalkFolder.c_str();
        }

        // 設定預設擺放event table的資料夾名稱
        void Terrain2d::SetEventFolder( const tchar_t* name )
        {
            m_EventFolder = name;
        }


        const tchar_t* Terrain2d::GetEventFolder()
        {
            return m_EventFolder.c_str();
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Terrain2d::Create( const LG::Point& origin,
            uint_t tileCountX, uint_t tileCountY,
            uint_t tileWidth, uint_t tileHeight,
            int  gridWidth, int gridHeight, shape_t gridType )
        {
            Destroy();

            //m_Origin = origin;
            m_Tiles.cx = tileCountX;
            m_Tiles.cy = tileCountY;

            //m_GridConverter->GetWidth() = tileWidth;
            //m_GridConverter->GetHeight() = tileHeight;

            m_MapSize.cx = m_Tiles.cx*tileWidth;
            m_MapSize.cy = m_Tiles.cy*tileHeight;

            bool ret = m_GridConverter->Create( tileWidth,
                tileHeight,
                gridWidth,
                gridHeight,
                gridType );//Topog2d::SetMapValue(m_GridConverter->GetWidth(), m_GridConverter->GetHeight(), gridWidth, gridHeight, gridType);

// This will update focused cell ID, since it's been erased in release
            Update();

            GspNode* node = NULL;
            if( ( ( CPGspParser* )m_ConfigureIO )->GetChildrenCount() == 0 )
            {
                node = new GspNode( "2DTERRAIN" );
                ( ( CPGspParser* )m_ConfigureIO )->AttachChildNode( node );
            }
            else
            {
                node = ( *( ( CPGspParser* )m_ConfigureIO ) )[ 0 ];
                /*
                2010 01 20  為何要這樣做 導致外面當掉? 為何? 不知道 先註解起來
                if(strcmp(node->GetStyle(), "2DTERRAIN") != 0)
                {
                    ((CPGspParser*)m_ConfigureIO)->DetachChildNode(node);
                    SAFE_DELETE(node);
                    node = new GspNode("2DTERRAIN");
                    ((CPGspParser*)m_ConfigureIO)->AttachChildNode(node);
                }
                */
            }

            node->WriteCommand( "NAME", "=", GetMapName() );

            tchar_t buff[ 255 ];
            sprintf( buff, "%d,%d,%d,%d,%d,%d,%d,%d,%d",
                origin.x, origin.y, tileCountX, tileCountY,
                tileWidth, tileHeight,
                gridWidth, gridHeight, gridType );

            node->WriteCommand( "SEGMENT", "=", buff );

            if( !ret )
                return false;
            return true;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        LG::Rect Terrain2d::GetTerritory( void )
        {
            //LG::Rect rect(0, 0,
            //               m_Tiles.cx*m_GridConverter->GetWidth(), m_Tiles.cy*m_GridConverter->GetHeight());
            LG::Rect rect( 0, 0, m_MapSize.cx, m_MapSize.cy );
            return rect;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::GridCenterPoint( Zone2d& resideZone, GameVertex GridSite )
        {
            return m_GridConverter->GridCenterPoint( GridSite ) + resideZone.GetPos();
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::ConvertGridToMap( const GameVertex& gridPos )
        {
            //GameVertex localMap = m_GridConverter->ChessboardToMapSite(gridPos);

            return m_GridConverter->ChessboardToMapSite( gridPos );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::ConvertMapToGrid( const GameVertex& mapPos )
        {
            return m_GridConverter->MapToChessboardSite( mapPos );
        }
        /*
                //=============================================================================
                // NAME:
                // DESC:
                //=============================================================================
                GameVertex Terrain2d::ConvertGridToScreen(const GameVertex& gridPos)
                {
                    GameVertex mapPos = ConvertGridToMap(gridPos);
                    return ConvertMapToScreen(mapPos);
                }
        */
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::ConvertScreenToGrid( const GameVertex& screenPos )
        {
            //GameVertex mapPos = ConvertScreenToMap(screenPos);
            return ConvertMapToGrid( ConvertScreenToMap( screenPos ) );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::ConvertMapToScreen( const GameVertex& mapPos )
        {
            return m_GridConverter->MapToScreenSite( mapPos );
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        GameVertex Terrain2d::ConvertScreenToMap( const GameVertex& screenPos )
        {
            return m_GridConverter->ScreenToMapSite( screenPos );
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        bool InsertEntityToRender(unsigned int order,
                                   void* pEntity,
                                   void* pData)
        {
            LGB::DrawNode::rendercontainer_t* render = reinterpret_cast<LGB::DrawNode::rendercontainer_t*>(pData);
            GSO::World::IEntity2d* entity = reinterpret_cast<GSO::World::IEntity2d*>(pEntity);
            entity->RenderTo(*render);

            return true;
        }


        void Terrain2d::RenderTo(LGB::DrawNode::rendercontainer_t &surfacecontsurface,
                                  LGB::DrawNode::rendercontainer_t &entitycontainer)

        {
            for(uint_t i = 0; i< m_pLoadedCellVec.size(); i++)
            {
                Zone2d* zone = reinterpret_cast<Zone2d*>(m_pLoadedCellVec[i]->pData);
                if (!zone)
                    return;

                zone->RenderTo(surfacecontsurface);
            }

            // Then Insert all Roles
            ForAllElement(InsertEntityToRender, &entitycontainer);
        }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool InsertEntityToRenderer( unsigned int order, void* pEntity, void* pData )
        {
            LGB::VMCRender2d* renderer = reinterpret_cast< LGB::VMCRender2d* >( pData );
            GSO::World::IEntity2d* entity = reinterpret_cast< GSO::World::IEntity2d* >( pEntity );
            //            renderer->InsertNode(entity, entity->RenderLayer);

            renderer->DoRender( entity );
            //entity->RenderTo(renderer->GetContainer(entity->RenderLayer));

            //entity->RenderTo(*renderer);
            /*
            if(strcmp(entity->ObjectName(), "Building2d") == 0)
                entity->RenderTo(renderer->m_BlitNode[LGB::DrawNode::kLayerBld]);
            else
                entity->RenderTo(renderer->m_BlitNode[LGB::DrawNode::kLayerRole]);
                */
            return true;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Terrain2d::SetZoneDrawState( LGB::DrawNode::DrawMethod state )
        {
            m_ZoneDrawState = state;
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Terrain2d::DrawTo( LGB::VMCRender2d* render )
        {
            if( render == NULL )
                return;

            // Insert blds of all loaded zones
            for( uint_t i = 0; i < DataStream::GetLoadedCellCount(); i++ )
            {
                Zone2d* zone = reinterpret_cast< Zone2d* >( DataStream::GetLoadedCellData( i ) );
                if( zone == NULL )
                    return;

                //zone->RenderTo(renderer);
                zone->RenderMethod = m_ZoneDrawState;
                switch( m_ZoneDrawState )
                {
                case LGB::DrawNode::RSNormal:
                    {

                    }
                    break;
                case LGB::DrawNode::RSClipAlpha:
                    {
                        zone->RenderTop = 0;
                        zone->RenderRight = zone->GetWidth();
                        zone->RenderLeft = 0;
                        zone->RenderBottom = zone->GetHeight();
                    }
                    break;
                }

                render->InsertTerrain( zone );
            }

            // Then Insert all Roles
            m_Scene->ForAllElement( InsertEntityToRenderer, render );
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Terrain2d::Destroy( void )
        {
            m_GridConverter->Clear();
            m_filename.clear();
            DataStream::ReleaseCells();

            m_Scene->SetFocusElement( NULL );
            for( uint_t i = 0; i < m_Scene->ElementCount(); i++ )
            {
                IEntity2d* entity = operator[]( i );
                SAFE_DELETE( entity );
            }
            m_Scene->ElementClear();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        //          讀入地圖資料
        ////////////////////////////////////////////////////////////////////////////////////////////
        /*
        bool Terrain2d::LoadData(const tchar_t* MapInfoPath,
                            int GridWidth, int GridHeight,
                            BYTE GridShape
                         )
        {
            if(MapInfoPath == NULL)
                return false;
            FILE *Data;
            if((Data=fopen(MapInfoPath,"rb")) == NULL)//開檔如果-1則失敗，傳回-1
            {
                tchar_t msg[300];
                sprintf(msg,"Load map information file %s failure !!!", MapInfoPath);
                MessageBox(NULL,msg, "Load Failed", MB_OK);
                return false;
            }
            SetGridValue(GridWidth, GridHeight, GridShape);
            tchar_t Title[4];
            fread(Title, sizeof(Title), 1, Data);
            if((Title[0] != 'P') && (Title[1] != 'M') && (Title[2] != 'I'))
            {
                MessageBox(NULL,"資料檔案格式錯誤!!!","讀檔錯誤", MB_OK);
                return false;
            }
            fread(&m_Chessboard_Cols, sizeof(m_Chessboard_Cols), 1, Data);
            fread(&m_Chessboard_Rows, sizeof(m_Chessboard_Rows), 1, Data);
            //動態建立存放每個地圖的二維陣列=====================================
            /*
            m_walkable.Allot(m_Chessboard_Cols, m_Chessboard_Rows);  //可走不可走
            m_MapObject.Allot(m_Chessboard_Cols, m_Chessboard_Rows); //物件
            m_MapObject.SetValue(0);
            m_event.Allot(m_Chessboard_Cols, m_Chessboard_Rows);
            */
            //end 動態建立存放每個地圖的二維陣列=================================

            //讀入walkable
            /*
            for(int i = 0 ; i < m_Chessboard_Rows; i++)
                   for(int j = 0; j < m_Chessboard_Cols; j++)
                       fread(&m_walkable(i, j),sizeof(bool),1,Data);
            dword_t ArrayCols;
               dword_t ArrayRows;
            //read event
            fread(&ArrayCols, sizeof(ArrayCols), 1, Data);
            fread(&ArrayRows, sizeof(ArrayRows), 1, Data);
            if((ArrayCols != m_Chessboard_Cols) && (ArrayRows != m_Chessboard_Rows))
            {
                fclose(Data);
                return false;
            }
            for(i = 0 ; i < m_Chessboard_Rows; i++)
                   for(int j = 0; j < m_Chessboard_Cols; j++)
                       fread(&m_event(i, j),sizeof(short),1,Data);
            fread(&ArrayCols, sizeof(ArrayCols), 1, Data);
            fread(&ArrayRows, sizeof(ArrayRows), 1, Data);
            m_MapGrid.Allot(ArrayCols, ArrayRows);
            for(i = 0 ; i < ArrayRows; i++)
                   for(int j = 0; j < ArrayCols; j++)
            //            fread(&m_MapGrid(i, j),sizeof(_Gridtype),1,Data);
               fclose(Data);
            return true;
            }
            */
            //==========================================================================================
            // NAME: SetScreenCenter()
            // DESC: 
            //==========================================================================================
        void Terrain2d::SetScreenSize( const GameVertex& screenSize )
        {
            GameVertex screencenter = screenSize;
            screencenter = screencenter / 2;

            m_Camera.SetPosition( screencenter );//m_GridConverter->ScreenToMapSite(screencenter);
        }

        //==========================================================================================
        // NAME: GetGridData()
        // DESC: get a grid data from zones
        //==========================================================================================
        void* Terrain2d::GetGridData( BoardStyle board, const GameVertex& mapPos )
        {
            int cellID = GetZoneId( mapPos );
            if( cellID == -1 )
            {
                return 0;
            }
            else
            {
                // find zone
                Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );

                if( !zone )
                {
                    return 0;
                }
                // convert to local pos
                GameVertex localPos;
                localPos.x = mapPos.x - zone->GetPos().x;
                localPos.y = mapPos.y - zone->GetPos().y;
                // convert to grid 
                GameVertex localGrid = m_GridConverter->MapToChessboardSite( localPos );
                //******* local grid couldn't be less 0
                if( ( localGrid.x < 0 ) || ( localGrid.y < 0 ) )
                    return NULL;
                // find data
                return zone->GetBoardData( board, localGrid.x, localGrid.y );
            }
        }

        //==========================================================================================
        // NAME: SetGridData()
        // DESC: get a grid data from zones
        //==========================================================================================
        void Terrain2d::SetGridData( BoardStyle board, const GameVertex& mappos, void* value )
        {
            int cellID = GetZoneId( mappos );
            if( cellID == -1 )
            {
                return;
            }
            else
            {
                // find zone
                Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );

                if( !zone )
                {
                    return;
                }

                GameVertex localGrid = m_GridConverter->MapToChessboardSite( mappos );
                SetGridData( zone, board, localGrid.x, localGrid.y, value );

            }
        }


        //==========================================================================================
        // NAME: GetGridData()
        // DESC: get a grid data from zones
        //==========================================================================================
        void* Terrain2d::GetGridData( Zone2d* resideZone, BoardStyle board, int gridX, int gridY )
        {
            int chessboardCols = m_GridConverter->GetCols();
            int chessboardRows = m_GridConverter->GetRows();
            if( gridX < 0 ||
                gridX >= chessboardCols ||
                gridY < 0 ||
                gridY >= chessboardRows )
            {
                // Exceeds the realm, access neighbors to find data
                GameVertex mapPos = ConvertGridToMap( GameVertex( gridX, gridY, 0 ) ) + resideZone->GetPos();
                int cellID = GetZoneId( mapPos );
                if( cellID == -1 )
                {
                    return 0;
                }
                else
                {
                    Zone2d* neighborZone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );

                    if( !neighborZone )
                        return 0;
                    GameVertex localPos;
                    localPos.x = mapPos.x - neighborZone->GetPos().x;
                    localPos.y = mapPos.y - neighborZone->GetPos().y;
                    GameVertex localGrid = m_GridConverter->MapToChessboardSite( localPos );
                    return neighborZone->GetBoardData( board, localGrid.x, localGrid.y );
                }
            }

            return resideZone->GetBoardData( board, gridX, gridY );
        }

        //==========================================================================================
        // NAME: SetGridData()
        // DESC: set a grid data to curr zone
        //==========================================================================================
        void Terrain2d::SetGridData( Zone2d* resideZone,
            BoardStyle board,
            int gridX, int gridY,
            void* value )
        {
            if( !resideZone )
            {
                return;
            }

            int chessboardCols = m_GridConverter->GetCols();
            int chessboardRows = m_GridConverter->GetRows();
            if( gridX < 0 ||
                gridX >= chessboardCols ||
                gridY < 0 ||
                gridY >= chessboardRows )
            {
                return;
            }

            resideZone->SetBoardData( board, gridX, gridY, value );
        }


        //==========================================================================================
        // NAME:
        // DESC: get a role form this world
        //==========================================================================================
        IEntity2d* Terrain2d::operator[] ( uint_t i )
        {
            return ( IEntity2d* )( this->m_Scene->operator[] )( i );
        }

        //==========================================================================================
        // NAME: GetRole()
        // DESC: get a role form this world
        //==========================================================================================
        Role2d* Terrain2d::Role( int idx )
        {
            return _G2ROLE( operator[]( idx ) );
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        IEntity2d* Terrain2d::GetEntityBySel( const tchar_t* objname, const GameVertex& mapPos, bool ignorefocus )
        {
            unsigned int count = m_Scene->ElementCount();
            std::vector<IEntity2d*> roleList;
            for( unsigned int i = 0; i < count; ++i )
            {
                IEntity2d* temp = reinterpret_cast< IEntity2d* >( operator[]( i ) );
                if( temp != NULL )
                {
                    if( temp->IsCollisionByImage( mapPos ) )
                    {
                        roleList.push_back( temp );
                        //return temp;
                    }
                }
            }

            if( roleList.size() == 0 )
                return NULL;

            // 用來比大小的資料
            int norm = 0;
            // 用來記最大的那一個
            int theOne = 0;

            if( !ignorefocus )
            {
                for( unsigned int i = 0; i < roleList.size(); ++i )
                {
                    if( roleList[ i ]->GetPos().y > norm )
                    {
                        norm = roleList[ i ]->GetPos().y;
                        theOne = i;
                    }
                }
            }
            else
            {
                for( unsigned int i = 0; i < roleList.size(); ++i )
                {
                    if( roleList[ i ] == GetFocusEntity() )
                    {
                        roleList[ i ] = NULL;
                        continue;
                    }

                    if( roleList[ i ]->GetPos().y > norm )
                    {
                        norm = roleList[ i ]->GetPos().y;
                        theOne = i;
                    }
                }
            }
            if( objname == NULL )
                return roleList[ theOne ];
            else
                return ( strcmp( roleList[ theOne ]->ObjectName(), objname ) == 0 ) ? roleList[ theOne ] : NULL;
        }


        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        void Terrain2d::GetEntityBySel( const tchar_t* objname, const GameVertex& mapPos, std::vector<IEntity2d*>& picklist )
        {
            unsigned int count = m_Scene->ElementCount();
            for( unsigned int i = 0; i < count; ++i )
            {
                IEntity2d* temp = reinterpret_cast< IEntity2d* >( operator[]( i ) );
                if( temp != NULL )
                {
                    if( temp->IsCollisionByImage( mapPos ) )
                    {
                        if( objname == NULL )
                            picklist.push_back( temp );
                        else
                        {
                            if( strcmp( temp->ObjectName(), objname ) == 0 )
                                picklist.push_back( temp );
                        }
                        //return temp;
                    }
                }
            }

            if( picklist.size() == 0 )
                return;


            return;
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        IEntity2d* Terrain2d::GetEntityByCD( const tchar_t* objname, const GameVertex& mapPos )
        {
            unsigned int count = m_Scene->ElementCount();
            for( unsigned int i = 0; i < count; ++i )
            {
                IEntity2d* temp = reinterpret_cast< IEntity2d* >( operator[]( i ) );
                if( temp->IsCollisionByBound( mapPos ) )
                {
                    if( temp == NULL )
                        return temp;
                    else
                    {
                        if( strcmp( temp->ObjectName(), objname ) == 0 )
                            return temp;
                    }
                }
            }
            return 0;
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        IEntity2d* Terrain2d::GetTypeEntityById( const tchar_t* type, entityid_t eid )
        {
            for( unsigned int i = 0; i < GetEntityCount(); ++i )
            {
                if( strcmp( operator[]( i )->ObjectName(), type ) == 0 )
                {
                    if( operator[]( i )->GetId() == eid )
                        return operator[]( i );
                }
            }
            return NULL;

        }


        //==========================================================================================
        // NAME: RoleByID()
        // DESC: get a role form this world
        //==========================================================================================
        IEntity2d* Terrain2d::GetEntityByID( entityid_t id )
        {
            for( uint_t i = 0; i < m_Scene->ElementCount(); ++i )
            {
                if( _G2ENTITY( operator[]( i ) )->GetId() == id )
                    return _G2ENTITY( operator[]( i ) );
            }

            return NULL;
        }

        //==========================================================================================
        // NAME:
        // DESC: 
        //==========================================================================================
        bool Terrain2d::IsEntityEmpty( void )
        {
            return m_Scene->IsElementEmpty();
        }

        //==========================================================================================
        // NAME:
        // DESC: 
        //==========================================================================================
        IEntity2d* Terrain2d::GetEntityByName( const tchar_t* characterName )
        {
            for( uint_t i = 0; i < m_Scene->ElementCount(); ++i )
            {
                IEntity2d* pRole = _G2ENTITY( operator[]( i ) );
                if( strcmp( pRole->m_CharacterName.c_str(), characterName ) == 0 )
                {
                    return pRole;
                }
            }

            return NULL;
        }


        //==========================================================================================
        // NAME: RoleIDByName()
        // DESC: 
        //==========================================================================================
        entityid_t Terrain2d::GetEntityIDByName( const tchar_t* characterName )
        {
            for( uint_t i = 0; i < m_Scene->ElementCount(); ++i )
            {
                IEntity2d* pRole = _G2ENTITY( operator[]( i ) );
                if( strcmp( pRole->m_CharacterName.c_str(), characterName ) == 0 )
                {
                    return pRole->GetId();
                }
            }

            return -1;
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        IEntity2d* Terrain2d::GetFocusEntity( void )
        {
            return _G2ROLE( m_Scene->GetFocusElement() );
        }

        //==========================================================================================
        // NAME: SetFocusRole()
        // DESC: get a role form this world
        //==========================================================================================
        void Terrain2d::SetFocusEntity( IEntity2d* role )
        {
            if( role != NULL )
            {
                m_Scene->SetFocusElement( role );
            }
            else
            {
                m_Scene->SetFocusElement( NULL );
            }

            /*
            if (m_pRenderer)
            {
                m_pRenderer->SetFocus(role);
            }
            */
        }


        //==========================================================================================
        // NAME: RoleIDByName()
        // DESC: 
        //==========================================================================================
        void Terrain2d::JoinEntity( IEntity2d* entity, const GameVertex& mappos )
        {
            if( entity == NULL )
                return;

            if( entity->GetWorld() != NULL )
                entity->GetWorld()->ExitEntity( entity );

            if( strcmp( entity->ObjectName(), _T( "Role2d" ) ) == 0 )
            {
                //return;
            }
            else if( strcmp( entity->ObjectName(), _T( "Building2d" ) ) == 0 )
            {
                int cellID = GetZoneId( mappos );
                if( cellID == -1 )
                {
                    return;
                }
                else
                {
                    Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );

                    if( zone == NULL )
                        return;

                    zone->JoinBuilding( ( GSO::World::Building2d* )entity );
                }
            }
            entity->ClearRoute();
            entity->SetWorld( this );
            entity->SetPos( mappos );
            m_Scene->InsertElement( entity );
            return;
        }

        //==========================================================================================
        // NAME: RoleIDByName()
        // DESC: 
        //==========================================================================================
        void Terrain2d::ExitEntity( IEntity2d* entity )
        {
            if( entity == NULL )
                return;

            if( GetFocusEntity() == entity )
                m_Scene->SetFocusElement( NULL );

            entity->BreakInteraction();

            if( strcmp( entity->ObjectName(), _T( "Building2d" ) ) == 0 )
            {
                int cellID = GetZoneId( entity->GetPos() );
                if( cellID != -1 )
                {
                    Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );
                    if( zone )
                    {
                        zone->ExitBuilding( ( ( GSO::World::Building2d* )entity ) );
                    }
                }
                else
                {
                    assert( 0 && "Can't find zone of building!" );
                }

                //return;
            }

            entity->ClearRoute();
            m_Scene->RemoveElement( entity );
            entity->SetWorld( NULL );
            // 如果有父節點 也把他從父節點給移除
            if( entity->GetParent() != NULL )
                entity->GetParent()->DetachChild( entity );

        }

        //==========================================================================================
        // NAME:
        // DESC: 
        //==========================================================================================
        size32_t Terrain2d::GetEntityCount( void )
        {
            return m_Scene->ElementCount();
        }

        //==========================================================================================
        // NAME:
        // DESC: 
        //==========================================================================================
        void Terrain2d::ForAllEntity( OnEntity udels, void* pData )
        {
            return m_Scene->ForAllElement( udels, pData );
        }



        /*
        //==========================================================================================
        // NAME: JoinRole()
        // DESC: Plant a role into this world
        //==========================================================================================
        void Terrain2d::JoinRole(Role2d* role)
        {
            if (role == 0)
                return;
            //role->m_Type = GSO::World::kTypeChar;
            role->SetWorld(this);
            m_Scene->InsertElement(role);
        }


        //==========================================================================================
        // NAME: DepartRole()
        // DESC: Eradicate a role form this world
        //==========================================================================================
        void Terrain2d::ExitRole(Role2d* role)
        {
            if (GetFocusEntity() == role)
                SetFocusElement(NULL);

            m_Scene->RemoveElement(role);
            role->SetWorld(NULL);
        }

        //==========================================================================================
        // NAME: JoinBuilding()
        // DESC: Plant a element into this world
        //==========================================================================================
        void Terrain2d::JoinBuilding(Building2d* bld, const GameVertex& mapPos)
        {
            if (bld == 0)
            {
                return;
            }

            //bld->m_Type = GSO::World::kTypeBld;
            int cellID = GetZoneId(mapPos);
            if (cellID == -1)
            {
                return;
            }
            else
            {
                Zone2d* zone = reinterpret_cast<Zone2d*>(GetCellData(cellID));

                if (!zone)
                {
                    return;
                }

                //bld->m_Type = GSO::World::kTypeBld;
                bld->SetWorld(this);
                zone->JoinBuilding(bld);

                m_Scene->InsertElement(bld);
            }
        }


        //==========================================================================================
        // NAME: DepartBuilding()
        // DESC: Eradicate a element form this world
        //==========================================================================================
        void Terrain2d::ExitBuilding(Building2d* bld)
        {
            if (bld == 0)
            {
                return;
            }

            if (GetFocusEntity() == bld)
            {
                SetFocusElement(0);
            }
            bld->SetWorld(0);
            m_Scene->RemoveElement(bld);


            int cellID = GetZoneId(bld->GetPos());
            if (cellID == -1)
            {
                return;
            }
            else
            {
                Zone2d* zone = reinterpret_cast<Zone2d*>(GetCellData(cellID));
                if (zone)
                {
                    zone->ExitBuilding(bld);
                }
            }
        }
        */



        //==========================================================================================
        // NAME: Object_Moving()
        // DESC: all role idle
        //==========================================================================================
        void Terrain2d::Update()
        {
            for( uint_t i = 0; i < m_RecycleBin.size(); i++ )
            {
                //ExitEntity(m_RecycleBin[i]->m_Descriptor);
                SAFE_DELETE( m_RecycleBin[ i ] );
            }
            m_RecycleBin.clear();

            if( m_Streaming )
            {
                //UpdateCenter(GetZoneId(m_GridConverter->ScreenToMapSite(m_Camera.GetPosition())));
                UpdateCenter( GetZoneId( m_Camera.GetPosition() ) );
            }
            else
            {
                // We don't do streaming, force it to load all zones here.
                //UpdateCenter(0);
            }

            for( uint_t j = 0; j < m_Scene->ElementCount(); j++ )
            {
                //GameVertex BeforeSitus = MapToChessboardSite(m_Temp_Object[j]->Map_Situs());
                //將目前所在位置格設為0
                //m_MapObject(BeforeSitus.x, BeforeSitus.y) = 0;
                //Set_Board_Grid(GATEWAY_BOARD, BeforeSitus.x, BeforeSitus.y, true);
                //((IEntity2d*) operator[](j))->Update();
                IEntity2d* ie2 = ( IEntity2d* )( *this )[ j ];
                if( ie2 )
                    ie2->Update();

                //將目前所在位置
                /*
                        CPRect  MapRange = m_Temp_Object[j]->Map_Rect();

                        GameVertex NowSitus = MapToChessboardSite(m_Temp_Object[j]->Map_Situs());
                        GameVertex Range0 = MapToChessboardSite(MapRange.p_l_t());
                        GameVertex Range1 = MapToChessboardSite(MapRange.p_r_t());
                        GameVertex Range2 = MapToChessboardSite(MapRange.p_r_b());
                        GameVertex Range3 = MapToChessboardSite(MapRange.p_l_b());

                        //將目前所在位置格設入
                        for(int h = Range0.y; h <= Range2.y; h++)
                        {
                            for(int w = Range3.x; w <= Range1.x; w++)
                            {
                                 if(m_Temp_Object[j]->Is_In_Area(Chessboard_to_Screen_Site(GameVertex(w, h))))
                                     m_MapObject(w, h) = m_Temp_Object[j];

                            }
                        }
                        //m_MapObject(NowSitus.x, NowSitus.y) = m_Temp_Object[j];


                        //將此格設為不可走
                        //Set_Board_Grid(GATEWAY_BOARD, NowSitus.x, NowSitus.y, false);
                        */
            }
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        void Terrain2d::ForceUpdate( void )
        {
            for( uint_t i = 0; i < m_RecycleBin.size(); i++ )
            {
                //ExitEntity(m_RecycleBin[i]->m_Descriptor);
                SAFE_DELETE( m_RecycleBin[ i ] );
            }
            m_RecycleBin.clear();


            if( m_Streaming )
            {
                ForceUpdateCenter( GetZoneId( m_Camera.GetPosition() ) );
            }
            else
            {
                // We don't do streaming, force it to load all zones here.
                UpdateCenter( 0 );
            }

            for( uint_t j = 0; j < m_Scene->ElementCount(); j++ )
            {
                IEntity2d* ie2 = ( IEntity2d* )( *this )[ j ];
                if( ie2 )
                    ie2->Update();
            }
        }

        //==========================================================================================
        // NAME:
        // DESC:
        //==========================================================================================
        void Terrain2d::SetArea( tchar_t** array, int id )
        {
            for( uint_t i = 0; i < m_GridConverter->GetRows(); i++ )
                for( uint_t j = 0; j < m_GridConverter->GetCols(); j++ )
                    array[ i ][ j ] = id;
        }

        /*
        ////////////////////////////////////////////////////////////////////////////////////////////
        //
        ////////////////////////////////////////////////////////////////////////////////////////////
        CPRect Terrain2d::MapRect(void)
        {
            CPRect maprect(m_Situs.x, m_Situs.y,
                            m_Situs.x + Scene_Width(),
                            m_Situs.y + Scene_Height());
            return maprect;
        }
        */
        //========================================================================================
        // NAME:
        // DESC:
        //========================================================================================
        bool Terrain2d::InsertZone( StreamCellDescriptor scdp, zoneid_t id )
        {
            if( GetCellCount() > ( m_Tiles.cx*m_Tiles.cy ) )
                return false;

            if( scdp == NULL )
            {
                return false;
                /*
                if(((CPGspParser*)m_ConfigureIO)->GetChildrenCount() == 0)
                    return false;

                tchar_t buff[255];
                GspNode* node = new GspNode("2DZONE");
                (*((CPGspParser*)m_ConfigureIO))[0]->AttachChild(node);
                node->WriteCommand("ID", "=", itoa(id, buff, 10));
                //node->WriteCommand("LOADRES", "=" , path);

                std::string gdFile = Sprite2d::GetCurrentDirectory();
                gdFile += "/Terrain/";
                gdFile += GetName();
                std::string evFile = Sprite2d::GetCurrentDirectory();
                evFile += "/Terrain/";
                evFile += GetName();

                if (id < 10)
                {
                    sprintf(buff, "ZGD0%d.wlk", id);
                    gdFile += buff;
                    sprintf(buff, "ZGD0%d.evt", id);
                    evFile += buff;
                }
                else
                {
                    sprintf(buff, "ZGD%d.wlk", id);
                    gdFile += buff;
                    sprintf(buff, "ZGD%d.evt", id);
                    evFile += buff;
                }

                node->WriteCommand("GDWALK", "=", gdFile.c_str());
                node->WriteCommand("GDEVENT", "=", evFile.c_str());
                scdp = node;
                */
            }



            /*
            tchar_t* descriptor = (tchar_t*)scdp;
            mcurrentpath(descriptor);
            //pfstd::String fname = descriptor;
            tchar_t* fname = new tchar_t[strlen(descriptor)+1];
            strcpy(fname, descriptor);

            //ZoneDescriptor* pDescriptor = new ZoneDescriptor;
            m_Descriptorlist.push_back(fname);
            */




            // Default BG Description
            /*
            std::string gdFile = "Terrain/";
            gdFile += GetName();
            std::string evFile = "Terrain/";
            evFile += GetName();
            std::string dscFile = "Terrain/";
            dscFile += GetName();
            tchar_t buff[32];

            if (Id < 10)
            {
                sprintf(buff, "ZGD0%d.wlk", Id);
                gdFile += buff;
                sprintf(buff, "ZGD0%d.evt", Id);
                evFile += buff;
                sprintf(buff, "Zone0%d.zsp", Id);
                dscFile += buff;
            }
            else
            {
                sprintf(buff, "ZGD%d.wlk", Id);
                gdFile += buff;
                sprintf(buff, "ZGD%d.evt", Id);
                evFile += buff;
                sprintf(buff, "Zone%d.zsp", Id);
                dscFile += buff;
            }
            // Default GD Description
            pDescriptor->SetDescription(gdFile.c_str(),
                                         ZoneDescriptor::kDscrptGDGateWay);
            // Default GD Description
            pDescriptor->SetDescription(evFile.c_str(),
                                         ZoneDescriptor::kDscrptGDEvent);
            // Default Description File
            pDescriptor->SetDescription(dscFile.c_str(),
                                         ZoneDescriptor::kDscrptFile);
                                         */
                                         /*
                                         std::string dscFile = "Terrain/";
                                         dscFile += GetName();
                                         tchar_t buff[32];
                                         if (Id < 10)
                                         {
                                             sprintf(buff, "Zone0%d.zsp", Id);
                                             dscFile += buff;
                                         }
                                         else
                                         {
                                             sprintf(buff, "Zone%d.zsp", Id);
                                             dscFile += buff;
                                         }
                                         */

                                         /*
                                         pDescriptor->SetDescription(dscFile.c_str(),
                                                                      ZoneDescriptor::kDscrptFile);
                                                                      */

            if( ( ( CPGspParser* )m_ConfigureIO )->GetChildrenCount() == 0 )
                return false;

            GspNode* node = ( GspNode* )scdp;
            ( *( ( CPGspParser* )m_ConfigureIO ) )[ 0 ]->AttachChildNode( node );

            return DataStream::InsertCell( scdp, id );
        }

        //========================================================================================
        // NAME: CurrentZone()
        // DESC: 
        //========================================================================================
        Zone2d* Terrain2d::CurrentZone()
        {
            if( m_Streaming )
            {
                return reinterpret_cast< Zone2d* >( GetCellData( GetFocusedCellID() ) );
            }
            else
            {
                return 0;
            }
        }

        //========================================================================================
        // NAME: 
        // DESC: 
        //========================================================================================
        Zone2d* Terrain2d::FineZone( zoneid_t id )
        {
            return ( Zone2d* )GetCellData( id );
        }

        //========================================================================================
        // NAME: GetZone()
        // DESC: 
        //========================================================================================
        zoneid_t Terrain2d::GetZoneId( const GameVertex& pos )
        {
            int id = 0XFFFFFFFF;
            switch( m_GridConverter->GetGirdShape() )
            {
            case _P_RECT_:
                {
                    id = ( ( pos.x / m_GridConverter->GetWidth() ) * 10000 ) + ( pos.y / m_GridConverter->GetHeight() );
                }
                break;
                // TODO: Pin 
                // 菱形格判斷 zone的演算法
            case _P_RHOMBUS_:
                {
                    id = 0;
                }
                break;
            }

            /*
            LG::Point offset;
            offset.x = pos.x - m_Origin.x;
            offset.y = pos.y - m_Origin.y;

            if(m_GridConverter->GetGirdShape() == _P_RECT_)
            {
                if (offset.x <0 ||
                     offset.y <0 ||
                     offset.x> m_GridConverter->GetWidth()*m_Tiles.cx ||
                     offset.y> m_GridConverter->GetHeight()*m_Tiles.cy)
                {
                    // It's outside the scope of the landscape
                    return id;
                }

                int col = (offset.x / m_GridConverter->GetWidth());
                int row = (offset.y / m_GridConverter->GetHeight());

                id = (row * m_Tiles.cx) + col;
            }
            */
            return id;
        }

        //========================================================================================
        // NAME: GetZone()
        // DESC: 
        //========================================================================================
        Zone2d* Terrain2d::GetZone( const GameVertex& mapPos )
        {
            int cellID = GetZoneId( mapPos );
            if( cellID == -1 )
            {
                return 0;
            }
            else
            {
                Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );
                return zone;
            }
        }

        //========================================================================================
        // NAME: GetZone()
        // DESC: 
        //========================================================================================
        void Terrain2d::GetLoadedZone( std::vector<Zone2d*>& loadedZones )
        {
            std::vector<int> loadedIDs;
            GetLoadedCellIDs( loadedIDs );

            std::vector<int>::iterator iID = loadedIDs.begin();
            for( ; iID != loadedIDs.end(); ++iID )
            {
                loadedZones.push_back( reinterpret_cast< Zone2d* >( GetCellData( *iID ) ) );
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================

#define _MAP_RECT_LIMIT_ 100

        void Terrain2d::Collide_Scroll( POINT point )
        {
            /*
                        if(m_pRender->GetWidget() == NULL)
                            return;
                        GameVertex mouse_pt;
                        GetCursorPos(&mouse_pt);
                        ScreenToClient((HWND) m_pRender->GetWidget(), &mouse_pt);
                        LG::Rect client_rect;
                        GetClientRect((HWND) m_pRender->GetWidget(), &client_rect);
                        if(PA::Intersect::Test(&client_rect, &mouse_pt))
                        {
                            if(point.x < _MAP_RECT_LIMIT_)
                            {
                                Zone2d* currZone = CurrentZone();
                                if(currZone && Sprite2d::GetOrigin().x < 0)
                                {
                                    GameVertex spt (Sprite2d::GetOrigin().x + 10,
                                                  Sprite2d::GetOrigin().y);
                                    currZone->GetPos() = spt;
                                    point.x = _MAP_RECT_LIMIT_;
                                }
                            }else if(point.x > m_pRender->GetWidth() - _MAP_RECT_LIMIT_)
                            {
                                  Zone2d* currZone = CurrentZone();
                                  if(currZone && Sprite2d::GetOrigin().x > (m_pRender->GetWidth() - currZone->ImageWidth()))
                                  {
                                    GameVertex spt (Sprite2d::GetOrigin().x - 10,
                                                  Sprite2d::GetOrigin().y);
                                    currZone->GetPos() = spt;
                                    point.x = m_pRender->GetWidth() - _MAP_RECT_LIMIT_;
                                }
                            }
                            if(point.y < _MAP_RECT_LIMIT_)
                            {
                                Zone2d* currZone = CurrentZone();
                                if(currZone && Sprite2d::GetOrigin().y < 0)
                                {
                                    GameVertex spt (Sprite2d::GetOrigin().x,
                                                  Sprite2d::GetOrigin().y + 10);
                                    currZone->GetPos() = spt;
                                    point.y = _MAP_RECT_LIMIT_;
                                }
                            }else if(point.y >
                                      (m_pRender->GetHeight() - _MAP_RECT_LIMIT_))
                            {
                                Zone2d* currZone = CurrentZone();
                                if(currZone && Sprite2d::GetOrigin().y > (m_pRender->GetHeight() - currZone->ImageHeight()))
                                {
                                    GameVertex spt (Sprite2d::GetOrigin().x,
                                                  Sprite2d::GetOrigin().y - 10);
                                    currZone->GetPos() = spt;
                                    point.y = m_pRender->GetHeight() - _MAP_RECT_LIMIT_;
                                }
                            }
                            ClientToScreen((HWND) m_pRender->GetWidget(), &point);
                            SetCursorPos(point.x, point.y);
                        }
                        return;
            */
        }

        //=============================================================================
        //
        //
        //=============================================================================
        //int m_ScrollSpeed;
        void Terrain2d::Query( GameVertex point, uint_t uMsg, uint_t wParam )
        {
            switch( uMsg )
            {
            case WM_MOUSEMOVE:
                //==deviec situs=================
                //m_Mouse_Map_Site = ScreenToMapSite(point);
                m_Mouse_Map_Site = m_GridConverter->ScreenToMapSite( point );
                //格子座標
                //m_Mouse_Chessboard_Site = MapToChessboardSite(m_Mouse_Map_Site);
                m_Mouse_Chessboard_Site = m_GridConverter->MapToChessboardSite( m_Mouse_Map_Site );
                //===============================
                break;
            }

            Collide_Scroll( point );

            return;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::SelectTrigger( const GameVertex mapPos, std::vector<const Trigger*>& TriggerList )
        {
            int cellID = GetZoneId( mapPos );
            if( cellID == -1 )
            {
                return;
            }
            else
            {
                Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );

                if( zone )
                    zone->GetTrigger( mapPos, TriggerList );
            }
        }

        //=============================================================================
        //
        //
        //=============================================================================
        bool Terrain2d::InsertTrigger( Trigger* pTrigger )
        {
            int cellID = GetZoneId( ( pTrigger->range.p_l_t() + pTrigger->range.p_r_b() ) / 2 );
            if( cellID != -1 )
            {
                Zone2d* zone = reinterpret_cast< Zone2d* >( GetCellData( cellID ) );
                if( zone )
                {
                    zone->InsertTrigger( pTrigger );
                    return true;
                }
            }
            return false;
        }

        //=============================================================================
        // NAME: RemoveTrigger()
        // DESC: 
        //=============================================================================
        bool Terrain2d::RemoveTrigger( Trigger* pTrigger )
        {
            std::vector<int> loadedIDs;
            GetLoadedCellIDs( loadedIDs );

            std::vector<int>::iterator iID = loadedIDs.begin();
            for( ; iID != loadedIDs.end(); ++iID )
            {
                if( reinterpret_cast< Zone2d* >( GetCellData( *iID ) )->RemoveTrigger( pTrigger ) )
                {
                    return true;
                }
            }

            return false;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::GetTriggerList( std::vector<Trigger*>& TriggerList )
        {
            std::vector<int> loadedIDs;
            GetLoadedCellIDs( loadedIDs );

            std::vector<int>::iterator iID = loadedIDs.begin();
            for( ; iID != loadedIDs.end(); ++iID )
            {
                reinterpret_cast< Zone2d* >( GetCellData( *iID ) )->GetTriggerList( TriggerList );
            }
        }

        //=============================================================================
        // NAME: SetPos()
        // DESC: 
        //=============================================================================
        void Terrain2d::SetPos( const LG::Point& pos )
        {
            Zone2d* currZone = CurrentZone();
            if( currZone )
            {
                currZone->GetPos() = pos;
            }
        }

        //=============================================================================
        // NAME:
        // DESC: 
        //=============================================================================
        void Terrain2d::PushToRecycleBin( IEntity2d* entity )
        {
            m_RecycleBin.push_back( entity );
        }

        //=============================================================================
        // NAME: LoadScript()
        // DESC: Load script language file
        //=============================================================================
        bool LoadAllCellCallBack( cellid_t id, GSO::StreamCellDescriptor desc, uint_t order, void* pDefData )
        {
            Terrain2d* terrain = ( Terrain2d* )pDefData;
            void* pData = terrain->GetCellData( id );
            void* pUserData = terrain->GetCellUserData( id );
            terrain->OnLoadCell( id, desc, &pData, &pUserData );
            return true;
        }

        int Terrain2d::LoadCfg( const tchar_t* path, bool streaming )
        {
            Destroy();

            m_Streaming = streaming;

            //CTerrainConfigure reader(this);
            //int result =  reader.Load(path);
            CPGspParser* loader = ( CPGspParser* )m_ConfigureIO;
            int result = loader->Load( path );

            GspTerrainConfigure reader( this );
            loader->DoPass( &reader );

            //loader->Save(path);

            if( result )
            {
                SetReleaseTime( 15000 );
                Update();
            }


            if( !m_Streaming )
            {
                SetReleaseTime( TIMER_STOP );
                UpdateAllCell();
                /*
                // We don't do streaming, force it to load all zones here.
                //ForAllCell(LoadAllCellCallBack, this);
                for(uint_t y=0; y<m_Tiles.cy; y++)
                {
                    for(uint_t x=0; x<m_Tiles.cx; x++)
                    {
                        UpdateCenter(0);
                    }
                }
                */
            }

            m_filename = path;
            return result;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        bool GetCellFunc( int id, uint_t order, void* pData )
        {
            std::vector<int>* allzones = ( std::vector<int>* )pData;
            allzones->push_back( id );
            return true;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::SaveCfg( const tchar_t* path )
        {
            m_filename = path;
            //CPG2Scene::Save(path, (Element2d*)this, getUnits());
            CPGspParser* newwriter = ( CPGspParser* )m_ConfigureIO;
            newwriter->Save( path );
            return;

            /*
            CPGspParser writer;

            tchar_t buf[255];

            writer.Open(path);

            writer.CreateObject("2DTERRAIN");
            sprintf(buf,
                     "%d,%d,%d,%d,%d,%d,%d,%d,%d",
                     m_Origin.x,
                     m_Origin.y,
                     m_Tiles.cx,
                     m_Tiles.cy,
                     m_GridConverter->GetWidth(),
                     m_GridConverter->GetHeight(),
                     m_GridConverter->GetGirdWidth(),
                     m_GridConverter->GetGirdHeight(),
                     m_GridConverter->GetGirdShape());

            // Note! The sequence of NAME and SEGMENT should not exchange
            writer.SetReadAttribute("NAME", GetName().c_str());
            writer.SetReadAttribute("SEGMENT", buf);

            std::vector<int> allzones;
            ForAllCell(GetCellFunc, &allzones);
            for (uint_t z = 0; z < allzones.size(); z++)
            {
                // Create Obj
                writer.CreateObject("2DZONE");

                // Attribute ID
                writer.SetReadAttribute("ID",
                                         itoa(allzones[z], buf, 10));

                // Attribute LOAD
                //tchar_t* name = allzones[z]->m_pDescriptor->GetDescription(ZoneDescriptor::kDscrptFile);
                //zd = GetCellDescriptor(allzones[z]);
                writer.SetReadAttribute("LOAD", (tchar_t*)GetCellDescriptor(allzones[z]));
                // Close Obj
                writer.CloseObject();

                Zone2d* zone = reinterpret_cast<Zone2d*>(GetCellData(allzones[z]));
                if (zone)
                {
                    zone->SaveZone(zone->GetConfigfname());
                    zone->ClearBuildings();
                }
             }
            allzones.clear();

            // TODO: Alex, Disable this feature in this project.
/*
            for (uint_t e = 0; e < m_Scene->ElementCount(); e++)
            {
                GSO::World::Role2d* role = Role(e);

                if (role->m_Type == GSO::World::kTypeBld)
                {
                    break;
                }

                writer.CreateObject("2DROLE");
                writer.SetReadAttribute("X",
                                         itoa(role->GetPos().x, buf, 10));
                writer.SetReadAttribute("Y",
                                         itoa(role->GetPos().y, buf, 10));

                tchar_t* name = (tchar_t*) role->GetFileName();
                if (role->GetCurrentDirectory() != NULL)
                {
                    tchar_t* s;
                    s = strstr(name, role->GetCurrentDirectory());
                    if (s != NULL)
                    {
                        name = s;
                        name += (strlen(role->GetCurrentDirectory()) + 1);
                    }
                }
                writer.SetReadAttribute("LOAD", name);

                sprintf(buf,
                         "%s,%d",
                         role->GetDataBaseName(),
                         role->GetDataBaseEntry());
                writer.SetReadAttribute("DATABASE", buf);
                writer.CloseObject();
            }
*/


//writer.CloseObject();

//writer.Close();
        }


        //=============================================================================
        //
        //
        //=============================================================================
        const tchar_t* Terrain2d::GetFilename( void )
        {
            return m_filename.c_str();
        }


        //=============================================================================
        //
        //
        //=============================================================================
        bool Terrain2d::IDExceed( int id )
        {
            if( id < 0 || id >= ( m_Tiles.cx * m_Tiles.cy ) )
            {
                return true;
            }
            return false;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        bool Terrain2d::IsValidNeighbor( int id, int neighborID )
        {
            if( ( ( id + 1 ) % m_Tiles.cx ) == 1 ) // id on left border
            {
                if( ( ( neighborID + 1 ) % m_Tiles.cx ) == 0 )
                {
                    return false;
                }
            }
            else if( ( ( id + 1 ) % m_Tiles.cx ) == 0 ) // id on right border
            {
                if( ( ( neighborID + 1 ) % m_Tiles.cx ) == 1 )
                {
                    return false;
                }
            }

            return true;
        }


        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::OnDestroyDescriptor( StreamCellDescriptor descriptor )
        {
            //tchar_t* fname = (tchar_t*)descriptor;
            //descriptor=NULL;
            //delete [] fname;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::OnGetNeighbor( int Id, std::vector<int>& neighbor )
        {
            /*
            if (!m_Streaming)
            {
                if(Id == 0)
                {
                    for (int i = 0; i < m_Tiles.cx; ++i)
                    {
                        for (int j = 0; j < m_Tiles.cy; ++j)
                        {
                            //int nID = i * m_Tiles.cx + j;
                            int nID = (i * 10000) + j;
                            if(Id != nID)
                            {
                                neighbor.push_back(nID);
                            }
                        }
                    }
                }
            }
            else
            */
            //if (m_Streaming)
            //{
                //assert((m_Tiles.cx && m_Tiles.cx == m_Tiles.cy) &&
                //        "Terrain2d::OnGetNeighbor() -- Currently the algorithm only supports squared terrain!");
                /*
                if (m_Tiles.cx == 1)
                {
                    return;
                }
                else if (m_Tiles.cx == 2)
                {
                    int ids[4] =
                    {
                        0, 1, 2, 3
                    };
                    for (int i = 0; i < 4; ++i)
                    {
                        if (ids[i] != Id)
                        {
                            neighbor.push_back(ids[i]);
                        }
                    }
                }
                */
                //else // Tile counts >= 3
            int neighborIDs[ 8 ] =
            {
                Id - 10001, // Left-Top
                Id - 1,     // Top
                Id + 9999, // Right-Top
                Id - 10000,                // Left
                Id + 10000,                // Right
                Id - 9999, // Bottom-Left
                Id + 1,     // Bottom
                Id + 10001  // Bottom-Right
            };

            for( int i = 0; i < 8; ++i )
            {
                if( neighborIDs[ i ] >= 0 )
                    neighbor.push_back( neighborIDs[ i ] );
            }

            /*
            {
                int neighborIDs[8] =
                {
                    Id - (m_Tiles.cx + 1), // Left-Top
                    Id - (m_Tiles.cx),     // Top
                    Id - (m_Tiles.cx - 1), // Right-Top
                    Id - (1),                // Left
                    Id + (1),                // Right
                    Id + (m_Tiles.cx - 1), // Bottom-Left
                    Id + (m_Tiles.cx),     // Bottom
                    Id + (m_Tiles.cx + 1)  // Bottom-Right
                };

                for (int i = 0; i < 8; ++i)
                {
                    if (!IDExceed(neighborIDs[i]))
                    {
                        if (IsValidNeighbor(Id, neighborIDs[i]))
                        {
                            if((Id>=0) &&
                                (Id<(m_Tiles.cx * m_Tiles.cy)) &&
                                (Id!=neighborIDs[i]))
                                neighbor.push_back(neighborIDs[i]);
                        }
                    }
                }
            }
            */
            //}
        }
        //=============================================================================
        //
        //
        //=============================================================================
        void* Terrain2d::OnLoadCell( int Id,
            StreamCellDescriptor CellDescriptor, //const tchar_t* filename,
            void** pData,
            void** pUserdata )
        {
            Zone2d* zone = new Zone2d( this, Id );

            /*
            zone->Board_Release_All();
            zone->board_list_[GATEWAY_BOARD] = zone->InsertBoard("Walkable.board",
                                                                  m_GridConverter->GetCols(),
                                                                  m_GridConverter->GetRows());
            zone->board_list_[EVENT_BOARD] = zone->InsertBoard("Event.board",
                                                                m_GridConverter->GetCols(),
                                                                m_GridConverter->GetRows());
                                                                */

            ( *pData ) = zone;
            //pfstd::String* fname = (pfstd::String*)CellDescriptor;
            zone->LoadZone( CellDescriptor );

            /*
            int idxCol, idxRow;
            idxCol = Id % m_Tiles.cx;
            idxRow = Id / m_Tiles.cx;

            zone->SetPos(LG::Point(m_Origin.x + (idxCol * m_GridConverter->GetWidth()),
                                     m_Origin.y + (idxRow * m_GridConverter->GetHeight())));
                                     */

            return zone;
        }

        //=============================================================================
        //
        //
        //=============================================================================
        void Terrain2d::OnReleaseCell( int Id,
            StreamCellDescriptor CellDescriptor, //const tchar_t* filename,
            void* pData,
            void* pUserdata )
        {
            Zone2d* zone = reinterpret_cast< Zone2d* >( pData );
            // TODO: Sean
            // 這裡有的時候會當掉，原因是因為傳進來的zone居然會是0
            // 所以，我這裡加了這行
            if( zone == 0 )
                return;

            const std::vector<Building2d*>& buildings = zone->GetBuildings();
            for( uint_t i = 0; i < buildings.size(); i++ )
            {
                Building2d* build = buildings[ i ];
                if( build == GetFocusEntity() )
                    SetFocusEntity( NULL );
                // TODO: Alex, in editor, there are chances we focus on bld, will it cause problem if we don't SetFocus(0) ?
                m_Scene->RemoveElement( build );
                SAFE_DELETE( build );
            }

            /*
            std::vector<Building2d*>::const_iterator iBld = buildings.begin();
            for (; iBld != buildings.end(); ++iBld)
            {
                // TODO: Alex, in editor, there are chances we focus on bld, will it cause problem if we don't SetFocus(0) ?
                m_Scene->RemoveElement((*iBld));
                delete (*iBld);
            }
            */
            zone->ClearBuildings();
            delete zone;
        }


        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetGirdWidth( void )
        {
            return m_GridConverter->GetGirdWidth();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetGirdHeight( void )
        {
            return m_GridConverter->GetGirdHeight();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetGirdShape( void )
        {
            return m_GridConverter->GetGirdShape();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetTileCols( void )
        {
            return m_GridConverter->GetCols();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetTileRows( void )
        {
            return m_GridConverter->GetRows();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetTileWidth( void )
        {
            return m_GridConverter->GetWidth();
        }

        //=============================================================================
        //
        //
        //=============================================================================
        uint_t Terrain2d::GetTileHeight( void )
        {
            return m_GridConverter->GetHeight();
        }


        //=============================================================================
        //
        //
        //=============================================================================
        /*
        void Terrain2d::SetZoneImageFile(int zoneID, const tchar_t* path)
        {
            // TODO: Alex, find zone data file by ID
            //             open it or create one
            //             write elemFile into zone data file
            //             close it
            //             InsertCell(zone data file) to cause it reload
            //             or just let Create to create one to ensure the file would exist
            // InsertZone(fileName, zoneID);
            //StreamCellDescriptor pDescriptor = GetCellDescriptor(zoneID);
            StreamCellDescriptor pDescriptor = GetCellDescriptor(zoneID);
            if (!pDescriptor.empty())
            {
                tchar_t fileName[256];
                sprintf(fileName, path);
                tchar_t* name = fileName;
                const tchar_t* currDir = Sprite2d::GetCurrentDirectory();
                if (currDir)
                {
                    tchar_t* s = strstr(name, currDir);
                    if (s != NULL)
                    {
                        name = s;
                        name += (strlen(currDir));
                    }
                }

                //pDescriptor->SetDescription(name, ZoneDescriptor::kDscrptBG);
                mcurrentpath((tchar_t*)name);
                DataStream::InsertCell(name, zoneID);
            }
        }
        */
        /*
                //=============================================================================
                //
                //
                //=============================================================================
                void Terrain2d::SetGridFile(int zoneID, const tchar_t* fileName)
                {
                    tchar_t path[256];
                    strcpy(path, fileName);
                    StreamCellDescriptor* pDescriptor = GetCellDescriptor(zoneID);

                    if(pDescriptor)
                    {
                        tchar_t* fName = 0;
                        if(Sprite2d::GetCurrentDirectory()!= NULL)
                        {
                            fName = strstr(path, Sprite2d::GetCurrentDirectory());
                            if(fName !=NULL )
                            {
                                fName = path;
                                fName+= (strlen(Sprite2d::GetCurrentDirectory()) + 1);
                            }
                        }
                        pDescriptor->SetDescription(ZoneDescriptor::kDscrptGDGateWay, fName);
                        DataStream::InsertCell(pDescriptor, zoneID);
                    }
                }
        */
    };//World
};//GSO


