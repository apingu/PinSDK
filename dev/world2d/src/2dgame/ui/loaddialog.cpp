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
#pragma warning (disable : 4786)

#include <parser/pgsp_file.h>
#include "Dialog2d.h"
#include <pf-string.h>


namespace GSO
{
    namespace UI
    {
    
        // 使用巨集不成...
        static int GET_INT_VAL( tchar_t* s )
        {
            tchar_t* cptr = strtok( s, _T( "," ) );
            
            if ( cptr )
            {
                return atoi( cptr );
            }
            else
            {
                return 0;
            }
        }
        
        
        void MakeScroll2d( Scroll2d* pWidget, const tchar_t* attribute, const tchar_t* oper, const tchar_t* value );
        //=============================================================================
        //  name : LoadScript()
        // DESC: Load script language file
        //=============================================================================
        //class CDialogConfigure : public GspReadCallback
        class CDialogConfigure: public GspReadCallback
        {
            Dialog2d* RootDlg;
        public:
        
            CDialogConfigure( Dialog2d* Dlg )
            {
                RootDlg = Dlg;
            }
            ~CDialogConfigure()
            {
            };
            
            GspObject OnCreateObject( const tchar_t* name, const tchar_t* description, GspObject parent )
            {
                IWidget2d* node = ( IWidget2d* ) parent;
                
                //#ifdef _DEBUGLOG
                //                tchar_t buf[64];    sprintf(buf,"Create %s\n",name);    OutputDebugString(buf);
                //#endif // _DEBUGLOG
                
                if ( ( strcmp( name, _T( "2DSTATIC" ) ) == 0 ) || ( strcmp( name, _T( "STATIC2D" ) ) == 0 ) )
                {
                    Static2d* button = new Static2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                else if ( ( strcmp( name, _T( "ITEMARRAY2D" ) ) == 0 ) || ( strcmp( name, _T( "2DBTNARRAY" ) ) == 0 ) ) // 舊版相容
                {
                    ItemArray2d* button = new ItemArray2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DSCROLL") == 0)
                else if ( ( strcmp( name, _T( "2DSCROLL" ) ) == 0 ) ||
                          ( strcmp( name, _T( "SCROLL2D" ) ) == 0 ) )
                {
                    Scroll2d* button = new Scroll2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                else if ( ( strcmp( name, _T( "2DRICHTEXT" ) ) == 0 ) ||
                          ( strcmp( name, _T( "RICHTEXT2D" ) ) == 0 ) )
                {
                    RichText2d* button = new RichText2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DTEXTLIST") == 0)
                else if ( ( strcmp( name, _T( "2DTEXTLIST" ) ) == 0 ) ||
                          ( strcmp( name, _T( "TEXTLIST2D" ) ) == 0 ) )
                {
                    RichText2d* button = new RichText2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                /*
                else if (strcmp(name, "2DSCROLL2") == 0)
                {
                    RichText2d* button = new RichText2d;
                    node->AttachItem(button);
                    return button;
                }
                */
                //else if (strcmp(name, "2DEDIT") == 0)
                else if ( ( strcmp( name, _T( "2DEDIT" ) ) == 0 ) ||
                          ( strcmp( name, _T( "EDIT2D" ) ) == 0 ) )
                {
                    Edit2d* button = new Edit2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DTAB") == 0)
                else if ( ( strcmp( name, _T( "2DTAB" ) ) == 0 ) ||
                          ( strcmp( name, _T( "TAB2D" ) ) == 0 ) )
                {
                    Tab2d* button = new Tab2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DPAGE") == 0)
                else if ( ( strcmp( name, _T( "2DPAGE" ) ) == 0 ) ||
                          ( strcmp( name, _T( "PAGE2D" ) ) == 0 ) )
                {
                    Tab2d::Page2d* button = new Tab2d::Page2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DBUTTON") == 0)
                else if ( ( strcmp( name, _T( "2DBUTTON" ) ) == 0 ) ||
                          ( strcmp( name, _T( "BUTTON2D" ) ) == 0 ) )
                {
                    Button2d* button = new Button2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DCHECKBOX") == 0)
                else if ( ( strcmp( name, _T( "2DCHECKBOX" ) ) == 0 ) ||
                          ( strcmp( name, _T( "CHECKBOX2D" ) ) == 0 ) )
                {
                    CheckBox2d* button = new CheckBox2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DMENU") == 0)
                else if ( ( strcmp( name, _T( "2DMENU" ) ) == 0 ) ||
                          ( strcmp( name, _T( "MENU2D" ) ) == 0 ) )
                {
                    Menu2d* button = new Menu2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DPROGRESS") == 0)
                else if ( ( strcmp( name, _T( "2DPROGRESS" ) ) == 0 ) ||
                          ( strcmp( name, _T( "PROGRESS2D" ) ) == 0 ) )
                {
                    Progress2d* button = new Progress2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "2DAMIN") == 0)
                else if ( ( strcmp( name, _T( "2DAMIN" ) ) == 0 ) ||
                          ( strcmp( name, _T( "ANIM2D" ) ) == 0 ) )
                {
                    AminBtn2d* button = new AminBtn2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                /*
                                else if(strcmp(name, "2DRADIO") == 0)
                                {
                                    Button2d *button = (Button2d*)new Radio2d;
                                    node->AttachItem(button);
                                    return button;
                                }
                */
                /*
                else if ((strcmp(name, _T("2DMAP")) == 0) ||
                          (strcmp(name, _T("MAP2D")) == 0))
                {
                    Button2d* button = (Button2d*)new Map2d;
                    node->AttachItem(button);
                    return button;
                }
                
                //else if (strcmp(name, "2DFREERECT") == 0)
                else if ((strcmp(name, _T("2DFREERECT")) == 0) ||
                          (strcmp(name, _T("FREERECT2D")) == 0))
                {
                    Button2d* button = (Button2d*)new FreeRect2d;
                    node->AttachItem(button);
                    return button;
                }
                */
                else if ( ( strcmp( name, _T( "NUMBER2D" ) ) == 0 ) ||
                          ( strcmp( name, _T( "2DNUMBER" ) ) == 0 ) )
                {
                    Number2d* button = new Number2d;
                    button->m_Expression = description;
                    node->AttachItem( button );
                    return button;
                }
                //else if (strcmp(name, "DIALOG2D") == 0)
                else if ( ( strcmp( name, _T( "DIALOG2D" ) ) == 0 ) )
                {
                    RootDlg->m_Expression = description;
                    return RootDlg;
                }
                
                return NULL;
            }
            
            
            void OnReadAttribute( const tchar_t* name,
                                  GspObject object,
                                  const tchar_t* attribute,
                                  const tchar_t* oper,
                                  const tchar_t* value )
            {
                //                tchar_t buf[64];    sprintf(buf,"Read Attr %s\n",name);    OutputDebugString(buf);
                /////////////////////////////////////////////////////////////////////
                // 2DSCROLL
                //if (strcmp(name, "2DSCROLL") == 0)
                if ( ( strcmp( name, _T( "2DSCROLL" ) ) == 0 ) ||
                        ( strcmp( name, _T( "SCROLL2D" ) ) == 0 ) )
                {
                    MakeScroll2d( ( Scroll2d* ) object, attribute, oper, value );
                }
                /////////////////////////////////////////////////////////////////////
                // RichText2d
                //else if(strcmp(name, "2DRICHTEXT") == 0)
                else if ( ( strcmp( name, _T( "2DRICHTEXT" ) ) == 0 ) ||
                          ( strcmp( name, _T( "RICHTEXT2D" ) ) == 0 ) )
                {
                    RichText2d* button = ( RichText2d* ) object;
                    
                    if ( ( strcmp( attribute, _T( "LOAD_BAR_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_BAR" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScroll, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_TbUP_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_TbUP" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScTbUp, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_UP_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_UP" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScUp, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_DIVIDER_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_DIVIDER" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScDivider, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_DOWN_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_DOWN" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScDown, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_TbDOWN_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_TbDOWN" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiScTbDown, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_EDIT_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_EDIT" ) ) == 0 ) )
                    {
                        button->LoadRes( RichText2d::RiCText, value );
                        return;
                    }
                    else if ( ( strcmp( attribute, _T( "LOAD_SHADOW_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_SHADOW" ) ) == 0 ) )
                    {
                        //                        button->LoadRes(RichText2d::RiCShadow, value);
                        return;
                    }
                    
                    //////////
                    if ( strcmp( attribute, _T( "LOAD_BAR_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScroll, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_TbUP_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScTbUp, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_UP_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScUp, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_DIVIDER_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScDivider, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_DOWN_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScDown, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_TbDOWN_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiScTbDown, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_EDIT_ANM" ) ) == 0 )
                    {
                        button->LoadAnm( RichText2d::RiCText, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOAD_SHADOW_ANM" ) ) == 0 )
                    {
                        //                        button->LoadRes(RichText2d::RiCShadow, value);
                        return;
                    }
                    //////////
                    else if ( strcmp( attribute, _T( "SCROLL_RECT" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int top = GET_INT_VAL( ( tchar_t* ) buff );
                        int bottom = GET_INT_VAL( NULL );
                        int left = GET_INT_VAL( NULL );
                        int right = GET_INT_VAL( NULL );
                        button->SetScrollRange( top, bottom, left, right );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int sc = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LAYER" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, "unknow attribute %s", attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // 2DITEMARRAY
                //else if (strcmp(name, "2DBTNARRAY") == 0)
                else if ( ( strcmp( name, _T( "ITEMARRAY2D" ) ) == 0 ) ||
                          ( strcmp( name, _T( "2DBTNARRAY" ) ) == 0 ) )
                {
                    ItemArray2d* button = ( ItemArray2d* ) object;
                    
                    if ( strcmp( attribute, _T( "ARRAY" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        button->Create( x, atoi( strtok( NULL, _T( "," ) ) ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ITEMSIZE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        button->SetItemSize( x, atoi( strtok( NULL, _T( "," ) ) ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADALLCELLRES" ) ) == 0 )
                    {
                        button->LoadAllCellRes( ( tchar_t* ) value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADALLCELLANM" ) ) == 0 )
                    {
                        button->LoadAllCellAnm( ( tchar_t* ) value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "SPACE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        button->SetSpace( x, atoi( strtok( NULL, _T( "," ) ) ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADMASK" ) ) == 0 )
                    {
                        button->LoadMaskRes( ( tchar_t* ) value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADRES" ) ) == 0 )
                    {
                        LG::Size aAmount = button->GetAmount();
                        // 因為會被RELEASE掉
                        // 所以要重建一次
                        button->LoadRes( ( tchar_t* ) value );
                        button->Create( aAmount.cx, aAmount.cy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "CELLDRAGABLE" ) ) == 0 )
                    {
                        button->SetCellDragable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADANM" ) ) == 0 )
                    {
                        button->LoadAnm( ( tchar_t* ) value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LAYER" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, _T( "unknow attribute %s" ), attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // 2DMAP
                //else if (strcmp(name, "2DMAP") == 0)
                /*
                else if ((strcmp(name, _T("2DMAP")) == 0) ||
                          (strcmp(name, _T("MAP2D")) == 0))
                {
                    Map2d* button = (Map2d*) object;
                    if (strcmp(attribute, _T("LOADALPHA")) == 0)
                    {
                        button->LoadAlpha(value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOADRES")) == 0)
                    {
                        button->LoadRes(value);
                        return;
                    }
                    else if (strcmp(attribute, _T("ID")) == 0)
                    {
                        button->SetId(atoi(value));
                        return;
                    }
                    else if (strcmp(attribute, _T("GID")) == 0)
                    {
                        button->SetGId(atoi(value));
                        return;
                    }
                    else if (strcmp(attribute, _T("POSITION")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        int x = atoi(strtok(buff, _T(",")));
                        int y = atoi(strtok(NULL, _T(",")));
                        int z = 0;
                        char* zstr=strtok(NULL, _T(","));
                        if(zstr !=NULL )
                            z=atoi(zstr);
                        button->Sprite2d::SetPos(GameVertex(x, y, z)) ;
                        return;
                    }
                    else if (strcmp(attribute, _T("TEXT")) == 0)
                    {
                        tchar_t buff[_MAX_STRING_LEN];
                        strcpy(buff, value);
                        tchar_t* text = strtok((tchar_t*) buff, _T(","));
                        int t = GET_INT_VAL(NULL);
                        int b = GET_INT_VAL(NULL);
                        int l = GET_INT_VAL(NULL);
                        int r = GET_INT_VAL(NULL);
                        int fg = GET_INT_VAL(NULL);
                        int bg = GET_INT_VAL(NULL);
                        int sx = GET_INT_VAL(NULL);
                        int sy = GET_INT_VAL(NULL);
                        int len = strlen(text);
                        text[len-1]=0; //pass
                        button->AddTextNode((text+1), t, b, l, r, fg, bg, sx, sy);
                        return;
                    }
                    else if(strcmp(attribute, _T("PLAYMODE")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        int mode = atoi(strtok((tchar_t*) buff, _T(",")));
                        int num  = atoi(strtok(NULL,              _T(",")));
                        button->m_Anim2d.SetPlayMode((GSO::Anim2d::IMAGE_PLAY_MODE)mode, num);
                        return;
                    }
                    else if(strcmp(attribute, _T("APLHA")) == 0)
                    {
                        button->RenderAlpha=atoi(value);
                
                        // 設定繪圖的FUNCTION
                        if(button->RenderAlpha==256)
                            button->RenderMethod=LGB::DrawNode::RSNormal;
                        else
                            button->RenderMethod=LGB::DrawNode::RSDarken;
                        return;
                    }
                    else if(strcmp(attribute, _T("LAYER")) == 0)
                    {
                        LGB::DrawNode::DrawLayer layer = (LGB::DrawNode::DrawLayer)atoi(value);
                
                        if((layer<LGB::DrawNode::kLayerRole) || (layer>LGB::DrawNode::MAX_DrawLayerS))
                            layer=LGB::DrawNode::kLayerUI;
                
                        button->RenderLayer=layer;
                        return;
                    }
                    else if(strcmp(attribute, _T("DRAGABLE")) == 0)
                    {
                        button->SetDragable((bool)atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("VISIBLE")) == 0)
                    {
                        button->SetVisible((bool)atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("ENABLE")) == 0)
                    {
                        button->SetEnable(atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("EXPRESSION"))== 0)
                    {
                        // 注解
                        button->m_Expression=value;
                        return;
                    }
                    else if(strcmp(attribute, _T("RESIDNAME"))== 0)
                    {
                        // 注解
                        button->m_ResoureName=value;
                        return;
                    }
                    else if(strcmp(attribute, _T("POPUPTYPE")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        char* pType=strtok((tchar_t*)buff, _T(","));
                        char* pMsec=strtok((tchar_t*)NULL, _T(","));
                        int type = button->GetPopupType();
                        if(pType)
                        {
                            type = atoi(pType);
                        }
                        int msec = button->GetPopupMsec();
                        if(pMsec)
                        {
                            msec = atoi(pMsec);
                        }
                        button->SetPopupType((POPUP_TYPE)type, msec);
                        return;
                    }
                    else
                    {
                        tchar_t msg[255];
                        sprintf(msg, "unknow attribute %s", attribute);
                        assert(0||msg);
                        return;
                    }
                }
                
                /////////////////////////////////////////////////////////////////////
                // 2DFREERECT
                //else if (strcmp(name, "2DFREERECT") == 0)
                else if ((strcmp(name, _T("2DFREERECT")) == 0) ||
                          (strcmp(name, _T("FREERECT2D")) == 0))
                {
                    FreeRect2d* button = (FreeRect2d*) object;
                    if (strcmp(attribute, _T("LOAD_LT")) == 0)
                    {
                        button->Load(0, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_T")) == 0)
                    {
                        button->Load(1, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_RT")) == 0)
                    {
                        button->Load(2, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_L")) == 0)
                    {
                        button->Load(3, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_R")) == 0)
                    {
                        button->Load(4, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_LB")) == 0)
                    {
                        button->Load(5, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_B")) == 0)
                    {
                        button->Load(6, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOAD_RB")) == 0)
                    {
                        button->Load(7, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("LOADRES")) == 0)
                    {
                        button->Load(-1, value);
                        return;
                    }
                    else if (strcmp(attribute, _T("ID")) == 0)
                    {
                        button->SetId(atoi(value));
                        return;
                    }
                    else if (strcmp(attribute, _T("GID")) == 0)
                    {
                        button->SetGId(atoi(value));
                        return;
                    }
                    else if (strcmp(attribute, _T("POSITION")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        int x = atoi(strtok(buff, _T(",")));
                        int y = atoi(strtok(NULL, _T(",")));
                        int z = 0;
                        char* zstr=strtok(NULL, _T(","));
                        if(zstr !=NULL )
                            z=atoi(zstr);
                        button->Sprite2d::SetPos(GameVertex(x, y, z)) ;
                        return;
                    }
                    else if (strcmp(attribute, _T("TEXT")) == 0)
                    {
                        tchar_t buff[_MAX_STRING_LEN];
                        strcpy(buff, value);
                        tchar_t* text = strtok((tchar_t*) buff, _T(","));
                        int t = GET_INT_VAL(NULL);
                        int b = GET_INT_VAL(NULL);
                        int l = GET_INT_VAL(NULL);
                        int r = GET_INT_VAL(NULL);
                        int fg = GET_INT_VAL(NULL);
                        int bg = GET_INT_VAL(NULL);
                        int sx = GET_INT_VAL(NULL);
                        int sy = GET_INT_VAL(NULL);
                        int len = strlen(text);
                        text[len-1]=0; //pass
                        button->AddTextNode((text+1), t, b, l, r, fg, bg, sx, sy);
                        return;
                    }
                    else if(strcmp(attribute, _T("PLAYMODE")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        int mode = atoi(strtok((tchar_t*) buff, _T(",")));
                        int num  = atoi(strtok(NULL,              _T(",")));
                        button->m_Anim2d.SetPlayMode((GSO::Anim2d::IMAGE_PLAY_MODE)mode, num);
                        return;
                    }
                    else if(strcmp(attribute, _T("APLHA")) == 0)
                    {
                        button->RenderAlpha=atoi(value);
                
                        // 設定繪圖的FUNCTION
                        if(button->RenderAlpha==256)
                            button->RenderMethod=LGB::DrawNode::RSNormal;
                        else
                            button->RenderMethod=LGB::DrawNode::RSDarken;
                        return;
                    }
                    else if(strcmp(attribute, _T("PLAYMODE")) == 0)
                    {
                        LGB::DrawNode::DrawLayer layer = (LGB::DrawNode::DrawLayer)atoi(value);
                
                        if((layer<LGB::DrawNode::kLayerRole) || (layer>LGB::DrawNode::MAX_DrawLayerS))
                            layer=LGB::DrawNode::kLayerUI;
                
                        button->RenderLayer=layer;
                        return;
                    }
                    else if(strcmp(attribute, _T("DRAGABLE")) == 0)
                    {
                        button->SetDragable((bool)atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("VISIBLE")) == 0)
                    {
                        button->SetVisible((bool)atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("ENABLE")) == 0)
                    {
                        button->SetEnable(atoi(value));
                        return;
                    }
                    else if(strcmp(attribute, _T("EXPRESSION"))== 0)
                    {
                        // 注解
                        button->m_Expression=value;
                        return;
                    }
                    else if(strcmp(attribute, _T("RESIDNAME"))== 0)
                    {
                        // 注解
                        button->m_ResoureName=value;
                        return;
                    }
                    else if(strcmp(attribute, _T("POPUPTYPE")) == 0)
                    {
                        tchar_t buff[255];
                        strcpy(buff, value);
                        char* pType=strtok((tchar_t*)buff, _T(","));
                        char* pMsec=strtok((tchar_t*)NULL, _T(","));
                        int type = button->GetPopupType();
                        if(pType)
                        {
                            type = atoi(pType);
                        }
                        int msec = button->GetPopupMsec();
                        if(pMsec)
                        {
                            msec = atoi(pMsec);
                        }
                        button->SetPopupType((POPUP_TYPE)type, msec);
                        return;
                    }
                    else
                    {
                        tchar_t msg[255];
                        sprintf(msg, "unknow attribute %s", attribute);
                        assert(0||msg);
                        return;
                    }
                }
                */
                /////////////////////////////////////////////////////////////////////
                // 2DMENU
                //else if (strcmp(name, "2DMENU") == 0)
                else if ( ( strcmp( name, _T( "2DMENU" ) ) == 0 ) ||
                          ( strcmp( name, _T( "MENU2D" ) ) == 0 ) )
                {
                    Menu2d* button = ( Menu2d* ) object;
                    
                    /*
                                        else if(strcmp(attribute, "LOADSUBRES") == 0)
                                        {
                                            int   Id = atoi(strtok((tchar_t*)value, _T(",")));
                                            tchar_t* filename = strtok(NULL, _T(","));
                                            button->LoadSubRes(Id, filename);
                                            return;
                                        }
                    */
                    if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, _T( "unknow attribute %s" ), attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // 2DTAB
                //else if (strcmp(name, "2DTAB") == 0)
                else if ( ( strcmp( name, _T( "2DTAB" ) ) == 0 ) ||
                          ( strcmp( name, _T( "TAB2D" ) ) == 0 ) )
                {
                    Tab2d* button = ( Tab2d* ) object;
                    
                    if ( strcmp( attribute, _T( "LOADRES" ) ) == 0 )
                    {
                        button->LoadRes( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, "unknow attribute %s", attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // 2DPROGRESS
                else if ( ( strcmp( name, _T( "2DPROGRESS" ) ) == 0 ) ||
                          ( strcmp( name, _T( "PROGRESS2D" ) ) == 0 ) )
                    //else if (strcmp(name, "2DPROGRESS") == 0)
                {
                    Progress2d* button = ( Progress2d* ) object;
                    
                    if ( strcmp( attribute, _T( "LOADPANELRES" ) ) == 0 )
                    {
                        button->LoadRes( Progress2d::ProgressCom::PgCPanel, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADBARRES" ) ) == 0 )
                    {
                        button->LoadRes( Progress2d::ProgressCom::PgCBar, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADPANELANM" ) ) == 0 )
                    {
                        button->LoadAnm( Progress2d::ProgressCom::PgCPanel, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADBARANM" ) ) == 0 )
                    {
                        button->LoadAnm( Progress2d::ProgressCom::PgCBar, value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "SETMAXSTEP" ) ) == 0 )
                    {
                        button->SetMaxStep( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "SETCURRSTEP" ) ) == 0 )
                    {
                        button->SetCurrStep( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, "unknow attribute %s", attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // object
                else if ( strcmp( name, _T( "NUMBER2D" ) ) == 0 ||
                          strcmp( name, _T( "2DNUMBER" ) ) == 0 )
                {
                    Number2d* button = ( Number2d* ) object;
                    
                    if ( strcmp( attribute, _T( "NUMBER" ) ) == 0 )
                    {
                        ( *button ) = atoi( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ALIGN" ) ) == 0 )
                    {
                        // 注解
                        button->SetAlign( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "FIGURE" ) ) == 0 )
                    {
                        // 注解
                        button->SetFigure( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "XSPACE" ) ) == 0 )
                    {
                        // 注解
                        button->SetXSpace( atoi( value ) );
                        return;
                    }
                }
                
                /////////////////////////////////////////////////////////////////////
                // object
                if ( strcmp( name, _T( "2DSTATIC" ) ) == 0 ||
                        strcmp( name, _T( "2DBUTTON" ) ) == 0 ||
                        strcmp( name, _T( "2DEDIT" ) ) == 0 ||
                        strcmp( name, _T( "2DTEXTLIST" ) ) == 0 ||
                        strcmp( name, _T( "2DAMIN" ) ) == 0 ||
                        strcmp( name, _T( "2DCHECKBOX" ) ) == 0 ||
                        strcmp( name, _T( "2DPAGE" ) ) == 0 ||
                        strcmp( name, _T( "STATIC2D" ) ) == 0 ||
                        strcmp( name, _T( "BUTTON2D" ) ) == 0 ||
                        strcmp( name, _T( "EDIT2D" ) ) == 0 ||
                        strcmp( name, _T( "TEXTLIST2D" ) ) == 0 ||
                        strcmp( name, _T( "Amin2d" ) ) == 0 ||
                        strcmp( name, _T( "CHECKBOX2D" ) ) == 0 ||
                        strcmp( name, _T( "PAGE2D" ) ) == 0 ||
                        strcmp( name, _T( "NUMBER2D" ) ) == 0 ||
                        strcmp( name, _T( "2DNUMBER" ) ) == 0 )
                    //                      strcmp(name, "2DRADIO") == 0 ||
                {
                    IWidget2d* button = ( IWidget2d* ) object;
                    
                    if ( strcmp( attribute, _T( "LOADRES" ) ) == 0 )
                    {
                        button->LoadRes( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADANM" ) ) == 0 )
                    {
                        button->LoadAnm( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADHINTRES" ) ) == 0 )
                    {
                        //tchar_t* text = strtok((tchar_t*) value, _T(","));
                        button->LoadHintRes( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADHINTANM" ) ) == 0 )
                    {
                        //tchar_t* text = strtok((tchar_t*) value, _T(","));
                        button->LoadHintAnm( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "HINTTEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->SetHintText( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        button->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        button->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        button->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        //button->SetText((text+1));
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        button->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        button->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( button->RenderAlpha == 256 )
                        {
                            button->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            button->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        button->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        button->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        button->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        button->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        button->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        button->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = button->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = button->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        button->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, "unknow attribute %s", attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
                /////////////////////////////////////////////////////////////////////
                // DIALOG2D
                else if ( ( strcmp( name, _T( "DIALOG2D" ) ) == 0 ) ||
                          ( strcmp( name, _T( "Dialog2d" ) ) == 0 ) )
                {
                    Dialog2d* dialog = ( Dialog2d* ) object;
                    
                    if ( strcmp( attribute, _T( "LOADRES" ) ) == 0 )
                    {
                        dialog->LoadRes( value );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "LOADANM" ) ) == 0 )
                    {
                        dialog->LoadAnm( value );
                        return;
                    }
                    /*
                    else if (strcmp(attribute, _T("AFONT")) == 0)
                    {
                        tchar_t fontfname[_MAX_LPATH];
                        strcpy(fontfname, strtok((tchar_t*) value, _T(",")));
                        int w = atoi(strtok(NULL, _T(",")));
                        int h = atoi(strtok(NULL, _T(",")));
                        int d = atoi(strtok(NULL, _T(",")));
                        int fmt = atoi(strtok(NULL, _T(",")));
                        int cache = atoi(strtok(NULL, _T(",")));
                        dialog->m_Font.LoadFont(FONT_SET_SBCS,
                                                 fontfname,
                                                 w,
                                                 h,
                                                 d,
                                                 fmt,
                                                 cache);
                        return;
                    }
                    else if (strcmp(attribute, _T("CFONT")) == 0)
                    {
                        tchar_t fontfname[_MAX_LPATH];
                        strcpy(fontfname, strtok((tchar_t*) value, _T(",")));
                        int w = atoi(strtok(NULL, _T(",")));
                        int h = atoi(strtok(NULL, _T(",")));
                        int d = atoi(strtok(NULL, _T(",")));
                        int fmt = atoi(strtok(NULL, _T(",")));
                        int cache = atoi(strtok(NULL, _T(",")));
                        dialog->m_Font.LoadFont(FONT_SET_DBCS,
                                                 fontfname,
                                                 w,
                                                 h,
                                                 d,
                                                 fmt,
                                                 cache);
                        return;
                    }
                    */
                    else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                    {
                        dialog->SetId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                    {
                        dialog->SetGId( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int x = atoi( strtok( buff, _T( "," ) ) );
                        int y = atoi( strtok( NULL, _T( "," ) ) );
                        int z = 0;
                        char* zstr = strtok( NULL, _T( "," ) );
                        
                        if ( zstr != NULL )
                        {
                            z = atoi( zstr );
                        }
                        
                        dialog->Sprite2d::SetPos( GameVertex( x, y, z ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                    {
                        tchar_t buff[ _MAX_STRING_LEN ];
                        strcpy( buff, value );
                        tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                        int t = GET_INT_VAL( NULL );
                        int b = GET_INT_VAL( NULL );
                        int l = GET_INT_VAL( NULL );
                        int r = GET_INT_VAL( NULL );
                        int fg = GET_INT_VAL( NULL );
                        int bg = GET_INT_VAL( NULL );
                        int sx = GET_INT_VAL( NULL );
                        int sy = GET_INT_VAL( NULL );
                        int len = strlen( text );
                        text[ len - 1 ] = 0; //pass
                        dialog->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        int mode = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                        int num = atoi( strtok( NULL, _T( "," ) ) );
                        dialog->m_Anim2d.SetPlayMode( ( GSO::Anim2d::IMAGE_PLAY_MODE )mode, num );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "APLHA" ) ) == 0 )
                    {
                        dialog->RenderAlpha = atoi( value );
                        
                        // 設定繪圖的FUNCTION
                        if ( dialog->RenderAlpha >= _RENDER_FULLALPHA )
                        {
                            dialog->RenderMethod = LGB::DrawNode::RSNormal;
                        }
                        else
                        {
                            dialog->RenderMethod = LGB::DrawNode::RSDarken;
                        }
                        
                        return;
                    }
                    else if ( strcmp( attribute, _T( "PLAYMODE" ) ) == 0 )
                    {
                        LGB::DrawNode::DrawLayer layer = ( LGB::DrawNode::DrawLayer )atoi( value );
                        
                        if ( ( layer < LGB::DrawNode::kLayerRole ) || ( layer > LGB::DrawNode::MAX_DrawLayerS ) )
                        {
                            layer = LGB::DrawNode::kLayerUI;
                        }
                        
                        dialog->RenderLayer = layer;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "DRAGABLE" ) ) == 0 )
                    {
                        dialog->SetDragable( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "VISIBLE" ) ) == 0 )
                    {
                        dialog->SetVisible( ( bool ) atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "ENABLE" ) ) == 0 )
                    {
                        dialog->SetEnable( atoi( value ) );
                        return;
                    }
                    else if ( strcmp( attribute, _T( "EXPRESSION" ) ) == 0 )
                    {
                        // 注解
                        dialog->m_Expression = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "RESIDNAME" ) ) == 0 )
                    {
                        // 注解
                        dialog->m_ResoureName = value;
                        return;
                    }
                    else if ( strcmp( attribute, _T( "POPUPTYPE" ) ) == 0 )
                    {
                        tchar_t buff[ 255 ];
                        strcpy( buff, value );
                        char* pType = strtok( ( tchar_t* ) buff, _T( "," ) );
                        char* pMsec = strtok( ( tchar_t* ) NULL, _T( "," ) );
                        int type = dialog->GetPopupType();
                        
                        if ( pType )
                        {
                            type = atoi( pType );
                        }
                        
                        int msec = dialog->GetPopupMsec();
                        
                        if ( pMsec )
                        {
                            msec = atoi( pMsec );
                        }
                        
                        dialog->SetPopupType( ( POPUP_TYPE ) type, msec );
                        return;
                    }
                    else
                    {
                        tchar_t msg[ 255 ];
                        sprintf( msg, "unknow attribute %s", attribute );
                        assert( 0 || msg );
                        return;
                    }
                }
            }
            
            void OnCloseObject( const tchar_t* name, GspObject object )
            {
                return;
            }
            
            
            
            void MakeScroll2d( Scroll2d* button, const tchar_t* attribute, const tchar_t* oper, const tchar_t* value )
            {
                if ( ( strcmp( attribute, _T( "LOAD_BAR_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_BAR" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCBar, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_TbUP_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_TbUP" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCTbUp, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_UP_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_UP" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCUp, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_DIVIDER_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_DIVIDER" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCDivider, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_DOWN_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_DOWN" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCDown, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_TbDOWN_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_TbDOWN" ) ) == 0 ) )
                {
                    button->LoadRes( Scroll2d::ScCTbDown, value );
                    return;
                }
                else if ( ( strcmp( attribute, _T( "LOAD_SHADOW_RES" ) ) == 0 ) || ( strcmp( attribute, _T( "LOAD_SHADOW" ) ) == 0 ) )
                {
                    //                    button->LoadRes(Scroll2d::scCShadow, value);
                    return;
                }
                
                //
                //////////
                if ( strcmp( attribute, _T( "LOAD_BAR_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCBar, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_TbUP_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCTbUp, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_UP_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCUp, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_DIVIDER_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCDivider, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_DOWN_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCDown, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_TbDOWN_ANM" ) ) == 0 )
                {
                    button->LoadAnm( Scroll2d::ScCTbDown, value );
                    return;
                }
                else if ( strcmp( attribute, _T( "LOAD_SHADOW_ANM" ) ) == 0 )
                {
                    //                        button->LoadRes(RichText2d::RiCShadow, value);
                    return;
                }
                //////////
                else if ( strcmp( attribute, _T( "SCROLL_RECT" ) ) == 0 )
                {
                    tchar_t buff[ 255 ];
                    strcpy( buff, value );
                    int top = atoi( strtok( ( tchar_t* ) buff, _T( "," ) ) );
                    int bottom = atoi( strtok( NULL, _T( "," ) ) );
                    int left = atoi( strtok( NULL, _T( "," ) ) );
                    int right = atoi( strtok( NULL, _T( "," ) ) );
                    button->SetScrollRange( top, bottom, left, right );
                    return;
                }
                else if ( strcmp( attribute, _T( "POSITION" ) ) == 0 )
                {
                    tchar_t buff[ 255 ];
                    strcpy( buff, value );
                    int x = atoi( strtok( buff, _T( "," ) ) );
                    int y = atoi( strtok( NULL, _T( "," ) ) );
                    int z = 0;
                    char* zstr = strtok( NULL, _T( "," ) );
                    
                    if ( zstr != NULL )
                    {
                        z = atoi( zstr );
                    }
                    
                    button->SetPos( GameVertex( x, y, z ) );
                    return;
                }
                else if ( strcmp( attribute, _T( "ID" ) ) == 0 )
                {
                    button->SetId( atoi( value ) );
                    return;
                }
                else if ( strcmp( attribute, _T( "GID" ) ) == 0 )
                {
                    button->SetGId( atoi( value ) );
                    return;
                }
                else if ( strcmp( attribute, _T( "MAXSTEP" ) ) == 0 )
                {
                    button->SetMaxStep( atoi( value ) );
                    return;
                }
                //                 else if (strcmp(attribute, _T("CURRSTEP")) == 0)
                //                 {
                //                     button->SetCurrStep(atoi(value));
                //                     return;
                //                 }
                else if ( strcmp( attribute, _T( "TEXT" ) ) == 0 )
                {
                    tchar_t buff[ _MAX_STRING_LEN ];
                    strcpy( buff, value );
                    tchar_t* text = strtok( ( tchar_t* ) buff, _T( "," ) );
                    int t = GET_INT_VAL( NULL );
                    int b = GET_INT_VAL( NULL );
                    int l = GET_INT_VAL( NULL );
                    int r = GET_INT_VAL( NULL );
                    int fg = GET_INT_VAL( NULL );
                    int bg = GET_INT_VAL( NULL );
                    int sx = GET_INT_VAL( NULL );
                    int sy = GET_INT_VAL( NULL );
                    int len = strlen( text );
                    text[ len - 1 ] = 0; //pass
                    button->AddTextNode( ( text + 1 ), t, b, l, r, fg, bg, sx, sy );
                    return;
                }
            }
        };
    };//namespace GUI
};//namespace GE

