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
#ifndef LIGHTING_H
#define LIGHTING_H

//#include "V3DBase.h"
//#include <Kernel3D.h>
#include <VfwGraphic.h>
////#include "Axis.h"
#include <Model3d.h>

#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
//light
#define Vf_NALIGHT                        0X0000
#define Vf_AMBIENT                        GL_AMBIENT
#define Vf_DIRECTION
#define Vf_SPOT                           GL_SPOT_DIRECTION
#define Vf_SPOT_SOFT                      GL_SPOT_EXPONENT


#define Vf_DIFFUSE                        GL_DIFFUSE
#define Vf_SPECULAR                       GL_SPECULAR
#define Vf_POSITION                       GL_POSITION
#define Vf_SPOT_DIRECTION                 GL_SPOT_DIRECTION
#define Vf_SPOT_CUTOFF                    GL_SPOT_CUTOFF
#define Vf_CONSTANT_ATTENUATION           GL_CONSTANT_ATTENUATION
#define Vf_LINEAR_ATTENUATION             GL_LINEAR_ATTENUATION
#define Vf_QUADRATIC_ATTENUATION          GL_QUADRATIC_ATTENUATION

//clear
#define Vf_CLEAR_IMAGEBUFFER              GL_COLOR_BUFFER_BIT
#define Vf_CLEAR_ZBUFFER                  GL_DEPTH_BUFFER_BIT

#endif

#ifdef _RWKERNEL_
//light
#define Vf_NALIGHT                        rpNALIGHTTYPE
#define Vf_AMBIENT                        rpLIGHTAMBIENT
#define Vf_LIGHTPOINT                     rpLIGHTPOINT
#define Vf_SPOT                           rpLIGHTSPOT
#define Vf_SPOT_SOFT                      rpLIGHTSPOTSOFT
#define Vf_DIRECTION                      rpLIGHTDIRECTIONAL
#endif



///////////////////////////////////////////////////////////////////////////////
//  light
namespace LGR
{
	class Light : public CPAxis
	{
		bool      m_isTurnOn;                                 // Lighting ON / OFF
		int       m_type;
		DWORD     m_Flags;
		LC::RGBAf m_color;
		float     m_radius;
		float     m_coneangle;

	#ifdef _RWKERNEL_
	public:
		RpLight*m_pLight;   
		operator  RpLight*() { return m_pLight; }
	#endif

	public:

		Light();
		~Light();

		Pvoid     CreateMatrix( Pvoid );

		Pvoid     Create( Pint type = Vf_AMBIENT );
		int       GetType( void );

		LC::RGBAf GetColor( void );
		void      SetColor( LC::RGBAf color );

		float     GetRadius( void );
		void      SetRadius( float radius );

		float     GetConeAngle( void );
		void      SetConeAngle( float angle );

		DWORD     GetFlags( void );
		void      SetFlags( DWORD flags );
				
		void      SetPos( const LG::Vertex3f* p );
		
		
		void      Enable( Pvoid );
		void      Disable( Pvoid );

		//Pvoid      Assign      ( Pword num );
		void     Turn( Pvoid );
		void     Turn_on( Pvoid );
		void     Turn_off( Pvoid );
		bool     IsTrueon( Pvoid ) { return m_isTurnOn; }

    
		void     Irradiate( CPModel* model );

	};
};

#endif