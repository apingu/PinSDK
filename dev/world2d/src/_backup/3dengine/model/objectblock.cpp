// ObjectBlock.cpp
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
#include "ObjectBlock.h"

CPObjectBlock::CPObjectBlock()
{
}


CPObjectBlock::~CPObjectBlock()
{
    Puint i = 0;
    DELETE_PVECTOR( m_pSphereVec, i );
    DELETE_PVECTOR( m_pLineVec, i );
    DELETE_PVECTOR( m_pBoxVec, i );
}


void CPObjectBlock::InsertSphere( LG::Vertex3f center, float radius )
{
    LG::Spheref* sphere= new LG::Spheref( center, radius );
    m_pSphereVec.push_back( sphere );
}


void CPObjectBlock::RemoveSphere( void )
{
}


void CPObjectBlock::InsertBox( LG::Vertex3f v0, LG::Vertex3f v1 )
{
    LG::BBoxf* cube= new LG::BBoxf( v0, v1 );
    m_pBoxVec.push_back( cube );
}


void CPObjectBlock::RemoveBox( void )
{
}


void CPObjectBlock::InsertLine( LG::Vertex3f v0, LG::Vertex3f v1 )
{
    LG::Line3df* line  = new LG::Line3df( v0, v1 );
    m_pLineVec.push_back( line );
}


void CPObjectBlock::RemoveLine( void )
{

}


