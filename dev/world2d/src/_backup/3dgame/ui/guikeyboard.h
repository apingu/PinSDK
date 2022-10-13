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

#ifndef SCREENKEYBOARD_H
#define SCREENKEYBOARD_H

#include <pdef.h>
#include <timage.h>
#include <Graphic3D.h>
#include <keyboard.h>
#include <gamepad.h>
#include <Array2d.tl>
#include <Device.h>


namespace GUI
{
    class KeyBoard
    {
        PT::Array2d<char> m_BtMap;

        DWORD       m_CurColor;
        LG::Point   m_CurSel; 
        LG::Rect    m_BtRect[KEY_MAXBUTTONS];

        tImage      m_Image;

    protected:

        virtual void SetMapping( PT::Array2d<char>* BtMap, LG::Rect* BtRect );

    public:

        KeyBoard();
        ~KeyBoard();
        
        void LoadImage( const char* filename );
        void MoveUp( void );
        void MoveDown( void );
        void MoveLeft( void );
        void MoveRight( void );    

        void SetCurColor( BYTE R, BYTE G, BYTE B );
        char GetCurKey( void );

        void Render( const LG::Rect& rect );
        void Update( void ); 


	protected:

		virtual void OnLoadImage( const char* filename, tImage* image ){};
    	virtual void OnRenderSel( const LG::Rect* rect ){};
		virtual void OnRenderImage( const tImage* image, const LG::Rect& rect ){};
    };


	class RwKeyBoard : public KeyBoard
	{
		CPTexture m_Texture;
		LGR::Camera  m_Camera;

		void OnLoadImage( const char* filename, tImage* image );
    	void OnRenderSel( const LG::Rect* rect );
		void OnRenderImage( const tImage* image, const LG::Rect& rect );
	};

};//namespace GUI

#endif//SCREENKEYBOARD_H


