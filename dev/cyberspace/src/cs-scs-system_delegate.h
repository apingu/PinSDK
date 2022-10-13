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


namespace cyberspace
{

    namespace scs
    {
        //typedef ObjDescriptor BufferDescriptor;
        
        //////////////////////////////////////////////////////////////////////////
        //
        class SystemDelegate
        {
        public:
        
            SystemDelegate() {}
            ~SystemDelegate() {}
            
            //virtual cs_section_uid_t OnSCSGetLocalServerUID() = 0;
            
            //
            // 別台要求轉送訊息
            //virtual void                    OnSCSPassSendMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len ) = 0;
            //
            //virtual void                    OnSCSPassRecvMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len ) = 0;
            // 別台要求關閉
            //virtual void                    OnSCSKickout( CsSessionDescriptor sdp ) = 0;

            virtual void                    OnSCSJoinSection( CsSectionDescriptor dpsection ) = 0;
            virtual void                    OnSCSExitSection( CsSectionDescriptor dpsection ) = 0;
            
            virtual void                    OnSCSJoinSession( CsSessionDescriptor dpsession ) = 0; // 別台建立一個新連線
            virtual void                    OnSCSExitSession( CsSessionDescriptor dpsession ) = 0; // 斷線

                                                                                             
            virtual void                    OnSCSPassRecvMessage( CsSessionDescriptor sdp, uniqueid_t uid, channel_uid_t cid, const ubyte_t* bp, _cs_size_t len ) = 0;  // 其他機器上的 clie
            //virtual void                    OnSCSRecvDefineRequest( uniqueid_t who, channel_uid_t cid, const ubyte_t* command, _cs_size_t len ) = 0;

            //////////////////////////////////////////////////////////////////////////////////////////////
            // 建立 section用
            virtual const _cs_char_t*          DoGetObjectName() = 0;
            virtual const _cs_char_t*          DoGetLocalHostname( _cs_char_t* buf, _cs_size_t len ) = 0;
            virtual const _cs_char_t*          DoGetLocalFunctionname( void ) = 0;
            virtual const _cs_char_t*          DoGetLocalAliasname( void ) = 0;
            virtual ushort_t                DoGetLocalServicePort( void ) = 0;
            virtual stimen_t                DoGetCreateTime() = 0;
            virtual uint_t                  DoGetLocalLimitConnectSize( void ) = 0;
            virtual uint_t                  DoGetLocalConnectedSize( void ) = 0;
            virtual Pflag                   DoGetState( void ) = 0;
        };
    }// namespace scs
}// namespace cyberspace

