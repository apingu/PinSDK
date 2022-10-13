




#include "HKOServerDB.h"

bool HKOServerDB::Initialize(void){
	RecvTable("account");
	return true;
}

bool HKOServerDB::Login(char *username, char *password){
	//判斷table內有沒有這組usr,pwd
	int i;
	const char *p;
	
	i = SearchTableData(0,1,username);
	if(i < 0)
		return false;
	
	p = GetTableData(0,2,i);
	if(!p)
		return false;

	if(strcmp(password,p) != 0)
		return false;

	return true;
}
void HKOServerDB::Show(void){
	const char *p = GetTableData(0,2,2);

	if(p)
		printf("%s \n",p);
	else
		printf("error. \n");
}
