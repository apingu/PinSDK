//CBoard.cpp map data class
//
//
//
//
//
//
//
//                                               Copyright    (C)    1999    Pin
//

//01. == C 系統文件/C++ 系統文件
#include <direct.h>
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <gadget\file_operator.h>
#include <template\arraymap.inl>
//
#include <memio.inl>

//03. == 其他 使用 函式庫的.h 文件


//04. == 此專案內.h 文件
#include "chessboard.h"


////////////////////////////////////////////////////////////////////////////////////////////
// Board class
class CPBoard : public pfstl::arraymap<data_t>
{

public:

    CPBoard() { m_Type = 0; release(); m_funcPosGetGrid = NULL; }
    ~CPBoard() { release(); }
    
    pfstd::CString      m_filename;
    //grid_t               m_Grid; //好像沒屁用
    size32_t               m_datalen;   // 每一格的資料長度
    uint_t               m_Type;
    //DataIter            Iter;
    
    OnPosGetGrid  m_funcPosGetGrid;
    void release( void ) {    clear(); }
    
};


namespace
{
    //board_t == PBOARD
    struct board_t
    {
        CPBoard      Table;
        tchar_t      Name[32];
        DataIter     Iter;
    };
};


#define DELETE_BOARD_T(b)   b->Table.release(); delete b;
#define GETBOARD(b)    (((board_t*)b)->Table)



//////////////////////////////////////////////////////////////////////////////////////////////////
//

//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPChessboard::CPChessboard()
{
    Destroy();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
CPChessboard::~CPChessboard()
{
    Destroy();
}

//=============================================================================
// NAME: Board
// DESC: return one board
//=============================================================================
void* CPChessboard::GetBoard( int board )
{
    if ( board < static_cast<int>( m_TableList.size() ) )
    {
        DataIter Iter = m_TableList.begin();
        
        for ( int i = 0; i < board; i++ )
        {
            if ( Iter == m_TableList.end() )
            { 
                return NULL;
            }
            
            Iter++;
        }
        
        return ( void* )( *Iter );
        //return (void*)m_TableList[board];
    }
    
    return NULL;
}


/*
//=============================================================================
// NAME: Board
// DESC: return one board
//=============================================================================
void* CPChessboard::GetBoard(PBOARD board)
{
    for(uint_t b=0; b<m_TableList.size(); b++)
    {
        if(((CPBoard*)m_TableList[b])->Name == name)
            return (void*)m_TableList[b];
    }
    return NULL;
}
*/


/*
//=============================================================================
// NAME: SetBoard
// DESC: return one board
//=============================================================================
int CPChessboard::SetBoard(void* b)
{
    board_t* board = (board_t*)b;
    m_TableList.push_back(board);
    board->Iter= m_TableList.end();
    return 1;
}
*/


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPChessboard::Destroy( void )
{
    //m_Grid.width  = 1;
    //m_Grid.height = 1;
    //m_Grid.shape  = _P_POINT_;
    //m_Cols  = 0;
    //m_Rows  = 0;
//    m_Grid_Table_Name.clear();
    Board_Release_All();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
void CPChessboard::SetGridData(int width, int height, short shape)
{
    m_Grid.width   = width;
    m_Grid.height  = height;
    m_Grid.shape   = shape;
    return;
}
*/

//=============================================================================
// NAME:
// DESC:
//
// fname_size | fname |
//
// chess.data(g file)
// grid_width | gird_height | gird_shape |
// cols count | rows count |
//
// board.data(pp file)
// board 1.....
//
//
//=============================================================================
error_t CPChessboard::Load( uint_t iBoard, const tchar_t* infname )
{
    tchar_t fname[_MAX_LFNAME];
    mcurrentpath( fname, infname );
    FileOperator bf;
    
    if ( bf.open( fname, "rb" ) == NULL )
    {
        return NULL;
    }
    
    CPBoard* bd = ( CPBoard* )GetBoard( iBoard );
    bd->m_filename = fname;
//    board_t* board = new board_t;
//    bf.read(&(board->Table.m_Grid), sizeof(grid_t), 1);
    tchar_t name[32];
    bf.read( name, 32, 1 );
    size32_t buflen;
    dword_t cols, rows;
    bf.read( &buflen, sizeof( shape_t ), 1 );
    bf.read( &cols, sizeof( dword_t ), 1 );
    bf.read( &rows, sizeof( dword_t ), 1 );
    InsertBoard( name, cols, rows, buflen );
    int* value = new int[cols];
    
    for ( uint_t y = 0; y < rows; y++ )
    {
        bf.read( value, sizeof( int ), cols );
        void* tbd = &( ( *bd )( 0, y ) );
        __pfmemcpy( tbd, value, sizeof( long )*cols );
        /*
        for(int x = 0; x <cols; x++)
        {
            bf.read(
            SetData(iBoard, x, y, (void*)value);
        }
        */
    }
    
    delete [] value;
    return 1;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
error_t CPChessboard::Save( PBOARD board, const tchar_t* infname, OnBoardSave savefunc )
{
    tchar_t fname[_MAX_LFNAME];
    mcurrentpath( fname, infname );
    assert( board );
    FileOperator chess;
    
    if ( chess.open( fname, "wb" ) == NULL )
    {
        // 假設有可能是 foler沒有建立
        tchar_t foldername[_MAX_LFNAME];
        getpathdir( foldername, fname );
        mkdir( foldername );
        
        if ( chess.open( fname, "wb" ) == NULL )
        {
            // 沒輒了
            return 0;
        }
    }
    
    board_t* b = ( board_t* )board;
//    chess.write(&b->Table.m_Grid, sizeof(grid_t), 1);
    //char* name = b->Name;
    chess.write( b->Name, 32, 1 );
    // 這個欄位先保留
    shape_t shape = b->Table.m_datalen;
    chess.write( &shape, sizeof( shape_t ), 1 );
    dword_t   cols  = GetBroadCols( board );
    chess.write( &cols, sizeof( dword_t ), 1 );
    dword_t   rows  = GetBroadRows( board );
    chess.write( &rows, sizeof( dword_t ), 1 );
    
    if ( savefunc != NULL )
    {
        for ( int y = 0; y < b->Table.row_size(); y++ )
        {
            for ( int x = 0; x < b->Table.col_size(); x++ )
            {
                void* v = GetData( board, x, y );
                assert( v );
                chess.write( v, savefunc( x, y, v ), 1 );
            }
        }
    }
    else
    {
        for ( int y = 0; y < b->Table.row_size(); y++ )
        {
            for ( int x = 0; x < b->Table.col_size(); x++ )
            {
                void* v = GetData( board, x, y );
                int data = ( int )v;
                chess.write( &data, sizeof( int ), 1 );
            }
        }
    }
    
    chess.close();
    /*
    std::vector<const char*> cb_fname;
    
    // write chess data
    FileOperator chess(".//chess.data", "wb");
    //chess.write(&m_Grid, sizeof(m_Grid),  1);
    //chess.write(&m_Cols, sizeof(int), 1);
    //chess.write(&m_Rows, sizeof(int), 1);
    cb_fname.push_back(chess.fpath());//*********************************
    chess.close();
    
    if(savefunc !=NULL )
    {
        //write board data
        std::vector<const tchar_t*> b_fname;
        for(uint_t i = 0; i < m_TableList.size(); i++)
        {
            board_t* b = (board_t*)m_TableList[i];
    
           if(b->Table.m_Type != record_t)
               continue;
    
    
            tchar_t temp_fname[_MAX_LPATH];
            sprintf(temp_fname, "%s%s", ".//", b->Name);
            FileOperator temp_board_f(temp_fname, "wb");
    
            chess.write(&b->Table.m_Grid, sizeof(grid_t), 1);
    
            for(int y = 0; y < b->Table.row_size(); y++)
            {
                for(int x = 0; x <b->Table.col_size(); x++)
                {
                    //int v = (int)GetData(i, x, y);
                    //uint_t savesize = 0;
                    //void* data = savefunc(savesize, GetData(i, x, y));
    
                    //temp_board_f.write(&v, sizeof(int), 1);
                }
            }
            b_fname.push_back(temp_fname);
            temp_board_f.close();
        }
    
        //tchar_t bpath[_MAX_LPATH];
        //combine save file
        FileOperator board("./board.data","wb");
        board.Packfiles(b_fname, "", _DELETE_SORUCE_FILE);
        cb_fname.push_back(board.fpath());
        board.close();
    }
    
    //combine chess and board file
    FileOperator cb_board(fname, "wb");
    cb_board.Packfiles(cb_fname, "", _DELETE_SORUCE_FILE);
    */
    return 1;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
pfstd::CString CPChessboard::GetBroadName( PBOARD board )
{
    return GETBOARD( board ).m_filename;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
// void CPChessboard::SetBroadGrid(PBOARD board, uint_t width, uint_t height, uint_t shape)
// {
//     GETBOARD(board).m_Grid.width = width;
//     GETBOARD(board).m_Grid.height = height;
//     GETBOARD(board).m_Grid.shape = shape;
//     return;
// }


// //=============================================================================
// // NAME:
// // DESC:
// //=============================================================================
// uint_t CPChessboard::GetBroadGridWidth(PBOARD board)
// {
//     return GETBOARD(board).m_Grid.width;
// }
//
//
// //=============================================================================
// // NAME:
// // DESC:
// //=============================================================================
// uint_t CPChessboard::GetBroadGridHeight(PBOARD board)
// {
//     return GETBOARD(board).m_Grid.height;
// }
//
//
// //=============================================================================
// // NAME:
// // DESC:
// //=============================================================================
// shape_t CPChessboard::GetBroadGridShape(PBOARD board)
// {
//     return GETBOARD(board).m_Grid.shape;
// }


//=============================================================================
// NAME:
// DESC:
//=============================================================================
dword_t CPChessboard::GetBroadCols( PBOARD board )
{
    return GETBOARD( board ).col_size();
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
dword_t CPChessboard::GetBroadRows( PBOARD board )
{
    return GETBOARD( board ).row_size();
}

// //=============================================================================
// // NAME:
// // DESC:
// //=============================================================================
// LG::Point CPChessboard::GetBroadGridPos(PBOARD board, LG::Point pos)
// {
//     LG::Point pfstl(-1, -1);
//     //CPBoard* board = (CPBoard*)GetBoard(name);
//
//     if(GETBOARD(board).m_funcPosGetGrid!= NULL)
//         return GETBOARD(board).m_funcPosGetGrid(pos,
//                                                    GETBOARD(board).m_Grid.width,
//                                                    GETBOARD(board).m_Grid.height);
//
//     return pfstl;
// }


//=============================================================================
// NAME: BoardInsert
// DESC: add one board in list end
//=============================================================================
PBOARD CPChessboard::InsertBoard( const tchar_t* name, uint_t Cols, uint_t Rows, size32_t buflen, void* Table, BoradType type )
{
    board_t* new_table = new board_t;
    new_table->Table.m_Type = type;
    new_table->Table.m_datalen = sizeof( long );
    new_table->Table.allot( Cols, Rows );
    new_table->Table.set_element( Table );
    memset( new_table->Name, 0, 32 );
    strcpy( new_table->Name, name );
    /*
    CPBoard* new_table = new CPBoard;
    new_table->m_Type = type;
    new_table->Name = name;
    new_table->allot(Cols, Rows);
    new_table->Set_element(Table);
    */
    assert( new_table != NULL );
    m_TableList.push_back( new_table );
    new_table->Iter = m_TableList.end();
    new_table->Iter--;
    return new_table;
}


//=============================================================================
// NAME: BoardRemove
// DESC: add one board in list end
//=============================================================================
void CPChessboard::RemoveBoard( PBOARD board )
{
    board_t* b = ( board_t* )board;
    m_TableList.erase( b->Iter );
    DELETE_BOARD_T( b );
    /*
    for(uint_t i = 0; i<m_TableList.size(); i++)
    {
        if(((CPBoard*)m_TableList[i])->Name == name)
        {
            delete ((CPBoard*)m_TableList[i]);
            m_TableList.erase(m_TableList.begin()+i);
        }
    }
    */
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
PBOARD CPChessboard::GetDescriptor( uint_t id )
{
    if ( id >= m_TableList.size() )
    { return NULL; }
    
    DataIter Iter = m_TableList.begin();
    
    for ( unsigned int i = 0; i < id; i++ )
    {
        if ( Iter == m_TableList.end() )
        { return NULL; }
        
        Iter++;
    }
    
    return ( void* )( *Iter );
    //return m_TableList[i];
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPChessboard::SetData( PBOARD board, uint_t col, uint_t row, void* Table )
{
    //CPBoard* bd = GetBoard(board);
    if ( board != NULL )
    { ( GETBOARD( board ) )( col, row ) = Table; }
    
    return;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPChessboard::SetData( int index, uint_t col, uint_t row, void* Table )
{
    CPBoard* bd = ( CPBoard* )GetBoard( index );
    
    if ( bd != NULL )
    { ( *bd )( col, row ) = Table; }
    
    return;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void* CPChessboard::GetData( PBOARD board, uint_t col, uint_t row )
{
    //CPBoard* bd = (CPBoard*)GetBoard(PBOARD);
    //現不要防當? 因為不應該有這樣的情形發生
    if ( ( col >= GETBOARD( board ).col_size() ) || ( row >= GETBOARD( board ).row_size() ) )
    { return NULL; }
    
    if ( board != NULL )
    { return ( GETBOARD( board ) )( col, row ); }
    
    return NULL;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
void* CPChessboard::GetData( int index, uint_t col, uint_t row )
{
    CPBoard* bd = ( CPBoard* )GetBoard( index );
    //現不要防當? 因為不應該有這樣的情形發生
    /*
    if((col >= bd->col_size()) ||
        (row >= bd->row_size()))
        return NULL;
        */
    
    if ( bd != NULL )
    {
        return ( *bd )( col, row );
    }
    
    return NULL;
}



/*
//=============================================================================
// NAME:
// DESC:
//=============================================================================
void CPChessboard::Board_Set_Grid(int board, int col, int row, int Table)
{
    if(board >= static_cast<int>(m_TableList.size()))
        return;
    CPBoard* bd = m_TableList[board];
    (*m_TableList[board])(col, row) = Table;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
int CPChessboard::Board_Get_Grid(int board, int col, int row)
{
    if(board >= static_cast<int>(m_TableList.size()))
        return -1;
    CPBoard* bd = m_TableList[board];
    return (*m_TableList[board])(col, row);
}
*/




//=============================================================================
// NAME:
// DESC:
//=============================================================================
/*
void* CPChessboard::Who_on_that_block(CPVertex pos)
{
    return 0;//+++++++m_MapObject(pos.x, pos.y);
}
*/



//=============================================================================
// NAME: Board_Count()
// DESC: return how much board now we have
//=============================================================================
int CPChessboard::Board_Count( void )
{
    return m_TableList.size();  ;
}

//=============================================================================
// NAME: Board_Allot_All()
// DESC: allot all board size
//=============================================================================
void CPChessboard::Board_Allot_All( uint_t Cols, uint_t Rows )
{
    //m_Cols = Cols;
    //m_Rows = Rows;
    //動態建立存放每個地圖的二維陣列=====================================
    //+++++m_MapObject.allot(Cols, Rows);
    //+++++m_MapObject.Set_Data(0);
    DataIter Iter = m_TableList.begin();
    
    //for(unsigned int i = 0; i<m_TableList.size(); i++)
    while ( Iter != m_TableList.end() )
    {
        GETBOARD( ( *Iter ) ).allot( Cols, Rows );
        Iter++;
    }
    
    //for(uint_t i = 0; i < m_TableList.size(); i++)
    //    GETBOARD(m_TableList[i]).allot(Cols, Rows);
    //end 動態建立存放每個地圖的二維陣列=================================
}

//=============================================================================
// NAME:  Board_Release_All()
// DESC:  release all board
//=============================================================================
void CPChessboard::Board_Release_All( void )
{
    DataIter Iter = m_TableList.begin();
    
    while ( Iter != m_TableList.end() )
    {
        board_t* e = ( board_t* )( *Iter ); //.m_pNode->Value;
        DELETE_BOARD_T( e );
        Iter++;
    }
    
    m_TableList.clear();
    /*
    for(uint_t j = 0; j < m_TableList.size(); j++)
    {
        board_t* b=(board_t*)m_TableList[j];
        SAFE_DELETE(b);
    }
    m_TableList.clear();
    */
    //+++++m_MapObject.clear();
}
