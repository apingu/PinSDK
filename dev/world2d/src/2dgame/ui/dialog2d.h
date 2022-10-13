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
//
//
//
#ifndef DIALOG2D_H
#define DIALOG2D_H

#include "IWidget2d.h"
#include "Static2d.h"
#include "Button2d.h"
#include "Edit2d.h"
#include "ItemArray2d.h"
#include "Scroll2d.h"
#include "Edit2d.h"
#include "Tab2d.h"
#include "Menu2d.h"
#include "Progress2d.h"
#include "AminBtn2d.h"
//#include "FreeRect2d.h"
//#include "Map2d.h"
#include "RichText2d.h"
#include "Number2d.h"

namespace GSO
{
    namespace UI
    {
        class Dialog2d : public IWidget2d
        {
            GameVertex     m_OgrineUI;
            char           m_CfgFilename[_MAX_LPATH];

        public:

            Dialog2d();
            ~Dialog2d();

            const tchar_t*  ObjectName() const { return _T("Dialog2d"); }


            virtual error_t SaveCfg(const tchar_t* filename);
            // 請在客端程式裡載入對應的gsp後，呼叫form2d的AttachItem(自己)
            virtual error_t LoadCfg(const tchar_t* filename);
            void            Destroy();        // 會delete

            virtual void    Clear(int s=0);//release res

            const char*     GetCfgFilename(void);
                        
            // 內部往上派送機制，任何子元件不得覆寫，客端程式僅能覆寫Dialog的以下界面
            // 請覆寫dialog衍生類的這些功能，並請將功能寫在這些覆寫的界面中
            // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
            // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
            // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
            // 但還是要往上送，所以要再呼叫base_class的onxxx
            virtual void     OnLoad      (void){};
            /*
            virtual void OnNormal    (IWidget2d* pWidget, LG::Point MouseSite){}
            virtual void OnMouseOver (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnLDBClick  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnRDBClick  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnRMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnLMouseUp  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnRMouseUp  (IWidget2d* pWidget, int KeyCode, LG::Point MouseSite){}
            virtual void OnDrag      (IWidget2d* pWidget, LG::Point MouseSite){}
            virtual void OnDragDrop  (IWidget2d* pWidget, LG::Point MouseSite){}
            virtual void OnDock      (IWidget2d* pWidget, dword_t docktime, LG::Point point){}
            virtual void OnShow      (IWidget2d* pWidget, bool f){}

            //leap over, if want to use, must SetTestLODrag first
            virtual void OnLODragIn  (IWidget2d* pWidget, IWidget2d* recvobj, LG::Point MouseSite){}
            virtual void OnLODragOut (IWidget2d* pWidget, IWidget2d* sendobj, LG::Point MouseSite, bool is_catched){}
            */
        private:

            /*
            // Input事件，客端程式請勿覆寫，寫底層子元件才可覆寫
            virtual void OnNull      (LG::Point point);
            virtual void OnMouseOver (int key, LG::Point point); 
            virtual void OnLMouseDown(int key, LG::Point point); 
            virtual void OnRMouseDown(int key, LG::Point point); 
            virtual void OnLMouseUp  (int key, LG::Point point); 
            virtual void OnRMouseUp  (int key, LG::Point point); 
            virtual void OnLDBClick  (int key, LG::Point point); 
            virtual void OnRDBClick  (int key, LG::Point point);
            virtual void OnDrag      (LG::Point point);
            virtual void OnDragDrop  (LG::Point point); 
            virtual void OnDock      (dword_t docktime, LG::Point point);
            virtual void OnShow      (bool f);
            */
        };
    };//namespace GUI
};//namespace GE



#endif