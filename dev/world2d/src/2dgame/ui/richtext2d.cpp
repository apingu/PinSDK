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
#include "RichText2d.h"
#include <parser/pgsp_file.h>
#include <pf-string.h>
#include <malloc.h>

namespace GSO
{
    namespace UI
    {
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        RichText2d::RichText2d():
            m_FocusLine( SV_NoFocusLine ),
            m_StartPos( 0 ),
            m_StopPos( 0 ),
            m_InsterPos( 0 ),
            m_StartLine( 0 )
        {
            m_FronColor = 0x20;
            m_BackColor = 0;
            m_InverseFrontColor = RGB( 200, 200, 200 ); //0x07e0;
            m_InverseBackColor = RGB( 128, 128, 128 ); //0x0f;
            SetDragable( false );
            //預定的數值資料
            m_FontSize( 7, 14 );
            //建立元件
            m_Scroll = new Scroll2d;
            m_Scroll->SetId( RiScroll );
            BaseClass::AttachItem( m_Scroll );
            m_Scroll->SetMaxStep( m_Scroll->GetCurrStep() + 1 );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        RichText2d::~RichText2d()
        {
            m_Scroll = NULL;
        }
        
        void RichText2d::DestroyAllItem( void )
        {
            BaseClass::DestroyAllItem();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        error_t RichText2d::LoadRes( RichTextCom btn, const tchar_t* filename )
        {
            error_t errcode = -1;
            
            if ( btn == RiCText )
            {
                errcode = BaseClass::LoadRes( filename );
                m_TextRectD.left = 4;
                m_TextRectD.right = GetClipImageWidth() - 6;
                m_TextRectD.top = 4;
                m_TextRectD.bottom = GetClipImageHeight() - 6;
            }
            else
            {
                errcode = m_Scroll->LoadRes( ( Scroll2d::ScrollCom )btn, filename );
            }
            
            return errcode;
        }
        
        error_t RichText2d::LoadAnm( RichTextCom btn, const tchar_t* filename )
        {
            error_t errcode = -1;
            
            if ( btn == RiCText )
            {
                errcode = BaseClass::LoadAnm( filename );
                m_TextRectD.left = 4;
                m_TextRectD.right = GetClipImageWidth() - 6;
                m_TextRectD.top = 4;
                m_TextRectD.bottom = GetClipImageHeight() - 6;
            }
            else
            {
                errcode = m_Scroll->LoadAnm( ( Scroll2d::ScrollCom )btn, filename );
            }
            
            return errcode;
        }
        
        /*
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::OnRenderTo(rendercontainer_t &container)
        {
            if (!GetVisible())
                return;
        
            BaseClass::OnRenderTo(container);
            m_Scroll->OnRenderTo(container);
        }
        */
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* RichText2d::GetComponentButton( RichTextCom id )
        {
            if ( ( id > RiScNon ) && ( id < RiScMaxCom ) )
            { return m_Scroll->GetComponentButton( ( Scroll2d::ScrollCom )id ); }
            else if ( id == RiScroll )
            { return m_Scroll; }
            else if ( id == RiCText )
            { return this; }
            
            return NULL;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC: 設定游標可移動的範圍
        //         如0, 100, 0, 0表示垂直的捲軸(格式為:上、下、左、右)
        //=============================================================================
        void RichText2d::SetScrollRange( int top, int bottom, int left, int right )
        {
            m_Scroll->SetPos( GameVertex( left, top, 0 ) );
            m_Scroll->SetScrollRange( 0, bottom - top, 0, right - left );
            //m_Scroll->SetScrollRange(top, bottom, left, right);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        LG::Rect RichText2d::GetScrollRange()
        {
            LG::Rect rt = m_Scroll->GetScrollRange();
            rt.top += m_Scroll->GetPos().y;
            rt.left += m_Scroll->GetPos().x;
            rt.right += m_Scroll->GetPos().x;
            rt.bottom += m_Scroll->GetPos().y;
            return rt;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool RichText2d::IsCollisionByImage( GameVertex mpos )
        {
            if ( !GetVisible() )
            { return false; }
            
            bool flag = m_Scroll->IsCollisionByImage( mpos );
            
            if ( flag == false )
            {
                return BaseClass::IsCollisionByImage( mpos );
            }
            
            return flag;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::SetFontSize( int w, int h )
        {
            m_FontSize.cx = w;
            m_FontSize.cy = h;
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::OnMouseWheel( int key, LG::Point point )
        {
            int CurrLine = m_Scroll->GetCurrStep();
            
            if ( key > 0 )
            {
                CurrLine--;
                
                if ( CurrLine < 0 )
                { CurrLine = 0; }
            }
            else
            {
                CurrLine++;
            }
            
            m_Scroll->SetCurrStep( CurrLine );
            SetStartLine( m_Scroll->GetCurrStep() );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void* RichText2d::OnGetTextNode()
        {
            if ( GetVisible() )
            {
                cText_Node* ptextnode = ( cText_Node* )this;
                ptextnode->m_TextMethod = cText_Node::TMRange;
                ptextnode->m_DrawTextLine = m_StartLine;
                ptextnode->m_DrawTextRect.top = m_TextRectD.top + GetPos().y;
                ptextnode->m_DrawTextRect.bottom = m_TextRectD.bottom + GetPos().y;
                ptextnode->m_DrawTextRect.left = m_TextRectD.left + GetPos().x;
                ptextnode->m_DrawTextRect.right = m_TextRectD.right + GetPos().x;
                return ptextnode;
            }
            
            return NULL;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        /*
        void RichText2d::OnBlitText(LGB::VMCText* font)
        {
            LG::Rect rect = m_TextRectD;
        
            rect.top += GetPos().y;
            rect.bottom += GetPos().y;
            rect.left += GetPos().x;
            rect.right += GetPos().x;
            DrawText(font, m_StartLine, rect);
        }
        */
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        uint_t RichText2d::GetLineSize()
        {
            return cText_Node::GetLineSize();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        uint_t RichText2d::GetFocusLine()
        {
            return m_FocusLine;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::SetMaxStep( uint_t value )
        {
            m_Scroll->SetMaxStep( value );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::SetStartLine( uint_t pos )
        {
            if ( m_StartLine != pos )
            {
                m_Scroll->SetCurrStep( pos );
                m_StartLine = pos;
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::HighLight( uint_t line , bool hl )
        {
            if ( line > m_TextNode.size() )
            { return; }
            
            tNode* node = GetTextNode( line );
            
            if ( hl )
            {
                node->m_ForeColor = m_InverseFrontColor;
                node->m_BackColor = m_InverseBackColor;
            }
            else
            {
                node->m_ForeColor = m_FronColor;
                node->m_BackColor = m_BackColor;
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        const char* RichText2d::GetLineText( uint_t line )
        {
            if ( line > m_TextNode.size() )
            { return NULL; }
            
            tNode* node = GetTextNode( line );
            return node->m_Text.c_str();
        }
        
        //對字串列表進行操作
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::DeleteString( int index )
        {
            //若沒有資料,則不執行刪除的動作
            if ( m_TextNode.empty() )
            { return; }
            
            //若沒指定要刪除的行時,指定目前選擇到的字串
            if ( index == SV_NotAssignLine )
            {
                index = m_FocusLine;
                m_FocusLine = SV_NoFocusLine;
            }
            
            //刪除字串
            delete PopTextNode( index );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::DeleteAllString()
        {
            m_StartLine = 0;
            ReleaseAllTextNode();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::SortString( int index )
        {
            //若沒有資料,則不執行排序的動作
            if ( m_TextNode.empty() )
            { return; }
            
            // 若沒指定要刪除的行時,指定目前選擇到的字串
            if ( index == SV_NotAssignLine )
            {
                index = m_FocusLine;
                m_FocusLine = 0;
            }
            
            //把字串搬移到最前面
            m_TextNode.insert( m_TextNode.begin(), PopTextNode( index ) );
        }
        
        
        
        //=============================================================================
        // 訊息派送機制
        //=====================================================================
        // NAME:
        // DESC:
        //=====================================================================
        void RichText2d::OnUpdate( void )
        {
            SetStartLine( m_Scroll->GetCurrStep() );
        }
        
        //=====================================================================
        // NAME:
        // DESC:
        //=====================================================================
        void RichText2d::OnLMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point point )
        {
            if ( pWidget->GetId() == RiScroll )
            {
                // 捲動文字
                SetStartLine( m_Scroll->GetCurrStep() );
            }
            
            IWidget2d::OnLMouseDown( this, KeyCode, point );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::OnLMouseDown( int key, LG::Point point )
        {
            LG::Point currpoint = point - GetPos();
            //currpoint.x-=4;
            //currpoint.y-=4;
            currpoint = GetLineNumberByPoint( currpoint, m_StartLine, m_TextRectD );
            
            if ( currpoint.y < 0 )
            {
                HighLight( m_FocusLine, false );
                m_FocusLine = SV_NoFocusLine;
            }
            else
            {
                if ( m_FocusLine != currpoint.y )
                {
                    HighLight( m_FocusLine, false );
                    m_FocusLine = currpoint.y;
                    HighLight( m_FocusLine, true );
                }
            }
            
            //SetStartLine(m_FocusLine);XXX
            BaseClass::OnLMouseDown( key, point );
        }
        
        
#ifndef UNICODE
        void RichText2d::AddString( tchar_t* str, int fg, int bg )
        {
            int BSD;
            tchar_t buff[1024];
            m_FronColor = fg;
            m_BackColor = bg;
            int cxs = 0;
            int cys = 0;
            tNode* txtnode = GetTextNode( m_StartLine );
            
            if ( txtnode )
            {
                cxs = txtnode->m_TextSpace.cx;
                cys = txtnode->m_TextSpace.cy;
                m_FontSize.cx = txtnode->m_FontSize.cx;
                m_FontSize.cy = txtnode->m_FontSize.cy;
            }
            
            // 一個頁面可以放下幾行
            int line_num = ( m_TextRectD.bottom - m_TextRectD.top ) /
                           ( m_FontSize.cy + cys );
            // 一個頁面可以放下幾個字
            int str_num = ( m_TextRectD.right - m_TextRectD.left ) /
                          ( m_FontSize.cx + cxs );
                          
            while ( strlen( str ) > str_num )
            {
                BSD = GetMBCSCharLength( str, str_num - 1 );
                memcpy( buff, str, BSD );
                buff[BSD] = '\0';
                AddTextNode( buff,
                             0,
                             m_FontSize.cy,
                             0,
                             GetClipImageWidth(),
                             fg, bg, cxs, cys );
                str += BSD;
            }
            
            if ( *str != '\0' )
            {
                AddTextNode( str,
                             0,
                             m_FontSize.cy,
                             0,
                             GetClipImageWidth(),
                             fg, bg, cxs, cys );
            }
            
            if ( ( m_TextNode.size() - m_StartPos ) > line_num )
            {
                SetStartLine( m_TextNode.size() - line_num );
            }
            
            int currs = GetLineSize() - line_num;
            
            if ( currs < 0 )
            { currs = 0; }
            
            m_Scroll->SetMaxStep( currs );
            m_Scroll->SetCurrStep( currs );
            //m_Scroll->SetMaxStep(m_Scroll->GetCurrStep() + 1);
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void RichText2d::SetText( tchar_t* data, int index )
        {
            int size = m_TextNode.size();
            
            if ( size <= index )
            {
                while ( m_TextNode.size() < index )
                {
                    --index;
                    AddString( _T( "" ), m_FronColor, m_BackColor );
                }
                
                AddString( data, m_FronColor, m_BackColor );
            }
            else
            {
                SetTextNode( index, data );
            }
        }
        
#else // UNICODE
        //=============================================================================
        // NAME: AddString
        // DESC: AddString() Unicode version
        //=============================================================================
        void RichText2d::AddString( wchar_t* str, int fg, int bg )
        {
            // 用來暫放切段的字
            wchar_t* buff = ( wchar_t* )alloca( 2 * ( str_num + 1 ) );
            // 這裡有點沒有意義....
            m_FronColor = fg;
            m_BackColor = bg;
            int cxs = 0;
            int cys = 0;
            tNode* txtnode = GetTextNode( m_StartLine );
        
            if ( txtnode )
            {
                cxs = txtnode->m_TextSpace.cx;
                cys = txtnode->m_TextSpace.cy;
                m_FontSize.cx = txtnode->m_FontSize.cx;
                m_FontSize.cy = txtnode->m_FontSize.cy;
            }
        
            // 一個頁面可以放下幾行
            int line_num = ( m_TextRectD.bottom - m_TextRectD.top ) /
                           ( m_FontSize.cy + cys );
            // 一個頁面可以放下幾個字
            int str_num = ( m_TextRectD.right - m_TextRectD.left ) /
                          ( m_FontSize.cx + cxs );
        
            // 這裡，unicode版本，不好這樣用吧...寬度不是不一樣嗎...
            while ( wcslen( str ) > str_num )
            {
                wcsncpy( buff, str, str_num );
                buff[str_num] = 0;
                AddTextNode( buff,
                             0,
                             m_FontSize.cy,
                             0,
                             GetClipImageWidth(),
                             fg,
                             bg );
                str += str_num;
            }
        
            if ( *str != '\0' )
            {
                AddTextNode( str,
                             0,
                             m_FontSize.cy,
                             0,
                             GetClipImageWidth(),
                             fg,
                             bg );
            }
        
            if ( ( m_TextNode.size() - m_StartPos ) > line_num )
            {
                SetCurrLine( m_TextNode.size() - line_num );
            }
        
            m_Scroll->SetMaxStep( GetLineSize() - line_num );
            m_Scroll->SetCurrStep( GetLineSize() - line_num );
            //m_Scroll->SetMaxStep(m_Scroll->GetCurrStep() + 2);
        }
        //=============================================================================
        // NAME: SetText
        // DESC: SetText() Unicode version
        //=============================================================================
        void RichText2d::SetText( wchar_t* data, int index )
        {
            int size = m_TextNode.size();
        
            if ( size <= index )
            {
                while ( m_TextNode.size() < index )
                {
                    --index;
                    AddString( L_T( "" ), m_FronColor, m_BackColor );
                }
        
                AddString( data, m_FronColor, m_BackColor );
            }
            else
            {
                SetTextNode( index, data );
            }
        }
#endif
        
        bool RichText2d::SaveCfg( GspNode* toNode )
        {
            tchar_t buf[255];
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
            //    item->WriteCommand("EXPRESSION", _T("="), m_Expression.c_str());
            lltoa( GetId(), buf, 10 );
            item->WriteCommand( _T( "ID" ), _T( "=" ), buf );
            lltoa( GetGId(), buf, 10 );
            item->WriteCommand( _T( "GID" ), _T( "=" ), buf );
            sprintf( buf, "%d,%d,%d", GetPos().x, GetPos().y, GetPos().z );
            item->WriteCommand( _T( "POSITION" ), _T( "=" ), buf );
            
            if ( !m_ResoureName.empty() )
            { item->WriteCommand( _T( "RESIDNAME" ), _T( "=" ), m_ResoureName.c_str() ); }
            
            {
                if ( ( name = GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    item->WriteCommand( "LOAD_EDIT_RES", _T( "=" ), name );
                }
                
                if ( ( name = GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    item->WriteCommand( "LOAD_EDIT_ANM", _T( "=" ), name );
                }
            }
            
            //tNode *txtnode;
            
            for ( int i = 0; GetLineSize(); i++ )
            {
                tNode* txtnode = GetTextNode( i );
                
                if ( !txtnode )
                { break; }
                
                const char* pstr = txtnode->m_Text.c_str();
                
                if ( txtnode->m_Text.empty() )
                { pstr = _T( "" ); }
                
                sprintf( buf, _T( "\"%s\",%d,%d,%d,%d,%d,%d,%d,%d" ), pstr,
                         txtnode->m_TextRect.top - GetPos().y, txtnode->m_TextRect.bottom - GetPos().y,
                         txtnode->m_TextRect.left - GetPos().x, txtnode->m_TextRect.right - GetPos().x,
                         txtnode->m_ForeColor, txtnode->m_BackColor,
                         txtnode->m_TextSpace.cx, txtnode->m_TextSpace.cy );
                item->WriteCommand( _T( "TEXT" ), _T( "=" ), buf );
            }
            
            if ( GetComponentButton( RiScroll ) != NULL )
            {
                if ( ( name =  GetComponentButton( RiScroll )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_BAR_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name =  GetComponentButton( RiScroll )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_BAR_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( RiScTbUp ) != NULL )
            {
                if ( ( name = GetComponentButton( RiScTbUp )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbUP_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( RiScTbUp )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbUP_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( RiScUp ) != NULL )
            {
                if ( ( name = GetComponentButton( RiScUp )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_UP_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( RiScUp )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_UP_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( RiScDivider ) != NULL )
            {
                if ( ( name = GetComponentButton( RiScDivider )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DIVIDER_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( RiScDivider )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DIVIDER_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( RiScDown ) != NULL )
            {
                if ( ( name = GetComponentButton( RiScDown )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DOWN_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( RiScDown )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_DOWN_ANM" ), _T( "=" ), name ); }
                }
            }
            
            if ( GetComponentButton( RiScTbDown ) != NULL )
            {
                if ( ( name = GetComponentButton( RiScTbDown )->GetResfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbDOWN_RES" ), _T( "=" ), name ); }
                }
                
                if ( ( name = GetComponentButton( RiScTbDown )->GetAnmfname() ) != "" )
                {
                    const tchar_t* currname = strsplitstr( ( const char* )name, ( const char* )Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    { name = currname; }
                    
                    if ( strcmp( name, _T( "" ) ) != 0 )
                    { item->WriteCommand( _T( "LOAD_TbDOWN_ANM" ), _T( "=" ), name ); }
                }
            }
            
            LG::Rect rect = GetScrollRange();
            /*
            LG::Rect rect;
            rect.top  = __min(m_Scroll->GetComponentButton(Scroll2d::ScCTbUp)->GetPos().y,
                             m_Scroll->GetComponentButton(Scroll2d::ScCUp)->GetPos().y);
            rect.left = __min(m_Scroll->GetComponentButton(Scroll2d::ScCTbUp)->GetPos().x,
                             m_Scroll->GetComponentButton(Scroll2d::ScCUp)->GetPos().x);
            
            rect.right= __min(m_Scroll->GetComponentButton(Scroll2d::ScCTbDown)->GetPos().x+m_Scroll->GetComponentButton(Scroll2d::ScCTbDown)->GetWidth(),
                             m_Scroll->GetComponentButton(Scroll2d::ScCDown)->GetPos().x+m_Scroll->GetComponentButton(Scroll2d::ScCDown)->GetWidth());
            rect.bottom = __max(m_Scroll->GetComponentButton(Scroll2d::ScCTbDown)->GetPos().y+m_Scroll->GetComponentButton(Scroll2d::ScCTbDown)->GetHeight(),
                               m_Scroll->GetComponentButton(Scroll2d::ScCDown)->GetPos().y+m_Scroll->GetComponentButton(Scroll2d::ScCDown)->GetHeight());
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
            return true;
        }
        
    }; // namespace UI
}; // namespace GSO
