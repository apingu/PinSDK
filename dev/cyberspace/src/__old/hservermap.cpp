//////////////////////////////////////////////////////////
// File Name: hservermap.cpp
//
// Author : Little Chio
// File created: 2003/5/20
// Last Modified : 2003/5/21
//
// Copyright (c) 2003, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#include "hservermap.h"

//=======================================================
// class HServerRegion
//=======================================================
// Constructor
HServerRegion::HServerRegion():
m_pOwner(NULL),
m_Column(0),
m_Row(0)
{
}

// Constructor, init by owner map, col and row number
HServerRegion::HServerRegion(HServerMap *pOwner, int col, int row):
m_pOwner(pOwner),
m_Column(col),
m_Row(row)
{
}

// Get the origin position of region.
Vector HServerRegion::GetOriginPos() const
{
	return m_pOwner!=NULL ? Vector(m_Column, 0, m_Row)*m_pOwner->m_RegionSize
			+ m_pOwner->m_OriginPos : Vector(0,0,0);
}

// Get the size of region. i.e. The region is square.
float HServerRegion::GetSize() const
{
	return m_pOwner!=NULL ? m_pOwner->m_RegionSize : 0;
}

// Get the region's location in the map's grid.
bool HServerRegion::GetColumnRow(int &col, int &row) const
{
	if(m_pOwner)
	{
		col = m_Column;
		row = m_Row;
		return true;
	}
	else return false;
}

// Enumerate the neighbor regions by index.
HServerRegion *HServerRegion::EnumNeighborRegion(int index) const
{
	if(index>=0 && index<(int)m_Neighbors.size())
		return m_Neighbors[index];
	else return NULL;
}

// Serialization via XMLNode
bool HServerRegion::Serialize(XMLNode &xml, bool is_storing, bool recursive)
{
	if(is_storing)
	{
		xml[_T("Column")].m_Text << m_Column;
		xml[_T("Row")].m_Text << m_Row;
	}
	else
	{
		xml[_T("Column")].m_Text >> m_Column;
		xml[_T("Row")].m_Text >> m_Row;
	}

	return true;
}

//=======================================================
// class HServerMap
//=======================================================
// Constructor.
HServerMap::HServerMap(int mapid):
m_MapId(mapid),
m_ColCount(0),
m_RowCount(0),
m_RegionSize(0),
m_OriginPos(0,0,0)
{
}

// Destructor.
HServerMap::~HServerMap()
{
	Destroy();
}

// Create the map. It creates the regions in form of grid.
void HServerMap::Create(int ncols, int nrows, float region_size, const Vector &offset)
{
	Destroy();

	// allocate map's grid
	m_RegionList.resize(ncols*nrows);
	
	// setup regions
	for(int i=0; i< nrows; i++)
	{
		for(int j=0; j< ncols; j++)
		{
			HServerRegion *region = CreateRegion();
			region->m_pOwner = this;
			region->m_Column = j;
			region->m_Row = i;
			m_RegionList[i*ncols+j].Attach(region, false);
		}
	}

	m_ColCount = ncols;
	m_RowCount = nrows;
	m_RegionSize = region_size;
	m_OriginPos = offset;

	BuildRegionNeighbors(); // rebuild neighbors
}

// Build region's neighbors
void HServerMap::BuildRegionNeighbors()
{
	static const int offset[8][2] = 
	{
		{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}
	};

	for(int i=0; i< m_RowCount; i++)
	{
		for(int j=0; j< m_ColCount; j++)
		{
			HServerRegion *region = m_RegionList[i*m_ColCount+j];

			if(region!=NULL)	// check valid region
			{
				region->m_Neighbors.clear();

				for(int k=0; k<8; k++)
				{
					int col = j+offset[k][0];
					int row = i+offset[k][1];

					// check valid range
					if(col>=0 && col<m_ColCount && row>=0 && row<m_RowCount)
					{
						HServerRegion *neighbor = m_RegionList[row*m_ColCount+col];
						if(neighbor) region->m_Neighbors.push_back(neighbor);
					}
				}
			}
		}
	}
}

// Destroy all the regions it contains.
void HServerMap::Destroy()
{
	m_RegionList.clear();
	m_ColCount = 0;
	m_RowCount = 0;
	m_RegionSize = 0;
	m_OriginPos = Vector(0,0,0);
}

// Set the origin position of map.
void HServerMap::SetOriginPos(const Vector &pos)
{
	m_OriginPos = pos;
}

// Get region where position is located in.
HServerRegion *HServerMap::GetRegionByPos(const Vector &pos) const
{
	if(m_RegionSize>0)
	{
	   float col = (pos.x - m_OriginPos.x) / m_RegionSize;
	   float row = (pos.z - m_OriginPos.z) / m_RegionSize;

	   if(col>=0 && col < (float)m_ColCount && row>=0 && row < (float)m_RowCount)
	   {
		   return m_RegionList[(int)col + ((int)row)*m_ColCount];
	   }
	}

    return NULL;
}

// Custom procedure of creating a region object.
HServerRegion *HServerMap::CreateRegion() const
{
	return new HServerRegion();
}

// Dispatch the messages : to all regions
void HServerMap::DispatchMessages(Packet &p)
{
	RegionList::iterator iter = m_RegionList.begin();
	while(iter!=m_RegionList.end())
	{
		if(*iter) (*iter)->DispatchMessages(p);
		iter++;
	}
}

// Serialization via XMLNode
bool HServerMap::Serialize(XMLNode &xml, bool is_storing, bool recursive)
{
	if(is_storing)
	{
		xml[_T("Columns")].m_Text << m_ColCount;
		xml[_T("Rows")].m_Text << m_RowCount;
		xml[_T("RegionSize")].m_Text << m_RegionSize;
		xml[_T("OriginPos")].m_Text << m_OriginPos;
		
		XMLNode &node = xml[_T("Regions")];
		for(unsigned int i=0; i< m_RegionList.size(); i++)
		{
			XMLNode region_node;
			if(m_RegionList[i]->Serialize(region_node, true))
			{
				region_node.m_Text = _T("Region");
				node.m_ChildNodes.push_back(region_node);
			}
		}
	}
	else
	{
		Destroy();

		// init with default values
		int ncols = 0, nrows = 0;
		float region_size = 0;
		Vector origin(0,0,0);

		xml[_T("Columns")].m_Text >> ncols;
		xml[_T("Rows")].m_Text >> nrows;
		xml[_T("RegionSize")].m_Text >> region_size;
		xml[_T("OriginPos")].m_Text >> origin;

		// check these settings and load regions
		if(ncols > 0 && nrows > 0 && region_size>0)
		{
			// allocate map's grid
			m_RegionList.resize(ncols*nrows);

			XMLNode &node = xml[_T("Regions")];

			for(unsigned int i=0; i< node.m_ChildNodes.size(); i++)
			{
				XMLNode &region_node = node.m_ChildNodes[i];

				if(region_node.m_Text==_T("Region"))
				{
					HServerRegion *region = CreateRegion();
					region->Serialize(region_node, false);
					
					// check if the region has valid column and row number
					if(region->m_Column>=0 && region->m_Column<ncols &&
					   region->m_Row>=0 && region->m_Row<nrows)
					{
						region->m_pOwner = this;
						m_RegionList[region->m_Row*ncols+region->m_Column].Attach(region, false);
					}
					else region->Release(); // invalid region, release it
				}
			}

			m_ColCount = ncols;
			m_RowCount = nrows;
			m_RegionSize = region_size;
			m_OriginPos = origin;

			BuildRegionNeighbors(); // rebuild neighbors
		}
		else return false;
	}

	return true;
}

// Get the dimension of map.
void HServerMap::GetDimension(int &ncols, int &nrows, float &region_size) const
{
	ncols = m_ColCount;
	nrows = m_RowCount;
	region_size = m_RegionSize;
}
