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
#pragma warning(disable:4786)

//01. == C 系統文件/C++ 系統文件
#include <assert.h>

//02. == 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <pf-timer.h>
#include <pf-debug.h>

//03. == 其他 使用 函式庫的.h 文件

//04. == 此專案內.h 文件
#include "message_dispatcher.h"


//namespace pfget
//{
    //=============================================================================
    //
    //
    //=============================================================================
    MessageDispatcher::MessageDispatcher()
    {
        //memset(channel_funcs_, 0, _cssizeof(channel_funcs_));
        //SetMaxChannel(1024);
        max_channel_id_ = 65535;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    MessageDispatcher::~MessageDispatcher()
    {
        for ( unsigned int i = 0; i < _MAX_CHANNEL_DIVIDE; i++ )
        {
            channel_funcs_[i].clear();
        }
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    unsigned int MessageDispatcher::GetMaxChannel( void )
    {
        //return channel_funcs_.size();
        return max_channel_id_;   //沒有用...
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void MessageDispatcher::SetMaxChannel( unsigned int size )
    {
        max_channel_id_ = size;
        //channel_funcs_.resize(size);
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool MessageDispatcher::RegisterChannel( funcchannelid_t chid, ChannelFunction func )
    {
        /*
        //? 好怪
        if(chid >= channel_funcs_.size())
            return false;
        channel_funcs_[chid] = (void *)func;
        */
        if ( chid >= max_channel_id_ )
        {
            assert( 0 && "Message channel chid over limit!!" );
            printf( "MessageDispatcher: channel chid [%d] over limit [%d] \n", chid, max_channel_id_ );
            return false;
        }
        
        int pi = chid % _MAX_CHANNEL_DIVIDE;
        
        if ( !channel_funcs_[pi].empty() )
        {
            std::map<funcchannelid_t, ChannelFunction>::iterator cpmi = channel_funcs_[pi].find( chid );
            
            //char buff[255];
            if ( cpmi != channel_funcs_[pi].end() )
            {
                Pf_WARNING( "MessageDispatcher: double Register channel [%d]\n", chid );
                return false;
            }
            
            //printf("MessageDispatcher: double Register channel [%d]\n", chid);
            //exit(0);
        }
        
        //char buff[256];
        //_stprintf(buff, "MessageDispatcher: Register channel [%d]\n", chid);
        Pf_INFO( "MessageDispatcher::RegisterChannel: Register channel [%d]\n", chid );
        channel_funcs_[pi].insert( std::map<funcchannelid_t, ChannelFunction>::value_type( chid, func ) );
        return true;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    bool MessageDispatcher::UnRegisterChannel( funcchannelid_t chid )
    {
        int pi = chid % _MAX_CHANNEL_DIVIDE;
        
        if ( !channel_funcs_[pi].empty() )
        {
            std::map<funcchannelid_t, ChannelFunction>::iterator cpmi = channel_funcs_[pi].find( chid );
            
            if ( cpmi != channel_funcs_[pi].end() )
            {
                channel_funcs_[pi].erase( cpmi );
            }
        }
        
        Pf_INFO( "MessageDispatcher::UnRegisterChannel: Unregister channel [%d]\n", chid );
        return true;
    }

    //=============================================================================
    //
    //
    //=============================================================================
    ChannelFunction MessageDispatcher::GetFunction( funcchannelid_t chid )
    {
        int pi = chid % _MAX_CHANNEL_DIVIDE;
        std::map<funcchannelid_t, ChannelFunction>::iterator cpmi = channel_funcs_[ pi ].find( chid );

        if ( cpmi != channel_funcs_[ pi ].end() )
        {
            return cpmi->second;
        }
        else
        {
            Pf_INFO( "MessageDispatcher: Channel [%d] not find\n", chid );
        }
        return NULL;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    int MessageDispatcher::DispatchMessage( const void* argu, funcchannelid_t chid, const ubyte_t* combin, unsigned int len )
    {
        int pi = chid % _MAX_CHANNEL_DIVIDE;
        std::map<funcchannelid_t, ChannelFunction>::iterator cpmi = channel_funcs_[pi].find( chid );
        
        if ( cpmi != channel_funcs_[pi].end() )
        {
            ChannelFunction func = ( ChannelFunction )cpmi->second;
            
            if ( func )
            {
                return ( *func )( argu, combin, len );
            }
        }
        else
        {
            Pf_WARNING( "MessageDispatcher: Channel [%d] not find\n", chid );
        }
        
        return _MESSAGE_CHANNEL_NOT_FIND;
    }
    
    //=============================================================================
    //
    //
    //=============================================================================
    void MessageDispatcher::GetAllChannelId( std::vector<funcchannelid_t>& channellist )
    {
        for ( unsigned int i = 0; i < _MAX_CHANNEL_DIVIDE; i++ )
        {
            std::map<funcchannelid_t, ChannelFunction>::iterator cpmi = channel_funcs_[i].begin();
            
            while ( cpmi != channel_funcs_[i].end() )
            {
                channellist.push_back( cpmi->first );
                cpmi++;
            }
        }
    }    
//};


