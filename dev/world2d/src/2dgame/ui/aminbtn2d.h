
#ifndef SHOWROLE2D_H
#define SHOWROLE2D_H

#include "Button2d.h"
//#include "role2d.h"

namespace GSO
{
    namespace UI
    {
        // 在ui界面上畫角色的元件，無底圖，單純將人物角色結合至ui的元件
        class AminBtn2d : public Static2d
        {
        protected:
            //GSO::World::AvatarRole* m_pRole;
            int m_ActImage;

        public:
            AminBtn2d()
            {
                m_ActImage = 180;//-1;//
            }

            void SetActImage(int count)
            {
                m_ActImage = count;
            }

            const tchar_t* ObjectName() const { return _T("Anim2d"); }

            //const tchar_t* GSPName() const {    return "2DANIM";    }
            // 把角色設定至元件
            /*
            void SetRole(GSO::World::AvatarRole* role)
            {
                //m_pRole = role;
            }
            */

            /*
            GSO::World::AvatarRole* GetRole()
            {
                //return m_pRole;
            }
            */

            /*
            // 繪製元件
            virtual void BlitImage(LGB::VMC* render, CPIcon* canvas)
            {
                CPIcon* icon = 0;

                if (m_pRole != 0 && GetVisible())
                {
                    short angle = m_pRole->m_Anim2d.GetNowAngle();
                    int frame = m_pRole->m_Anim2d.GetNowFrameStep();

                    for (int i = 0; i < m_pRole->GetLayers(); ++i)
                    {
                        if (m_ActImage >= 0)
                            icon = m_pRole->GetSortedImage(i, m_ActImage);
                        else
                            icon = m_pRole->Animation();//GetImage(m_pRole->GetNowFrame());

                        if (icon == 0)
                            continue;
                        render->AlphaBlit(icon, GetPos(), canvas);
                    }

                    m_pRole->m_Anim2d.ChangeDirection(angle);
                    m_pRole->m_Anim2d.SetNowFrameStep(frame);
                }
            }
            */
        };
    };
};

#endif