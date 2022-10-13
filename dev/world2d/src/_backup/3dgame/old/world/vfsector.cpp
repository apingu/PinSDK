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
//                                               Copyright    (C)    2000    Pin
//
#include "VfSector.h"
#include "Texture.h"
#include <Debug.h>
#include <PFile.h>


#ifdef _RWKERNEL_
#include <RwFile.h>  //3DEngine
RpAtomic* ClumpGetAtomics(RpAtomic* atomic, void* data);
#endif

const Pdword gMyBsp = 0x50534250;//PBSP

//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
VfSector::VfSector()
{
	ID = 0;
	//m_Descriptor = NULL;

#ifdef _RWKERNEL_
	m_pWorld = NULL;
#else
	m_collisionsize = 0;
	//m_numVertices = 0;    /* Num vertices */
	//m_Vertices = NULL;
#endif
}


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
VfSector::~VfSector()
{
	Release();
}


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
VfError VfSector::Create(VfBBox bBox)
{
	VfError result = VFE_SUCCESS;
	m_fname.clear();

#ifdef _RWKERNEL_
	// Destory world
	if (m_pWorld != NULL)
		RpWorldDestroy(m_pWorld);
	m_pWorld = NULL;

	if ( ( m_pWorld = RpWorldCreate( &(RwBBox)bBox ) ) == NULL)
		return VFE_CREATEWORLDFAIL;
#endif
	m_BBox = bBox;

	return result;
}


//=============================================================================
//  Name : Constructure
//  Desc :
//=============================================================================
#ifdef _RWKERNEL_
RpWorldSector* SectorGetPolygon(RpWorldSector* worldSector, void* data)
{
	std::vector<VfVertex3f>* vlist = (std::vector<VfVertex3f>*) data;
	for (int i = 0; i < RpWorldSectorGetNumPolygons(worldSector); i++)
		vlist->push_back(worldSector->vertices[i]);
	return worldSector;
}
#endif


VfError VfSector::Create(Pcstr fname)
{
	if( fname==NULL )
		return 0;

	VfError result = VFE_SUCCESS;
	m_fname = fname;

	CPTexture texture;
	m_texfname = texture.folder();


#ifdef _RWKERNEL_
	// Destory world
	if (m_pWorld != NULL)
		RpWorldDestroy(m_pWorld);
	m_pWorld = NULL;

	RwFileFunctions* fileFunctions;
	RwFileFunctions RWFileInterface;

	fileFunctions = RwOsGetFileInterface();
	RWFileInterface = *fileFunctions;

	fileFunctions->rwfopen = RWFile::_PakFOpen;
	fileFunctions->rwfclose = RWFile::_PakFClose;
	fileFunctions->rwfread = RWFile::_PakFRead;
	fileFunctions->rwfwrite = RWFile::_PakFWrite;
	fileFunctions->rwfgets = RWFile::_PakFGets;
	fileFunctions->rwfputs = RWFile::_PakFPuts;
	fileFunctions->rwfeof = RWFile::_PakFEof;
	fileFunctions->rwfseek = RWFile::_PakFSeek;
	fileFunctions->rwfflush = RWFile::_PakFFlush;
	fileFunctions->rwftell = RWFile::_PakFTell;


	for (Puint i = 0; i < 3; i++)
	{
		RwStream* stream;

		stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, m_fname);

		if (stream)
		{
			if (RwStreamFindChunk(stream, rwID_WORLD, NULL, NULL))
			{
				if ((m_pWorld = RpWorldStreamRead(stream)) != NULL)
				{
					RwStreamClose(stream, NULL);
					break;
				}
				//Create( szPathname );
			}
			//RwStreamClose(stream, NULL);
		}
	}

	*fileFunctions = RWFileInterface;


	if (m_pWorld != NULL)
	{
		RwBBox* box = RpWorldGetBBox(m_pWorld);
		m_BBox = (*box);

		//get sector data
		//std::vector<VfVertex3f> m_vlist;
		//RpWorldForAllWorldSectors(m_pWorld, SectorGetPolygon, &m_vlist);
	}
	else
	{
		DebugMessage(" Load .sbp file failed", "VFRW");
		return result = VFE_UNKNOW;
	}

	RwError err;
	RwErrorGet(&err);

	switch (err.errorCode)
	{
	case 22:
		{
			DebugMessage("failed to load world textures", "VFRW");
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
#endif//_RWKERNEL_

#ifdef _VFWKERNEL_

	CPFile f;
	f.open(fname, "rb");
	Pdword title;
	f.read(&title, sizeof(Pdword), 1);
	if (title == gMyBsp)
	{
		f.read(&(m_geometry.m_numVertices), sizeof(Puint), 1);
		m_geometry.m_vertices = new VfSpace3DVertex[m_geometry.m_numVertices];
		for (Puint v = 0; v < m_geometry.m_numVertices; v++)
			f.read(&(m_geometry.m_vertices[v]), sizeof(VfSpace3DVertex), 1);

		f.read(&(m_geometry.m_numPolygons), sizeof(Puint), 1);
		m_geometry.m_polygons = new VfPolygonIndex[m_geometry.m_numPolygons];
		for (Puint p = 0; p < m_geometry.m_numPolygons; p++)
			f.read(&(m_geometry.m_polygons[v]), sizeof(VfPolygonIndex), 1);
	}

	#endif  //_VFWKERNEL_
	//return 0;

	return result;//( m_pWorld != NULL );
}


//=============================================================================
//  Name :
//  Desc :
//=============================================================================
#ifdef _RWKERNEL_
RpLight* SectorReleaseLightCallBack(RpLight* light, void* data)
{
	RpWorld* sector = (RpWorld*) data;
	//RpWorldRemoveLight (sector, light);
	RpLightDestroy(light);
	return light;
}
#endif

void VfSector::Release(void)
{
#ifdef _RWKERNEL_
	if (m_pWorld != NULL)
		RpWorldDestroy(m_pWorld);
	m_pWorld = NULL;
#endif //_RWKERNEL_
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfBBox VfSector::GetBBox(void)
{
	return m_BBox;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
Pcstr VfSector::filename(void)
{
	return m_fname;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
VfVertex3f VfSector::Origin(void)
{
#ifdef _RWKERNEL_
	//return *(VfVertex3f*)RpWorldGetOrigin( m_pWorld );
	return VfVertex3f();
#endif
}



//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpWorldSector* SectorSaveVertex(RpWorldSector* sector, void* data)
{
	CPFile* file = (CPFile*) data;
	Puint vnum = sector->numVertices;
	Puint pnum = sector->numPolygons;

	file->write(&gMyBsp, sizeof(char) * 4, 1);

	file->write(&vnum, sizeof(Puint), 1);
	for (Puint v = 0; v < vnum; v++)
	{
		VfSpace3DVertex vertex;

		vertex.pos.x = sector->vertices[v].x;
		vertex.pos.y = sector->vertices[v].y;
		vertex.pos.z = sector->vertices[v].z;

		vertex.normal.x = sector->normals[v].x;
		vertex.normal.y = sector->normals[v].y;
		vertex.normal.z = sector->normals[v].z;

		file->write(&vertex, sizeof(VfSpace3DVertex), 1); 
		//sector->texCoords[i]
	}

	file->write(&pnum, sizeof(Puint), 1);
	for (Puint p = 0; p < pnum; p++)
	{
		Puint Polygon[5];
		Polygon[0] = sector->polygons[p].matIndex;
		Polygon[1] = 3;
		Polygon[2] = sector->polygons[p].vertIndex[0];
		Polygon[3] = sector->polygons[p].vertIndex[1];
		Polygon[4] = sector->polygons[p].vertIndex[2];
		file->write(&Polygon, sizeof(Puint) * 5, 1);
	}
	return sector;
}
#endif  //_RWKERNEL_
VfError VfSector::Save(Pcstr fname)
{
	CPFile file;
	file.open(fname, "wb");

#ifdef _RWKERNEL_
	RpWorldForAllWorldSectors(m_pWorld, SectorSaveVertex, &file);
#endif

	return VFE_SUCCESS;
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpWorldSector* PolygonsOfSector(RpWorldSector* sector, void* data)
{
	Puint* count = (Puint*) data;
	(*count) += sector->numPolygons;
	return sector;
}
#endif//_RWKERNEL_
Puint VfSector::GetPolygonsCount(void)
{
#ifdef _RWKERNEL_
	if (m_pWorld == NULL)
		return 0;
	Puint count = 0;
	RpWorldForAllWorldSectors(m_pWorld, PolygonsOfSector, &count);
	return count;
#endif//_RWKERNEL_
}


//==========================================================================================
//  Name :
//  Desc :
//==========================================================================================
#ifdef _RWKERNEL_
RpWorldSector* VerticesOfSector(RpWorldSector* sector, void* data)
{
	Puint* count = (Puint*) data;
	(*count) += sector->numVertices;
	return sector;
}
#endif//_RWKERNEL_
Puint VfSector::GetVerticesCount(void)
{
#ifdef _RWKERNEL_
	if (m_pWorld == NULL)
		return 0;
	Puint count = 0;
	RpWorldForAllWorldSectors(m_pWorld, VerticesOfSector, &count);
	return count;
#endif//_RWKERNEL_
}






