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


//#include "RwEngine.h"
//#include "RwObject.h"
#include "Model.h"
#include "vector"


//namespace RW{

//class CWorld;
#define MAX_HANIM_SEQS  100

///////////////////////////////////////////////////////////////////////////////
//  PCRWModle

class PCAnim// : public CObject
{
public:

                        PCAnim();
                        ~PCAnim();

    // Construction
    bool                Create( Pcstr szFilename, Pcstr szImagePath = NULL );

    //Destruction
    void                Destroy( void );

public:

    //HAnim

    RpHAnimAnimation*   m_pHAnimSeqs[ MAX_HANIM_SEQS ];

    RwInt32             m_nHAnimNumSeqs;
    RwInt32             m_nHAnimCurSeqs;
    RwReal              m_fHAnimPerSec;

    RwBool              m_bHAnimOn;
    RwBool              m_bHasHAnim;

    RwBool              HAnimLoadANM( Pcstr szFilename );
    void                HAnimUpdate( Pfloat delta );
    void                HAnimDestroy( void );

    //for RenderWare Compatible

private:

    RpClump*            m_pClump;
    RwFrame*            m_pFrame;

public:

    PCAnim( RpClump* clump )
    {
        m_pClump = clump;
    }
    const PCAnim& operator=( RpClump* clump )
    {
        m_pClump = clump; return *this;
    }

    operator RpClump*()
    {
        return m_pClump;
    }
};


//}// namespace RW



//#endif