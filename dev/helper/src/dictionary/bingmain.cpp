
// ****************************************************************************
// File: Main.cpp
// Desc: Bing/Microsoft Translator support
//
// ****************************************************************************
#include "stdafx.h"
#include "BingTranslate.h"

// Your Bing translator account ID and key/secret goes here
// Sign up for one: https://datamarket.azure.com/dataset/bing/microsofttranslator
#define MY_ID  "MyID"
#define MY_KEY "MySecretKeyString"

void main(int argc, char *argv[])
{
	// Can connect to the Bing Translator?
	BingTranslate::RESULT Result;
	if(BingTranslate::Translator::CanConnect(Result))
	{
		printf("Connected via: \"%s\".\n", Result.Info.c_str());

		// Instance translator
		BingTranslate::Translator BingX(MY_ID, MY_KEY, Result);
		if(Result.IsSuccess())
		{
			// Translate string			
			if(BingX.Translate("Hello world!", -1, "en", "de", Result))
			{
				printf("\nResult: \"%s\"\n", Result.Info.c_str());
			}
		}

		if(!Result.IsSuccess())
			printf("\nError:\n%s\n", Result.Info.c_str());
	}
	else
	{
		printf("Error! Can't connect to the Bing Translator.\n");
		printf("Reason: \"%s\".\n", Result.Info.c_str());
	}
}

