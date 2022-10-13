

#include "XMySqlDB.h"
#include "DataLock.h"

enum eDBServerState {
	DB_UNLINK = -1,
	DB_FILE_ERROR,
	DB_LINKING,
	DB_READY,
};
enum eDBDataState {
	USABILITY = 0,
	DB_DOWNLOAD,
	DB_UPLOAD,
	SERVICE_LOCK,
};

#define DB_NAME_SIZE	20
#define DB_PORT_SIZE	10
#define TABLE_NAME_SIZE	20
#define USR_NAME_SIZE	32
#define USR_PWD_SIZE	32

class ServerDatabase : public XMySqlDB{
protected:
	eDBServerState	m_state;
	unsigned char	m_db_name[DB_NAME_SIZE];
	unsigned char	m_db_port[DB_PORT_SIZE];
	unsigned char	m_table_name[TABLE_NAME_SIZE];
	unsigned char	m_usr_name[USR_NAME_SIZE];
	unsigned char	m_usr_pwd[USR_PWD_SIZE];

public:
	ServerDatabase(const char* filename){
		InitialingData();
		Create(filename);
	}
	ServerDatabase(){
		InitialingData();
		Create("config.xml");
	}

	void InitialingData(void){
		m_state = DB_UNLINK;
	}

	bool Create(const char* filename){
		if( LoadConfigure(filename) ){
			m_state = DB_FILE_ERROR;
			return false;
		}
		m_state = DB_LINKING;

		printf( "Link to %s DB server, %s table, port:%s, usr:'%s'<%s>.\n",
			m_db_name, m_table_name, m_db_port, m_usr_name, m_usr_pwd );

		return true;
	}

    bool LoadConfigure( const char* filename )
    {
		XMLNode xml;

		if(xml.LoadFromFile(filename))
		{
			XMLNode DBCfg = xml[_T("DATABASE")];

			if( DBCfg[_T("Hostname")].m_Text.length() < sizeof(m_db_name) )
				DBCfg[_T("Hostname")].m_Text	>> m_db_name;
			else
				return false;
				
			if( DBCfg[_T("Port")].m_Text.length() < sizeof(m_db_port) )
				DBCfg[_T("Port")].m_Text		>> m_db_port;
			else
				return false;
				
			if( DBCfg[_T("DBName")].m_Text.length() < sizeof(m_table_name) )
				DBCfg[_T("DBName")].m_Text		>> m_table_name;
			else
				return false;

			if( DBCfg[_T("Username")].m_Text.length() < sizeof(m_usr_name) )
				DBCfg[_T("Username")].m_Text	>> m_usr_name;
			else
				return false;

			if( DBCfg[_T("Password")].m_Text.length() < sizeof(m_usr_pwd) )
				DBCfg[_T("Password")].m_Text	>> m_usr_pwd;
			else
				return false;

			return true;
		}
		return false;
	}

    void Connect(void)
    {
		if( Connect(m_table_name, m_db_name, m_db_port, m_usr_name, m_usr_pwd) ){
		}
		else
		{
		}

        printf( "Cyber server start........\n" );
        if( TCPService::StartUp( AllowedPlayers, port ) != 1 )
		{
			printf( "failure....\n" );
			ShutDown();
			return;
		}
        printf( "Name : %s", GetLocalHostName( message, 255 ) );
        printf( "  Ip : %s", GetLocalAddress( message, 255 ) );
        printf( "  Port : %d\n", GetServicePort() );
        printf( "Max allowed connect = %d\n", GetAllowedPlayers() );

		//Open Database
		printf( "Search for Database.....", GetAllowedPlayers() );

		printf( "Cyber server start successful.\n" );
        OnInitialing();
    }

};