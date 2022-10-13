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
#ifndef ZONE2D_H
#define ZONE2D_H

//#include <element2d.h>


#include <chessboard_def.h>
#include "ientity2d.h"
#include "vmc-render2d.h"
#include "pf-cstring.h"
//#include <tStringArray.h>


class CPChessboard;

enum BoardStyle{
    WALK_BOARD = 0,
    EVENT_BOARD = 1,
    MAX_STYLE = 2
};

namespace GSO
{
    namespace World
    {
        class  Terrain2d;
        class  Building2d;
        struct Trigger;

        class Zone2d: public IEntity2d//, public CPChessboard
        {

            pfstd::CString    zone_disp_;

        public:
            Zone2d( Terrain2d* pTerrain, uint_t id );
            virtual ~Zone2d();

            const tchar_t*   ObjectName() const { return _T( "Zone2d" ); }


            error_t          LoadBoard( uint_t iBoard, const tchar_t* filename );
            bool             IsBoardEmpty( void );
            void             SetBoardData( BoardStyle board, uint_t col, uint_t row, void* value = NULL );
            void*            GetBoardData( BoardStyle board, uint_t col, uint_t row );
                             
                             
                             
            void             JoinBuilding( Building2d* pBuilding );
            void             ExitBuilding( Building2d* pBuilding );
            void             ClearBuildings();
                             
            error_t          LoadZone( void* pCellDescriptor );
            error_t          SaveZone( void* pCellDescriptor );


            void             GetTrigger( const GameVertex mapPos, std::vector<const Trigger*>& TriggerList );
            void             InsertTrigger( Trigger* pTrigger );
            bool             RemoveTrigger( Trigger* pTrigger );
            void             ClearTriggers();
            void             GetTriggerList( std::vector<Trigger*>& events );


            pfstd::CString   GetWaklablefname( void );
            pfstd::CString   GetEventfname( void );

            // Interfaces of DrawNode
            virtual void     OnRenderTo( rendercontainer_t& container ){ Element2d::OnRenderTo( container ); }

            const std::vector<Building2d*>& GetBuildings() { return buildings_; }
            //virtual void OnRenderTo(LGB::VMCRender2d &renderer);

        public:



        private:

            //StreamCellDescriptor m_Descriptor;
            Terrain2d*                   p_terrain_;
            CPChessboard*                p_chessboard_;
            std::vector<Trigger*>        triggers_;
            std::vector<Building2d*>     buildings_;
            PBOARD                       board_list_[ MAX_STYLE ];

            //public:
                //void* m_CellDescriptor;
        };

    };
};


#endif //ZONE2D_H