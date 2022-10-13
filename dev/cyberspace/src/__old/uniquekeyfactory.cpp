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

#include "UniqueKeyFactory.h"
#include <time.h>
#include <map>

std::map< ukey_t, char* > g_UniqueKeys;

ukey_t CreateUKey( void )
{
	ukey_t newkey = time( NULL );
	while( VerifyUKey(newkey) )
	{
		newkey = time( NULL );
	}
	g_UniqueKeys.insert( std::map< ukey_t, char* >::value_type( newkey, NULL ) );
	return newkey;
}


bool CancelUKey( ukey_t key )
{
	if( VerifyUKey( key ) )
	{
		g_UniqueKeys.erase( key );
		return true;
	}
	return false;
}


bool VerifyUKey( ukey_t key )
{
	if( key == 0 )
		return false;
	std::map< ukey_t, char* >::iterator iter = g_UniqueKeys.find( key );
	//find resource
	if( iter == g_UniqueKeys.end() )
	{
		return false;		
	}
	return true;
}

bool InsertUKey( ukey_t key )
{
	if( VerifyUKey( key ) )
	{
		return false;
	}
	g_UniqueKeys.insert( std::map< ukey_t, char* >::value_type( key, NULL ) );
	return true;
}