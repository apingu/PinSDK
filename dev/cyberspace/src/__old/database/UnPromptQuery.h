//
//
//   非即時 查詢
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

#include <XDatabase.h>

class UnPromptQuery
{

public:

    class OnQueryListener
    {
    public:
        virtual void OnResult( const XDBResult result ) = 0;
    };
    
    UnPromptQuery();
    ~UnPromptQuery();
    
    static void Create( int process );
    static void Destroy();
    
    static void Query( XDataBase* pQueryDB, const char* query_string, UnPromptQuery::OnQueryListener* pListener = NULL );
};