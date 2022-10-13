//
//

//2
#include <graphic\icon.h>

//3
#include <pf-string.h>
#include <parser/pgsp_file.h>

//4
#include "Number2d.h"

namespace GSO
{
    namespace UI
    {
    
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Number2d::Number2d()
        {
            m_Number = 0;
            m_Space = 2;
            m_Align = 0;
            m_Figure = 0;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Number2d::~Number2d()
        {
            // 交給 ELEMENT去DELETE吧
            for ( uint_t i = 0; i < m_NumberVec.size(); i++ )
            {
                //BaseClass::DetachChild(m_NumberVec[i]);
                BaseClass::DetachItem( m_NumberVec[i] );
                SAFE_DELETE( m_NumberVec[i] );
            }
            
            m_NumberVec.clear();
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        longlong_t& Number2d::operator=( longlong_t number )
        {
            if ( number == m_Number )
            {
                return m_Number;
            }
            
            // TODO: Pin
            // 現在只有畫正數的功能
            m_Number = number;
            char buffA[512];
            char buff[512];
            lltoa( number, buffA, 10 );
            memset( buff, 0, 512 );
            uint_t nlen = strlen( buffA );
            uint_t slen = __max( nlen, m_Figure );
            
            // 要補位的狀況
            if ( slen == m_Figure )
            {
                int muln = m_Figure - nlen;
                strcpy( &buff[muln], buffA );
                
                for ( int i = 0; i < muln; i++ )
                {
                    buff[i] = '0';
                }
            }
            else
            {
                strcpy( buff, buffA );
            }
            
            {
                // 因為第一個數字用媽媽 所以這邊決定還要加上幾個子物件
                slen--;
                
                if ( slen > m_NumberVec.size() )
                {
                    for ( int i = m_NumberVec.size(); i < slen; i++ )
                    {
                        m_NumberVec.push_back( new IWidget2d );
                        m_NumberVec.back()->LoadRes( GetResfname() );
                        m_NumberVec.back()->LoadAnm( GetAnmfname() );
                        BaseClass::AttachItem( m_NumberVec.back() );
                        //BaseClass::AttachChild(m_NumberVec.back());
                    }
                }
                else if ( slen < m_NumberVec.size() )
                {
                    uint_t numlen = slen - m_NumberVec.size();
                    
                    for ( uint_t i = slen; i < m_NumberVec.size(); i++ )
                    {
                        //BaseClass::DetachChild(m_NumberVec[i]);
                        BaseClass::DetachItem( m_NumberVec[i] );
                        SAFE_DELETE( m_NumberVec[i] );
                    }
                    
                    m_NumberVec.erase( m_NumberVec.begin() + slen, m_NumberVec.end() );
                }
                
                char cb[2] = {0X0000};
                cb[0] = buff[0];
                //
                FaceTo( atoi( cb ) );
                
                // 把數字的圖 改向成那一個
                for ( int i = 0; i < m_NumberVec.size(); i++ )
                {
                    cb[0] = buff[i + 1];
                    m_NumberVec[i]->FaceTo( atoi( cb ) );
                }
            }
            
            SetPos( GetPos() );
            return m_Number;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Number2d::SetAlign( int t )
        {
            m_Align = t;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Number2d::SetXSpace( int t )
        {
            if ( m_Space != t )
            {
                m_Space = t;
                longlong_t oldnum = m_Number;
                m_Number++;  // 改變原本的數字讓他強制會被重設
                Number2d::operator = ( oldnum );
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Number2d::SetFigure( int t )
        {
            if ( m_Figure != t )
            {
                m_Figure = t;
                longlong_t oldnum = m_Number;
                m_Number++;  // 改變原本的數字讓他強制會被重設
                Number2d::operator = ( oldnum );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Number2d::SetPos( GameVertex pos )
        {
            if ( m_Align == 1 )
            {
                int w = 0;
                w += GetWidth();
                
                for ( uint_t i = 0; i < m_NumberVec.size(); i++ )
                {
                    w += m_Space;
                    w += m_NumberVec[i]->GetWidth();
                }
                
                pos.x -= ( w / 2 );
            }
            
            IWidget2d::SetPos( pos );
            int x = GetPos().x;
            int y = GetPos().y;
            int z = GetPos().z;
            int w = IWidget2d::GetWidth();
            const CPIcon* icon;
            icon = GetImage();
            
            if ( icon )
            { 
                w -= icon->GetClipOffsetX(); 
            }
            
            // Number存放從第二個數字開始
            for ( uint_t i = 0; i < m_NumberVec.size(); i++ )
            {
                w += m_Space;
                m_NumberVec[i]->SetPos( GameVertex( x + w, y, z ) );
                w += m_NumberVec[i]->GetWidth();
                icon = m_NumberVec[i]->GetImage();
                
                if ( icon )
                { 
                    w -= icon->GetClipOffsetX();
                }
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool Number2d::SaveCfg( GspNode* toNode )
        {
            tchar_t buf[255];
            const tchar_t* pstr;
            pfstd::CString name = GetResfname();
            // 改大寫
            tchar_t* objname = new tchar_t[strlen( ObjectName() ) + 1];
            strcpy( objname, ObjectName() );
            strtoupper( objname );
            GspNode* item = new GspNode( objname, m_Expression.c_str() );
            SAFE_DELETE_ARRAY( objname );
            toNode->AttachChildNode( item );
            // 共通部分
            //if(!m_Expression.empty())
            //    item->WriteCommand(_T("EXPRESSION"), _T("="), m_Expression.c_str());
            lltoa( GetId(), buf, 10 );
            item->WriteCommand( _T( "ID" ), _T( "=" ), buf );
            lltoa( GetGId(), buf, 10 );
            item->WriteCommand( _T( "GID" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d,%d,%d" ), GetPos().x, GetPos().y, GetPos().z );
            item->WriteCommand( _T( "POSITION" ), _T( "=" ), buf );
            
            if ( !m_ResoureName.empty() )
            { item->WriteCommand( _T( "RESIDNAME" ), _T( "=" ), m_ResoureName.c_str() ); }
            
            name = GetResfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                { name = currname; }
                
                item->WriteCommand( _T( "LOADRES" ), _T( "=" ), name );
            }
            
            name = GetAnmfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                { name = currname; }
                
                item->WriteCommand( _T( "LOADANM" ), _T( "=" ) , name );
            }
            
            tNode* txtnode;
            
            for ( int i = 0; 1; i++ )
            {
                txtnode = GetTextNode( i );
                
                if ( !txtnode )
                { break; }
                
#ifndef UNICODE
                
                if ( NULL == ( pstr = txtnode->m_Text.c_str() ) )
                { pstr = _T( " " ); }
                
#else
                pstr = _T( "\"\"" );
#endif
                sprintf( buf, _T( "%s,%d,%d,%d,%d,%d,%d" ), pstr,
                         txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                         txtnode->m_TextRect.left, txtnode->m_TextRect.right,
                         txtnode->m_ForeColor, txtnode->m_BackColor );
                item->WriteCommand( _T( "TEXT" ), _T( "=" ), buf );
            }
            
            sprintf( buf, _T( "%d" ), m_Figure );
            item->WriteCommand( _T( "FIGURE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), m_Space );
            item->WriteCommand( _T( "XSPACE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%lld" ), m_Number );
            item->WriteCommand( _T( "NUMBER" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d,%d" ), m_Anim2d.GetPlayMode(), m_Anim2d.GetPlayInterrupt() );
            item->WriteCommand( _T( "PLAYMODE" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d" ), RenderAlpha );
            item->WriteCommand( _T( "ALPHA" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d" ), RenderLayer );
            item->WriteCommand( _T( "LAYER" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d,%d" ), GetPopupType(), GetPopupMsec() );
            item->WriteCommand( _T( "POPUPTYPE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetDragable() );
            item->WriteCommand( _T( "DRAGABLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetVisible() );
            item->WriteCommand( _T( "VISIBLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetEnable() );
            item->WriteCommand( _T( "ENABLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), m_Align );
            item->WriteCommand( _T( "ALIGN" ), _T( "=" ), buf );
            return true;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Number2d::DestroyAllItem( void )
        {
            //
            return;
        }
        
        /*
        
        bool Number2d::AttachItem(IWidget2d* item)
        {
            // 不做任何事 別人不准Attach我的ITME
            return false;
        }
        //=============================================================================
        // NAME:
        // DESC: 這裡繼承只是為了讓她變 Private
        //=============================================================================
        bool Number2d::DetachItem(IWidget2d* item)
        {
            return false;
            // 不做任何事 別人不准Detach我的ITME
            for (uint_t i = 0; i < m_NumberVec.size(); i++)
            {
                if (m_NumberVec[i] == item)
                {
                    m_NumberVec.erase(m_NumberVec.begin() + i);
                    break;
                }
            }
            return BaseClass::DetachItem(item);
        }
        */
    };
};
