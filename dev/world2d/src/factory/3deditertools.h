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
#ifndef Editer3DTools_H
#define Editer3DTools_H

#include <RenderGraphic3D.h>
#include <Model3d.h>
#include <Light.h>


void RenderOriginAxis( LC::RGBAb u, LC::RGBAb r, LC::RGBAb a, float len );

void RenderModelUnitAxis( CPModel* model, LC::RGBAb u, LC::RGBAb r, LC::RGBAb a );
void RenderModelBoundingBox( CPModel* model, LC::RGBAb color );
void RenderModelBoundingSphere( CPModel* model, LC::RGBAb color, PA::Matrixf* mat );
void RenderModelSelectSphere( CPModel* model, LC::RGBAb color, PA::Matrixf* mat );

//void DrawCone( LGR::Light* light, RwReal coneAngle, RwReal coneSize, RwReal coneRatio);
void DrawDirectLight( LGR::Light* light );
void DrawPointLight( LGR::Light* light );
void DrawCurrentLight( LGR::Light* light );



//void RenderWorldLights( CP3DWorld* world, LC::RGBAb color );
//void RenderWorldCameras( CP3DWorld* world, LC::RGBAb color );



#endif
