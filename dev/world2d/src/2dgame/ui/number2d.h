//
// 圖形數字物件
//
#ifndef NUMBER2D_H
#define NUMBER2D_H

#include "IWidget2d.h"
#include <vector>
#include <string>

namespace GSO
{
    namespace UI
    {
        class Number2d : public IWidget2d
        {
            longlong_t               m_Number;
            std::vector<IWidget2d*>  m_NumberVec;
            int                      m_Space;
            int                      m_Align;
            int                      m_Figure;  // 固定幾位數
            typedef IWidget2d BaseClass;


        public:
            Number2d();
            ~Number2d();

            const tchar_t*  ObjectName() const {return _T("Number2d");}

            longlong_t &         operator=(longlong_t number);

            void               SetAlign(int t);
            void               SetXSpace(int t);
            void               SetFigure(int t);
                               
                               
            virtual void       SetPos(GameVertex pos);

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
            virtual void       DestroyAllItem(void);
        };
    };
};



#endif //NUMBER2D_H