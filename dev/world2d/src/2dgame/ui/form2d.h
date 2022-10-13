//CForm.h header file of CForm.cpp
//
// User InterFace Class
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

#ifndef FORM2D_H
#define FORM2D_H

#pragma warning (disable : 4786)


#include "Dialog2d.h"
#include "MsgBox2d.h"
#include <ivmc-render.h>

// Forward Declarations
namespace LGB
{
    class IVMCRender;
}

namespace GSO
{
    namespace UI
    {
        enum BasicButton { kWinBKG = 100,
                           kWinTitle,
                           kWinClose,
                           kWinSize,
                           kMsgBKG = 200,
                           kMsgOk,
                           kMsgCancel,
                         };
                         
        // 目前Form2d的作為僅在管理dialogs的I/O，及部分的狀態
        class Form2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;
        public:
        
            Form2d();            
            ~Form2d();
            
            const tchar_t*      ObjectName() const { return _T( "Form2d" ); }
            virtual void        DoModal( IWidget2d* dlg );
            void                Destroy();        // 會delete
            
            //
            Dialog2d*           GetFocusDialog( void );
            //
            void                SetFocusDialog( Dialog2d* dlg );
            Dialog2d*           GetDialog( int dialog_id );
            void                ShowDialog( int dialog_id, bool flag = true );
            void                ShowAll( bool flag = true );
            bool                IsDialogVisible( int dialog_id );
            
            GSO::UI::IWidget2d* GetTriggerWidget( LG::Point pfstl );
            
            
            void                CreateMessageBox( void* btn, tchar_t* elem );
            void                MessageBox( tchar_t* str );
            
            void                Sort( void );
            virtual bool        Query( LG::Point point, uint_t uMsg, uint_t wParam );   //各元素的事件
            
            int                 GetWidgetCount() {return GetItemCount();}
            
            virtual void        DrawTo( LGB::IVMCRender* render );
            
            
        protected:
            bool                m_IsModal;
            Dialog2d*           m_FocusDialog;
            
        private:
        
            virtual void        Clear( int s = 0 );
            // 對外不給用
            virtual error_t     LoadRes( const tchar_t* elmname ) {return -1;}     //open elem
            virtual error_t     LoadAnm( const tchar_t* anmname ) {return -1;}     //open elem
        };
    };//namespace GUI
};//namespace GE

#endif