#ifndef CSERPDBFUNCTION_H
#define CSERPDBFUNCTION_H

#include <assert.h>
#include "CsDef.h"
#include "CsErr.h"
#include <XDatabase.h>



namespace Cyberspace
{
 namespace ERP
 {
#define MYSQLDB       ((XDataBase*)mydb)
#define GET_DB_RESULT( r, col, row ) ( MYSQLDB->GetQueryResult( r, col, row ) )

  namespace DBPersonal
  {


  };

  namespace DBDiary
  {

  };

  namespace DBWorktime
  {

  };
 }
};



#endif