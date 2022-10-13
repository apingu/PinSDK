/*
 *  File:       Progress2d.h
 *  Desc:       可顯示進度狀態，或做為血量表的元件
 *  Creater:    Sean
 *  Create:     04/08/14
 *  Updater:    Sean
 *  Update:     04/08/18
 *  Desc:       目前改這樣應該沒問題了
 */

#ifndef PROGRESS2D_H
#define PROGRESS2D_H

#include "Button2d.h"

namespace GSO
{
    namespace UI
    {
        // 血條
        class Progress2d : public GSO::UI::IWidget2d
        {
            typedef IWidget2d BaseClass;

            class Bar2d : public GSO::UI::IWidget2d
            {
            friend Progress2d;

                LG::Size m_ImageSize;    // 圖形尺寸
                int      m_MaxValue;     // 最大值
                int      m_CurrValue;    // 實際值

            public:
                Bar2d();
                ~Bar2d();

            private:
                LG::Rect        OnGetRenderRect(void);

            };

            GSO::UI::Progress2d::Bar2d* m_ValueBar;

        public:

            enum ProgressCom
            {
                PgCBar   =  1,        // 血條
                PgCPanel =  2,        // 底板
                PgMax,
            };


            Progress2d();
            ~Progress2d();

            const tchar_t*  ObjectName() const { return _T("Progress2d"); }


            virtual error_t    LoadRes(ProgressCom com, const tchar_t* filename);
            virtual error_t    LoadAnm(ProgressCom com, const tchar_t* filename);
                               
            IWidget2d*         GetComponentButton(ProgressCom id);
                               
            // 改最大值         
            void               SetMaxStep(int value);
            int                GetMaxStep(void);
            // 設定目前值       
            void               SetCurrStep(int value);
            int                GetCurrStep(void);
            //void             SetCurrValueText(void);

            //! 編輯器存檔用, 要能將本類的屬性加入 node 中
            //! @return: true 表示底下的 node 需要繼續列舉, false 代表不要列舉
            virtual bool       SaveCfg(GspNode *toNode);

            // 限制使用
            //IWidget2d*         operator[]    (uint_t i)          {return NULL;}
            //uint_t             GetItemCount  (void) const        {return 0;}
            IWidget2d*         GetItem       (entityid_t index)  {return NULL;}
            bool               AttachItem    (IWidget2d* item){return false;}
            bool               DetachItem    (IWidget2d* item){return false;}
            bool               ReplaceItem   (const IWidget2d* oldc, IWidget2d* newc){return false;}

        private:
            virtual error_t    LoadRes(const tchar_t* resname){return BaseClass::LoadRes(resname);}
            virtual error_t    LoadAnm(const tchar_t* anmname){return BaseClass::LoadAnm(anmname);}

        };
    };
};



#endif