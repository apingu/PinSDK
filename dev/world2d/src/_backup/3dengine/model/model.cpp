//Model.cpp
//
//  3D game model data
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
//                                               Copyright    (C)    2001    Pin
//
#include "Model.h"
#include <Debug.h>


#ifdef _RWKERNEL_
#include <RwFile.h>
RpAtomic* ClumpGetAtomics( RpAtomic* atomic, void* data );
#endif


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
CPModel::CPModel()
{
#ifdef _RWKERNEL_
    m_pClump = NULL;
#endif
}

//=============================================================================
//  Name : Distructure
//  Desc :
//=============================================================================
CPModel::~CPModel()
{
    Destroy();
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
Pstr CPModel::filename( void )
{
    return ( Pstr ) m_fname;
}

Pstr CPModel::texfname( void )
{
    return ( Pstr ) m_texfname;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
int CPModel::empty( void )
{
#ifdef _RWKERNEL_
    if( ( !m_fname.empty() ) || ( m_pClump != NULL ) )
        return 0;
#endif
    return 1;
}

//=============================================================================
//  Name : Destory
//  Desc :
//=============================================================================
void CPModel::Destroy( void )
{
    if( empty() )
        return;
    //memset( m_modelPath, NULL, sizeof( m_modelPath ) );
    //memset( m_texFolder, NULL, sizeof( m_texFolder ) );
    m_fname.clear();
	m_texfname.clear();

#ifdef _RWKERNEL_
    if( m_pClump != NULL )
    {
        RpClumpDestroy( m_pClump );
        m_pClump = NULL;
    }
#endif
    for( int i = 0;i < ( int ) m_object.size();i++ )
        delete m_object[ i ];

    m_object.clear();
}

//=============================================================================
//  Name : Load
//  Desc :
//=============================================================================
VfError CPModel::Load( Pcstr path )
{

    VfError result  = VFE_SUCCESS;
    Destroy();
    m_fname = path; 

	CPTexture texture;
	m_texfname = texture.folder();

#ifdef _RWKERNEL_

    RwFileFunctions* fileFunctions;
    RwFileFunctions RWFileInterface;

    fileFunctions = RwOsGetFileInterface();
    RWFileInterface = *fileFunctions;

    fileFunctions->rwfopen = RWFile::_PakFOpen;
    fileFunctions->rwfclose = RWFile::_PakFClose;
    fileFunctions->rwfread = RWFile::_PakFRead;
    fileFunctions->rwfgets = RWFile::_PakFGets;
    fileFunctions->rwfeof = RWFile::_PakFEof;
    fileFunctions->rwfseek = RWFile::_PakFSeek;

    //path = RsPathnameCreate(RWSTRING("./models/clump.dff"));
#ifdef SKY
    RpSkyTextureSetDefaultMipmapK( -1.0f );
#endif
	RpClump* clump=NULL;

    //RwImageSetPath("./robot/textures/");
    RwStream* stream= RwStreamOpen( rwSTREAMFILENAME,
                                    rwSTREAMREAD,
                                    ( Pstr ) m_fname );
    //TexDictionaryGetName(TexDictionaryName, path);
    //RwImageSetPath(texpath);
    //RsPathnameDestroy(path);

    if( stream )
    {
        if( RwStreamFindChunk( stream, rwID_CLUMP, NULL, NULL ) )
        {
            if( ( clump = RpClumpStreamRead( stream ) ) == 0 )
                return -VFE_CREATEMODELFAIL;
        }
        RwStreamClose( stream, NULL );
    }

    *fileFunctions = RWFileInterface;

    if( clump != NULL )
    {
		operator=( clump );
    }else
    {
        DebugMessage( " Load .dff file failed", "VFRW" );
        return result = VFE_UNKNOW;
    }


    RwError err;
    RwErrorGet( &err );

    switch( err.errorCode )
    {
    case 22:
    {
        DebugMessage( "failed to load model textures", "VFRW" );
        result = VFE_TEXTURELOADFAIL;
        break;
    }
    default:
    {
        //DebugMessage( " Unknow failed", "VFRW" );
        //result = VFE_UNKNOW;
        result = VFE_SUCCESS;
        break;
    }
    }

    //return ( m_pClump != NULL );
#endif
    UpdateBoundBox();
    UpdateBoundSphere();
    return result;
}


//=============================================================================
//  Name : LoadAtomic
//  Desc :
//=============================================================================
/*
RpAtomic* CPModel::LoadAtomic(const char *path, RwTexDictionary *texDict)
{
  RpAtomic *atomic = NULL;
  RpClump *clump;

  if(clump = LoadClump(path, texDict))
  {
    RpClumpForAllAtomics(clump, CloneAtomic, (void*)&atomic);
    RpClumpDestroy(clump);
  }

  return atomic;
}
*/

//=============================================================================
//  Name : GetObjectCount
//  Desc :
//=============================================================================
int CPModel::GetObjectCount( void )
{
    //int numAtomics = 0;
    //RpClumpForAllAtomics( m_pClump, CountAtomic, (void*)&numAtomics );
    return m_object.size();
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CPModel::UpdateBoundBox( void )
{
    for( int i = 0;i < GetObjectCount();i++ )
    {
		VfBBox objBBox;
        m_object[ i ]->LocalBoundBox( &objBBox );
        if( m_BBox.vertex[ 0 ].x <= objBBox.vertex[ 0 ].x )
            m_BBox.vertex[ 0 ].x = objBBox.vertex[ 0 ].x;
        if( m_BBox.vertex[ 0 ].y <= objBBox.vertex[ 0 ].y )
            m_BBox.vertex[ 0 ].y = objBBox.vertex[ 0 ].y;
        if( m_BBox.vertex[ 0 ].z <= objBBox.vertex[ 0 ].z )
            m_BBox.vertex[ 0 ].z = objBBox.vertex[ 0 ].z;

        if( m_BBox.vertex[ 1 ].x > objBBox.vertex[ 1 ].x )
            m_BBox.vertex[ 1 ].x = objBBox.vertex[ 1 ].x;
        if( m_BBox.vertex[ 1 ].y > objBBox.vertex[ 1 ].y )
            m_BBox.vertex[ 1 ].y = objBBox.vertex[ 1 ].y;
        if( m_BBox.vertex[ 1 ].z > objBBox.vertex[ 1 ].z )
            m_BBox.vertex[ 1 ].z = objBBox.vertex[ 1 ].z;

        //if( !m_object.empty() )
        //return m_object[0]->LocalBoundBox( m_BBox );
    }

}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
void CPModel::UpdateBoundSphere( void )
{
    VfVertex3f center, temp;

    if( m_object.empty() )
        return;

    // first time
    VfBSphere objsphere;
    m_object[ 0 ]->LocalBoundSphere( &objsphere );

    m_BSphere.center = objsphere.center;
    m_BSphere.radius = objsphere.radius;

    //VfVertex3f* orgver = Position();

    for( int i = 1;i < GetObjectCount();i++ )
    {
        m_object[ i ]->LocalBoundSphere( &objsphere );
        //objsphere.center = objsphere.center + m_object[i]->Position();
        //VfVertex3f vpos = Position();
        //objsphere.center = objsphere.center + vpos;

        //objsphere.center = objsphere.center + m_BSphere->center;
        temp = objsphere.center - m_BSphere.center;
        Pfloat separation   = ( Pfloat ) temp.length();

        if( separation + objsphere.radius < m_BSphere.radius )
            continue;

        // Calculate the clump's new bounding-sphere radius...
        Pfloat radius   = ( separation + objsphere.radius + m_BSphere.radius ) * 0.5f;

        //Calculate the clump's new bounding-sphere center...
        temp = temp * 1.0f / separation;
        //RwV3dScale(&temp, &temp, 1.0f/separation);
        temp = temp * objsphere.radius - m_BSphere.radius;
        center = m_BSphere.center + objsphere.center;
        center = center + temp;
        center = center * 0.5f;

        m_BSphere.center = center;
        m_BSphere.radius = radius;
        //return 
    }
    return;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
LG::BBoxf CPModel::GetBBox( void )
{
	LG::BBoxf box;
	box.vertex[0] = m_BBox.vertex[0] + *Position();
	box.vertex[1] = m_BBox.vertex[1] + *Position();
	return box;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
LG::Spheref CPModel::GetBSphere( void )
{
	LG::Spheref sphere;
	sphere.center = m_BSphere.center + *Position();
	sphere.radius = m_BSphere.radius;
	return sphere;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
LG::BBoxf CPModel::GetLocalBox( void )
{
	return m_BBox;
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
LG::Spheref CPModel::GetLocalSphere( void )
{
	return m_BSphere;
}






#ifdef _RWKERNEL_
/////////////////////////////////////////////////////////////////
// Renderware function
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

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
CPModel::CPModel( RpClump* model )
{
	operator=( model );


}

///////////////////////////////////////////////////////////////////////////////
//
RpAtomic* ClumpGetAtomics( RpAtomic* atomic, void* data )
{
    CPObject* obj   = new CPObject( atomic );
    //obj->m_pAtomic = atomic;
    //obj->m_pMatrix = RpAtomicGetFrame( atomic );

    std::vector<CPObject*>* object  = ( std::vector<CPObject*>* ) data;
    object->push_back( obj );
    return atomic;
}

//=============================================================================
//  Name :
//  Desc :  set clump and object class
//=============================================================================
RpClump& CPModel::operator=( RpClump* model )
{
    if( model == NULL )
        return *model;
    if( m_pClump != NULL )
        Destroy();
    m_pClump = model;
    m_pMatrix = RpClumpGetFrame( m_pClump );
    RpClumpForAllAtomics( m_pClump, ClumpGetAtomics, &m_object );
    return *model;
}

#endif

