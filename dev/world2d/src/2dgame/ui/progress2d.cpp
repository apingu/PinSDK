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
#include "Progress2d.h"
#include <parser/pgsp_file.h>
#include <pf-string.h>

namespace GSO
{
    namespace UI
    {

        Progress2d::Bar2d::Bar2d()
        {
            RenderMethod = LGB::DrawNode::RSClipAlpha;
            m_MaxValue = 100;
            m_CurrValue = 0;
            m_ImageSize(0, 0);
        }


        Progress2d::Bar2d::~Bar2d()
        {

        }

        LG::Rect Progress2d::Bar2d::OnGetRenderRect(void)
        {
            LG::Rect rect;
            if (Animation())
            {
                int value = 0;
                if (m_MaxValue != 0)
                {
                    value = (m_ImageSize.cx * m_CurrValue) / m_MaxValue;
                }
                if (value > m_ImageSize.cx)
                    value = m_ImageSize.cx;
                RenderTop=0;
                RenderRight=value;
                RenderLeft=0;
                RenderBottom=m_ImageSize.cy;
                //rect(0, 0, value, m_ImageSize.cy);
                //render->AlphaClipperBlit(icon, rect, GetPos(), canvas);
            }
            return rect;
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Progress2d::Progress2d()
        {
            m_ValueBar= new Bar2d;
            m_ValueBar->SetId(1);
            BaseClass::AttachItem(m_ValueBar);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Progress2d::~Progress2d()
        {

        }


    
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        error_t Progress2d::LoadRes(ProgressCom com, const tchar_t* filename)
        {
            error_t ret = 1;
            if(com==PgCPanel)
            {
                ret = Element2d::LoadRes(filename);
            }
            else
            {
                ret = m_ValueBar->LoadRes(filename);
                m_ValueBar->m_ImageSize.cx = m_ValueBar->GetClipImageWidth();
                m_ValueBar->m_ImageSize.cy = m_ValueBar->GetClipImageHeight();
                //置中
                LG::Point pos = GetPos();
                pos.x+= (GetClipImageWidth()-m_ValueBar->GetClipImageWidth())/2;
                pos.y+= (GetClipImageHeight()-m_ValueBar->GetClipImageHeight())/2;
            }

            return ret;
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        error_t Progress2d::LoadAnm(ProgressCom com, const tchar_t* filename)
        {
            error_t ret = 1;
            if(com==PgCPanel)
            {
                ret = Element2d::LoadAnm(filename);
            }
            else
            {
                ret = m_ValueBar->LoadAnm(filename);
            }

            return ret;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        IWidget2d* Progress2d::GetComponentButton(ProgressCom id)
        {
            if(id==PgCBar)
                return m_ValueBar;
            else if(id==PgCPanel)
                return this;
            return NULL;
        }

        //=============================================================================
        // NAME: 
        // DESC: 改最大值
        //=============================================================================
        void Progress2d::SetMaxStep(int value)
        {
            m_ValueBar->m_MaxValue = value;
            m_ValueBar->OnGetRenderRect();
        }

        //=============================================================================
        // NAME: 
        // DESC: 設定目前值
        //=============================================================================
        void Progress2d::SetCurrStep(int value)
        {
            m_ValueBar->m_CurrValue = value;
            m_ValueBar->OnGetRenderRect();
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        int Progress2d::GetMaxStep(void)
        {
            return m_ValueBar->m_MaxValue;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        int Progress2d::GetCurrStep(void)
        {
            return m_ValueBar->m_CurrValue;
        }


        //=============================================================================
        // NAME: 
        // DESC: Virtual function of IWidget2d        
        //=============================================================================        
        // 存檔用. 將屬性寫入 node ===============================================
        bool Progress2d::SaveCfg(GspNode *toNode)
        {
            tchar_t buf[255];
            const tchar_t* pstr;
            pfstd::CString name = GetResfname();

            // 改大寫
            tchar_t* objname = new tchar_t[strlen(ObjectName())+1];
            strcpy(objname, ObjectName());
            strtoupper(objname);

            GspNode* item = new GspNode(objname, m_Expression.c_str());
            SAFE_DELETE_ARRAY(objname);

            toNode->AttachChildNode(item);
            // 共通部分
            //if(!m_Expression.empty())
            //    item->WriteCommand(_T("EXPRESSION"), _T("="), m_Expression.c_str());
            lltoa(GetId(), buf, 10);
            item->WriteCommand(_T("ID"), _T("="), buf);
            lltoa(GetGId(), buf, 10);
            item->WriteCommand(_T("GID"), _T("="), buf);
            sprintf(buf, _T("%d,%d,%d"), GetPos().x, GetPos().y, GetPos().z);
            item->WriteCommand(_T("POSITION"), _T("="), buf);
            if(!m_ResoureName.empty())
                item->WriteCommand(_T("RESIDNAME"), _T("="), m_ResoureName.c_str());



            name = GetResfname();
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADPANELRES"), _T("="), name);
            }

            name = GetAnmfname() ;
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADPANELANM"), _T("=") , name);
            }

            name = m_ValueBar->GetResfname() ;
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADBARRES"), _T("="), name);
            }

            name = m_ValueBar->GetAnmfname();
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADBARANM"), _T("=") , name);
            }
           
            tNode *txtnode;
            
            for(int i = 0; 1; i++)
            {
                txtnode = GetTextNode(i);
                if(!txtnode)
                    break;
#ifndef UNICODE
                if(NULL==(pstr=txtnode->m_Text.c_str()))
                    pstr=" ";
#else
                if(NULL==(pstr=(const tchar_t*)txtnode->m_Text.c_str()))    // 這邊的處理方式並不正確, 但是目前並不重要
                    pstr=" ";
#endif
                sprintf(buf,_T("%s,%d,%d,%d,%d,%d,%d"), pstr,
                    txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                    txtnode->m_TextRect.left,txtnode->m_TextRect.right,
                    txtnode->m_ForeColor,txtnode->m_BackColor);
                item->WriteCommand(_T("TEXT"), _T("="), buf);
            }

            item->WriteCommand(_T("SETMAXSTEP"), _T("=") , itoa(GetMaxStep(), buf, 10));
            item->WriteCommand(_T("SETCURRSTEP"), _T("=") , itoa(GetCurrStep(), buf, 10));

            sprintf(buf,_T("%d,%d"), m_Anim2d.GetPlayMode(), m_Anim2d.GetPlayInterrupt());
            item->WriteCommand(_T("PLAYMODE"), _T("=") , buf);

            sprintf(buf,_T("%d"), RenderAlpha);
            item->WriteCommand(_T("ALPHA"), _T("=") , buf);

            sprintf(buf,_T("%d"), RenderLayer);
            item->WriteCommand(_T("LAYER"), _T("=") , buf);

            sprintf(buf, _T("%d,%d"), GetPopupType(), GetPopupMsec());
            item->WriteCommand(_T("POPUPTYPE"), _T("="), buf);

            sprintf(buf, _T("%d"), GetDragable());
            item->WriteCommand(_T("DRAGABLE"), _T("="), buf);

            sprintf(buf, _T("%d"), GetVisible());
            item->WriteCommand(_T("VISIBLE"), _T("="), buf);

            sprintf(buf, _T("%d"), GetEnable());
            item->WriteCommand(_T("ENABLE"), _T("="), buf);

    
            return true;
        }



        /*
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Progress2d::SetCurrValueText(void)
        {
            char buff[100];
            sprintf(buff, "%d / %d", m_CurrValue, m_MaxValue);
            SetTextNode(0, buff, GetPos().y, +GetPos().y+15 ,GetPos().x, GetPos().x+200);
        }
        */

        /*
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void Progress2d::BlitImage(LGB::VMC* render,CPIcon* canvas)
        {
            CPIcon* icon = Animation();
            if (icon)
            {
                int value = 0;
                if (m_MaxValue != 0)
                    value = m_ImageSize.cx * m_CurrValue / m_MaxValue;
                if (value > m_ImageSize.cx)
                    value = m_ImageSize.cx;
                LG::Rect rect(0, 0, value, m_ImageSize.cy);
                render->AlphaClipperBlit(icon, rect, GetPos(), canvas);
            }
        }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================


    };
};




