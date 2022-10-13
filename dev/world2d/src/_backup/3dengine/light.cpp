//Light.h
//
//
//    3D light
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    2000    Pin
//

#include "Light.h"

namespace LGR
{

///////////////////////////////////////////////////////////////////////////////
//  light


//=============================================================================
//  Name : 
//  Desc : Constructor func
//=============================================================================
Light::Light()
{
    //m_num = num;
    ID = -1; 
    m_type = Vf_NALIGHT;
    m_isTurnOn = FALSE;
	m_Flags = 3;
    m_color( 1.0, 1.0, 1.0, 1.0 );
}


//=============================================================================
//  Name : 
//  Desc : Disconstructor func
//=============================================================================
Light::~Light()
{
#ifdef _RWKERNEL_
    RpLightDestroy( m_pLight );
#endif
}

/*
//=============================================================================
//  Name : Enable()
//  Desc : assign a number to light
//=============================================================================
void Light::Assign( Pword num )
{
    if( num <= 7 )
        m_num = 0x4000 + num;

}
*/


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void Light::Create( Pint type )
{
    m_type = type;
#ifdef _RWKERNEL_
    m_pLight = RpLightCreate( type );
    SetColor( m_color );
    RpLightSetFlags( m_pLight, rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD );
	m_Flags = rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD;
    //if it is an ambient light, it doesn't need frame 
    if( type != Vf_AMBIENT )
	{
        RpLightSetFrame( m_pLight, ( RwFrame * ) CPAxis::Create() );
		LG::Vertex3f v;
		v.x = 0.00; v.y = 0.00; v.z = 1.00;
		SetUp( &v );
		v.x = 1.00; v.y = 0.00; v.z = 0.00;
		SetRight( &v );
		v.x = 0.00; v.y = -1.00; v.z = 0.00;
		SetAt( &v );
		v.x = 0.00; v.y = 0.00; v.z = 0.00;
		SetPos( &v );
		if( type = Vf_SPOT )
		{
			SetRadius( 10000 );
			SetConeAngle( 0.75 );

		}
	}
	
#endif
    // if it is not a ambient light, it will has situation, so it need a matrix
    //if( type != Vf_AMBIENT )
    //  CreateMatrix();
    return;
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
int Light::GetType( void )
{
    return m_type;
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
float Light::GetRadius( void )
{
	#ifdef _RWKERNEL_
	return RpLightGetRadius( m_pLight );
    #endif
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void Light::SetRadius( float radius )
{
	m_radius = radius;
	#ifdef _RWKERNEL_
	RpLightSetRadius( m_pLight, radius );
    #endif
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
float Light::GetConeAngle( void )
{
	#ifdef _RWKERNEL_
	return RpLightGetConeAngle( m_pLight );
    #endif
}


//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void Light::SetConeAngle( float angle )
{
	m_coneangle = angle;
	#ifdef _RWKERNEL_
	RpLightSetConeAngle( m_pLight, angle );
    #endif
}


//=============================================================================
//  Name : 
//  Desc :
//=============================================================================
void Light::SetColor( LC::RGBAf color )
{
#ifdef _RWKERNEL_
    m_color = color;
    if( m_pLight != 0 )
	{
		RwRGBAReal l;
		l.red = color.R;
		l.green = color.G;
		l.blue = color.B;
		l.alpha = color.A;
        RpLightSetColor( m_pLight, &l );
	}
#endif
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    float dl[]  ={color.R, color.G, color.B, color.A};
    glLightfv( m_num, m_type, dl );
#endif
}
//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
LC::RGBAf Light::GetColor( void )
{
    return m_color;
}

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
DWORD Light::GetFlags( void )
{
	return m_Flags;
}

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void Light::SetFlags( DWORD flags )
{ 
	m_Flags = flags;
#ifdef _RWKERNEL_
	RpLightSetFlags( m_pLight, flags );
#endif
}

//=============================================================================
//  Name :
//  Desc : 
//=============================================================================
void Light::CreateMatrix( void )
{
#ifdef _RWKERNEL_
    RpLightSetFrame( m_pLight, ( RwFrame * ) CPAxis::Create() );
#endif
}

//=============================================================================
//  Name : Enable()
//  Desc : 
//=============================================================================
void Light::Enable( void )
{
    m_isTurnOn = TRUE;
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    glEnable( m_num );
#endif
    return;
}


//=============================================================================
//  Name : Enable()
//  Desc : 
//=============================================================================
void Light::Disable( void )
{
    m_isTurnOn = FALSE;
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    glDisable( m_num );
#endif
    return;
}

//=============================================================================
//  Name : Turn()
//  Desc : if on it will be off if off it will be on
//=============================================================================
void Light::Turn( void )
{
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    if( m_isTurnOn )
        glDisable( GL_LIGHTING );
    else
        glEnable( GL_LIGHTING );
    m_isTurnOn = 1 - m_isTurnOn;
#endif
    return;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Light::Turn_on( void )
{
    m_isTurnOn = TRUE;  
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    glEnable( GL_LIGHTING );
#endif
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void Light::Turn_off( void )
{
    m_isTurnOn = FALSE;
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    glDisable( GL_LIGHTING );
#endif
}


//=============================================================================
//  Name : Set_LightPosition()
//  Desc :
//=============================================================================
void Light::SetPos( const LG::Vertex3f* p )
{
    //m_Position = pt;
    
#ifdef _RWKERNEL_
	CPAxis::SetPos( p );
#endif
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
	float lp[]  ={p->x, p->y, p->z, 1.0};
    glLightfv( m_num, GL_POSITION, lp );            // Position The Light
#endif
    return;
}




/*
//=============================================================================
//  Name : Set_AmbientLight()
//  Desc :
//=============================================================================
void Light::Set_Ambient( VfRGBAf light )
{ 
    m_AmbientColor = light;
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    float al[] = { light.R, light.G, light.B, light.A };
    glLightfv( m_num, GL_AMBIENT, al );
#endif
    return;
}
//=============================================================================
//  Name : Set_DiffuseLight()
//  Desc :
//=============================================================================
void Light::Set_Diffuse( VfRGBAf light )
{
    m_DiffuseColor = light;
#if defined (_XGLKERNEL_) || defined (_WGLKERNEL)
    float dl[] = { light.R, light.G, light.B, light.A };
    glLightfv( m_num, GL_DIFFUSE, dl );
#endif
    return;
}
*/


void Light::Irradiate( CPModel* model )
{
#ifdef _RWKERNEL_
    if( ( m_pLight != NULL ) && ( model->m_pClump != NULL ) )
        RpClumpAddLight( model->m_pClump, m_pLight );
#endif
}

};