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
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "cs-core_server.h"
//#include "cs-database_service.h"
//#include <map>

typedef _cs_string                           channel_nid_t;  // name

namespace cyberspace
{
    enum CsHttpContentType
    {
        CsHttpEncType_Unknown     = 0,
        CsHttpEncType_Application = 1,
        CsHttpEncType_Multipart   = 2,      // multipart/form-data 它可以混合多種資料格式並一次傳送，當然非文字的資料必須要編碼為二進位字串。用 MPFDParser 解
        CsHttpEncType_Text        = 3,
        CsHttpEncType_Audio       = 4,
        CsHttpEncType_Image       = 5,
        CsHttpEncType_Video       = 6,
        CsHttpEncType_VND         = 7,
    };

    enum CsHttpCodingType
    {
        CsHttpEncType_Normal      = 1,      // 沒有壓縮的http內容
        CsHttpEncType_GZip        = 2,      // 有壓縮的http內容
    };


    class HttpServer : public CoreServer
    {

        //std::string         content_type_;
    public:

        HttpServer( const _cs_char_t* name = NULL );
        virtual ~HttpServer();

        virtual const _cs_char_t*    GroupName() const override { return ( "ServerGroup" ); }     // configure file 裡 XML <XXXXX> 會對應讀該名稱
        virtual const _cs_char_t*    ObjectName() const override { return ( "HttpServer" ); }      // configure file 裡 XML <XXXXX> 會對應讀該名稱

        // 通知客戶端 分配到的id
        void                         SetSessionUID( _IN CoreConnector* session, _IN cs_session_uid_t sid );
        cs_session_uid_t             GetSessionUID( _IN CoreConnector* session );

        CsHttpContentType            GetContentTypeType( _IN CoreConnector* session );
        _cs_string                   GetContentTypeValues( _IN CoreConnector* session );

        _cs_string                   GetMethod( _IN CoreConnector* session );


    protected:

        // 用這個來取代原本的 callback
        //virtual void        OnAcceptSocket( _IN CoreConnector* session ) = 0;
        //virtual void        OnKickoutSocket( _IN CoreConnector* session ) = 0;
        //virtual _csbinbuf   OnReceiveSocket( _IN CoreConnector* session, _IN CsHttpContentType enctype, _IN const char_t* actstr, _IN const ubyte_t* comstr, _IN uint_t comlen ) = 0;

        //CsHttpContentType              GetEncType( _IN CoreConnector* session );

        bool                         RegisterFunction( _IN channel_nid_t act_uid, _IN bool is_auth, _IN OnMemberFunction func );
        void                         UnRegisterFunction( _IN channel_nid_t act_uid );
       

    private:

        // 自行建立自己的seesion 物件
        CoreConnector*               OnCreateConnector();
        // 自行消滅自己的seesion 物件
        void                         OnDestoryConnector( CoreConnector* session );


        //void                       OnAccept( _IN CoreConnector* session ) {};
        //! call back when client cutoff connect
        //void                       OnKickout( _IN CoreConnector* session ) {};

        int                          OnRecvMessage( _IN CoreConnector* session );
        
        //=============================================================================
        _cs_string                   OnRecvOutputDebug( _IN CoreConnector* session, _IN const char* comstr, _IN int comlen );
    };
}