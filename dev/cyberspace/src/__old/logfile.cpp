#include LogFile.h

LogFile::LogFile( const char* filename )
{
	if( ( m_Logf = fopen( filename, "aw" ) ) == NULL )
	{
		exit(0);
	}
}


void LogFile::SetLogMessage( const char* title, char* msg )
{
    time_t timep;
    time( &timep );
	struct tm* p = localtime( &timep );
    char message[ 256 ];
    sprintf( message, "%s : %s", title, asctime( p ) );
    fwrite( message, strlen( message ), 1, m_Logf );

    fwrite( msg, strlen( msg ), 1, m_Logf );
	fflush( m_Logf );
}