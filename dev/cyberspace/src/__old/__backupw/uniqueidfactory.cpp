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
#pragma warning (disable : 4786)

#include "UniqueIdFactory.h"
#include <time.h>
#include <map>
#ifdef _WIN32
#include <windows.h>
#endif

std::map< UniqueId_t, char* > g_UniqueKeys;

UniqueId_t CreateUKey( void )
{
	UniqueId_t newkey = time( NULL );
	while( VerifyUKey(newkey) )
	{
#ifdef _WIN32
		newkey = GetTickCount();
#endif
	}
	g_UniqueKeys.insert( std::map< UniqueId_t, char* >::value_type( newkey, NULL ) );
	return newkey;
}


bool CancelUKey( UniqueId_t key )
{
	if( VerifyUKey( key ) )
	{
		g_UniqueKeys.erase( key );
		return true;
	}
	return false;
}


bool VerifyUKey( UniqueId_t key )
{
	if( key == 0 )
		return false;
	std::map< UniqueId_t, char* >::iterator iter = g_UniqueKeys.find( key );
	//find resource
	if( iter == g_UniqueKeys.end() )
	{
		return false;		
	}
	return true;
}

bool InsertUKey( UniqueId_t key )
{
	if( VerifyUKey( key ) )
	{
		return false;
	}
	g_UniqueKeys.insert( std::map< UniqueId_t, char* >::value_type( key, NULL ) );
	return true;
}