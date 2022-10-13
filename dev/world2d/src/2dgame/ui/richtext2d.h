#ifndef RICHTEXT2D_H
#define RICHTEXT2D_H

#include "Scroll2d.h"
#include <string>

namespace GSO
{
    namespace UI
    {
        //-----------------------------------------------------------------------------
        class RichText2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;

        public:
            enum RichTextCom
            { 
                RiScNon     = Scroll2d::ScCNon,             // 沒有
                RiScTbUp    = Scroll2d::ScCTbUp,            // 
                RiScUp      = Scroll2d::ScCUp,              //  
                RiScDivider = Scroll2d::ScCDivider,         // 游標
                RiScDown    = Scroll2d::ScCDown,            // 往下捲
                RiScTbDown  = Scroll2d::ScCTbDown,          // 往下一頁
                RiScMaxCom  = Scroll2d::ScCMAXCom,          // 結束
                RiScroll    = Scroll2d::ScCBar,             // 底部
                RiScText    = Scroll2d::ScCMAX,
                RiCText     = Scroll2d::ScCMAX + 1,
                RiCMax,
//                               RiCShadow,
            };
            enum StateValue 
            {
                SV_NoFocusLine = -1,
                SV_NotAssignLine = -1 
            };

        public:
            RichText2d();
            ~RichText2d();

            const tchar_t*  ObjectName() const { return _T("RichText2d"); }

            virtual error_t    LoadRes(RichTextCom btn, const tchar_t* filename);
            virtual error_t    LoadAnm(RichTextCom btn, const tchar_t* filename);
                                   
            IWidget2d*         GetComponentButton(RichTextCom id);

            void               DestroyAllItem(void);

            // 設定字元寬高，in pixel
            void               SetFontSize(int w, int h);
                               
            // 取得有幾行       
            uint_t             GetLineSize();
                               
            // 取得目前行       
            uint_t             GetFocusLine();
                               
            void               SetStartLine(uint_t pos);
                               
            void               SetMaxStep(uint_t value);
                               
            const char*        GetLineText(uint_t line);

            // Sean:
            // 可以考慮放到private，除非一次可以highlight好幾行，作其它的用途
            // 
            void               HighLight(uint_t line, bool hl);

            //對字串列表進行操作
            // 若無指定列，則刪除目前選中的那一行
            void               DeleteString(int index = SV_NotAssignLine);
                                   
            void               DeleteAllString();

            // 若無指定列，則以目前選中的那一行進行排序
            void               SortString(int index = SV_NotAssignLine);
                               
                               
                               
#ifndef UNICODE                
            void               AddString(tchar_t* str, int fg, int bg);
            void               SetText(tchar_t* data, int index = 0);
#else // UNICODE               
            void               AddString(wchar_t* str, int fg, int bg);
            void               SetText(wchar_t* data, int index = 0);
#endif                     

            //-----------------------------------------------------------------
            // 委托函式
            // 設定游標可移動的範圍
            // 如0, 100, 0, 0表示垂直的捲軸(格式為:上、下、左、右)
            void               SetScrollRange(int top, int bottom, int left, int right);
            LG::Rect           GetScrollRange();
            
            
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

            void               OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point point);
            void               OnLMouseDown(int key, LG::Point point);
            void               OnMouseWheel(int key, LG::Point point);
            virtual void*      OnGetTextNode();
            //virtual void       OnBlitText(LGB::VMCText* font);
            virtual bool       IsCollisionByImage(GameVertex mpos);
            virtual void       OnUpdate    (void);


        private:
            Scroll2d*          m_Scroll;
//            Button2d*          m_Shadow;//===>被美術凹的結果 >_<凡見此段註解之程式 切計切計 企劃雖是程式最大的敵人，但也不可以對美術輕敵
            uint_t             m_FocusLine;
            int                m_StartPos;
            int                m_StopPos;
            int                m_InsterPos;
            int                m_StartLine;
            LG::Size           m_FontSize;
            LG::Rect           m_TextRectD;
            int                m_FronColor;
            int                m_BackColor;
            int                m_InverseFrontColor;    // Add by Wood, 20050112
            int                m_InverseBackColor;            
        };
    };
};
#endif
