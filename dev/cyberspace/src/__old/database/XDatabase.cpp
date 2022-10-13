
#include "XDatabase.h"
#include <string.h>

void XDataBase::SetCharSet(const char* CharSet)
{
	m_CharSet= CharSet;
}


_tstring XDataBase::SafeEscapeString(const tchar_t* s)
{
	_tstring str;
	tchar_t* out = new tchar_t[strlen(s)*2];

	//tchar_t seps[]   = "\0\'\"\b\n\r\tz\\%_";
	int j=0;
	for(size32_t i = 0; i < strlen(s); i++)
	{
		switch(s[i])
		{
		case 0:
		case '\'':
		case '\"':
		case 'b':
		case 'n':
		case 'r':
		case 't':
		case 'z':
		case '\\':
		case '%':
		case '_':
			out[j]='\\';
			j++;
		default:
			out[j]=s[i];
			j++;
			break;
		}
	}
	str=out;
	delete [] out;  
	return str;
}
