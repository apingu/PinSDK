//VfwGraphic
//
//    graphic type framework
//        synchronization with RW
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
#ifndef VFWGRAPHIC_H
#define VFWGRAPHIC_H

#include <VfwKernel.h>
#include <Color.h>
#include <aMath.h>
#include <Graphic3D.h>
#include <tImage.h>

/*
#ifdef _RWKERNEL_
#include <Graphic2D.h>
#endif

#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)

#endif
*/

///////////////////////////////////////////////////////////////////////////////
// float RGBA color 
class VfRGBAf : public LC::RGBAf
{
public:

    //RwRGBAReal  m_Color;
    VfRGBAf( float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f )
    {
        R = r; G = g; B = b; A = a;
    }
    ~VfRGBAf()
    {
    };

#ifdef _RWKERNEL_
    VfRGBAf( const RwRGBAReal& color )
    {
        *( RwRGBAReal * ) this = color;
    }
    operator RwRGBAReal&()
    {
        return ( *( RwRGBAReal * )this );
    }
    //void operator() ( const RwRGBAReal& color ){ *(RwRGBAReal *) this = color; }
#endif
};

///////////////////////////////////////////////////////////////////////////////
// Byte RGBA color 
class VfRGBA : public LC::RGBAb
{
public:

    //RwRGBAReal  m_Color;
    VfRGBA( Pbyte r = 0.0f, Pbyte g = 0.0f, Pbyte b = 0.0f, Pbyte a = 0.0f )
    {
        R = r; G = g; B = b; A = a;
    }
    ~VfRGBA()
    {
    };

#ifdef _RWKERNEL_
    VfRGBA( const RwRGBA& color )
    {
        *( RwRGBA * ) this = color;
    }
    operator RwRGBA&()
    {
        return ( *( RwRGBA * )this );
    }
    //void operator() ( const RwRGBA& color ){ *(RwRGBA *) this = color; }
#endif
};

///////////////////////////////////////////////////////////////////////////////
// 2d float vertex 
class VfVertex2f : public LG::Vertex2f
{
public:
    VfVertex2f( float X = 0.0f, float Y = 0.0f )
    {
        x = X; y = Y;
    }
    ~VfVertex2f()
    {
    };

#ifdef _RWKERNEL_
    VfVertex2f( const RwV2d& vertex )
    {
        *( RwV2d * ) this = vertex;
    }
    operator RwV2d()
    {
        return ( *( RwV2d * )this );
    }
    void operator=( RwV2d vertex )
    {
        x = vertex.x; y = vertex.y;
    }
#endif

    VfVertex2f& operator=( const VfVertex2f& rkVertex )
    {
        x = rkVertex.x;
        y = rkVertex.y;
        return *this;
    }

    VfVertex2f& operator=( const LG::Vertex2f& rkVertex )
    {
        x = rkVertex.x;
        y = rkVertex.y;
        return *this;
    }

    bool operator ==( const VfVertex2f& rkVertex ) const
    {
        return ( x == rkVertex.x && y == rkVertex.y );
    }

    bool operator!=( const VfVertex2f& rkVertex ) const
    {
        return ( x != rkVertex.x || y != rkVertex.y );
    }

    // Here we overload the + operator so we can add Vertexs together 
    VfVertex2f operator+( VfVertex2f vVertex )
    {
        return VfVertex2f( x + vVertex.x, y + vVertex.y );
    }

    // Here we overload the += operator so we can add Vertexs together 
    void operator+=( VfVertex2f vVertex )
    {
        x += vVertex.x; y += vVertex.y;
        return ;
    }

    // Here we overload the - operator so we can subtract Vertexs 
    VfVertex2f operator-( VfVertex2f vVertex )
    {
        return VfVertex2f( x - vVertex.x, y - vVertex.y );
    }

    // Here we overload the * operator so we can multiply by scalars
    VfVertex2f operator*( float num )
    {
        return VfVertex2f( x * num, y * num );
    }

    // Here we overload the / operator so we can divide by a scalar
    VfVertex2f operator/( float num )
    {
        return VfVertex2f( x / num, y / num );
    }
};



///////////////////////////////////////////////////////////////////////////////
// 3d float vertex 
class VfVertex3f : public LG::Vertex3f
{
public:

    VfVertex3f( float X = 0.0f, float Y = 0.0f, float Z = 0.0f )
    {
        x = X; y = Y; z = Z;
    }
    VfVertex3f( const VfVertex3f& v3f )
    {
        *( VfVertex3f * ) this = v3f;
    }
    ~VfVertex3f()
    {
    };

#ifdef _RWKERNEL_
    VfVertex3f( const RwV3d& vertex )
    {
        *( RwV3d * ) this = vertex;
    }
    operator RwV3d()
    {
        return ( *( RwV3d * )this );
    }
    void operator=( RwV3d vertex )
    {
        x = vertex.x; y = vertex.y; z = vertex.z;
    }
#endif

    VfVertex3f& operator=( const VfVertex3f& rkVertex )
    {
        x = rkVertex.x;
        y = rkVertex.y;
        z = rkVertex.z;
        return *this;
    }

    VfVertex3f& operator=( const LG::Vertex3f& rkVertex )
    {
        x = rkVertex.x;
        y = rkVertex.y;
        z = rkVertex.z;
        return *this;
    }

    bool operator ==( const VfVertex3f& rkVertex ) const
    {
        return ( x == rkVertex.x && y == rkVertex.y && z == rkVertex.z );
    }

    bool operator!=( const VfVertex3f& rkVertex ) const
    {
        return ( x != rkVertex.x || y != rkVertex.y || z != rkVertex.z );
    }

    // Here we overload the + operator so we can add Vertexs together 
    VfVertex3f operator+( VfVertex3f vVertex )
    {
        return VfVertex3f( x + vVertex.x, y + vVertex.y, z + vVertex.z );
    }

    // Here we overload the += operator so we can add Vertexs together 
    void operator+=( VfVertex3f vVertex )
    {
        x += vVertex.x; y += vVertex.y; z += vVertex.z;
        return ;
    }

    // Here we overload the - operator so we can subtract Vertexs 
    VfVertex3f operator-( VfVertex3f vVertex )
    {
        return VfVertex3f( x - vVertex.x, y - vVertex.y, z - vVertex.z );
    }

    // Here we overload the * operator so we can multiply by scalars
    VfVertex3f operator*( float num )
    {
        return VfVertex3f( x * num, y * num, z * num );
    }

    // Here we overload the / operator so we can divide by a scalar
    VfVertex3f operator/( float num )
    {
        return VfVertex3f( x / num, y / num, z / num );
    }

#ifdef  _RWKERNEL_
    double length( void )
    {
        return RwV3dLength( &( RwV3d ) ( *this ) );
    }
#endif
};


#ifdef _RWKERNEL_
inline VfVertex3f operator+( RwV3d v1, RwV3d v2 )
{
    RwV3d out;
    RwV3dAdd( &out, &v1, &v2 );
    return VfVertex3f( out );
}
inline VfVertex3f operator-( RwV3d v1, RwV3d v2 )
{
    RwV3d out;
    RwV3dSub( &out, &v1, &v2 );
    return VfVertex3f( out );
}

inline VfVertex3f operator*( RwV3d v1, RwReal m )
{
    RwV3d out;
    RwV3dScale( &out, &v1, m );
    return VfVertex3f( out );
}

inline VfVertex3f operator/( RwV3d v1, RwReal d )
{
    RwV3d out;
    out.x = v1.x / d;
    out.y = v1.y / d;
    out.z = v1.z / d;
    return VfVertex3f( out );
}
#endif

typedef VfVertex3f VfVector3f;

///////////////////////////////////////////////////////////////////////////////
// bounding box
class VfBBox : public LG::BBoxf
{
public:
    VfBBox()
    {
    };
    VfBBox( float X1, float Y1, float Z1, float X2, float Y2, float Z2 )
    {
        vertex[ 0 ].x = X1;vertex[ 0 ].y = Y1;vertex[ 0 ].z = Z1;
        vertex[ 1 ].x = X2;vertex[ 1 ].y = Y2;vertex[ 1 ].z = Z2;
    }
	VfBBox( LG::Cube<float> c )
	{
	    vertex[0].x=c.vertex[0].x;
		vertex[0].y=c.vertex[0].y;
		vertex[0].z=c.vertex[0].z;
	    vertex[1].x=c.vertex[0].x;
		vertex[1].y=c.vertex[0].y;
		vertex[1].z=c.vertex[0].z;
	}

    ~VfBBox()
    {
    };
#ifdef _RWKERNEL_

    VfBBox( RwBBox bbox )
    {
        operator = ( bbox );
        //vertex[0].x = bbox.sup.x; vertex[0].y = bbox.sup.y; vertex[0].z = bbox.sup.z;
        //vertex[1].x = bbox.inf.x; vertex[1].y = bbox.inf.y; vertex[1].z = bbox.inf.z;
    }


    RwBBox& operator=( RwBBox& bbox )
    {
        vertex[ 0 ].x = bbox.sup.x; vertex[ 0 ].y = bbox.sup.y; vertex[ 0 ].z = bbox.sup.z;
        vertex[ 1 ].x = bbox.inf.x; vertex[ 1 ].y = bbox.inf.y; vertex[ 1 ].z = bbox.inf.z;
        return bbox;
    }

    operator RwBBox()
    {
        RwBBox  bbox;
        bbox.sup.x = vertex[ 0 ].x; bbox.sup.y = vertex[ 0 ].y; bbox.sup.z = vertex[ 0 ].z;
        bbox.inf.x = vertex[ 1 ].x; bbox.inf.y = vertex[ 1 ].y; bbox.inf.z = vertex[ 1 ].z;
        return bbox;
    }

#endif
};


///////////////////////////////////////////////////////////////////////////////
// bounding sphere
class VfBSphere : public LG::Sphere<float>
{
public:
    VfBSphere( Pfloat cx = 0, Pfloat cy = 0, Pfloat cz = 0, Pfloat r = 0 )
    {
        operator() ( cx, cy, cz, r );
    }
    ~VfBSphere()
    {
    };
#ifdef _RWKERNEL_
    VfBSphere( RwSphere sphere )
    {
        center.x = sphere.center.x; center.y = sphere.center.y; center.z = sphere.center.z;
        radius = sphere.radius;
    };
    operator RwSphere()
    {
        return *( RwSphere * )this;
    }
#endif

    //operator CPSphere& () { return *this; }
};

#define DEXCPSphere( rs )    return (CPSphere*)&rs;



///////////////////////////////////////////////////////////////////////////////
// bounding sphere
class VfRect : public LG::Rect
{
public:
    VfRect()
    {
    };
    ~VfRect()
    {
    };

#ifdef _RWKERNEL_
    VfRect( RwRect rect )
    {
        left = rect.x; top = rect.y; right = rect.x + rect.w; bottom = rect.y +
                                                                       rect.h;
    }
    operator RwRect()
    {
        return *( RwRect * )this;
    }
    //RwV3d& operator= ( RwV3d vertex ){ x = vertex.x; y = vertex.y; z = vertex.z; }
#endif
};


class VfImage : public tImage
{
public:

    VfImage()
    {
        memset( this, 0, sizeof( tImage ) ) ;
    }
    VfImage( Puint w, Puint h, Puint d )
    {
        width = w; height = h; depth = d;
    }
    ~VfImage()
    {
    };

#ifdef _RWKERNEL_
    inline RwImage& operator=( RwImage& rwi )
    {
        format = rwi.flags;
        width = rwi.width ;
        height = rwi.height  ;
        depth = rwi.depth;
        pixels = rwi.cpPixels;
        palette = ( Pdword * ) rwi.palette;
        //RwImageAllocatePixels( &rwi );
        return rwi;
    };
#endif
};


/*
#ifdef _RWKERNEL_
inline RwImage& operator=( RwImage& rwi, tImage ti )
{
    rwi.flags    = ti.format;
    rwi.width    = ti.width;
    rwi.height   = ti.height;
    rwi.depth    = ti.depth;
    rwi.cpPixels = ti.pixels;
    rwi.palette  = (RwRGBA*)ti.palette;
    return rwi;
};
#endif
*/


class VfSpace3DVertex
{
public:
	LG::Vertex3f  pos;
    LG::Vertex3f  normal;         // (x, y, z) normal Vertex
    LG::Vertex2f  textureCoord;   // (u, v) texture coordinate
    LG::Vertex2f  lightmapCoord;  // (u, v) lightmap coordinate
    Pbyte         color[ 4 ];             // RGBA color for the vertex 
};



class VfPolygonIndex
{
public:
    Puint   matIndex;
    Puint   numVertices;
    Puint*  vertIndex;
};


class Vf3DGeometry
{
public:

    Puint           m_numVertices;    // Num vertices
    VfSpace3DVertex*m_vertices;       //
    Puint           m_numPolygons;    // Num polygons
    VfPolygonIndex* m_polygons;       // Polygons themselves

    Vf3DGeometry()
    {
        m_numVertices = 0;
        m_vertices = NULL;
        m_numPolygons = 0;
        m_polygons = NULL;
    }

    ~Vf3DGeometry()
    {
        if( m_vertices != NULL )
            delete [] m_vertices;
        if( m_polygons != NULL )
            delete [] m_polygons;
    }
};

///////////////////////////////////////////////////////////////////////////////
// engine graphic info

struct VfMetrics
{
    Puint   numTriangles;
    Puint   numProcTriangles;
    Puint   numVertices;
    Puint   numTextureUploads;
    Puint   sizeTextureUploads;
    Puint   numResourceAllocs;
    void*   devSpecificMetrics;
};



typedef    error_t VfError;


#define VFE_SUCCESS                         1
#define VFE_UNKNOW                          0
//texture
#define VFE_CREATEWORLDFAIL                -2
#define VFE_CREATEANIMFAIL                 -3
#define VFE_CREATEMODELFAIL                -4
#define VFE_TEXTURELOADFAIL                -5


#ifdef _RWKERNEL_
typedef RpHAnimAnimation VfAnimSequence;
//typedef RwImage                VfImage;
typedef RwImageCallBackRead VfImageCallBackRead;
typedef RwImageCallBackWrite VfImageCallBackWrite;

//light
#define Vf_NALIGHT                        rpNALIGHTTYPE
#define Vf_AMBIENT                        rpLIGHTAMBIENT
#define Vf_SPOT                           rpLIGHTSPOT
#define Vf_SPOT_SOFT                      rpLIGHTSPOTSOFT
#define Vf_DIRECTION                      rpLIGHTDIRECTIONAL




//#define Vf_SPECULAR
//#define Vf_POSITION                       rpLIGHTPOSITIONINGSTART
//#define Vf_SPOT_EXPONENT                  rpLIGHTSPOTSOFT
//#define Vf_SPOT_CUTOFF                    rpLIGHTSPOT
//#define Vf_CONSTANT_ATTENUATION           
//#define Vf_LINEAR_ATTENUATION             
//#define Vf_QUADRATIC_ATTENUATION          
//#define  VfLIGHNOTTYPE    rpLIGHTDIRECTIONAL
//#define  Vf_AMBIENT       rpLIGHTAMBIENT
//#define  VfLIGHNOTTYPE    rpLIGHTPOINT
//#define  VfLIGHNOTTYPE    rpLIGHTSPOT
//#define  VfLIGHNOTTYPE    rpLIGHTSPOTSOFT
//#define  VfLIGHNOTTYPE    rpLIGHTTYPEFORCEENUMSIZEINT

//clear
#define Vf_CLEAR_IMAGEBUFFER    rwCAMERACLEARIMAGE
#define Vf_CLEAR_ZBUFFER        rwCAMERACLEARZ

#endif




///////////////////////////////////////////////////////////////////////////////
//
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)

typedef class CPAnimSequence
{
public:

                                CPAnimSequence();
                                ~CPAnimSequence();
    std::vector<CPMatrixf>      m_seqlist;
}VfAnimSequence;


typedef tImage VfImage;
typedef VfImage*( *VfImageCallBackRead )( Pcstr imageName );
typedef VfImage*( *VfImageCallBackWrite )( VfImage* image, Pcstr imageName );


//light
#define Vf_NALIGHT                        0X0000
#define Vf_AMBIENT                        GL_AMBIENT
#define Vf_DIRECTION
#define Vf_SPOT                           GL_SPOT_DIRECTION
#define Vf_SPOT_SOFT                      GL_SPOT_EXPONENT


#define Vf_DIFFUSE                        GL_DIFFUSE
#define Vf_SPECULAR                       GL_SPECULAR
#define Vf_POSITION                       GL_POSITION
#define Vf_SPOT_DIRECTION                 GL_SPOT_DIRECTION
#define Vf_SPOT_CUTOFF                    GL_SPOT_CUTOFF
#define Vf_CONSTANT_ATTENUATION           GL_CONSTANT_ATTENUATION
#define Vf_LINEAR_ATTENUATION             GL_LINEAR_ATTENUATION
#define Vf_QUADRATIC_ATTENUATION          GL_QUADRATIC_ATTENUATION

//clear
#define Vf_CLEAR_IMAGEBUFFER              GL_COLOR_BUFFER_BIT
#define Vf_CLEAR_ZBUFFER                  GL_DEPTH_BUFFER_BIT


#endif

#endif// VFWGRAPHIC_H


