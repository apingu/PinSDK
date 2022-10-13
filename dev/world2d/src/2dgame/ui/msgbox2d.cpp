#include "MsgBox2d.h"


namespace GSO
{
    namespace UI
    {
        GSO::UI::Dialog2d                    MsgBox2d::s_Dialog;
        GSO::UI::Static2d*                   MsgBox2d::s_pContent=NULL;
        const MsgBoxReply*                   MsgBox2d::s_pRelpy=NULL;
        std::map<UIMSG_TYPEID, std::string>  MsgBox2d::s_typelist;
        longlong_t                             MsgBox2d::s_ContentId=-1;


        MsgBox2d::MsgBox2d()
        {
        }


        MsgBox2d::~MsgBox2d()
        {
            GSO::UI::IWidget2d* parent = (GSO::UI::IWidget2d*)s_Dialog.GetParent();
            if(parent)
                parent->DetachItem(&s_Dialog);
        }

        const GSO::UI::Dialog2d* MsgBox2d::Create(GSO::UI::IWidget2d* pParent)
        {
            if(pParent)
            {
                MsgBox2d::s_Dialog.SetId(GSO_STATIC_ID);
                pParent->AttachItem(&s_Dialog);
            }
            s_Dialog.SetVisible(false);
            return &s_Dialog;
        }

        void MsgBox2d::Destroy(void)
        {
            if(s_Dialog.GetParent())
                ((GSO::UI::IWidget2d*)s_Dialog.GetParent())->DetachItem(&s_Dialog);
            s_Dialog.SetVisible(false);
        }

        bool MsgBox2d::AddType(UIMSG_TYPEID type, const tchar_t* filename)
        {
            std::map<UIMSG_TYPEID, std::string>::iterator iter = s_typelist.find(type);
            if(iter != s_typelist.end())
            {
                s_typelist.erase(iter);
            }
            s_typelist.insert(std::map<UIMSG_TYPEID, std::string>::value_type(type, filename));
            return true;

        }

        void MsgBox2d::DelType(UIMSG_TYPEID type)
        {
            s_typelist.erase(type);
        }

        void MsgBox2d::SetTextContentId(longlong_t id)
        {
            s_ContentId= id;
        }

        longlong_t MsgBox2d::GetTextContentId(void)
        {
            return s_ContentId;
        }

        bool MsgBox2d::Show(const tchar_t* text, UIMSG_TYPEID type, const MsgBoxReply* func)
        {
            std::map<UIMSG_TYPEID, std::string>::iterator iter = s_typelist.find(type);
            if(iter==s_typelist.end())
                return false;

            if(s_Dialog.GetVisible())
            {
                Close();
            }

            s_Dialog.LoadCfg(iter->second.c_str());

            // 要先關一次
            s_Dialog.SetVisible(false);
            s_Dialog.SetVisible(true);
            s_pContent = (GSO::UI::Static2d*)s_Dialog.GetItem(s_ContentId);
            if(s_pContent)
                s_pContent->SetText(text);
            s_pRelpy = func;
            GSO::UI::IWidget2d::SetFocusWidget(&s_Dialog);

            return true;
        }

        void MsgBox2d::Close(void)
        {
            s_Dialog.SetVisible(false);
        }

    };
};
