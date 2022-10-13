
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <mysql/mysql.h>
#include "database.h"



//database 物件的初始化
Database::Database()
{
	is_connected = false;	// No connection yet
	strcpy(error, "No connection");
}

//-------------------------------------------------------------------------------------------------

//database 物件的結束
Database::~Database()
{
	disconnect();		// Disconnect if connected to database
}

//-------------------------------------------------------------------------------------------------

//連結的狀態
int Database:: ConnectStatus()
{
	if (is_connected == false) return DB_CONNECTION_FAILS;
	return DB_CONNECTION_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

int Database::init()
{
	return DB_COMMANDEXEC_OK;
}

//-------------------------------------------------------------------------------------------------

char *Database::errorMessage()
{
	if ( is_connected == false) return error;
	return mysql_error(&mysql);
}

//-------------------------------------------------------------------------------------------------

int Database::connect(char *host, char *port, char *db)
{
	char *unused = port;

	if (mysql_connect(&mysql, host, NULL, NULL) == NULL) {
		strcpy(error, "Connect to database failed");
		return DB_ERROR;
	}

	if (mysql_select_db(&mysql, db))
	{
		mysql_close(&mysql);
		strcpy(error, "No such database");	
		return DB_ERROR;
	}

	unused++;			// Just to suppress a compiler warning
	connected = true;
	return DB_CONNECTION_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

void Database::disconnect()
{
	if (connected == false) return;
	mysql_close(&mysql);
	connected = false;
}

//-------------------------------------------------------------------------------------------------

int Database::reset()
{
	return DB_COMMANDEXEC_OK;
}

//-------------------------------------------------------------------------------------------------

DBResult *Database::exec(char *sqlFormat, ...)
{
	va_list ap;
	char sqlCommand[5000];

	va_start(ap, sqlFormat);
	vsnprintf(sqlCommand, 5000, sqlFormat, ap);
	va_end(ap);

	return new DBResult(&mysql, sqlCommand);
}

//-------------------------------------------------------------------------------------------------

void Database::exec(DBResult *res, char *sqlFormat, ...)
{
	va_list ap;
	char sqlCommand[5000];

	va_start(ap, sqlFormat);
	vsnprintf(sqlCommand, 5000, sqlFormat, ap);
	va_end(ap);

	res->init(&mysql, sqlCommand);
}


//-------------------------------------------------------------------------------------------------
// ------------------- Database result implementation ------------------
//-------------------------------------------------------------------------------------------------




DBResult::DBResult()
{
	result     = NULL;
	haveError  = false;
}

//-------------------------------------------------------------------------------------------------

DBResult::DBResult(MYSQL *mysql, char *query)
{
	result     = NULL;
	haveError  = false;
	init(mysql, query);
}

//-------------------------------------------------------------------------------------------------

DBResult::~DBResult()
{
	if (result != NULL)			// Free memory resources
		mysql_free_result(result);
}

//-------------------------------------------------------------------------------------------------

void DBResult::init(MYSQL *mysql, char *query)
{
	if (result != NULL) {                       // clear the result struct at first 
		mysql_free_result(result);
		result = NULL;
	}

	if (mysql_query(mysql, query) == 0) {		// query OK
		result = mysql_store_result(mysql);

		

		
		if (result == NULL) {			// empty query


			if (mysql_num_fields(mysql) == 0){
				haveError = false;
			}
			else{
				haveError = true;
			}

		} 
		else {
			haveError = false;
		}



	} 
	else {
		haveError  = true;
	}
}

//-------------------------------------------------------------------------------------------------

int DBResult::status()
{
	if (haveError)      return DB_ERROR;
	if (result == NULL) return DB_COMMAND_OK;
	return DB_TUPLES_OK;
}

//-------------------------------------------------------------------------------------------------

int DBResult::nrTuples()
{
	if (result == NULL) return 0;
	return mysql_num_rows(result);
}

//-------------------------------------------------------------------------------------------------

int DBResult::nrFields()
{
	if (result == NULL) return 0;
	return mysql_num_fields(result);
}

//-------------------------------------------------------------------------------------------------

char *DBResult::fieldName(int n)
{
	MYSQL_FIELD *field;

	if (result == NULL) return NULL;
	mysql_field_seek(result, n);
	field = mysql_fetch_field(result);
	if (field == NULL) return NULL;
	return field->name;
}

//-------------------------------------------------------------------------------------------------

int DBResult::fieldSize(int n)
{
	MYSQL_FIELD *field;

	if (result == NULL) return 0;
	mysql_field_seek(result, n);
	field = mysql_fetch_field(result);
	if (field == NULL) return 0;
	return field->length;
}

//-------------------------------------------------------------------------------------------------

int DBResult::fieldSize(char *name)
{
	int i;

	if (result == NULL) return 0;
	for (i = 0; i < nrFields(); i++)
		if (!strcmp(name, fieldName(i)))
			return fieldSize(i);
	return 0;
}

//-------------------------------------------------------------------------------------------------

void DBResult::seekTuple(int tuple)
{
	if (result == NULL) return;
	mysql_data_seek(result, tuple);
}

//-------------------------------------------------------------------------------------------------

char **DBResult::getTuple()
{
	MYSQL_ROW row;

	if (result == NULL) return NULL;
	row = mysql_fetch_row(result);
	return row;
}

//-------------------------------------------------------------------------------------------------

char **DBResult::getTuple(int tuple)
{
	seekTuple(tuple);
	return getTuple();
}
