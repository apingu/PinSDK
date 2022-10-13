#ifndef PMSGDEF_H
#define PMSGDEF_H

#include "CsErr.h"

typedef unsigned int   userid_t;
typedef long	       ConferenceId_t;


#define _MAX_FRIENDLIST    100


#define PMSG_USER_ONLINE    1
#define PMSG_USER_OFFLINE   2
#define PMSG_USER_LEAVE     3
#define PMSG_USER_BUSY      5
#define PMSG_USER_EAT       6


#define PMSG_MF_EXIST      -2
#define PMSG_MF_FAILURE    -1
#define PMSG_MF_SUCCESS     1 

enum PMSG_CONFERENCE_STATE
{
    PMSG_CONFERENCE_FAILURE  =0,
	PMSG_CONFERENCE_SUCCESS,
	PMSG_CONFERENCE_OFFLINE,
};









#endif //PMSGDEF_H