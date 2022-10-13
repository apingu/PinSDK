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
//
#include "IWidget2d.h"
#include <parser/pgsp_file.h>
//#include "CharCode.h"
#include <time.h>
#include <pf-time.h>
#include <pf-string.h>
#include <algorithm>
#include <pf-debug.h>


static pftml::Timer timer;

#define HINT_TOP    2
#define HINT_DOWN   14
#define HINT_LEFT   2
#define HINT_RIGHT  200

namespace GSO
{
    namespace UI
    {
        class _Hint: public GSO::UI::IWidget2d
        {
        public:
        
            /*
            virtual void OnBlitText(LGB::VMCText* font)
            {
                SetTextColor(0x01);
                GameVertex npos = GetPos();// + GameVertex(-20, 20);
                //DrawText(font, &GameVertex(npos + GameVertex(1, 0)));
                DrawText(font, &GameVertex(npos + GameVertex(0, 1, 0)));
                DrawText(font, &GameVertex(npos + GameVertex(1, 1, 0)));
                //DrawText(font, &GameVertex(npos + GameVertex(0, -1)));
            
                SetTextColor(0xffffffff);
                DrawText(font, &npos);
            };
            */
        };
        
        
        // 偽裝成一個iwidget
        class WidgetEffect
        {
            friend class GSO::UI::IWidget2d;
            
            IWidget2d*                 m_pIWidget;
            
            POPUP_TYPE                  m_PopupType;      // 出現特效的方式
            pftml::Timer                     m_PopupTimer;     // 出現特效的計時器
            mstimen_t                    m_PopupMSec;      // 出現特效的總時間
            bool                        m_bPopupUpdate;
            float                       m_Cnt;
            int                         m_PopupUpdateCnt;
            LGB::DrawNode::DrawMethod   m_LastRenderMethod;//上次的繪圖方式
            GameVertex                  m_LastPos;
            mstimen_t                    m_BeginTime;
            
        public:
        
            WidgetEffect( IWidget2d* iwidget )
            {
                m_PopupType = POPUP_TYPE_NONE;
                m_PopupMSec = 150;
                m_Cnt = 5.0;
                m_LastRenderMethod = iwidget->RenderMethod;
                m_bPopupUpdate = false;
                m_pIWidget = iwidget;
            }
            
            ~WidgetEffect()
            {
                m_pIWidget = NULL;
            }
            
            void SetPopupType( POPUP_TYPE popuptype, dword_t popupmsec )
            {
                m_PopupType = popuptype;
                m_PopupMSec = popupmsec;
                m_Cnt = m_PopupMSec / 50;
            }            
            
            void OpenVisibleEffect()
            {
                if ( m_pIWidget )
                {
                    m_BeginTime = pftml::Time::GetTickMSTime();
                    
                    switch ( m_PopupType )
                    {
                    // 用縮放的方式出現
                    case POPUP_TYPE_ZOOM:
                        {
                            m_pIWidget->RenderMethod = LGB::DrawNode::RSScaleAlpha;
                            m_PopupTimer.SetCycle( 50 );
                            m_PopupTimer.Reset();
                            m_pIWidget->RenderSizeCx = 0;
                            m_pIWidget->RenderSizeCy = 0;
                            // 記住原本的繪圖方式
                            m_LastRenderMethod = m_pIWidget->RenderMethod;
                            m_bPopupUpdate = true;
                            m_LastPos = m_pIWidget->GetPos();
                            m_PopupUpdateCnt = 0;
                            
                            for ( uint_t i = 0; i < m_pIWidget->GetItemCount(); i++ )
                            {
                                IWidget2d* iwedget = m_pIWidget->operator[]( i );
                                iwedget->SetPopupType( POPUP_TYPE_ZOOM, m_PopupMSec );
                                WidgetEffect* pWE = iwedget->m_pWidgetEffect;
                                pWE->OpenVisibleEffect();
                            }
                        }
                        break;
                        
                    case POPUP_TYPE_FADE:
                        {
                            m_pIWidget->RenderMethod = LGB::DrawNode::RSNormal;
                            // 每N秒加1 加到時間剛好滿
                            m_PopupTimer.SetCycle( 50 );
                            // 計時歸0
                            m_PopupTimer.Reset();
                            m_pIWidget->RenderAlpha = 0;
                            // 記住原本的繪圖方式
                            m_LastRenderMethod = m_pIWidget->RenderMethod;
                            m_bPopupUpdate = true;
                            m_PopupUpdateCnt = 0;
                            m_LastPos = m_pIWidget->GetPos();
                            
                            for ( uint_t i = 0; i < m_pIWidget->GetItemCount(); i++ )
                            {
                                IWidget2d* iwedget = m_pIWidget->operator[]( i );
                                iwedget->SetPopupType( POPUP_TYPE_FADE, m_PopupMSec );
                                WidgetEffect* pWE = iwedget->m_pWidgetEffect;
                                pWE->OpenVisibleEffect();
                            }
                        }
                        break;
                        
                    default:
                        {
                        }
                        break;
                    }
                }
            }
            
            
            void Update()
            {
                if ( m_pIWidget )
                {
                    if ( m_bPopupUpdate )
                    {
                        if ( m_pIWidget->GetVisible() )
                        {
                            switch ( m_PopupType )
                            {
                            // 用縮放的方式出現
                            case POPUP_TYPE_ZOOM:
                                {
                                    int w = m_pIWidget->GetWidth();
                                    int h = m_pIWidget->GetHeight();
                                    
                                    if ( ( m_pIWidget->RenderSizeCx < w ) ||
                                            ( m_pIWidget->RenderSizeCy < h ) )
                                    {
                                        if ( m_PopupTimer.Interval() >= TIMER_INTERRUPT )
                                        {
                                            Pf_INFO( "WidgetEffect::Update: %lldd Time %d\n", m_pIWidget->GetId(), pftml::Time::GetTickMSTime() - m_BeginTime );
                                            m_PopupUpdateCnt++;
                                            float fw = ( ( float ) w ) / m_Cnt;
                                            float fh = ( ( float ) h ) / m_Cnt;
                                            float fc = m_PopupUpdateCnt;
                                            m_pIWidget->RenderSizeCx = fw * fc;
                                            m_pIWidget->RenderSizeCy = fh * fc;
                                            
                                            if ( m_pIWidget->RenderSizeCx >= w )
                                            {
                                                m_pIWidget->RenderSizeCx = w;
                                            }
                                            
                                            if ( m_pIWidget->RenderSizeCy >= h )
                                            {
                                                m_pIWidget->RenderSizeCy = h;
                                            }
                                            
                                            for ( uint_t i = 0; i < m_pIWidget->GetItemCount(); i++ )
                                            {
                                                IWidget2d* iwedget = m_pIWidget->operator[]( i );
                                                WidgetEffect* pWE = iwedget->m_pWidgetEffect;
                                                float fpx = ( ( ( float ) pWE->m_LastPos.x ) - ( ( float ) m_LastPos.x ) );
                                                float fpy = ( ( ( float ) pWE->m_LastPos.y ) - ( ( float ) m_LastPos.y ) );
                                                float fx = fpx / m_Cnt;
                                                float fy = fpy / m_Cnt;
                                                int newx = m_pIWidget->GetPos().x + ( fx * fc );
                                                int newy = m_pIWidget->GetPos().y + ( fy * fc );
                                                
                                                if ( newx < pWE->m_LastPos.x )
                                                {
                                                    iwedget->GetPos().x = newx;
                                                }
                                                else
                                                {
                                                    iwedget->GetPos().x = pWE->m_LastPos.x;
                                                }
                                                
                                                if ( newy < pWE->m_LastPos.y )
                                                {
                                                    iwedget->GetPos().y = newy;
                                                }
                                                else
                                                {
                                                    iwedget->GetPos().y = pWE->m_LastPos.y;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        m_pIWidget->RenderMethod = m_LastRenderMethod;
                                        m_bPopupUpdate = false;
                                        m_pIWidget->GetPos() = m_LastPos;
                                    }
                                }
                                break;
                                
                            // 用淡入淡出的方式出現
                            case POPUP_TYPE_FADE:
                                {
                                    if ( m_pIWidget->RenderAlpha < _RENDER_FULLALPHA )
                                    {
                                        if ( m_PopupTimer.Interval() >= TIMER_INTERRUPT )
                                        {
                                            Pf_INFO( "WidgetEffect::Update: %lldd Time %d\n", m_pIWidget->GetId(), pftml::Time::GetTickMSTime() - m_BeginTime );
                                            m_pIWidget->RenderAlpha += ( _RENDER_FULLALPHA / m_Cnt );
                                            m_PopupUpdateCnt++;
                                        }
                                    }
                                    else
                                    {
                                        m_pIWidget->RenderAlpha = _RENDER_FULLALPHA;
                                        m_pIWidget->RenderMethod = m_LastRenderMethod;
                                        m_bPopupUpdate = false;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
            
        };
        
        
        IWidget2d* IWidget2d::s_pFocusWidget = 0;
        
        //找拖到誰家
        GSO::Element2d* SearchForWhoDragIn( GSO::Element2d* elem, void* pData )
        {
            GSO::Element2d* p = elem;
            
            while ( p != NULL )
            {
                if ( !p->GetVisible() )
                {
                    return NULL;
                }
                
                if ( !p->GetEnable() )
                {
                    return NULL;
                }
                
                p = p->GetParent();
            }
            
            //          if (elem->GetVisible() == false)
            //              return NULL;
            LG::Point* pfstl = ( LG::Point* )pData;
            
            if ( elem->GetCurEvenMsg() == EventMessage::MOUSEDRAGDROP )
            {
                return NULL;
            }
            
            if ( elem->IsCollision( *pfstl ) == TRUE )
            {
                return elem;
            }
            
            return NULL;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d::IWidget2d()
            : m_pHint( NULL ), m_pDesc( NULL ), m_bTestLODrag( false )
        {
            RenderLayer = LGB::DrawNode::kLayerUI;
            m_bModal = false;
            m_pWidgetEffect = new WidgetEffect( this );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d::~IWidget2d()
        {
            if ( s_pFocusWidget == this )
            {
                s_pFocusWidget = NULL;
            }
            
            SAFE_DELETE( m_pHint );
            SAFE_DELETE( m_pDesc );
            DestroyAllItem();
            SAFE_DELETE( m_pWidgetEffect );
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        int IWidget2d::Update( int count )
        {
            // 不可見還是得 update
            //if (!GetVisible())
            //  return 0;
            ( ( WidgetEffect* ) m_pWidgetEffect )->Update();
            UpdateRoute();
            return BaseClass::Update( count );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::Clear( int s )
        {
            SAFE_DELETE( m_pHint );
            SAFE_DELETE( m_pDesc );
            m_Caption.clear();
            BaseClass::Clear( s );
        }
        
        //=============================================================================
        // NAME:
        // DESC: 為了要變動Z  讓子物件的Z值不會小於父物件
        //=============================================================================
        /*
        GSO::Element2d* IWidgetZValueChildren(GSO::Element2d* elem, void* pDate)
        {
            GameVertex* pos = (GameVertex*)pDate;
            GameVertex newpos=elem->GetPos();
            if(newpos.z<pos->z);
            {
                newpos.z=pos->z;
                elem->Sprite2d::SetPos(newpos);
            }
            return NULL;
        }
        
        void IWidget2d::SetPos(GameVertex pos)
        {
            Element2d::SetPos(pos);
        
            // 擺在後面做 讓 如果原本z大於的子物件 先做一次相對的移動
            ForAllChildren(IWidgetZValueChildren, &pos);
        }
        */
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetTestLODrag( bool f )
        {
            m_bTestLODrag = f;
        }
        
        
        ///////////////////////////////////////////////////////////////////////////////
        // item funtion group
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::DestroyAllItem( void )
        {
            while ( GetItemCount() != 0 )
            {
                GSO::UI::IWidget2d* pWidget = ( GSO::UI::IWidget2d* )IWidget2d::operator[]( 0 );
                IWidget2d::DetachItem( pWidget );
                // ???
                pWidget->DestroyAllItem();
                SAFE_DELETE( pWidget );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool IWidget2d::ReplaceItem( const IWidget2d* oldc, IWidget2d* newc )
        {
            return ReplaceChild( oldc, newc );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* IWidget2d::GetRootItem( void )
        {
            return ( IWidget2d* ) BaseClass::GetRootElement();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* IWidget2d::GetParentItem( void )
        {
            return ( IWidget2d* ) BaseClass::GetParent();
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool IWidget2d::AttachItem( IWidget2d* item )
        {
            if ( !IsStaticId( item->GetId() ) )
            {
                IWidget2d* pFindWidget = ( ( IWidget2d* ) GetRootElement() )->GetItem( item->GetId() );
                
                if ( pFindWidget != NULL )
                {
                    assert( 0 && "item id double attach" );
                }
            }
            
            return BaseClass::AttachChild( item );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool IWidget2d::DetachItem( IWidget2d* item )
        {
            return BaseClass::DetachChild( item );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        uint_t IWidget2d::GetItemCount( void ) const
        {
            return GetChildSize();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* IWidget2d::operator[]( uint_t i )
        {
            return ( IWidget2d* ) BaseClass::operator[]( i );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* IWidget2d::GetItem( entityid_t index )
        {
            return ( IWidget2d* ) GetChild( index );
        }
        
        
        
        
        ///////////////////////////////////////////////////////////////////////////////
        //
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetFocusWidget( void )
        {
            SetFocusWidget( this );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        bool SoryByDepth( GSO::Element2d* elem1, GSO::Element2d* elem2 )
        {
            //DrawNode* icon0 = ((DrawNode*)elem1);
            //DrawNode* icon1 = ((DrawNode*)elem2);
            if ( ( elem1 )->GetPos().z < ( elem2 )->GetPos().z )
            {
                return true;
            }
            
            return false;
        }
        
        GSO::Element2d* SortWidgetItem( GSO::Element2d* elem, void* pDate )
        {
            IWidget2d* pWidget = ( IWidget2d* ) elem;
            std::stable_sort( pWidget->GetChildrenList()->begin(), pWidget->GetChildrenList()->end(), SoryByDepth );
            return NULL;
        }
        
        
        void IWidget2d::SetFocusWidget( IWidget2d* pWidget )
        {
            if ( s_pFocusWidget == pWidget )
            {
                return;
            }
            
            s_pFocusWidget = pWidget;
            
            if ( pWidget == NULL )
            {
                return;
            }
            
            IWidget2d* widget_parent = ( IWidget2d* ) pWidget->GetParent();
            IWidget2d* curr_widget = pWidget;
            
            while ( widget_parent != NULL )
            {
                widget_parent->DetachItem( curr_widget );
                widget_parent->AttachItem( curr_widget );
                std::stable_sort( widget_parent->GetChildrenList()->begin(), widget_parent->GetChildrenList()->end(), SoryByDepth );
                //              std::vector<IWidget2d*> modelvec;
                //              for(int i = 0; i<widget_parent->GetItemCount(); i++)
                //              {
                //                  IWidget2d* wed=(*widget_parent)[i];
                //                  if(wed->m_bModal)
                //                  {
                //                      modelvec.push_back(wed);
                //                  }
                //              }
                //              for(int i = 0; i<modelvec.size(); i++)
                //              {
                //                  widget_parent->DetachItem(modelvec[i]);
                //                  widget_parent->AttachItem(modelvec[i]);
                //              }
                curr_widget = widget_parent;
                widget_parent = ( IWidget2d* ) curr_widget->GetParent();
            }
            
            //pWidget->GetMainElement()->ForAllChildren(SortWidgetItem, NULL);
        }
        
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        IWidget2d* IWidget2d::GetFocusWidget()
        {
            return s_pFocusWidget;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::DoModal( bool f )
        {
            if ( m_bModal = f )
            {
                IWidget2d* widget_parent = ( IWidget2d* ) GetParent();
                
                if ( widget_parent != NULL )
                {
                    widget_parent->DetachItem( this );
                    widget_parent->AttachItem( this );
                }
            }
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::OnRenderTo( rendercontainer_t& container )
        {
            BaseClass::OnRenderTo( container );
            
            if ( ( m_pHint != NULL ) && ( m_pHint->GetVisible() ) )
            {
                m_pHint->OnRenderTo( container );
            }
            
            if ( ( m_pDesc != NULL ) && ( m_pDesc->GetVisible() ) )
            {
                m_pDesc->OnRenderTo( container );
            }
        }
        
        void* IWidget2d::OnGetTextNode()
        {
            if ( GetVisible() )
            {
                cText_Node* ptextnode = ( cText_Node* )this;
                ptextnode->m_TextMethod = TMShadow;
                ptextnode->m_TextPosition = GetPos();
                return ptextnode;
            }
            
            return NULL;
        }
        
        /*
        void IWidget2d::OnBlitText(LGB::VMCText* font)
        {
            if (m_TextNode.empty())
                return;
        
            cText_Node::tNode* n = GetTextNode(0);
        
            int fcolor = n->m_ForeColor;
            n->m_ForeColor=n->m_ShadowColor;
            GameVertex npos = GetPos();// + GameVertex(-20, 20);
            //DrawText(font, &GameVertex(npos + GameVertex(1, 0)));
            DrawText(font, &GameVertex(npos + GameVertex(0, 1, 0)));
            DrawText(font, &GameVertex(npos + GameVertex(1, 1, 0)));
        
            n->m_ForeColor=fcolor;
            DrawText(font, &npos);
        
            //DrawText(font, &GetPos());
        }
        */
        
        
        const TCHAR* IWidget2d::GetText( int index )
        {
            tNode* node = GetTextNode( index );
            
            if ( node != NULL )
            {
                return node->m_Text.c_str();
            }
            
            return NULL;
        }
        
        void IWidget2d::SetText( const TCHAR* text, int index )
        {
            if ( text == NULL )
            {
                return;
            }
            
            cText_Node::tNode* n = NULL;
            
            while ( ( n = GetTextNode( index ) ) == NULL )
            {
                AddTextNode( "", 0, GetClipImageHeight(), 0, GetClipImageWidth() );
            }
            
            cText_Node::SetText( text, index );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::LoadHintRes( const tchar_t* filename )
        {
            if ( m_pHint == NULL )
            {
                m_pHint = new _Hint;
            }
            
            //m_pHint->Release();
            m_pHint->LoadRes( filename );
            //m_HintSubPos.x =0-(m_pHint->GetClipImageWidth() / 4);
            //m_HintSubPos.y =0-(m_pHint->GetClipImageHeight());
            m_pHint->SetVisible( false );
        }
        
        void IWidget2d::LoadHintAnm( const tchar_t* filename )
        {
            if ( m_pHint == NULL )
            {
                m_pHint = new _Hint;
            }
            
            //m_pHint->Release();
            m_pHint->LoadAnm( filename );
            //m_pHint->SetVisible(false);
        }
        
        void IWidget2d::SetHintState( uibehave_t state )
        {
            if ( m_pHint )
            {
                m_pHint->SetState( state );
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetHintText( const tchar_t* text, int top, int bottom, int left, int right, int fg, int bg, int sx, int sy )
        {
            if ( m_pHint == NULL )
            {
                m_pHint = new _Hint;
            }
            
#ifdef UNICODE
            TCHAR mybuffer[ 1024 ];
            Ansi2Unicode( CP_ACP, text, mybuffer );
            m_pHint->SetTextNode( 0, mybuffer, text, top, bottom, left, right, fg, bg, sx, sy );
            return;
#else
            m_pHint->SetTextNode( 0, text, top, bottom, left, right, fg, bg, sx, sy );
            return;
#endif
        }
        //=============================================================================
        // NAME: GetHintText
        // DESC: Get hint text. By Wood Woo.
        //=============================================================================
        const tchar_t* IWidget2d::GetHintText()
        {
            if ( m_pHint == NULL )
            {
                return NULL;
            }
            
            tNode* tn = m_pHint->GetTextNode( 0 );
            
            if ( tn != NULL )
            {
                return tn->m_Text.c_str();
            }
            
            return NULL;
        }
        
        
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::LoadDesc( const tchar_t* filename )
        {
            if ( m_pDesc == NULL )
            {
                m_pDesc = new IWidget2d;
            }
            
            m_pDesc->Clear();
            m_pDesc->LoadRes( filename );
            m_pDesc->AddTextNode( _T( "" ) );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetDescText( const tchar_t* text )
        {
            if ( m_pDesc == NULL )
            {
                m_pDesc = new IWidget2d;
                m_pDesc->AddTextNode( _T( "" ), HINT_TOP, HINT_DOWN, HINT_LEFT, HINT_RIGHT, 256, 0 );
                //m_pDesc->AddTextNode(_T(""), 4, 49, 4, 150, 256, 0);
            }
            
            m_pDesc->SetTextNode( 0, ( tchar_t* ) text, HINT_TOP, HINT_DOWN, HINT_LEFT, HINT_RIGHT );
        }
        
        
        //=============================================================================
        // NAME:
        // DESC: 改變狀態
        //=============================================================================
        void IWidget2d::SetState( uibehave_t state )
        {
            BaseClass::FaceTo( state );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        uibehave_t IWidget2d::GetState()
        {
            return ( uibehave_t ) ( int ) BaseClass::GetNowAngle();
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetVisible( bool f )
        {
            //          if (f)
            //              SetFocusWidget(this);
            //          else
            //              SetFocusWidget(NULL);
            // 要顯現該原件
            if ( ( GetVisible() == false ) && ( f == true ) )
            {
                ( ( WidgetEffect* ) m_pWidgetEffect )->OpenVisibleEffect();
            }
            
            BaseClass::SetVisible( f );
            
            if ( !f )
            {
                // 只要一有人被關起來 就把整個FocusWidget清空 以免Query的時候有問題
                // 抓到看不見的Widget
                IWidget2d* focus = GetFocusWidget();
                
                if ( focus )
                {
                    if ( !focus->GetVisible() )
                    {
                        SetFocusWidget( NULL );
                    }
                }
            }
        }
        
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetEnable( bool f )
        {
            BaseClass::SetEnable( f );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::SetPopupType( POPUP_TYPE popuptype, dword_t popupmsec )
        {
            ( ( WidgetEffect* ) m_pWidgetEffect )->SetPopupType( popuptype, popupmsec );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        POPUP_TYPE IWidget2d::GetPopupType()
        {
            return ( ( WidgetEffect* ) m_pWidgetEffect )->m_PopupType;
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        dword_t IWidget2d::GetPopupMsec()
        {
            return ( ( WidgetEffect* ) m_pWidgetEffect )->m_PopupMSec;
        }
        
        
        
        /*
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::OnSortChilds(std::vector<GSO::Element2d*>& childs)
        {
        //            if (s_pFocusWidget != 0)
        //                return;
            for (uint_t i = 0; i < childs.size(); i++)
            {
                if (childs[i]==GetFocusWidget())
                {
                    IWidget2d* elem = (IWidget2d*)childs[i];
        
                    IWidget2d* widget_parent = this;
                    IWidget2d* curr_widget = elem;
                    while (widget_parent != NULL)
                    {
                        widget_parent->DetachItem(curr_widget);
                        widget_parent->AttachItem(curr_widget);
                        curr_widget = widget_parent;
                        widget_parent = (IWidget2d*)curr_widget->GetParent();
                    }
        
                    return;
                }
            }
        }
        */
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        /*
        IWidget2d* IWidget2d::ForAllChildWidget(OnElementChildren func, void* pDate)
        {
            if (!GetVisible())
                return NULL;
        
            for(uint_t i = GetChildSize(); i > 0; --i)
            {
                IWidget2d* pWidget = operator[](i-1)->ForAllChildWidget(func, pDate);
                if(pWidget!= NULL)
                    return pWidget;
            }
            if(func!= NULL)
                return (IWidget2d*)(*func)(this, pDate);
            return NULL;
        }
        */
        
        
        ///////////////////////////////////////////////////////////////////////////////
        // Input事件，客端程式請勿覆寫
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::OnNull( LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            if ( m_pHint != NULL )
            {
                m_pHint->SetVisible( false );
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnNormal( this, point );
            }
            else
            {
                OnNormal( this, point );
            }
        }
        
        void IWidget2d::OnMouseOver( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            if ( m_pHint != NULL )
            {
                m_pHint->SetVisible( false );
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnMouseOver( this, key, point );
            }
            else
            {
                OnMouseOver( this, key, point );
            }
        }
        
        /*
                        void IWidget2d::OnMouseWheel(int key, LG::Point point)
                        {
                            if (!GetEnable())
                                return;
        
                            if (m_pHint != NULL)
                                m_pHint->SetVisible(false);
        
                            IWidget2d* parent = (IWidget2d*)GetParent();
                            if (parent != NULL)
                                parent->OnMouseWheel(this, key, point);
                            else
                                OnMouseWheel(this, key, point);
                        }*/
        
        
        void IWidget2d::OnLMouseDown( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            // 致能
            SetFocusWidget();
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnLMouseDown( this, key, point );
            }
            else
            {
                OnLMouseDown( this, key, point );
            }
        }
        void IWidget2d::OnRMouseDown( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnRMouseDown( this, key, point );
            }
            else
            {
                OnRMouseDown( this, key, point );
            }
        }
        void IWidget2d::OnLMouseUp( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnLMouseUp( this, key, point );
            }
            else
            {
                OnLMouseUp( this, key, point );
            }
        }
        void IWidget2d::OnRMouseUp( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnRMouseUp( this, key, point );
            }
            else
            {
                OnRMouseUp( this, key, point );
            }
        }
        
        void IWidget2d::OnLDBClick( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnLDBClick( this, key, point );
            }
            else
            {
                OnLDBClick( this, key, point );
            }
        }
        void IWidget2d::OnRDBClick( int key, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnRDBClick( this, key, point );
            }
            else
            {
                OnRDBClick( this, key, point );
            }
        }
        
        void IWidget2d::OnUserNotify( uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnUserNotify( this, uMsg, Param1, Param1, point );
            }
            else
            {
                OnUserNotify( this, uMsg, Param1, Param1, point );
            }
        }
        
        
        void IWidget2d::OnDrag( LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnDrag( this, point );
            }
            else
            {
                OnDrag( this, point );
            }
        }
        
        void IWidget2d::OnDragDrop( LG::Point point )
        {
            if ( !GetEnable() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                OnDragDrop( this, point );
                return;
            }
            
            parent->OnDragDrop( this, point );
            
            if ( !m_bTestLODrag )
            {
                return;
            }
            
            IWidget2d* pWidget = ( IWidget2d* ) GetRootElement()->ForAllChildren( SearchForWhoDragIn, &point );
            
            if ( pWidget != NULL )
            {
                pWidget->OnLODragIn( pWidget, this, point );
            }
            
            OnLODragOut( parent, this, point, ( pWidget != NULL ) );
        }
        void IWidget2d::OnKeyDown( int key )
        {
            if ( GetFocusWidget() == NULL )
            {
                return;
            }
            
            if ( !GetFocusWidget()->GetEnable() )
            {
                return;
            }
            
            if ( GetFocusWidget() != NULL )
            {
                GetFocusWidget()->OnKeyDown( GetFocusWidget(), key );
            }
            else
            {
                OnKeyDown( GetFocusWidget(), key );
            }
            
            return;
        }
        void IWidget2d::OnKeyUp( int key )
        {
            if ( GetFocusWidget() == NULL )
            {
                return;
            }
            
            if ( !GetFocusWidget()->GetEnable() )
            {
                return;
            }
            
            if ( GetFocusWidget() != NULL )
            {
                GetFocusWidget()->OnKeyUp( GetFocusWidget(), key );
            }
            else
            {
                OnKeyUp( GetFocusWidget(), key );
            }
            
            return;
            /*
                        if (GetEnable())
                        {
                            IWidget2d* parent = (IWidget2d*)GetParent();
                            if(parent == NULL)
                            {
                                OnKeyUp(this, key);
                                return;
                            }
                            parent->OnKeyUp(this, key);
                        }
            */
        }
        
        void IWidget2d::OnChar( unsigned nChar )
        {
            if ( GetFocusWidget() == NULL )
            {
                return;
            }
            
            if ( !GetFocusWidget()->GetEnable() )
            {
                return;
            }
            
            if ( GetFocusWidget() != NULL )
            {
                GetFocusWidget()->OnChar( GetFocusWidget(), nChar );
            }
            else
            {
                OnChar( GetFocusWidget(), nChar );
            }
            
            return;
        }
        
        void IWidget2d::OnMouseWheel( int key, LG::Point point )
        {
            if ( GetFocusWidget() == NULL )
            {
                return;
            }
            
            if ( !GetFocusWidget()->GetEnable() )
            {
                return;
            }
            
            OnMouseWheel( GetFocusWidget(), key, point );
        }
        
        void IWidget2d::OnDock( dword_t docktime, LG::Point point )
        {
            if ( GetEnable() )
            {
                if ( ( m_pHint != NULL ) && ( !m_pHint->GetVisible() ) ) //不然會一直跳位置
                {
                    // 要先設為VISIBLE 否則 下面的 長寬會取不到
                    m_pHint->SetVisible( true );
                    //                  LG::Point hpos = point;
                    //                  hpos.x -= m_pHint->GetClipImageWidth() / 4;
                    //                  hpos.y -= m_pHint->GetClipImageHeight();
                    m_pHint->SetPos( point );
                }
                
                IWidget2d* parent = ( IWidget2d* ) GetParent();
                
                if ( parent == NULL )
                {
                    OnDock( this, docktime, point );
                    return;
                }
                
                parent->OnDock( this, docktime, point );
            }
        }
        
        void IWidget2d::OnShow( bool f )
        {
            if ( !GetVisible() )
            {
                return;
            }
            
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnShow( this, f );
            }
            else
            {
                OnShow( this, f );
            }
        }
        
        void IWidget2d::OnMoving( const GameVertex& pos )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnMoving( this, pos );
            }
            else
            {
                OnMoving( this, pos );
            }
        }
        
        /////////////////////////////////////////////////////////////////////////////
        // 內部往上派送機制，不得覆寫，只有dialog的才可以覆寫
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        void IWidget2d::OnNormal( IWidget2d* pWidget, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnNormal( pWidget, MouseSite );
        }
        void IWidget2d::OnKeyDown( IWidget2d* pWidget, int KeyCode )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnKeyDown( pWidget, KeyCode );
        }
        void IWidget2d::OnKeyUp( IWidget2d* pWidget, int KeyCode )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnKeyUp( pWidget, KeyCode );
        }
        void IWidget2d::OnChar( IWidget2d* pWidget, unsigned nChar )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnChar( pWidget, nChar );
        }
        /*
        void IWidget2d::OnMouseMove(IWidget2d* pWidget, int KeyCode, LG::Point MouseSite)
        {
            IWidget2d* parent = (IWidget2d*)GetParent();
            if(parent == NULL)
                return;
        
            parent->OnMouseMove(pWidget, KeyCode, MouseSite);
        }
        */
        void IWidget2d::OnMouseOver( IWidget2d* pWidget,
                                     int KeyCode,
                                     LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnMouseOver( pWidget, KeyCode, MouseSite );
        }
        
        void IWidget2d::OnMouseWheel( IWidget2d* pWidget,
                                      int KeyCode,
                                      LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnMouseWheel( pWidget, KeyCode, MouseSite );
        }
        
        void IWidget2d::OnLDBClick( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnLDBClick( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnRDBClick( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnRDBClick( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnLMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnLMouseDown( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnRMouseDown( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnRMouseDown( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnLMouseUp( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnLMouseUp( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnRMouseUp( IWidget2d* pWidget, int KeyCode, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnRMouseUp( pWidget, KeyCode, MouseSite );
        }
        void IWidget2d::OnDrag( IWidget2d* pWidget, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnDrag( pWidget, MouseSite );
        }
        void IWidget2d::OnDragDrop( IWidget2d* pWidget, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnDragDrop( pWidget, MouseSite );
        }
        void IWidget2d::OnDock( IWidget2d* pWidget, dword_t docktime, LG::Point point )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnDock( pWidget, docktime, point );
        }
        void IWidget2d::OnLODragIn( IWidget2d* pWidget, IWidget2d* sender, LG::Point MouseSite )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent != NULL )
            {
                parent->OnLODragIn( pWidget, sender, MouseSite );
            }
        }
        void IWidget2d::OnLODragOut( IWidget2d* pWidget, IWidget2d* sendobj, LG::Point MouseSite, bool is_catched )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnLODragOut( pWidget, sendobj, MouseSite, is_catched );
        }
        
        void IWidget2d::OnUserNotify( IWidget2d* pWidget, uint_t uMsg, uint_t Param1, uint_t Param2, LG::Point point )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnUserNotify( pWidget, uMsg, Param1, Param2, point );
        }
        
        void IWidget2d::OnShow( IWidget2d* pWidget, bool f )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnShow( pWidget, f );
        }
        
        void IWidget2d::OnMoving( IWidget2d* pWidget, const GameVertex pos )
        {
            IWidget2d* parent = ( IWidget2d* ) GetParent();
            
            if ( parent == NULL )
            {
                return;
            }
            
            parent->OnMoving( pWidget, pos );
        }
        
        
        // 存檔用. 將屬性寫入 node ===============================================
        bool IWidget2d::SaveCfg( GspNode* toNode )
        {
            tchar_t buf[ 255 ];
            const tchar_t* pstr;
            pfstd::CString name = GetResfname();
            // 改大寫
            tchar_t* objname = new tchar_t[ strlen( ObjectName() ) + 1 ];
            strcpy( objname, ObjectName() );
            strtoupper( objname );
            GspNode* item = new GspNode( objname, m_Expression.c_str() );
            SAFE_DELETE_ARRAY( objname );
            toNode->AttachChildNode( item );
            // 共通部分
            //if(!m_Expression.empty())
            //  item->WriteCommand(_T("EXPRESSION"), _T("="), m_Expression.c_str());
            lltoa( GetId(), buf, 10 );
            item->WriteCommand( _T( "ID" ), _T( "=" ), buf );
            lltoa( GetGId(), buf, 10 );
            item->WriteCommand( _T( "GID" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d,%d,%d" ), GetPos().x, GetPos().y, GetPos().z );
            item->WriteCommand( _T( "POSITION" ), _T( "=" ), buf );
            
            if ( !m_ResoureName.empty() )
            {
                item->WriteCommand( _T( "RESIDNAME" ), _T( "=" ), m_ResoureName.c_str() );
            }
            
            name = GetResfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                {
                    name = currname;
                }
                
                item->WriteCommand( _T( "LOADRES" ), _T( "=" ), name );
            }
            
            name = GetAnmfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                {
                    name = currname;
                }
                
                item->WriteCommand( _T( "LOADANM" ), _T( "=" ), name );
            }
            
            for ( int i = 0; 1; i++ )
            {
                tNode* txtnode = GetTextNode( i );
                
                if ( !txtnode )
                {
                    break;
                }
                
                pstr = txtnode->m_Text.c_str();
                
                if ( txtnode->m_Text.empty() )
                {
                    pstr = _T( "" );
                }
                
                sprintf( buf, _T( "\"%s\",%d,%d,%d,%d,%d,%d,%d,%d" ), pstr,
                         txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                         txtnode->m_TextRect.left, txtnode->m_TextRect.right,
                         txtnode->m_ForeColor, txtnode->m_BackColor,
                         txtnode->m_TextSpace.cx, txtnode->m_TextSpace.cy );
                item->WriteCommand( _T( "TEXT" ), _T( "=" ), buf );
            }
            
            if ( m_pHint )
            {
                name = m_pHint->GetResfname();
                
                if ( !name.empty() )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    {
                        name = currname;
                    }
                    
                    item->WriteCommand( _T( "LOADHINTRES" ), _T( "=" ), name );
                }
                
                name = m_pHint->GetAnmfname();
                
                if ( !name.empty() )
                {
                    const tchar_t* currname = strsplitstr( name, Sprite2d::GetCurrentDirectory() );
                    
                    if ( currname != NULL )
                    {
                        name = currname;
                    }
                    
                    item->WriteCommand( _T( "LOADHINTANM" ), _T( "=" ), name );
                }
                
                if ( GetHintText() )
                {
                    for ( int i = 0; 1; i++ )
                    {
                        tNode* txtnode = m_pHint->GetTextNode( i );
                        
                        if ( !txtnode )
                        {
                            break;
                        }
                        
                        pstr = txtnode->m_Text.c_str();
                        
                        if ( txtnode->m_Text.empty() )
                        {
                            pstr = _T( "" );
                        }
                        
                        sprintf( buf, _T( "\"%s\",%d,%d,%d,%d,%d,%d,%d,%d" ), pstr,
                                 txtnode->m_TextRect.top, txtnode->m_TextRect.bottom,
                                 txtnode->m_TextRect.left, txtnode->m_TextRect.right,
                                 txtnode->m_ForeColor, txtnode->m_BackColor,
                                 txtnode->m_TextSpace.cx, txtnode->m_TextSpace.cy );
                        item->WriteCommand( _T( "HINTTEXT" ), _T( "=" ), buf );
                    }
                }
            }
            
            sprintf( buf, _T( "%d,%d" ), m_Anim2d.GetPlayMode(), m_Anim2d.GetPlayInterrupt() );
            item->WriteCommand( _T( "PLAYMODE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), RenderAlpha );
            item->WriteCommand( _T( "ALPHA" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), RenderLayer );
            item->WriteCommand( _T( "LAYER" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d,%d" ), GetPopupType(), GetPopupMsec() );
            item->WriteCommand( _T( "POPUPTYPE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetDragable() );
            item->WriteCommand( _T( "DRAGABLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetVisible() );
            item->WriteCommand( _T( "VISIBLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetEnable() );
            item->WriteCommand( _T( "ENABLE" ), _T( "=" ), buf );
            
            for ( unsigned int i = 0; i < GetItemCount(); i++ )
            {
                IWidget2d* pWidget = operator[]( i );
                pWidget->SaveCfg( item );
            }
            
            return true;
        }
        
        bool IWidget2d::IsStaticId( entityid_t wid )
        {
            if ( ( wid >= 1000 ) && ( wid != GSO_STATIC_ID ) )
            {
                return false;
            }
            
            return true;
        }
        
        const tchar_t* IWidget2d::GetAnsiText( int index )
        {
#ifndef UNICODE
            return GetText( index );
#else
            static tchar_t mybuffer[ 1024 ];
            const TCHAR* mytext = GetText( index );
            
            if ( mytext == NULL )
            {
                return NULL;
            }
            
            Unicode2Ansi( CP_ACP, mytext, mybuffer, 1024 );
            return mybuffer;
#endif
        }
        
        void IWidget2d::SetAnsiText( const tchar_t* txt, int index )
        {
#ifndef UNICODE
            SetText( ( TCHAR* ) txt, index );
#else
            TCHAR mybuffer[ 1024 ];
            Ansi2Unicode( CP_ACP, txt, mybuffer );
            SetText( mybuffer, index );
#endif
        }
        
        bool IWidget2d::OnCollision( GameVertex pos )
        {
            // 因為 在ui中 z是先後順序 卻沒有影響圖形畫的位置 所以先把 滑鼠減回來
            pos.y -= GetPos().z;
            return IsCollisionByImage( pos );
        }
        
        // 移到同一個層級的最上面
        void IWidget2d::MoveTop()
        {
            IWidget2d* widget_parent = ( IWidget2d* ) GetParent();
            
            if ( widget_parent != NULL )
            {
                widget_parent->DetachItem( this );
                widget_parent->AttachItem( this );
            }
        }
    };
    
};

//tchar_t* IWidget2d_GetLoadResName(GSO::UI::IWidget2d *wid)
//{
//  tchar_t* name = (tchar_t*)wid->GetFileName();
//    if (GSO::Sprite2d::GetCurrentDirectory() != NULL)
//    {
//        tchar_t* s;
//        s = strstr(name, GSO::Sprite2d::GetCurrentDirectory());
//        if (s != NULL)
//        {
//            name = s;
//            name += (strlen(GSO::Sprite2d::GetCurrentDirectory()));
//        }
//    }
//  return name;
//}

