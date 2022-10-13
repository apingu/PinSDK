//Surface.cpp base map class
//
//
// static -->> - (ID) - 2
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

//01. == C 系統文件/C++ 系統文件
#include <queue>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <gadget\file_operator.h>
#include <aintersect.inl>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "scene2d.h"


namespace GSO
{
    ///////////////////////////////////////////////////////////////////////////////
    // surface class
    int  Surface_Compare_Name( const tchar_t* filename );
    bool Surface_Add_Path( const tchar_t* fpath, int f, void* data );
    int  Surface_Make_Path_Table( const tchar_t* src_path, int factorial = 0 );
    std::vector<const tchar_t*>  all_file_path_table;

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    Scene2d::Scene2d() : EntityPool()
    {
        FileSystem folder_path;
        folder_path.Scanfiles( ".", Surface_Add_Path );
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool DeleteEntity( unsigned int order, void* pEntity, void* pData )
    {
        delete (Element2d *)pEntity;
        return TRUE;
    }

    Scene2d::~Scene2d()
    {
        //DeleteAllElem();
        ForAllEntityBuffer( DeleteEntity, NULL );
        return;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    LG::Rect Scene2d::GetRect( void )
    {
        LG::Rect rect;
        /*
            rect.top = m_Origin.y;
            rect.left = m_Origin.x;
            rect.right = m_Origin.x + (GetCellWidth() * GetCols());
            rect.bottom = m_Origin.y + (GetCellHeight() * GetRows());
            */
        return rect;
    }

    /*
    //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
    void Scene2d::PasteTerrain(LGB::CMemCanvas* canvas)
    {
        if((m_pRender == NULL)  && (m_pRender->GetWidget() == NULL))
            return;
        for(uint_t i = 0; i<m_pLoadedCellVec.size(); i++)
        {
            LG::Point situ = m_pLoadedCellVec[i];
            Element2d* elem = (Element2d*)GetCellData(situ.x, situ.y);
               if((elem != NULL) && (elem->Visible))
                   m_pRender->Blit(elem->Animation(),
                                 elem->ScreenPos(), canvas);
        }
    }
    */
    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool Scene2d::IsElementEmpty( void )
    {
        return IsEntityEmpty();
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void Scene2d::InsertElement( Element2d* elem )
    {
        //elem->m_pOrigin = &m_Origin;
        elem->m_Descriptor = EntityPool::InsertEntity( elem );
        return;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void Scene2d::RemoveElement( Element2d* elem )
    {
        EntityPool::RemoveEntityD( elem->m_Descriptor );
        //elem->m_pOrigin = NULL;
        elem->m_Descriptor = NULL;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void Scene2d::SetFocusElement( Element2d* elem )
    {
        if ( elem == NULL )
            EntityPool::SetFocusEntity( NULL );
        else
            EntityPool::SetFocusEntity( elem->m_Descriptor );
        return;
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    Element2d* Scene2d::GetFocusElement( void )
    {
        return (Element2d *)EntityPool::GetFocusEntity();
    }

    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    void Scene2d::ElementClear( void )
    {
        EntityPool::EntityClear();
    }

    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    void Scene2d::ForAllElement( OnEntityBuffer udels, void* pData )
    {
        ForAllEntityBuffer( udels, pData );
    }

    //==========================================================================================
    // NAME:
    // DESC:
    //==========================================================================================
    class PosYSort
    {
    public:
        PosYSort() {}
        bool operator() ( Element2d* elem1, Element2d* elem2 ) const;
    };

    bool PosYSort::operator() ( Element2d* elem1, Element2d* elem2 ) const
    {
        if ( elem1->GetPos().y > elem2->GetPos().y )
        {
            return true;
        }
        else if ( elem1->GetPos().y == elem2->GetPos().y )
        {
            if ( elem1->GetPos().x > elem2->GetPos().x )
            {
                return true;
            }
        }
        return false;
    }

    Element2d* Scene2d::GetElemBySel( const LG::Point& mapPos )
    {
        std::priority_queue<Element2d*, std::vector<Element2d*>, PosYSort> sortedQueue;

        Element2d* picked = 0;
        unsigned int count = EntityCount();
        for ( unsigned int i = 0; i < count; ++i )
        {
            picked = reinterpret_cast<Element2d*>( operator[]( i ) );
            if ( picked && picked->IsCollisionByImage( mapPos ) )
            {
                sortedQueue.push( picked );
            }
        }

        if ( !sortedQueue.empty() )
        {
            return sortedQueue.top();
        }

        return 0;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    //template <class void*>
    void Scene2d::Link( const tchar_t* fname )
    {
        int ret = Surface_Compare_Name( fname );
        if ( ret == -1 )
            return;


        //Release();
        //EntityEraseAll();
        EntityClear();
        //m_pEntitySeqs.clear();

        //Load(all_file_path_table[ret]);

        return;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    int Surface_Compare_Name( const tchar_t* filename )
    {
        tchar_t drive[ _MAX_DRIVE ];
        tchar_t dir[ _MAX_LDIR ];
        tchar_t fname[ _MAX_LFNAME ];
        tchar_t ext[ _MAX_LEXT ];

        for ( uint_t i = 0; i < all_file_path_table.size(); i++ )
        {
            _splitpath( all_file_path_table[ i ], drive, dir, fname, ext );
            strcat( fname, ext );
            if ( strcmp( fname, filename ) == 0 )
                return i;
        }
        return -1;
    }


    //=============================================================================
    // NAME: 
    // DESC:
    //=============================================================================
    bool Surface_Add_Path( const tchar_t* fpath, int f, void* data )
    {
        if ( strstr( fpath, _T( ".pin" ) ) || strstr( fpath, _T( ".PIN" ) ) ||
            strstr( fpath, _T( ".pinz" ) ) || strstr( fpath, _T( ".PINZ" ) ) ||
            strstr( fpath, _T( ".pelm" ) ) || strstr( fpath, _T( ".PELM" ) ) ||
            strstr( fpath, _T( ".pmap" ) ) || strstr( fpath, _T( ".pmap" ) ) )
        {
            all_file_path_table.push_back( fpath );
        }
        return true;
    }




    /*
       //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
       void Scene2d::SetFocusElem(Element2d* elem)
       {
           if(elem != NULL)
               SetFocusEntity(elem->m_Descriptor);
           else
               SetFocusEntity(NULL);
           return;
       }
       //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
       Element2d* Scene2d::GetFocusElem(void)
       {
           return (Element2d *) GetFocusEntity();
       }
    */

    /*
       //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
       void Scene2d::PasteEntity(int i, CPIcon* canvas)
       {
           if(m_pRender == NULL)
               return;
           if(m_pRender->GetWidget() != NULL)
               m_pRender->Blit(((Element2d *) Entity(i))->Animation(),
                                ((Element2d *) Entity(i))->ScreenPos(),
                                canvas);
           return;
       }
       //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
       void Scene2d::PasteBackGround(CPIcon* canvas)
       {
           if(m_pRender == NULL)
               return;
           if((Visible) && (m_pRender->GetWidget() != NULL))
               m_pRender->AlphaBlit(Animation(), ScreenPos(), canvas);
           return ;
       }
       //=============================================================================
       // NAME: Flip_Scene()
       // DESC: exchange two scene
       //=============================================================================
       void Scene2d::Flip_Scene(CPIcon* canvas, const tchar_t* path, int offSet)
       {
       }
       //=============================================================================
       // NAME: Drag_To()
       // DESC: compute new drag to situs (use for mouse drag)
       //=============================================================================
       void Scene2d::DragTo(GVertex mpos)
       {
           Element2d::DragTo(mpos);
           //GVertex offSet(new_norm.x - m_Situs.x, new_norm.y - m_Situs.y);
           //m_Situs = new_norm;
           mpos = mpos - Pos();  // because in scene Norm == (0 , 0)
           for(Puint i = 0;i <EntityCount();i++)
           {
               ((Element2d *) Entity(i))->DragTo(mpos);
           }
           //Norm() = m_Situs;
       }
       //=============================================================================
       // NAME: Scroll_To()
       // DESC: compute element situs when map scroll (use for keydown)
       //=============================================================================
       void Scene2d::Scroll_To(GVertex new_norm)
       {
           //GVertex offSet(new_norm.x - m_Situs.x, new_norm.y - m_Situs.y);
           m_Situs = new_norm;
           for(int i = 0; i <Unit_Count(); i++)
           {
               ((Element2d*)m_pUnitSeqs[i])->Scroll_To(m_Situs);
           }
           //m_Situs   = new_norm;
           //Norm() = new_norm;
           return;
       }
       //========================================================================================
       // NAME: Load_BG()
       // DESC: load a elem file to be background
       //========================================================================================
       int Scene2d::LoadBackGround(const tchar_t* path)
       {
           //CPFile pf(path, "rb");
           return Element2d::Load(path);
       }
       //=============================================================================
       // NAME:
       // DESC:
       //=============================================================================
    #define _MAP_RECT_LIMIT_ 100
       void Scene2d::Collide_Scroll(POINT point)
       {
           if(m_pRender->GetWidget() == NULL)
               return;
           GVertex mouse_pt;
           GetCursorPos(&mouse_pt);
           ScreenToClient((HWND)m_pRender->GetWidget(), &mouse_pt);
           LG::Rect client_rect;
           GetClientRect((HWND)m_pRender->GetWidget(), &client_rect);
           if(PA::Intersect::Test(&client_rect, &mouse_pt))
           {
               if(point.x <_MAP_RECT_LIMIT_)
               {
                   if(Norm().x <0)
                   {
                       GVertex spt(Norm().x + 10, Norm().y);
                       Pos() = spt;
                       point.x = _MAP_RECT_LIMIT_;
                   }
               }else if(point.x> m_pRender->GetWidth() - _MAP_RECT_LIMIT_)
               {
                   if(Norm().x> (m_pRender->GetWidth() - ImageWidth()))
                   {
                       GVertex spt(Norm().x - 10, Norm().y);
                       Pos() = spt;
                       point.x = m_pRender->GetWidth() - _MAP_RECT_LIMIT_;
                   }
               }
               if(point.y <_MAP_RECT_LIMIT_)
               {
                   if(Norm().y <0)
                   {
                       GVertex spt(Norm().x, Norm().y + 10);
                       Pos() = spt;
                       point.y = _MAP_RECT_LIMIT_;
                   }
               }else if(point.y> (m_pRender->GetHeight() - _MAP_RECT_LIMIT_))
               {
                   if(Norm().y> (m_pRender->GetHeight() - ImageHeight()))
                   {
                       GVertex spt(Norm().x, Norm().y - 10);
                       Pos() = spt;
                       point.y = m_pRender->GetHeight() - _MAP_RECT_LIMIT_;
                   }
               }
               ClientToScreen((HWND) m_pRender->GetWidget(), &point);
               SetCursorPos(point.x, point.y);
           }
           return;
       }
        */
};//namespace GE

