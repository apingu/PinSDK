#include "PaserDecode.h"
#include <assert.h>
#include <Windows.h>
#include <time.h>




PaserDecode::PaserDecode()
{



}

PaserDecode::~PaserDecode()
{


}


char* freadline( FILE* f, char* buffer, size_t size )
{
	do
	{
		if ( fgets( buffer, size, f ) == NULL )
			return NULL;
	}
	while(( buffer[0] == '\n' ) || feof(f));  //it will jump black line

	int len = strlen( buffer );
	buffer[strlen( buffer ) - 2] = '\0'; //ODOA problem
	return buffer;    
}

int findnextline( const std::vector<std::string>* lines, unsigned int startline, const char* keyword )
{
	for( int i=startline; i<lines->size(); i++ )
	{
		if( (*lines)[i]== keyword )
			return i;
	}
	return -1;
}

void PaserDecode::LoadConfigure( const char* fname )
{
	
	FILE* f= fopen( fname, "rb" );
	if( f==NULL )
		return ;

	while( !feof(f) )
	{
		char commbuff[_MAX_FNAME];
		freadline( f, commbuff, _MAX_FNAME );
		if( strcmp(commbuff, "")==0)
			continue;
		m_CommandLine.push_back(commbuff);

		// pass one
		char* p  = strtok( commbuff, " " );
		if( strcmp( p, ":")==0 )
		{
			p  = strtok( NULL, " " );
			m_linemark.insert( std::map< std::string, int >::value_type(p, m_CommandLine.size()-1) );
		}
	}
}

void PaserDecode::Execute( unsigned int taskid, unsigned int startline )
{
	
	for( unsigned int i=startline; i<m_CommandLine.size(); i++ )
	{
		//char* commbuf = new char[m_CommandLine.size()+1];
		char commbuf[255];
		strcpy( commbuf, m_CommandLine[i].c_str() );
		

		char* p  = strtok( commbuf, " " );
		

		if( strcmp( p, ":")==0 )
		{
			continue;
		}
		else if( strcmp( p, "GOTO")==0 )
		{
			p  = strtok( NULL, " " );
			std::map< std::string, int >::iterator iter = m_linemark.find(p);
			if( iter!=m_linemark.end() )
				i=iter->second;
			continue;
		}
		else if( strcmp( p, "END" )==0 )
		{
			p  = strtok( NULL, " " );
			if(  atoi(p)==taskid )
				return;
		}
		else if( strcmp( p, "IF" )==0 )
		{
			std::map< std::string, int > variable;
			p  = strtok( NULL, " (,=" );
			while( p!=NULL )
			{
				std::string type=p;
				p = strtok( NULL, " )," );
				variable.insert( std::map< std::string, int >::value_type(type, atoi(p)) );
				p  = strtok( NULL, " =)" );
			}
			i = findnextline( &m_CommandLine, i, "ENDIF" );

		}
		else if( strcmp( p, "@" )==0 )
		{

		    time_t timep;
			time(&timep);

			struct tm* currtimes=localtime(&timep);

			struct tm nexttimes;
			memcpy( &nexttimes, currtimes, sizeof(tm) );
			printf( "wait \n");

			p  = strtok( NULL, ":" );
			nexttimes.tm_hour = atoi(p);
			p  = strtok( NULL, ":" );
			nexttimes.tm_min = atoi(p);
			p  = strtok( NULL, ":" );
			nexttimes.tm_sec = atoi(p);
			unsigned int currms = atoi(p);

			while( difftime(mktime(&nexttimes), timep)>0)
			{
				Sleep(0);
				time(&timep);
			}

			unsigned int nextms = GetTickCount();
			while(( GetTickCount()-nextms )<currms)
			{
				Sleep(0);
			}		
		}
		else if( strcmp( p, "@D" )==0 )
		{

		}
		else if( strcmp( p, "@O+" )==0 )
		{

		}
		else if( strcmp( p, "@O-" )==0 )
		{

		}

		else
		{
			
			char filebuff[_MAX_FNAME];
			strcpy(filebuff, p);
			p  = strtok( NULL, " " );

			/*			
			std::vector<char*> paramlist;
			
			while( p!=NULL )
			{
				char* parambuff = new char[strlen(p)+1];
				strcpy( parambuff, p );
				paramlist.push_back(parambuff);
				p  = strtok( NULL, " " );
			}
			*/

			HINSTANCE ans=::ShellExecute( NULL, "open", (filebuff), p, NULL, SW_SHOW );
			//HINSTANCE ans=::ShellExecute( NULL, "open", "TestComm.exe", NULL, NULL, SW_SHOW );
			/*
			for( int a=0; a<paramlist.size(); a++ )
			{
				delete [] paramlist[a];
			}
			paramlist.clear();
			*/

			long ii = (long)ans;
			switch( ii )
			{
			case ERROR_FILE_NOT_FOUND:
				printf( "The specified file was not found. " );
				break;
			case ERROR_PATH_NOT_FOUND:
				printf( "The specified path was not found. " );
				break;
			case ERROR_BAD_FORMAT:
				printf( "The .exe file is invalid (non-Microsoft Win32 .exe or error in .exe image). " );
				break;
			case SE_ERR_ACCESSDENIED:
				printf("The operating system denied access to the specified file. " );
				break;
			case SE_ERR_ASSOCINCOMPLETE:
				printf( "The file name association is incomplete or invalid. " );
				break;
			case SE_ERR_DDEBUSY:
				printf( "The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed. " );
				break;
			case SE_ERR_DDEFAIL:
				printf( "The DDE transaction failed. ");
				break;
			case SE_ERR_DDETIMEOUT:
				printf("The DDE transaction could not be completed because the request timed out. ");
				break;
			case SE_ERR_DLLNOTFOUND:
				printf( "The specified dynamic-link library (DLL) was not found." );
				break;
// 			case SE_ERR_FNF:
// 				printf( "The specified file was not found. ");
// 				break;
			case SE_ERR_NOASSOC:
				printf( "There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable. " );
				break;
			case SE_ERR_OOM:
				printf( "There was not enough memory to complete the operation. " );
				break;
// 			case SE_ERR_PNF:
// 				printf( "The specified path was not found. " );
// 				break;
			case SE_ERR_SHARE:
				printf("A sharing violation occurred. " );
				break;
			}

		}



		//delete [] commbuf;
	}

}
