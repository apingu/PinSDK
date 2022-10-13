// CPShape.h header file of graphics shape
//
//
//  Graphics define
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

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
//

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "pf-def.h"
#include "graphic2D.h"


//1=3D  0=type(0=2f, 1=3L, 2=3f) 01
#define  _P_VERTEX2F_   0X3F21 //點
#define  _P_VERTEX3L_   0X3001 //點
#define  _P_VERTEX3F_   0X3F01 //點
#define  _P_VECTOR3F_   0X3F11 //向量
#define  _P_LINEV3F_    0X3F02 //線
#define  _P_PLANE3DF_   0X3F03 //平面
#define  _P_SPHEREF_    0X3F00 //圓形
#define  _P_CUBE_       0X3F08 //矩形


//=============================================================================
// NAME:
// DESC: This is our basic 3D class.  This will be used to store the UV coordinates.
//=============================================================================

namespace LG
{
    ///////////////////////////////////////////////////////////////////////////////
    //  vertex class
    template <class _Type>
    class Vertex2
    {
    public:

        _Type   x, y;

        Vertex2(): x( 0 ), y( 0 ) {}
        // This is our constructor that allows us to initialize our data upon creating an instance
        Vertex2( _Type X, _Type Y )                       { operator()( X, Y ); }
        Vertex2( _Type afCoordinate[ 2 ] )                  { x = afCoordinate[ 0 ]; y = afCoordinate[ 1 ]; }
        Vertex2( const Vertex2<_Type>& rVertex )          { operator() ( rVertex.x, rVertex.y ); }

        _Type& operator[]( unsigned int i ) const         { return ( _Type& ) * ( &x + i ); }

        Vertex2& operator=( const Vertex2& rVertex )      { operator() ( rVertex.x, rVertex.y ); return *this; }

        void operator() ( _Type X = 0, _Type Y = 0 )      { x = X; y = Y; }

        bool operator==( const Vertex2& rVertex ) const   { return ( ( x == rVertex.x ) && ( y == rVertex.y ) ); }

        bool operator!=( const Vertex2& rVertex ) const   { return ( ( x != rVertex.x ) || ( y != rVertex.y ) ); }

        // Here we overload the + operator so we can add Vertexs together
        Vertex2 operator+( Vertex2 vVertex ) const        { return Vertex2( vVertex.x + x, vVertex.y + y ); }

        // Here we overload the - operator so we can subtract Vertexs
        Vertex2 operator-( Vertex2 vVertex ) const        { return Vertex2( x - vVertex.x, y - vVertex.y ); }

        // Here we overload the*operator so we can multiply by scalars
        Vertex2 operator*( _Type num ) const              { return Vertex2( x * num, y * num ); }

        // Here we overload the/operator so we can divide by a scalar
        Vertex2 operator/( _Type num ) const              { return Vertex2( x / num, y / num ); }
    };

    typedef Vertex2<float> Vertex2f;



    ///////////////////////////////////////////////////////////////////////////////
    //  Vertex3

    //=============================================================================
    // NAME:
    // DESC: This is our basic 3D point/Vertex class
    //=============================================================================
    template <class _Type>
    class Vertex3
    {
    public:

        _Type   x;
        _Type   y;
        _Type   z;

        Vertex3(): x( 0 ), y( 0 ), z( 0 ) {}
        // This is our constructor that allows us to initialize our data upon creating an instance
        Vertex3( _Type nx, _Type ny, _Type nz )                    { operator() ( nx, ny, nz ); }
        Vertex3( _Type afCoordinate[ 3 ] )                         { operator() ( afCoordinate[ 0 ], afCoordinate[ 1 ], afCoordinate[ 2 ] ); }
        Vertex3( const Vertex3<_Type>& rVertex )                   { operator() ( rVertex.x, rVertex.y, rVertex.z ); }

        _Type& operator[]( unsigned int i ) const                  { return ( _Type& ) * ( &x + i ); }

        Vertex3<_Type> operator= ( const Vertex3<_Type>& rVertex ) { operator() ( rVertex.x, rVertex.y, rVertex.z ); return *this; }
        void operator() ( _Type X = 0, _Type Y = 0, _Type Z = 0 )  { x = X; y = Y; z = Z; }
        bool operator==( const Vertex3<_Type>& rVertex ) const     { return ( ( x == rVertex.x ) && ( y == rVertex.y ) && ( z == rVertex.z ) ); }
        bool operator!=( const Vertex3<_Type>& rVertex ) const     { return ( ( x != rVertex.x ) || ( y != rVertex.y ) || ( z != rVertex.z ) ); }

        // Here we overload the + operator so we can add Vertexs together
        Vertex3<_Type> operator+( const Vertex3<_Type>& vVertex ) const { return Vertex3( x + vVertex.x, y + vVertex.y, z + vVertex.z ); }

        // Here we overload the - operator so we can subtract Vertexs
        Vertex3<_Type> operator-( Vertex3<_Type> vVertex ) const  { return Vertex3( x - vVertex.x, y - vVertex.y, z - vVertex.z ); }

        // Here we overload the*operator so we can multiply by scalars
        Vertex3<_Type> operator*( _Type num ) const      { return Vertex3( x * num, y * num, z * num ); }

        // Here we overload the/operator so we can divide by a scalar
        Vertex3<_Type> operator/( _Type num ) const      { return Vertex3( x / num, y / num, z / num ); }
    };

    typedef Vertex3<float>     Vertex3f;
    typedef Vertex3<long32_t>  Vertex3l;


    template <class _Type>
    class Vertex4: public Vertex3<_Type>
    {
    public:

        _Type   w;

        Vertex4(): w( 0 ) {}
        // This is our constructor that allows us to initialize our data upon creating an instance
        Vertex4( _Type nx, _Type ny, _Type nz, _Type nw )      { operator() ( nx, ny, nz, nw ); }
        Vertex4( _Type afCoordinate[ 4 ] )                       { operator() ( afCoordinate[ 0 ], afCoordinate[ 1 ], afCoordinate[ 2 ], afCoordinate[ 3 ] ); }
        Vertex4( const Vertex4<_Type>& rVertex )               { operator() ( rVertex.x, rVertex.y, rVertex.z, rVertex.w ); }

        _Type& operator[]( unsigned int i ) const              { return ( _Type& ) * ( &this->x + i ); }

        Vertex4<_Type> operator= ( const Vertex4<_Type>& rVertex )   { operator() ( rVertex.x, rVertex.y, rVertex.z, rVertex.w ); return *this; }
        void operator() ( _Type X = 0, _Type Y = 0, _Type Z = 0, _Type W = 0 )    { this->x = X; this->y = Y; this->z = Z; this->w = W; }
        bool operator==( const Vertex4<_Type>& rVertex ) const    { return ( this->x == rVertex.x && this->y == rVertex.y && this->z == rVertex.z && this->w == rVertex.w ); }
        bool operator!=( const Vertex4<_Type>& rVertex ) const    { return ( this->x != rVertex.x || this->y != rVertex.y || this->z != rVertex.z || this->w != rVertex.w ); }

        // Here we overload the + operator so we can add Vertexs together
        Vertex4<_Type> operator+( const Vertex4<_Type>& vVertex ) const { return Vertex4( this->x + vVertex.x, this->y + vVertex.y, this->z + vVertex.z, this->w + vVertex.w ); }

        // Here we overload the - operator so we can subtract Vertexs
        Vertex4<_Type> operator-( Vertex4<_Type> vVertex ) const  { return Vertex4( this->x - vVertex.x, this->y - vVertex.y, this->z - vVertex.z, this->w - vVertex.w ); }

        // Here we overload the*operator so we can multiply by scalars
        Vertex4<_Type> operator*( _Type num ) const      { return Vertex4( this->x * num, this->y * num, this->z * num, this->w * num ); }

        // Here we overload the/operator so we can divide by a scalar
        Vertex4<_Type> operator/( _Type num ) const      { return Vertex4( this->x / num, this->y / num, this->z / num, this->w / num ); }
    };

    typedef Vertex4<float> Vertex4f;
    typedef Vertex4<long32_t>  Vertex4l;



    ///////////////////////////////////////////////////////////////////////////////
    //  Vector class
    /*
    template <class _Type>
    class Vector3 : public Vertex3<_Type>
    {
    public:

     Vector3()              { operator()(0, 0, 0); }
     Vector3(_Type X, _Type Y, _Type Z)       { operator()(X, Y, Z); }
     Vector3(_Type afCoordinate[3])         { x = afCoordinate[0]; y = afCoordinate[1]; z = afCoordinate[2]; }

     Vector3(const Vector3<_Type> &vVector)      { operator() (vVector.x, vVector.y, vVector.z); }

     Vector3<_Type> &operator = (const Vector3<_Type> &vVector) { operator()(vVector.x, vVector.y, vVector.z); return *this; }
     Vector3<_Type> &operator = (const Vertex3<_Type>& vVertex) { operator()(vVertex.x, vVertex.y, vVertex.z); return *this; }

     void operator() (_Type X = 0, _Type Y = 0, _Type Z = 0)    { x=X; y=Y; z=Z; }

     // Here we overload the + operator so we can add Vertexs together
     Vector3<_Type> operator+(const Vector3<_Type> &vVector) const { return Vector3(x + vVector.x, y + vVector.y, z + vVector.z); }
     // Here we overload the - operator so we can subtract Vertexs
     Vector3<_Type> operator-(const Vector3<_Type> &vVector) const { return Vector3(x - vVector.x, y - vVector.y, z - vVector.z); }

     // Here we overload the*operator so we can multiply by scalars
     Vector3<_Type> operator*(_Type num) const      { return Vector3<_Type>(x*num, y*num, z*num); }

     // Here we overload the/operator so we can divide by a scalar
     Vector3<_Type> operator/(_Type num) const      { return Vector3<_Type>(x/num, y/num, z/num); }
    };
    */

#define Vector3  Vertex3
    typedef Vertex3<float> Vector3f;

    const   Vector3f   _XAxisf( 1, 0, 0 );
    const   Vector3f   _YAxisf( 0, 1, 0 );
    const   Vector3f   _ZAxisf( 0, 0, 1 );


    ///////////////////////////////////////////////////////////////////////////////
    /*
    template <class _Type>
    class Vector4 : public Vector3<_Type>
    {
    public:

     _Type   w;

     Vector4()             { operator()(0, 0, 0, 0); }

     Vector4(_Type X, _Type Y, _Type Z, _Type W)   { operator()(X, Y, Z, W); }

     Vector4(_Type afCoordinate[4])     { x = afCoordinate[0]; y = afCoordinate[1]; z = afCoordinate[2]; w = afCoordinate[3]; }

     Vector4(const Vector4<_Type> &vVector)     { operator() (vVector.x, vVector.y, vVector.z, vVector.w); }

     Vector4<_Type> &operator =(const Vector4<_Type> &vVector) { operator()(vVector.x, vVector.y, vVector.z, vVector.w); return *this; }

     void operator() (_Type X = 0, _Type Y = 0, _Type Z = 0, _Type W = 0) { x=X; y=Y; z=Z; w=W; }

     // Here we overload the + operator so we can add Vertexs together
     Vector4 operator+(const Vector4 &vVector) const  { return Vector4(x + vVector.x, y + vVector.y, z + vVector.z, w + vVector.w); }
     // Here we overload the - operator so we can subtract Vertexs
     Vector4 operator-(const Vector4 &vVector) const  { return Vector4(x - vVector.x, y - vVector.y, z - vVector.z, w - vVector.w); }

     // Here we overload the*operator so we can multiply by scalars
     Vector4 operator*(_Type num) const      { return Vector4(x*num, y*num, z*num, w*num); }

     // Here we overload the/operator so we can divide by a scalar
     Vector4 operator/(_Type num) const      { return Vector4(x/num, y/num, z/num, , w/num); }
    };
    */

#define Vector4  Vertex4
    typedef Vertex4<float> Vector4f;


    ///////////////////////////////////////////////////////////////////////////////
    //  matrix class
    //
    // a0 a3 a6
    // a1 a4 a7
    // a2 a5 a8
    //
    template <class _Type>
    class Matrix3
    {
    public:

        _Type m[ 9 ];

        Matrix3()
        {
            m[ 0 ] = 0;
            m[ 1 ] = 0;
            m[ 2 ] = 0;
            m[ 3 ] = 0;
            m[ 4 ] = 0;
            m[ 5 ] = 0;
            m[ 6 ] = 0;
            m[ 7 ] = 0;
            m[ 8 ] = 0;
        }

        Matrix3( _Type m0, _Type m1, _Type m2,
            _Type m3, _Type m4, _Type m5,
            _Type m6, _Type m7, _Type m8 )
        {
            m[ 0 ] = m0;
            m[ 1 ] = m1;
            m[ 2 ] = m2;
            m[ 3 ] = m3;
            m[ 4 ] = m4;
            m[ 5 ] = m5;
            m[ 6 ] = m6;
            m[ 7 ] = m7;
            m[ 8 ] = m8;
        }

        _Type& operator[]( unsigned int i ) const
        {
            return m[ i ];
        }

        bool operator==( const Matrix3& rkMatrix ) const
        {
            return ( memcmp( m, rkMatrix.m, sizeof( _Type ) * 9 ) == 0 );
        }

        bool operator!=( const Matrix3& rkMatrix ) const
        {
            return ( memcmp( m, rkMatrix.m, sizeof( _Type ) * 9 ) != 0 );
        }

        Matrix3& operator=( const Matrix3& rkMatrix ) const
        {
            memcpy( m, rkMatrix.m, sizeof( _Type ) * 9 );
            return *this;
        }
    };

    typedef Matrix3<float> Matrix3f;


    ///////////////////////////////////////////////////////////////////////////////
    //  matrix class
    //
    // a0 a4 a8  a12
    // a1 a5 a9  a13
    // a2 a6 a10 a14
    // a3 a7 a11 a15
    //
    const int _MATRIX4_LEN = 16;

    template <class _Type>
    class Matrix4
    {
    public:

        _Type m[ _MATRIX4_LEN ];

        Matrix4()
        {
            m[ 0 ] = 0;
            m[ 1 ] = 0;
            m[ 2 ] = 0;
            m[ 3 ] = 0;
            m[ 4 ] = 0;
            m[ 5 ] = 0;
            m[ 6 ] = 0;
            m[ 7 ] = 0;
            m[ 8 ] = 0;
            m[ 9 ] = 0;
            m[ 10 ] = 0;
            m[ 11 ] = 0;
            m[ 12 ] = 0;
            m[ 13 ] = 0;
            m[ 14 ] = 0;
            m[ 15 ] = 0;
        }

        Matrix4( _Type m00, _Type m01, _Type m02, _Type m03,
            _Type m04, _Type m05, _Type m06, _Type m07,
            _Type m08, _Type m09, _Type m10, _Type m11,
            _Type m12, _Type m13, _Type m14, _Type m15 )
        {
            m[ 0 ] = m00;
            m[ 1 ] = m01;
            m[ 2 ] = m02;
            m[ 3 ] = m03;
            m[ 4 ] = m04;
            m[ 5 ] = m05;
            m[ 6 ] = m06;
            m[ 7 ] = m07;
            m[ 8 ] = m08;
            m[ 9 ] = m09;
            m[ 10 ] = m10;
            m[ 11 ] = m11;
            m[ 12 ] = m12;
            m[ 13 ] = m13;
            m[ 14 ] = m14;
            m[ 15 ] = m15;
        }


        LG::Vertex3<_Type> operator*( const LG::Vertex3<_Type>& rkVector ) const
        {
            LG::Vertex3<_Type> kProd;
            kProd.x = ( rkVector.x * m[ 0 ] ) +
                ( rkVector.y * m[ 4 ] ) +
                ( rkVector.z * m[ 8 ] );
            kProd.y = ( rkVector.x * m[ 1 ] ) +
                ( rkVector.y * m[ 5 ] ) +
                ( rkVector.z * m[ 9 ] );
            kProd.z = ( rkVector.x * m[ 2 ] ) +
                ( rkVector.y * m[ 6 ] ) +
                ( rkVector.z * m[ 10 ] );
            return kProd;
        }


        //
        // v.x  m[0]  m[4]  m[8]   m[12]  vx*m[0] + v.y*m[4] + v.z*m[8] + v.w*m[12]
        // v.y *   m[1]  m[5]  m[9]   m[13]  =  vx*m[1] + v.y*m[5] + v.z*m[9] + v.w*m[13]
        // v.z  m[2]  m[6]  m[10]  m[14]  vx*m[2] + v.y*m[6] + v.z*m[10] + v.w*m[14]
        // v.w  m[3]  m[7]  m[11]  m[15]  vx*m[3] + v.y*m[7] + v.z*m[11] + v.w*m[15]
        //
        LG::Vertex4<_Type> operator*( const LG::Vertex4<_Type>& rkVector ) const
        {
            LG::Vertex4<_Type> kProd;
            kProd.x = ( rkVector.x * m[ 0 ] ) +
                ( rkVector.y * m[ 4 ] ) +
                ( rkVector.z * m[ 8 ] ) +
                ( rkVector.w * m[ 12 ] );
            kProd.y = ( rkVector.x * m[ 1 ] ) +
                ( rkVector.y * m[ 5 ] ) +
                ( rkVector.z * m[ 9 ] ) +
                ( rkVector.w * m[ 13 ] );
            kProd.z = ( rkVector.x * m[ 2 ] ) +
                ( rkVector.y * m[ 6 ] ) +
                ( rkVector.z * m[ 10 ] ) +
                ( rkVector.w * m[ 14 ] );
            kProd.w = ( rkVector.x * m[ 3 ] ) +
                ( rkVector.y * m[ 7 ] ) +
                ( rkVector.z * m[ 11 ] ) +
                ( rkVector.w * m[ 15 ] );
            return kProd;
        }


        //
        // m[0]  m[4]  m[8]   m[12]  m[0]  m[4]  m[8]   m[12]
        // m[1]  m[5]  m[9]   m[13] *   m[1]  m[5]  m[9]   m[13]  =
        // m[2]  m[6]  m[10]  m[14]  m[2]  m[6]  m[10]  m[14]
        // m[3]  m[7]  m[11]  m[15]  m[3]  m[7]  m[11]  m[15]
        //
        // ([0]*[0])+([1]*[4])+([2]*[8]) +([3]*[12]), ([4]*[0])+([5]*[4])+([6]*[8]) +([7]*[12]), ([8]*[0])+([9]*[4])+([10]*[8]) +([11]*[12]), ([12]*[0])+([13]*[4])+([14]*[8]) +([15]*[12])
        // ([0]*[1])+([1]*[5])+([2]*[9]) +([3]*[13]), ([4]*[1])+([5]*[5])+([6]*[9]) +([7]*[13]), ([8]*[1])+([9]*[5])+([10]*[9]) +([11]*[13]), ([12]*[1])+([13]*[5])+([14]*[9]) +([15]*[13])
        // ([0]*[2])+([1]*[6])+([2]*[10])+([3]*[14]), ([4]*[2])+([5]*[6])+([6]*[10])+([7]*[14]), ([8]*[2])+([9]*[6])+([10]*[10])+([11]*[14]), ([12]*[2])+([13]*[6])+([14]*[10])+([15]*[14])
        // ([0]*[3])+([1]*[7])+([2]*[11])+([3]*[15]), ([4]*[3])+([5]*[7])+([6]*[11])+([7]*[15]), ([8]*[3])+([9]*[7])+([10]*[11])+([11]*[15]), ([12]*[3])+([13]*[7])+([14]*[11])+([15]*[15])
        //
        LG::Matrix4<_Type> operator*( const LG::Matrix4<_Type>& rkMatrix ) const
        {
            LG::Matrix4<_Type> kProd;
            for( int i = 0; i < 4; i++ )
            {
                for( int j = 0; j < 4; j++ )
                {
                    kProd.m[ i * 4 + j ] = ( m[ i * 4 + 0 ] * rkMatrix.m[ 0 * 4 + j ] ) +
                        ( m[ i * 4 + 1 ] * rkMatrix.m[ 1 * 4 + j ] ) +
                        ( m[ i * 4 + 2 ] * rkMatrix.m[ 2 * 4 + j ] ) +
                        ( m[ i * 4 + 3 ] * rkMatrix.m[ 3 * 4 + j ] );
                }
            }
            return kProd;
        }


    };

    typedef Matrix4<float> Matrix4f;


    ///////////////////////////////////////////////////////////////////////////////
    //  line class
    template <class _Type>
    class Line3d
    {
    public:

        Vertex3<_Type>  vertex[ 2 ];

        Line3d() {};
        Line3d( Vertex3<_Type> v0, Vertex3<_Type> v1 )
        {
            vertex[ 0 ] = v0;
            vertex[ 1 ] = v1;
        }
    };

    typedef Line3d<float> Line3df;

    ///////////////////////////////////////////////////////////////////////////////
    // Plane
    template <class _Type>
    class Plane3d
    {
    public:

        Vector3<_Type>  normal;
        float     cnst;

        Plane3d()
        {
            cnst = 0;
        }
        //Plane3d(){_Type x, _Type y, _Type z, _Type c)
        //{ normal.x = x; normal.y = y; normal.z = z; cnst = c; }
        Plane3d( const Plane3d<_Type>& other ): cnst( other.cnst ),
            normal( other.normal )
        {
        };

        ~Plane3d()
        {
        };
    };

    typedef Plane3d<float> Plane3df;

    ///////////////////////////////////////////////////////////////////////////////
    //  column class
    template <class _Type>
    class Column
    {
    public:
        Vertex3<_Type>  top;
        Vertex3<_Type>  bottom;
        float     radius;
    };


    ///////////////////////////////////////////////////////////////////////////////
    //  cube class
    template <class _Type>
    class Cube
    {
    public:

        Vertex3<_Type>  vertex[ 2 ];

        Cube()
        {
            vertex[ 0 ].x = vertex[ 0 ].y = vertex[ 0 ].z = vertex[ 1 ].x = vertex[ 1 ].y = vertex[ 1 ].z = 0;
        }
        Cube( float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
        {
            vertex[ 0 ].x = X1;
            vertex[ 0 ].y = Y1;
            vertex[ 0 ].z = Z1;
            vertex[ 1 ].x = X2;
            vertex[ 1 ].y = Y2;
            vertex[ 1 ].z = Z2;
        }

        Cube( const Vertex3<_Type>& v0, const Vertex3<_Type>& v1 )
        {
            vertex[ 0 ] = v0;
            vertex[ 1 ] = v1;
        }


        ~Cube()
        {
        }

        Cube<_Type>& operator=( const Cube<_Type>& box )
        {
            vertex[ 0 ] = box.vertex[ 0 ];
            vertex[ 1 ] = box.vertex[ 1 ];
            return *this;
        }


        Vertex3<_Type> center( void )
        {
            return Vertex3<_Type>( ( vertex[ 0 ] + vertex[ 1 ] ) / 2 );
        }
    };

    typedef Cube<float> BBoxf;
    typedef Cube<float> Cubef;

    ///////////////////////////////////////////////////////////////////////////////
    //  sphere class
    template <class _Type>
    class Sphere
    {
    public:

        Vertex3<_Type>  center;
        float     radius;


        Sphere( float cx = 0, float cy = 0, float cz = 0, float r = 0 )
        {
            operator() ( cx, cy, cz, r );
        }

        Sphere( const Vertex3<_Type>& c, float r )
        {
            operator() ( c, r );
        }

        ~Sphere()
        {
        };

        void operator()( float cx = 0, float cy = 0, float cz = 0, float r = 0 )
        {
            center.x = cx;
            center.y = cy;
            center.z = cz;
            radius = r;
        };

        void operator()( const Vertex3<_Type>& c, float r )
        {
            center = c;
            radius = r;
        }

        /*
          float isCollision(CPSphere s)
          {
         return (radius + s.radius) - ((float)center.Length(s.center));
          }
          float isCollision(CPCube c)
          {
         return -1.0f;
          }
          */
    };

    typedef Sphere<float> Spheref;


    ///////////////////////////////////////////////////////////////////////////////
    //  Polygon class
    template <class _Type>
    class Polygon
    {
    public:
        Vertex3<_Type>* vertex;
        UINT   numVertices;

        Polygon( UINT size = 0 )
        {
            vertex = NULL;
            Allot( size );
        }
        ~Polygon()
        {
            Clear();
        }

        void Allot( UINT size )
        {
            Clear();
            if( size > 0 )
            {
                vertex = new Vertex3<_Type>[ size ];
            }
            numVertices = size;
        }

        void Clear( void )
        {
            SAFE_DELETE_ARRAY( vertex );
            numVertices = 0;
        }

        Vertex3<_Type>& operator []( UINT i )
        {
            if( ( vertex != NULL ) && ( i > numVertices ) )
            {
                return vertex[ i ];
            }
            return 0;
        }
    };

    typedef Polygon<float> Polygonf;


    ///////////////////////////////////////////////////////////////////////////////
    //  TRIANGLE3D class
    template <class _Type>
    class Triangle3d
    {
    public:

        Vertex3<_Type>  vertex[ 3 ];

        Triangle3d()
        {
        };
        Triangle3d( Vertex3<_Type> v0, Vertex3<_Type> v1, Vertex3<_Type> v2 )
        {
            operator() ( v0, v1, v2 );
        }

        ~Triangle3d()
        {
        };

        void operator()( const Vertex3<_Type>& v0,
            const Vertex3<_Type>& v1,
            const Vertex3<_Type>& v2 )
        {
            vertex[ 0 ] = v0;
            vertex[ 1 ] = v1;
            vertex[ 2 ] = v2;
        }

        Triangle3d<_Type>& operator=( const Triangle3d<_Type>& triangle )
        {
            vertex[ 0 ] = triangle.vertex[ 0 ];
            vertex[ 1 ] = triangle.vertex[ 1 ];
            vertex[ 2 ] = triangle.vertex[ 2 ];
            return *this;
        }
    };

    typedef Triangle3d<float> Triangle3df;


    ///////////////////////////////////////////////////////////////////////////////
    template <class _Type>
    class Quads4
    {
    public:

        Vertex3<_Type>  vertex[ 4 ];

        Quads4() {};

        Quads4( const Vertex3<_Type>& v0,
            const Vertex3<_Type>& v1,
            const Vertex3<_Type>& v2,
            const Vertex3<_Type>& v3 )
        {
            operator()( v0, v1, v2, v3 );
        }

        ~Quads4() {};

        void operator()( const Vertex3<_Type>& v0,
            const Vertex3<_Type>& v1,
            const Vertex3<_Type>& v2,
            const Vertex3<_Type>& v3 )
        {
            vertex[ 0 ] = v0;
            vertex[ 1 ] = v0;
            vertex[ 2 ] = v0;
            vertex[ 3 ] = v0;
        }

        bool operator==( const Quads4& rkQuads ) const
        {
            return ( vertex[ 0 ] == rkQuads.vertex[ 0 ] ) && ( vertex[ 1 ] == rkQuads.vertex[ 1 ] ) && ( vertex[ 2 ] == rkQuads.vertex[ 2 ] ) && ( vertex[ 3 ] == rkQuads.vertex[ 3 ] );
        }

        bool operator!=( const Quads4& rkQuads ) const
        {
            return ( vertex[ 0 ] != rkQuads.vertex[ 0 ] ) || ( vertex[ 1 ] != rkQuads.vertex[ 1 ] ) || ( vertex[ 2 ] != rkQuads.vertex[ 2 ] ) || ( vertex[ 3 ] != rkQuads.vertex[ 3 ] );
        }

        Quads4& operator=( const Quads4& rkQuads ) const
        {
            operator()( rkQuads.vertex[ 0 ], rkQuads.vertex[ 1 ], rkQuads.vertex[ 2 ], rkQuads.vertex[ 3 ] );
            return *this;
        }
    };


    typedef Quads4<float> Quads4f;


    ///////////////////////////////////////////////////////////////////////////////
    //  Angle class
    // 用來表示距離x軸 y軸 與 z軸的角度
    template <class _Type>
    class Angle3
    {
    public:

        _Type roll;   // axis X  degree(因為ogl也是用degree當單位)
        _Type pitch;  // axis Y  degree
        _Type yaw; // axis Z  degree

        Angle3(): roll( 0 ), pitch( 0 ), yaw( 0 ) {}
        Angle3( _Type r, _Type p, _Type y ): roll( r ), pitch( p ), yaw( y ) {}
        Angle3( const Angle3& a3 ) { operator=( a3 ); }


        bool operator==( const Angle3& rkAngle ) const   { return ( roll == rkAngle.roll ) && ( pitch == rkAngle.pitch ) && ( yaw == rkAngle.yaw ); }

        bool operator!=( const Angle3& rkAngle ) const   { return ( roll != rkAngle.roll ) || ( pitch != rkAngle.pitch ) || ( yaw != rkAngle.yaw ); }

        Angle3& operator=( const Angle3& rkAngle ) const { roll = rkAngle.roll; pitch = rkAngle.pitch; yaw = rkAngle.yaw; return *this; }
    };

    typedef Angle3<float> Angle3f;


    ///////////////////////////////////////////////////////////////////////////////
    // 3D物件的 位置單位
    // up 向上的向量
    // to 向前的向量
    // at 所在的位置
    template <class _Type>
    class Axis3
    {
    public:

        _Type    up;   //radian(因為ogl也是用degree當單位)
        Vector3<_Type> fw;
        Vertex3<_Type> at;

        Axis3()    { up = 0; fw( 0, 0, 1 ); at( 0, 0, 0 ); }
        Axis3( const Axis3& a3 ) { operator=( a3 ); }
        ~Axis3() {}

        bool operator==( const Axis3& rkAxis ) const { return ( up == rkAxis.up ) && ( fw == rkAxis.fw ) && ( at == rkAxis.at ); }

        bool operator!=( const Axis3& rkAxis ) const { return ( up != rkAxis.up ) || ( fw != rkAxis.fw ) || ( at != rkAxis.at ); }

        Axis3& operator=( const Axis3& rkAxis )   { up = rkAxis.up; fw = rkAxis.fw; at = rkAxis.at; return *this; }
    };

    typedef Axis3<float> Axis3f;

};// namespace LG