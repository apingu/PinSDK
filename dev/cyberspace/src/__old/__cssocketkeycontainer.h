#ifndef CSSOCKETKEYCONTINER_H
#define CSSOCKETKEYCONTINER_H

#include "IOSocket.h"
//#include "CstGameMember.h"


namespace Cyberspace
{
	template <class _Type>
	class CsSocketKeyContainer
	{
		IO::TCPSocket* socket;
		_Type          key;

	public:
		CsSocketKeyContainer( IO::TCPSocket* s ){ socket=s; key=(_Type)(s->GetKey()); }
		~CsSocketKeyContainer()
		{ 
			Release();
		}

		_Type Wait( uint_t count )
		{
			for( uint_t i=0; i< count; i++ )
			{
				if( key==NULL )
				{
					Sleep(20);
					key=(_Type)socket->GetKey();
					continue;
				}
				else
					return key;
			}
#ifdef _DEBUG
			OutputDebugString( "Wait for get key failure\n" );
#endif
			return key;
		}

		void Release( void )
		{
			if( socket==NULL )
				return;

			if( key!=NULL )
			{
			    socket->SetKey(key);
				key=NULL;
			}
			
			socket=NULL;
		}

		operator  _Type ()
		{
			//printf( "Get Member key %d\n", key );
			return key;
		}
	};

};


#endif