/*
 *  File:       Tab2d.h
 *  Desc:       
 *  Creater:    Kent
 *  Create:     
 *  Updater:    Sean
 *  Update:     04/09/02
 *  Desc:       
 */

#ifndef TAB2D_H
#define TAB2D_H

#include "Button2d.h"

namespace GSO
{
    namespace UI
    {
        class Tab2d : public GSO::UI::IWidget2d
        {
        public:
            class Page2d;

        private:
            typedef IWidget2d BaseClass;
            int                  m_SelPage;
               std::vector<Page2d*> m_Pages;

        public:
            class Page2d : public GSO::UI::IWidget2d
            {
                typedef IWidget2d BaseClass;
            public:
                Page2d(){};
                ~Page2d(){};

                const tchar_t*  ObjectName() const    { return _T("Page2d"); }
            };


        public:
            Tab2d();
            ~Tab2d();

            const tchar_t*  ObjectName() const        { return _T("Tab2d"); }
            //const char* GSPName() const {    return "2DTAB";    }
            
            //virtual void AttachPage(const char* fname);
            uint_t            GetSelPage(void);
            void              SetSelPage(uint_t i);
                              
            bool              AttachItem(IWidget2d* item);
            bool              DetachItem(IWidget2d* item);
                              
            bool              AttachPage(Page2d* page);
            bool              DetachPage(Page2d* page);
                              
            uint_t              GetPageCount  (void) const;


            // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
            // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
            // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
            // 但還是要往上送，所以要再呼叫base_class的onxxx

            // 限制使用
            //IWidget2d*         operator[]    (uint_t i)          {return NULL;}
            //uint_t             GetItemCount  (void) const        {return 0;}
            IWidget2d*         GetItem       (entityid_t index)  {return NULL;}
            bool               ReplaceItem   (const IWidget2d* oldc, IWidget2d* newc){return false;}

        protected:

            //virtual void OnSortChilds(std::vector<GSO::Element2d*>& childs){}
            virtual void       OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void       OnLMouseUp(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);

            // Input事件，客端程式請勿覆寫，寫底層子元件視情況覆寫
            /*
            virtual void OnNull(LG::Point point)          {}
            virtual void OnKeyDown(int key)                {} 
            virtual void OnKeyUp(int key)                    {} 
            virtual void OnMouseOver(int key, LG::Point point)    {}
            virtual void OnLMouseDown(int key, LG::Point point)    {}
            virtual void OnRMouseDown(int key, LG::Point point)    {}
            virtual void OnLMouseUp(int key, LG::Point point)        {}
            virtual void OnRMouseUp(int key, LG::Point point)        {}
            virtual void OnLDBClick(int key, LG::Point point)        {}
            virtual void OnRDBClick(int key, LG::Point point)        {}
            virtual void OnDrag(LG::Point point)                    {}
            virtual void OnDragDrop(LG::Point point)                {}
            */


        };
    };
};



#endif