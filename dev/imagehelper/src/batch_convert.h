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
//
//
//
//
//
#ifndef BATCHCONVERT_H
#define BATCHCONVERT_H

#include <vector>

void BatchConvertElem(std::vector<const char*>* filenames);

typedef void ( _CALLBACK *OnStandElemFolder)(const char* ojbname, std::vector<const char*>& Iconlist, void* pData);

// 預設的存檔function
void SaveIconListAsElem(const char* ojbname, std::vector<const char*>& Iconlist, void* pData);

// 掃描格式化的資料夾
int  ScanStandFolderToElem(const char* folder, OnStandElemFolder func=SaveIconListAsElem, void* pData=NULL);

// 等於呼叫上面兩個
int  BatchConvertFolderToElem(const char* folder);


#endif