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
#ifndef IENTITY2D_H
#define IENTITY2D_H


#include <element2d.h>
//#include <dynarray.inl>
#include <text_node.h>
#include <set>

//Active status
namespace GSO
{
    namespace World
    {
        class Terrain2d;

        class IEntity2d : public Element2d, public cText_Node //, public OBPathSearch
        {

            static GameVertex       m_Origin;

        protected:
            std::set< IEntity2d* >  m_TargetSeq;
            /// 居住地
            Terrain2d*              m_Inhabit;

        public:

            IEntity2d();
            ~IEntity2d();

            const tchar_t*         ObjectName() const    { return _T("IEntity2d"); }

            // TODO: Alex, Let's reconsider these members
            std::string           m_CharacterName;
            
            /// 設定role2d的居住地
            void                   SetWorld(Terrain2d* world) { m_Inhabit = world; }
            /// 取得居住地
            Terrain2d*             GetWorld(void)             { return m_Inhabit; }

            void                   BuildInteraction(IEntity2d* target);
            void                   BreakInteraction(void);

            //origin point
            virtual GameVertex     OnGetOrigin();
            static  void           SetRefOrigin(GameVertex org);
            static  GameVertex     GetRefOrigin(void);

        protected:

            virtual void           OnResponse(IEntity2d* who){};


        public:
            // text
            virtual const tchar_t* GetText(int index = 0);
            virtual void           SetText(const tchar_t* data, int index = 0);

        protected:
            virtual void*          OnGetTextNode();
            //virtual void           OnBlitText(LGB::VMCText* font);

        };

    };//World

};//GE



#endif

