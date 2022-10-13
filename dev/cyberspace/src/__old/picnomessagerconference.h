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
	class MessagerConference
	{
		friend class MessagerServer;


		ConferenceId_t       m_Id;
		std::set< userid_t > m_Participants;
		T_Lock               m_utLock;

			
	public:

		MessagerConference( ConferenceId_t id );
		~MessagerConference();


		ConferenceId_t GetId( void );
		bool           Empty( void ){ return m_Participants.empty(); }

	private:

		void JoinParticipant( userid_t uid );
		void ExitParticipant( userid_t uid );

	};

};


#endif