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
#ifndef BUILDING2D_H
#define BUILDING2D_H

#include "ientity2d.h"
#include "role2d.h"

namespace GSO
{
    namespace World
    {
        class Building2d : public IEntity2d
        {
        public:
            Building2d();
            ~Building2d();

            const tchar_t*     ObjectName() const { return _T("Building2d"); }
            void               Do(const tchar_t* anmname);  //set act
        };

    };//World
};//GSO


#endif