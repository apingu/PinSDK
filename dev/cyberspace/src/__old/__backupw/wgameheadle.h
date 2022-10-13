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
#ifndef WGAMEHEADLE_H
#define WGAMEHEADLE_H

#include "WGameDefine.h"


enum GAMECOMMAND { gcLOGIN = MAX_MESSAGE_CHANNEL,
                   gcLOGOUT = MAX_MESSAGE_CHANNEL + 1, };

struct GameHLogin
{
    unsigned long usernamelen;
    unsigned long passwordlen;
    char* username;
    char* password;
};

struct GameHeadle
{
    unsigned long len;
    unsigned long channel;
    bool encryption;
};



#endif