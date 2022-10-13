#include "Menu2d.h"
#include <assert.h>

namespace GSO
{
    namespace UI
    {
        Menu2d::Menu2d() 
        {
        }

        Menu2d::~Menu2d()
        {
        }

        // 讀個別btn的資料
        void Menu2d::LoadSubRes(int btn_id, const tchar_t* filename)
        {
            Button2d* btn = new Button2d;
            btn->SetId(btn_id);
            btn->LoadRes(filename);
            AttachItem(btn);
        }

        void Menu2d::SetSubText(int btn_id,
                tchar_t* text,
                int top,
                int bottom,
                int left,
                int right,
                int fg,
                int bg)
        {
            Button2d* btn = (Button2d*) GetItem(btn_id);
            if (btn == NULL)
                return;
            btn->AddTextNode(text, top, bottom, left, right, fg, bg);
        }

        bool Menu2d::AttachItem(IWidget2d* item)
        {
            assert(item != NULL &&_T("Cant Attach NULL child!!!"));
            bool ret = IWidget2d::AttachItem(item);
            UpdateCellPos();
            return ret;
        }

        bool Menu2d::DetachItem(IWidget2d* item)
        {
            assert(item != NULL &&_T("Cant Detach NULL child!!!"));
            bool ret = IWidget2d::DetachItem(item);
            UpdateCellPos();
            return ret;
        }

        // 切換某個子元件的顯示
        void Menu2d::ToggleItemVisible(int wig_id, bool flag)
        {
            LG::Point pos = GetPos();
            IWidget2d* wig = GetItem(wig_id);
            assert(wig != NULL);
            wig->SetVisible(flag);
            UpdateCellPos();
            SetPos(pos);
        }

        void Menu2d::UpdateCellPos(void)
        {
            IWidget2d* pWidget = NULL;
            LG::Point pos(GetPos());
            for (unsigned int i = 0 ; i < GetItemCount() ; ++i)
            {
                pWidget = operator[](i);
                if (!pWidget->GetVisible()) continue;
                pWidget->SetPos(pos);
                pos.y += pWidget->GetHeight();
            }
        }
    };
};