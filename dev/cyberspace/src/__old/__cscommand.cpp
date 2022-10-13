// Cyber space common define file
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
#include "CsCommand.h"


//======================================
//	Login Command Message
//======================================
namespace CsLoginMessage
{
	void SetLoginMessage(CsCommandMessage *msg, const char *username, const char *password)
	{
		char *p;

		msg->CommandHeader.CommandId = CsCLOGIN_REQUEST;
		msg->CommandHeader.CommandLen = strlen(username) + 1 + strlen(password) + 1;
		
		p = msg->Buff;

		strcpy(p , username);
		p += strlen(username);
		*p++ = '\0';

		strcpy(p , password);
		p += strlen(password);
		*p++ = '\0';
	}
	void SetFeedbackMessage(CsCommandMessage *msg, const char *feedback){
		msg->CommandHeader.CommandId = CsCLOGIN_FEEDBACK;
		msg->CommandHeader.CommandLen = strlen(feedback) + 1;
		strcpy(msg->Buff , feedback);
	}

	char* GetUsername( CsCommandMessage *msg )
	{
		return msg->Buff;
	}

	char* GetPassword( CsCommandMessage *msg )
	{
		char *p = msg->Buff;
		p += strlen(p);
		return p+1;
	}
};
