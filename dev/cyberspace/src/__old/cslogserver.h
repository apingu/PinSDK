#pragma once

#include "CsDef.h"
#include <database/XDatabase.h>


namespace Cyberspace
{
    class LogServer// : public UnitServer
    {
        std::vector<XDataBase*>   m_Database;

    public:

        LogServer();
        ~LogServer();

        const tchar_t*          ObjectName() const { return "LogService"; }

        virtual int             Shutdown( void );

        XDataBase*              GetLogDB( _IN _cs_sessionid_t id );

        size32_t                GetLogDBSize();

        void                    SetLogMessage( _IN _cs_sessionid_t id, _IN CsLogMode m, _IN const tchar_t* event, _IN tchar_t* date, _IN const tchar_t* format, _IN ... );

        virtual bool            OnLoadConfigure( std::map<_tstring, _tstring> setting ) = 0;

    protected:

        virtual XDataBase*      OnCreateLogDB( void );
        virtual void            OnDeleteLogDB( _IN XDataBase* db );

    };
}