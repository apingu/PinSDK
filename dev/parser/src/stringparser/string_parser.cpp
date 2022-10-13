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
#include "string_parser.h"


//=============================================================================
// NAME:
// DESC:
//=============================================================================
AliasString::AliasString()
{

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
AliasString::~AliasString()
{

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void AliasString::SetAliasString( const char* string )
{
    m_ParamField.clear();
    //m_AliasString=string;

    char* pTempbuf = new char[ strlen( string ) + 1 ];
    strcpy( pTempbuf, string );
    // 先分析 原本字串
    // 建立有哪些 參數相對應的欄位
    const char* pAlias = strtok( pTempbuf, " " );
    if( pAlias )
    {
        // 第一個是指令
        m_AliasString = pAlias;
        //tempvec.push_back(pAlias);
        pAlias = strtok( NULL, " " );
        while( pAlias )
        {
            if( ( *pAlias ) == '%' )
            {
                ParamField field;
                m_ParamField.push_back( field );
                m_ParamField.back().name = pAlias;
                m_ParamField.back().value = "";
            }
            pAlias = strtok( NULL, " " );
        }
    }
    delete[] pTempbuf;

}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
void AliasString::AddEquivalent( const char* string )
{
    if( string == NULL )
    {
        return;
    }
    if( ( string[ 0 ] == 0 ) || ( string[ 0 ] == 13 ) || ( string[ 0 ] == '\n' ) )
    {
        return;
    }

    std::string stringA = string;
    m_EquivalentVec.push_back( stringA );
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
const char* AliasString::GetAliasName()
{
    return m_AliasString.c_str();
}

//=============================================================================
// NAME:
// DESC:
//=============================================================================
bool AliasString::CheckFormatString( const char* string )
{
    bool bret = false;
    char* pBuff = new char[ strlen( string ) + 1 ];
    strcpy( pBuff, string );
    char* p = strtok( pBuff, " " );
    if( p )
    {
        if( strcmp( p, m_AliasString.c_str() ) == 0 )
        {
            bret = true;
        }
    }
    SAFE_DELETE_ARRAY( pBuff );
    return bret;
}


//=============================================================================
// NAME:
// DESC:
//=============================================================================
std::vector<std::string> AliasString::OutputFormat( const char* alias )
{
    std::vector<std::string> outputvec;

    if( alias == NULL )
    {
        return outputvec;
    }

    char* pTempbuf = new char[ strlen( alias ) + 1 ];
    strcpy( pTempbuf, alias );

    //----------------------------------------------------
    // 把 指令 跟 參數 分解出來
    // command1 aaa dd cccc
    // =>tempvec[0]==aaa  tempvec[1]==dd tempvec[2]==cccc
    // 先找出 以空白 為分隔的 所有字串
    std::vector<char*> paramvecbb;
    char* pStringMask = new char[ strlen( alias ) + 1 ];
    splitstringstring( pStringMask, pTempbuf );
    char* pParamN = strtok( pTempbuf, " \"" );
    while( pParamN )
    {
        paramvecbb.push_back( pParamN );
        pParamN = strtok( NULL, " \"" );
    }
    makestringstring( pStringMask, pTempbuf );
    SAFE_DELETE_ARRAY( pStringMask );

    /*
    std::string divstring;
    char* pStr=pTempbuf;
    while((*pStr)!='\0')
    {
     // 這是一個字串
     if((*pStr)=='\"')
     {
      while((*pStr)!='\0')
      {
       pStr++;
       if((*pStr)!='\"')
       {
        divstring+=(*pStr);
       }
       else
       {
        break;
       }
      }

      paramvec.push_back(divstring);
      divstring.clear();
      pStr++;
      if((*pStr)=='\0')
      {
       break;
      }
      continue;
     }

     // 找到空白之後
     if((*pStr)==' ')
     {
      if(!divstring.empty())
      {
       paramvec.push_back(divstring);
       divstring.clear();
      }
     }
     else
     {
      divstring+=(*pStr);
     }

     pStr++;
    }


    if(!divstring.empty())
    {
     paramvec.push_back(divstring);
     divstring.clear();
    }
    */


    std::vector<std::string> paramvec;

    for( size_t i = 0; i < paramvecbb.size(); i++ )
    {
        paramvec.push_back( paramvecbb[ i ] );
    }

    if( paramvec.empty() )
    {
        SAFE_DELETE_ARRAY( pTempbuf );
        return outputvec;
    }

    // 指令不合
    if( m_AliasString != paramvec[ 0 ] )
    {
        SAFE_DELETE_ARRAY( pTempbuf );
        return outputvec;
    }

    // 第一個當成指令 所以 不需要
    paramvec.erase( paramvec.begin() );

    if( m_ParamField.size() != paramvec.size() )
    {
        return outputvec;
    }

    //-----------------------------------------------------------------------------
    // 把資料放入 參數表中
    int maxparamsize = __min( m_ParamField.size(), paramvec.size() );
    for( int i = 0; i < maxparamsize; i++ )
    {
        m_ParamField[ i ].value = paramvec[ i ];
    }

    for( size_t i = 0; i < m_EquivalentVec.size(); i++ )
    {
        // TODO: Pin 先固定 以後再 做動態
        char pString[ 1024 ];
        char pString2[ 1024 ];

        strcpy( pString, m_EquivalentVec[ i ].c_str() );

        for( size_t j = 0; j < m_ParamField.size(); j++ )
        {
            strreplace( pString2, 1024, pString, m_ParamField[ j ].name.c_str(), m_ParamField[ j ].value.c_str() );
            strcpy( pString, pString2 );
        }

        outputvec.push_back( pString );
    }

    return outputvec;
}
