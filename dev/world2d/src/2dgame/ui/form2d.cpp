//Form.cpp user interface class
//
// FUNCTION : 遊戲使用者介面物件
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
#pragma warning (disable : 4786)
#include <pf-debug.h>
#include <gadget\file_operator.h>

#include "Form2d.h"


namespace GSO
{
    namespace UI
    {
#define _G2BUTTON(e)     ((Button2d*)e)
        //////////////////////////////////////////////////////////////////////////////////////////////
        //  Form2d
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Form2d::Form2d()
        {
            m_IsModal = false;
            m_FocusDialog = 0;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Form2d::~Form2d()
        {}
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool Form2d::IsDialogVisible( int dialog_id )
        {
            if ( GetDialog( dialog_id ) != 0 )
            {
                return GetDialog( dialog_id )->GetVisible();
            }
            
            return false;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::DoModal( IWidget2d* dlg )
        {
            //m_FocusDialog = dlg;
            if ( dlg != 0 )
            {
                IWidget2d::SetFocusWidget( dlg );
                m_IsModal = true;
            }
            else
            {
                m_IsModal = false;
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::Clear( int s )
        {
            IWidget2d::Clear( s );
            return;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::Destroy()
        {
            Clear();
            Initiate();
            DestroyAllItem();
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Dialog2d* Form2d::GetFocusDialog( void )
        {
            return m_FocusDialog;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::SetFocusDialog( Dialog2d* dlg )
        {
            m_FocusDialog = dlg;
            
            if ( dlg == 0 )
            {
                return;
            }
            
            SetFocusWidget( dlg );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Dialog2d* Form2d::GetDialog( int dialog_id )
        {
            return ( Dialog2d* ) GetItem( dialog_id );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::ShowDialog( int dialog_id, bool flag )
        {
            if ( GetDialog( dialog_id ) == 0 )
            {
                return;
            }
            
            SetFocusDialog( GetDialog( dialog_id ) );
            GetDialog( dialog_id )->SetVisible( flag );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::ShowAll( bool flag )
        {
            for ( uint_t i = 0; i < GetItemCount(); i++ )
            {
                operator[]( i )->SetVisible( flag );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        //找拖到誰家
        GSO::Element2d* SearchTestTouch( GSO::Element2d* elem, void* pData )
        {
            GSO::Element2d* p = elem;
            
            while ( p != NULL )
            {
                if ( !p->GetVisible() )
                {
                    return NULL;
                }
                
                p = p->GetParent();
            }
            
            //            if (elem->GetVisible() == false)
            //                return NULL;
            LG::Point* pfstl = ( LG::Point* )pData;
            
            if ( elem->GetCurEvenMsg() == EventMessage::MOUSEDRAGDROP )
            {
                return NULL;
            }
            
            if ( elem->IsCollisionByImage( *pfstl ) == TRUE )
            {
                return elem;
            }
            
            return NULL;
        }
        
        GSO::UI::IWidget2d* Form2d::GetTriggerWidget( LG::Point pfstl )
        {
            return ( GSO::UI::IWidget2d* )ForAllChildren( SearchTestTouch, &pfstl );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::CreateMessageBox( void* btn, tchar_t* elem )
        {}
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Form2d::MessageBox( tchar_t* str )
        {}
        
        //==========================================================================================
        // NAME: Sort()
        // DESC: sort Dialog
        //==========================================================================================
        void Form2d::Sort( void )
        {
            //return;
            if ( GetFocusDialog() == 0 )
            {
                return;
            }
            
            // search
            uint_t count = GetItemCount();
            
            for ( uint_t i = 0; i < count; i++ )
            {
                if ( GetFocusDialog() == operator[]( i ) )
                {
                    DetachItem( GetFocusDialog() );
                    //AttachChild(GetFocusDialog());
                    AttachItem( GetFocusDialog() );
                    return;
                }
            }
        }
        
        
        
        //=============================================================================
        // NAME: Query()
        // DESC: Query for button has event and set select element addr as m_select_unit
        //=============================================================================
        bool Form2d::Query( LG::Point point, uint_t uMsg, uint_t wParam )   //各元素的事件
        {
            EventMessage message = EventMessage::NONE;
            bool ran = false;
            
            if ( ( !GetVisible() ) || ( !GetEnable() ) )
            {
                return false;
            }
            
            if ( GetItemCount() == 0 )
            {
                return false;
            }
            
            /*
            // 先問 focus pWidget 的狀況
            IWidget2d* focuswidget = GetFocusWidget();
            if(focuswidget)
            {
                if(focuswidget->GetMainElement()==this)
                {
                    if(focuswidget->GetVisible())
                    {
                        message=focuswidget->Proc(point, uMsg, wParam);
                    }
                }
            }
            */
            
            if ( message != EventMessage::NONE )
            {
                return true;
            }
            
            for ( int i = ( GetItemCount() - 1 ); i >= 0; i-- )
            {
                Dialog2d* dlg = ( Dialog2d* )operator[]( i );
                
                if ( dlg == NULL )
                {
                    continue;
                }
                
                if ( !m_IsModal )
                {
                    message = (EventMessage)dlg->Proc( point, uMsg, wParam );
                }
                else
                {
                    continue;
                }
                
                if ( message == EventMessage::NONE )
                {
                    continue;
                }
                else if ( message == EventMessage::LBUTTONDOWN )
                {
                    SetFocusDialog( dlg );
                    Sort();
                    return true;
                }
                
                if ( message != EventMessage::NONE )
                {
                    return true;
                }
            }
            
            return Proc( point, uMsg, wParam );
        }
        
        
        // Interfaces of DrawNode
        //==========================================================================================
        // NAME: RenderTo()
        // DESC:
        //=========================================================================================
        void Form2d::DrawTo( LGB::IVMCRender* render )
        {
            for ( uint_t i = 0; i < GetItemCount(); i++ )
            {
                Dialog2d* dlg = ( Dialog2d* )operator[]( i );
                
                if ( dlg == NULL )
                {
                    continue;
                }
                
                if ( dlg->GetVisible() )
                {
                    render->DoRender( dlg );
                }
            }
        }
        
    };//namespace GUI
};//namespace GE

