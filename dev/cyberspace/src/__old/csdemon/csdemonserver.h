//   CsDemonServer
//            Master tool server
//
//
//
//
//
//
//
//
//
//
#ifndef CSDEMONSERVER_H
#define CSDEMONSERVER_H

#include <Cyberspace_server.h>


namespace Cyberspace
{

    class DemonServer : public Server
	{

	public:

        DemonServer();
		~DemonServer();

        const char*         ObjectName() const { return "DemonServer"; }
        void                DoInputCommand(const char* commandstring, uniqueid_t uid);

        int                 ShutDown();        

	private:

        int                 StartUp(uint_t allowplayers, unsigned short port);


        virtual DWORD       OnGetVersion();


		// on join one server in server group
		void                OnJoinServer( serverid_t sid );

		// on update one server in server group
		void                OnUpdateServer( serverid_t sid );

		// on exit one server in server group
		void                OnExitServer( serverid_t sid );

		//virtual void        OnServerInitialing( void );


        static int          DoGetCommandRequest(const ServerEvocator* evocator, const void* pData, unsigned int len);
        static int          DoSCSGetCommandFeedback(const SCS::ServerEvocator* evocator, const void* pData, unsigned int len);
       

	};
}

#endif