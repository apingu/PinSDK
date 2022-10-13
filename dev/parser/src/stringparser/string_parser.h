//
//
//  alias  = XXXXX %a %b %c
//  string = AA1 %a
//     AA2 %b %c
//
//
//

#ifndef STRINGPARSER_H
#define STRINGPARSER_H

#include <vector>
#include <string>
#include <map>


class AliasString
{
    struct ParamField
    {
        std::string  name;
        std::string  value;
    };
    
    std::string                 m_AliasString;
    std::vector<ParamField>     m_ParamField;
    std::vector<std::string>    m_EquivalentVec;
    
public:

    AliasString();
    ~AliasString();
    
    void                        SetAliasString      ( const char* string );
    void                        AddEquivalent       ( const char* string );
    const char*                 GetAliasName        ();
    // 檢查輸入的字串 符不符合 格式
    bool                        CheckFormatString   ( const char* string );
    std::vector<std::string>    OutputFormat        ( const char* alias );
};



#endif// ALIASSTRING_H