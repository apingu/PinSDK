//Role.h header file of Role.cpp
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

#ifndef BUTTON2D_H
#define BUTTON2D_H

#pragma warning (disable : 4786)

//#include <dynarray.inl>
//#include "Render2d.h"
#include "IWidget2d.h"

namespace GSO
{
    namespace UI
    {
        class Button2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;
            std::vector<pfstd::CString> m_StateFname;
            
        public:
            Button2d();
            ~Button2d();
            
            const tchar_t* ObjectName() const { return _T( "Button2d" ); }
            
            //set the state read filename()
            virtual void   SetStateName( uibehave_t state, const tchar_t* name );
            
            //because we must change the image state to enable state, so we overwrite it
            virtual void   SetEnable( bool f );
            
            
        protected:
            // Input事件，客端程式請勿覆寫，寫底層子元件視情況覆寫
            virtual void OnNull( LG::Point point );
            virtual void OnKeyDown( int key );
            virtual void OnKeyUp( int key );
            
            virtual void OnMouseOver( int key, LG::Point point );
            virtual void OnLMouseDown( int key, LG::Point point );
            virtual void OnRMouseDown( int key, LG::Point point );
            virtual void OnLMouseUp( int key, LG::Point point );
            virtual void OnRMouseUp( int key, LG::Point point );
            virtual void OnLDBClick( int key, LG::Point point );
            virtual void OnRDBClick( int key, LG::Point point );
            virtual void OnDrag( LG::Point point );
            virtual void OnDragDrop( LG::Point point );
        };
        
        
        
        ///////////////////////////////////////////////////////////////////////////////
        //Check box
        
        class CheckBox2d : public IWidget2d
        {
            typedef Button2d BaseClass;
            
        private:
        
            bool    m_Checked;
            
        public:
        
            CheckBox2d();
            ~CheckBox2d();
            
            const tchar_t* ObjectName() const { return _T( "CheckBox2d" ); }
            
            bool           GetCheck( void );
            void           SetCheck( bool b );
            
            //because we must change the image state to enable state, so we overwrite it
            virtual void   SetEnable( bool f );
            
            
        private:
            // Input事件，客端程式請勿覆寫，寫底層子元件視情況覆寫
            //virtual    void OnNull(LG::Point point);
            //virtual void OnKeyDown(int key);
            //virtual void OnKeyUp(int key);
            //virtual void OnMouseOver(int key, LG::Point point);
            //virtual void OnLMouseDown(int key, LG::Point point);
            //virtual void OnRMouseDown(int key, LG::Point point);
            virtual void OnLMouseDown( int key, LG::Point point );
            //virtual void OnRMouseUp(int key, LG::Point point);
            //virtual void OnLDBClick(int key, LG::Point point);
            //virtual void OnRDBClick(int key, LG::Point point);
            //virtual void OnDrag(LG::Point point);
            //virtual void OnDragDrop(LG::Point point);
        };
    };//namespace GUI
};//namespace GE






#endif