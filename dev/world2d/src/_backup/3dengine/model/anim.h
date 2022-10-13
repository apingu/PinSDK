//Anim.h
//
//    3D Role animation 
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
//                                               Copyright    (C)    2002    Pin
//
#ifndef ANIM_H
#define ANIM_H
//#include "RwEngine.h"
//#include "RwObject.h"
//#include "Model.h"
#include "Model.h"
#include <vector>


//namespace RW{

//class CWorld;
//#define MAX_HANIM_SEQS  100


///////////////////////////////////////////////////////////////////////////////
//  PCRWModle

class CPAnim// : public CPModel
{
private:

    //Destruction


    //std::vector<VfAnimSequence*>    m_pHAnimSeqs;
    //Puint                           m_nHAnimCurSeqs;
#ifdef _RWKERNEL_
    RpHAnimHierarchy*   m_blend[ 2 ];
#endif
    VfAnimSequence*     m_pHAnim;

    Pfloat              m_persec;
    Pfloat              m_duration;
    Pfloat              m_currenttime;


    void                Destroy( void );

public:

    CPAnim();
    ~CPAnim();

    operator   VfAnimSequence*() { return m_pHAnim; }


    //HAnim

    //RpHAnimAnimation *m_pHAnimSeqs[ MAX_HANIM_SEQS];

    //RwInt32           m_nHAnimNumSeqs;


    //RwBool            m_bHAnimOn;
    //RwBool            m_bHasHAnim;

    //RwBool            HAnimLoadANM  ( Pcstr szFilename );

    //void              UpdateAnimation( RwReal deltaTime );

    //Pbool             Do            ( Puint curAnim );

    VfError Load( Pcstr szFilename );

    VfAnimSequence* getAnim( Pvoid )
    {
        return m_pHAnim;
    }
    Pfloat getDuration( Pvoid )
    {
        return m_duration;
    }
    Pfloat getCurrenttime( Pvoid )
    {
        return m_currenttime;
    }

    Pbool   Transform( CPModel* model );
    Pfloat  Update( CPModel* model, Pfloat delta );

    Pvoid   BeginBlend( CPModel* model );
    Pfloat  Blend( CPModel* model, Pfloat delta, CPModel* pre, CPModel* next );


    //for RenderWare Compatible

public:

    //CPAnim( RpClump* clump )                   { m_pClump = clump; }


    //operator RpClump* ()                        { return m_pClump; }
};


//}// namespace RW



#endif
