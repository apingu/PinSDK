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
#ifndef CSFILEMANGER_H
#define CSFILEMANGER_H

#include <direct.h>

namespace Cyberspace
{

	class CsFileManager
	{
	public:
		CsFileManager();
        ~CsFileManager();
	};


	////////////////////////////////////////////////////////////////////////////
	//


	CsFileManager::CsFileManager()
	{
		mkdir(".\\HOME" );
	}

	CsFileManager::~CsFileManager()
	{

	}
};











#endif