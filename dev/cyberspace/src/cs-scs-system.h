//
// series CsAbtpCnSocket
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

//01.== C 系統文件/C++ 系統文件

//02.== 其他 PinSDK 函式庫的.h 文件 隱藏的放後面
#include <template/vmapl.inl>
#include <template/collectpool.inl>

//03.== 其他 使用 函式庫的.h 文件

//04.== 此專案內.h 文件
#include "cs-def.h"
#include "cs-pri_def.h"
#include "cs-scs-system_delegate.h"


#define _MAX_SCSSHAREBUFFER_GROUP  10

namespace cyberspace
{
    namespace scs
    {
        //class SBuffer;
        //class SectionServer;
        //class SectionClient;
        
        class System
        {
            //friend class cyberspace::scs::SectionServer;
            SystemDelegate*                                  delegate_;

            CsAbtpPrSocket*                                  section_agent_;  // 接收 / 傳送的socket
            cs_section_uid_t                                 server_id_;

			pfstl::vmapl<CsSectionEntity*>                   sections_;       // 分散式server群組 列表
			pfstl::vmapl<CsSessionEntity*>                   sessions_;       // 所有連線列表
            
			T_Lock                                           num_type_lock_;
			int                                              num_master_;
			int                                              num_clone_;

			int                                              thread_Id_;
            bool                                             is_alive_;
            ustimen_t                                        timeout_polling_;
			CsLogMode                                        log_mode_;

            
        public:
        
            System( SystemDelegate* pDelegate );
            ~System();
            
            int                             StartUp( _IN ushort_t port );
            void                            Shutdown();

            //////////////////////////////////////////////////////////////////////////
            unsigned short                  GetServicePort();
            _cs_size_t                       GetSectionSize();
            _cs_size_t                       GetConnectSize( _IN CseCONNECT_TYPE t );
            cs_section_uid_t                GetSectionId();
            cs_section_uid_t                GetSectionId( _IN const _cs_char_t* servername );
            //bool                            GetSectionInfo( _IN cs_section_uid_t sid, _OUT CsSectionEntity& info );
            void                            GetAllSectionInfo( _OUT std::vector<ServerInfo>& section_list );

            CsSectionDescriptor             GetSectionDescriptor( _IN cs_section_uid_t sectionid );
            static cs_section_uid_t         GetSectionSectionId( _IN CsSectionDescriptor section );
            static _cs_string                GetSectionSectionName( _IN CsSectionDescriptor section );


            //////////////////////////////////////////////////////////////////////////
            int                             InsertSection( _IN const CsSectionEntity* section );   // 當我收到要加入一台 伺服器
            int                             RemoveSection( _IN cs_section_uid_t sectionid );       // 當我收到要移除一台 伺服器
            int                             UpdateSection( _OUT CsSectionEntity& section );        // 取得本機的section


            //////////////////////////////////////////////////////////////////////////
            // 建立 > 加入 > 移除 > 消滅
            // 建立出一個描述值
            //CsAbtpCnSocket*                    CreateCsPSockConn();
            //void                            DeleteCsPSockConn( CsAbtpCnSocket* scn );
            //static void                     FullCsPSockConn( _INOUT CsAbtpCnSocket* scn/*, _IN cs_session_uid_t sid, _IN macn_t mac*/ );


            //////////////////////////////////////////////////////////////////////////
            // 通知大家 加入一個client
            bool                            CreateSession( _IN cs_session_uid_t sessionid, _IN CsAbtpCnSocket* scn );
            // 通知大家 移除一個client 不管是不是Master
            bool                            DeleteSession( _IN cs_session_uid_t sessionid );
            // 同步資料
            //void                            SyncSession( CsSessionEntity* sdp );
            
            //////////////////////////////////////////////////////////////////////////
            CsSessionDescriptor             GetSessionDescriptor( _IN cs_session_uid_t sessionid );
            static cs_session_uid_t         GetSessionSessionUID( _IN CsSessionDescriptor sdp );
            //static macn_t                 GetSessionMac( CsSessionEntity* sdp );
            static ipn6_t                   GetSessionIpn6( _IN CsSessionDescriptor sdp );
            //static ipa_t                  GetSessionIpa( CsSessionEntity* sdp );
            static ushort_t                 GetSessionPort( _IN CsSessionDescriptor sdp );
            static cs_section_uid_t         GetSessionSectionId( _IN CsSessionDescriptor sdp );
            static CsAbtpCnSocket*          GetSessionSocket( _IN CsSessionDescriptor sdp );
            static stimen_t                 GetSessionCreateSec( _IN CsSessionDescriptor sdp );
            static bool                     GetSessionIsMaster( _IN CsSessionDescriptor sdp );
            static bool                     GetSessionIsClone( _IN CsSessionDescriptor sdp );
            static bool                     GetSessionIsInvalid( _IN CsSessionDescriptor sdp );
           
            // return old node
            //static CsSessionNote*           SetSessionNote( CsSessionEntity* sdp, CsSessionNote* node );
            //static void                     DelSessionNote( CsSessionEntity* sdp, CsSessionNote* node );
            //static CsSessionNote*           GetSessionNote( CsSessionEntity* sdp, notetid_t notetid );
            

            //////////////////////////////////////////////////////////////////////////
            int                             SendMessage( _IN CsSessionDescriptor dest, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
            // 送訊息給指定的server
            int                             SendMessage( _IN cs_section_uid_t dest, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
            // 送訊息給所有的server
            int                             BroadcastMessage( _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
            // 送訊息給除了自己以外的server
            int                             BroadcastMessageToOther( _IN uniqueid_t from, _IN channel_uid_t cid, const _IN ubyte_t* bp, _IN _cs_size_t len );
            // 送訊息給除了自己以外與非隱藏的server
            int                             BroadcastMessageToOtherAndVisible( _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
            // 送訊息給除了自己以外且類別名稱相同的server
            int                             BroadcastMessageToOtherWithObject( _IN const _cs_char_t* objectname, _IN uniqueid_t from, _IN channel_uid_t cid, _IN const ubyte_t* bp, _IN _cs_size_t len );
            
            
        private:
        
            //CsSessionEntity*                CreateSessionDescriptor( const CsSectionDescriptor sn );
            //void                            DeleteSessionDescriptor( CsSessionEntity* scn );
            
          
            // 收到別人加入一個連線的處理
            void                            OnInsertSession( _IN const CsSessionEntity* buff, _IN _cs_size_t len );
            // 收到別人移除一個連線的處理
            void                            OnRemoveSession( _IN const CsSessionEntity* buff, _IN _cs_size_t len );

            virtual void                    OnRecvDefineRequest( _IN uniqueid_t eid, _IN channel_uid_t cid, _IN ubyte_t* command, _IN _cs_size_t len );
            
            //void                            OnSyncSession( net::TCPSocket* skt, const CsSectionDescriptor buff, _cs_size_t len );
            
            // 收到有一個server 並完成資料同步
            //void                            OnSectionSync( net::TCPSocket* skt );
            
            //void                            OnSectionFree( net::TCPSocket* skt );
            
            // 取得是由我建立的 sessionentity
            //void                            GetAllMasterSesseionEntity( std::vector<CsSessionEntity>& vec );
            
            static void                     PollingProcess( _IN void* data );
        };
        
    }
    
};