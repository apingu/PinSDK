//Role.cpp game world role class
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
//                                                                  Pin
//
#include "Button2d.h"

namespace GSO
{
    namespace UI
    {
    
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Button2d::Button2d()
        {
            m_StateFname.resize( kStateMax );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Button2d::~Button2d()
        {
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::SetStateName( uibehave_t state, const tchar_t* name )
        {
            m_StateFname[state] = name;
            return;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::SetEnable( bool f )
        {
            BaseClass::SetEnable( f );
            
            if ( f )
            { SetState( kStateNormal ); }
            else
            { SetState( kStateDisable ); }
        }
        
        
        // Input事件，客端程式請勿覆寫
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnNull( LG::Point point )
        {
            //if (GetEnable())
            {
                /*
                if(GetId() == 2010)
                {
                    tchar_t buff[255];
                    sprintf(buff, "On Mouse null now =%d, new =%d", m_Anim2d.GetNowAngle(), kStateCross);
                    OutputDebugString(buff);
                }
                */
                SetState( kStateNormal );
                BaseClass::OnNull( point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnMouseOver( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                /*
                tchar_t buff[255];
                sprintf(buff, "On Mouse Over now =%d, new =%d", m_Anim2d.GetNowAngle(), kStateCross);
                OutputDebugString(buff);
                */
                SetState( kStateCross );
                BaseClass::OnMouseOver( key, point );
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnLMouseDown( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStatePressDown );
                BaseClass::OnLMouseDown( key, point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnRMouseDown( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStatePressDown );
                BaseClass::OnRMouseDown( key, point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnLMouseUp( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStateCross );
                BaseClass::OnLMouseUp( key, point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnRMouseUp( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStateCross );
                BaseClass::OnRMouseUp( key, point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnLDBClick( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStatePressDown );
                BaseClass::OnLDBClick( key, point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnRDBClick( int key, LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStatePressDown );
                BaseClass::OnRDBClick( key, point );
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnDrag( LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStateCross );
                BaseClass::OnDrag( point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnDragDrop( LG::Point point )
        {
            //if (GetEnable())
            {
                SetState( kStateNormal );
                BaseClass::OnDragDrop( point );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnKeyDown( int key )
        {
            //if (GetEnable())
            {
                SetState( kStatePressDown );
                BaseClass::OnKeyDown( key );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Button2d::OnKeyUp( int key )
        {
            //if (GetEnable())
            {
                SetState( kStateCross );
                BaseClass::OnKeyUp( key );
            }
        }
        
        
        ///////////////////////////////////////////////////////////////////////////////
        // Check box
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        CheckBox2d::CheckBox2d()
        {
            m_Checked = false;
        }
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        CheckBox2d::~CheckBox2d()
        {
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool CheckBox2d::GetCheck( void )
        {
            return m_Checked;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::SetCheck( bool b )
        {
            m_Checked = b;
            
            if ( m_Checked )
            { SetState( kStatePressDown ); }
            else
            { SetState( kStateNormal ); }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC: because we must change the image state to enable state, so we overwrite it
        //=============================================================================
        void CheckBox2d::SetEnable( bool f )
        {
            IWidget2d::SetEnable( f );
            
            if ( !f )
            {
                SetState( kStateDisable );
            }
            else
            {
                if ( m_Checked )
                { SetState( kStatePressDown ); }
                else
                { SetState( kStateNormal ); }
            }
        }
        
        
        
        // Input事件，客端程式請勿覆寫
        
        /*
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnNull(LG::Point point)
        {
            return;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnMouseOver(int key, LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnMouseOver(this, key, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnLMouseDown(int key, LG::Point point)
        {
        
            //m_Checked=!m_Checked;
            ((IWidget2d *) GetParent())->OnLMouseDown(this, key, point);
        }
        */
        /*
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnRMouseDown(int key, LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnRMouseDown(this, key, point);
        }
        */
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnLMouseDown( int key, LG::Point point )
        {
            SetCheck( m_Checked = !m_Checked );
            ( ( IWidget2d* ) GetParent() )->OnLMouseDown( this, key, point );
        }
        
        /*
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnRMouseUp(int key, LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnRMouseUp(this, key, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnLDBClick(int key, LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnLDBClick(this, key, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnRDBClick(int key, LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnRDBClick(this, key, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnDrag(LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnDrag(this, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnDragDrop(LG::Point point)
        {
            ((IWidget2d *) GetParent())->OnDragDrop(this, point);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnKeyDown(int key)
        {
            ((IWidget2d *) GetParent())->OnKeyDown(this, key);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void CheckBox2d::OnKeyUp(int key)
        {
            ((IWidget2d *) GetParent())->OnKeyUp(this, key);
        }
        */
        
        
    };//namespace GUI
};//namespace GE

