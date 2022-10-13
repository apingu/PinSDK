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
#ifndef V3DCAMERA_H
#define V3DCAMERA_H

#include "V3DBase.h"


///////////////////////////////////////////////////////////////////////////////
//  Camera

class V3DCamera
{
    float       m_scale;
    float       m_yrot;             // Y Rotation
    float       m_walkbias;
    float       m_walkbiasangle;
    float       m_lookupdown;
    float       m_heading;
    CPVertex3f  m_vPosition;                   // The camera's position

public:

                V3DCamera();
                ~V3DCamera();

    CPVertex3f Position( Pvoid )
    {
        return m_vPosition;
    }
    Pfloat Angle( Pvoid )
    {
        return m_heading;
    }
    Pvoid   Look( Pvoid );

    Pvoid   PositionCamera( CPVertex3f position,
                            CPVertex3f view,
                            CPVertex3f upVector );

    Pvoid   Look_Up( float angle = 2.0 );
    Pvoid   Look_Down( float angle = 2.0 );
    Pvoid   Look_Left( float angle = 2.0 );
    Pvoid   Look_Right( float angle = 2.0 );

    Pvoid   Zoom_In( float distance = 0.25 );
    Pvoid   Zoom_Out( float distance = 0.25 );


    Pvoid   Rise( float distance = 0.05 );
    Pvoid   Fall( float distance = 0.05 );
    Pvoid   Advance( float distance = 0.05 ); //move advance
    Pvoid   Retreat( float distance = 0.05 ); //move back

    Pvoid   Move( float speed = 0.05 );    // This will move the camera forward or backward depending on the speed

    Pvoid   Move_Left( float distance = 0.05 ); //move left
    Pvoid   Move_Right( float distance = 0.05 ); //move right
};


#endif
