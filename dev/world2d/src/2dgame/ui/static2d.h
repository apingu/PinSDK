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

#ifndef STATIC2D_H
#define STATIC2D_H


#include "IWidget2d.h"

namespace GSO
{
    namespace UI
    {
        class Static2d : public IWidget2d
        {
            typedef IWidget2d BaseClass;

        public:
            Static2d();
            ~Static2d();

            const tchar_t* ObjectName() const { return _T("Static2d"); }
            
            void SetState(int state);
            int  GetState(void);
        };
    };
};

#endif