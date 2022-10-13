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
#pragma once

#pragma warning(disable:4786)

#include <map>
#include <vector>
#include <pf-def.h>

typedef unsigned short         funcchannelid_t;

//namespace pfget  // Pf gadget
//{
#define _MESSAGE_CHANNEL_NOT_FIND -99

    const unsigned int _MAX_MESSAGE_CHANNEL = 0XFFFF;
    const unsigned int _MAX_CHANNEL_DIVIDE = 1000;

    typedef __SLAPI int( *const ChannelFunction )( const void* argu, const ubyte_t* combin, unsigned int comlen );
    //typedef __SLAPI void* ChannelFunction;


    class MessageDispatcher
    {
        typedef std::map<funcchannelid_t, ChannelFunction> ChannelMap;

        unsigned int max_channel_id_;

        ChannelMap   channel_funcs_[ _MAX_CHANNEL_DIVIDE ];

    public:

        MessageDispatcher();
        virtual ~MessageDispatcher();

        void           SetMaxChannel( _IN unsigned int size );
        unsigned int   GetMaxChannel( void );

        /**
         註冊一個callback函式。
         @param cid 使用的channel號碼。最大值為65535。
         @param func 要註冊的函式指標。
         */
        bool            RegisterChannel( _IN funcchannelid_t chid, _IN ChannelFunction func );
        bool            UnRegisterChannel( _IN funcchannelid_t chid );

        ChannelFunction GetFunction( _IN funcchannelid_t chid );
        int             DispatchMessage( _IN const void* argu, _IN funcchannelid_t chid, _IN const ubyte_t* combin, _IN unsigned int comlen );

        void            GetAllChannelId( _OUT std::vector<funcchannelid_t>& channellist );
    };
//};