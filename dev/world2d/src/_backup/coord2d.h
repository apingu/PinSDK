/*----------------------------------------------------------------------------------------
 *  Coord2d class
 *
 *
 *
 *
 *
 *
 *---------------------------------------------------------------------------------------*/

#ifndef _COORD2D_H_
#define _COORD2D_H_

#include <stdio.h>
#include <stdlib.h>
#include <Graphic2d.h>

namespace PM
{
    class Coord2d
    {
    private:
    
        int m_Width;
        int m_Height;
        int m_Offset;
        
    public:
    
        Coord2d()
        {
            m_Width = 1;
            m_Height = 1;
            m_Offset = 0;
        };
        Coord2d( int width, int height )
        {
            operator() ( width, height );
        };
        Coord2d( int width, int height, int offset )
        {
            operator() ( width, height, offset );
        };
        ~Coord2d()
        {
        };
        
        inline void operator()( int width, int height )
        {
            m_Width = width;
            m_Height = height;
            m_Offset = 0;
        };
        
        inline void operator()( int width, int height, int offset )
        {
            m_Width = width;
            m_Height = height;
            m_Offset = offset;
        };
        
        inline void SetWidth( int w ) { m_Width = w; }
        inline void SetHeight( int h ) { m_Height = h; }
        inline void SetOffset( int o ) { m_Offset = o; }
        /*
        inline POINT Seat( int x, int y ) //Site in screen
        {
            LG::Point   p;
        
            x = x - m_Offset;
        
            float   Scale       = 1;
            if( ( m_Width - m_Height ) > 0 )
                Scale = ( ( float )
                          ( ( float ) m_Width / ( float ) m_Height ) );
            else if( ( m_Width - m_Height ) == 0 )
                Scale = ( ( float )
                          ( ( float ) m_Width / ( float ) m_Height ) ) +
                        1;
        
            p.x = ( x + y + 1 ) * ( m_Width / ( int ) Scale );
            p.y = ( y - x ) * ( m_Height / ( int ) Scale );
        
            return p;
        }
        
        inline LG::Point Coord( int x, int y ) //Site in Coord
        {
            LG::Point   r;
        
            y = y * ( m_Width / m_Height );
        
            if( ( x - y ) < 0 )
            {
                r.x = ( ( ( x - y ) / m_Width ) - 1 );
                r.y = ( ( x + y ) / m_Width );
            }else
            {
                r.x = ( ( x - y ) / m_Width );
                r.y = ( ( x + y ) / m_Width );
            }
            r.x = r.x + m_Offset;
        
            return r;
        }
        */
        
        
        
        //=============================================================================
        //  Name :
        //  Desc :　轉換矩形實體座標
        //=============================================================================
        inline LG::Point Rect_Chessboard_to_Map_Site( LG::Point site )
        {
            LG::Point EntityPoint;
            EntityPoint.x = site.x * m_Width + ( m_Width >> 1 );
            EntityPoint.y = site.y * m_Height + ( m_Height >> 1 );
            return EntityPoint;
        }
        
        
        //=============================================================================
        //  Name :
        //  Desc :　轉換矩形自訂座標
        //=============================================================================
        inline LG::Point Rect_Map_to_Chessboard_Site( LG::Point site )
        {
            LG::Point EntityPoint;
            
            if ( site.x >= 0 )
            {
                EntityPoint.x = site.x / m_Width ;
            }
            else
            {
                EntityPoint.x = ( site.x / m_Width ) - 1 ;
            }
            
            if ( site.y >= 0 )
            {
                EntityPoint.y = site.y / m_Height;
            }
            else
            {
                EntityPoint.y = ( site.y / m_Height ) - 1;
            }
            
            return EntityPoint;
        }
        
        
        //=============================================================================
        //  Name :
        //  Desc :　將實體點轉換為菱形格座標 64 128
        //=============================================================================
        inline LG::Point Rhomb_Map_to_Chessboard_Site_128_64( LG::Point site )
        {
            LG::Point r;
            int x   = site.x;
            int y   = site.y;
            y <<= 1;
            r.x = ( x - y ) >> 7;   // (x - 2y) / 64 // (>> 7 為寬128)
            r.y = ( x + y ) >> 7;   // (x + 2y) / 64
            //加上X偏移量 將X軸起點從負值偏移至0
            r.x = r.x + m_Offset;
            return r;
        }
        
        //=============================================================================
        //  Name :
        //  Desc :　將實體點轉換為菱形格座標   格子寬, 格子高
        //=============================================================================
        inline LG::Point Rhomb_Map_to_Chessboard_Site( LG::Point site )
        {
            LG::Point   r;
            site.y = site.y * ( ( float )m_Width / ( float )m_Height );
            
            if ( ( site.x - site.y ) < 0 )
            {
                r.x = ( ( ( site.x - site.y ) / m_Width ) - 1 );
                r.y = ( ( site.x + site.y ) / m_Width );
            }
            else
            {
                r.x = ( ( site.x - site.y ) / m_Width );
                r.y = ( ( site.x + site.y ) / m_Width );
            }
            
            r.x = r.x + m_Offset;
            return r;
            //rhomb( w, h, m_Rhomb_Chessboard_Misregistration );
            //return rhomb.Coord( ScreenToMapSite.x, ScreenToMapSite.y );
        }
        
        
        //=============================================================================
        //  Name :
        //  Desc : 將菱形格座標轉換為實體點(傳出格點的中點)64 128
        //=============================================================================
        inline LG::Point Rhomb_Chessboard_to_Map_Site_128_64( LG::Point site )
        {
            LG::Point p;
            //減X偏移量 將X軸起點從0偏移回原先的值
            int x   = site.x - m_Offset;
            int y   = site.y;
            p.x = ( x + y + 1 ) * ( 128 >> 1 );
            p.y = ( y - x ) * ( 64 >> 1 );
            return p;
        }
        
        
        //=============================================================================
        //  Name :
        //  Desc :  將菱形格座標轉換為實體點(傳出格點的中點) 格子寬, 格子高
        //=============================================================================
        inline LG::Point Rhomb_Chessboard_to_Map_Site( LG::Point site )
        {
            LG::Point   p;
            site.x = site.x - m_Offset;
            float   Scale       = 1;
            
			if ( ( m_Width - m_Height ) > 0 )
			{
				Scale = ( ( float ) ( ( float ) m_Width / ( float ) m_Height ) );
			}
			else if ( ( m_Width - m_Height ) == 0 )
			{
				Scale = ( ( float ) ( ( float ) m_Width / ( float ) m_Height ) ) + 1;
			}
                        
            /*
                        p.x = ( site.x + site.y + 1 ) * ( m_Width / ( int ) Scale );
                        p.y = ( site.y - site.x ) * ( m_Height / ( int ) Scale );
            */
            // TODO: Alex, the algorithm should be something like this, I'm wondering why Pin's algorithm goes that way?
            p.x = ( site.x + site.y + 1 ) * ( float )m_Width * 0.5f;
            p.y = ( site.y - site.x ) * ( float )m_Height * 0.5f;
            return p;
            //rhomb( w, h, m_Rhomb_Chessboard_Misregistration );
            //return rhomb.Seat( GridSite.x, GridSite.y );
        }
        
        
        
        
    };
    
};//namespace PM






#endif
