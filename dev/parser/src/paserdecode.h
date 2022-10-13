#ifndef PASERDECODE_H
#define PASERDECODE_H


#include <vector>
#include <string>
#include <map>

class PaserDecode
{

	std::map< std::string, int > m_linemark;
	std::vector<std::string>  m_CommandLine;

public:

	PaserDecode();
	~PaserDecode();

	void    LoadConfigure( const char* fname ) ;

	void    Execute( unsigned int taskid=0, unsigned int startline=0 );





};




#endif // PASERDECODE_H