//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#include "building2d.h"

namespace GSO
{
    namespace World
    {
        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Building2d::Building2d()
        {
            RenderLayer = LGB::DrawNode::kLayerBld;
        }

        //=============================================================================
        // NAME: 
        // DESC:
        //=============================================================================
        Building2d::~Building2d()
        {

        }

//        void Building2d::RenderTo(LGB::VMCRender2d& renderer)
//        {
//            renderer.InsertNode(this, RenderLayer);    
//        }
        
        void Building2d::Do(const tchar_t* anmname)
        {
            IEntity2d::LoadAnm(anmname);
        }

    };//World
};//GSO
