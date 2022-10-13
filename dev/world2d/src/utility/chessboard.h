//chessboard.h header file of Chessboard.cpp
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
//                                               Copyright    (C)    1999    Pin
//
#ifndef CHESSBOARD_H
#define CHESSBOARD_H


//#include <DynArray.tl>
//#include <ArrayMap.tl>

//#include <pdef.h>
#include <data_def.h>
#include <pf-cstring.h>
#include "chessboard_def.h"



typedef size32_t  ( _CALLBACK *OnBoardSave )( uint_t x, uint_t y, void* pData );
typedef LG::Point ( _CALLBACK *OnPosGetGrid )( LG::Point pos, uint_t GW, uint_t GH );


////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class
class CPChessboard
{

    DataList             m_TableList;

    void*  GetBoard( int index );

public:

    CPChessboard();
    ~CPChessboard();


    void          Destroy( void );
    error_t       Save( PBOARD board, const tchar_t* filename, OnBoardSave savefunc = NULL );
    error_t       Load( uint_t iBoard, const tchar_t* filename );

    PBOARD        InsertBoard( const tchar_t* name, uint_t Cols, uint_t Rows, size32_t buflen, void* value = NULL, BoradType type = record_t );
    void          RemoveBoard( PBOARD board );

    PBOARD        GetDescriptor( uint_t i );
    void          SetData( PBOARD board, uint_t col, uint_t row, void* value = NULL );
    void          SetData( int board, uint_t col, uint_t row, void* value = NULL );
    void*         GetData( PBOARD board, uint_t col, uint_t row );
    void*         GetData( int board, uint_t col, uint_t row );


    pfstd::CString  GetBroadName( PBOARD board );
    dword_t          GetBroadCols( PBOARD board );
    dword_t          GetBroadRows( PBOARD board );
    //     uint_t      GetBroadGridWidth    (PBOARD board);
    //     uint_t      GetBroadGridHeight   (PBOARD board);
    //     shape_t   GetBroadGridShape    (PBOARD board);
    //  void      SetBroadGrid         (PBOARD board, uint_t width, uint_t height, shape_t shape);

    //     LG::Point GetBroadGridPos      (PBOARD board, LG::Point pos);
    //     void      RegisterPosGetGrid   (shape_t shape, OnPosGetGrid func);

    bool      IsBoardEmpty( void ) { return m_TableList.empty(); }

    int       Board_Count( void );
    void      Board_Allot_All( uint_t Cols, uint_t Rows );
    void      Board_Release_All( void );
};

#endif
