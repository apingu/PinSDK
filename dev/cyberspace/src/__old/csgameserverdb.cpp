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
#include "CsGameServerDB.h"
///#include "CsGameServer.h"

namespace Cyberspace
{
	CsGameServerDB::CsGameServerDB(){
		m_state;
	}

	void CsGameServerDB::Initialing(void){

		//Query username and password from database
		Query("select * from account");
		m_table[login_table] = Query_result();

	}

	void CsGameServerDB::OnUpdateData(void){

	}

	u_char * CsGameServerDB::GetTable(int table, int column, int row){
		Row temp;
		Result::iterator i;

		i = m_table[login_table].begin();
		i += row;
		temp = *i;

		return (u_char*)temp[column].c_str();
	}

	bool CsGameServerDB::Login(char *username, char *password){
		Row row;
		Result::iterator i;

		if(!IsReady())
			return false;

		for (i = m_table[login_table].begin(); i != m_table[login_table].end(); i++) {
			row = *i;
			//strcpy(temp,res.names(1).c_str());取得欄位名稱
			if(strcmp(username,row["Username"].c_str()) != 0)
				continue;
			if(strcmp(password,row["Password"].c_str()) != 0)
				continue;

			return true;
		}
		return false;
	}


};//Cyberspace