#ifndef _PData_H_
#define _PData_H_

#include <iostream.h>
#include <string>

class PDATA : public std::string
{
    int   Len ;
    char* data;

public:
    PDATA();
    PDATA(char* str);
    PDATA(std::string datastr);
    ~PDATA();

    void operator()(char* str);
    void operator()(std::string datastr);
    int  DataLen(char* str, char cstr);
};



#endif