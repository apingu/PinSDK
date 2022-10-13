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

//2
#include <graphic\icon.h>

//3
#include <pf-string.h>
#include <parser\pgsp_file.h>

//4
#include "Scroll2d.h"


#define DEFAULD_MAX_VALUE 50
#define DEFAULD_CURR_VALUE 0

namespace GSO
{
    namespace UI
    {
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Scroll2d::Scroll2d():
            m_TbStep( SRR_TbStep ),
            m_Step( SRR_Step ),
            m_MaxValue( DEFAULD_MAX_VALUE ),
            m_CurrValue( DEFAULD_CURR_VALUE ),
            m_SetRange( false )
        {
            for ( unsigned int i = ScCTbUp; i < ScCMAXCom ; ++i )
            {
                //m_pScC[i] = new Button2d;
                m_pScC[i].SetId( i );
                BaseClass::AttachItem( &( m_pScC[i] ) );
            }
            
            m_pScC[ScCDivider].SetDragable( true );
            SetCurrStep( m_CurrValue );
        }
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Scroll2d::~Scroll2d()
        {
            // 底層機制自動清空，所以只要把記著的指標清空就好
            for ( unsigned int i = ScCTbUp ; i < ScCMAXCom ; ++i )
            {
                BaseClass::DetachItem( &( m_pScC[i] ) );
                //m_pScC[i] = NULL;
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int Scroll2d::Update( int count )
        {
            if ( !GetVisible() )
            { return 0; }
            
            UpdateRoute();
            SetCurrStep( m_CurrValue );
            return BaseClass::Update( count );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* Scroll2d::GetComponentButton( ScrollCom id )
        {
            if ( id <= ScCNon )
            { return NULL; }
            
            if ( id < ScCMAXCom )
            { return &( m_pScC[id] ); }
            
            if ( id == ScCBar )
            { return ( IWidget2d* )this; }
            
            return NULL;
        }
        
        
        void Scroll2d::DestroyAllItem( void )
        {
            // 因為他的child不是new 出來的 所以別動他
            return;
            
            for ( unsigned int i = ScCTbUp ; i != ScCMAXCom ; ++i )
            {
                BaseClass::DetachItem( &( m_pScC[i] ) );
            }
            
            IWidget2d::DestroyAllItem();
            
            for ( unsigned int i = ScCTbUp ; i != ScCMAXCom ; ++i )
            {
                BaseClass::AttachItem( &( m_pScC[i] ) );
            }
        }
        
        ////////////////////////////////////////////////////////////////////////////////
        // 元件自己的功能
        
        //=============================================================================
        // NAME:
        // DESC: 設定捲動能力
        //=============================================================================
        void Scroll2d::SetScrollStep( uint_t TbStep, uint_t Step )
        {
            m_TbStep = TbStep;
            m_Step = Step;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC: 設定游標可移動的範圍
        //         如0, 100, 0, 0表示垂直的捲軸(格式為:上、下、左、右)
        //=============================================================================
        void Scroll2d::SetScrollRange( int top, int bottom, int left, int right )
        {
            LG::Rect rt( left, top, right, bottom );
            m_SetRange = true;
            m_Range( left, top, right, bottom );
            m_SubRange = m_Range;
            //m_SubRange(0,0, 0, 0);
            //if (m_pScC[ScCTbUp] != NULL)
            {
                m_pScC[ScCTbUp].SetPos( LG::Point( m_Position.x + m_SubRange.left,
                                                   m_Position.y + m_SubRange.top ) );
                const CPIcon* img = m_pScC[ScCTbUp].GetImage( 0 );
                
                if ( img )
                { m_SubRange.top += img->GetClipHeight(); }
            }
            //if (m_pScC[ScCUp] != NULL)
            {
                m_pScC[ScCUp].SetPos( LG::Point( m_Position.x + m_SubRange.left,
                                                 m_Position.y + m_SubRange.top ) );
                const CPIcon* img = m_pScC[ScCUp].GetImage( 0 );
                
                if ( img )
                { m_SubRange.top += img->GetClipHeight(); }
            }
            //if (m_pScC[ScCDown] != NULL)
            {
                const CPIcon* img = m_pScC[ScCDown].GetImage( 0 );
                
                if ( img )
                { m_SubRange.bottom -= img->GetClipHeight(); }
                
                m_pScC[ScCDown].SetPos( LG::Point( m_Position.x + m_SubRange.left,
                                                   m_Position.y + m_SubRange.bottom ) );
            }
            //if (m_pScC[ScCTbDown] != NULL)
            {
                const CPIcon* img = m_pScC[ScCTbDown].GetImage( 0 );
                
                if ( img )
                { m_SubRange.bottom -= img->GetClipHeight(); }
                
                m_pScC[ScCTbDown].SetPos( LG::Point( m_Position.x + m_SubRange.left,
                                                     m_Position.y + m_SubRange.bottom ) );
            }
            //if (m_pScC[ScCDivider] != NULL)
            {
                //為了不讓divider拉覆蓋過 down button
                const CPIcon* img = m_pScC[ScCDivider].GetImage( 0 );
                
                if ( img )
                { m_SubRange.bottom -= img->GetClipHeight(); }
                
                m_pScC[ScCDivider].SetPos( LG::Point( m_Position.x + m_SubRange.left,
                                                      m_Position.y + m_SubRange.top ) );
            }
            m_SetRange = false;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        LG::Rect Scroll2d::GetScrollRange() const
        {
            return m_Range;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Scroll2d::SetMaxStep( uint_t value )
        {
            m_MaxValue = value;
        }
        
        //=============================================================================
        // NAME:
        // DESC: 改變元件代表的數值
        //=============================================================================
        void Scroll2d::SetCurrStep( uint_t value )
        {
            bool bfunc = false;
            
            // 當有改變時 就要呼叫CALLBACK
            if ( m_CurrValue != value )
            { bfunc = true; }
            
            if ( value > m_MaxValue )
            { m_CurrValue = m_MaxValue; }
            else
            { m_CurrValue = value; }
            
            //if (m_pScC[ScCDivider] == NULL)
            //    return;
            LG::Point p;
            
            if ( m_MaxValue == 0 )
            {
                p.x = m_Position.x +
                      m_SubRange.left;// +
                //(m_SubRange.width()) * m_CurrValue /
                //m_MaxValue;
                p.y = m_Position.y +
                      m_SubRange.top;
            }
            else
            {
                p.x = m_Position.x +
                      m_SubRange.left;// +
                //(m_SubRange.width()) * m_CurrValue /
                //m_MaxValue;
                p.y = m_Position.y +
                      m_SubRange.top +
                      ( m_SubRange.height() ) * m_CurrValue /
                      m_MaxValue;
            }
            
            m_pScC[ScCDivider].SetPos( p );
            
            if ( bfunc )
            { OnUserNotify( PE_MSGID_SCROLL, m_CurrValue, 0, p ); }
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC: Virtual function of IWidget2d
        //=============================================================================
        error_t Scroll2d::LoadRes( ScrollCom com, const tchar_t* filename )
        {
            error_t errcode = -1;
            
            if ( com == ScCBar )
            {
                errcode = IWidget2d::LoadRes( filename );
                m_Range( 0, 0, BaseClass::GetClipImageWidth(), BaseClass::GetClipImageHeight() );
            }
            else
            {
                if ( com >= ScCMAXCom )
                { return errcode; }
                
                errcode = m_pScC[com].LoadRes( filename );
            }
            
            SetScrollRange( m_Range.top, m_Range.bottom, m_Range.left, m_Range.right );
            return errcode;
        }
        
        //=============================================================================
        // NAME:
        // DESC: Virtual function of IWidget2d
        //=============================================================================
        error_t Scroll2d::LoadAnm( ScrollCom com, const tchar_t* filename )
        {
            IWidget2d* pWidget = GetComponentButton( com );
            
            if ( pWidget )
            {
                return pWidget->LoadAnm( filename );
            }
            
            return -1;
        }
        
        //=============================================================================
        // NAME:
        // DESC: Virtual function of IWidget2d
        //=============================================================================
        // 存檔用. 將屬性寫入 node ===============================================
        bool Scroll2d::SaveCfg( GspNode* toNode )
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
            sprintf( buf, "%d,%d,%d", GetPos().x, GetPos().y, GetPos().z );
            item->WriteCommand( _T( "POSITION" ), _T( "=" ), buf );
            
            if ( !m_ResoureName.empty() )
            { item->WriteCommand( _T( "RESIDNAME" ), _T( "=" ), m_ResoureName.c_str() ); }
            
            tNode* txtnode;
            
            for ( int i = 0; 1; i++ )
            {
                txtnode = GetTextNode( i );
                
                if ( txtnode == NULL )
                { break; }
                
#ifndef UNICODE
                
                if ( NULL == ( pstr = txtnode->m_Text.c_str() ) )
                { pstr = " "; }
                
#else
                
                if ( NULL == ( pstr = ( const char* )txtnode->m_Text.c_str() ) )    // 這邊的處理方式並不正確, 但是目前並不重要
                { pstr = " "; }
                
#endif
                sprintf( buf, _T( "%s,%d,%d,%d,%d,%d,%d" ), pstr,
                         txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                         txtnode->m_TextRect.left, txtnode->m_TextRect.right,
                         txtnode->m_ForeColor, txtnode->m_BackColor );
                item->WriteCommand( _T( "TEXT" ), _T( "=" ), buf );
            }
            
            {
                if ( ( name = GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_BAR_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_BAR_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( Scroll2d::ScCTbUp ) != NULL )
            {
                if ( ( name = GetComponentButton( Scroll2d::ScCTbUp )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbUP_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( Scroll2d::ScCTbUp )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbUP_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( Scroll2d::ScCUp ) != NULL )
            {
                if ( ( name = GetComponentButton( Scroll2d::ScCUp )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_UP_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( Scroll2d::ScCUp )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_UP_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( Scroll2d::ScCDivider ) != NULL )
            {
                if ( ( name = GetComponentButton( Scroll2d::ScCDivider )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DIVIDER_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( Scroll2d::ScCDivider )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DIVIDER_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( Scroll2d::ScCDown ) != NULL )
            {
                if ( ( name = GetComponentButton( Scroll2d::ScCDown )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DOWN_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( Scroll2d::ScCDown )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DOWN_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( Scroll2d::ScCTbDown ) != NULL )
            {
                if ( ( name = GetComponentButton( Scroll2d::ScCTbDown )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbDOWN_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( Scroll2d::ScCTbDown )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbDOWN_ANM" ), _T( "=" ), name ); }
                }
            }
            
            LG::Rect rect = GetScrollRange();
            /*
            rect.top  = __min(m_pScC[Scroll2d::ScCTbUp]->GetPos().y-GetPos().y,
                             m_pScC[Scroll2d::ScCUp]->GetPos().y-GetPos().y);
            rect.left = __min(m_pScC[Scroll2d::ScCTbUp]->GetPos().x-GetPos().x,
                             m_pScC[Scroll2d::ScCUp]->GetPos().x-GetPos().x);
            
            rect.right= __max(m_pScC[Scroll2d::ScCTbDown]->GetPos().x-GetPos().x+m_pScC[Scroll2d::ScCTbDown]->GetWidth(),
                             m_pScC[Scroll2d::ScCDown]->GetPos().x-GetPos().x+m_pScC[Scroll2d::ScCDown]->GetWidth());
            rect.bottom = __max(m_pScC[Scroll2d::ScCTbDown]->GetPos().y-GetPos().y+m_pScC[Scroll2d::ScCTbDown]->GetHeight(),
                               m_pScC[Scroll2d::ScCDown]->GetPos().y-GetPos().y+m_pScC[Scroll2d::ScCDown]->GetHeight());
            */
            sprintf( buf, _T( "%d,%d,%d,%d" ), rect.top, rect.bottom, rect.left, rect.right );
            item->WriteCommand( _T( "SCROLL_RECT" ), _T( "=" ), buf );
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
            sprintf( buf, _T( "%d" ), GetMaxStep() );
            item->WriteCommand( "MAXSTEP", _T( "=" ) , buf );
            //sprintf(buf,_T("%d"), GetCurrStep());
            //item->WriteCommand("CURRSTEP", _T("=") , buf);
            return true;
        }
        
        /*
        //=============================================================================
        // NAME:
        // DESC: Virtual function of DrawNode
        //=============================================================================
        void Scroll2d::OnRenderTo(rendercontainer_t &container)
        {
            if (!GetVisible())
                return;
        
            IWidget2d::OnRenderTo(container);
            for (unsigned int i = ScCTbUp; i < ScCMAXCom ; ++i)
            {
                if (m_pScC[i] != NULL)
                    m_pScC[i]->OnRenderTo(container);
            }
        }
        */
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool Scroll2d::OnIsCollision( GameVertex pos )
        {
            LG::Rect rect( GetScrollRange().left + m_Position.x,
                           GetScrollRange().top + m_Position.y,
                           GetScrollRange().right + m_Position.x,
                           GetScrollRange().bottom + m_Position.y );
                           
            if ( ( rect.left    < pos.x ) &&
                    ( rect.right > pos.x ) &&
                    ( rect.top    < pos.y ) &&
                    ( rect.bottom > pos.y ) )
            {
                return true;
            }
            
            return false;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void Scroll2d::OnLMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            int DivPos = m_CurrValue;
            
            switch ( pWidget->GetId() )
            {
            case ScCTbUp:
                if ( ( int )m_TbStep <= DivPos )
                { DivPos -= m_TbStep; }
                else
                { DivPos = 0; }
                
                break;
                
            case ScCUp:
                if ( ( int )m_Step <= DivPos )
                { DivPos -= m_Step; }
                else
                { DivPos = 0; }
                
                break;
                
            case ScCDivider:
                break;
                
            case ScCDown:
                DivPos += m_Step;
                
                if ( ( unsigned int )DivPos > m_MaxValue )
                { DivPos = m_MaxValue; }
                
                break;
                
            case ScCTbDown:
                DivPos += m_TbStep;
                
                if ( ( unsigned int )DivPos > m_MaxValue )
                { DivPos = m_MaxValue; }
                
                break;
                
            default://捲頁
                break;
            }
            
            SetCurrStep( DivPos );
            
            if ( ( ( IWidget2d* )GetParent() ) )
            { ( ( IWidget2d* )GetParent() )->OnLMouseDown( this, KeyCode, MouseSite ); }
        }
        
        // 因為BAR的部分是自己 所以要做在這邊
        void Scroll2d::OnLMouseDown( int key, LG::Point point )
        {
            int DivPos = m_CurrValue;
            
            if ( m_MaxValue == 0 )
            {
                DivPos = 0;//(point.y - m_SubRange.top);
            }
            else
            {
                int py = ( GetPos().y + m_SubRange.top );
                float rng = ( ( float )( point.y - py ) ) / ( ( float )m_SubRange.height() );
                DivPos = rng * m_MaxValue;
            }
            
            SetCurrStep( DivPos );
            IWidget2d::OnLMouseDown( key, point );
        }
        
        void Scroll2d::OnLMouseUp( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            ( ( IWidget2d* )GetParent() )->OnLMouseUp( this, KeyCode, MouseSite );
        }
        
        void Scroll2d::OnDrag( IWidget2d* pWidget, LG::Point MouseSite )
        {
            if ( pWidget == &( m_pScC[ScCDivider] ) )
            {
                //int pos = (MouseSite.y-(m_pScC[ScCDivider].GetClipImageHeight()/2)) - (m_Position.y+m_SubRange.top);
//                 int pos = (MouseSite.y) - (m_Position.y+m_SubRange.top);
//                 if(pos<0)
//                     pos=0;
//
//                 float ff=(float)pos/(float)(m_SubRange.bottom-m_SubRange.top);
//                 if(ff>1.0)
//                     ff=1.0;
//                 pos-=(int)(ff*(m_pScC[ScCDivider].GetClipImageHeight()*1.5));
//                 //pos-=m_pScC[ScCDivider].GetClipImageHeight()>>1;
//                 if(pos<0)
//                     pos=0;
//                 //int sh  = (m_SubRange.bottom-m_SubRange.top)/m_Step;
//                 SetCurrStep((pos*m_MaxValue)/100);
                int DivPos = m_CurrValue;
                int py = ( GetPos().y + m_SubRange.top );
                int pyy = ( GetPos().y + m_SubRange.bottom );
                
                if ( m_MaxValue == 0 )
                {
                    DivPos = 0;//(point.y - m_SubRange.top);
                }
                else
                {
                    int yy = MouseSite.y;//pWidget->GetPos().y;//+(m_pScC[ScCDivider].GetClipImageHeight()*0.5);
                    
                    if ( yy <= py )
                    { DivPos = 0; }
                    else if ( yy >= pyy )
                    { DivPos = m_MaxValue; }
                    else
                    {
                        float rng = ( ( float )( yy - py ) ) / ( ( float )m_SubRange.height() );
                        DivPos = rng * m_MaxValue;
                    }
                }
                
                SetCurrStep( DivPos );
            }
        }
        
        void Scroll2d::OnLMouseUp( int key, LG::Point point )
        {
//             int pos = (point.y+(m_pScC[ScCDivider].GetClipImageHeight()/2)) - (m_Position.y+m_SubRange.top);
//             int sh  = (m_SubRange.bottom-m_SubRange.top)/m_Step;
//             SetCurrStep(pos/sh);
        }
        
        
        void Scroll2d::OnMouseWheel( int key, LG::Point point )
        {
            int DivPos = m_CurrValue;
            
            if ( key <= 0 )
            {
                DivPos += m_Step;
                
                if ( DivPos > m_MaxValue )
                {
                    DivPos = m_MaxValue; 
                }
            }
            else
            {
                if ( m_Step <= DivPos )
                { 
                    DivPos -= m_Step; 
                }
                else
                {
                    DivPos = 0; 
                }
            }
            
            SetCurrStep( DivPos );
            IWidget2d::OnMouseWheel( key, point );
        }
        
        void Scroll2d::OnMoving( IWidget2d* pWidget, const GameVertex pos )
        {
            if ( m_SetRange )
            { return; }
            
            if ( ( pWidget == &( m_pScC[ScCTbUp] ) ) ||
                    ( pWidget == &( m_pScC[ScCUp] ) ) ||
                    ( pWidget == &( m_pScC[ScCTbDown] ) ) ||
                    ( pWidget == &( m_pScC[ScCDown] ) ) )
            {
                LG::Point tp = m_pScC[ScCTbUp].GetPos();
                const CPIcon* timg = m_pScC[ScCTbUp].GetImage( 0 );
                
                if ( timg == NULL )
                {
                    tp = m_pScC[ScCUp].GetPos();
                    timg = m_pScC[ScCUp].GetImage( 0 );
                }
                
                LG::Point bp = m_pScC[ScCTbDown].GetPos();
                const CPIcon* bimg = m_pScC[ScCTbDown].GetImage( 0 );
                
                if ( bimg == NULL )
                {
                    bp = m_pScC[ScCDown].GetPos();
                    bimg = m_pScC[ScCDown].GetImage( 0 );
                }
                
                if ( ( timg ) && ( bimg ) )
                { SetScrollRange( tp.y - GetPos().y, bp.y + bimg->GetClipHeight() - GetPos().y, tp.x - GetPos().x, bp.x + bimg->GetClipWidth() - GetPos().x ); }
            }
        }
        
    }; // namespace UI
}; // namespace GSO

