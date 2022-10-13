//Topog.h header file of Togog.cpp
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
//                                                         Pin
//

#ifndef TOPOG2D_H
#define TOPOG2D_H

#include <game_def.h>
#include <chessboard_def.h>


namespace GSO
{
    namespace World
    {

        ///////////////////////////////////////////////////////////////////////////////////////////
        //  Topog class
        class Topog2d
        {
        private:

            uint_t     m_Rows;                             // 切割格子後算出來的Row數
            uint_t     m_Cols;                             // 切割格子後算出來的Col數
            grid_t     m_Grid;                             // 格子的定義(長. 寬. 形狀)
            LG::Size   m_Range;                            // 地形的大小
            int        m_Rhomb_Chessboard_Misregistration; // 菱形格X偏移
            float      m_RhombGridWHScale;                 // 菱形格寬高比例


            typedef GameVertex ( _CALLBACK *OnSiteConvert )( GameVertex GridSite );
            //OnSiteConvert  m_pScreenToChessboardSite;
            OnSiteConvert  m_pMapToChessboardSite;
            //OnSiteConvert  m_pChessboardToScreenSite;
            OnSiteConvert  m_pChessboardToMapSite;



            //             virtual void InitSiteConvertFunc(shape_t gridtype,
            //                                               OnSiteConvert map2chessboardSite,
            //                                               OnSiteConvert chessboard2mapSite);


            float        Rhomb_Grid_WHScale( int GridWidth, int GridHeight );   // 計算地形格的長寬比例
            int          Rhomb_Chessboard_Revise( int GridWidth,
                int GridHeight,
                int MapHeight );              // 換算地圖左下角得出偏移值

        public:

            Topog2d();
            ~Topog2d();


            bool                Create( int sizeW, int sizeH,
                int gridW, int gridH, shape_t shape );


            void                Clear( void );


            uint_t                GetWidth( void )      { return m_Range.cx; }
            uint_t                GetHeight( void )     { return m_Range.cy; }
            uint_t                GetCols( void )       { return m_Cols; }
            uint_t                GetRows( void )       { return m_Rows; }
            uint_t                GetGirdWidth( void )  { return m_Grid.width; }
            uint_t                GetGirdHeight( void ) { return m_Grid.height; }
            uint_t                GetGirdShape( void )  { return m_Grid.shape; }


            void                MarkArea( GameVertex site, int Distance, int id );                //mark off area
            void                ClearArea( int id );                                              //clear area


            //格子計算
            GameVertex          ScreenToMapSite( GameVertex ScreenSite );            //捲動相對座標
            GameVertex          MapToScreenSite( GameVertex MapSite );               //相對座標
            GameVertex          ScreenToChessboardSite( GameVertex ScreenSite );     //將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
            GameVertex          ChessboardToScreenSite( GameVertex GridSitus );      //將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
            inline GameVertex   MapToChessboardSite( GameVertex site ) { return m_pMapToChessboardSite( site ); } //相對座標
            inline GameVertex   ChessboardToMapSite( GameVertex site ) { return m_pChessboardToMapSite( site ); } //相對座標

            GameVertex          GridCenterPoint( GameVertex GridSite ); //computer grid center point

        protected:

            // 取得 螢幕位在地圖上的位置
            virtual GameVertex  OnGetScreenDatumMark() = 0;


            /*
            //矩形
            virtual GameVertex    Rect_Chessboard_to_Map_Site(GameVertex GridSitus, uint_t w, uint_t h);        //轉成矩形格點座標
            virtual GameVertex    Rect_Map_to_Chessboard_Site(GameVertex MapSite, uint_t w, uint_t h);        //將矩形格點座標轉成螢幕點座標

            //菱形
            virtual GameVertex    Rhomb_Chessboard_to_Map_Site(GameVertex GridSite, uint_t w, uint_t h);        //將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
            virtual GameVertex    Rhomb_Map_to_Chessboard_Site(GameVertex MapSite, uint_t w, uint_t h);         //將實體點轉換為菱形格座標                 parameters1:Grid width  parameters2:Grid height

            //128 64菱形
            virtual GameVertex    Rhomb_Chessboard_to_Map_Site_128_64(GameVertex GridSite, uint_t w=128, uint_t h=64);   //轉成128 64菱形格點座標
            virtual GameVertex    Rhomb_Map_to_Chessboard_Site_128_64(GameVertex MapSite, uint_t w=128, uint_t h=64);    //將128 64菱形格點座標轉成真實點座標
            */

        };
    };//World
};//GE


#endif