//Topog.cpp map board compute class
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
#include <chessboard.h>
#include <coord2d.h>
#include <math.h>
#include "topog2d.h"


namespace GSO
{
    namespace World
    {
        PM::Coord2d coord;

        inline GameVertex Point_Chessboard_to_Map_Site(GameVertex site)        //轉成矩形格點座標
        {
            return site;
        }

        inline GameVertex Point_Map_to_Chessboard_Site(GameVertex site)        //將矩形格點座標轉成螢幕點座標
        {
            return site;
        }

        //矩形
        inline GameVertex Rect_Chessboard_to_Map_Site(GameVertex site)        //轉成矩形格點座標
        {
            return coord.Rect_Chessboard_to_Map_Site(site);
        }

        inline GameVertex Rect_Map_to_Chessboard_Site(GameVertex site)        //將矩形格點座標轉成螢幕點座標
        {
            return coord.Rect_Map_to_Chessboard_Site(site);
        }

        //菱形
        inline GameVertex Rhomb_Chessboard_to_Map_Site(GameVertex site)        //將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
        {
            return coord.Rhomb_Chessboard_to_Map_Site(site);
        }

        inline GameVertex Rhomb_Map_to_Chessboard_Site(GameVertex site)         //將實體點轉換為菱形格座標                 parameters1:Grid width  parameters2:Grid height
        {
            return coord.Rhomb_Map_to_Chessboard_Site(site);
        }

        inline GameVertex Rhomb_Chessboard_to_Map_Site_128_64(GameVertex site)   //轉成128 64菱形格點座標
        {
            return coord.Rhomb_Chessboard_to_Map_Site_128_64(site);
        }

        inline GameVertex Rhomb_Map_to_Chessboard_Site_128_64(GameVertex site)    //將128 64菱形格點座標轉成真實點座標
        {
            return coord.Rhomb_Map_to_Chessboard_Site_128_64(site);
        }



        //=============================================================================
        // NAME: 
        // DESC: constructor
        //=============================================================================
        Topog2d::Topog2d()
        {
            Clear();
//            m_pScreenDatum=NULL;
            m_pMapToChessboardSite = Point_Chessboard_to_Map_Site;
            m_pChessboardToMapSite = Point_Map_to_Chessboard_Site;
        }


        //=============================================================================
        // NAME: 
        // DESC: distructor
        //=============================================================================
        Topog2d::~Topog2d()
        {
            Clear();
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Topog2d::Create(int sizeW, int sizeH,
                                   int gridW, int gridH, shape_t shape)
        {
            if((sizeW == 0) || (sizeH == 0) ||
                (gridW == 0) || (gridH == 0))
            {
                return false;
            }


            m_Grid.width  = gridW;
            m_Grid.height = gridH;
            m_Grid.shape  = shape;
            m_Range.cx    = sizeW;
            m_Range.cy    = sizeH;

            coord.SetWidth(gridW);
            coord.SetHeight(gridH);
            coord.SetOffset(0);  //**這裡要先清空　很重要

            //  設定初始化的地圖轉換function
            //init funciton
            //InitSiteConvertFunc(shape,
                                 //m_pScreenToChessboardSite,
            //                     m_pMapToChessboardSite,
                                 //m_pChessboardToScreenSite,
            //                     m_pChessboardToMapSite);

            switch(shape)
            {

            case _P_POINT_:
                m_pMapToChessboardSite = Point_Map_to_Chessboard_Site;
                m_pChessboardToMapSite = Point_Chessboard_to_Map_Site;
                break;

            case _P_RECT_: 
                m_pMapToChessboardSite    = Rect_Map_to_Chessboard_Site;
                m_pChessboardToMapSite    = Rect_Chessboard_to_Map_Site;
                break;

            case _P_RHOMBUS_:

                if((m_Grid.width == 128) && (m_Grid.height == 64))
                {
                    m_pMapToChessboardSite = Rhomb_Map_to_Chessboard_Site_128_64;
                    m_pChessboardToMapSite = Rhomb_Chessboard_to_Map_Site_128_64;
                }
                else
                {
                    m_pMapToChessboardSite = Rhomb_Map_to_Chessboard_Site;
                    m_pChessboardToMapSite = Rhomb_Chessboard_to_Map_Site;
                }
                //ScreenToChessboardSite = Rhomb_Screen_to_Chessboard_Site;
                //ChessboardToScreenSite = Rhomb_Chessboard_to_Screen_Site;

                break;
            }


            if(m_Grid.shape == _P_RHOMBUS_) //菱形格資料
            {
                Rhomb_Grid_WHScale(gridW, gridH);
                Rhomb_Chessboard_Revise(gridW, gridH, sizeH);//計算出菱形格的地圖偏移，使其座標為正值
                //coord(width, height, m_Rhomb_Chessboard_Misregistration);
                coord.SetOffset(m_Rhomb_Chessboard_Misregistration);

                // Derive cols and rows of grids to create chessboard
                GameVertex screenToMapSite;
                GameVertex gridSite;
    
                screenToMapSite.x = sizeW;    //換算地圖右上角得出矩陣寬
                screenToMapSite.y = 0;
                gridSite = Rhomb_Map_to_Chessboard_Site(screenToMapSite);
                m_Cols = gridSite.x;

                screenToMapSite.x = sizeW;    //換算地圖右下角得出矩陣高
                screenToMapSite.y = sizeH;
                gridSite = Rhomb_Map_to_Chessboard_Site(screenToMapSite);
                m_Rows = gridSite.y;
            }
            else if(m_Grid.shape == _P_RECT_)   // 矩形..........................
            {
                m_Cols = (int) ceil((double) (sizeW / gridW));
                m_Rows = (int) ceil((double) (sizeH / gridH));
            }

            return true;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Topog2d::Clear(void)
        {
//            CPChessboard::Destroy();
            //Pos()(0, 0);
            coord.SetWidth(1);
            coord.SetHeight(1);
            coord.SetOffset(0);  //**這裡要先清空　很重要
            m_RhombGridWHScale = 1;
            m_Rhomb_Chessboard_Misregistration = 0;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Topog2d::MarkArea(GameVertex site, int Distance, int id)
        {
            /*
            for(int j = (site.y - Distance); j <= (site.y + Distance); j++)
                for(int i = (site.x - Distance); i <= (site.x + Distance); i++)
                    if((abs(i - site.x) + abs(j - site.y)) <= Distance)
                        if((i >= 0) && (j >= 0))
                                //m_MapGrid(j, i).floor = id;        //可走的格子
                             */
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Topog2d::ClearArea(int id)
        {
            /*
            for(int i = 0 ; i < m_MapGrid.Row_Size() ; i++)
                    for(int j = 0 ; j < m_MapGrid.Col_Size() ; j++)
                      //m_MapGrid(i, j).floor = id;
                      */
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
//         void Topog2d::InitSiteConvertFunc(shape_t gridtype,
//                                            OnSiteConvert map2chessboardSite,
//                                            OnSiteConvert chessboard2mapSite)
// 
//         {
// 
// 
//         }



        //=============================================================================
        // NAME: 
        // DESC: 計算地形格的長寬比例
        //=============================================================================
        float Topog2d::Rhomb_Grid_WHScale(int GridWidth, int GridHeight)
        {
            return m_RhombGridWHScale = (float)
                                        ((float) GridWidth /
                                          (float) GridHeight);
        }


        //=============================================================================
        // NAME: 
        // DESC: 換算地圖左下角得出偏移質
        //=============================================================================
        int Topog2d::Rhomb_Chessboard_Revise(int GridWidth,
                                              int GridHeight,
                                              int MapHeight)
        {
            GameVertex ScreenToMapSite;
            GameVertex GridSite;
            m_Rhomb_Chessboard_Misregistration = 0;
            ScreenToMapSite.x = 0;           //換算地圖左下角得出偏移質
            ScreenToMapSite.y = MapHeight;
            GridSite = Rhomb_Map_to_Chessboard_Site(ScreenToMapSite);
            return m_Rhomb_Chessboard_Misregistration = abs(GridSite.x);
        }

/*
        //=============================================================================
        // NAME: 
        // DESC: 建立菱形格座標2維陣列
        //=============================================================================
        bool Topog2d::CreateChessboard(int MapWidth,
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


            GameVertex ScreenToMapSite;
            GameVertex GridSite;
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
//            Board_Release_All();
//            Board_Allot_All(m_Cols, m_Rows);
//            InsertBoard("Walkable.board", m_Cols, m_Rows);
//            InsertBoard("Event.board", m_Cols, m_Rows);

            return true;
        }
*/
//        bool Topog2d::CreateChessboard(void)
//        {
//            return CreateChessboard(ImageWidth(),
//                                     ImageHeight(),
//                                     m_Grid.width,
//                                     m_Grid.height,
//                                     m_Grid.shape);
//        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
//        void Topog2d::NewChessboard(int MapWidth,
//                                     int MapHeight,
//                                     int GridWidth,
//                                     int GridHeight,
//                                     shape_t GridShape)
//        {
//            //Scene_Width()   = MapWidth;
//            //Scene_Height()  = MapHeight;
//            CreateChessboard(ImageWidth(),
//                              ImageHeight(),
//                              GridWidth,
//                              GridHeight,
//                              GridShape);
//        }


        //=============================================================================
        // NAME: 
        // DESC: Site in screen
        //=============================================================================
        GameVertex Topog2d::MapToScreenSite(GameVertex Situs)
        {
            return Situs + OnGetScreenDatumMark();//GSO::World::IEntity2d::GetRefOrigin m_ScreenDatum;//GetDatum();//GetScreenDatum();
        }


        //=============================================================================
        // NAME: 
        // DESC: Site in map
        //=============================================================================
        GameVertex Topog2d::ScreenToMapSite(GameVertex Situs)
        {
            return Situs - OnGetScreenDatumMark();//m_ScreenDatum;//GetDatum();//GetScreenDatum();
        }


        //=============================================================================
        // NAME: 
        // DESC: 將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
        //=============================================================================
        GameVertex Topog2d::ScreenToChessboardSite(GameVertex ScreenSite)
        {
            return MapToChessboardSite(ScreenToMapSite(ScreenSite));
        }   

        //=============================================================================
        // NAME: 
        // DESC: 將菱形格座標轉換為實體點(傳出格點的中點) parameters1:Grid width  parameters2:Grid height
        //=============================================================================
        GameVertex Topog2d::ChessboardToScreenSite(GameVertex GridSitus)
        { 
            return MapToScreenSite(ChessboardToMapSite(GridSitus)); 
        }


        //=============================================================================
        // NAME: 
        // DESC: 傳回格子的中心點
        //=============================================================================
        GameVertex Topog2d::GridCenterPoint(GameVertex GridSite)
        {
            //GameVertex ScreenToMapSite= ChessboardToMapSite(GridSite);
            return  MapToScreenSite(ChessboardToMapSite(GridSite));
        }

        /*
        //=============================================================================
        // NAME: 
        // DESC:　轉換矩形實體座標
        //=============================================================================
        inline LG::Point Topog2d::Rect_Chessboard_to_Map_Site(LG::Point GridSite, uint_t w, uint_t h)
        {
            LG::Point EntityPoint;

            EntityPoint.x = GridSite.x * w + (w >> 1);
            EntityPoint.y = GridSite.y * h + (h >> 1);

            return EntityPoint;
        }


        //=============================================================================
        // NAME: 
        // DESC:　轉換矩形自訂座標
        //=============================================================================
        LG::Point Topog2d::Rect_Map_to_Chessboard_Site(LG::Point ScreenToMapSite, uint_t w, uint_t h)
        {
            LG::Point EntityPoint;

            EntityPoint.x = ScreenToMapSite.x / w ;
            EntityPoint.y = ScreenToMapSite.y / h;

            return EntityPoint;
        }


        //=============================================================================
        // NAME: 
        // DESC:　將實體點轉換為菱形格座標 64 128
        //=============================================================================
        LG::Point Topog2d::Rhomb_Map_to_Chessboard_Site_128_64(LG::Point ScreenToMapSite, uint_t w, uint_t h)
        {
            LG::Point r;
            int x   = ScreenToMapSite.x;
            int y   = ScreenToMapSite.y;

            y <<= 1;

            r.x = (x - y) >> 7;   // (x - 2y) / 64 // (>> 7 為寬128)
            r.y = (x + y) >> 7;   // (x + 2y) / 64

            //加上X偏移量 將X軸起點從負值偏移至0
            r.x = r.x + m_Rhomb_Chessboard_Misregistration;
            return r;
        }

        //=============================================================================
        // NAME: 
        // DESC:　將實體點轉換為菱形格座標   格子寬, 格子高
        //=============================================================================
        LG::Point Topog2d::Rhomb_Map_to_Chessboard_Site(LG::Point ScreenToMapSite, uint_t w, uint_t h)
        
        {
            rhomb(w, h, m_Rhomb_Chessboard_Misregistration);
            return rhomb.Coord(ScreenToMapSite.x, ScreenToMapSite.y);
        }


        //=============================================================================
        // NAME: 
        // DESC: 將菱形格座標轉換為實體點(傳出格點的中點)64 128
        //=============================================================================
        LG::Point Topog2d::Rhomb_Chessboard_to_Map_Site_128_64(LG::Point GridSite, uint_t w, uint_t h)
        {
            LG::Point p;
            //減X偏移量 將X軸起點從0偏移回原先的值
            int x   = GridSite.x - m_Rhomb_Chessboard_Misregistration;
            int y   = GridSite.y;

            p.x = (x + y + 1) * (128 >> 1);
            p.y = (y - x) * (64 >> 1);

            return p;
        }


        //=============================================================================
        // NAME: 
        // DESC:  將菱形格座標轉換為實體點(傳出格點的中點) 格子寬, 格子高
        //=============================================================================
        LG::Point Topog2d::Rhomb_Chessboard_to_Map_Site(LG::Point GridSite, uint_t w, uint_t h)
        {
            rhomb(w, h, m_Rhomb_Chessboard_Misregistration);
            return rhomb.Seat(GridSite.x, GridSite.y);
        }
        */


        /*
        
        GameVertex Topog2d::Chessboard_to_Map_Site(GameVertex GridSite)
        {
            switch(m_Grid.shape)
            {
            case _P_POINT_:return GridSite;
            case _P_RECT_:return Rect_Chessboard_to_Map_Site(GridSite,
                                                              m_Grid.width,
                                                              m_Grid.height);
            case _P_RHOMBUS_:return Rhomb_Chessboard_to_Map_Site(GridSite,
                                                                  m_Grid.width,
                                                                  m_Grid.height);
            }
            return GridSite;
        }


        GameVertex Topog2d::Map_to_Chessboard_Site(GameVertex Map_Situs)
        {
            switch(m_Grid.shape)
            {
            case _P_POINT_:return Map_Situs;
            case _P_RECT_:return Rect_Map_to_Chessboard_Site(Map_Situs,
                                                              m_Grid.width,
                                                              m_Grid.height);
            case _P_RHOMBUS_:return Rhomb_Map_to_Chessboard_Site(Map_Situs,
                                                                  m_Grid.width,
                                                                  m_Grid.height);
            }
            return Map_Situs;
        }

        GameVertex Topog2d::Screen_to_Chessboard_Site(GameVertex Screen_Situs)
        {
            switch(m_Grid.shape)
            {
            case _P_POINT_:return ScreenToMapSite(Screen_Situs);
            case _P_RECT_:return Rect_Screen_to_Chessboard_Site(Screen_Situs,
                                                                 m_Grid.width,
                                                                 m_Grid.height);
            case _P_RHOMBUS_:return Rhomb_Screen_to_Chessboard_Site(Screen_Situs,
                                                                     m_Grid.width,
                                                                     m_Grid.height);
            }
            return Screen_Situs;
        }


        GameVertex Topog2d::Chessboard_to_Screen_Site(GameVertex Grid)
        {
            switch(m_Grid.shape)
            {
            case _P_POINT_:return MapToScreenSite(Grid);
            case _P_RECT_:return Rect_Chessboard_to_Screen_Site(Grid,
                                                                 m_Grid.width,
                                                                 m_Grid.height);
            case _P_RHOMBUS_:return Rhomb_Chessboard_to_Screen_Site(Grid,
                                                                     m_Grid.width,
                                                                     m_Grid.height);
            }
            return Grid;
        }
        */

    };//World
};//GE
