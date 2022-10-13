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
#ifndef STREAMDEF_H
#define STREAMDEF_H

#include <PfDef.h>
//#include <tString.h>
#include <string>

typedef int cellid_t;
#define Invalid_CellId 0XFFFFFFFF

namespace GSO
{

	//typedef std::string StreamCellDescriptor;
	typedef void*       StreamCellDescriptor;

	class StreamCell 
	{
	public:
		StreamCell(cellid_t id)
		{
			Id=id;
			pData=NULL;
			pUserData=NULL;
		}
		virtual ~StreamCell()
		{
			//delete m_pDescriptor;
		}
		cellid_t       Id;
		StreamCellDescriptor Descriptor;
		void*          pData;
		void*          pUserData;
		//UINT           X;
		//UINT           Y;
		//UINT           Z;
	};
};
#endif//STREAMDEF_H 
