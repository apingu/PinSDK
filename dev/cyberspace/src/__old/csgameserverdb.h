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
#ifndef CSGAMESERVER_DB_H
#define CSGAMESERVER_DB_H

#include "ServerDB.h"
///#include "CsGameServer.h"
enum eDatabaseTable{
	login_table = 0,
	end_table
};
enum eLoginTable{
	user_name = 0,
	pass_word = 0,
	end_login
};

namespace Cyberspace
{
	class CsGameServerDB : public ServerDB
	{
	protected:
		DBResult m_table[end_table];

		virtual void Initialing(void);
		virtual void OnUpdateData(void);

	public:
		CsGameServerDB();
		~CsGameServerDB(){};

		u_char * GetTable(int table, int column, int row);

		bool Login(char *username, char *password);

	};


};//Cyberspace

#endif