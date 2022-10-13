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
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "amath.inl"

namespace PA
{
    class CollData
    {
    public:

        CollData()
        {
            IsCollision = false;
            Dist = 0;
        }
        ~CollData()
        {
        };

        bool            IsCollision;
        float           Dist;
        LG::Polygonf    Poly; 
        LG::Vertex3f    Normal;
        LG::Vertex3f    IntersectVertex;
    };


    class Intersect
    {
    public:

        Intersect()
        {
        };
        ~Intersect()
        {
        };


        static bool Test( const LG::Quadr* q, const LG::Point* p )
        {
            if( ( _IS_UNDER( &q->vertex[ 0 ], &q->vertex[ 1 ], p ) <= 0 ) &&
                ( _IS_UNDER( &q->vertex[ 1 ], &q->vertex[ 2 ], p ) <= 0 ) &&
                ( _IS_UNDER( &q->vertex[ 3 ], &q->vertex[ 2 ], p ) >= 0 ) &&
                ( _IS_UNDER( &q->vertex[ 0 ], &q->vertex[ 3 ], p ) >= 0 ) )
                return true;
            return false;
        }

        static bool Test( const LG::Octagon* o, const LG::Point* p )
        {
            if( ( _IS_UNDER( &o->vertex[ 0 ], &o->vertex[ 1 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 1 ], &o->vertex[ 2 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 2 ], &o->vertex[ 3 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 3 ], &o->vertex[ 4 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 4 ], &o->vertex[ 5 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 5 ], &o->vertex[ 6 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 6 ], &o->vertex[ 7 ], p ) <= 0 ) &&
                ( _IS_UNDER( &o->vertex[ 7 ], &o->vertex[ 0 ], p ) <= 0 ) )
                return true;
            return false;
        }

        static bool Test( const LG::Rect* r, const LG::Point* p )
        {
            if( ( p->x <= r->right ) &&
                ( p->x >= r->left ) &&
                ( p->y <= r->bottom ) &&
                ( p->y >= r->top ) )
                return true;
            return false;
        }


        static bool Test( const LG::Rect* r0, const LG::Rect* r1 )
        {
            return  ( __min( ( r1->right ), ( r0->right ) ) >= __max( ( r1->left ), ( r0->left ) ) &&
                      __min( ( r1->bottom ), ( r0->bottom ) ) >= __max( ( r1->top ), ( r0->top ) ) );
        }

        static float Test( const LG::Spheref* s0, const LG::Spheref* s1 )
        {
            LG::Vertex3f    v                   = ( ( LG::Vertex3f )
                                                    s0->center );
            return ( s0->radius + s1->radius ) -
                   ( ( float ) VertexToVertexFunc( &s0->center, &s1->center ) );
        }

        static float Test( const LG::Spheref* s, const LG::Plane3df* p )
        {
            //float dis = (float)Distance( s->center, *p );
            return ( float )
                   s->radius -
                   ( float )
                   abs( ( int )
                   VertexToPlane( &s->center, p ) );
        }

        static LG::Vertex3f Test( const LG::Vertex3f& v,
                                  const LG::Vertex3f& LineStart,
                                  const LG::Vertex3f& LineEnd )
        {
            /*
                LG::Vertex3f intersect; 
                LG::Vertex3f v0;
                    LG::Vertex3f v1;
                    v0.x = v.x - LineStart.x; v0.y = v.y - LineStart.y; v0.z = v.z - LineStart.z;
                    v1.x = v.x - LineEnd.x;   v1.y = v.y - LineEnd.y;   v1.z = v.z - LineEnd.z;
                    float f1 = (float)VertexToVertex( v, LineStart );
                    float f2 = (float)VertexToVertex( v, LineEnd );
                float Dist = (f1/(f1+f2));
                    
                    float len = (float)VertexToVertex( LineEnd, LineStart ) * Dist;
                intersect.x = LineStart.x + ((LineEnd.x - LineStart.x) * Dist);
                intersect.y = LineStart.y + ((LineEnd.y - LineStart.y) * Dist);
                intersect.z = LineStart.z + ((LineEnd.z - LineStart.z) * Dist);
                  return intersect;
                */



            float           LineMag;
            float           U;
            LG::Vertex3f    Intersection;

            LineMag = ( float ) VertexToVertexFunc( &LineEnd, &LineStart );

            U = ( ( ( v.x - LineStart.x ) * ( LineEnd.x - LineStart.x ) ) +
                  ( ( v.y - LineStart.y ) * ( LineEnd.y - LineStart.y ) ) +
                  ( ( v.z - LineStart.z ) * ( LineEnd.z - LineStart.z ) ) ) /
                ( LineMag * LineMag );

            if( U <0.0f || U> 1.0f )
                return Intersection;   // closest point does not fall within the line segment

            Intersection.x = LineStart.x + U * ( LineEnd.x - LineStart.x );
            Intersection.y = LineStart.y + U * ( LineEnd.y - LineStart.y );
            Intersection.z = LineStart.z + U * ( LineEnd.z - LineStart.z );
            return Intersection;
        }


        static float Test( const LG::Spheref* s, const LG::Cubef* c )
        {
            return -1.0f;
        }

        static float Test( const LG::Cubef* s, const LG::Cubef* c )
        {
            return -1.0f;
        }
    };
};//namespace PA