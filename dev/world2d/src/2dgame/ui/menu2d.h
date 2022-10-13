/*
 *  File:       Menu2d.h
 *  Desc:       可變數量的一維button，類麼btnarray2d
 *  Creater:    Kent
 *  Create:     ?
 *  Updater:    Sean
 *  Update:     04/08/30
 *  Desc:       目前大致完成更新
 */



#ifndef MENU2D_H
#define MENU2D_H

#include "Button2d.h"
#include <vector>

namespace GSO
{
    namespace UI
    {
        class Menu2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;
        public:
            Menu2d();
            ~Menu2d();

            const tchar_t* ObjectName() const { return _T("Menu2d"); }

            // 讀個別btn的資料
            void LoadSubRes(int btn_id, const tchar_t* filename);


            void SetSubText(int btn_id,
                             tchar_t* text,
                             int top,
                             int bottom,
                             int left,
                             int right,
                             int fg,
                             int bg);


            bool AttachItem(IWidget2d* item);


            bool DetachItem(IWidget2d* item);


            // 切換某個子元件的顯示
            void ToggleItemVisible(int wig_id, bool flag);


        protected:
            virtual void UpdateCellPos(void);

        };
    };
};

#endif