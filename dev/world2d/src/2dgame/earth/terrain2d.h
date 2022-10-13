//Map.h header file of map.cpp
//
// Map Class
//
//
//
//             map tchar_t have inside tchar_t and outside tchar_t, 
//             inside tchar_t now have define grid middle point tchar_t, floor tchar_t
//             outside tchar_t read include grid structure tchar_t
//
//
//
//
//
//                                                         Pin
//

#ifndef TERRAIN2D_H
#define TERRAIN2D_H

#pragma warning (disable : 4786)

//map object
#include "role2d.h"
#include "building2d.h"
#include "zone2d.h"
#include "camera2d.h"
#include <vmc-render2d.h>
#include <data_stream.h>
//#include <VMC.h>



typedef cellid_t zoneid_t;

typedef bool ( _CALLBACK *OnEntity )( unsigned int order, void* pEntity, void* pData );
typedef bool ( _CALLBACK *OnZone )( uint_t order, const GSO::World::Zone2d* zone, void* pDefData );


// Forward Declarations
class CPGspParser;


namespace GSO
{
    class Scene2d;

    namespace World
    {
        class Topog2d;

        struct Trigger
        {
            shape_t    shape;
            LG::Rect   range;
            int        type;
            GameVertex pos;
            tchar_t    data[ 256 ];
        };


        ///////////////////////////////////////////////////////////////////////////////////
        //  Map class
        class Terrain2d: public DataStream//, public Scene2d//, public LGB::DrawNode
        {
        public:

            Terrain2d();
            virtual ~Terrain2d();


            // 設定地圖的名子
            void                SetMapName( const tchar_t* name );
            const tchar_t*      GetMapName( void );

            // 設定預設擺放walk table的資料夾名稱
            void                SetWalkFolder( const tchar_t* name );
            const tchar_t*      GetWalkFolder( void );

            // 設定預設擺放event table的資料夾名稱
            void                SetEventFolder( const tchar_t* name );
            const tchar_t*      GetEventFolder( void );

            bool                Create( const LG::Point& origin,
                uint_t tileCountX, uint_t tileCountY, //Zone 的數量
                uint_t tileWidthX, uint_t tileWidthY, //Zone 的長寬
                int  gridWidth, int gridHeight, shape_t gridType );

            LG::Rect            GetTerritory( void );

            void                Destroy( void );

            bool                InsertZone( StreamCellDescriptor scdp, zoneid_t id );
            Zone2d*             CurrentZone();
            Zone2d*             FineZone( zoneid_t id );
            Zone2d*             GetZone( const GameVertex& mapPos );
            zoneid_t            GetZoneId( const GameVertex& mapPos );
            void                GetLoadedZone( std::vector<Zone2d*>& loadedZones );

            void                SetPos( const LG::Point& pos );
            void                SetScreenSize( const GameVertex& screenSize );

            //file
            int                 LoadCfg( const tchar_t* path, bool streaming = true );
            void                SaveCfg( const tchar_t* path );

            const tchar_t*      GetFilename( void );

            /////////////////////////////////////////////////
            // 取得entity
            IEntity2d*          operator[] ( uint_t i );
            Role2d*             Role( int idx );

            IEntity2d*          GetEntityByID( entityid_t id );
            IEntity2d*          GetEntityByName( const tchar_t* characterName );
            entityid_t          GetEntityIDByName( const tchar_t* characterName );

            // 以影像來pick
            virtual IEntity2d*     GetEntityBySel( const tchar_t* objname, const GameVertex& mapPos, bool ignorefocus = false );
            virtual void         GetEntityBySel( const tchar_t* objname, const GameVertex& mapPos, std::vector<IEntity2d*>& picklist );

            // 以碰撞區塊來pick
            virtual IEntity2d*  GetEntityByCD( const tchar_t* objname, const GameVertex& mapPos );

            virtual IEntity2d*  GetTypeEntityById( const tchar_t* objname, entityid_t eid );

            bool                IsEntityEmpty( void );

            //////////////////////////////////////////////////
            //
            IEntity2d*            GetFocusEntity( void );
            void                SetFocusEntity( IEntity2d* role );

            //////////////////////////////////////////////////
            // 
            //內部會依據 objectname 判斷是 building 或是role, 未來會取代下面四個function
            void                JoinEntity( IEntity2d* role, const GameVertex& mapPos );
            void                ExitEntity( IEntity2d* role );
            size32_t            GetEntityCount( void );
            void                ForAllEntity( OnEntity udels, void* pData );


            //動作
            void                Update( void );
            void                ForceUpdate( void );


            virtual void        DrawTo( LGB::VMCRender2d* render );

            //貼圖
            //void                SetRender(LGB::VMCRender2d* pRenderer) { m_pRenderer = pRenderer; }
            void                SetZoneDrawState( LGB::DrawNode::DrawMethod state );

            //
            virtual void        Query( GameVertex point, uint_t uMsg, uint_t wParam );     //捲動

            void                SelectTrigger( const GameVertex mapPos, std::vector<const Trigger*>& TriggerList );

            bool                InsertTrigger( Trigger* pTrigger );
            bool                RemoveTrigger( Trigger* pTrigger );


            void                GetTriggerList( std::vector<Trigger*>& TriggerList );


            //路徑
            void                SetArea( tchar_t** array, int id );

            void*               GetGridData( BoardStyle board, const GameVertex& mapPos );
            void                SetGridData( BoardStyle board, const GameVertex& mapPos, void* value );
            void*               GetGridData( Zone2d* resideZone, BoardStyle board, int gridX, int gridY );
            void                SetGridData( Zone2d* resideZone, BoardStyle board, int gridX, int gridY, void* value );

            GameVertex          ConvertGridToMap( const GameVertex& gridPos );
            GameVertex          ConvertMapToGrid( const GameVertex& mapPos );
            GameVertex          ConvertScreenToGrid( const GameVertex& screenPos );
            GameVertex          ConvertMapToScreen( const GameVertex& mapPos );
            GameVertex          ConvertScreenToMap( const GameVertex& screenPos );

            GameVertex          GridCenterPoint( Zone2d& resideZone, GameVertex GridSite );

            uint_t              GetMapCX( void )        { return m_MapSize.cx; }
            uint_t              GetMapCY( void )        { return m_MapSize.cy; }


            // 切割的最小單位格的值
            uint_t              GetGirdWidth( void );
            uint_t              GetGirdHeight( void );
            uint_t              GetGirdShape( void );

            uint_t              GetTileCols( void );
            uint_t              GetTileRows( void );
            uint_t              GetTileWidth( void );
            uint_t              GetTileHeight( void );

            uint_t              GetTilesWidth( void )   { return m_Tiles.cx; }
            uint_t              GetTilesHeight( void )  { return m_Tiles.cy; }

            void                PushToRecycleBin( IEntity2d* entity );

        public:

            virtual void        OnDestroyDescriptor( StreamCellDescriptor descriptor );

            virtual void        OnGetNeighbor( int Id, std::vector<int>& neighbor );
            virtual void*       OnLoadCell( int Id,
                StreamCellDescriptor CellDescriptor,
                void** pData,
                void** pUserdata );
            virtual void        OnReleaseCell( int Id,
                StreamCellDescriptor CellDescriptor,
                void* pData,
                void* pUserdata );

        public:

            GameVertex          m_Mouse_Map_Site;
            GameVertex          m_Mouse_Chessboard_Site;
            bool                m_bLoadRes;
            Camera2d            m_Camera;

        protected:
            virtual void        Collide_Scroll( POINT point );

        private:

            Scene2d*            m_Scene;
            Topog2d*            m_GridConverter;
            CPGspParser*        m_ConfigureIO;

            LG::Size            m_Tiles;
            LG::Size            m_MapSize;

            //LG::Point           m_Origin;
            std::string         m_filename;
            std::string         m_Name;
            std::string         m_WalkFolder;
            std::string         m_EventFolder;

            bool                      m_Streaming;
            LGB::DrawNode::DrawMethod m_ZoneDrawState;

        private:

            bool                IDExceed( int id );
            bool                IsValidNeighbor( int id, int neighborID );

            std::vector<IEntity2d*> m_RecycleBin;

        };
    };//World
};//GE

#endif
