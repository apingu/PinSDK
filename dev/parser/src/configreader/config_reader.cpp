/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or
 * mailto:support@zona.net
 *
 * Filename: ConfigReader.cpp
 * Original Author: Mark Harmon
 * Notes:
 * To Do:
 *
 * $History: $
 * 1.0 - Aug 15th release of demo code
 *
 ********************************************************************/

 //01. == C 系統文件/C++ 系統文件
#include <string.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <Unicode.h>

 //03. == 其他 使用 函式庫的.h 文件

 //04. == 此專案內.h 文件
#include "config_reader.h"


#ifndef FALSE
#define FALSE 0l
#endif
#ifndef TRUE
#define TRUE 1l
#endif

/*
#if defined(_PS2) || defined(_XBOX)
#include "file.h"
#define _tfopen  file_open
#define _tfseek  file_seek
#define _tftell  file_tell
#define _tfread  file_read
#define _tfwrite file_write
#define _tfclose file_close

#define _T(x)  x
#define strcpy   strcpy
#define strlen   strlen
#define strstr   strstr
#define _stprintf _stprintf
#define atoi  atoi
#endif
*/


#define _tfopen  fopen
#define _tfseek  fseek
#define _tftell  ftell
#define _tfread  fread
#define _tfwrite fwrite
#define _tfclose fclose


FILE* force_fopen( const char* filename, const char* mode )
{
    return _tfopen( filename, mode );
    /*
    FILE* f=NULL;
    for(int i = 0; i<10; i++)
    {
     f  = _tfopen(filename, mode);
     if(f == NULL)
     {
      if(errno==2)
      {
       return false;
      }
      else
      {
       pfstd::MSSleep(10);
      }
     }
     else
     {
      return f;
     }
    }
    return NULL;
    */
}

//////////////////////////////////////////////////////////////////////
//
namespace
{
    //////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////
    struct ConfigReaderData
    {
        _mstring   name;
        _mbsvector value;
        _mstring   section_name;
        _mstring   comment_out;
    };

    //////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////
    bool IsCommentOutCharactor( const char* read_buff )
    {
        if( read_buff == NULL )
        {
            return false;
        }

        int read_len = strlen( read_buff );

        if( read_len >= 1 )
        {
            if( ( read_buff[0] == '#' ) ||
                ( read_buff[0] == ';' ) )
            {
                return true;
            }
        }

        if( read_len >= 2 )
        {
            if( ( read_buff[ 0 ] == '/' ) ||
                ( read_buff[ 1 ] == '/' ) )
            {
                return true;
            }
        }
        return false;
    }

    //////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////
    bool CConfigReaderLoad( _OUT std::vector<ConfigReaderData>& out_vec, FILE* file )
    {
        _tfseek( file, 0, SEEK_END );
        int fsize = _tftell( file );
        _tfseek( file, 0, SEEK_SET );

        if( fsize == 0 )
        {
            return FALSE;
        }

        _mstring section_name;

        int lii = 0;
        char* read_buff = new char[ fsize ];
        while( fgets( read_buff, fsize, file ) != NULL )
        {
            char* line_buff = read_buff;
            // utf8 with bom
            if( lii == 0 )
            {
                if( IsUTF8( line_buff ) == 2 )
                {
                    line_buff += 3;
                }
            }
            lii++;

            ConfigReaderData ld;

            line_buff[ strlen( line_buff ) ] = '\0'; // eat the newline fgets() stores /r/n
            //printf( "%s\n", buf );

            std::string line = line_buff;
            int current = 0;
            // 除去前後空白
            line.erase( 0, line.find_first_not_of( " " ) );
            line.erase( line.find_last_not_of( "\n" ) + 1 );
            line.erase( line.find_last_not_of( "\r" ) + 1 );
            line.erase( line.find_last_not_of( " " ) + 1 );

            // 區段標籤
            if( ( line[ 0 ] == '[' ) && ( line.back() == ']' ) )
            {
                // 去除前後 中括號
                section_name = line;
                section_name.erase( 0, 1 );
                section_name.erase( section_name.size() - 1 , 1 );
                ld.section_name = section_name;
                out_vec.push_back( ld );
                // 下一行
                continue;
            }

            // 有 // 或 沒有 = 代表應該就不是一個設定的格式
            if( IsCommentOutCharactor( line.c_str() ) )
            {
                // 空白欄位?
                ld.comment_out = line;
                out_vec.push_back( ld );
                continue;
            }

            int pos = line.find_first_of( "=", current );

            std::string name = line.substr( current, pos - current );
            std::string value = line.substr( pos + 1 );

            // 除去前後空白
            name.erase( 0, name.find_first_not_of( " " ) );
            name.erase( name.find_last_not_of( " " ) + 1 );

            _mbsvector value_vec;
            {
                bool     is_str = false;
                _mstring value_string;

                for( uint_t vi = 0; vi < value.size(); vi++ )
                {
                    if( !is_str )
                    {
                        if( value[ vi ] == ( '\"' ) )
                        {
                            is_str = true;
                        }

                        // 註解 直接跳到下一行
                        else if( IsCommentOutCharactor( &value[ vi ] ) )
                        {
                            break;
                        }
                    }
                    else
                    {
                        // \" \\  \\n
                        if( value[ vi ] == ( '\\' ) )
                        {
                            // 強制放入下一個
                            vi++;
                            value_string.append( 1, value[ vi ] );
                        }
                        else if( value[ vi ] == ( '\"' ) )
                        {
                            value_vec.push_back( value_string );
                            value_string.clear();
                            is_str = false;
                        }
                        else if( value[ vi ] == ( '\n' ) )
                        {
                            value_string.append( 1, value[ vi ] );
                            value_vec.push_back( value_string );
                            value_string.clear();
                            is_str = false;
                        }
                        else
                        {
                            value_string.append( 1, value[ vi ] );
                        }
                    }
                }
            }

            // 去除前後空格 跟 斷行
            //value.erase( 0, value.find_first_not_of( "\" " ) );
            //value.erase( value.find_last_not_of( "\" \n\r" ) + 1 );


            // 值必須是 " "  因此 後引號後面的都去掉
            //  前後必須是 "" 字串 格式錯誤
            //if ( ( value.front() != '\"' ) && ( value.back() != '\"' ) )
            //{
            //    continue;
            //}
            //value.erase( value.find_last_not_of( "\"" ) + 1 );
            //value.erase( value.find_last_not_of( "\"" ) + 1 );

            ld.name = name;
            ld.value = value_vec;
            out_vec.push_back( ld );
            //content_.insert( std::map<std::string, std::string>::value_type( name, value ) );
        }
        delete[] read_buff;
        return true;
    }
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConfigReader::CConfigReader()
{
    is_write_enabled_ = FALSE;
    filename_.clear();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
CConfigReader::~CConfigReader()
{
    CloseConfigFile();
}


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
bool CConfigReader::OpenConfigFile( const char* fname )
{
    if( fname == NULL )
    {
        return FALSE;
    }

    FILE* file = ( FILE* )force_fopen( fname, ( "a+b" ) );

    if( file )
    {
        is_write_enabled_ = TRUE;
    }
    else
    {
        file = ( FILE* )force_fopen( fname, ( "rb" ) );
    }

    //  file_ = fopen(fname,"rb");
    if( file == NULL )
    {
        return FALSE;
    }

    filename_ = fname;

    _tfseek( file, 0, SEEK_END );
    int fsize = _tftell( file );
    _tfseek( file, 0, SEEK_SET );

    if( fsize == 0 )
    {
        return FALSE;
    }

    std::vector<ConfigReaderData> out_vec;
    CConfigReaderLoad( out_vec, file );

    fclose( file );

    _mstring section_name;
    for( uint_t ot = 0; ot < out_vec.size(); ot++ )
    {
        if( !out_vec[ ot ].section_name.empty() )
        {
            section_name = out_vec[ ot ].section_name;
        }

        if( !out_vec[ ot ].name.empty() )
        {
            _tstring key_name;
            // 有區段 就用 區段．名稱去找
            if( !section_name.empty() )
            {
                key_name = section_name + _T('.') + out_vec[ ot ].name;
            }
            else
            {
                key_name = out_vec[ ot ].name;
            }
            content_.insert( _mbstablesi::value_type( out_vec[ ot ].name, out_vec[ ot ].value ) );
        }
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void CConfigReader::CloseConfigFile()
{
    filename_.clear();
    content_.clear();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
bool CConfigReader::SaveConfigFile( const char* fname )
{
    FILE* file = ( FILE* )force_fopen( fname, ( "w+b" ) );

    //  file_ = fopen(fname,"rb");
    if( file == NULL )
    {
        return FALSE;
    }

    filename_ = fname;

    _tfseek( file, 0, SEEK_END );
    int fsize = _tftell( file );
    _tfseek( file, 0, SEEK_SET );

    _mbstablesi::iterator iter = content_.begin();
    while( iter != content_.end() )
    {
        _mstring line_string;
        pfstd::strprintf( line_string, "%s=", iter->first.c_str() );
        for( uint_t vi = 0; vi < iter->second.size(); vi++ )
        {
            pfstd::strprintfa( line_string, "\"%s\"", iter->second[ vi ].c_str() );
        }
        pfstd::strprintfa( line_string, "\n" );

        fwrite( line_string.c_str(), line_string.size(), 1, file );
        iter++;
    }
    fclose( file );

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// STRINGS
///////////////////////////////////////////////////////////////////////////////
_mbsvector CConfigReader::GetString( const char* name )
{
    if( content_.empty() )
    {
        return _mbsvector();
    }

    _mbstablesi::iterator iter = content_.find( name );
    if( iter != content_.end() )
    {
        return iter->second;
    }
    return _mbsvector();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void CConfigReader::SetString( const char* name, _mbsvector value )
{
    content_.insert( _mbstablesi::value_type( name, value ) );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
const char* CConfigReader::GetOneString( const char* name )
{
    if( content_.empty() )
    {
        return NULL;
    }

    _mbstablesi::iterator iter = content_.find( name );
    if( iter != content_.end() )
    {
        if( iter->second.empty() )
        {
            return NULL;
        }
        return iter->second[ 0 ].c_str();
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void CConfigReader::SetOneString( const char* name, const char* value )
{
    _mbsvector values;
    values.push_back( value );
    SetString( name, values );
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
_mbstablesi::iterator CConfigReader::begin()
{
    return content_.begin();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
_mbstablesi::iterator CConfigReader::end()
{
    return content_.end();
}

///////////////////////////////////////////////////////////////////////////////
// automatically opens and closes file
char* CConfigReader::GetStringOnce( const char* fname, const char* name )
{
    CConfigReader cf;
    if( !cf.OpenConfigFile( fname ) )
    {
        return NULL;
    }

    _mbsvector vec = cf.GetString( name );
    if( vec.empty() )
    {
        return NULL;
    }

    const char* buf = vec[ 0 ].c_str();
    //const char* buf = cf.GetString( name );
    //if ( strcmp( buf, "" ) == 0 )
    //{
    //    return NULL;
    //}

    int len = strlen( buf );
    char* out = new char[ len + 1 ];
    strcpy( out, buf );
    return out;
}

///////////////////////////////////////////////////////////////////////////////
//
//std::map<std::string, std::string> CConfigReader::GetOnce( const char* fname )
//{
//    CConfigReader cf;
//    if ( !cf.OpenConfigFile( fname ) )
//    {
//        return std::map<std::string, std::string>();
//    }
//    return cf.content_;
//}

///////////////////////////////////////////////////////////////////////////////
//int CConfigReader::WriteString( const char* name, const char* value )
//{
//    if ( !is_write_enabled_ )
//    {
//        return FALSE;
//    }
//
//    content_.insert( std::map<std::string, std::string> ::value_type( name, value ) );
//    return 1;
//}