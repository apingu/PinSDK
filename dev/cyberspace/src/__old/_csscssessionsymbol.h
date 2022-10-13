//
//
//
//
//
//
//
//
#pragma once

#include "CsEntityDef.h"
#include <map>

namespace Cyberspace
{
    namespace SCS
    {
        class Connection;
        class PeerSection;
        
        //////////////////////////////////////////////////////////////////////////
        // 代表每一個連結到SERVER的CLINET
        class SessionSymbol: private _CsSessionEntity
        {
            friend class Cyberspace::SCS::System;
            
            T_Lock                                     m_utlock;                 //*[16]*/;
            long32_t                                   m_PrivateFlag;            // 自己用的
            long32_t                                   m_PublicFlag;             // 給別人用的
            stimen_t                                   m_LastUsedSec;            //上次使用的時間
            Cyberspace::Connection*                    m_pSkt;                   //如果是本機連線 紀錄 實際連線的 socket 否則就 設為NULL 別台機器的連線
            Cyberspace::SCS::PeerSection*              m_pCreateSection;         //which server create this node
            Cyberspace::SCS::PeerSection*              m_pConnectSection;        //which server connect this node            
            std::map<notetid_t, CsSessionNote*>        m_SessionNoteVec;
            
        public:
        
            SessionSymbol()  { m_pSkt = NULL; Clear(); }
            ~SessionSymbol() { m_pSkt = NULL; Clear(); }
            
        private:
        
            inline void Initial( const _CsSessionEntity* c, Cyberspace::Connection* pMySkt )
            {
                TS_Lock lock( &m_utlock );
                sessionid     = c->sessionid;
                //mac           = c->mac;
                serverid      = c->serverid;
                ipn6          = c->ipn6;
                port          = c->port;
                createsec     = c->createsec;
                flag          = c->flag;
                m_PublicFlag  = 0;
                m_LastUsedSec = 0;
                m_pSkt        = pMySkt;
                //group       = c->group;
                //power       = c->power;
                //index       = c->index;
                //m_Key       = NULL;
                //SetPrivateFlagBit( _CONNECTNODE_INVALID, false );
                CANCEL_BIT_FLAG( m_PrivateFlag, _CONNECTNODE_INVALID );
                
                if ( m_pSkt == NULL )
                {
                    //SetPrivateFlagBit( _CONNECTNODE_MASTER, false );
                    CANCEL_BIT_FLAG( m_PrivateFlag, _CONNECTNODE_MASTER );
                }
                else
                {
                    //SetPrivateFlagBit( _CONNECTNODE_MASTER, true );
                    SET_BIT_FLAG( m_PrivateFlag, _CONNECTNODE_MASTER );
                }
            }
            
            void Clear()
            {
                TS_Lock lock( &m_utlock );
                sessionid = CsINVALID_ID;
                
                if ( m_pSkt == NULL )
                {
                    serverid = CsINVALID_ID;  // 如果是 master 不應該被清掉
                }
                
                memset( &ipn6, 0, _cssizeof( ipn6_t ) );
                //group = 0;
                //power = 0;
                //index = 0;
                flag = 0;
                m_PrivateFlag = 0;
                m_pCreateSection = NULL;
                m_pConnectSection = NULL;
                m_LastUsedSec = 0;
                
                for ( std::map<notetid_t, CsSessionNote*>::iterator it = m_SessionNoteVec.begin(); it != m_SessionNoteVec.end(); it++ )
                {
                    SAFE_DELETE( it->second );
                }
            }
            
            bool  operator == ( _CsSessionEntity& c )
            {
                TS_Lock lock( &m_utlock/*[ ]*/ );
                
                if ( ( sessionid == c.sessionid ) && ( serverid == c.serverid ) && ( memcmp( &ipn6, &c.ipn6, _cssizeof( ipn6 ) ) == 0 ) /*&& ( mac == c.mac )*/ )
                {
                    return true;
                }
                
                return false;
            }
            
            inline void Copy( _CsSessionEntity& c )
            {
                TS_Lock lock( &m_utlock );
                c.sessionid = sessionid;
                //c.mac = mac;
                c.serverid = serverid;
                //c.group = group;
                //c.power = power;
                //c.index = index;
                c.ipn6 = ipn6;
                c.createsec = createsec;
                c.flag = flag;
            }
            
            inline bool                    IsMaster()                         { TS_Lock lock( &m_utlock );        return ( m_pSkt == NULL ) ? false : true; }
            
            inline sessionid_t             GetCid()                           { TS_Lock lock( &m_utlock/*[0]*/ ); return sessionid; }
            //inline macn_t                  GetMid()                           { TS_Lock lock( &m_utlock/*[1]*/ ); return mac; }
            inline ipn6_t                  GetIpn6()                          { TS_Lock lock( &m_utlock/*[4]*/ ); return ipn6; }
            inline ushort_t                GetPort()                          { TS_Lock lock( &m_utlock/*[4]*/ ); return port; }
            inline serverid_t              GetSid()                           { TS_Lock lock( &m_utlock/*[2]*/ ); return serverid; }
            inline stimen_t                GetCreateSec()                     { TS_Lock lock( &m_utlock/*[5]*/ ); return createsec; }
            inline long32_t                GetState()                         { TS_Lock lock( &m_utlock/*[8]*/ ); return m_PrivateFlag; }
            inline Cyberspace::Connection* GetPeerClient()                    { TS_Lock lock( &m_utlock/*[9]*/ ); return m_pSkt; }
            inline stimen_t                GetLastUsedSec()                   { TS_Lock lock( &m_utlock/*[ ]*/ ); return m_LastUsedSec; }
            inline PeerSection*            GetCreateFrom()                    { TS_Lock lock( &m_utlock/*[9]*/ ); return m_pConnectSection; }
            inline PeerSection*            GetConnectFrom()                   { TS_Lock lock( &m_utlock/*[9]*/ ); return m_pConnectSection; }
            inline bool                    GetPrivateFlagBit( ushort_t  bit ) {	TS_Lock lock( &m_utlock/*[8]*/ ); return IF_BIT_FLAG( m_PrivateFlag, bit ); }
            
            // 紀錄該物件上次被使用的時間
            inline void                    SetUsed() { TS_Lock lock( &m_utlock/*[ ]*/ );  m_LastUsedSec = time( NULL ); }
            inline void                    SetConnectFrom( PeerSection* pFromSkt )
            {
                TS_Lock lock( &m_utlock/*[7]*/ );           // 第一次 建立者
                
                if ( pFromSkt )
                {
                    serverid = pFromSkt->serverid;
                }
                
                if ( m_pCreateSection == NULL )
                {
                    m_pCreateSection = pFromSkt;
                }
                
                m_pConnectSection = pFromSkt;
            }
            
            inline void                    SetPrivateFlagBit( ushort_t bit, bool n ) { TS_Lock lock( &m_utlock/*[8]*/ );  ( n ) ? SET_BIT_FLAG( m_PrivateFlag, bit ) : CANCEL_BIT_FLAG( m_PrivateFlag, bit ); }
            // return old note
            inline CsSessionNote*          InsertNote( CsSessionNote* node )
            {
                if ( node == NULL )
                {
                    return NULL;
                }
                
                TS_Lock lock( &m_utlock );
                CsSessionNote* oldnode = NULL;
                std::map<notetid_t, CsSessionNote*>::iterator it = m_SessionNoteVec.find( node->GetTypeId() );
                
                if ( it != m_SessionNoteVec.end() )
                {
                    oldnode = it->second;
                }
                
                m_SessionNoteVec.insert( std::map<notetid_t, CsSessionNote*>::value_type( node->GetTypeId(), node ) );
                return oldnode;
            }
            
            inline void                    RemoveNote( CsSessionNote* node )
            {
                TS_Lock lock( &m_utlock );
                m_SessionNoteVec.erase( node->GetTypeId() );
            }
            
            inline CsSessionNote*          FindNote( notetid_t notetid )
            {
                TS_Lock lock( &m_utlock );
                std::map<notetid_t, CsSessionNote*>::iterator it = m_SessionNoteVec.find( notetid );
                
                if ( it != m_SessionNoteVec.end() )
                {
                    return it->second;
                }
                
                return NULL;
            }
        };
    }
}

//#endif //CsSCSSessionSYMBOL_H