//Image_Editer.h header file of Image_Editer.cpp
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
//                                                         Pin
//

#ifndef IMAGEFACTORY_H
#define IMAGEFACTORY_H

#include "stdlib.h"
#include <Icon.h>
//#include <DeList.tl>

//#define CPE_Icon(p) ((CPEIcon*)p)

class CPIconFactory// : public CPIcon
{

public:

    CPIconFactory();
    ~CPIconFactory();
    
    
    //pin control
    //static int      Change_Offset(CPIcon* icon, short RX, short RY);              //pin offset control
    
    static int      SaveZLine    ( CPIcon* icon );
    static int      SaveOffset   ( CPIcon* icon );
    static int      SaveHitPoint ( CPIcon* icon );
    static int      SaveRect     ( CPIcon* icon );
    static int      SaveAll      ( CPIcon* icon );
    
    
    //draw contant
};


#endif
