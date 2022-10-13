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
//
//
//
//
//
#ifndef PICNOCONFERENCE_H
#define PICNOCONFERENCE_H

#include <set>
#include <T_Lock.h>
#include "PMSGDef.h"

namespace Picno
{

	typedef void (*ForAllUserIdCallback)( userid_t uid, void* pdata );

	class MessengerConference
	{
		friend class MessengerServer;

		
		ConferenceId_t       m_Id;
		std::set< userid_t > m_Participants;
		T_Lock               m_utLock;

			
	public:

		MessengerConference( ConferenceId_t id );
		~MessengerConference();


		ConferenceId_t GetId( void );
		bool           Empty( void ){ return m_Participants.empty(); }
		void           ForAllUserId( ForAllUserIdCallback func, void* pdata );

	private:

		void JoinParticipant( userid_t uid );
		void ExitParticipant( userid_t uid );

	};

};


#endif