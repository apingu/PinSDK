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

#include "V3DCamera.h"


const float piover180   = 0.0174532925f;

///////////////////////////////////////////////////////////////////////////////
//  Camera


//=============================================================================
//  Name : 
//  Desc : Constructor func
//=============================================================================
V3DCamera::V3DCamera()
{
    m_yrot = 0;
    m_scale = 1;
    m_walkbias = 0;
    m_walkbiasangle = 0;
    m_lookupdown = 0;
    m_vPosition.y = 0.5f;
    m_heading = 0;
}




//=============================================================================
//  Name : 
//  Desc : Disconstructor func
//=============================================================================
V3DCamera::~V3DCamera()
{
}

//=============================================================================
//  Name : Take()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Look( Pvoid )
{
    glLoadIdentity();                       // Reset The View
    glRotatef( m_lookupdown, 1.0f, 0, 0 );
    glRotatef( 360.0f - m_heading, 0, 1.0f, 0 );
    glTranslatef( -m_vPosition.x, -m_walkbias - m_vPosition.y, -m_vPosition.z );

    return;
}



//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Look_Up( float angle )
{
    m_lookupdown -= angle;
    return;
}


//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Look_Down( float angle )
{
    m_lookupdown += angle;
    return;
}

//=============================================================================
//  Name : 
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Look_Left( float angle )
{
    m_heading += angle;
    m_yrot = m_heading;
    return;
}


//=============================================================================
//  Name : Look_Right()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Look_Right( float angle )
{
    m_heading -= angle;
    m_yrot = m_heading;
    return;
}


//=============================================================================
//  Name : Zoom_In()
//  Desc : Set camera view zoom in
//=============================================================================
Pvoid V3DCamera::Zoom_In( float distance )
{
    m_scale += distance;
    //m_vPosition.z = m_vPosition.z + distance;
    return;
}



//=============================================================================
//  Name : Zoom_Out()
//  Desc : Set camera view zoom out
//=============================================================================
Pvoid V3DCamera::Zoom_Out( float distance )
{
    m_scale -= distance;
    return;
}


//=============================================================================
//  Name : Move
//  Desc : This will move the camera forward or backward depending on the speed
//=============================================================================
Pvoid V3DCamera::Move( float speed )
{
}

//=============================================================================
//  Name : Move_Advance()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Advance( float distance )
{
    m_vPosition.x -= ( float ) sin( m_heading * piover180 ) * distance;
    m_vPosition.z -= ( float ) cos( m_heading * piover180 ) * distance;
    if( m_walkbiasangle >= 359.0f )
    {
        m_walkbiasangle = 0.0f;
    }else
    {
        m_walkbiasangle += 10;
    }
    m_walkbias = ( float ) sin( m_walkbiasangle * piover180 ) / 20.0f;
    return;
}


//=============================================================================
//  Name : Move_Back()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Retreat( float distance )
{
    m_vPosition.x += ( float ) sin( m_heading * piover180 ) * distance;
    m_vPosition.z += ( float ) cos( m_heading * piover180 ) * distance;
    if( m_walkbiasangle <= 1.0f )
    {
        m_walkbiasangle = 359.0f;
    }else
    {
        m_walkbiasangle -= 10;
    }
    m_walkbias = ( float ) sin( m_walkbiasangle * piover180 ) / 20.0f;
    return;
}


//=============================================================================
//  Name : Move_Left()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Move_Left( float distance )
{
    m_vPosition.x = m_vPosition.x - distance;
    return;
}


//=============================================================================
//  Name : Move_Right()
//  Desc : 
//=============================================================================
Pvoid V3DCamera::Move_Right( float distance )
{
    m_vPosition.x = m_vPosition.x + distance;
    return;
}


//=============================================================================
//  Name : Move_Up()
//  Desc : Set camera view origin point
//=============================================================================
Pvoid V3DCamera::Rise( float distance )
{
    m_vPosition.y = m_vPosition.y + distance;
    return;
}


//=============================================================================
//  Name : Move_Down()
//  Desc : Set camera view origin point
//=============================================================================
Pvoid V3DCamera::Fall( float distance )
{
    m_vPosition.y = m_vPosition.y - distance;
    return;
}

