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
//                                               Copyright    (C)    2000    Pin
//
#ifndef ELEMENT3D_H
#define ELEMENT3D_H

#include "Figure3d.h"

class CP3DElement : public CP3DAction
{
	MEM::String  m_Propfname;
	UINT         m_Entry;

public:

	CP3DElement();
	~CP3DElement();
	
	CPModel* operator()(void){ return Model(); }

	char*      GetPropfname( void )        { return m_Propfname; }
	void       SetPropfname( char* fname ) { m_Propfname = fname; }
	UINT       GetPropentry(void)          { return m_Entry; }
	void       SetPropentry( UINT e )      { m_Entry = e; }
	bool       ReadProp( void* st )        { return false; }

};




#endif//ELEMENT_H
