#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <string>

namespace html
{
    class Parser
    {
    public:
    
        Parser();
        ~Parser();
        
        static void        SetEOLChar( const char* eol );
        static std::string Encode    ( const char* ins );
        static std::string Decode    ( const char* ins );
    };
}

#endif //HTMLPARSER_H