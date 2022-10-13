//Chessboard.h header file of Chessboard.cpp
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
#include <datadef.h>
#include <tString.h>
#include "Chessboarddef.h"


typedef size32_t    ( *BoardSaveBC )( UINT x, UINT y, void* pData );
typedef LG::Point ( *PosGetGridCallBack )( LG::Point pos, UINT GW, UINT GH );


////////////////////////////////////////////////////////////////////////////////////////////
// ChessBoard class
class CPChessboard
{

    DataList             m_TableList;
    
    void*  GetBoard      ( int index );
    
public:

    CPChessboard();
    ~CPChessboard();
    
    
    void            Destroy             ( void );
    error_t         Save                ( PBOARD board, const tchar_t* filename, BoardSaveBC savefunc = NULL );
    error_t         Load                ( UINT iBoard, const tchar_t* filename );
    
    PBOARD          InsertBoard         ( const tchar_t* name, UINT Cols, UINT Rows, size32_t buflen, void* value = NULL, BoradType type = record_t );
    void            RemoveBoard         ( PBOARD board );
    
    PBOARD          GetDescriptor       ( UINT i );
    void            SetData             ( PBOARD board, UINT col, UINT row, void* value = NULL );
    void            SetData             ( int board, UINT col, UINT row, void* value = NULL );
    void*           GetData             ( PBOARD board, UINT col, UINT row );
    void*           GetData             ( int board, UINT col, UINT row );
    
    
    PfStd::String_t GetBroadName        ( PBOARD board );
    DWORD           GetBroadCols        ( PBOARD board );
    DWORD           GetBroadRows        ( PBOARD board );
// 	UINT      GetBroadGridWidth    (PBOARD board);
// 	UINT      GetBroadGridHeight   (PBOARD board);
// 	shape_t   GetBroadGridShape    (PBOARD board);
//  void      SetBroadGrid         (PBOARD board, UINT width, UINT height, shape_t shape);

// 	LG::Point GetBroadGridPos      (PBOARD board, LG::Point pos);
// 	void      RegisterPosGetGrid   (shape_t shape, PosGetGridCallBack func);

    bool           IsBoardEmpty         ( void ) { return m_TableList.empty(); }
    
    int            Board_Count          ( void );
    void           Board_Allot_All      ( UINT Cols, UINT Rows );
    void           Board_Release_All    ( void );
};

#endif
