/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or
 * mailto:support@zona.net
 *
 * Filename: configreader.h.cpp
 * Original Author: Mark Harmon
 *
 * $History: $
 * 1.0 - Aug 15th release of demo code
 * 2.0 - Changed to use standard c libs, for compatibility
 ********************************************************************/
 //////////////////////////////////////////////////////////////////////
 // configreader.h: interface for the CConfigReader class.
 //
 //  Can be used two ways, for quick one-time requests then use the
 // ...Once methods which are static but slow if called repeatedly.
 // For reading many config values, call instanciate this class and
 // call non-static methods.
 //
 // CConfigReader expects the following format in the config file for
 // all data types:  name="value"
 //////////////////////////////////////////////////////////////////////

#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include <pf-def.h>
//#include "ZGlobal.h"
//#include <rcharse.h>

#ifdef _XBOX
#include <xtl.h>
#endif //_XBOX


class CConfigReader
{
    _mstring     filename_;
    _mbstablesi  content_;

    //FILE*                               file_;
    int          is_write_enabled_;


public:

    CConfigReader();
    virtual ~CConfigReader();

    bool                   OpenConfigFile( const char* fname );
    void                   CloseConfigFile( void );
    bool                   SaveConfigFile( const char* fname );

    /** caller is responisble for freeing memory. use delete [] */
        //  有區段 就會變成 section_name.name
    _mbsvector             GetString( const char* name );
    void                   SetString( const char* name, _mbsvector values );

    const char*            GetOneString( const char* name );
    void                   SetOneString( const char* name, const char* value );

    //int                WriteString( const char* name, const char* value );

    _mbstablesi::iterator  begin();
    _mbstablesi::iterator  end();


    /** caller is responisble for freeing memory. use delete [] */
    static char*           GetStringOnce( const char* fname, const char* name );

    //static std::map<std::string, std::string> GetOnce( const char* fname );
};

#endif


