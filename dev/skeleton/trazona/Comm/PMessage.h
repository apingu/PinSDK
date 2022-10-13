//
//
//
//
//
//
//
//
//
//    message class for network
//
//
//
//
//
//
#ifndef PMESSAGE_H
#define PMESSAGE_H

//#include "pdef.h"

typedef unsigned int  Puint;
typedef unsigned char Pbyte;


class CPMessage
{

public:

	Puint  size;
	Pbyte* buf;

	CPMessage(){ size = 0; buf = NULL; }
	~CPMessage(){ Release(); }



	void put( Pbyte* msg, Puint Size )
	{
	    size = Size;
		buf  = msg;
	}

	Puint get( Pbyte* msg )
	{
		msg = buf;
	    return size;
	}

	void New( Pbyte* msg, Puint Size )
	{
		size = Size;
	    buf = new Pbyte[size];
	    memcpy( buf, msg, size );
	}

	void Release( void )
	{ 
		size = 0; 
		if( buf!=NULL )
		    delete [] buf;
		buf = NULL;
	}
};

#endif