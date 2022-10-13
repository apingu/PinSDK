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
#ifndef WZONE_H
#define WZONE_H

#include <vector>
#include "WEntity.h"


namespace WNE
{
    class Zone
    {
        std::vector<Entity*> m_Resident;
        std::vector<Zone*> m_Neighborhood;

    public:
        Zone();
        ~Zone();
    };
};



#endif//WZONE_H
