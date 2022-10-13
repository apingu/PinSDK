#ifndef CSVERIFY_H
#define CSVERIFY_H


#include "CsDef.h"
#include "IOServer.h"
//#include "Cyberspace_server.h"

namespace Cyberspace
{

    class VerifyServer : protected IO::TCPServer
	{
	public:

		VerifyServer(){};
		~VerifyServer(){};
	};
}

#endif //CSVERIFY_H

