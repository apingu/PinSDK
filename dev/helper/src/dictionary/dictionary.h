//
//
//    建立一個 字典檔
//    字串對應到 TranslateData
//
//
//
//
//
//
#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <map>
#include <string>
#include <vector>
#include <pf-def.h>

namespace helper
{
#define _MAX_DIC    1000

    typedef std::vector<_mstring>    stringlist_t;

    enum LanguageSet
    {
        ls_tw = 0,
        ls_jp = 1,
        ls_max,
    };

    class Dictionary
    {
        //_mstring m_SkipChar;
        LanguageSet                       original_;
        LanguageSet                       translate_;

        struct TranslateData
        {
            _mstring     text;
            int          num_used;
        };

        std::map<_mstring, TranslateData> dictionary_words_[ _MAX_DIC ];      // 字典檔
        std::map<_mstring, TranslateData> decompose_words_[ _MAX_DIC ];       // 字典檔
        std::map<_mstring, _mstring>      uninterpretable_words_[ _MAX_DIC ];

    public:

        Dictionary();
        ~Dictionary();

        // 加入一個字典檔 excel xml 2003
        //void        Append(const char_t* xmlfile);


        // 設置字串前後要 略過的字元 like  \r\n   AAAABBB CCCC   \r\n   => AAAABBB CCCC
        void        SetSkipChar( const char_t* ins );

        // 加入一個新的字
        void        AddWord( const char_t* ins1, const char_t* ins2 );

        // 加入句子分割字元
        void        AddSeparator( const char_t* ins );

        // 翻譯
        _mstring    Translate( const char_t* ins );

        _mstring    TranslateByCompose( const char_t* ins );

        void        SetTranslate( LanguageSet origin, LanguageSet translate );
        _mstring    TranslateFromExcite( const char_t* ins );
        _mstring    TranslateFromBing( const char_t* ins );

        // 清除字典內容
        void        ClearContent();

        // 取出所以的內容
        //void        GetContent(std::vector<_mstring> &st, std::vector<_mstring> &tt, std::vector<int> &ct);
        void        GetContent( std::vector<stringlist_t>& nodes );

        void        GetUninterpretableWords( std::vector<stringlist_t>& words );
    };
}

#endif //_DICTIONARY_H