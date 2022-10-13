#include "excite_translater.h"

#include <pf-string.h>

#ifdef _WINDOWS
//#pragma comment (lib, "Utility.lib")
#endif

#include <pf-ziplib.h>
//#ifdef _UNICODE
//#pragma comment (lib, "PZLibU.lib")
//#else
//#pragma comment (lib, "PZLib.lib")
//#endif
//
//#pragma comment (lib, "NetSocket.lib")

#include <parser/http-parser.h>
#include <parser/html-parser.h>
//#pragma comment (lib, "parser.lib")

namespace helper
{
    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    ExciteTranslater::ExciteTranslater()
    {
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    ExciteTranslater::~ExciteTranslater()
    {
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================
    void ExciteTranslater::OnReceive()
    {
    }

    //=====================================================================================
    // NAME:
    // DESC:
    //=====================================================================================

    _mstring GetLocalLang( LPCSTR pszF, __in LPCSTR pszT )
    {
        if ( strcmp( pszT, "CH" ) == 0 )
        {
            char_t buff[ 1024 ];
            sprintf( buff, "wb_lp=%s%s&swb_lp=%s%s&big5=yes&before_lang=%s&after_lang=%s&big5_lang=yes", pszF, pszT, pszF, pszT, pszF, pszT );
            return buff;
        }

        return "";
    }


    _mstring GetLocalPage( const char_t* translage )
    {
        if ( strcmp( translage, "CH" ) == 0 )
        {
            return "chinese";
        }

        return "";
    }

    _mstring ExciteTranslater::Translate( __in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang )
    {
        if ( strcmp( pszInput, "" ) == 0 )
        {
            return "";
        }

        //const char_t repword[] = {"&Backslash;"};
        const char_t repword[] = { "##Bksl37##;" };
        // the web site seem ignore '\' so I replace to my define word first
        //&rob32;
        _mstring transs = _mstring( pszFromLang ) + _mstring( pszToLang );
        int inoutl = urlcodec_buffer_length( pszInput ) * strlen( repword );
        char_t* inouts = new char_t[ inoutl ];
        //strcpy(inouts, pszInput);
        strreplace( inouts, inoutl, pszInput, "\\", repword );
        _mstring localstr = GetLocalLang( pszFromLang, pszToLang );
        char_t* contentbuff = new char_t[ inoutl + transs.size() + localstr.size() + 256 + 1 ];
        //sprintf(contentbuff,  "before=%s&wb_lp=%s&%s&after=", http::url_encode(inouts), transs.c_str(), localstr.c_str());
        sprintf( contentbuff, "_qf__formTrans=&_token=06afdcbca9946&%s&auto_detect=off&auto_detect=on&before=%s&after=", localstr.c_str(), http::url_encode( inouts ) );
        /*
        {
            FILE* f = fopen("TestA.txt", "wb");
            if(f)
            {

                char_t obuffA[1024];
                strcpy(obuffA, pszInput);
                http::url_encode(obuffA);
                fwrite(obuffA, strlen(obuffA), 1, f);
                url_decode(obuffA);
                fwrite(obuffA, strlen(obuffA), 1, f);
                fclose(f);
            }
        }
        */
        delete[] inouts;
        char_t* httpheader = new char_t[ strlen( contentbuff ) + 2048 ];
        sprintf( httpheader, "\
POST /world/%s/ HTTP/1.1\n\
Host: www.excite.co.jp\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate\n\
Referer: http://www.excite.co.jp/world/chinese/\n\
Cookie: xtl_s=68e4ac0350f36c1abd025; xtl_detect=1; UID=B402689550EA2C18; registered=no; __utma=148185315.523190124.1357524118.1358242952.1358309877.14; __utmz=148185315.1358130204.11.4.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=%%E7%%BF%%BB%%E8%%AD%%AF; rsi_segs=K10143_0; exck=we_ar=4410|la_star=4|hm_pref=13|cl_mem=0|clt_mem=0|clh_mem=0|we_ar_usr_flg=0; __utmc=148185315; __utmb=148185315\n\
Connection: keep-alive\n\
Content-Type: application/x-www-form-urlencoded\n\
Content-Length: %d\n\
\n\
%s", GetLocalPage( pszToLang ).c_str(), strlen( contentbuff ), contentbuff );
        /*
        {
            FILE* f = fopen("TestA.txt", "wb");
            if(f)
            {
                fwrite(httpheader, strlen(httpheader), 1, f);
                fclose(f);
            }
        }

        {
            FILE* f = fopen("TestAA.txt", "wb");
            if(f)
            {
                fwrite(contentbuff, strlen(contentbuff), 1, f);
                fclose(f);
            }
        }
        */
        delete[] contentbuff;

        if ( net::TCPClient::Create() <= 0 )
        {
            return "";
        }

        if ( net::TCPClient::Connect( "www.excite.co.jp", 80 ) == false )
        {
            return "";
        }

        net::TCPClient::Send( (ubyte_t*)httpheader, strlen( httpheader ) );
        delete[] httpheader;

        http::Parser httpparser;
        int recvret = 0;

        // 要確定資料流已經收完了
        do
        {
            char_t bp[ 5120 ];
            memset( bp, 0, 5120 );
            recvret = net::TCPClient::Recv( (ubyte_t*)bp, 5120 );

            if ( recvret > 0 )
            {
                httpparser.addBytes( bp, recvret );
            }
            else if ( recvret < 0 )
            {
                return "";
            }
        } while ( recvret > 0 );

        Close();
        int context_coding = 0;
        const char_t* Context_Encoding = httpparser.getField( "content-encoding" );

        if ( Context_Encoding )
        {
            if ( strcmp( Context_Encoding, "gzip" ) == 0 )
            {
                context_coding = 1;
            }
        }

        BYTE* abDst = NULL;
        const BYTE* bodybuf = (const BYTE*)httpparser.getBody();
        int         contlen = httpparser.getContentLength();

        if ( context_coding == 1 )
        {
            int bodylen = httpparser.getBodyLength();
            contlen = pfziplib::gzip::GetMaxUncompressedLen( bodybuf, bodylen );

            if ( contlen <= 0 )
            {
                contlen = httpparser.getContentLength() * 10;
            }

            abDst = new BYTE[ contlen ];
            memset( abDst, 0, contlen );
            size32_t retl = pfziplib::gzip::UncompressBuffer( bodybuf, bodylen, abDst, contlen );
        }
        else
        {
            contlen = httpparser.getBodyLength();
            abDst = new BYTE[ contlen ];
            memset( abDst, 0, contlen );
            memcpy( abDst, bodybuf, contlen );
        }

        /*
        FILE* f = fopen("TestB.txt", "wb");
        if(f)
        {
            fwrite(abDst, contlen, 1, f);
            fclose(f);
        }
        */
        _mstring outstr;
        const char_t needmark[] = { "<textarea id=\"after\" " };
        char_t* datab = strstr( (char_t*)abDst, needmark );

        if ( datab )
        {
            datab += strlen( needmark );
            const char_t needsubmark[] = { " name=\"after\">" };
            datab = strstr( (char_t*)datab, needsubmark );
            datab += strlen( needsubmark );
            char_t* datae = strstr( (char_t*)datab, "</textarea>" );
            ( *datae ) = 0;
            char_t* outrbuff = new char_t[ strlen( datab ) + 1 ];
            strreplace( outrbuff, strlen( datab ) + 1, datab, repword, "\\" );
            _mstring teststr = outrbuff;
            outstr = html::Parser::Decode( outrbuff );
            delete[] outrbuff;
            /*
            FILE* f = fopen("TestC.txt", "ab");
            if(f)
            {
                fwrite(datab, strlen(datab), 1, f);
                fclose(f);
            }
            */
        }

        SAFE_DELETE_ARRAY( abDst );
        httpparser.release();
        return outstr;
    }



    _mstring GetLocalStringB( const char_t* translage )
    {
        if ( strcmp( translage, "CH" ) == 0 )
        {
            return "big5=yes";
        }

        return "";
    }

    _mstring ExciteTranslater::TranslateB( __in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang )
    {
        if ( strcmp( pszInput, "" ) == 0 )
        {
            return "";
        }

        if ( net::TCPClient::Create() <= 0 )
        {
            return "";
        }

        if ( net::TCPClient::Connect( "www.excite.co.jp", 80 ) == false )
        {
            return "";
        }

        //const char_t repword[] = {"&Backslash;"};
        const char_t repword[] = { "##Bksl37##;" };
        // the web site seem ignore '\' so I replace to my define word first
        //&rob32;
        _mstring transs = _mstring( pszFromLang ) + _mstring( pszToLang );
        int inoutl = urlcodec_buffer_length( pszInput ) * strlen( repword );
        char_t* inouts = new char_t[ inoutl ];
        //strcpy(inouts, pszInput);
        strreplace( inouts, inoutl, pszInput, "\\", repword );
        _mstring localstr = GetLocalStringB( pszToLang );
        char_t* contentbuff = new char_t[ inoutl + transs.size() + localstr.size() + 256 + 1 ];
        sprintf( contentbuff, "before=%s&wb_lp=%s&%s&after=", http::url_encode( inouts ), transs.c_str(), localstr.c_str() );
        delete[] inouts;
        char_t* httpheader = new char_t[ strlen( contentbuff ) + 2048 ];
        sprintf( httpheader, "\
POST /world/fantizi/ HTTP/1.1\n\
Host: www.excite.co.jp\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate\n\
Connection: keep-alive\n\
Referer: http://www.excite.co.jp/world/fantizi/\n\
Cookie: xtl_s=2e9da4f550ebd08995c55; xtl_detect=1; UID=B402689550EA2C18; registered=no; __utma=148185315.523190124.1357524118.1357631471.1357637944.7; __utmz=148185315.1357637944.7.3.utmcsr=eal.com.tw|utmccn=(referral)|utmcmd=referral|utmcct=/q&a_030.html; rsi_segs=K10143_0; exck=we_ar=4410|la_star=4|hm_pref=13|cl_mem=0|clt_mem=0|clh_mem=0|we_ar_usr_flg=0; __utmc=148185315; __utmb=148185315\n\
Content-Type: application/x-www-form-urlencoded\n\
Content-Length: %d\n\
\n\
%s", (int)strlen( contentbuff ), contentbuff );
        delete[] contentbuff;
        net::TCPClient::Send( (ubyte_t*)httpheader, strlen( httpheader ) );
        delete[] httpheader;
        http::Parser httpparser;
        int recvret = 0;

        // 要確定資料流已經收完了
        do
        {
            char_t bp[ 5120 ];
            memset( bp, 0, 5120 );
            recvret = net::TCPClient::Recv( (ubyte_t*)bp, 5120 );

            if ( recvret > 0 )
            {
                httpparser.addBytes( bp, recvret );
            }
            else if ( recvret < 0 )
            {
                return "";
            }
        } while ( recvret > 0 );

        /*
        {
        FILE* f = fopen("TestA.txt", "wb");
        if(f)
        {
        fwrite(httpparser.getRawBuffer(), httpparser.getRawLength(), 1, f);
        fclose(f);
        }
        }
        */
        int context_coding = 0;
        const char_t* Context_Encoding = httpparser.getField( "content-encoding" );

        if ( Context_Encoding )
        {
            if ( strcmp( Context_Encoding, "gzip" ) == 0 )
            {
                context_coding = 1;
            }
        }

        BYTE* abDst = NULL;
        const BYTE* bodybuf = (const BYTE*)httpparser.getBody();
        int         contlen = httpparser.getContentLength();

        if ( context_coding == 1 )
        {
            int bodylen = httpparser.getBodyLength();
            contlen = pfziplib::gzip::GetMaxUncompressedLen( bodybuf, bodylen );

            if ( contlen <= 0 )
            {
                contlen = httpparser.getContentLength() * 10;
            }

            abDst = new BYTE[ contlen ];
            memset( abDst, 0, contlen );
            size32_t retl = pfziplib::gzip::UncompressBuffer( bodybuf, bodylen, abDst, contlen );
        }
        else
        {
            contlen = httpparser.getBodyLength();
            abDst = new BYTE[ contlen ];
            memset( abDst, 0, contlen );
            memcpy( abDst, bodybuf, contlen );
        }

        /*
        FILE* f = fopen("TestB.txt", "wb");
        if(f)
        {
            fwrite(abDst, contlen, 1, f);
            fclose(f);
        }
        */
        _mstring outstr;
        const char_t needmark[] = { "<textarea name=\"after\" id=\"after\">" };
        char_t* datab = strstr( (char_t*)abDst, needmark );

        if ( datab )
        {
            datab += strlen( needmark );
            char_t* datae = strstr( (char_t*)datab, "</textarea>" );
            ( *datae ) = 0;
            char_t* outrbuff = new char_t[ strlen( datab ) + 1 ];
            strreplace( outrbuff, strlen( datab ) + 1, datab, repword, "\\" );
            outstr = outrbuff;
            delete[] outrbuff;
            /*
            FILE* f = fopen("TestC.txt", "ab");
            if(f)
            {
                fwrite(datab, strlen(datab), 1, f);
                fclose(f);
            }
            */
        }

        SAFE_DELETE_ARRAY( abDst );
        httpparser.release();
        Close();
        return outstr;
    }

}


