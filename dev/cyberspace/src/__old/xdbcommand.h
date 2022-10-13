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
#ifndef XDB_COMMAND_H
#define XDB_COMMAND_H

//--------------------------------------------------------
// Database Command Define
//--------------------------------------------------------
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select AccountId from account where Username = '%s' and Password = '%s'",usr,pwd
//#define TRY_CONNECTION	10
#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"SELECT AccountId FROM account WHERE Username = '"usr"' and Password = '"pwd"'"
//#define DB_COMMAND_CHECK_LOGIN(usr,pwd)		"select AccountId from account where Username = '"usr"' and Password = '"pwd"'"






#endif // XDATABASE_H