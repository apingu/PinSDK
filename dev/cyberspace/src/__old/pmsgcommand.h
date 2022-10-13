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
#ifndef PMSGCOMMAND_H
#define PMSGCOMMAND_H

#include "CsDef.h"
#include "CsProtocol.h"
#include "PMSGDef.h"

#define CsID_MESSAGER_BASE   _CsMAXCHANNEL+5000
//#define SEND_MSG_MESSAGE( s, m )  CsCore::CsSendPacket( s, CsINVALID_ID, (m)->Header.CommandId, (m)->Buff, (m)->Header.CommandLen )


enum PMSGCOMMAND { //PMSG_C_S_LOGIN_REQUEST  = CsID_MESSAGER_BASE+1,  //要求登入
                   //PMSG_S_C_LOGIN_FEEDBACK,
			       PMSG_C_S_LOGOUT_REQUEST    = CsID_MESSAGER_BASE+1,
			       PMSG_S_C_LOGOUT_REQUEST,

				   PMSG_S_C_ENUMFRIENDID_REQUEST,   // reply to enum all friends,

   				   
				   //
   				   PMSG_S_C_UPDATE_MYNICKNAME_REQUEST,
				   PMSG_S_C_UPDATENICKNAME_REQUEST,


				   PMSG_C_S_GET_USERPICTURE_REQUEST,
				   //
				   PMSG_S_C_UPDATE_PICTURE_REQUEST,            // 要求更換其他使用者照片

				   //
				   PMSG_S_C_UPDATE_MYSTATE_REQUEST,
				   PMSG_S_C_UPDATESTATE_REQUEST,

				   //
				   PMSG_S_C_UPDATE_MYEVENT_REQUEST,
				   PMSG_S_C_UPDATEEVENT_REQUEST,

				   //		   
   				   PMSG_C_S_UPDATE_MYNICKNAME_REQUEST,
			       PMSG_C_S_UPDATE_MYPICTURE_REQUEST,
				   PMSG_C_S_UPDATE_MYSTATE_REQUEST,
	   

				   PMPG_C_S_MAKEFRIEND_REQUEST,               // ask to make friend with someone
				   PMPG_S_C_MAKEFRIEND_REQUEST,               // ask to make friend with someone
				   PMPG_C_S_MAKEFRIEND_FEEDBACK,              // reply to make friend with someone
				   PMPG_S_C_MAKEFRIEND_FEEDBACK,              // reply to make friend with someone

				   PMPG_C_S_DELETEFRIEND_REQUEST,             // ask to make friend with someone

   				   PMSG_C_S_SHOCKUSER_REQUEST,
				   PMSG_S_C_SHOCKUSER_REQUEST,

				   // conference
				   PMSG_C_S_CREATE_CONFERENCEWITH_REQUEST,
				   PMSG_S_C_CREATE_CONFERENCEWITH_FEEDBACK,

				   PMSG_C_S_INVITE_CONFERENCE_REQUEST,               // 邀請其他使用者加入對話
                   PMSG_S_C_INVITE_CONFERENCE_REQUEST,               // 被其他使用者邀請加入對話
				   PMSG_C_S_INVITE_CONFERENCE_FEEDBACK,              // 接受/拒絕其他使用者的對話邀請
				   PMSG_S_C_INVITE_CONFERENCE_FEEDBACK,              // 接受/拒絕其他使用者的對話邀請
				   PMSG_C_S_LEAVE_CONFERENCE_REQUEST,
				   PMSG_S_C_LEAVE_CONFERENCE_REQUEST,


   				   PMSG_C_S_CONFERENCEMESSAGE_REQUEST,    // client送訊息給其他人
				   PMSG_S_C_CONFERENCEMESSAGE_REQUEST,    // server轉送訊息給其他人

				   PMSG_C_S_CREATE_USER,
				   PMSG_S_C_CREATE_USER,

				   PMSG_C_S_DELETE_USER,
				   PMSG_S_C_DELETE_USER,
};



///////////////////////////////////////////////////////////////////////////////////////
// Login request

namespace PMSGLoginMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
						          const char* username,
								  const char* password )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%s",
									        username,
									        password );
		}

		inline size_t GetFeedback( char* buff,
			                       short& result )
		{

			return CsFactory::SplitArgument( buff,
									         "%h",
									         &result );
		}


	};

	namespace Server
	{
		inline size_t SetFeedback( char* buff, short result )
		{
			return CsFactory::MakeArgument( buff,
									        "%h",
									        result );

		}


		inline size_t GetRequest( char* buff, char* username, char* password )
		{
			return CsFactory::SplitArgument( buff,
				                             "%s%s",
									         username,
									         password );
		}
	};
};



///////////////////////////////////////////////////////////////////////////////////////
// Logout request
namespace PMSGLogoutMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
								  userid_t uid )
		{
			return CsFactory::MakeArgument( buff,
									        "%d",
									        uid );
		}
		
	    inline size_t GetRequest( char* buff,
								  userid_t& uid )
		{
			return CsFactory::SplitArgument( buff,
									         "%d",
									         &uid );
		}

	};

	namespace Server
	{
		inline size_t GetRequest( char* buff,
								  userid_t& uid )
		{
			return CsFactory::SplitArgument( buff,
									         "%d",
									         &uid );
		}
		
		inline size_t SetRequest( char* buff,
								  userid_t uid )
		{
			return CsFactory::MakeArgument( buff,
									         "%d",
									         uid );
		}
	};

};


///////////////////////////////////////////////////////////////////////////////////////
//

namespace PMSGEnumFriendIdMessage
{
	namespace Client
	{
		inline size_t GetRequest( char* buff,
			                      userid_t& fid )
		{
			return CsFactory::SplitArgument( buff,
									        "%d",
											&fid );
		}
	};

	namespace Server
	{
		inline size_t SetRequest( char* buff,
			                      userid_t fid )
		{
			return CsFactory::MakeArgument( buff,
									        "%d",
											fid );
		}
	};
}



///////////////////////////////////////////////////////////////////////////////////////
// update nickname
namespace PMSGUpdateNicknameMessage
{
	namespace Client
	{
		// PMSG_C_S_UPDATEMYNICKNAME_REQUEST
		inline size_t SetRequest( char* buff,
			                      userid_t id,
								  const char* nickname )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%s",
											id,
									        nickname );
		}

		// PMPG_S_C_UPDATEFRIENDNICKNAME_REQUEST
		inline size_t GetRequest( char* buff,
                                  userid_t& id,
								  char* nickname )
		{
			return CsFactory::SplitArgument( buff,
									         "%d%s",
											 &id,
									         nickname );
		}
	};

	namespace Server
	{
		// PMSG_C_S_UPDATEMYNICKNAME_REQUEST
		inline size_t GetRequest( char* buff,
                                  userid_t& id,
								  char* nickname )
		{
			return CsFactory::SplitArgument( buff,
									         "%d%s",
											 &id,
									         nickname );
		}

		// 通知他的朋友 暱稱已改變
		// PMPG_S_C_UPDATEFRIENDNICKNAME_REQUEST
		inline size_t SetRequest( char* buff,
			                      userid_t id,
								  const char* nickname )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%s",
											id,
									        nickname );
		}
	};
};

///////////////////////////////////////////////////////////////////////////////////////
// update state
namespace PMSGUpdateStateMessage
{
	namespace Client
	{

		inline size_t SetRequest( char* buff,
			                      userid_t fid,
								  short state )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%h",
											fid,
											state );
		}

		inline size_t GetRequest( char* buff,
			                      userid_t& fid,
							      short& state )
		{
			return CsFactory::SplitArgument( buff,
									        "%d%h",
											&fid,
											&state );
		}
	};

	namespace Server
	{
		inline size_t SetRequest( char* buff,
			                      userid_t fid,
								  short state )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%h",
											fid,
											state );
		}

		inline size_t GetRequest( char* buff,
		                          userid_t& fid,
							      short& state )
		{
			return CsFactory::SplitArgument( buff,
									        "%d%h",
											&fid,
											&state );
		}
	};
};



///////////////////////////////////////////////////////////////////////////////////////
// update event
namespace PMSGUpdateEventMessage
{
	namespace Client
	{
		inline size_t GetRequest( char* buff,
			                      userid_t& fid,
							      short& event )
		{
			return CsFactory::SplitArgument( buff,
									        "%d%h",
											&fid,
											&event );
		}
	};

	namespace Server
	{
		inline size_t SetRequest( char* buff,
			                      userid_t fid,
								  short event )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%h",
											fid,
											event );
		}
	};
};

///////////////////////////////////////////////////////////////////////////////////////
// update picture
// int		from_id;	// 傳送者 ID(必定單獨一人,>0)
// int		to_id;	    // 接收者id
// short	Piclen;	    // 名稱之後接訊息(>0)
// char	Pic[1];      	// 訊息 buffer(name+msg)
// inline int size(){	return sizeof(SMCMessengerPicture)+Piclen;	}
namespace PMSGUpdatePictureMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
			                      userid_t uid, 
								  void* pic, size_t len )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%v", uid,
											pic, len );
		}
	
	};

	namespace Server
	{
		 inline void GetRequest( char* buff,
                                 userid_t& uid,
							     void* &pic, size_t& len )
		{
	        char* buf=buff;
			uid = (*(userid_t*)buf);
			buf+=sizeof(userid_t);
			len = (*(unsigned short*)buf);
			buf+=sizeof(unsigned short);
			pic = (void*)(buf);
			return;
		}


	};
};


///////////////////////////////////////////////////////////////////////////////////////
///
namespace PMSGGetPictureMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
			                      userid_t uid )
		{
			return CsFactory::MakeArgument( buff,
									        "%d", uid );
		}

		inline void GetFeedback( char* buff,
			                     userid_t& uid, 
							     void* &pic, size_t& len )
		{
	        char* buf=buff;
			uid = (*(userid_t*)buf);
			buf+=sizeof(userid_t);
			len = (*(unsigned short*)buf);
			buf+=sizeof(unsigned short);
			pic = (void*)(buf);

			return;
		}
	
	};

	namespace Server
	{
		 inline void GetRequest( char* buff,
                                 userid_t& uid )
		{
			CsFactory::SplitArgument( buff,
							         "%d", &uid );
			return;
		}


		inline size_t SetFeedback( char* buff,
			                       userid_t uid, 
								   void* pic, size_t len )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%v", uid,
											pic, len );
		}

	};
};




///////////////////////////////////////////////////////////////////////////////////////
// shock friend's widget
namespace PMSGShockFriendMessage
{
	namespace Client
	{
		size_t SetRequest( char* buff, ConferenceId_t cid, userid_t from )
		{
			return CsFactory::MakeArgument( buff,
									        "%l%d",
											cid,
											from );
		}

		void GetRequest( char* buff, ConferenceId_t &cid, userid_t &from )
		{
			CsFactory::SplitArgument( buff,
							          "%l%d",
									  &cid,
									  &from );
			return;
		}

	};

	namespace Server
	{
		size_t SetRequest( char* buff, ConferenceId_t cid, userid_t from )
		{
			return CsFactory::MakeArgument( buff,
									        "%l%d",
											cid,
											from );
		}

		void GetRequest( char* buff, ConferenceId_t &cid, userid_t &from )
		{
			CsFactory::SplitArgument( buff,
							          "%l%d",
									  &cid,
									  &from );
			return;
		}
	};
};

///////////////////////////////////////////////////////////////////////////////////////
// chat
namespace PMSGChatCreateConferenceWith
{
	namespace Client
	{
		inline size_t SetRequest( char* buff, userid_t uid )
		{
			return CsFactory::MakeArgument( buff, "%d", uid );
		}

		inline void GetFeedback( char* buff,
			                     ConferenceId_t& cid,
								 userid_t& uid, short &result )
		{
			CsFactory::SplitArgument( buff, "%l%d%h", &cid, &uid, &result );
			return;
		}
	};

	namespace Server
	{
		inline void GetRequest( char* buff,
			                    userid_t& uid )
		{
			CsFactory::SplitArgument( buff, "%d", &uid );
			return;
		}

		inline size_t SetFeedback( char* buff,
			                       ConferenceId_t cid,
								   userid_t uid, short result )
		{
			return CsFactory::MakeArgument( buff, "%l%d%h", cid, uid, result );
		}

	};
};


namespace PMSGChatLeaveConference
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
			                      ConferenceId_t cid )
		{
			return CsFactory::MakeArgument( buff, "%l", cid );
		}

		inline void GetRequest( char* buff,
			                    ConferenceId_t& cid, userid_t& uid )
		{
			CsFactory::SplitArgument( buff, "%l%d", &cid, &uid );
			return;
		}
	};

	namespace Server
	{
		inline void GetRequest( char* buff,
			                    ConferenceId_t& cid )
		{
			CsFactory::SplitArgument( buff, "%l", &cid );
			return;
		}

		inline size_t SetRequest( char* buff,
			                      ConferenceId_t cid, userid_t uid )
		{
			return CsFactory::MakeArgument( buff, "%l%d", cid, uid );
		}

	};
};


namespace PMSGChatInviteConference
{
	namespace Client
	{
		inline size_t SetRequest( char* buff, ConferenceId_t cid, userid_t with )
		{
			return CsFactory::MakeArgument( buff, "%l%d", cid, with );
		}

		inline void GetRequest( char* buff, ConferenceId_t& cid, userid_t& from )
		{
			CsFactory::SplitArgument( buff, "%l%d", &cid, &from );
			return;
		}

		inline size_t SetFeedback( char* buff, ConferenceId_t cid, userid_t from, short reply )
		{
			return CsFactory::MakeArgument( buff, "%l%d%h", cid, from, reply );
		}

		inline void GetFeedback( char* buff, ConferenceId_t& cid, userid_t& with, char* nickname, short& reply )
		{
			CsFactory::SplitArgument( buff, "%l%d%s%h", &cid, &with, nickname, &reply );
			return;
		}
	};

	namespace Server
	{
		inline void GetRequest( char* buff, ConferenceId_t& cid, userid_t& with )
		{
			CsFactory::SplitArgument( buff, "%l%d", &cid, &with );
			return;
		}

		inline size_t SetRequest( char* buff, ConferenceId_t cid, userid_t from )
		{
			return CsFactory::MakeArgument( buff, "%l%d", cid, from );
		}

		inline void GetFeedback( char* buff, ConferenceId_t& cid, userid_t& from, short& reply )
		{
			CsFactory::SplitArgument( buff, "%l%d%h", &cid, &from, &reply );
			return;
		}

		inline size_t SetFeedback( char* buff, ConferenceId_t cid, userid_t with, const char* nickname, short reply )
		{
			return CsFactory::MakeArgument( buff, "%l%d%s%h", cid, with, nickname, reply );
		}

	};
};

namespace PMSGChatSendMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff, ConferenceId_t cid, const char* msg )
		{
			return CsFactory::MakeArgument( buff, "%l%s", cid, msg );
		}


		inline size_t GetRequest( char* buff, ConferenceId_t& cid, userid_t& from, char* msg )
		{
			return CsFactory::SplitArgument( buff, "%l%d%s", &cid, &from, msg );
		}

	};


	namespace Server
	{
		inline void GetRequest( char* buff, ConferenceId_t& cid, char* msg )
		{
			CsFactory::SplitArgument( buff, "%l%ss", &cid, msg );
			return;
		}

		inline size_t SetRequest( char* buff, ConferenceId_t cid, userid_t from, const char* msg )
		{
			return CsFactory::MakeArgument( buff, "%l%d%s", cid, from, msg );
		}
	};

//	namespace Client
//	{
//		inline size_t SetRequest( char* buff, ConferenceId_t cid, const char* msg, DWORD color, int size, const char* type )
//		{
//			return CsFactory::MakeArgument( buff, "%l%s%d%d%s", cid, msg, color, size, type );
//		}
//
//
//		inline size_t GetRequest( char* buff, ConferenceId_t& cid, userid_t& from, char* msg, DWORD& color, int& size, char* type )
//		{
//			return CsFactory::SplitArgument( buff, "%l%d%s%d%d%s", &cid, &from, msg, color, size, type );
//		}
//
//	};
//
//
//	namespace Server
//	{
//		inline void GetRequest( char* buff, ConferenceId_t& cid, char* msg, DWORD& color, int& size, char* type )
//		{
//			CsFactory::SplitArgument( buff, "%l%s%d%d%s", &cid, msg, &color, &size, type );
//			return;
//		}
//
//		inline size_t SetRequest( char* buff, ConferenceId_t cid, userid_t from, const char* msg, DWORD color, int size, char* type )
//		{
//			return CsFactory::MakeArgument( buff, "%l%d%s%d%d%s", cid, from, msg, color, size, type );
//		}
//	};

}


///////////////////////////////////////////////////////////////////////////////////////
// make friend
namespace PMSGMakefriendMessage
{
	namespace Client
	{
		// PMPG_C_S_MAKEFRIEND_REQUEST
		inline size_t SetRequest( char* buff,
								  userid_t tid )
		{
			return CsFactory::MakeArgument( buff,
									        "%d",
									        tid );
		}

		inline size_t GetRequest( char* buff,
			                      char* nickname,
			                      userid_t& fid )

		{
			return CsFactory::SplitArgument( buff,
									         "%s%d",
											 nickname,
											 &fid );
		}

		// PMPG_C_S_MAKEFRIEND_FEEDBACK
		inline size_t SetFeedback( char* buff,
			                       userid_t fid,
								   short  result )
		{
			return CsFactory::MakeArgument( buff,
									        "%d%h",
											fid,
											result );
		}

		// PMPG_S_C_MAKEFRIEND_FEEDBACK
		inline size_t GetFeedback( char* buff,
			                       char* nickname,
								   userid_t& tid,
								   short& result )
		{
			return CsFactory::SplitArgument( buff,
									         "%s%d%h",
											 nickname,
									         &tid,
											 &result );
		}
	};

	namespace Server
	{
		inline size_t GetRequest( char* buff,
								  userid_t& tid )
		{
			return CsFactory::SplitArgument( buff,
									         "%d",
									         &tid );
		}

		// PMPG_S_C_MAKEFRIEND_REQUEST
		inline size_t SetRequest( char* buff,
			                      const char* nickname,
			                      userid_t fid )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%d",
											nickname,
											fid );
		}

		// PMPG_S_C_MAKEFRIEND_FEEDBACK
		inline size_t GetFeedback( char* buff,
			                       userid_t& fid,
								   short& result )
		{
			return CsFactory::SplitArgument( buff,
									         "%d%h",
											 &fid,
											 &result );
		}

		// PMPG_S_C_MAKEFRIEND_FEEDBACK
		inline size_t SetFeedback( char* buff,
			                       const char* nickname,
								   userid_t tid,
								   short result )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%d%h",
											nickname,
									        tid,
											result );
		}

	};
};


namespace PMSGDeletefriendMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
								  userid_t tid )
		{
			return CsFactory::MakeArgument( buff,
									        "%d",
									        tid );
		}
	};

	namespace Server
	{
		inline size_t GetRequest( char* buff,
								  userid_t& tid )
		{
			return CsFactory::SplitArgument( buff,
									         "%d",
									         &tid );
		}
	};
};


namespace PMSGCreateUserMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
								  const char* username , const char* password )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%s", username, password );
		}

		inline size_t GetFeedback( char* buff,
			                       char* nickname,
								   short& result )
		{
			return CsFactory::SplitArgument( buff,
									         "%s%h",
											 nickname,
											 &result );
		}
	};

	namespace Server
	{
		inline size_t GetRequest( char* buff,
								  char* username , char* password )
		{
			return CsFactory::SplitArgument( buff,
									         "%s%s",
									         username, password );
		}

		inline size_t SetFeedback( char* buff,
			                       const char* nickname,
								   short result )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%h",
											nickname,
											result );
		}

	};
};


namespace PMSGDeleteUserMessage
{
	namespace Client
	{
		inline size_t SetRequest( char* buff,
								  const char* username , const char* password )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%s", username, password );
		}

		inline size_t GetFeedback( char* buff,
			                       char* nickname,
								   short& result )
		{
			return CsFactory::SplitArgument( buff,
									         "%s%h",
											 nickname,
											 &result );
		}
	};

	namespace Server
	{
		inline size_t GetRequest( char* buff,
								  char* username , char* password )
		{
			return CsFactory::SplitArgument( buff,
									         "%s%s",
									         username, password );
		}

		inline size_t SetFeedback( char* buff,
			                       const char* nickname,
								   short result )
		{
			return CsFactory::MakeArgument( buff,
									        "%s%h",
											nickname,
											result );
		}

	};
};







#endif //PMSGCOMMAND_H

