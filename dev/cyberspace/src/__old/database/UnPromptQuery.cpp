#include "UnPromptQuery.h"
#include <T_Lock.h>
#include <PfThread.h>
#include <PfStdlib.h>
#include <string>
#include <vector>
#include <list>



struct DBUpdateQuery
{
    XDataBase*                              pQueryDB;
    _tstring                                QueryString;
    UnPromptQuery::OnQueryListener*         pQueryListener;
    static T_Lock                           s_lUID;
    static int                              s_nUID;
};
int DBUpdateQuery::s_nUID = 0;

static int                                  s_Max_Update_Size = 0;
typedef std::list<DBUpdateQuery*>           DBUpdateQueryList_t;
typedef std::vector<DBUpdateQueryList_t>    DBUpdateQueryVec_t;

std::vector<T_Lock>*                        s_DBUdateLock = NULL;
DBUpdateQueryVec_t*                         s_DBUpdateQuery = NULL;
std::vector<PfSystem::Thread::Handle>       s_QuerThread;
bool                                        s_DBUpdateIdle = true;

int  GetDBAgentUseIndex();
void DBAgentUpdateQuery( void* argv );

UnPromptQuery::UnPromptQuery()
{
}

UnPromptQuery::~UnPromptQuery()
{
}

void UnPromptQuery::Create( int process )
{
    s_DBUpdateIdle = true;
    s_Max_Update_Size = process;
    s_DBUdateLock = new std::vector<T_Lock>;
    s_DBUpdateQuery = new DBUpdateQueryVec_t;
    s_DBUdateLock->resize( s_Max_Update_Size );
    s_DBUpdateQuery->resize( s_Max_Update_Size );
    
    for ( int i = 0; i < s_Max_Update_Size; i++ )
    {
        int* idx = new int;
        ( *idx ) = i;
        s_QuerThread.push_back( PfSystem::Thread::BeginProcess( DBAgentUpdateQuery, idx, _T( "UnPromptQuery::Create" ) ) );
    }
}

void UnPromptQuery::Destroy()
{
    s_DBUpdateIdle = false;
    
    for ( int idx = 0; idx < s_Max_Update_Size; idx++ )
    {
        TS_Lock lock( &( *s_DBUdateLock )[idx] );
        std::list<DBUpdateQuery*>::iterator it = ( *s_DBUpdateQuery )[idx].begin();
        
        while ( it != ( *s_DBUpdateQuery )[idx].end() )
        {
            DBUpdateQuery* procquer = ( *it );
            
            if ( procquer->pQueryDB != NULL )
            {
                XDBResult result = procquer->pQueryDB->ExecQuery( procquer->QueryString.c_str() );
                procquer->pQueryDB->FreeResult( result );
            }
            
            SAFE_DELETE( procquer );
        }
        
        ( *s_DBUpdateQuery )[idx].clear();
    }
    
    s_QuerThread.clear();
    s_DBUdateLock->clear();
    s_DBUpdateQuery->clear();
    SAFE_DELETE( s_DBUdateLock );
    SAFE_DELETE( s_DBUpdateQuery );
}


void DBAgentUpdateQuery( void* argv )
{
	int* idptr = ( int* )argv;
    int idx = *idptr;
    SAFE_DELETE( idptr );
    DBUpdateQuery* procquer = NULL;
    
    while ( s_DBUpdateIdle )
    {
        {
            TS_Lock lock( &( *s_DBUdateLock )[idx] );
            
            if ( !( *s_DBUpdateQuery )[idx].empty() )
            {
                procquer = ( *s_DBUpdateQuery )[idx].front();
                ( *s_DBUpdateQuery )[idx].pop_front();
            }
            else
            {
                procquer = NULL;
            }
        }
        
        if ( procquer )
        {
            if ( procquer->pQueryDB != NULL )
            {
                XDBResult result = procquer->pQueryDB->ExecQuery( procquer->QueryString.c_str() );
                
                if ( procquer->pQueryListener != NULL )
                {
                    procquer->pQueryListener->OnResult( result );
                }
                
                procquer->pQueryDB->FreeResult( result );
            }
            
            SAFE_DELETE( procquer );
        }
        else
        {
            //PfStd::MsSleep( 1 );
        }
    }
}

void UnPromptQuery::Query( XDataBase* pQueryDB, const char* query_string, UnPromptQuery::OnQueryListener* pListener )
{
    if ( s_Max_Update_Size == 0 )
    {
        return;
    }
    
    if ( pQueryDB == NULL )
    {
        return;
    }
    
    DBUpdateQuery* updatequery = new DBUpdateQuery;
    updatequery->pQueryDB = pQueryDB;
    updatequery->QueryString = query_string;
    updatequery->pQueryListener = pListener;
    // TODO: Pin
    // 用記憶體當依據 不知道會不會出事
    //int uid = (int)updatequery->pQueryDB;
    //int pos = (uid % s_Max_Update_Size);
    int uid = 0;
    int pos = 0;
    {
        TS_Lock lock( &DBUpdateQuery::s_lUID );
        uid = DBUpdateQuery::s_nUID;
        DBUpdateQuery::s_nUID++;
        pos = ( uid % s_Max_Update_Size );
    }
    TS_Lock querylock( &( *s_DBUdateLock )[pos] );
    ( *s_DBUpdateQuery )[pos].push_back( updatequery );
}
