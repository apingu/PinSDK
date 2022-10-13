#ifndef ROLEMANAGER_H
#define ROLEMANAGER_H

#include "Scene2d.h"
#include "VMC.h"

class CPIcon;
class Role2d;

namespace GSO
{
    namespace World
    {

        class RoleManager: public Scene2d
        {
		public:
            RoleManager();
            virtual ~RoleManager();

            Role2d* Role( int idx );
	        Role2d* RoleByID( int id );
            Role2d* GetFocusRole( void );
            void    SetFocusRole( Role2d* role );
            void    JoinRole( Role2d* role );
            void    DepartRole( Role2d* role );

            void    Update();

			void    RenderRole( CPIcon* canvas ); 
            void    RoleSort( void );
			void    RoleAlphaSort( void );

			void         SetRender( LGB::VMC* vmc ){ m_pRender = vmc; }
            LGB::VMC*    GetRender( void ){ return m_pRender; }

		private:
			LGB::VMC*              m_pRender;  
		};

	}; // namespace World
}; // namespace GSO



#endif