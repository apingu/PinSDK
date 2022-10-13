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
#include "ientity2d.h"
#include "terrain2d.h"
//#include <T_Lock.h> 

namespace GSO
{
    namespace World
    {
        //T_Lock g_Lock;
        GameVertex     IEntity2d::m_Origin;

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IEntity2d::IEntity2d()
        {
            m_Inhabit = NULL;
        }

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IEntity2d::~IEntity2d()
        {
            BreakInteraction();
            if(m_Inhabit != NULL)
            {
                m_Inhabit->ExitEntity(this);
            }
            m_Inhabit = NULL;
        }


        //=============================================================================
        // NAME: 
        //  Desc 建立影響
        //=============================================================================
        void IEntity2d::BuildInteraction(IEntity2d* target)
        {
            // TODO: Pin 會當 先不要做
            return;
            //TS_Lock lock(&g_Lock);
            if(target == NULL)
                return;
            
            m_TargetSeq.insert(std::set<IEntity2d*>::value_type(target));
            target->m_TargetSeq.insert(std::set<IEntity2d*>::value_type(this));
            //if (m_TargetSeq.find(target) < 0)  // if had build relative, we won't build againe
                //m_TargetSeq.push_back(target);
            //if (target->m_TargetSeq.find(this) < 0)  // if had build relative, we won't build againe
                //target->m_TargetSeq.push_back(this);
            return;
        }


        //=============================================================================
        // NAME: Relative_Target_Sever()
        // DESC: break relation with all Unit
        //=============================================================================
        void IEntity2d::BreakInteraction(void)
        {
            // TODO: Pin 會當 先不要做
            return;
            //TS_Lock lock(&g_Lock);
            std::set< IEntity2d* >::iterator iter=m_TargetSeq.begin();
            while(iter!=m_TargetSeq.end())
            {
//                IEntity2d* entity = (*iter);
//                std::set< IEntity2d* >::iterator deliter = entity->m_TargetSeq.find(this);
//                if(deliter!=entity->m_TargetSeq.end())
//                    entity->m_TargetSeq.erase(deliter);

                (*iter)->m_TargetSeq.erase(this);
                ++iter;
            }

            m_TargetSeq.clear();
            //TempTarget.clear();
            return;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        GameVertex IEntity2d::OnGetOrigin(void) 
        {
            return m_Origin; 
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IEntity2d::SetRefOrigin(GameVertex org)
        {
            m_Origin=org; 
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        GameVertex IEntity2d::GetRefOrigin(void)
        {
            return m_Origin; 
        }




        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Text

        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void* IEntity2d::OnGetTextNode()
        {
            if(GetVisible())
            {
                cText_Node* ptextnode = (cText_Node*)this;
                GameVertex npos = GetPos();// + GameVertex(-20, 20);
                npos.y-=npos.z;
                ptextnode->m_TextMethod=TMShadow;
                ptextnode->m_TextPosition=npos;
                return ptextnode;
            }
            return NULL;
        }

        /*
        void IEntity2d::OnBlitText(LGB::VMCText* font)
        {
            if (m_TextNode.empty())
                return;

            cText_Node::tNode* n = GetTextNode(0);

            int fcolor = n->m_ForeColor;
            n->m_ForeColor=0x400808;
            GameVertex npos = GetPos();// + GameVertex(-20, 20);
            npos.y-=npos.z;
            //DrawText(font, &GameVertex(npos + GameVertex(1, 0)));
            DrawText(font, &GameVertex(npos + GameVertex(0, 1, 0)));
            DrawText(font, &GameVertex(npos + GameVertex(1, 1, 0)));

            n->m_ForeColor=fcolor;
            DrawText(font, &npos);

            //DrawText(font, &GetPos());
        }
        */


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        const tchar_t* IEntity2d::GetText(int index)
        {
            tNode *node = GetTextNode(index);
            if (node != NULL)
                return node->m_Text.c_str();
            return NULL;
        }


        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IEntity2d::SetText(const tchar_t* text, int index)
        {
            if(text == NULL)
                return;

            cText_Node::tNode* n=NULL;
            while((n=GetTextNode(index)) == NULL)
            {
                AddTextNode("", 0, GetClipImageHeight(), 0, GetClipImageWidth());
            }
            cText_Node::SetText(text, index);
        }
    };
};
