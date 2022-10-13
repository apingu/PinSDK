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
#include "PicnoMessagerConference.h"


namespace Picno
{
	//===============================================================
	//
	//
	//===============================================================	
	MessagerConference::MessagerConference( ConferenceId_t id )
	{
		m_Id = id;
	}

	//===============================================================
	//
	//
	//===============================================================	
	MessagerConference::~MessagerConference()
	{

	}

	//===============================================================
	//
	//
	//===============================================================	
	ConferenceId_t MessagerConference::GetId( void )
	{
		return m_Id;
	}

	//===============================================================
	//
	//
	//===============================================================
	void MessagerConference::JoinParticipant( userid_t uid )
	{
		TS_Lock lock(&m_utLock);
		m_Participants.insert( std::set< userid_t >::value_type(uid) );
	}

	//===============================================================
	//
	//
	//===============================================================
	void MessagerConference::ExitParticipant( userid_t uid )
	{
		TS_Lock lock(&m_utLock);
		m_Participants.erase( uid );
	}

};