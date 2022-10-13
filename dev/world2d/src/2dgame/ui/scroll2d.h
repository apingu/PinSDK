
#ifndef SCROLL2D_H
#define SCROLL2D_H

#include "Button2d.h"


#define  PE_MSGID_SCROLL    ((int)EventMessage::USERDEF) + 305    //當捲動發生時

namespace GSO
{
    namespace UI
    {
        class Scroll2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;
        public:
            enum ScrollCom  
            { 
                ScCNon = -1,        // 沒有
                ScCTbUp = 0,        // 
                ScCUp,              //  
                ScCDivider,         // 游標
                ScCDown,            // 往下捲
                ScCTbDown,          // 往下一頁
                ScCMAXCom,          // 結束
                ScCBar,             // 底部
                ScCMAX,
            };

            enum ScrollRollRange
            {
                SRR_Step = 1,
                SRR_TbStep = 5 
            };

        public:
            Scroll2d();
            virtual ~Scroll2d();

            const tchar_t*     ObjectName() const { return _T("Scroll2d"); }

            virtual int        Update(int count = -1);


            //virtual void    OnRenderTo(rendercontainer_t &container);
            // Sean:
            // 這算是重載...但是參數並不相同，是否會造成ambigious?
            virtual error_t       LoadRes(ScrollCom com, const tchar_t* filename);
            virtual error_t    LoadAnm(ScrollCom com, const tchar_t* filename);

            virtual void       DestroyAllItem(void);


        public:
            //-----------------------------------------------------------------
            // 設定元件的範圍
            // 如0, 100, 0, 0表示垂直的捲軸(格式為:上、下、左、右)
            void               SetScrollRange(int top, int bottom, int left, int right);
            LG::Rect           GetScrollRange() const;
            // 設定捲動能力
            void               SetScrollStep(uint_t TbStep = SRR_TbStep, uint_t Step = SRR_Step);
            // 設定最大的計數值
            void               SetMaxStep(uint_t value);
            // 取得最大的計數值
            uint_t               GetMaxStep() const { return m_MaxValue; }
            // 改變元件代表的數值
            void               SetCurrStep(uint_t value);
            // 取得目前游標所在的位置(值)
            uint_t               GetCurrStep() const { return m_CurrValue; }

            IWidget2d*         GetComponentButton(ScrollCom id);

            //! 編輯器存檔用, 要能將本類的屬性加入 node 中
            //! @return: true 表示底下的 node 需要繼續列舉, false 代表不要列舉
            virtual bool       SaveCfg(GspNode *toNode);



            // 限制使用
            //IWidget2d*         operator[]    (uint_t i)          {return NULL;}
            //uint_t             GetItemCount  (void) const        {return 0;}
            IWidget2d*         GetItem       (entityid_t index){return NULL;}
            bool               AttachItem    (IWidget2d* item){return false;}
            bool               DetachItem    (IWidget2d* item){return false;}
            bool               ReplaceItem   (const IWidget2d* oldc, IWidget2d* newc){return false;}


        private:

            virtual error_t    LoadRes(const tchar_t* resname){return BaseClass::LoadRes(resname);}
            virtual error_t    LoadAnm(const tchar_t* anmname){return BaseClass::LoadAnm(anmname);}

            virtual bool       OnIsCollision(GameVertex pos);
            virtual void       OnLMouseDown(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void       OnLMouseUp(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite);
            virtual void       OnDrag(IWidget2d* pWidget, LG::Point MouseSite);
                               
            virtual void       OnLMouseDown(int key, LG::Point point); 
            virtual void       OnLMouseUp(int key, LG::Point point); 
            virtual void       OnMouseWheel(int key, LG::Point point);

            virtual void       OnMoving    (IWidget2d* pWidget, const GameVertex pos);

        
            LG::Rect    m_Range;           // 元件的範圍
            LG::Rect    m_SubRange;        // 捲動範圍
            Button2d    m_pScC[ScCMAXCom]; // 放置所有的子元件
            uint_t      m_TbStep;          // tb(turbo)，表示大捲頁一次數量，ScC
            uint_t      m_Step;            // 小捲頁，ScC
            uint_t      m_MaxValue;
            uint_t      m_CurrValue;
            bool        m_SetRange;

        };

    }; // 
}; // namespace GSO

#endif