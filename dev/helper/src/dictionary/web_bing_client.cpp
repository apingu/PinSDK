#include "web_bing_client.h"


#include <pf-ziplib.h>
#include <parser/http-parser.h>

#pragma comment (lib, "netsocket.lib")
#pragma comment (lib, "zipbli.lib")
#pragma comment (lib, "parser.lib")

namespace Helper
{

	//=====================================================================================
	// NAME: 
	// DESC:
	//=====================================================================================
    WebBingClient::WebBingClient()
	{

	}

	//=====================================================================================
	// NAME: 
	// DESC:
	//=====================================================================================
	WebBingClient::~WebBingClient()
	{
	
	}

	//=====================================================================================
	// NAME: 
	// DESC:
	//=====================================================================================
	void WebBingClient::OnReceive(net::TCPSocket* s)
	{

	}

	//=====================================================================================
	// NAME: 
	// DESC:
	//=====================================================================================
	std::string WebBingClient::Translate(__in LPCSTR pszInput, int iInputLen, __in_opt LPCSTR pszFromLang, __in LPCSTR pszToLang)
	{
		if(strcmp(pszInput, "")==0)
			return "";

		if(net::TCPClient::Create()<=0)
			return "";

		if(net::TCPClient::Connect("www.bing.com", 80)==false)
			return "";

		//m_Origin = "ja";
		//m_Transl = "zh-cht";


		std::string transs = std::string(pszFromLang)+std::string(pszToLang);
		int inoutl = urlcodec_buffer_length(pszInput);
		char* inouts = new char[inoutl];
		strcpy(inouts, pszInput);

		
		char httpheader[10240];
		sprintf(httpheader, "\
GET /v2/ajax.svc/TranslateArray?appId=%%22Apingu%%22&texts=[%%22%s%%22]&from=%%22%%22&to=%%22%s%%22&oncomplete=_mstc3&onerror=_mste3&loc=en&ctr=Taiwan&rgp=88133cc HTTP/1.1\n\
Host: api.microsofttranslator.com\n\
User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:17.0) Gecko/20100101 Firefox/17.0\n\
Accept: */*\n\
Accept-Language: en-US,en;q=0.5\n\
Accept-Encoding: gzip, deflate\n\
Connection: keep-alive\n\
Referer: http://www.bing.com/translator/\n\
Cookie: SRCHD=MS=2641235&D=2615132&AF=NOFORM; SRCHUSR=AUTOREDIR=0&GEOVAR=&DOB=20121221; _FS=NU=1; _SS=SID=6673D34DC0EC4F4E8E5BE7A7BA3F9A69; _HOP=",
							url_encode(inouts), m_Transl.c_str());

		delete [] inouts;

		{
			FILE* f = fopen("Request.txt", "wb");
			if(f)
			{
				fwrite(httpheader, strlen(httpheader), 1, f);
				fclose(f);
			}
		}
    
		GetTcpSocket()->Send(httpheader, strlen(httpheader));
	
		HttpParser httpparser;
		int recvret=0;
		// 要確定資料流已經收完了
		do
		{
			byte_t bp[5120];
			memset(bp, 0, 5120);
			recvret = GetTcpSocket()->Recv(bp, 5120);
			if(recvret>0)
			{
			    httpparser.addBytes(bp, recvret);
			}
			else if(recvret<0)
			{
				return "";
			}
		}while(recvret>0);

		{
		FILE* f = fopen("TestA.txt", "wb");
		if(f)
		{
		fwrite(httpparser.getRawBuffer(), httpparser.getRawLength(), 1, f);
		fclose(f);
		}
		}


        int context_coding=0;
		const char* Context_Encoding = httpparser.getValue("content-encoding");
		if(Context_Encoding)
		{
			if(strcmp(Context_Encoding, "gzip")==0)
			{
				context_coding = 1;
			}
		}

		BYTE* abDst = NULL;
		const BYTE* bodybuf   = (const BYTE*)httpparser.getBody();
		int         contlen   = httpparser.getContentLength();
		if(context_coding == 1)
		{
			int bodylen   = httpparser.getBodyLength();
			contlen = IO::GZip::GetMaxUncompressedLen(bodybuf, bodylen);
			if(contlen<=0)
				contlen = httpparser.getContentLength()*10;
			abDst = new BYTE[contlen];
			memset(abDst, 0, contlen);
			size32_t retl = IO::GZip::UncompressBuffer(bodybuf, bodylen, abDst, contlen);
		}
		else
		{
			contlen = httpparser.getBodyLength();
			abDst = new BYTE[contlen];
			memset(abDst, 0, contlen);
			memcpy(abDst, bodybuf, contlen);
		}

		/*
		FILE* f = fopen("TestB.txt", "wb");
		if(f)
		{
			fwrite(abDst, contlen, 1, f);
			fclose(f);
		}
		*/

		std::string outstr;

		const char needmark[] = {"<textarea name=\"after\" id=\"after\">"};
		char* datab = strstr((char*)abDst, needmark);
		if(datab)
		{
			datab += strlen(needmark);
			char* datae = strstr((char*)datab, "</textarea>");
			(*datae)=0;
			outstr = datab;
			
			//FILE* f = fopen("TestC.txt", "ab");
			//if(f)
			//{
			//	fwrite(datab, strlen(datab), 1, f);
			//	fclose(f);
			//}
		}

		SAFE_DELETE_ARRAY(abDst);
		httpparser.release();

		Close();
		return outstr;
	}
}


