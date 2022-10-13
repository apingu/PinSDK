/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ConfigReader.h.cpp
 * Original Author: Mark Harmon
 * 
 * $History: $
 * 1.0 - Aug 15th release of demo code
 * 2.0 - Changed to use standard c libs, for compatibility
 ********************************************************************/
//////////////////////////////////////////////////////////////////////
// ConfigReader.h: interface for the CConfigReader class.
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
//#include "ZGlobal.h"
//#include <rcharse.h>

#ifdef _XBOX
#include <xtl.h>
#endif //_XBOX


class CConfigReader  
{
public:
	void CloseConfigFile(void);
	int OpenConfigFile(char* fname);

  /** caller is responisble for freeing memory. use delete [] */
	char* GetString(char* name);
  /** caller is responisble for freeing memory. use delete [] */
  static char* GetStringOnce(char* fname, char* name);
	int WriteString(char* name, char* value);

  float GetFloat(char* name, float defaultVal=0);
  static float GetFloatOnce(char* fname, char* name, float defaultVal=0);
	int WriteFloat(char* name, float value);

  int GetInt(char* name, int defaultVal=0);
  static int GetIntOnce(char* fname, char* name, int defaultVal=0);
	int WriteInt(char* name, int value);

  /** caller is responisble for freeing memory. use delete [] */
  char* GetFileBuffer(long *fsize);
  static char* GetFileBufferOnce(char *fname, long *fsize);
  static char* GetFileBuffer(long *fsize, FILE *file);
  
  CConfigReader();
	virtual ~CConfigReader();
protected:
	static char* GetStringValue(char* name, FILE *file, long* strOffset = NULL);
	static float GetFloatValue(char* name, FILE *file, float defaultVal=0);
	static int GetIntValue(char* name, FILE *file, int defaultVal=0);
#if defined(_WINDOWS)
	static WriteString(char* name, char* value, FILE *file, char *filename);
#else
	static int WriteString(char* name, char* value, FILE *file, char *filename);
#endif

  FILE *m_file;
  char m_filename[256];
  int bWriteEnabled;
};

#endif 

