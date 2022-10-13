//
// application binary translate protocol
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
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "cs-core_server.h"
//#include "cs-database_service.h"
//#include <map>


namespace cyberspace
{
    class AbtpServer : public CoreServer
    {

        //std::string         content_type_;
    public:

        AbtpServer( const _cs_char_t* name = NULL );
        ~AbtpServer();

        virtual const _cs_char_t*     GroupName() const { return ( "ServerGroup" ); }     // configure file 裡 XML <XXXXX> 會對應讀該名稱
        virtual const _cs_char_t*     ObjectName() const { return ( "AbtpServer" ); }      // configure file 裡 XML <XXXXX> 會對應讀該名稱


        void                          SetSessionUID( _IN CoreConnector* session, _IN cs_session_uid_t sid );
        cs_session_uid_t              GetSessionUID( _IN CoreConnector* session );



    protected:

        // 用這個來取代原本的 callback
        //virtual void        OnAcceptSocket( _IN CoreConnector* session ) = 0;
        //virtual void        OnKickoutSocket( _IN CoreConnector* session ) = 0;
        //virtual _csbinbuf   OnReceiveSocket( _IN CoreConnector* session, _IN CsHttpContentType enctype, _IN const char_t* actstr, _IN const ubyte_t* comstr, _IN uint_t comlen ) = 0;

        //CsHttpContentType              GetEncType( _IN CoreConnector* session );


        void                          RegisterFunction( _IN _cs_string act_uid, _IN OnMemberFunction func );
        void                          UnRegisterFunction( _IN _cs_string act_uid );


    private:

        // 隱藏
        int                           SendMessage( _IN CoreConnector* session, _IN const ubyte_t* bp, _IN uint_t len );
        size32_t                      GetRecvLength( _IN CoreConnector* session );

        // 自行建立自己的seesion 物件
        CoreConnector*                OnCreateConnector();
        // 自行消滅自己的seesion 物件
        void                          OnDestoryConnector( CoreConnector* session );


        //void                       OnAccept( _IN CoreConnector* session ) {};
        //! call back when client cutoff connect
        //void                       OnKickout( _IN CoreConnector* session ) {};

        int                           OnRecvMessage( _IN CoreConnector* session );

        //=============================================================================
        std::string                   OnRecvOutputDebug( _IN CoreConnector* session, _IN const char* comstr, _IN int comlen );
    };
}