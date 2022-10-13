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

//01. == C 系統文件/C++ 系統文件
#include <map>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-def.h>
#include <pf-stdlib.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "html-parser.h"
#include "html-entities.h"

namespace html
{
    std::string s_HTMLEOFChar = _EOL_;
    std::map<std::string, std::string> s_HtmlEscapeStrMapA;
    std::map<std::string, std::string> s_HtmlEscapeStrMapB;
    //std::map<int, std::string>  s_HtmlEscapeIdxMapB;
    
    void Init()
    {
        if ( s_HtmlEscapeStrMapA.empty() )
        {
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( "\"", "quot" ) );
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( "&", "amp" ) );
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( "<", "lt" ) );
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( ">", "gt" ) );
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( "\r", "#10" ) );
            s_HtmlEscapeStrMapA.insert( std::map<std::string, std::string>::value_type( "\n", "#10" ) );
            std::map<std::string, std::string>::iterator iter = s_HtmlEscapeStrMapA.begin();
            
            while ( iter != s_HtmlEscapeStrMapA.end() )
            {
                s_HtmlEscapeStrMapB.insert( std::map<std::string, std::string>::value_type( iter->second, iter->first ) );
                iter++;
            }
            
            //
            s_HtmlEscapeStrMapB.insert( std::map<std::string, std::string>::value_type( "nbsp", " " ) );
            s_HtmlEscapeStrMapB.erase( "#10" );
            //s_HtmlEscapeStrMapB.insert(std::map<std::string, std::string>::value_type("#10", "\r\n"));
            s_HtmlEscapeStrMapB.insert( std::map<std::string, std::string>::value_type( "#10", "\n" ) );
            //
        }
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Parser::Parser()
    {
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    Parser::~Parser()
    {
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    void Parser::SetEOLChar( const char* eol )
    {
        s_HTMLEOFChar = eol;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    std::string Parser::Encode( const char* ins )
    {
        Init();
        
        if ( ins == NULL )
        {
            return "";
        }
        
        if ( strcmp( ins, "" ) == 0 )
        {
            return "";
        }
        
        int   inl = strlen( ins );
        int   outl = ( inl * 12 ) + 1;
        char* outs = new char[ outl ];
        memset( outs, 0, outl );
        int j = 0;
        
        for ( int i = 0; i < inl; i++ )
        {
            char ch[ 32 ];
            ch[ 0 ] = ins[ i ];
            ch[ 1 ] = 0;
            //strcpy(ch, ins + i);
            std::map<std::string, std::string>::iterator iter = s_HtmlEscapeStrMapA.find( ch );
            
            if ( iter != s_HtmlEscapeStrMapA.end() )
            {
                char word[ 32 ];
                _stprintf( word, "&%s;", iter->second.c_str() );
                strcat( outs, word );
                j += strlen( word );
                
                if ( ( ins[ i ] == '\r' ) || ( ins[ i ] == '\n' ) )
                {
                    if ( ( ins[ i + 1 ] == '\r' ) || ( ins[ i + 1 ] == '\n' ) )
                    {
                        i++;
                    }  //\n
                }
            }
            else
            {
                outs[ j ] = ins[ i ];
                j++;
            }
        }
        
        std::string datacontent = outs;
        SAFE_DELETE_ARRAY( outs );
        return datacontent;
    }
    
    //=============================================================================
    // NAME:
    // DESC:
    //=============================================================================
    std::string Parser::Decode( const char* ins )
    {
        Init();
        
        if ( ins == NULL )
        {
            return "";
        }
        
        if ( strcmp( ins, "" ) == 0 )
        {
            return "";
        }
        
        int   inl = strlen( ins );
        int   outl = inl + 1;
        char* outs = new char[ outl ];
        memset( outs, 0, outl );
        char* oo = outs;
        //int j = 0;
        
        for ( int i = 0; i < inl; i++ )
        {
            bool brep = true;
            
            if ( ins[ i ] == '&' )
            {
                const char* bp = ins + i;
                const char* ep = strstr( bp, ";" );
                
                if ( ep != NULL )
                {
                    char word[ 32 ];
                    int  wlen = ep - bp - 1;
                    memcpy( word, bp + 1, wlen );
                    word[ wlen ] = 0;
                    int oldwl = strlen( word );
                    
                    if ( word[ 0 ] == '#' )
                    {
                        int no = atoi( word + 1 );
                        _stprintf( word, "#%d", no );
                    }
                    
                    std::map<std::string, std::string>::iterator iter = s_HtmlEscapeStrMapB.find( word );
                    
                    if ( iter != s_HtmlEscapeStrMapB.end() )
                    {
                        memcpy( oo, iter->second.c_str(), iter->second.size() );
                        oo += iter->second.size();
                    }
                    
                    i += oldwl + 1; // it will add one next in loop so only add 1
                    brep = false;
                }
            }
            
            if ( brep == true )
            {
                ( *oo ) = ins[ i ];
                oo++;
            }
        }
        
        std::string datacontent = outs;
        SAFE_DELETE_ARRAY( outs );
        return datacontent;
    }
    
}

