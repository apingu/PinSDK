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
#include "ItemArray2d.h"
#include <parser/pgsp_file.h>
#include <pf-string.h>
//#include <aintersect.inl>

namespace GSO
{
    namespace UI
    {
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        ItemArray2d::ItemArray2d()
        {
            m_Amount(0, 0);         // 二維陣列的xy
            m_Space(0, 0);          // 每個子元件的間距，分x間距和y間距
            m_ItemSize(0, 0);
            m_FocusItem = NULL;
            m_Mask=new Button2d;
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        ItemArray2d::~ItemArray2d()
        {
            SAFE_DELETE(m_Mask);
        }
        //=============================================================================
        // NAME: 
        // DESC: 設定其陣列大小，並初始空間
        //=============================================================================
        void ItemArray2d::Create(int x, int y, const char* subtype)
        {
            DestroyAllItem();

            uint_t size = x* y;
            m_Amount(x, y);
            if(subtype !=NULL )
            {
                if(strcmp(subtype, _T("Button2d")) == 0)
                {
                    for (uint_t i = 0; i < size; ++i)
                    {
                        Button2d* btn = new Button2d;
                        btn->SetId(i);
                        AttachItem(btn);
                    }
                }
            }
            else
            {
                for (uint_t i = 0; i < size; ++i)
                {
                    Static2d* btn = new Static2d;
                    btn->SetId(i);
                    AttachItem(btn);
                }
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool ItemArray2d::InsertCell(uint_t x,uint_t y,IWidget2d* w)
        {
            int index = y* m_Amount.cx + x;
            if (index >= GetItemCount())
                return false;

            IWidget2d* pWidget = operator[](index);
            if (pWidget == NULL)
                return false;
            w->SetId(pWidget->GetId());
            ReplaceItem(pWidget, w);
            m_InsertSeq[index]=w;
            delete pWidget;
            return true;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::RemoveCell(IWidget2d* w)
        {
            if(m_FocusItem==w)
                m_FocusItem=NULL;
            for (uint_t i = 0; i < m_InsertSeq.size(); i++)
            {
                if (m_InsertSeq[i] == w)
                {
                    m_InsertSeq.erase(m_InsertSeq.begin() + i);
                    break;
                }
            }
            IWidget2d::DetachItem(w);
        }

        //=============================================================================
        // NAME: 
        // DESC: 這裡繼承只是為了讓她變 Private
        //=============================================================================
        bool ItemArray2d::AttachItem(IWidget2d* item)
        {
            // 超過設定的大小
            uint_t size = m_Amount.cx* m_Amount.cy;
            bool ins=false;
            for (uint_t i = 0; i < m_InsertSeq.size(); i++)
            {
                if (m_InsertSeq[i] == NULL)
                {
                    m_InsertSeq[i]=item;
                    ins=true;
                    break;
                }
            }
            if(!ins)
            {
                if(m_InsertSeq.size()<size)
                {
                    m_InsertSeq.push_back(item);
                    ins=true;
                }
            }
            if(ins)
                return IWidget2d::AttachItem(item);
            return false;            
        }

        //=============================================================================
        // NAME: 
        // DESC: 這裡繼承只是為了讓她變 Private
        //=============================================================================
        bool ItemArray2d::DetachItem(IWidget2d* item)
        {
            //這個不能擺在這邊 不然位置會依直變動
            for (uint_t i = 0; i < m_InsertSeq.size(); i++)
            {
                if (m_InsertSeq[i] == item)
                {
                    m_InsertSeq[i]=NULL;
                    break;
                }
            }
            return IWidget2d::DetachItem(item);
        }

        //=============================================================================
        // NAME: 
        // DESC: 把前景的圖全部release
        //=============================================================================
        void ItemArray2d::ClearAllCellImage(void)
        {
            for (unsigned int i = 0; i < GetItemCount(); ++i)
            {
                IWidget2d* pWidget = operator[](i);
                pWidget->Clear();
            }
        }

        //=============================================================================
        // NAME: 
        // DESC: 和create成對，將之前new出來的空間釋放
        //=============================================================================
        void ItemArray2d::DestroyAllItem(void)
        {
            m_Amount(0, 0);
            IWidget2d::DestroyAllItem();
            m_InsertSeq.clear();
            //SAFE_DELETE(m_Mask);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        IWidget2d* ItemArray2d::GetCell(uint_t x,uint_t y)
        {
            return m_InsertSeq[y * m_Amount.cx + x];
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        IWidget2d* ItemArray2d::GetCell(GameVertex mousepos)
        {
            int px = mousepos.x - m_Position.x;
            int py = mousepos.y - m_Position.y;
            if ((px < 0) || (py < 0))
                return 0;
            int cx = px / (m_ItemSize.cx + m_Space.cx);
            int cy = py / (m_ItemSize.cy + m_Space.cy);
            int count = cx + cy* m_Amount.cx;
            if (count >= GetItemCount())
                return 0;
            return (IWidget2d*) m_InsertSeq[count];
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool ItemArray2d::IsCell (IWidget2d* w)
        {
            if(w->GetParent()==this)
                return true;
            return false;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        IWidget2d* ItemArray2d::operator[](uint_t i)
        {
            if(m_InsertSeq.empty())
                return NULL;
            return (IWidget2d *) m_InsertSeq[i];
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void  ItemArray2d::SetCellDragable(bool t)
        {
            for(uint_t i = 0; i<m_InsertSeq.size(); i++)
            {
                m_InsertSeq[i]->SetDragable(t);
            }
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::SetCellAcceptDragIn(bool t)
        {
            for(uint_t i = 0; i<m_InsertSeq.size(); i++)
            {
                m_InsertSeq[i]->SetTestLODrag(t);
            }
        }

        //=============================================================================
        // NAME: 
        // DESC: 將前景全部載入同一張圖
        //=============================================================================
        void ItemArray2d::LoadAllCellRes(const tchar_t* filename)
        {
            for (unsigned int i = 0 ; i < GetItemCount() ; ++i)
            {
                operator[](i)->LoadRes(filename);
                operator[](i)->SetId(i);
            }
            return;
        }

        void ItemArray2d::LoadAllCellAnm(const tchar_t* filename)
        {
            for (unsigned int i = 0 ; i < GetItemCount() ; ++i)
            {
                operator[](i)->LoadAnm(filename);
            }
            return;
        }

        //=============================================================================
        // NAME: 
        // DESC: 對指定對象進行載入的動作，如果傳入null字串，則釋放該指定對象(ps，如果傳空字串，則指定對象不改變)
        //=============================================================================
        void ItemArray2d::LoadCellRes(uint_t x,uint_t y,tchar_t* filename)
        {
            int index = y* m_Amount.cx + x;

            if (index >= GetItemCount())
                return;

            if (filename)
                m_InsertSeq[index]->LoadRes(filename);
            else
                m_InsertSeq[index]->Clear();
            //UpdateCellPos();

        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::LoadAllHint(tchar_t* resfname, tchar_t* anmfname)
        {
            for (unsigned int i = 0 ; i < GetItemCount() ; ++i)
            {
                if(resfname)
                    operator[](i)->LoadHintRes(resfname);
                if(anmfname)
                    operator[](i)->LoadHintAnm(anmfname);

            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::LoadAllDesc(tchar_t* filename)
        {
            for (unsigned int i = 0 ; i < GetItemCount() ; ++i)
            {
                operator[](i)->LoadDesc(filename);
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::LoadMaskRes(tchar_t* filename)
        {
            m_Mask->LoadRes(filename);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        pfstd::CString ItemArray2d::GetMaskRes(void)
        {
            return m_Mask->GetResfname();
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::SetText(uint_t x,uint_t y,tchar_t* str,uint_t index)
        {
            GetCell(x, y)->SetText(str, index);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::SetText(uint_t id,tchar_t* str,uint_t index)
        {
            operator[](id)->SetText(str, index);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        const tchar_t* ItemArray2d::GetText(uint_t x,uint_t y,uint_t index)
        {
            return GetCell(x, y)->GetText(index);
        }

        void ItemArray2d::OnRenderTo(rendercontainer_t &container)
        {
            if (!GetVisible())
                return;

            BaseClass::OnRenderTo(container);
            /*
            DrawNode::OnRenderTo(container);
            for(uint_t i = 0; i<m_InsertSeq.size(); i++)
            {
                m_InsertSeq[i]->OnRenderTo(container);
            }
            */

            if(m_FocusItem !=NULL )
            {
                if(m_Mask->GetImageCount()>0)
                    container.push_back(m_Mask);
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        int ItemArray2d::Update(int count)
        {
            BaseClass::Update();
            UpdateCellPos();
               return 1;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::SetSpace(int x, int y)
        {
            m_Space(x, y);
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        LG::Size ItemArray2d::GetSpace(void)
        {
            return m_Space;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void ItemArray2d::SetItemSize(int cx, int cy)
        {
            m_ItemSize(cx, cy);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        LG::Size ItemArray2d::GetItemSize(void)
        {
            return m_ItemSize;
        }

        //=============================================================================
        // NAME: 
        // DESC: 取得致能子元件的index
        //=============================================================================
        entityid_t ItemArray2d::GetFocusIndex(void)
        {
            return m_FocusItem->GetId();
        }


        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        LG::Size ItemArray2d::GetAmount(void)
        {
            return m_Amount;
        }

        //=============================================================================
        // NAME: 
        // DESC: Virtual function of IWidget2d        
        //=============================================================================        
        // 存檔用. 將屬性寫入 node ===============================================
        bool ItemArray2d::SaveCfg(GspNode *toNode)
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


            //這個要擺在前面
            name = GetResfname();
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADRES"), _T("="), name);
            }


            name = GetAnmfname();
            if(!name.empty())
            {
                const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                if(currname != NULL)
                    name=currname;
                item->WriteCommand(_T("LOADANM"), _T("=") , name);
            }

            if(!GetMaskRes().empty())
                item->WriteCommand(_T("LOADMASK"), _T("="), GetMaskRes());



            sprintf(buf, _T("%d,%d"), GetAmount().cx, GetAmount().cy);
            item->WriteCommand(_T("ARRAY"), _T("="), buf);

            sprintf(buf, _T("%d,%d"), GetItemSize().cx, GetItemSize().cy);
            item->WriteCommand(_T("ITEMSIZE"), _T("="), buf);

            sprintf(buf, _T("%d,%d"), GetSpace().cx, GetSpace().cy);
            item->WriteCommand(_T("SPACE"), _T("="), buf);

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



            if(operator[](0) !=NULL )
            {
                if((name = operator[](0)->GetResfname()) != NULL)
                {
                    const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                    if(currname != NULL)
                        name=currname;
                    item->WriteCommand(_T("LOADALLCELLRES"), _T("="), name);
                }

                if((name = operator[](0)->GetAnmfname()) != NULL)
                {
                    const tchar_t* currname = strsplitstr(name, Sprite2d::GetCurrentDirectory());
                    if(currname != NULL)
                        name=currname;
                    item->WriteCommand(_T("LOADALLCELLANM"), _T("="), name);
                }

                int celldragable=operator[](0)->GetDragable();
                sprintf(buf, _T("%d"), celldragable);
                item->WriteCommand(_T("CELLDRAGABLE"), _T("="), buf);
            }


            tNode *txtnode;
            
            for(int i = 0; 1; i++)
            {
                txtnode = GetTextNode(i);
                if(!txtnode)
                    break;
#ifndef UNICODE
                if(NULL==(pstr=txtnode->m_Text.c_str()))
                    pstr=_T(" ");
#else
                if(NULL==(pstr=(const char*)txtnode->m_Text.c_str()))    // 這邊的處理方式並不正確, 但是目前並不重要
                    pstr=_T(" ");
#endif
                sprintf(buf,_T("%s,%d,%d,%d,%d,%d,%d"), pstr,
                    txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                    txtnode->m_TextRect.left,txtnode->m_TextRect.right,
                    txtnode->m_ForeColor,txtnode->m_BackColor);
                item->WriteCommand(_T("TEXT"), _T("="), buf);
            }


//            for (i = 0; i < GetItemCount(); i++)
//            {
//                IWidget2d* pWidget = operator[](i);
//                pWidget->Save(item);
//            }

    
            return true;
        }



        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        //
        /*
        virtual void OnDrag(LG::Point point)
        {
            if(m_ActiveBtn != NULL)
            {
                m_Focus = _GET_FOCUS;
                m_pBtn[m_ActiveBtn->Id]->Visible = false;//DragTo(point);
            }
        }
        // 
        virtual void OnDragDrop(LG::Point point)
        {
            if(m_ActiveBtn != NULL)
            {
                m_Focus = _GET_FOCUS;
                m_pBtn[m_ActiveBtn->Id]->Visible = true;    
                //m_pBtn[m_Count]->SetPos(GetPos().x+ImageWidth()*GetCount()->x, GetPos().y+ImageHeight()*GetCount()->y);
            }
        }
        // 
        virtual void DragTo(GameVertex new_norm)
        {
            Element2d::DragTo(new_norm);
            //m_Position = new_norm - m_tmp_focus_elm_relative_pos;
            UpdateData();
        }
        */


        // 內部往上派送機制，客端程式僅能覆寫Dialog的以下界面
        // 任何子元件不需要覆寫，通常是在編寫複合元件時，才會有複寫的必要
        // 在複合元件的覆寫，是為了要攔劫自己的子元件被觸發時的訊息
        // 但還是要往上送，所以要再呼叫base_class的onxxx
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnKeyDown(IWidget2d* pWidget,int KeyCode)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnKeyDown(pWidget, KeyCode);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnKeyUp(IWidget2d* pWidget,int KeyCode)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnKeyUp(pWidget, KeyCode);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnNormal(IWidget2d* pWidget,
                                   LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnNormal(pWidget, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        void ItemArray2d::OnMouseMove(IWidget2d* pWidget,
                                      int KeyCode,
                                      LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            pWidget->SetGId(GetId());
            BaseClass::OnMouseMove(pWidget, KeyCode, MouseSite);
        }
        */
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnMouseOver(IWidget2d* pWidget,
                                      int KeyCode,
                                      LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId()); 
            BaseClass::OnMouseOver(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnLDBClick(IWidget2d* pWidget,
                                     int KeyCode,
                                     LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnLDBClick(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnRDBClick(IWidget2d* pWidget,
                                     int KeyCode,
                                     LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnRDBClick(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnLMouseDown(IWidget2d* pWidget,
                                       int KeyCode,
                                       LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnLMouseDown(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnRMouseDown(IWidget2d* pWidget,
                                       int KeyCode,
                                       LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId());
            BaseClass::OnRMouseDown(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnLMouseUp(IWidget2d* pWidget,
                                     int KeyCode,
                                     LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId()); 
            BaseClass::OnLMouseUp(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnRMouseUp(IWidget2d* pWidget,
                                     int KeyCode,
                                     LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId()); 
            BaseClass::OnRMouseUp(pWidget, KeyCode, MouseSite);
        }
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnDrag(IWidget2d* pWidget,LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId()); 
            BaseClass::OnDrag(pWidget, MouseSite);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        void ItemArray2d::OnDragDrop(IWidget2d* pWidget,LG::Point MouseSite)
        {
            m_FocusItem = pWidget;
            //pWidget->SetGId(GetId()); 
            BaseClass::OnDragDrop(pWidget, MouseSite);
        }

        //=============================================================================
        // NAME: 
        // DESC: 更新所有子元件的位置
        //=============================================================================
        void ItemArray2d::UpdateCellPos(void)
        {
            IWidget2d* pWidget = NULL;
            LG::Point pos = m_Position;
            if (m_ItemSize.cx == 0)
                m_ItemSize = LG::Size(32, 32);
            for (int i = 0 ; i < m_Amount.cy ; ++i)
            {
                pos.x = m_Position.x;
                for (int j = 0 ; j < m_Amount.cx ; ++j)
                {
                    uint_t index = i* m_Amount.cx + j;
                    pWidget = m_InsertSeq[index];
                    if(pWidget)
                    {
                        if (pWidget->GetCurEvenMsg() != EventMessage::MOUSEDRAG)
                            pWidget->SetPos(pos);
                    }
                    pos.x += m_ItemSize.cx + m_Space.cx;
                }
                pos.y += m_ItemSize.cy + m_Space.cy;
            }

            if (m_FocusItem != NULL)
            {
                m_Mask->SetPos(m_FocusItem->GetPos());
            }
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        /*
        void ItemArray2d::OnSortChilds(std::vector<GSO::Element2d*>& childs)
        {
            for (uint_t i = 0; i < childs.size(); i++)
            {
                if (childs[i]->IsFocus())
                {
                    Element2d* elem = childs[i];
                    DetachChild(elem);
                    AttachChild(elem);
                    m_Focus = TRUE;
                    return;
                }
            }
        }
        */

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        LG::Size ItemArray2d::GetSize(void)
        {
            return LG::Size((m_ItemSize.cx + m_Space.cx) * m_Amount.cx,
                             (m_ItemSize.cy + m_Space.cy) * m_Amount.cy);
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        bool ItemArray2d::OnCollision(GameVertex pos)
        {
            if(GetImageCount() == 0)
            {
                LG::Size st= GetSize();
                RECT rt;
                rt.left  =GetPos().x;
                rt.top   =GetPos().y;
                rt.right =GetPos().x+st.cx;
                rt.bottom=GetPos().y+st.cy;
                if((BETWEEN(rt.left, pos.x, rt.right))&&
                    (BETWEEN(rt.top,  pos.y, rt.bottom)))
                {
                    return true;
                }
                return false;
            }
            return IsCollisionByImage(pos); 
        }

        //=============================================================================
        // NAME: 
        // DESC: 因為ITEMARRAY底下的CHILD　ＩＤ　有特殊性，所以不給找
        //=============================================================================
    };
};



