/*
 *    File:        render2dex.h
 *    Desc:        在HKO專案裡最底層的繪圖機
 *    Creater:    Sean
 *    Create:        04/07/09
 *    Updater:    
 *    Update:        
 */
#ifndef RENDER2DEX_H
#define RENDER2DEX_H

#include <vmc-render2d.h>
#include "terrain2d.h"


class IParticalEmitter;

namespace LGB
{
    class Render2dEX : public LGB::VMCRender2d
    {
        typedef LGB::VMCRender2d BaseClass;
    public:
        enum RenderFlow  { RFNormal,    RFPerspective };
        enum DrawStateEx { RSExClip = LGB::DrawNode::RSUser + 1 };

        Render2dEX();
        ~Render2dEX();


        void        SetTerrainSystem    (GSO::World::Terrain2d* terrain_system)       { m_pTerrainSystem = terrain_system;    }
        void        SetParticalSystem    (IParticalEmitter*      partical_system)    { m_ParticalSystem = partical_system;    }


        void        SetRenderFlow(RenderFlow rf);

        void        UpdateBackground();

    public:

        GSO::Element2d m_BlitSky;
        GSO::Element2d m_BlitMountain;
        GSO::Element2d m_BlitCloud;
        GSO::Element2d m_BlitMountain2;
        GSO::Element2d m_BlitCloud2;
        GSO::Element2d m_BlitFarCloud;
        GSO::Element2d m_BlitFarCloud2;
        GSO::Element2d m_BlitMask;

    public:
        virtual void BlitAll();

    protected:
        void        BlitEntityBack();
        void        BlitEntityFront();

    private:    // 會用private是因為，我就不相信還會有人要繼承它
        void        BlitPartical();

        RenderFlow               m_RenderFlow;
        IParticalEmitter*      m_ParticalSystem;
        GSO::World::Terrain2d* m_pTerrainSystem;

    };
}



#endif