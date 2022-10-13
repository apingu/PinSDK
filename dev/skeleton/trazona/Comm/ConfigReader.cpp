/*********************************************************************
 * Copyright (c) 2001, Zona, Inc. All Rights Reserved
 * Please visit http://www.zona.net  or 
 * mailto:support@zona.net
 *
 * Filename: ConfigReader.cpp
 * Original Author: Mark Harmon
 * Notes:   
 * To Do:   
 * 
 * $History: $
 * 1.0 - Aug 15th release of demo code
 * 
 ********************************************************************/

#include "ConfigReader.h"
#include <stdio.h>
//#include <memory.h>
#include <string.h>
#include <stdlib.h>

#ifndef FALSE
#define FALSE 0l
#endif
#ifndef TRUE
#define TRUE 1l
#endif

#if defined(_PS2) || defined(_XBOX)
#include "file.h"
#define _tfopen  file_open
#define _tfseek  file_seek
#define _tftell  file_tell
#define _tfread  file_read
#define _tfwrite file_write
#define _tfclose file_close

#define _T(x)     x
#define strcpy   strcpy
#define strlen   strlen
#define strstr   strstr
#define sprintf sprintf
#define atoi     atoi
#endif

#if defined(_WINDOWS)
#define _tfopen  fopen
#define _tfseek  fseek
#define _tftell  ftell
#define _tfread  fread
#define _tfwrite fwrite
#define _tfclose fclose
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigReader::CConfigReader()
{
  m_file = NULL;
  bWriteEnabled = FALSE;
  memset(m_filename, 0 , 256);
}

CConfigReader::~CConfigReader()
{
  CloseConfigFile();
}


///////////////////////////////////////////////////////////////////////////////
int CConfigReader::OpenConfigFile(char *fname)
{
  if(fname == NULL) return FALSE;
  if(m_file != NULL) return TRUE;

  m_file = (FILE*) _tfopen(fname,("a+b"));

  if(m_file) bWriteEnabled = TRUE;
  else m_file = (FILE*) _tfopen(fname,("rb"));

//  m_file = fopen(fname,"rb");
  if(m_file == NULL) return FALSE;

  strcpy(m_filename, fname);

  return TRUE;
}

void CConfigReader::CloseConfigFile()
{
  if(m_file != NULL) _tfclose(m_file);

  m_file = NULL;
  memset(m_filename, 0 , 256);
}


///////////////////////////////////////////////////////////////////////////////
// FILE BUFFER
///////////////////////////////////////////////////////////////////////////////

char* CConfigReader::GetFileBufferOnce(char *fname, long *fsize)
{
  char *value;

  // open file
  if(fname == NULL) return false;
  FILE *file = (FILE*) _tfopen(fname, ("rb"));
  if(file == NULL) return NULL;

  // get value
  value = GetFileBuffer(fsize, file);

  // close file
  _tfclose(file);

  return value;
}

///////////////////////////////////////////////////////////////////////////////
char* CConfigReader::GetFileBuffer(long *fsize)
{
  return GetFileBuffer(fsize, m_file);
}

///////////////////////////////////////////////////////////////////////////////
// Assumes that file is in ascii format. The only way around this is to enforce
// some type of config file header information specifying the format of the 
// text in the file.
char* CConfigReader::GetFileBuffer(long *fsize, FILE *file)
{
  char *fileBuf;

  if(file == NULL) return NULL;

  _tfseek(file, 0, SEEK_END);
  *fsize = _tftell(file);
  _tfseek(file, 0, SEEK_SET);

  if(*fsize == 0) return NULL;

  fileBuf = new char[*fsize];
  if(!fileBuf) return NULL;

  if(!_tfread(fileBuf, sizeof(char), *fsize, file))
  {
    delete [] fileBuf;
    return NULL;
  }

#ifdef UNICODE
  // if this is unicode then convert text to wide char format
  *fsize = strlen((char*)fileBuf) + 1;
  char *wFileBuf = new char[*fsize];
  if(!wFileBuf) return NULL;

  sprintf(wFileBuf,_T("%S"),fileBuf);

  delete [] fileBuf;
  return wFileBuf;
#else
  return fileBuf;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// STRINGS
///////////////////////////////////////////////////////////////////////////////

char* CConfigReader::GetString(char *name)
{
  if(m_file == NULL) return NULL;
  return GetStringValue(name,m_file);
}


///////////////////////////////////////////////////////////////////////////////
// reads a named value from config file
// caller must free buffer used by char* by calling delete [] char*
char* CConfigReader::GetStringValue(char* name, FILE *file, long* strOffset)
{
  // read file
  char *tempValue = NULL;
  char *tempValueEnd = NULL;
  char *value = NULL;
  char *fileBuf, *pFileBuf;
  long fsize;
  long strSize;
  long nameLength = strlen(name);

  pFileBuf = fileBuf = GetFileBuffer(&fsize, file);
  if(!fileBuf) return NULL;

  do
  {
    // find name
    pFileBuf = strstr(pFileBuf, name);
    if(!pFileBuf)
    {
      delete [] fileBuf;
      return NULL;
    }
    
    // make sure name starts on new line and that next character is '=' 
    if((pFileBuf == fileBuf || pFileBuf[-1] == ('\r') || 
       pFileBuf[-1] == ('\n')) && pFileBuf[nameLength] == ('='))
    {
      tempValue = pFileBuf;
    }
    else pFileBuf++;

  }while(!tempValue);

  // store offset to name/value if necessary
  if(strOffset != NULL) *strOffset = tempValue - fileBuf - 2;

  // get value
  tempValue = strstr(tempValue,("\""));
  tempValue++; // jump passed quotes
  tempValueEnd = tempValue;
  while(*tempValueEnd != '\"') tempValueEnd++; // find end

  // allocate memory for char* and copy value
  strSize = (tempValueEnd - tempValue) + 1;  // +1 for null terminating 0
  value = new char[strSize];
  memset(value,0,strSize * sizeof(char));
  memcpy(value,tempValue, (strSize-1) * sizeof(char));

  // used for writing functionality. sets file pointer to point to
  // end of name/value string
  _tfseek(file, (tempValueEnd-fileBuf+1), SEEK_SET);

  delete [] fileBuf;

  return value;
}

///////////////////////////////////////////////////////////////////////////////
// automatically opens and closes file
char* CConfigReader::GetStringOnce(char* fname, char* name)
{
  char* value;

  // open file
  if(fname == NULL) return false;
  FILE *file = (FILE*) _tfopen(fname,("rb"));
  if(file == NULL)  return NULL;

  // get value
  value = GetStringValue(name,file);

  // close file
  _tfclose(file);

  return value;
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::WriteString(char *name, char *value, FILE *file, char *filename)
{
  long fsize;
  long start,end;
  long valueLen;
  char* result=NULL;
  char* nameValue=NULL;

  // create new name/value string for config file
  valueLen = strlen(value);
  nameValue = new char[strlen(name)+valueLen+12]; // + 12 to add other formatting characters
  sprintf(nameValue,("\r\n%s=\"%s\""),name,value);

  // check to see if entry already exists
  result = GetStringValue(name, file, &start);
  if(result != NULL)
  {
    // don't need this anymore
    delete [] result;

    // name/value pair already exists, mark the end pos of name/value in file
    end = _tftell(file); 
    
    // get file's size
    _tfseek(file, 0, SEEK_END);
    fsize = _tftell(file);

    // reposition file pointer
    _tfseek(file, end, SEEK_SET);

    if(fsize == end)
    { 
      // string is at end of file
      long newsize = fsize - (fsize - start);

      //read contents of file, up to start, into temp buffer 
      char* fbuf = new char[newsize+1];
      _tfread(fbuf, newsize, 1, file);

      // close file
      _tfclose(file);

      // reopen same file deleting original contents
      _tfopen(filename, ("w+b"));

      // write back buffer, leaving out the old name/value we are going to replace
      _tfwrite(fbuf, newsize, 1, file);

      delete [] fbuf;
    }
    else
    {
      // string is in middle of file
      long newsize = fsize - (fsize - start);

      // read entire contents of current config file
      char* fbuf = new char[fsize+1];
      _tfread(fbuf, fsize, 1, file);

      // close file
      _tfclose(file);

      // reopen same file deleting original contents
      _tfopen(filename, ("w+b"));

      // write file up to point where name/value was found
      _tfwrite(fbuf, newsize, 1, file);

      // write new name/value pair
      _tfwrite(nameValue, strlen(nameValue)*sizeof(char), 1, file);

      // write file from end of where name/value was found to end
      _tfwrite(fbuf, fsize - end, 1, file);
      
      delete [] nameValue;
      delete [] fbuf;

      return TRUE;
    }
  }

  // string does not exist or was removed above
  _tfseek(file, 0, SEEK_END);
  _tfwrite(nameValue, strlen(nameValue)*sizeof(char), 1, file);

  delete [] nameValue;

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::WriteString(char* name, char* value)
{
  if(!m_file || !bWriteEnabled) return FALSE;
  return WriteString(name, value, m_file, m_filename);
}

///////////////////////////////////////////////////////////////////////////////
// FLOATS
///////////////////////////////////////////////////////////////////////////////

float CConfigReader::GetFloatValue(char* name, FILE *file, float defaultVal)
{
#if 0
  char* strValue = GetStringValue(name, file);
  if(!strValue) return defaultVal;

  float value = (float)atof(strValue);

  delete [] strValue;
  return value;
#else
  return defaultVal;
#endif
}

///////////////////////////////////////////////////////////////////////////////
float CConfigReader::GetFloat(char* name, float defaultVal)
{
  if(m_file == NULL) return NULL;
  return GetFloatValue(name, m_file,defaultVal);
}

///////////////////////////////////////////////////////////////////////////////
float CConfigReader::GetFloatOnce(char* fname, char* name, float defaultVal)
{
  float value;

  // open file
  if(fname == NULL) return false;

  FILE *file = (FILE*) _tfopen(fname, ("a+b"));
  if(file == NULL) return defaultVal;

  // get value
  value = GetFloatValue(name, file, defaultVal);

  // close file
  _tfclose(file);

  return value;
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::WriteFloat(char* name, float value)
{
  char strFloat[64];

  if(!m_file || !bWriteEnabled) return FALSE;
  
  sprintf(strFloat, ("%f"),value);
  return WriteString(name, strFloat, m_file, m_filename);
}


///////////////////////////////////////////////////////////////////////////////
// INTEGERS
///////////////////////////////////////////////////////////////////////////////

int CConfigReader::GetIntValue(char* name, FILE *file, int defaultVal)
{
  char* strValue = GetStringValue(name, file);
  if(!strValue) return defaultVal;

  int value = atoi(strValue);

  delete [] strValue;
  return value;
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::GetInt(char* name, int defaultVal)
{
  if(m_file == NULL) return defaultVal;
  return GetIntValue(name, m_file, defaultVal);
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::GetIntOnce(char* fname, char* name, int defaultVal)
{
  int value;

  if(fname == NULL) return false;

  // open file
  FILE *file = (FILE*) _tfopen(fname, ("a+b"));
  if(file == NULL) return defaultVal;

  // get value
  value = GetIntValue(name, file, defaultVal);

  // close file
  _tfclose(file);

  return value;
}

///////////////////////////////////////////////////////////////////////////////
int CConfigReader::WriteInt(char* name, int value)
{
  char strInt[64];

  if(!m_file || !bWriteEnabled) return FALSE;

  sprintf(strInt,("%d"),value);

  return WriteString(name, strInt, m_file, m_filename);
}

///////////////////////////////////////////////////////////////////////////////
