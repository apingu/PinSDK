#ifndef _PEP_H_
#define _PEP_H_

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include <vector>

#include "CImage.h"


class PEP
{
	char Msg[50];

	
public :

	int  m_VedioMask;
	int  m_StartOffset;
	BYTE *m_WriteData;
	char Title[4];
	//char m_Destext[_MAX_EXT];

	PEP();
    ~PEP();
	
	int   Save(const char *destfilepath, std::vector<HPINMAP*> PicList,     int Group1Count, ...);  //讀入檔案
	int   Save(const char *destfilepath, std::vector<const char*> FilePath, int Group1Count, ...);  //讀入檔案

	//int  loadPep(const char *pathname, WORD **StatusList, std::vector<void*> *PicList);            //讀入Pop圖形包裝檔
    //int  loadPep(BYTE *pepfile       , WORD **StatusList, std::vector<void*> *PicList);            //讀入Pop圖形包裝檔
	int   loadPep(const char *pathname, WORD **StatusList, std::vector<BYTE*> *PicList);            //讀入Pop圖形包裝檔
	int   loadPep(BYTE *pepfile       , WORD **StatusList, std::vector<BYTE*> *PicList);            //讀入Pop圖形包裝檔
	int   loadPep(const char *pathname, WORD **StatusList, std::vector<HPINMAP*> *PicList);         //讀入Pop圖形包裝檔
	int   loadPep(BYTE *pepfile       , WORD **StatusList, std::vector<HPINMAP*> *PicList);         //讀入Pop圖形包裝檔
	BYTE* loadUnit(const char *pathname, int number, int offset = 0);         //讀入Pop圖形包裝檔
	BYTE* loadUnit(FILE *f, int number, int offset = 0);                      //讀入Pop圖形包裝檔


};
#endif