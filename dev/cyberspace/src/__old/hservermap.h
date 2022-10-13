//////////////////////////////////////////////////////////
// File Name: hservermap.h
//
// Author : Little Chio
// File created: 2003/5/20
// Last Modified : 2003/5/20
//
// Copyright (c) 2003, Gamania Inc. All Rights Reserved.
//////////////////////////////////////////////////////////

#ifndef __HSERVERMAP_H__
#define __HSERVERMAP_H__

#include "hservergroup.h"
#include "hserverentity.h"

class HServerMap;   // forward declaration

/**
* class HServerRegion
* This class represent a region in space, and manages the entities located in
* the region. The broadcasting messages in the region are also called 'region
* messages'.
*/
class HServerRegion : public HServerGroup
{
public:

	/** Constrcutor */
	HServerRegion();

	/** Constructor, init by owner map, col and row number. */
	HServerRegion(HServerMap *pOwner, int col, int row);

	/**
	* Get the origin position of region. Origin position means the origin point,
	* the minimun corner of region, in world space.
	*/
	Vector GetOriginPos() const;

	/** Get the size of region. i.e. The region is square. */
	float GetSize() const;

	/*
	 * Get the region's location in the map's grid.
	 * @param col Reference to retrieve the column number.
	 * @param row Reference to retrieve the row number.
	 * @return False if no owner map is set yet.
	 */
	bool GetColumnRow(int &col, int &row) const;

	/** Get the owner map. */
	HServerMap *GetOwner() const { return m_pOwner; }

	/** Region is not for DSM, so return NULL.  */
	DSM_Chunk *Clone() const { return NULL; }

	/** Class name of region. */
	const char *ClassName() const { return "Region"; }

    /** 
     * Serialization. Load/save data from XMLNode. 
     * @param xml The XMLNode which contains the stream of data.
     * @param is_storing  True for saving to XMLNode, false for loading from XMLNode.
     * @return Return true if success.
     */
	bool Serialize(XMLNode &xml, bool is_storing, bool recursive=false);

	/** 
	 * Enumerate the neighbor regions by index. Neighbor region means the region
	 * adjacent to this region, so they can receive the broadcasting messages from
	 * this region.
	 */
	HServerRegion *EnumNeighborRegion(int index) const;

protected:

	HServerMap *m_pOwner;	/**< The owner map. */
	int m_Column;			/**< Column number in the map. */
	int m_Row;				/**< Row number in the map. */
	std::vector<HServerRegion *> m_Neighbors;	/**< Neighboring regions. */

	friend class HServerMap;
};

// smart pointer for HServerRegion
typedef ref_counted_ptr<HServerRegion> HServerRegionPtr;

/**
* class HServerMap
* This class manages multiple region objects in grid-style since the grid algorithm
* could locate an entity to a region very fast and cheap.
*/
class HServerMap : public HServerObject
{
public:

	/** Constructor. */
	HServerMap(int mapid);

	/** 
	 * Create the map. It creates the regions in form of grid.
	 * @param ncols,nrows Dimension of the grid. Column is X direction, and row is Z direction.
	 * @param region_size Size for each region.
	 * @param origin The map's origin position. It represents the minimun position of regions.
	 */
	virtual void Create(int ncols, int nrows, float region_size, const Vector &origin);

	/** Destroy all the regions it contains. */
	virtual void Destroy();

	/** Set the origin position of map. */
	void SetOriginPos(const Vector &pos);

	/** Get region where position is located in. */
	HServerRegion *GetRegionByPos(const Vector &pos) const;

	/** Map is not for DSM, so return NULL.  */
	DSM_Chunk *Clone() const { return NULL; }

	/** Class name of map. */
	const char *ClassName() const { return "Map"; }
	
	/**
	 * Dispatch messages from packet. Messages are supposed to be parsed here 
	 * and executed according to the message's content.
	 * @param p The packet storing the messages.
	 */
	void DispatchMessages(Packet &p);

    /** 
     * Serialization. Load/save data from XMLNode. 
     * @param xml The XMLNode which contains the stream of data.
     * @param is_storing  True for saving to XMLNode, false for loading from XMLNode.
     * @return Return true if success.
     */
	bool Serialize(XMLNode &xml, bool is_storing, bool recursive=false);

	/** Get the map's id. */
	int MapId() const { return m_MapId; }

	/** 
	 * Get the dimension of map.
	 * @param ncols Reference to retrieve the number of columns.
	 * @param nrows Reference to retrieve the number of rows.
	 * @param region_size Reference to retrieve the size of each region.
	 */
	void GetDimension(int &ncols, int &nrows, float &region_size) const;

	/** Get origin position of map. */
	Vector GetOriginPos() const { return m_OriginPos; }

protected:

	/** Destructor. */
	~HServerMap();

	/** Rebuild region's neighbors. It's called whenever the map is changed. */
	virtual void BuildRegionNeighbors();

	/** Custom procedure for creating a region object. */
	virtual HServerRegion *CreateRegion() const;

	int m_MapId;			  /**< Map id. */
	int m_ColCount;			  /**< Number of columns in grid. */
	int m_RowCount;		      /**< Number of rows in grid. */
	float m_RegionSize;	      /**< Region size. */
	Vector m_OriginPos;		  /**< Origin position of the map. */

	typedef std::vector<HServerRegionPtr> RegionList;
	RegionList m_RegionList;  /**< Regions in the map. */

	friend class HServerRegion;
};

// smart pointer for HServerMap
typedef ref_counted_ptr<HServerMap> HServerMapPtr;


#endif
