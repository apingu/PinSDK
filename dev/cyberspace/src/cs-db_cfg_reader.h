//
//
//
//
//  讀configure檔內 DB node
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////
#pragma once

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-stdlib.h>
#include <database/xdatabase.h>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-log_file_service.h"

namespace cyberspace
{
    namespace DBCfgReader
    {
        static int       g_dbcfg_outid = 0;
        static CsLogMode g_dbcfg_logmode;

        inline int GetMaxConnect( std::map<_cs_string, _cs_string> setting, const _cs_char_t* dbname )
        {
            _cs_string       logmessage;
            _cs_string       cfgstring;

            std::map<_cs_string, _cs_string>::iterator it;
            _pf_tstrprintf( cfgstring, ( "%s>>MaxConnect" ), dbname );
            it = setting.find( cfgstring.c_str() );
            if ( it != setting.end() )
            {
                return  _tstoi( it->second.c_str() );
            }
            return 0;
        }

        inline bool LoadCfg( std::map<_cs_string, _cs_string> setting, const _cs_char_t* dbname, int& maxtable, std::vector<XDatabase*>& database )
        {
            _cs_string       logmessage;
            _cs_string       cfgstring;

            int            db_max_connection = 0;
            _cs_string      db_name;
            _cs_string      db_hostname;
            _cs_string      db_username;
            _cs_string      db_password;
            _cs_string      db_charset;
            unsigned short db_port = 0;
            {
                std::map<_cs_string, _cs_string>::iterator it;
                _pf_tstrprintf( cfgstring, ( "%s>>MaxConnect" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_max_connection = _tstoi( it->second.c_str() );
                }
                else
                {
                    _pf_tstrprintf( logmessage, ( "||  Can't find < %s >\n" ), cfgstring.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>DBName" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_name = it->second;
                }
                else
                {
                    _pf_tstrprintf( logmessage, ( "||  Can't find < %s >\n" ), cfgstring.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>Hostname" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_hostname = it->second;
                }
                else
                {
                    _pf_tstrprintf( logmessage, ( "||  Can't find < %s >\n" ), cfgstring.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>Port" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_port = _tstoi( it->second.c_str() );
                }

                _pf_tstrprintf( cfgstring, ( "%s>>Username" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_username = it->second;
                }
                else
                {
                    _pf_tstrprintf( logmessage, ( "||  Can't find < %s >\n" ), cfgstring.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>Password" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_password = it->second;
                }
                else
                {
                    _pf_tstrprintf( logmessage, ( "||  Can't find < %s >\n" ), cfgstring.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>CharSet" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    db_charset = it->second;
                }

                _pf_tstrprintf( cfgstring, ( "%s>>MaxTable" ), dbname );
                it = setting.find( cfgstring.c_str() );
                if ( it != setting.end() )
                {
                    maxtable = atoi( it->second.c_str() );
                }
            }

            {
                if ( database.size() != db_max_connection )
                    return false;
            }

            {
                cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, ( "||  Search for %s database...." ), dbname );

                if ( database.empty() )
                {
                    _pf_tstrprintf( logmessage, ( "**  No database object on %s!!!\n" ), db_name.c_str() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    return false;
                }
                else
                {
                    int conncnt = 0;
                    if ( !db_charset.empty() )
                    {
                        _pf_tstrprintf( logmessage, ( "\n||    DBName = %s  Hostname = %s  Port = %d  CharSet = %s  " ), db_name.c_str(), db_hostname.c_str(), db_port, db_charset.c_str() );
                        cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    }
                    else
                    {
                        _pf_tstrprintf( logmessage, ( "\n||    DBName = %s  Hostname = %s  Port = %d  " ), db_name.c_str(), db_hostname.c_str(), db_port );
                        cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );
                    }

                    for ( uint_t i = 0; i < database.size(); i++ )
                    {
                        if ( database[ i ]->Connect( db_name.c_str(), db_hostname.c_str(), db_port, db_username.c_str(), db_password.c_str() ) )
                        {
                            conncnt++;
                        }

                        if ( !db_charset.empty() )
                        {
                            database[ i ]->SetCharSet( db_charset.c_str() );
                        }
                    }

                    _pf_tstrprintf( logmessage, ( "Connected = %d/%d\n" ), conncnt, database.size() );
                    cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, logmessage.c_str() );

                    if ( conncnt == database.size() )
                    {
                        cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, ( "||    ....successful\n" ) );
                    }
                    else
                    {
                        cyberspace::LogFileSerivce::OutputMessage( g_dbcfg_outid, g_dbcfg_logmode, ( "||    ....failure\n" ) );
                        return false;
                    }
                }
            }
            return true;
        }
    }
}
