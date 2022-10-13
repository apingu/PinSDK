// Server.cpp : Defines the entry point for the console application.
//

#ifndef HKO_SERVER_DB_CPP
#define HKO_SERVER_DB_CPP

#include "XMySqlDB.h"

class HKOServerDB : public XMySqlDB
{
public:
	bool Initialize(void);

	bool Login(char *username, char *password);
	void Show(void);
};

#endif
