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
//                                               Copyright    (C)    2002    Pin
//
#ifndef DRAWGEOMERTY_H
#define DRAWGEOMERTY_H

#include <Color.h>
#include <aMath.h>
#include <Graphic3D.h>


void RenderVertex     ( LG::Vertex3f vertex, LC::RGBAb color, float size = 1, PA::Matrixf* mat = NULL );
void RenderVertices   ( LG::Vertex3f* vertex, int len, LC::RGBAb color, float size = 1, PA::Matrixf* mat = NULL );
void RenderLine       ( LG::Vertex3f vet1, LG::Vertex3f vet2, LC::RGBAb color, float width = 1, PA::Matrixf* mat = NULL );
void RenderCube       ( LG::BBoxf cube, LC::RGBAb color, float width = 1, PA::Matrixf* mat = NULL );
void RenderSphere     ( LG::Spheref sphere, LC::RGBAb color, float width = 1, PA::Matrixf* mat = NULL );
void RenderCircle     ( LG::Spheref sphere, LC::RGBAb color, float width = 1, PA::Matrixf* mat = NULL );
void RenderPyramid    ( LG::Spheref sphere, float height, LC::RGBAb color, float width = 1, PA::Matrixf* mat = NULL );
void RenderHexahedron ( LG::Vertex3f vertex[8], LC::RGBAb color, PA::Matrixf* mat = NULL );
void RenderCone       ( const PA::Matrixf* mat, float coneAngle, float coneSize, float coneRatio );

void RenderAxis       ( PA::Matrixf axis, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float width = 1, PA::Matrixf* mat = NULL );
void RenderUnitAxis   ( PA::Matrixf axis, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float mul = 1, float width = 1, PA::Matrixf* mat = NULL );

void RenderNet        ( LG::Rect rect, Puint Width, Puint Height, LC::RGBAb color, float linewidth = 1, PA::Matrixf* mat = NULL );


#endif
