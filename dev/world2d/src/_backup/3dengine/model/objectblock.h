//  ObjectBlock.h
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
//                                               Copyright    (C)    2003    Pin
//
#ifndef OBJECTBLOCK_H
#define OBJECTBLOCK_H

#include <Graphic3D.h>
#include <vector>

class CPObjectBlock
{
private:

    std::vector<LG::Spheref*>      m_pSphereVec;
    std::vector<LG::Line3df*>      m_pLineVec;
    std::vector<LG::BBoxf*>        m_pBoxVec;

public:

    CPObjectBlock();
    ~CPObjectBlock();

    error_t Load() {};
    error_t Save() {};


    void    InsertSphere( LG::Vertex3f center, float radius );
    void    RemoveSphere( void );

    void    InsertBox( LG::Vertex3f v0, LG::Vertex3f v1 );
    void    RemoveBox( void );

    void    InsertLine( LG::Vertex3f v0, LG::Vertex3f v1 );
    void    RemoveLine( void );

    //void Sift( 
};



#endif
