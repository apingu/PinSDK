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
#include "BtnArray2d.h"
#include "PGSPFile.h"
#include <fString.h>
//#include <aIntersect.inl>

namespace GSO
{
    namespace UI
    {
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        BtnArray2d::BtnArray2d()
        {
            m_Amount( 0, 0 );         // 二維陣列的xy
            m_Space( 0, 0 );          // 每個子元件的間距，分x間距和y間距
            m_ItemSize( 0, 0 );
            m_FocusItem = NULL;
            m_Mask = NULL;
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        BtnArray2d::~BtnArray2d()
        {
            Release();
        }
        //=============================================================================
        //  Name : 
        //  Desc : 設定其陣列大小，並初始空間
        //=============================================================================
        void BtnArray2d::Create( int x,int y )
        {
            Release();

            UINT size = x* y;
            m_Amount( x, y );
            for ( UINT i = 0; i < size; ++i )
            {
                Button2d* btn = new Button2d;
                btn->SetId( i );
                AttachItem( btn );
            }
            m_Mask = new Button2d;
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        bool BtnArray2d::InsertCell( UINT x,UINT y,IWidget2d* w )
        {
            int index = y* m_Amount.cx + x;
            if ( index >= GetItemCount() )
                return false;

            IWidget2d* widget = operator[]( index );
            if ( widget == NULL )
                return false;
            w->SetId( widget->GetId() );
            ReplaceChild( widget, w );
            m_InsertSeq.push_back( w );
            delete widget;
            return true;
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::AttachItem( IWidget2d* item )
        {
            m_InsertSeq.push_back( item );
            IWidget2d::AttachItem( item );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::DetachItem( IWidget2d* item )
        {
            for ( UINT i = 0; i < m_InsertSeq.size(); i++ )
            {
                if ( m_InsertSeq[i] == item )
                {
                    m_InsertSeq.erase( m_InsertSeq.begin() + i );
                    break;
                }
            }
            IWidget2d::DetachItem( item );
        }

        //=============================================================================
        //  Name : 
        //  Desc : 把前景的圖全部release
        //=============================================================================
        void BtnArray2d::ReleaseFrontImage( void )
        {
            for ( unsigned int i = 0; i < GetItemCount(); ++i )
            {
                IWidget2d* widget = operator[]( i );
                widget->Release();
            }
        }

        //=============================================================================
        //  Name : 
        //  Desc : 和create成對，將之前new出來的空間釋放
        //=============================================================================
        void BtnArray2d::Release( void )
        {
            m_Amount( 0, 0 );
            while ( GetItemCount() != 0 )
            {
                IWidget2d* widget = BaseClass::operator[]( 0 );
                DetachChild( widget );
                SAFE_DELETE( widget );
            }
            SAFE_DELETE( m_Mask );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        IWidget2d* BtnArray2d::GetCell( UINT x,UINT y )
        {
            return m_InsertSeq[y * m_Amount.cx + x];
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        IWidget2d* BtnArray2d::GetCell( GameVertex mousepos )
        {
            int px = mousepos.x - m_Position.x;
            int py = mousepos.y - m_Position.y;
            if ( ( px < 0 ) || ( py < 0 ) )
                return 0;
            int cx = px / ( m_ItemSize.cx + m_Space.cx );
            int cy = py / ( m_ItemSize.cy + m_Space.cy );
            int count = cx + cy* m_Amount.cx;
            if ( count >= GetItemCount() )
                return 0;
            return ( IWidget2d* ) m_InsertSeq[count];
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        IWidget2d* BtnArray2d::operator[]( UINT i )
        {
            return ( IWidget2d * ) m_InsertSeq[i];
        }

        //=============================================================================
        //  Name : 
        //  Desc : 將前景全部載入同一張圖
        //=============================================================================
        void BtnArray2d::LoadAllCellRes( const char* filename )
        {
            for ( unsigned int i = 0 ; i < GetItemCount() ; ++i )
            {
                operator[]( i )->LoadRes( filename );
                operator[]( i )->SetId( i );
            }
            return;
        }
        //=============================================================================
        //  Name : 
        //  Desc : 對指定對象進行載入的動作，如果傳入null字串，則釋放該指定對象(ps，如果傳空字串，則指定對象不改變)
        //=============================================================================
        void BtnArray2d::LoadCellRes( UINT x,UINT y,char* filename )
        {
            int index = y* m_Amount.cx + x;

            if ( index >= GetItemCount() )
                return;

            if ( filename )
                m_InsertSeq[index]->LoadRes( filename );
            else
                m_InsertSeq[index]->Release();
            //UpdateCellPos();

        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::LoadAllHint( char* filename )
        {
            for ( unsigned int i = 0 ; i < GetItemCount() ; ++i )
            {
                operator[]( i )->LoadHint( filename );
            }
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::LoadAllDesc( char* filename )
        {
            for ( unsigned int i = 0 ; i < GetItemCount() ; ++i )
            {
                operator[]( i )->LoadDesc( filename );
            }
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::LoadMaskRes( char* filename )
        {
            m_Mask->LoadRes( filename );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		const char* BtnArray2d::GetMaskRes( void )
		{
			return m_Mask->GetResfname();
		}

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::SetText( UINT x,UINT y,TCHAR* str,UINT index )
        {
            GetCell( x, y )->SetText( str, index );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::SetText( UINT id,TCHAR* str,UINT index )
        {
            operator[]( id )->SetText( str, index );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        const TCHAR* BtnArray2d::GetText( UINT x,UINT y,UINT index )
        {
            return GetCell( x, y )->GetText( index );
        }

		void BtnArray2d::RenderTo( rendercontainer_t& container )
		{
            if ( !GetVisible() )
                return;

			BaseClass::RenderTo(container);

			if( m_Mask!= NULL )
				container.push_back(m_Mask);
//                renderer.InsertNode( m_Mask, LGB::DrawNode::kLayerUI );
		}

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		int BtnArray2d::Update( int count )
		{
			BaseClass::Update();
			UpdateCellPos();
   		    return 1;
		}

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		void BtnArray2d::SetSpace( int x, int y )
		{
			m_Space( x, y );
		}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
		LG::Size BtnArray2d::GetSpace( void )
		{
			return m_Space;
		}

        //=============================================================================
        //  Name :
        //  Desc :
        //=============================================================================
        void BtnArray2d::SetItemSize(int cx, int cy)
		{
			m_ItemSize(cx, cy);
		}

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		LG::Size BtnArray2d::GetItemSize( void )
		{
			return m_ItemSize;
		}

        //=============================================================================
        //  Name : 
        //  Desc : 取得致能子元件的index
        //=============================================================================
		int BtnArray2d::GetFocusIndex( void )
		{
			return m_FocusItem->GetId();
		}


        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		LG::Size BtnArray2d::GetAmount( void )
		{
			return m_Amount;
		}

        //=============================================================================
        //  Name : 
        //  Desc : Virtual function of IWidget2d        
        //=============================================================================		
		// 存檔用. 將屬性寫入 node ===============================================
		bool BtnArray2d::Save(GspNode *toNode)
		{
            char buf[255];
			const char *pstr;
            const char* name = ( char* )GetFileName();

            GspNode* item = new GspNode( ObjectName() );
			toNode->AttachChild( item );
			// 共通部分
            itoa( GetId(), buf, 10 );
            item->WriteCommand( "ID", "=", buf );
            itoa( GetGId(), buf, 10 );
            item->WriteCommand( "GID", "=", buf );
            sprintf( buf, "%d,%d", GetPos().x, GetPos().y );
            item->WriteCommand( "POSITION", "=", buf );

			sprintf( buf, "%d,%d", GetAmount().cx, GetAmount().cy );
			item->WriteCommand( "ARRAY", "=", buf );

			sprintf( buf, "%d,%d", GetItemSize().cx, GetItemSize().cy );
			item->WriteCommand( "ITEMSIZE", "=", buf );

			sprintf( buf, "%d,%d", GetSpace().cx, GetSpace().cy );
			item->WriteCommand( "SPACE", "=", buf );
				

			if( ( name = GetResfname() ) != NULL )
			{
			    const char* currname = strdivstr( name, Sprite2d::GetCurrentDirectory() );
				if( currname != NULL )
					name=currname;
				item->WriteCommand( "LOADRES", "=", name );
			}

			
			if( ( name = GetAnmName() ) != NULL )
			{
			    const char* currname = strdivstr( name, Sprite2d::GetCurrentDirectory() );
				if( currname != NULL )
					name=currname;
				item->WriteCommand( "LOADANM", "=" , name );
			}

			if( GetMaskRes()!=NULL )
				item->WriteCommand( "LOADMASK", "=", GetMaskRes() );


			if( operator[](0)!=NULL )
			{
				if( ( name = operator[](0)->GetResfname() ) != NULL )
				{
					const char* currname = strdivstr( name, Sprite2d::GetCurrentDirectory() );
					if( currname != NULL )
						name=currname;
					item->WriteCommand( "LOADALLCELLRES", "=", name );
				}
			}


			tNode *txtnode;
			
			for( int i=0; 1; i++ )
			{
				txtnode = GetTextNode(i);
				if(!txtnode)
					break;
#ifndef UNICODE
				if(NULL==(pstr=txtnode->m_Text.c_str()))
					pstr=" ";
#else
				if(NULL==(pstr=(const char *)txtnode->m_Text.c_str()))	// 這邊的處理方式並不正確, 但是目前並不重要
					pstr=" ";
#endif
				sprintf(buf,"%s,%d,%d,%d,%d,%d,%d", pstr,
					txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
					txtnode->m_TextRect.left,txtnode->m_TextRect.right,
					txtnode->m_ForeColor,txtnode->m_BackColor);
				item->WriteCommand( "TEXT", "=", buf );
			}


//            for ( i = 0; i < GetItemCount(); i++ )
//            {
//				IWidget2d* widget = operator[]( i );
//				widget->Save( item );
//            }

			return true;

		
			return true;
		}



        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        //
        /*
        virtual void OnDrag( LG::Point point )
        {
            if(m_ActiveBtn != NULL)
            {
                m_Focus = _GET_FOCUS;
                m_pBtn[m_ActiveBtn->Id]->Visible = false;//DragTo( point );
            }
        }
        // 
        virtual void OnDragDrop( LG::Point point )
        {
            if(m_ActiveBtn != NULL)
            {
                m_Focus = _GET_FOCUS;
                m_pBtn[m_ActiveBtn->Id]->Visible = true;    
                //m_pBtn[m_Count]->SetPos( GetPos().x+ImageWidth()*GetCount()->x, GetPos().y+ImageHeight()*GetCount()->y  );
            }
        }
        // 
        virtual void DragTo( GameVertex new_norm )
        {
            Element2d::DragTo( new_norm );
            //m_Position = new_norm - m_tmp_focus_elm_relative_pos;
            UpdateData();
        }
        */


        // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
        // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
        // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
        // 但還是要往上送，所以要再呼叫base_class的onxxx
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnKeyDown( IWidget2d* widget,int KeyCode )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnKeyDown( widget, KeyCode );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnKeyUp( IWidget2d* widget,int KeyCode )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnKeyUp( widget, KeyCode );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnNormal( IWidget2d* widget,
                                   LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnNormal( widget, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		/*
        void BtnArray2d::OnMouseMove( IWidget2d* widget,
                                      int KeyCode,
                                      LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnMouseMove( widget, KeyCode, MouseSite );
        }
		*/
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnMouseOver( IWidget2d* widget,
                                      int KeyCode,
                                      LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() ); 
            BaseClass::OnMouseOver( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnLDBClick( IWidget2d* widget,
                                     int KeyCode,
                                     LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnLDBClick( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnRDBClick( IWidget2d* widget,
                                     int KeyCode,
                                     LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnRDBClick( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnLMouseDown( IWidget2d* widget,
                                       int KeyCode,
                                       LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnLMouseDown( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnRMouseDown( IWidget2d* widget,
                                       int KeyCode,
                                       LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() );
            BaseClass::OnRMouseDown( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnLMouseUp( IWidget2d* widget,
                                     int KeyCode,
                                     LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() ); 
            BaseClass::OnLMouseUp( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnRMouseUp( IWidget2d* widget,
                                     int KeyCode,
                                     LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() ); 
            BaseClass::OnRMouseUp( widget, KeyCode, MouseSite );
        }
        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnDrag( IWidget2d* widget,LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() ); 
            BaseClass::OnDrag( widget, MouseSite );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        void BtnArray2d::OnDragDrop( IWidget2d* widget,LG::Point MouseSite )
        {
            m_FocusItem = widget;
            widget->SetGId( GetId() ); 
            BaseClass::OnDragDrop( widget, MouseSite );
        }

        //=============================================================================
        //  Name : 
        //  Desc : 更新所有子元件的位置
        //=============================================================================
        void BtnArray2d::UpdateCellPos( void )
        {
            IWidget2d* widget = NULL;
            LG::Point pos = m_Position;
            if ( m_ItemSize.cx == 0 )
                m_ItemSize = LG::Size( 32, 32 );
            for ( int i = 0 ; i < m_Amount.cy ; ++i )
            {
                pos.x = m_Position.x;
                for ( int j = 0 ; j < m_Amount.cx ; ++j )
                {
                    UINT index = i* m_Amount.cx + j;
                    widget = m_InsertSeq[index];
                    if ( widget->GetCurEvenMsg() != PE_MOUSEDRAG )
                        widget->SetPos( pos );
                    pos.x += m_ItemSize.cx + m_Space.cx;
                }
                pos.y += m_ItemSize.cy + m_Space.cy;
            }

            if ( m_FocusItem != NULL )
                m_Mask->SetPos( m_FocusItem->GetPos() );
        }

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
		/*
        void BtnArray2d::OnSortChilds( std::vector<GSO::Element2d*>& childs )
        {
            for ( UINT i = 0; i < childs.size(); i++ )
            {
                if ( childs[i]->IsFocus() )
                {
                    Element2d* elem = childs[i];
                    DetachChild( elem );
                    AttachChild( elem );
                    m_Focus = TRUE;
                    return;
                }
            }
        }
		*/

        //=============================================================================
        //  Name : 
        //  Desc :
        //=============================================================================
        LG::Size BtnArray2d::GetSize( void )
        {
            return LG::Size( ( m_ItemSize.cx + m_Space.cx ) * m_Amount.cx,
                             ( m_ItemSize.cy + m_Space.cy ) * m_Amount.cy );
        }
    };
};



