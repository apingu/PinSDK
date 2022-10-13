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
//              Copyright (C) 2001 Pin
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件
#include <math.h>
#include <stdlib.h> //abs
//#if defined(_X86_)
#include <xmmintrin.h>
#include <emmintrin.h>
//#endif

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include <graphic3d.h>
#include <memio.inl>


#define _PI_                    3.14159f
#define _TWO_PI_                6.28318f //2.0*_PI_;
#define _HALF_PI_               1.570795 //0.5*_PI_;
#define _PI_DIV_180             0.01745f
#define _MIN_FLOAT_             100.0f   //取到小數點後面幾位  10->1位 100->2位 100->3位 以此類推


// 依照_MIN_FLOAT_的定義取到小數點下幾位數
#define _SET_PRECISION(f1)      (int(f1*_MIN_FLOAT_) / _MIN_FLOAT_)

#define _SLOPE(p1, p2)          ((float)((float)((p2)->y - (p1)->y) / (float)((p2)->x - (p1)->x)))
#define _IS_UNDER(p1,p2,p3)     (((p2)->y - (p1)->y)*((p3)->x - (p1)->x) - ((p3)->y - (p1)->y)*((p2)->x - (p1)->x))


typedef float degree_t;    // 角度(degree  0~360)
typedef float radius_t;    // 徑度(radius  0~2pi)

// 角度換算成徑度
#define _DEGREE_TO_RADIAN(d)    ((float)(d * (_PI_DIV_180)))
// 徑度換算成角度
#define _RADIAN_TO_DEGREE(r)    ((float)(r / (_PI_DIV_180)))



///////////////////////////////////////////////////////////////////////////////
//  result = (p) + lambda(pq)
#define Point_on_Line(resule, p, q, lambda)\
    ((result)->x = (p)->x + lambda * ((q)->x - (p)->x),\
     (result)->y = (p)->y + lambda * ((q)->y - (p)->y),\
     (result)->z = (p)->z + lambda * ((q)->z - (p)->z))


// dot product of two vector if perpendicular == 0
// 向量內積
#define _Dot_product(a, b)   ((float)(((a)->x * (b)->x) + ((a)->y * (b)->y) + ((a)->z * (b)->z)))

inline float Dot_product( const LG::Vector3f* a, const LG::Vector3f* b )
{
    return _Dot_product( a, b );
}

#if defined(_X86_)
inline float SSE_Dot_product( const LG::Vector3f* a, const LG::Vector3f* b )
{
    __m128 fa = { a->x, a->y, a->z, 0.0f };
    __m128 fb = { b->x, b->y, b->z, 0.0f };

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVUPS   xmm0, fa
        MOVUPS   xmm1, fb
        MULPS    xmm0, xmm1
        MOVUPS   fa, xmm0
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVUPS   xmm0, fa ;\
        MOVUPS   xmm1, fb ;\
        MULPS    xmm0, xmm1  ;\
        MOVUPS   fa,   xmm0  ;"
        : "=m"
        ( fa )
        : "m"( fa ), "m"(fb)
        __ASM_END__
#endif
        return _m128_get_f32( fa, 0 ) + _m128_get_f32( fa, 1 ) + _m128_get_f32( fa, 2 ); //fa.m128_f32[0] + fa.m128_f32[1] + fa.m128_f32[2];
}
#endif

// cross product of two vector if n perpendicular a and b == 0
// normal vector
// 兩向量的法向量
// 向量外積
#define _Cross_product(n, a, b)  ((float)((n)->x = ((a)->y * (b)->z) - ((a)->z * (b)->y), \
                                          (n)->y = ((a)->z * (b)->x) - ((a)->x * (b)->z), \
                                          (n)->z = ((a)->x * (b)->y) - ((a)->y * (b)->x)))

inline void Cross_product( LG::Vector3f* n, const LG::Vector3f* a, const LG::Vector3f* b )
{
    _Cross_product( n, a, b );
}

#if defined(_X86_)
inline void SSE_Cross_product( LG::Vector3f* n, const LG::Vector3f* a, const LG::Vector3f* b )
{
    __m128 a1 = { a->y, a->z, a->x, 0.0f };
    __m128 a2 = { a->z, a->x, a->y, 0.0f };
    __m128 b1 = { b->z, b->x, a->y, 0.0f };
    __m128 b2 = { b->y, b->z, b->x, 0.0f };
    //__m128 r0 = {0.0f, 0.0f, 0.0f, 0.0f};

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVUPS   xmm0, a1
        MOVUPS   xmm1, a2
        MOVUPS   xmm3, b1
        MOVUPS   xmm4, b2

        MULPS xmm0, xmm3
        MULPS xmm1, xmm4
        SUBPS xmm0, xmm1
        MOVUPS   a1, xmm0
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVUPS   xmm0, a1   ;\
   MOVUPS   xmm1, a2   ;\
   MOVUPS   xmm3, b1   ;\
   MOVUPS   xmm4, b2   ;\
        ;\
   MULPS xmm0, xmm3 ;\
   MULPS xmm1, xmm4 ;\
   SUBPS xmm0, xmm1 ;\
   MOVUPS   a1, xmm0   ;"
        : "=m"
        ( a1 )
        : "m"( a1 ), "m"( a2 ), "m"( b1 ), "m"(b2)
        __ASM_END__
#endif
        n->x = _m128_get_f32( a1, 0 );
    n->y = _m128_get_f32( a1, 1 );
    n->z = _m128_get_f32( a1, 2 );
}
#endif

// (y1*y2-y2*y1)-(x1*z2-x2*z1)+(x1*y2-x2*y1)
// 可以得到 順時針還是逆時針的差別
// 順-  逆+
//#define Cross_product2(a, b)    ((float)(((a.y*b.y)-(b.y*a.y))-((a.x*b.z)-(b.x*a.z))+((a.x*b.y)-(b.x*a.y))))


#define _LengthXY(a)    ((float)sqrt((float)(((a)->x * (a)->x) + ((a)->y * (a)->y))))
#define _LengthXYZ(a)      ((float)sqrt((float)_Dot_product((a), (a))))

inline float LengthXYZ( const LG::Vector3f* a )
{
    return _LengthXYZ( a );
}

#if defined(_X86_)
inline float SSE_LengthXYZ( const LG::Vector3f* a )
{
    return ( (float)sqrt( (float)SSE_Dot_product( a, a ) ) );
}
#endif

// 兩個向量是否垂直
#define _Is_Vertical(a, b)    (((float)(((a)->x * (b)->x) + ((a)->y * (b)->y) + ((a)->z * (b)->z))) == 0)? true: false

// 向量相減
#define _Subt_vec(AB, a, b)   ((AB)->x = (b)->x - (a)->x, \
                               (AB)->y = (b)->y - (a)->y, \
                               (AB)->z = (b)->z - (a)->z)


#define _Square(n)      (n*n)
// 向量夾角 回傳一個徑度量
// 最多是 180 所以超過的部分得自己做換算
#define _VectorIncludedRadian(v0, v1)  (((float)acos((float)(_Dot_product(v0, v1) / (_LengthXYZ(v0)*_LengthXYZ(v1))))))

inline float VectorIncludedRadian( const LG::Vector3f* v0, const LG::Vector3f* v1 )
{
    return _VectorIncludedRadian( v0, v1 );
}

#if defined(_X86_)
inline float SSE_VectorIncludedRadian( const LG::Vector3f* v0, const LG::Vector3f* v1 )
{
    return ( (float)acos( (float)( SSE_Dot_product( v0, v1 ) / ( SSE_LengthXYZ( v0 ) * SSE_LengthXYZ( v1 ) ) ) ) );
}
#endif


/*
inline float VectorIncludedRadian(LG::Vector3f zv, LG::Vector3f v0)
{
 return (((float)(((zv.y*v0.y)-(v0.y*zv.y))-((zv.x*v0.z)-(v0.x*zv.z))+((zv.x*v0.y)-(v0.x*zv.y))))<0)? (_TWO_PI_ - ((float)acos((float)(Dot_product(zv, v0)/(LengthXYZ(zv)*LengthXYZ(v0)))))): ((float)acos((float)(Dot_product(zv, v0)/(LengthXYZ(zv)*LengthXYZ(v0)))));
}
*/


// 可忽略的極小值
#define EPS 1e-7


// 單位向量化
// unit vector
inline void Normalize( LG::Vector3f* vec )
{
    float len = (float)_LengthXYZ( vec );
    if ( len < EPS )
    {
        return;
    }

    vec->x = _SET_PRECISION( vec->x / len );
    vec->y = _SET_PRECISION( vec->y / len );
    vec->z = _SET_PRECISION( vec->z / len );

    //vec.x /= len;
    //vec.y /= len;
    //vec.z /= len;
    return;
}

//static __m128 _FULL_MIN_FLOAT = {_MIN_FLOAT_, _MIN_FLOAT_, _MIN_FLOAT_, _MIN_FLOAT_};
#if defined(_X86_)
inline void SSE_Normalize( LG::Vector3f* vec )
{
    float len = (float)SSE_LengthXYZ( vec );
    if ( len < EPS )
    {
        return;
    }

    //(int(f1*_MIN_FLOAT_)/_MIN_FLOAT_)
    __m128 va = { vec->x, vec->y, vec->z, 0.0f };

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVUPS   xmm0, va
        MOVSS xmm1, len
        SHUFPS   xmm1, xmm1, 0x00
        DIVPS xmm0, xmm1
        MOVUPS   va, xmm0
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVUPS   xmm0, va   ;\
   MOVSS xmm1, len  ;\
   SHUFPS   xmm1, xmm1, 0x00 ;\
   DIVPS xmm0, xmm1    ;\
   MOVUPS   va,   xmm0    ;"
        : "=m"
        ( va )
        : "m"( va ), "m"(len)
        __ASM_END__
#endif

        vec->x = _m128_get_f32( va, 0 );
    vec->y = _m128_get_f32( va, 1 );
    vec->z = _m128_get_f32( va, 2 );

    //vec.x /= len;
    //vec.y /= len;
    //vec.z /= len;
    return;
}
#endif


inline double CubeXLen( const LG::BBoxf* c )
{
    LG::Vector3f vec( c->vertex[ 0 ].x - c->vertex[ 0 ].x,
        c->vertex[ 0 ].y - c->vertex[ 0 ].y,
        c->vertex[ 0 ].z - c->vertex[ 1 ].z );
    return _LengthXYZ( &vec );
}

inline double CubeZLen( const LG::BBoxf* c )
{
    LG::Vector3f vec( c->vertex[ 0 ].x - c->vertex[ 1 ].x,
        c->vertex[ 0 ].y - c->vertex[ 0 ].y,
        c->vertex[ 0 ].z - c->vertex[ 0 ].z );
    return _LengthXYZ( &vec );
}

inline double CubeYLen( const LG::BBoxf* c )
{
    LG::Vector3f vec( c->vertex[ 0 ].x - c->vertex[ 0 ].x,
        c->vertex[ 0 ].y - c->vertex[ 1 ].y,
        c->vertex[ 0 ].z - c->vertex[ 0 ].z );
    return _LengthXYZ( &vec );
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
//
//
//  2D line formula ax + by + c = 0 have p1(x0, y0) p2(x1, y1)
//  a = (y0 - y1)
//  b = (x1 - x0)
//  c = (x0y1 - y0x1)
//
//
//
// 兩個點得出一線
inline void Linear_Constants( LG::Linear* src, const LG::Point* p0, const LG::Point* p1 )
{
    src->normal.x = p0->y - p1->y;
    src->normal.y = p1->x - p0->x;
    src->cnst = ( p0->x * p1->y ) - ( p1->x * p0->y );
    return;
}


// 兩的點的距離
inline double VertexToVertex( const LG::Vertex3f* v0, const LG::Vertex3f* v1 )
{
    float lx = ( v1->x - v0->x );
    float ly = ( v1->y - v0->y );
    float lz = ( v1->z - v0->z );
    return sqrt( ( ( lx * lx ) + ( ly * ly ) + ( lz * lz ) ) );
}

#if defined(_X86_)
inline double SSE_VertexToVertex( const LG::Vertex3f* v0, const LG::Vertex3f* v1 )
{
    __m128 va0 = { v0->x, v0->y, v0->z, 0.0f };
    __m128 va1 = { v1->x, v1->y, v1->z, 0.0f };

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVUPS   xmm0, va0
        MOVUPS   xmm1, va1
        SUBPS xmm0, xmm1
        MOVUPS   xmm1, xmm0
        MULPS xmm0, xmm1
        MOVUPS   va0, xmm0
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVUPS   xmm0, va0  ;\
   MOVUPS   xmm1, va1  ;\
   SUBPS xmm0, xmm1 ;\
   MOVUPS   xmm1, xmm0 ;\
   MULPS xmm0, xmm1 ;\
   MOVUPS   va0,  xmm0 ;"
        : "=m"
        ( va0 )
        : "m"( va0 ), "m"(va1)
        __ASM_END__
#endif

        return sqrt( ( _m128_get_f32( va0, 0 ) + _m128_get_f32( va0, 1 ) + _m128_get_f32( va0, 2 ) ) );
}
#endif


// 一個向量 到平面的距離
inline double VertexToPlane( const LG::Vertex3f* v, const LG::Plane3df* p )
{
    return ( ( p->normal.x * v->x ) +
        ( p->normal.y * v->y ) +
        ( p->normal.z * v->z ) +
        p->cnst ) /
        _LengthXYZ( &p->normal );
}

// 一個向量 到平面的距離
#if defined(_X86_)
inline double SSE_VertexToPlane( const LG::Vertex3f* v, const LG::Plane3df* p )
{
    return ( ( p->normal.x * v->x ) +
        ( p->normal.y * v->y ) +
        ( p->normal.z * v->z ) +
        p->cnst ) /
        SSE_LengthXYZ( &p->normal );
}
#endif



// 三個點得出一面
inline void Plane_Constants( LG::Plane3df* p,
    const LG::Vertex3f* v0,
    const LG::Vertex3f* v1,
    const LG::Vertex3f* v2 )
{
    LG::Vector3f pq, pr; //difference vector PQ and PR
    // To speedup the macros
    _Subt_vec( &pq, v0, v1 );
    _Subt_vec( &pr, v0, v2 );
    //Cross_product(p.normal, pq, pr);

    p->normal.x = ( pq.y * pr.z ) - ( pq.z * pr.y );
    p->normal.y = ( pq.z * pr.x ) - ( pq.x * pr.z );
    p->normal.z = ( pq.x * pr.y ) - ( pq.y * pr.x );
    p->cnst = 0 - ( _Dot_product( &p->normal, v0 ) );
    return;
}


// inline double VectorIncludedAngle(const LG::Vector3f &v0, const LG::Vector3f &v1)
// {
//  float angle = (float)acos(Dot_product(v0, v1)/
//          (LengthXYZ(v0)*LengthXYZ(v1)));
//  return 360 * (angle/_TWO_PI_);
// }


/*
inline double VertexToLine(const LG::Vertex3f &v, const LG::Vertex3f &LineStart, const LG::Vertex3f &LineEnd)
{
 float LineMag;
 float U;
 LG::Vertex3f Intersection;

 LineMag = (float)VertexToVertex(LineEnd, LineStart);

 U = (((v.x - LineStart.x) * (LineEnd.x - LineStart.x)) +
    ((v.y - LineStart.y) * (LineEnd.y - LineStart.y)) +
    ((v.z - LineStart.z) * (LineEnd.z - LineStart.z))) /
    (LineMag * LineMag);

 if(U < 0.0f || U > 1.0f)
  return 0;   // closest point does not fall within the line segment

 Intersection.x = LineStart.x + U * (LineEnd.x - LineStart.x);
 Intersection.y = LineStart.y + U * (LineEnd.y - LineStart.y);
 Intersection.z = LineStart.z + U * (LineEnd.z - LineStart.z);

 return VertexToVertex(v, Intersection);
}
*/

/*
inline LG::Vertex3f Projection(LG::Vertex3f &v, float focus)
{
 LG::Vertex3f v2;
 v2.x = v.x * (focus/v.z);
 v2.y = v.y * (focus/v.y),
 v2.z = v.z;
 return v2;
}
*/


/*
 float Distance(const LG::Vertex3f* v)
 {

  return (abs((normal.x * v->x) + (normal.y * v->y) + (normal.z * v->z))) /
     sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z)) ;
 }
*/


// 3點的法向量和它們的平面常數
/*
typedef struct
{
 LG::Vertex3f normal;  //法向量
 float   cnst;
}Plane;
*/


/*
inline void plane_constants(Polynomial3D* pqr, float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2)
{
 LG::Vertex3f pq, pr; //difference vector PQ and PR
 LG::Vertex3f* n = (LG::Vertex3f*)(&(pqr->normal));
 // To speedup the macros
 Subt_vec(pq, p, q);
 Cross_product((*n), pq, pr);
 pqr->cnst = Dot_product((*n), p);
}
*/


#define _INFINITE3D 1e20

#define _Linear_comb(r, p, pq, l)\
    ((r)->x = (p)->x + l * (pq)->x,/*Similar to Point_on_line()*/\
     (r)->y = (p)->y + l * (pq)->y,\
     (r)->z = (p)->z + l * (pq)->z)
#define _Is_zero(x)  (fabs(x) < EPS)


inline void sect_line_and_plane( LG::Vertex3f* s,
    float* lambda,
    bool* parallel,
    LG::Vertex3f* a,
    LG::Vertex3f* ab,
    LG::Plane3df* p )
{
    //begin
    LG::Vertex3f* n = (LG::Vertex3f*)&( p->normal );
    //speed up the macro
    float n_ab = _Dot_product( n, ab );
    if ( _Is_zero( n_ab ) )
    {
        //Either the line coincides with the plane or it is parallel to the plane
        *parallel = TRUE;
        if ( _Is_zero( _Dot_product( n, a ) - p->cnst ) )
        {
            *lambda = 0;
        }
        else
        {
            ( *lambda ) = (float)( _INFINITE3D );
        }
    }
    else
    {
        parallel = FALSE;
        *lambda = ( p->cnst - _Dot_product( n, a ) ) / n_ab;
    }
    _Linear_comb( s, a, ab, *lambda );
}



//===========================================================================================================
//  矩陣
//===========================================================================================================

// 將矩陣單位化
static LG::Matrix4f IdentityMatrix( 1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );

inline void   SetIdentityMatrix( LG::Matrix4f* result )
{
    __pfmemcpy( result->m, IdentityMatrix.m, LG::_MATRIX4_LEN * sizeof( float ) );
    //result->m[0] = 1.0f; result->m[4] = 0.0f; result->m[8]  = 0.0f; result->m[12] = 0.0f;
    //result->m[1] = 0.0f; result->m[5] = 1.0f; result->m[9]  = 0.0f; result->m[13] = 0.0f;
    //result->m[2] = 0.0f; result->m[6] = 0.0f; result->m[10] = 1.0f; result->m[14] = 0.0f;
    //result->m[3] = 0.0f; result->m[7] = 0.0f; result->m[11] = 0.0f; result->m[15] = 0.1f;
}

inline void Mat4MulVec3( LG::Vertex3f* ra, const LG::Matrix4f* ma, const LG::Vertex3f* va )
{
    ( *ra ) = ( *ma ) * ( *va );
}

#if defined(_X86_)
inline void SSE_Mat4MulVec3( LG::Vertex3f* ra, const LG::Matrix4f* ma, const LG::Vertex3f* va )
{
    __m128 va0 = { va->x, va->y, va->z, 0.0f };

    __m128 ma0 = { ma->m[ 0 ], ma->m[ 1 ], ma->m[ 2 ],  0.0f };
    __m128 ma1 = { ma->m[ 4 ], ma->m[ 5 ], ma->m[ 6 ],  0.0f };
    __m128 ma2 = { ma->m[ 8 ], ma->m[ 9 ], ma->m[ 10 ], 0.0f };

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVSS   xmm1, _m128_get_f32( va0, 0 )
        SHUFPS  xmm1, xmm1, 0x00
        MOVSS   xmm2, _m128_get_f32( va0, 1 )
        SHUFPS  xmm2, xmm2, 0x00
        MOVSS   xmm3, _m128_get_f32( va0, 2 )
        SHUFPS  xmm3, xmm3, 0x00

        MOVUPS  xmm4, ma0
        MOVUPS  xmm5, ma1
        MOVUPS  xmm6, ma2

        MULPS   xmm4, xmm1
        MULPS   xmm5, xmm2
        MULPS   xmm6, xmm3
        ADDPS   xmm4, xmm5
        ADDPS   xmm4, xmm6

        MOVUPS ma0, xmm4
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVSS   xmm1, _m128_get_f32(va0, 0) ;\
   SHUFPS  xmm1, xmm1, 0x00   ;\
   MOVSS   xmm2, _m128_get_f32(va0, 1) ;\
   SHUFPS  xmm2, xmm2, 0x00   ;\
   MOVSS   xmm3, _m128_get_f32(va0, 2) ;\
   SHUFPS  xmm3, xmm3, 0x00   ;\
            ;\
   MOVUPS  xmm4, ma0       ;\
   MOVUPS  xmm5, ma1       ;\
   MOVUPS  xmm6, ma2       ;\
            ;\
   MULPS   xmm4, xmm1      ;\
   MULPS   xmm5, xmm2      ;\
   MULPS   xmm6, xmm3      ;\
   ADDPS   xmm4, xmm5      ;\
   ADDPS   xmm4, xmm6      ;\
            ;\
   MOVUPS ma0,  xmm4       ;"
        : "=m"
        ( ma0 )
        : "m"( va0 ), "m"( ma0 ), "m"( ma1 ), "m"(ma2)
        __ASM_END__
#endif

        ra->x = _m128_get_f32( ma0, 0 ); //ma0.m128_f32[0];
    ra->y = _m128_get_f32( ma0, 1 ); //ma0.m128_f32[1];
    ra->z = _m128_get_f32( ma0, 2 ); //ma0.m128_f32[2];
}
#endif

inline void Mat4MulMat4( LG::Matrix4f* dm, const LG::Matrix4f* m0, const LG::Matrix4f* m1 )
{
    ( *dm ) = ( *m0 ) * ( *m1 );
}

#if defined(_X86_)
inline void SSE_Mat4MulMat4( LG::Matrix4f* dm, const LG::Matrix4f* m0, const LG::Matrix4f* m1 )
/*float result[4][4], float left[4][4], float right[4][4])*/
{
    float* result = dm->m;
    const float* left = m0->m;
    const float* right = m1->m;

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOV   eax, left
        MOV   edi, right
        MOV   edx, result
        MOVUPS   xmm4, [ edi ]
        MOVUPS   xmm5, [ edi + 16 ]
        MOVUPS   xmm6, [ edi + 32 ]
        MOVUPS   xmm7, [ edi + 48 ]
        MOV   edi, 0
        MOV   ecx, 4
        _loop:
    MOVUPS   xmm0, [ eax + edi ]
        MOVUPS   xmm1, xmm0
        MOVUPS   xmm2, xmm0
        MOVUPS   xmm3, xmm0
        SHUFPS   xmm0, xmm0, 00000000b
        SHUFPS   xmm1, xmm1, 01010101b
        SHUFPS   xmm2, xmm2, 10101010b
        SHUFPS   xmm3, xmm3, 11111111b
        MULPS xmm0, xmm4
        MULPS xmm1, xmm5
        MULPS xmm2, xmm6
        MULPS xmm3, xmm7
        ADDPS xmm2, xmm0
        ADDPS xmm3, xmm1
        ADDPS xmm3, xmm2
        MOVUPS[ edx + edi ], xmm3
        ADD   edi, 16
        LOOP   _loop
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOV   eax,  left      ;\
   MOV   edi,  right     ;\
   MOV   edx,  result    ;\
   MOVUPS   xmm4,    [edi]     ;\
   MOVUPS   xmm5,    [edi+16]     ;\
   MOVUPS   xmm6,    [edi+32]     ;\
   MOVUPS   xmm7,    [edi+48]     ;\
   MOV   edi,  0      ;\
   MOV   ecx,  4      ;\
  _loop:           ;\
   MOVUPS   xmm0,    [eax+edi]    ;\
   MOVUPS   xmm1,    xmm0      ;\
   MOVUPS   xmm2,    xmm0      ;\
   MOVUPS   xmm3,    xmm0      ;\
   SHUFPS   xmm0,    xmm0, 00000000b ;\
   SHUFPS   xmm1,    xmm1, 01010101b ;\
   SHUFPS   xmm2,    xmm2, 10101010b ;\
   SHUFPS   xmm3,    xmm3, 11111111b ;\
   MULPS xmm0,    xmm4      ;\
   MULPS xmm1,    xmm5      ;\
   MULPS xmm2,    xmm6      ;\
   MULPS xmm3,    xmm7      ;\
   ADDPS xmm2,    xmm0      ;\
   ADDPS xmm3,    xmm1      ;\
   ADDPS xmm3,    xmm2      ;\
   MOVUPS   [edx+edi],  xmm3      ;\
   ADD   edi,  16     ;\
   LOOP   _loop       ;"
        :
    : "r"
        ( result ), "r"( left ), "r"(right)
        __ASM_END__
#endif
}
#endif

inline bool InvertMatrix( LG::Matrix4f* dm, const LG::Matrix4f* sm )
{
    int i, j, k, swap;
    double t;
    double temp[ 4 ][ 4 ];

    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < 4; j++ )
        {
            temp[ i ][ j ] = sm->m[ i * 4 + j ];
        }
    }

    SetIdentityMatrix( dm );

    for ( i = 0; i < 4; i++ )
    {
        /*
        ** Look for largest element in column
        */
        swap = i;
        for ( j = i + 1; j < 4; j++ )
        {
            if ( fabs( temp[ j ][ i ] ) > fabs( temp[ i ][ i ] ) )
            {
                swap = j;
            }
        }

        if ( swap != i )
        {
            /*
            ** Swap rows.
            */
            for ( k = 0; k < 4; k++ )
            {
                t = temp[ i ][ k ];
                temp[ i ][ k ] = temp[ swap ][ k ];
                temp[ swap ][ k ] = t;

                t = dm->m[ i * 4 + k ];
                dm->m[ i * 4 + k ] = dm->m[ swap * 4 + k ];
                dm->m[ swap * 4 + k ] = t;
            }
        }

        if ( temp[ i ][ i ] == 0 )
        {
            /*
            ** No non-zero pivot.  The matrix is singular, which shouldn't
            ** happen.  This means the user gave us a bad matrix.
            */
            return false;
        }

        t = temp[ i ][ i ];
        for ( k = 0; k < 4; k++ )
        {
            temp[ i ][ k ] /= t;
            dm->m[ i * 4 + k ] /= t;
        }
        for ( j = 0; j < 4; j++ )
        {
            if ( j != i )
            {
                t = temp[ j ][ i ];
                for ( k = 0; k < 4; k++ )
                {
                    temp[ j ][ k ] -= temp[ i ][ k ] * t;
                    dm->m[ j * 4 + k ] -= dm->m[ i * 4 + k ] * t;
                }
            }
        }
    }
    return true;
}

#if defined(_X86_)
inline bool SSE_InvertMatrix( LG::Matrix4f* dm, const LG::Matrix4f* sm )
{
    int i, j, k, swap;
    double t;
    double temp[ 4 ][ 4 ];

    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < 4; j++ )
        {
            temp[ i ][ j ] = sm->m[ i * 4 + j ];
        }
    }

    SetIdentityMatrix( dm );

    for ( i = 0; i < 4; i++ )
    {
        /*
        ** Look for largest element in column
        */
        swap = i;
        for ( j = i + 1; j < 4; j++ )
        {
            if ( fabs( temp[ j ][ i ] ) > fabs( temp[ i ][ i ] ) )
            {
                swap = j;
            }
        }

        if ( swap != i )
        {
            /*
            ** Swap rows.
            */
            for ( k = 0; k < 4; k++ )
            {
                t = temp[ i ][ k ];
                temp[ i ][ k ] = temp[ swap ][ k ];
                temp[ swap ][ k ] = t;

                t = dm->m[ i * 4 + k ];
                dm->m[ i * 4 + k ] = dm->m[ swap * 4 + k ];
                dm->m[ swap * 4 + k ] = t;
            }
        }

        if ( temp[ i ][ i ] == 0 )
        {
            /*
            ** No non-zero pivot.  The matrix is singular, which shouldn't
            ** happen.  This means the user gave us a bad matrix.
            */
            return false;
        }

        t = temp[ i ][ i ];
        for ( k = 0; k < 4; k++ )
        {
            temp[ i ][ k ] /= t;
            dm->m[ i * 4 + k ] /= t;
        }
        for ( j = 0; j < 4; j++ )
        {
            if ( j != i )
            {
                t = temp[ j ][ i ];
                for ( k = 0; k < 4; k++ )
                {
                    temp[ j ][ k ] -= temp[ i ][ k ] * t;
                    dm->m[ j * 4 + k ] -= dm->m[ i * 4 + k ] * t;
                }
            }
        }
    }
    return true;
}
#endif



// X軸旋轉矩陣
inline void   SetXAxisRotationMatrix( LG::Matrix4f* result, float radian )
{
    SetIdentityMatrix( result );
    if ( abs( (long32_t)radian ) <= EPS )
    {
        return;
    }
    float sina = (float)sin( radian );
    float cosa = (float)cos( radian );
    /*result->m[0] = 1.0f; result->m[4] = 0.0f; result->m[8]  =  0.0f; result->m[12] = 0.0f;*/
    /*result->m[1] = 0.0f;*/result->m[ 5 ] = cosa;
    result->m[ 9 ] = -sina;/*result->m[13] = 0.0f;*/
    /*result->m[2] = 0.0f;*/result->m[ 6 ] = sina;
    result->m[ 10 ] = cosa;/*result->m[14] = 0.0f;*/
    /*result->m[3] = 0.0f; result->m[7] = 0.0f; result->m[11] =  0.0f; result->m[15] = 1.0f;*/
}

// Y軸旋轉矩陣
inline void   SetYAxisRotationMatrix( LG::Matrix4f* result, float radian )
{
    SetIdentityMatrix( result );
    if ( abs( (long32_t)radian ) <= EPS )
    {
        return;
    }
    float sina = (float)sin( radian );
    float cosa = (float)cos( radian );
    result->m[ 0 ] = cosa; /*result->m[4] = 0.0f;*/ result->m[ 8 ] = sina; /*result->m[12] = 0.0f;*/
    /*result->m[1] =  0.0f; result->m[5] = 1.0f; result->m[9]  = 0.0f; result->m[13] = 0.0f;*/
    result->m[ 2 ] = -sina; /*result->m[6] = 0.0f;*/ result->m[ 10 ] = cosa; /*result->m[14] = 0.0f;*/
    /*result->m[3] =  0.0f; result->m[7] = 0.0f; result->m[11] = 0.0f; result->m[15] = 1.0f;*/
}

// Z軸旋轉矩陣
inline void   SetZAxisRotationMatrix( LG::Matrix4f* result, float radian )
{
    SetIdentityMatrix( result );
    if ( abs( (long32_t)radian ) <= EPS )
    {
        return;
    }
    float sina = (float)sin( radian );
    float cosa = (float)cos( radian );
    result->m[ 0 ] = cosa;
    result->m[ 4 ] = -sina; /*result->m[8]  = 0.0f; result->m[12] = 0.0f;*/
    result->m[ 1 ] = sina;
    result->m[ 5 ] = cosa; /*result->m[9]  = 0.0f; result->m[13] = 0.0f;*/
    /*result->m[2] = 0.0f; result->m[6] =  0.0f; result->m[10] = 0.0f; result->m[14] = 0.0f;*/
    /*result->m[3] = 0.0f; result->m[7] =  0.0f; result->m[11] = 0.0f; result->m[15] = 1.0f;*/
}


//
// 要計算 在空間中有任意的一個點(x ,y, z) ，打算對著向量（u , v , c) 做旋轉A度 (徑度量) 所需　要的矩陣
// 任一點 乘 該矩陣即可得到 新的點
// OK
inline void SetVectorRotationMatrix( LG::Matrix4f* result, const LG::Vector3f* axis, float radian )
{
    SetIdentityMatrix( result );
    if ( abs( (long32_t)radian ) <= EPS )
    {
        return;
    }
    // SETP1 要先單位向量化 不然會變大
    LG::Vector3f na = ( *axis );
    Normalize( &na ); //為對應OPENGL OPENGL 的矩陣只取到 小數點下 1位數
    float cosa = (float)cos( radian );
    float sina = (float)sin( radian );
    float ax = na.x;
    float ay = na.y;
    float az = na.z;
    float ax2 = ax * ax;
    float ay2 = ay * ay;
    float az2 = az * az;
    float bcosa = ( 1 - cosa );
    float ax_sina = ( ax * sina );
    float ay_sina = ( ay * sina );
    float az_sina = ( az * sina );
    float ax_ay_bcosa = ( ax * ay * ( bcosa ) );
    float az_ax_bcosa = ( az * ax * ( bcosa ) );
    float ay_az_bcosa = ( ay * az * ( bcosa ) );

    result->m[ 0 ] = (ax2)+( cosa * ( 1 - ( ax2 ) ) );
    result->m[ 1 ] = ax_ay_bcosa + az_sina;
    result->m[ 2 ] = az_ax_bcosa - ay_sina;
    //result->m[3]  = 0;

    result->m[ 4 ] = ax_ay_bcosa - az_sina;
    result->m[ 5 ] = (ay2)+( cosa * ( 1 - ( ay2 ) ) );
    result->m[ 6 ] = ay_az_bcosa + ax_sina;
    //result->m[7]  = 0;

    result->m[ 8 ] = az_ax_bcosa + ay_sina;
    result->m[ 9 ] = ay_az_bcosa - ax_sina;
    result->m[ 10 ] = (az2)+( cosa * ( 1 - ( az2 ) ) );
    //result->m[11] = 0;
    //
    //result->m[12] = 0;
    //result->m[13] = 0;
    //result->m[14] = 0;
    //result->m[15] = 1;
    return;
}

#if defined(_X86_)
inline void SSE_SetVectorRotationMatrix( LG::Matrix4f* result, const LG::Vector3f* axis, float radian )
{
    SetIdentityMatrix( result );
    if ( abs( (long32_t)radian ) <= EPS )
    {
        return;
    }
    // SETP1 要先單位向量化 不然會變大
    LG::Vector3f na = ( *axis );
    SSE_Normalize( &na ); //為對應OPENGL OPENGL 的矩陣只取到 小數點下 1位數
    float cosa = cos( radian );
    float sina = sin( radian );
    //float ax = na.x;      float ay = na.y;      float az = na.z;
    //float ax2   = ax*ax;     float ay2   = ay*ay;        float az2   = az*az;
    float bcosa = ( 1 - cosa );

    //float ax_sina = (ax*sina);    float ay_sina = (ay*sina);    float az_sina = (az*sina);
    // float ax_ay_bcosa = (ax*ay*(bcosa)); float az_ax_bcosa = (az*ax*(bcosa)); float ay_az_bcosa = (ay*az*(bcosa));

    __m128 a1 = { na.x, na.y, na.z, 0.0f };
    __m128 a11 = { na.y, na.z, na.x, 0.0f };
    __m128 a2 = { 0.0f, 0.0f, 0.0f, 0.0f };
    __m128 a_sina = { 0.0f, 0.0f, 0.0f, 0.0f };
    __m128 aa_bcosa = { 0.0f, 0.0f, 0.0f, 0.0f };  // ax_ay_bcosa, ay_az_bcosa, az_ax_bcosa
    __m128 cosa_x_1 = { 0.0f, 0.0f, 0.0f, 0.0f };  // ax_ay_bcosa, ay_az_bcosa, az_ax_bcosa
    float  c1 = 1.0f;

#if defined(__MSVC__)
    __ASM_BEGIN__
        MOVUPS   xmm0, a1    // float ax = na.x;      float ay = na.y;      float az = na.z;
        MOVUPS   xmm1, xmm0
        MULPS xmm1, xmm0  // float ax2   = ax*ax;     float ay2   = ay*ay;        float az2  = az*az;
        MOVUPS   a2, xmm1

        MOVSS xmm2, sina
        SHUFPS   xmm2, xmm2, 0x00  // sina   sina   sina
        MULPS xmm2, xmm0
        MOVUPS   a_sina, xmm2  // float ax_sina = (ax*sina);     float ay_sina = (ay*sina);     float az_sina = (az*sina);

        MOVSS xmm3, bcosa
        SHUFPS   xmm3, xmm3, 0x00  // bcosa   bcosa   bcosa
        MOVUPS   xmm4, a11
        MULPS xmm4, xmm0
        MULPS xmm4, xmm3
        MOVUPS   aa_bcosa, xmm4  // ax_ay_bcosa, ay_az_bcosa, az_ax_bcosa

        // (cosa*(1-(a2.m128_f32[0]))), (cosa*(1-(a2.m128_f32[1]))), (cosa*(1-(a2.m128_f32[2])))
        MOVSS xmm5, c1    //
        SHUFPS   xmm5, xmm5, 0x00  // 1     , 1       , 1
        SUBPS xmm5, xmm1  // 1-(a2.m128_f32[0]), (1-(a2.m128_f32[1])), (1-(a2.m128_f32[2]))
        MOVSS xmm6, cosa
        SHUFPS   xmm6, xmm6, 0x00  // cosa     , cosa    , cosa
        MULPS xmm5, xmm6  //(cosa*(1-(a2.m128_f32[0]))), (cosa*(1-(a2.m128_f32[1]))), (cosa*(1-(a2.m128_f32[2])))
        MOVUPS   cosa_x_1, xmm5
        __ASM_END__
#endif
#if defined(__GNUC__)
        __ASM_BEGIN__
        "   MOVUPS   xmm0,  a1   ;  # float ax = na.x;       float ay = na.y;       float az = na.z;  \
   MOVUPS   xmm1,  xmm0    ;\
   MULPS xmm1,  xmm0    ;  # float ax2   = ax*ax;      float ay2   = ay*ay;      float az2  = az*az; \
   MOVUPS   a2,    xmm1    ;\
            ;\
   MOVSS xmm2,  sina    ;\
   SHUFPS   xmm2,  xmm2, 0x00 ;  # sina   sina   sina \
   MULPS xmm2,  xmm0    ;\
   MOVUPS   a_sina,   xmm2    ;  # float ax_sina = (ax*sina);   float ay_sina = (ay*sina);   float az_sina = (az*sina); \
            ;\
   MOVSS xmm3,  bcosa   ; \
   SHUFPS   xmm3,  xmm3, 0x00 ;  # bcosa   bcosa   bcosa \
   MOVUPS   xmm4,  a11  ;\
   MULPS xmm4,  xmm0    ;\
   MULPS xmm4,  xmm3    ;\
   MOVUPS   aa_bcosa, xmm4    ;  # ax_ay_bcosa, ay_az_bcosa, az_ax_bcosa \
            ;\
   MOVSS xmm5,  c1   ;  # \
   SHUFPS   xmm5,  xmm5, 0x00 ;  # 1     , 1       , 1 \
   SUBPS xmm5,  xmm1    ;  # 1-(a2.m128_f32[0]), (1-(a2.m128_f32[1])), (1-(a2.m128_f32[2])) \
   MOVSS xmm6,  cosa    ;\
   SHUFPS   xmm6,  xmm6, 0x00 ;  #  cosa     , cosa    , cosa \
   MULPS xmm5,  xmm6    ;  # (cosa*(1-(a2.m128_f32[0]))), (cosa*(1-(a2.m128_f32[1]))), (cosa*(1-(a2.m128_f32[2]))) \
   MOVUPS   cosa_x_1, xmm5    ; "
        :
    : "m"
        ( a1 ), "r"( sina ), "r"( bcosa ), "m"( a11 ), "r"( c1 ), "r"(cosa)
        __ASM_END__
#endif

        result->m[ 0 ] = _m128_get_f32( a2, 0 ) + _m128_get_f32( cosa_x_1, 0 ); //(cosa*(1-(a2.m128_f32[0])));
    result->m[ 1 ] = _m128_get_f32( aa_bcosa, 0 ) + _m128_get_f32( a_sina, 2 );
    result->m[ 2 ] = _m128_get_f32( aa_bcosa, 2 ) - _m128_get_f32( a_sina, 1 );
    //result->m[3]  = 0;

    result->m[ 4 ] = _m128_get_f32( aa_bcosa, 0 ) - _m128_get_f32( a_sina, 2 );
    result->m[ 5 ] = _m128_get_f32( a2, 1 ) + _m128_get_f32( cosa_x_1, 1 ); //(cosa*(1-(a2.m128_f32[1])));
    result->m[ 6 ] = _m128_get_f32( aa_bcosa, 1 ) + _m128_get_f32( a_sina, 0 );
    //result->m[7]  = 0;

    result->m[ 8 ] = _m128_get_f32( aa_bcosa, 2 ) + _m128_get_f32( a_sina, 1 );
    result->m[ 9 ] = _m128_get_f32( aa_bcosa, 1 ) - _m128_get_f32( a_sina, 0 );
    result->m[ 10 ] = _m128_get_f32( a2, 2 ) + _m128_get_f32( cosa_x_1, 2 ); //(cosa*(1-(a2.m128_f32[2])));
    //result->m[11] = 0;

    //result->m[12] = 0;
    //result->m[13] = 0;
    //result->m[14] = 0;
    //result->m[15] = 1;
    return;
}
#endif


//////////////////////////////////////////////////////////////////////////
// func

#if defined(_X86_)
static float  ( *Dot_productFunc )( const LG::Vector3f* a, const LG::Vector3f* b ) = SSE_Dot_product;
static void   ( *Cross_productFunc )( LG::Vector3f* n, const LG::Vector3f* a, const LG::Vector3f* b ) = SSE_Cross_product;
static float  ( *LengthXYZFunc )( const LG::Vector3f* a ) = SSE_LengthXYZ;
static float  ( *VectorIncludedRadianFunc )( const LG::Vector3f* v0, const LG::Vector3f* v1 ) = SSE_VectorIncludedRadian;
static void   ( *NormalizeFunc )( LG::Vector3f* vec ) = SSE_Normalize;
static double ( *VertexToVertexFunc )( const LG::Vertex3f* v0, const LG::Vertex3f* v1 ) = SSE_VertexToVertex;
static void   ( *Mat4MulVec3Func )( LG::Vertex3f* ra, const LG::Matrix4f* ma, const LG::Vertex3f* va ) = SSE_Mat4MulVec3;
static void   ( *Mat4MulMat4Func )( LG::Matrix4f* dm, const LG::Matrix4f* m0, const LG::Matrix4f* m1 ) = SSE_Mat4MulMat4;
static void   ( *SetVectorRotationMatrixFunc )( LG::Matrix4f* result, const LG::Vector3f* axis, float radian ) = SSE_SetVectorRotationMatrix;
static bool   ( *InvertMatrixFunc )( LG::Matrix4f* dm, const LG::Matrix4f* sm ) = SSE_InvertMatrix;
#else
static float  ( *Dot_productFunc )( const LG::Vector3f* a, const LG::Vector3f* b ) = Dot_product;
static void   ( *Cross_productFunc )( LG::Vector3f* n, const LG::Vector3f* a, const LG::Vector3f* b ) = Cross_product;
static float  ( *LengthXYZFunc )( const LG::Vector3f* a ) = LengthXYZ;
static float  ( *VectorIncludedRadianFunc )( const LG::Vector3f* v0, const LG::Vector3f* v1 ) = VectorIncludedRadian;
static void   ( *NormalizeFunc )( LG::Vector3f* vec ) = Normalize;
static double ( *VertexToVertexFunc )( const LG::Vertex3f* v0, const LG::Vertex3f* v1 ) = VertexToVertex;
static void   ( *Mat4MulVec3Func )( LG::Vertex3f* ra, const LG::Matrix4f* ma, const LG::Vertex3f* va ) = Mat4MulVec3;
static void   ( *Mat4MulMat4Func )( LG::Matrix4f* dm, const LG::Matrix4f* m0, const LG::Matrix4f* m1 ) = Mat4MulMat4;
static void   ( *SetVectorRotationMatrixFunc )( LG::Matrix4f* result, const LG::Vector3f* axis, float radian ) = SetVectorRotationMatrix;
static bool   ( *InvertMatrixFunc )( LG::Matrix4f* dm, const LG::Matrix4f* sm ) = InvertMatrix;
#endif


enum InstructionFuncType
{
    _X86_FUNC = 0,
    _X86_SSE_FUNC = 1,
};

inline void SetInstruction( InstructionFuncType ft = _X86_SSE_FUNC )
{
    switch ( ft )
    {
    case _X86_FUNC:
        {
            Dot_productFunc = Dot_product;
            Cross_productFunc = Cross_product;
            LengthXYZFunc = LengthXYZ;
            VectorIncludedRadianFunc = VectorIncludedRadian;
            NormalizeFunc = Normalize;
            VertexToVertexFunc = VertexToVertex;
            Mat4MulVec3Func = Mat4MulVec3;
            Mat4MulMat4Func = Mat4MulMat4;
            SetVectorRotationMatrixFunc = SetVectorRotationMatrix;
            InvertMatrixFunc = InvertMatrix;
        }
        break;
#if defined(_X86_)
    case _X86_SSE_FUNC:
        {
            Dot_productFunc = SSE_Dot_product;
            Cross_productFunc = SSE_Cross_product;
            LengthXYZFunc = SSE_LengthXYZ;
            VectorIncludedRadianFunc = SSE_VectorIncludedRadian;
            NormalizeFunc = SSE_Normalize;
            VertexToVertexFunc = SSE_VertexToVertex;
            Mat4MulVec3Func = SSE_Mat4MulVec3;
            Mat4MulMat4Func = SSE_Mat4MulMat4;
            SetVectorRotationMatrixFunc = SSE_SetVectorRotationMatrix;
            InvertMatrixFunc = SSE_InvertMatrix;
        }
        break;
#endif
    }
};



//
//
// 繞 X 軸 旋轉
//    1   0  0    0
//  x =  0  cos(r)  -sin(r)   0
//    0  sin(r)   cos(r)   0
//    0   0  0    1
//
// 繞 Y 軸 旋轉
//   cos(b)  0    sin(b)   0
//  y =  0   1  0    0
//  -sin(b)  0    cos(b)   0
//    0   0  0    1
//
// 繞 Z 軸 旋轉
//    cos(a)  -sin(a) 0    0
//  z =  sin(a)   cos(a) 0    0
//  0  0  1    0
//  0  0  0    1
//
// 3個相乘
//
//  R =
//
//
// 填入3個角度 以得到 換算後的旋轉矩陣
//
inline void AngleToMatrix( LG::Matrix4f* result, const LG::Angle3f* a3f )
{
    // STEP1 換算成radian
    float radiiansx = _DEGREE_TO_RADIAN( a3f->roll );
    float radiiansy = _DEGREE_TO_RADIAN( a3f->pitch );
    float radiiansz = _DEGREE_TO_RADIAN( a3f->yaw );
    float sina = (float)sin( radiiansx );
    float cosa = (float)cos( radiiansx );
    float sinb = (float)sin( radiiansy );
    float cosb = (float)cos( radiiansy );
    float sinr = (float)sin( radiiansz );
    float cosr = (float)cos( radiiansz );

    float cosa_cosr = cosa * cosr;
    float sina_cosr = sina * cosr;
    float sina_sinr = sina * sinr;
    float cosa_sinr = cosa * sinr;

    result->m[ 0 ] = cosa_cosr - ( cosb * sina_sinr );
    result->m[ 1 ] = ( -cosa_sinr ) - ( cosb * sina_cosr );
    result->m[ 2 ] = sinb * sina;
    result->m[ 3 ] = 0;
    result->m[ 4 ] = (sina_cosr)+( cosb * cosa_sinr );
    result->m[ 5 ] = ( -sina_sinr ) + ( cosb * cosa_cosr );
    result->m[ 6 ] = -sinb * cosa;
    result->m[ 7 ] = 0;
    result->m[ 8 ] = sinb * sinr;
    result->m[ 9 ] = sinb * cosr;
    result->m[ 10 ] = cosb;
    result->m[ 11 ] = 0;
    result->m[ 12 ] = 0;
    result->m[ 13 ] = 0;
    result->m[ 14 ] = 0;
    result->m[ 15 ] = 1;
    return;
}

inline void AxisToMatrix( LG::Matrix4f* result, const LG::Axis3f* a3f )
{

}

//===========================================================================================================
//  座標軸
//===========================================================================================================
//
//
//  傳入面朝的向量 與 該法向量 得到 3個轉動的角度
//  OK
//
inline void AxisToAngle( LG::Angle3f* result, const LG::Axis3f* a3f )
{
    LG::Vector3f v0( 0, 0, 1 );
    LG::Vector3f v1( 0, a3f->fw.y, a3f->fw.z );
    LG::Vector3f v2( a3f->fw.x, 0, a3f->fw.z );

    result->roll = _RADIAN_TO_DEGREE( VectorIncludedRadianFunc( &v0, &v1 ) );
    result->pitch = _RADIAN_TO_DEGREE( VectorIncludedRadianFunc( &v0, &v2 ) );
    //result->yaw   = RADIAN_TO_DEGREE(VectorIncludedRadian(LG::Vector3f(0, 1, 0),  LG::Vector3f(a3f->up.x, a3f->up.y, 0)));
    result->yaw = a3f->up;
}

// 傳入 與 x, y, z軸的角度 return 換算的單位向量
inline void AngleToAxis( LG::Axis3f* result, const LG::Angle3f* a3f )
{
    // STEP1 換算成radian
    float radiiansx = _DEGREE_TO_RADIAN( a3f->roll );
    float radiiansy = _DEGREE_TO_RADIAN( a3f->pitch );
    //float radiiansz = DEGREE_TO_RADIAN(a3f->yaw);

    float cos_a = (float)cos( radiiansx );
    float sin_a = (float)sin( radiiansx );
    float cos_b = (float)cos( radiiansy );
    float sin_b = (float)sin( radiiansy );

    result->fw( cos_a * sin_b, -sin_a, cos_a * cos_b );
    //result->up(sin(radiiansz), cos(radiiansz), 0);
    result->up = a3f->yaw;
}

// 傳入Axis 傳出 他的 up forward left 單位向量(距離為1的點)
// 如果要得到點 自己在加上 at
inline void AxisToVertex( LG::Vector3f& fw, LG::Vector3f& up, LG::Vector3f& lf, const LG::Axis3f* a3f )
{
    fw = a3f->fw;
    //step 1 : 先將 前方的向量 單位化
    NormalizeFunc( &fw );
    up = LG::_YAxisf;
    lf = LG::_XAxisf;
    LG::Vector3f v0( fw.x, 0, fw.z );
    //Normalize(v0);
    // step 2 : 先算出 沿著Y軸轉的角度 與 沿著X軸轉的角度
    float rtty = 0.0f;
    if ( ( fw.x != 0 ) || ( fw.z != 0 ) )
    {
        rtty = VectorIncludedRadianFunc( &LG::_ZAxisf, &v0 );
    } // 在xz平面上 沿著y軸的旋轉角度

    if ( fw.x < 0 )
    {
        rtty = _TWO_PI_ - rtty;
    }

    // step 3 : 把up 跟 lf 也沿著 Y軸轉同樣的角度
    if ( rtty != 0.0f )
    {
        LG::Matrix4f mty;
        SetYAxisRotationMatrix( &mty, rtty );
        Mat4MulVec3Func( &lf, &mty, &lf );
    }
    // step 4 :  up 是 fw 跟 lf的法向量  /*計算fw 沿著Y軸提升的角度*/
    Cross_productFunc( &up, &fw, &lf );
    //float rttx=VectorIncludedRadian(v0, fw);   // 在xz平面上 沿著x軸的旋轉角度
    // step 5 : 把up 沿著 新的lf 軸轉同樣的角度
    //if(rttx !=0.0f)
    //{
    // LG::Matrix4f mtx;
    // SetVectorRotationMatrix(&mtx, &lf, rttx);
    // up = mtx*up;
    //}


    //float ttd = Dot_product(up, fw);
    /*
    if(ttd !=0)
    {
     float aa=(up.x * fw.x);
     float bb=(up.y * fw.y);
     float cc=(up.z * fw.z);
     double dd=0.029986236f-0.029984800f;
     int kkk=0;
    }
    */



    // step 6 : 沿著fw 轉動up的角度
    if ( a3f->up != 0.0f )
    {
        LG::Matrix4f mtz;
        SetVectorRotationMatrixFunc( &mtz, &fw, _DEGREE_TO_RADIAN( a3f->up ) );
        Mat4MulVec3Func( &up, &mtz, &up );
        //up = mtz*up;
    }

    // step 5 : right 為 fw 與 up 的法向量 有旋轉過up 再重算一次lf
    if ( a3f->up != 0.0f )
    {
        Cross_productFunc( &lf, &up, &fw );
    }

    NormalizeFunc( &up );
    NormalizeFunc( &lf );
}

//
// 在空間中有任意的一個點(x ,y, z) ，打算對著向量（u , v , c) 做旋轉A度 (徑度量) 之後得到的點
// 空間點繞任意軸旋轉變換公式
// P點繞A向量旋轉θ角後得到P'：
// P' = Pcosθ + (A × P)sinθ + A(A•P)(1 - cosθ)
// 注意：視口為向量指向的位置，就是向量指向你，θ為逆時針旋轉的角。
// A × P = (Ay*Pz - Az*Py,Az*Px - Ax*Pz,Ax*Py - Ay*Px)
// 注意：A必須是單位向量

inline void SetVectorRotationMatrix( LG::Vertex3f* vr, const LG::Vertex3f* vs, const LG::Vector3f* axis, float radian )
{

}

inline bool CreateRay( int winx, int winy,
    const LG::Matrix4f* projMatrix,
    const LG::Vertex3f* camerapos,
    const int viewport[ 4 ],
    LG::Vertex3f* raypos, LG::Vector3f* raydir )
{
    //螢幕座標到投影後座標
    //Screen coordinate to viewport coordinate
    int w = viewport[ 2 ] - viewport[ 0 ];
    int h = viewport[ 3 ] - viewport[ 1 ];
    double _x1 = 2.0 * winx / w - 1;
    double _y1 = 1 - 2.0 * winy / h;
    LG::Vertex3f v1( _x1, _y1, 1.0 );

    //投影座標到相機座標
    //Viewport coordinate to Carmera coordinate
    double _x2 = v1.x / projMatrix->m[ 0 ];
    double _y2 = v1.y / projMatrix->m[ 5 ];
    LG::Vertex3f v2( _x2, _y2, 1.0 );

    //建立pickray
    //Carmera coordinate to world coordinate
    LG::Vertex3f ss( camerapos->x, camerapos->y, camerapos->z );
    LG::Vector4f dir0( v2.x - ss.x, v2.y - ss.y, v2.z - ss.z, 0.0f );
    NormalizeFunc( &dir0 );
    LG::Vertex4f pos0( ss.x, ss.y, ss.z, 1.0f );

    //pickray轉換世界座標
    LG::Matrix4f mm( 1.0f, 0.0f, 0.0f, camerapos->x,
        0.0f, 1.0f, 0.0f, camerapos->y,
        0.0f, 0.0f, 1.0f, camerapos->z,
        0.0f, 0.0f, 0.0f, 1.0f );

    ( *raypos ) = mm * pos0;
    ( *raydir ) = mm * dir0;

    return true;

}

inline LG::Vertex3f UnProject( int winx, int winy,
    const LG::Matrix4f* modelMatrix,
    const LG::Matrix4f* projMatrix,
    const int viewport[ 4 ] )
{
    LG::Vertex4f pos0;
    return pos0;
}

#include <set>

namespace MathSet
{
    ///////////////////////////////////////////////////////////////////////////////
    //  聯集
    inline void Union( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutUnion )
    {
        OutUnion.clear();
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            OutUnion.insert( std::set<long64_t>::value_type( *itera ) );
            itera++;
        }

        std::set<long64_t>::iterator iterb = ( ( std::set<long64_t> )SetB ).begin();
        while ( iterb != SetB.end() )
        {
            OutUnion.insert( std::set<long64_t>::value_type( *itera ) );
            iterb++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  交集
    inline void Interaction( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutInteraction )
    {
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterb = ( ( std::set<long64_t> )SetB ).find( *itera );
            if ( iterb != SetB.end() )
            {
                OutInteraction.insert( std::set<long64_t>::value_type( *itera ) );
            }
            itera++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  聯集 和 交集 (減少重複行為)
    inline void Union_Interaction( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutUnion,
        std::set<long64_t>& OutInteraction )
    {
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterb = ( ( std::set<long64_t> )SetB ).find( *itera );
            if ( iterb != SetB.end() )
            {
                OutInteraction.insert( std::set<long64_t>::value_type( *itera ) );
            }
            OutUnion.insert( std::set<long64_t>::value_type( *itera ) );
            itera++;
        }

        std::set<long64_t>::iterator iterb = ( ( std::set<long64_t> )SetB ).begin();
        while ( iterb != SetB.end() )
        {
            OutUnion.insert( std::set<long64_t>::value_type( *itera ) );
            iterb++;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    //  差集
    inline void Difference( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutDifference )
    {
        std::set<long64_t> SetBTmp = ( std::set<long64_t> )SetB;
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterbt = SetBTmp.find( *itera );
            if ( iterbt == SetBTmp.end() )
            {
                OutDifference.insert( std::set<long64_t>::value_type( *itera ) );
            }
            else
            {
                SetBTmp.erase( iterbt );
            }
            itera++;
        }

        std::set<long64_t>::iterator iterb = SetBTmp.begin();
        while ( iterb != SetBTmp.end() )
        {
            OutDifference.insert( *iterb );
            iterb++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  聯集 和 差集
    inline void Union_Difference( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutUnion,
        std::set<long64_t>& OutDifference )
    {
        std::set<long64_t> SetBTmp = ( std::set<long64_t> )SetB;
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterbt = SetBTmp.find( *itera );
            if ( iterbt == SetBTmp.end() )
            {
                OutDifference.insert( std::set<long64_t>::value_type( *itera ) );
            }
            else
            {
                SetBTmp.erase( iterbt );
                OutUnion.insert( std::set<long64_t>::value_type( *iterbt ) );
            }
            OutUnion.insert( std::set<long64_t>::value_type( *itera ) );
            itera++;
        }

        std::set<long64_t>::iterator iterbt = SetBTmp.begin();
        while ( iterbt != SetBTmp.end() )
        {
            OutDifference.insert( *iterbt );
            OutUnion.insert( std::set<long64_t>::value_type( *iterbt ) );
            iterbt++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  補集  只有A
    inline void Complementary( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutComplementaryA )
    {
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterb = ( ( std::set<long64_t> )SetB ).find( *itera );
            if ( iterb == SetB.end() )
            {
                OutComplementaryA.insert( std::set<long64_t>::value_type( *itera ) );
            }
            itera++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //  補集  A B
    inline void Complementary( const std::set<long64_t> SetA, const std::set<long64_t> SetB,
        std::set<long64_t>& OutComplementaryA,
        std::set<long64_t>& OutComplementaryB )
    {
        OutComplementaryB = ( std::set<long64_t> )SetB;
        std::set<long64_t>::iterator itera = ( ( std::set<long64_t> )SetA ).begin();
        while ( itera != SetA.end() )
        {
            std::set<long64_t>::iterator iterb = OutComplementaryB.find( *itera );
            if ( iterb == OutComplementaryB.end() )
            {
                OutComplementaryA.insert( std::set<long64_t>::value_type( *itera ) );
            }
            else
            {
                OutComplementaryB.erase( iterb );
            }
            itera++;
        }
    }
}