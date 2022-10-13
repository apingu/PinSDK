//project file
/*
    To test the library, include "PfParser.h" from an application project
    and call PfParserTest().
    
    Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

extern "C" int ParserTest();

int ParserTest()
{
    return ++s_Test;
}