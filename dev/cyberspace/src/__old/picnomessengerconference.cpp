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
#include "PicnoMessengerConference.h"


namespace Picno
{
	//===============================================================
	//
	//
	//===============================================================	
	MessengerConference::MessengerConference( ConferenceId_t id )
	{
		m_Id = id;
	}

	//===============================================================
	//
	//
	//===============================================================	
	MessengerConference::~MessengerConference()
	{

	}

	//===============================================================
	//
	//
	//===============================================================	
	ConferenceId_t MessengerConference::GetId( void )
	{
		return m_Id;
	}

	//===============================================================
	//
	//
	//===============================================================
	void MessengerConference::JoinParticipant( userid_t uid )
	{
		TS_Lock lock(&m_utLock);
		m_Participants.insert( std::set< userid_t >::value_type(uid) );
	}

	//===============================================================
	//
	//
	//===============================================================
	void MessengerConference::ExitParticipant( userid_t uid )
	{
		TS_Lock lock(&m_utLock);
		m_Participants.erase( uid );
	}

	//===============================================================
	//
	//
	//===============================================================
	void MessengerConference::ForAllUserId( ForAllUserIdCallback func, void* pdata )
	{
		TS_Lock lock(&m_utLock);
		std::set< userid_t >::iterator iter = m_Participants.begin();
		while( iter!=m_Participants.end() )
		{
			(*func)( (*iter), pdata );
			iter++;
		}
	}

};