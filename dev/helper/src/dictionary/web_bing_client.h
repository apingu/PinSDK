#ifndef _WEBBINGCLIENT_H
#define _WEBBINGCLIENT_H

#include<map>
#include<string>
#include<vector>
#include<net/net-client.h>


namespace Helper
{
    //const char WebBingClient_LanguageTable[][10] =
    //{
    //    "zh-cht",
    //    "ja",
    //};

    class WebBingClient: public net::TCPClient
    {
        //std::string m_Origin;
        std::string m_Transl;

    public:

        WebBingClient();
        ~WebBingClient();

        void        OnReceive( net::TCPSocket* s );
        std::string Translate( __in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang );
    };
}


#endif