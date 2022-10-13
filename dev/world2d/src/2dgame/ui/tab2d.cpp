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
#include "Tab2d.h"

namespace GSO
{
    namespace UI
    {
        /*
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Tab2d::Page2d::OnNull(LG::Point point)
        {
            SetState(kStateNormal);
            BaseClass::OnNull(point);
        }
        
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Tab2d::Page2d::OnLMouseDown(int key, LG::Point point)
        {
            SetState(kStatePressDown);
            BaseClass::OnLMouseDown(key, point);
        }
        */
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        void Tab2d::Page2d::DragTo(GameVertex newpos)
        {
            IWidget2d* TabWidget = (IWidget2d*)GetParent();//->DragTo(newpos);
            for (int i = TabWidget->GetItemCount() - 1; i >= 0; --i)
            {
                IWidget2d* wid=(*TabWidget)[i];
                if(strcmp(wid->ObjectName(), "Page2d") == 0)
                {
                    wid->DragTo(newpos);
                }
            }
        }
        */


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Tab2d::Tab2d()
        {
            m_SelPage=-1;        
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Tab2d::~Tab2d()
        {
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        uint_t Tab2d::GetSelPage(void)
        {
            return m_SelPage;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Tab2d::SetSelPage(uint_t id)
        {
            if(GetItemCount() == 0)
                return;

            //if(m_SelPage == id)
                //return;

            int firstpage=-1;
            m_SelPage=-1;

            for (int i = (m_Pages.size()-1); i >= 0; i--)
            {
                if(strcmp(m_Pages[i]->ObjectName(), _T("Page2d")) == 0)
                {
                    firstpage = i;
                    Page2d* wid=(Page2d*)m_Pages[i];
                    if(id== i)
                    {
                        wid->SetState(kStatePressDown);
                        m_SelPage = i;
                        //DetachChild(wid);
                        DetachItem(wid);
                         //AttachChild(wid);
                        AttachItem(wid);
                    }
                    else
                    {
                        wid->SetState(kStateNormal);
                    }
                }
            }

            if(m_SelPage==-1)
            {
                if(firstpage==-1)
                    return;
                Page2d* wid=(Page2d*)operator[](firstpage);
                m_SelPage = firstpage;
                wid->SetState(kStatePressDown);
                //DetachChild(wid);
                //AttachChild(wid);
                DetachItem(wid);
                AttachItem(wid);

            }
        
        }
        

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        void Tab2d::AttachPage(const tchar_t* fname)
        {
            Page2d* page = new Page2d;
            page->LoadRes(fname);
            AttachItem(page);
            return;
        }
        */

        bool Tab2d::AttachItem(IWidget2d* item)
        {
            m_Pages.push_back((Page2d*)item);
            bool ret = BaseClass::AttachItem(item);
            //SetSelPage(0);
            //OnLMouseDown(operator[](0), 0, LG::Point());
            return ret;
        }

        bool Tab2d::DetachItem(IWidget2d* item)
        {
            for(uint_t i = 0; i< m_Pages.size(); i++)
            {
                if(m_Pages[i]==item)
                {
                    m_Pages.erase(m_Pages.begin()+i);
                    if(m_SelPage==i)
                        m_SelPage = -1;

                    break;
                }
            }

            bool ret = BaseClass::DetachItem(item);
            //if(page->GetState()==kStatePressDown)
            //SetSelPage(0);
            //OnLMouseDown(operator[](0), 0, LG::Point());
            return ret;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Tab2d::AttachPage(Page2d* page)
        {
            return Tab2d::AttachItem((Page2d*)page);
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool Tab2d::DetachPage(Page2d* page)
        {
            return Tab2d::DetachItem((Page2d*)page);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        uint_t Tab2d::GetPageCount(void) const
        {
            return GSO::UI::IWidget2d::GetItemCount();
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Tab2d::OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite)
        {
            if(strcmp(pWidget->ObjectName(), _T("Page2d")) == 0)
            {
                m_SelPage = 0;
                for (int i = GetItemCount() - 1; i >= 0; --i)
                {
                    Page2d* wid=(Page2d*)operator[](i);
                    if(wid==pWidget)
                    {
                        m_SelPage = i;
                        pWidget->SetState(kStatePressDown);
                        //DetachChild(pWidget);
                        //AttachChild(pWidget);
                        DetachItem(pWidget);
                        AttachItem(pWidget);

                    }
                    else
                        wid->SetState(kStateNormal);
                }
            }
            IWidget2d::OnLMouseDown(pWidget, KeyCode, MouseSite);

        }
        
       //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Tab2d::OnLMouseUp(IWidget2d* pWidget, int KeyCode,  LG::Point MouseSite)
        {
            IWidget2d::OnLMouseUp(pWidget, KeyCode, MouseSite);
            /*
            for (int i = GetItemCount() - 1; i >= 0; --i)
            {
                if (operator[](i)->IsFocus())
                {
                    Element2d* elem = operator[](i);
                    DetachChild(elem);
                    AttachChild(elem);
                    return;
                }
            }
            */
        }

    };
};