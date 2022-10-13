/*
 *  File:       Edit2d.h
 *  Desc:       可編輯文字的元件
 *  Creater:    Sean
 *  Create:     04/08/14
 *  Updater:    Sean
 *  Update:     04/08/18
 *  Desc:       目前大致完成更新，但再考慮要不要把那些開放的變數改起來
 */

#ifndef EDIT2D_H
#define EDIT2D_H

#include "Button2d.h"
#include <string>

namespace GSO
{
    namespace UI
    {
//#define CURSOR_TIME 500

        class Edit2d : public IWidget2d
        {
            typedef Element2d BaseClass;

        protected:

            int  m_CaretTime;
            int  m_LastTime;
            int  m_InsertPos;
            bool m_Cursor;
            BYTE m_CursorSign;
            bool m_AcceptDBCS;    // 允許非ASCII的寬字元. False 適用於帳號密碼輸入
            // 用來計算字元游標(caret)的閃爍時間
            //virtual void JudgeCaretTime(int time);

        public:

            Edit2d();
            ~Edit2d();

            const tchar_t*  ObjectName() const { return _T("Edit2d"); }

            virtual error_t LoadRes     (const tchar_t* filename);
            virtual int     Update        (int count = -1);//effect children

            virtual void    OnKeyDown   (IWidget2d* pWidget, int KeyCode);
            virtual void    OnChar        (IWidget2d* pWidget, unsigned nChar);
            void            SetCaretTime(mstimen_t time);

            static void     SetCursorSign(tchar_t c);

            void            SetMask     (bool b);

        private:

            virtual void    OnLMouseUp  (int key, LG::Point point);

            static tchar_t  s_CursorSign;

        };
    };
};

#endif