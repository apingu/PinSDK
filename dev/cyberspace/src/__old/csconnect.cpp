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
#include "CsConnect.h"

namespace CsCore
{
    int g_Identification_Number = 0;
};

namespace CsFactory
{
	void MakeCommand( CsCommandMessage* out, int CommandId, const char* fmt, ... )
	{
		out->Header.CommandId=CommandId;

		char* obuf = out->Buff;
        char* ptr = (char*)fmt;

        va_list argvlist;
        va_start( argvlist, fmt );     /* Initialize variable arguments. */
        //while( i != 0 )
		while( (*ptr)!=0 )
		{
			if( (*ptr) == '%' )
			{
				ptr++;
				switch((*ptr))
				{
				case 'c':
					(*(char*)obuf) = (char)va_arg( argvlist, char);
					obuf+=sizeof(char);
					break;
				case 'd':
					(*(int*)obuf) = (int)va_arg( argvlist, int);
					obuf+=sizeof(int);
					break;
		        case 'l':
					(*(long*)obuf) = (long)va_arg( argvlist, long);
					obuf+=sizeof(long);
					break;
				case 'h':
					(*(short*)obuf) = (short)va_arg( argvlist, short);
					obuf+=sizeof(short);
					break;
				case 's':
					{
						char* str = (char*)va_arg( argvlist, char*);
						unsigned short len = strlen(str);
					    (*(unsigned short*)obuf) = len;
						obuf+=sizeof( unsigned short );
						strcpy( obuf, str );
						obuf+=len+1;
					}
					break;
				case 'v':
					{
						void* bin = va_arg( argvlist, void* );
						unsigned short binlen = va_arg( argvlist, unsigned short );
						(*(unsigned short*)obuf) = binlen;
						obuf+=sizeof( unsigned short );
						memcpy( obuf, bin, binlen );
						obuf+=binlen;
					}
					break;
				}
            //sum += 1;
            //count++;
           	}
			ptr++;
		}
        va_end( argvlist );              /* Reset variable arguments.      */
		out->Header.CommandLen = (obuf)-(out->Buff);
	}

    void SplitCommand( CsCommandMessage* out, int CommandId, const char* fmt, ... )
	{
		if( CommandId != out->Header.CommandId )
		{
			assert( CommandId != out->Header.CommandId );
			return;
		}

		char* obuf = out->Buff;
        char* ptr = (char*)fmt;

        va_list argvlist;
        va_start( argvlist, fmt );     /* Initialize variable arguments. */
        //while( i != 0 )
		while( (*ptr)!=0 )
		{
			if( (*ptr) == '%' )
			{
				ptr++;
				switch((*ptr))
				{
				case 'd':
					{
				        (*va_arg( argvlist, int* )) = (*(int*)obuf);
					    obuf+=sizeof(int);
					}
					break;
		        case 'l':
					{
				        (*va_arg( argvlist, long* )) = (*(long*)obuf);
					    obuf+=sizeof(long);
					}
					break;
				case 'h':
					{
					    (*va_arg( argvlist, short* )) = (*(short*)obuf);
					    obuf+=sizeof(short);
					}
					break;
				case 's':
					{
						//char* str=obuf;
						unsigned short len = (*(unsigned short*)obuf);
						obuf+=sizeof( unsigned short );
						//char* argvbuf = (char*)va_arg( argvlist, char*);
						(char*)va_arg( argvlist, char*)=obuf;
						//memcpy( argvbuf, obuf, len );
						//argvbuf[len+1]='\0';
						obuf+=len+1;
					}
					break;
				case 'v':
					{
						va_arg( argvlist, void* ) = obuf+sizeof( unsigned short );
						unsigned short binlen = (*(unsigned short*)obuf);
						(*va_arg( argvlist, unsigned short* )) = binlen;
						obuf+=( sizeof( unsigned short ) + binlen );
					}
					break;


				}
            //sum += 1;
            //count++;
           	}
			ptr++;
		}
        va_end( argvlist );              /* Reset variable arguments.      */
		assert( out->Header.CommandLen != (obuf- (out->Buff)) );
	}
};


