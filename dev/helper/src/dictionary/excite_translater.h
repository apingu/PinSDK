#ifndef _EXCITETRANSLATER_H
#define _EXCITETRANSLATER_H

#include <map>
#include <string>
#include <vector>
#include <net/net-client.h>

#include"dictionary.h"

namespace helper
{
    //const char_t ExciteTranslater_LanguageTable[][10] =
    //{
    //    "CH",
    //    "JA",
    //};

    class ExciteTranslater: public net::TCPClient
    {
    public:

        ExciteTranslater();
        ~ExciteTranslater();

        void     OnReceive();
        _mstring Translate( __in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang );
        _mstring TranslateB( __in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang );
    };
}


#endif