#ifndef PICNOMESSENGERUSER_H
#define PICNOMESSENGERUSER_H

#include "PMSGDef.h"


namespace Picno
{

	class MessengerUser
	{
		struct friendinfo
		{
			userid_t uid;
		};

		T_Lock m_lock;
		std::map< userid_t, friendinfo > m_FriendList;

	public:

		MessengerUser( userid_t uid );
		~MessengerUser();

		void AddFriend( userid_t uid ){};
		void DelFriend( userid_t uid ){};
		bool FindFriend( userid_t uid ){ return true; }
		
	};




};


#endif