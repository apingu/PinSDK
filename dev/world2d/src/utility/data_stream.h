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
#ifndef DATASTREAM_H
#define DATASTREAM_H


#pragma warning (disable : 4786)
#include "data_stream_def.h"
#include <graphic3d.h>
#include <vector>
#include <map>
#include <pf-timer.h>
#include <template\arraymap.inl>

#define invalid_cellid 0XFFFFFFFF

//typedef bool (*CellCallBack)(const GSO::StreamCell* cell, uint_t order, void* pDefData);
typedef bool ( CALLBACK *OnCellID )( cellid_t id, GSO::StreamCellDescriptor desc, uint_t order, void* pDefData );

namespace GSO
{
    class DataStream
    {
        int                                m_FocusedCellId;
        std::map< cellid_t, StreamCell* >  m_RealmBuf;
        std::vector< StreamCell* >         m_WaitLoadSeq;
        std::vector< StreamCell* >         m_WaitReleaseSeq;
        pftml::Timer                            m_LoadTimer;
        pftml::Timer                            m_ReleaseTimer;
        std::vector< StreamCell* >         m_pLoadedCellVec;


        //CellRealm* operator[] (uint_t index){ return m_RealmBuf[index]; }
        //StreamCell* GetCell(cellid_t Id);

    public:
        uint_t                  ForAllCell( OnCellID callback, void* pData );


    public:

        DataStream();
        virtual ~DataStream();

        int                     GetFocusedCellID() const { return m_FocusedCellId; }
        void                    GetLoadedCellIDs( std::vector<int>& idList ) const;
        size32_t                GetLoadedCellCount( void );
        void*                   GetLoadedCellData( uint_t i );

        uint_t                  GetCellCount( void );
        uint_t                  GetWaitLoadSeqSize( void );
        uint_t                  GetWaitReleaseSeqSize( void );

        //        const char*     GetCellName    (int Id);
        StreamCell*             GetStreamCell( cellid_t Id );
        StreamCellDescriptor    GetCellDescriptor( cellid_t Id );
        void*                   GetCellData( cellid_t Id );
        void*                   GetCellUserData( cellid_t Id );


        void                    ForceUpdateCenter( cellid_t Id );
        void                    UpdateCenter( cellid_t Id );
        // if you don't want use streaming
        void                    UpdateAllCell( void );
        void                    ForAllNeighborhood( cellid_t Id, OnCellID func, void* data );
        //void                    ForAllNeighborhood(cellid_t Id, OnZone func, void* data);

        //bool                  InsertCell(const char* filename, int Id);
        bool                    InsertCell( StreamCellDescriptor CellDescriptor, cellid_t Id );
        bool                    RemoveCell( cellid_t Id );
        void                    ReleaseCells();

        void                    SetLoadTime( mstimen_t t );
        void                    SetReleaseTime( mstimen_t t );

        bool                    IsLoadingFinish( void );

        //protected:
        virtual void            OnDestroyDescriptor( StreamCellDescriptor descriptor ) {};

        virtual void            OnGetNeighbor( cellid_t Id,
            std::vector< int >& neighbor )
        {
        };
        virtual void*           OnLoadCell( cellid_t Id,
            StreamCellDescriptor CellDescriptor,//const char* filename,
            void** data,
            void** userdata ) = 0;
        virtual void            OnReleaseCell( cellid_t Id,
            StreamCellDescriptor CellDescriptor,//const char* filename,
            void* data,
            void* userdata ) = 0;

    };

};



#endif