//Scene_Editer.cpp  scene edit class
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
//                                                                  Pin
//

#include "App2dFactory.h"
#include <assert.h>
#include <PFFile.h>
#include <PFString.h>


const int _MAP_RECT_LIMIT_ = 50;
//bool      Form2d::s_bSelLock=false;

//void Form2d::DragTo(GameVertex new_norm)
//{
//	using namespace GSO;
//	Dialog2d::DragTo(new_norm);
//}

namespace GSO
{
    namespace Global
    {
        extern bool g_Element2dEffectChildren;
    };
};


namespace GSO
{
    namespace Global
    {
        extern bool g_Element2dEffectChildren;
    };
};

namespace App2dFactory
{
    Widget::Widget()
    {
        GSO::Global::g_Element2dEffectChildren = false;
    }
    
    Widget::~Widget()
    {
        GSO::Global::g_Element2dEffectChildren = true;
    }
    
    
    void Widget::SetAllChildrenEnable( GSO::UI::IWidget2d* bu, bool v )
    {
        if ( bu == NULL )
        { return; }
        
        bu->SetEnable( v );
        
        for ( UINT i = 0; i < bu->GetItemCount(); i++ )
        {
            SetAllChildrenEnable( ( *bu )[i], v );
        }
        
        return;
    }
    void Widget::SetAllChildrenVisible( GSO::UI::IWidget2d* bu, bool v )
    {
        if ( bu == NULL )
        { return; }
        
        bu->SetEnable( v );
        
        for ( UINT i = 0; i < bu->GetItemCount(); i++ )
        {
            SetAllChildrenVisible( ( *bu )[i], v );
        }
        
        return;
    }
    void Widget::SetAllChildrenDragable( GSO::UI::IWidget2d* bu, bool v )
    {
        if ( bu == NULL )
        { return; }
        
        bu->SetEnable( v );
        
        for ( UINT i = 0; i < bu->GetItemCount(); i++ )
        {
            SetAllChildrenDragable( ( *bu )[i], v );
        }
        
        return;
    }
    void Widget::SetAllChildrenMoveable( GSO::UI::IWidget2d* bu, bool v )
    {
        if ( bu == NULL )
        { return; }
        
        bu->SetEnable( v );
        
        for ( UINT i = 0; i < bu->GetItemCount(); i++ )
        {
            SetAllChildrenMoveable( ( *bu )[i], v );
        }
        
        return;
    }
    
    bool Widget::ForAllChildrenT2B( GSO::UI::IWidget2d* root, ForAllChildrenWidgetCB ccd, void* pdata )
    {
        if ( ( *ccd )( root, pdata ) == false )
        { return false; }
        
        for ( UINT i = 0; i < root->GetItemCount(); i++ )
        {
            if ( ForAllChildrenT2B( ( *root )[i], ccd, pdata ) == false )
            { return false; }
        }
        
        return true;
    }
    
    bool Widget::ForAllChildrenB2T( GSO::UI::IWidget2d* root, ForAllChildrenWidgetCB ccd, void* pdata )
    {
        for ( UINT i = 0; i < root->GetItemCount(); i++ )
        {
            if ( ForAllChildrenB2T( ( *root )[i], ccd, pdata ) == false )
            { return false; }
        }
        
        if ( ( *ccd )( root, pdata ) == false )
        { return false; }
        
        return true;
    }
    
    
    LG::Point Widget::GetLClickRef( GSO::UI::IWidget2d* bu )
    {
        //LG::Point pt = bu->GetPos()-bu->GetRelativePos();
        //if(pt==LG::Point(0, 0))
        //	return bu->GetPos();
        return bu->GetPos();
        //return g_ClickPoint[kLClick0]-bu->GetPos();
    }
    
    
    //////////////////////////////////////////////////////////////////////////
    //
    Form2d::Form2d()
    {
        //AttachItem(m_pDialog);
        m_pDialog = NULL;
    }
    
    Form2d::~Form2d()
    {
        DetachItem( m_pDialog );
        SAFE_DELETE( m_pDialog );
    }
    
    void Form2d::SaveData( void )
    {
        // TODO: Alex,
#if 0
        HElem2d data;
        GetElemData( &data );
        CPElementFactory::SaveData( this, data );
        return;
#endif
    }
    
    
    GSO::Element2d* SaveAllWidgetFunc( GSO::Element2d* elem, void* pDate )
    {
        int* cnt = ( int* )pDate;
        ( *cnt )++;
        elem->SaveRes();
        return NULL;
    }
    
    int Form2d::SaveAllWidget( void )
    {
        int ii = 0;
        
        if ( WidgetCount() == 0 )
        { return 0; }
        
        int cnt = 0;
        ForAllChildren( SaveAllWidgetFunc, &cnt, true, true, false );
        return ii;
    }
    
    
    int    _FORM_SCROLL_STEP_ = 0;
    
    struct ProcMessage
    {
        POINT point;
        UINT uMsg;
        UINT wParam;
    };
    
    
    bool QueryAllChildren( GSO::UI::IWidget2d* iwidget, void* pdata )
    {
        ProcMessage* msg = ( ProcMessage* )pdata;
        int message = 0;
        //GSO::UI::IWidget2d* focuswidget = GSO::UI::IWidget2d::GetFocusWidget();
        GameVertex oldpos = iwidget->GetPos();
        bool_t enable   = iwidget->GetEnable();
        bool_t visible  = iwidget->GetVisible();
        bool_t dragable = iwidget->GetDragable();
        iwidget->SetEnable( TRUE );
        iwidget->SetVisible( TRUE );
        iwidget->SetDragable( TRUE );
        message = iwidget->Proc( msg->point, msg->uMsg, msg->wParam );
        iwidget->SetEnable( enable );
        iwidget->SetVisible( visible );
        iwidget->SetDragable( dragable );
        
        //GSO::UI::IWidget2d::SetFocusWidget(focuswidget);
        
        if ( message == PE_LBUTTONDOWN ) //event happen to one button
        {
            GSO::UI::IWidget2d::SetFocusWidget( iwidget );
            return false;
        }
        
        return true;
    }
    
    void Form2d::Destroy()
    {
        DetachItem( m_pDialog );
        SAFE_DELETE( m_pDialog );
    }
    
    const char* Form2d::GetCfgFilename()
    {
        if ( m_pDialog )
        { 
            return m_pDialog->GetCfgFilename(); 
        }
        
        return "";
    }
    
    bool Form2d::IsCreate()
    {
        if ( m_pDialog )
        { 
            return true; 
        }
        
        return false;
    }
    
    int Form2d::LoadCfg( const char* filename )
    {
        IWidget2d::DetachItem( m_pDialog );
        SAFE_DELETE( m_pDialog );
        m_pDialog = new GSO::UI::Dialog2d;
        int r = m_pDialog->LoadCfg( filename );
        IWidget2d::AttachItem( m_pDialog );
        return r;
    }
    
    int Form2d::SaveCfg( const char* filename )
    {
        if ( m_pDialog )
        {
            //m_pDialog->Update();
            return m_pDialog->SaveCfg( filename );
        }
        
        return -1;
    }
    
    bool Form2d::AttachItem( IWidget2d* item )
    {
        if ( m_pDialog == NULL )
        {
            if ( strcmp( item->ObjectName(), "Dialog2d" ) == 0 )
            {
                m_pDialog = ( GSO::UI::Dialog2d* )item;
                IWidget2d::AttachItem( m_pDialog );
                return true;
            }
            else
            {
                m_pDialog = new GSO::UI::Dialog2d;
                IWidget2d::AttachItem( m_pDialog );
            }
        }
        else if ( m_pDialog == item )
        {
            IWidget2d::DetachItem( m_pDialog );
            IWidget2d::AttachItem( m_pDialog );
            return true;
        }
        
        return m_pDialog->AttachItem( item );
    }
    
    void Form2d::ProcB( POINT point, UINT uMsg, UINT wParam )
    {
        ProcMessage msg;
        msg.point = point;
        msg.uMsg = uMsg;
        msg.wParam = wParam;
        IWidget2d* focuswidget = GetFocusWidget();
        
        if ( focuswidget != NULL )
        {
            //if(uMsg == WM_LBUTTONDOWN)
            //{
            //    int kkk=0;
            //}
            if ( uMsg == WM_KEYDOWN )
            {
                GameVertex widgetpos;
                
                switch ( wParam )
                {
                case VK_UP:
                    widgetpos.y--;
                    break;
                    
                case VK_DOWN:
                    widgetpos.y++;
                    break;
                    
                case VK_LEFT:
                    widgetpos.x--;
                    break;
                    
                case VK_RIGHT:
                    widgetpos.x++;
                    break;
                    
                case VK_PRIOR:
                    {
                        // 把選中的ENTITY 高度+1
                        widgetpos.z++;
                    }
                    break;
                    
                case VK_NEXT:
                    {
                        // 把選中的ENTITY 高度+1
                        widgetpos.z--;
                    }
                    break;
                }
                
                std::vector<GSO::Element2d*> pList;
                GetAllSelected( pList );
                
                // 單選
                if ( ( pList.size() == 1 ) &&
                        ( pList[0] == focuswidget ) )
                {
                    GameVertex newpos = focuswidget->GetPos() + widgetpos;
                    
                    if ( GetAsyncKeyState( VK_CONTROL ) )
                    {
                        focuswidget->Sprite2d::SetPos( newpos );
                    }
                    else
                    {
                        focuswidget->SetPos( newpos );
                    }
                }
                else
                {
                    GameVertex newpos = focuswidget->GetPos() + widgetpos;
                    //focuswidget->SetPos(newpos);
                    bool ff = false;
                    std::vector<GSO::Element2d*> pList;
                    GetAllSelected( pList );
                    
                    for ( size_t i = 0; i < pList.size(); i++ )
                    {
                        if ( pList[i] == focuswidget )
                        {
                            ff = true;
                        }
                        
                        GameVertex newpos = pList[i]->GetPos() + widgetpos;
                        pList[i]->Sprite2d::SetPos( newpos );
                    }
                    
                    if ( !ff )
                    {
                        focuswidget->SetPos( newpos );
                    }
                }
                
                return;
            }
        }
        
        //SetFocusWidget(NULL);
        //if(GetAsyncKeyState(VK_CONTROL))
        //{
        //Widget::ForAllChildrenB2T(this, QueryAllChildren, &msg);
        //}
        //else
        {
            GSO::UI::IWidget2d* iwidget = GSO::UI::IWidget2d::GetFocusWidget();
            
            if ( iwidget )
            {
                GameVertex oldpos   = iwidget->GetPos();
                bool_t enable       = iwidget->GetEnable();
                bool_t visible      = iwidget->GetVisible();
                bool_t dragable     = iwidget->GetDragable();
                bool_t dragchildren = iwidget->GetDragChildren();
                
                if ( GetAsyncKeyState( VK_CONTROL ) )
                { iwidget->SetDragChildren( false ); }
                
                iwidget->SetEnable( TRUE );
                iwidget->SetVisible( TRUE );
                iwidget->SetDragable( TRUE );
                bool bb = GSO::Global::g_Element2dEffectChildren;
                GSO::Global::g_Element2dEffectChildren = false;
                int message = iwidget->Proc( msg.point, msg.uMsg, msg.wParam );
                GSO::Global::g_Element2dEffectChildren = bb;
                GameVertex newpos   = iwidget->GetPos();
                iwidget->SetEnable( enable );
                iwidget->SetVisible( visible );
                iwidget->SetDragable( dragable );
                iwidget->SetDragChildren( dragchildren );
                
                if ( message == PE_MOUSEDRAG )
                {
                    GameVertex widgetpos = newpos - oldpos;
                    std::vector<GSO::Element2d*> pList;
                    GetAllSelected( pList );
                    
                    // 單選
                    if ( ( pList.size() == 1 ) &&
                            ( pList[0] == focuswidget ) )
                    {
                    }
                    else
                    {
                        focuswidget->SetPos( oldpos );
                        //GameVertex newpos = focuswidget->GetPos()+widgetpos;
                        //focuswidget->SetPos(newpos);
                        bool ff = false;
                        std::vector<GSO::Element2d*> pList;
                        GetAllSelected( pList );
                        
                        for ( int i = 0; i < pList.size(); i++ )
                        {
                            if ( pList[i] == focuswidget )
                            {
                                ff = true;
                            }
                            
                            GameVertex newpos = pList[i]->GetPos() + widgetpos;
                            pList[i]->Sprite2d::SetPos( newpos );
                        }
                        
                        if ( !ff )
                        {
                            focuswidget->SetPos( newpos );
                        }
                    }
                }
            }
        }
        //if(GSO::UI::IWidget2d::GetFocusWidget()!=this)
        //SetFocus(_LOST_FOCUS);
    }
    
    
    
    struct UIGetFnameResult
    {
        const char*         fname;
        GSO::UI::IWidget2d* result;
    };
    
    
    bool QueryAllChildrenFname( GSO::UI::IWidget2d* iwidget, void* pdata )
    {
        UIGetFnameResult* result = ( UIGetFnameResult* )pdata;
        
        //if(iwidget->GetResfname().empty())
        if ( strcmp( iwidget->GetResfname(), "" ) == 0 )
        { return true; }
        
        if ( strcmp( iwidget->GetResfname(), result->fname ) == 0 )
        {
            result->result = iwidget;
            return false;
        }
        
        return true;
    }
    
    GSO::UI::IWidget2d* Form2d::GetEntityByFname( const char* fname )
    {
        UIGetFnameResult result;
        result.fname = fname;
        result.result = NULL;
        Widget::ForAllChildrenB2T( this, QueryAllChildrenFname, ( void* )( &result ) );
        return result.result;
    }
    
    std::string PossibleMakeCopyFliename( const char* fromfname )
    {
        // 不存在 就用直接用
        if ( !PfFileExist( fromfname ) )
        { return fromfname; }
        
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];
        _splitpath( fromfname, drive, dir, fname, ext );
        // c:\AAAA\cccc\ff(0).ee
        char newfname[_MAX_PATH];
        int ccn = 0;
        
        do
        {
            sprintf( newfname, "%s%s%s(%d)%s", drive, dir, fname, ccn, ext );
            ccn++;
        }
        while ( PfCopyFileEx( fromfname, newfname, true ) != 1 );
        
        return newfname;
    }
    
    GSO::UI::IWidget2d* NewWidgetFromWidget( GSO::UI::IWidget2d* pWidget, bool bCopyFile )
    {
        GSO::UI::IWidget2d* pCloneWidget;
        std::string resfname;
        std::string anmfname;
        
        if ( !bCopyFile )
        {
            resfname = pWidget->GetResfname();
            anmfname = pWidget->GetAnmfname();
        }
        else
        {
            resfname = PossibleMakeCopyFliename( pWidget->GetResfname() );
            anmfname = PossibleMakeCopyFliename( pWidget->GetAnmfname() );
        }
        
        if ( strcmp( pWidget->ObjectName(), "IWidget2d" ) == 0 )
        {
            GSO::UI::IWidget2d* pNewWidget = new GSO::UI::IWidget2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Static2d"  ) == 0 )
        {
            GSO::UI::Static2d* pNewWidget = new GSO::UI::Static2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Edit2d"    ) == 0 )
        {
            GSO::UI::Edit2d* pNewWidget = new GSO::UI::Edit2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Button2d"  ) == 0 )
        {
            GSO::UI::Button2d* pNewWidget = new GSO::UI::Button2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "AminBtn2d" ) == 0 )
        {
            GSO::UI::AminBtn2d* pNewWidget = new GSO::UI::AminBtn2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "CheckBox2d" ) == 0 )
        {
            GSO::UI::CheckBox2d* pNewWidget = new GSO::UI::CheckBox2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Menu2d"    ) == 0 )
        {
            GSO::UI::Menu2d* pNewWidget = new GSO::UI::Menu2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "ItemArray2d" ) == 0 )
        {
            GSO::UI::ItemArray2d* pNewWidget = new GSO::UI::ItemArray2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Number2d"  ) == 0 )
        {
            GSO::UI::Number2d* pNewWidget = new GSO::UI::Number2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Scroll2d"  ) == 0 )
        {
            GSO::UI::Scroll2d* pNewWidget = new GSO::UI::Scroll2d;
            
            for ( int i = 0; i < GSO::UI::Scroll2d::ScCMAX; i++ )
            {
                GSO::UI::Scroll2d::ScrollCom ii = ( GSO::UI::Scroll2d::ScrollCom )i;
                GSO::UI::IWidget2d* pSubWidget = ( ( GSO::UI::Scroll2d* )pWidget )->GetComponentButton( ii );
                
                if ( pSubWidget )
                {
                    std::string subresfname;
                    std::string subanmfname;
                    
                    if ( !bCopyFile )
                    {
                        std::string subresfname = pWidget->GetResfname();
                        std::string subanmfname = pWidget->GetAnmfname();
                    }
                    else
                    {
                        subresfname = PossibleMakeCopyFliename( pWidget->GetResfname() );
                        subanmfname = PossibleMakeCopyFliename( pWidget->GetAnmfname() );
                    }
                    
                    pNewWidget->LoadRes( ii, subresfname.c_str() );
                    pNewWidget->LoadAnm( ii, subanmfname.c_str() );
                    pNewWidget->SetPos( pSubWidget->GetPos() );
                }
            }
            
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "RichText2d" ) == 0 )
        {
            GSO::UI::RichText2d* pNewWidget = new GSO::UI::RichText2d;
            
            for ( int i = 0; i < GSO::UI::RichText2d::RiCMax; i++ )
            {
                GSO::UI::RichText2d::RichTextCom ii = ( GSO::UI::RichText2d::RichTextCom )i;
                GSO::UI::IWidget2d* pSubWidget = ( ( GSO::UI::RichText2d* )pWidget )->GetComponentButton( ii );
                
                if ( pSubWidget )
                {
                    std::string subresfname;
                    std::string subanmfname;
                    
                    if ( !bCopyFile )
                    {
                        std::string subresfname = pWidget->GetResfname();
                        std::string subanmfname = pWidget->GetAnmfname();
                    }
                    else
                    {
                        subresfname = PossibleMakeCopyFliename( pWidget->GetResfname() );
                        subanmfname = PossibleMakeCopyFliename( pWidget->GetAnmfname() );
                    }
                    
                    pNewWidget->LoadRes( ii, subresfname.c_str() );
                    pNewWidget->LoadAnm( ii, subanmfname.c_str() );
                    pNewWidget->SetPos( pSubWidget->GetPos() );
                }
            }
            
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Progress2d" ) == 0 )
        {
            GSO::UI::Progress2d* pNewWidget = new GSO::UI::Progress2d;
            
            for ( int i = 0; i < GSO::UI::Progress2d::PgMax; i++ )
            {
                GSO::UI::Progress2d::ProgressCom ii = ( GSO::UI::Progress2d::ProgressCom )i;
                GSO::UI::IWidget2d* pSubWidget = ( ( GSO::UI::Progress2d* )pWidget )->GetComponentButton( ii );
                
                if ( pSubWidget )
                {
                    std::string subresfname;
                    std::string subanmfname;
                    
                    if ( !bCopyFile )
                    {
                        std::string subresfname = pWidget->GetResfname();
                        std::string subanmfname = pWidget->GetAnmfname();
                    }
                    else
                    {
                        subresfname = PossibleMakeCopyFliename( pWidget->GetResfname() );
                        subanmfname = PossibleMakeCopyFliename( pWidget->GetAnmfname() );
                    }
                    
                    pNewWidget->LoadRes( ii, subresfname.c_str() );
                    pNewWidget->LoadAnm( ii, subanmfname.c_str() );
                    pNewWidget->SetPos( pSubWidget->GetPos() );
                }
            }
            
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Tab2d"     ) == 0 )
        {
            GSO::UI::Tab2d* pNewWidget = new GSO::UI::Tab2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        if ( strcmp( pWidget->ObjectName(), "Page2d"    ) == 0 )
        {
            GSO::UI::Tab2d::Page2d* pNewWidget = new GSO::UI::Tab2d::Page2d;
            pNewWidget->LoadRes( resfname.c_str() );
            pNewWidget->LoadAnm( anmfname.c_str() );
            pCloneWidget = pNewWidget;
        }
        
        HElem2d hElem2d;
        pWidget->GetHeader( &hElem2d );
        pCloneWidget->SetHeader( &hElem2d );
        return pCloneWidget;
    }
    
    GSO::UI::IWidget2d* Form2d::CloneWidget( GSO::UI::IWidget2d* pWidget, bool bCopyFile )
    {
        if ( pWidget == NULL )
        { return NULL; }
        
        GSO::UI::IWidget2d* pMe = NewWidgetFromWidget( pWidget, bCopyFile );
        pMe->SetPos( pWidget->GetPos() );
        
        for ( uint_t i = 0; i < pWidget->GetItemCount(); i++ )
        {
            GSO::UI::IWidget2d* pMyItem = ( *pWidget )[i];
            GSO::UI::IWidget2d* pNewItem = CloneWidget( pMyItem, bCopyFile );
            pMe->AttachItem( pNewItem );
        }
        
        return pMe;
    }
    
    bool SortByIdFunc ( GSO::UI::IWidget2d* elem1, GSO::UI::IWidget2d* elem2 )
    {
        //DrawNode* icon0 = ((DrawNode*)elem1);
        //DrawNode* icon1 = ((DrawNode*)elem2);
        if ( elem1->GetId() < elem2->GetId() )
        { return true; }
        
        return false;
    }
    
    
    void SortById( GSO::UI::IWidget2d* parent )
    {
        std::vector<GSO::UI::IWidget2d*> widgetlist;
        
        for ( uint_t i = 0; i < parent->GetItemCount(); i++ )
        {
            widgetlist.push_back( parent->operator[]( i ) );
        }
        
        std::stable_sort( widgetlist.begin(),
                          widgetlist.end(), SortByIdFunc );
                          
        for ( uint_t i = 0; i < widgetlist.size(); i++ )
        {
            parent->DetachItem( widgetlist[i] );
            parent->AttachItem( widgetlist[i] );
        }
    }
    
    void SortChildById( GSO::UI::IWidget2d* parent )
    {
        SortById( parent );
        
        for ( uint_t i = 0; i < parent->GetItemCount(); i++ )
        {
            SortChildById( parent->operator[]( i ) );
        }
    }
    
    void Form2d::Sort()
    {
        SortById( this );
        
        for ( uint_t i = 0; i < GetItemCount(); i++ )
        {
            SortChildById( operator[]( i ) );
        }
    }
    
    
    GSO::Element2d* GetAllChildrenResFname( GSO::Element2d* elem, void* pData )
    {
        std::vector<std::string>* namelist = ( std::vector<std::string>* )pData;
        namelist->push_back( elem->GetResfname() );
        return NULL;
    }
    
    
    void Form2d::GetAllResFilename( std::vector<std::string>& fnamelist )
    {
        ForAllChildren( GetAllChildrenResFname, &fnamelist );
    }
    
    GSO::Element2d* GetAllChildrenAnmFname( GSO::Element2d* elem, void* pData )
    {
        std::vector<std::string>* namelist = ( std::vector<std::string>* )pData;
        namelist->push_back( elem->GetAnmfname() );
        return NULL;
    }
    
    void  Form2d::GetAllAnmFilename( std::vector<std::string>& fnamelist )
    {
        ForAllChildren( GetAllChildrenAnmFname, &fnamelist );
    }
    /*
    void CALLBACK Form2d::Scroll_Query(POINT point, UINT uMsg, UINT wParam)
    {
    
        if(m_pRender==NULL)
    	    return;
    
        if((m_pRender->GetWidget() == 0) || (m_pRender->GetWidget() != GetFocus()) || (GetFocusWidget() == 0))
    	    return;
    
        switch (uMsg)
        {
        case WM_KEYDOWN:
    	    switch(wParam)
    	    {
            case 37:
    		    {
    			    GameVertex spt(GetFocusWidget()->GetPos().x - 1, GetFocusWidget()->GetPos().y);
    			    GetFocusWidget()->GetPos() = spt;
    			    //Scroll_To(spt);
    			    break;
    		    }
            case 38:
    		    {
    			    GameVertex spt(GetFocusWidget()->GetPos().x, GetFocusWidget()->GetPos().y - 1);
    			    GetFocusWidget()->GetPos() = spt;
    			    //Scroll_To(spt);
    			    break;
    		    }
            case 39:
    		    {
    			    GameVertex spt(GetFocusWidget()->GetPos().x + 1, GetFocusWidget()->GetPos().y);
    			    GetFocusWidget()->GetPos() = spt;
    			    //Scroll_To(spt);
    			    break;
    		    }
            case 40:
    		    {
    			    GameVertex spt(GetFocusWidget()->GetPos().x, GetFocusWidget()->GetPos().y + 1);
    			    GetFocusWidget()->GetPos() = spt;
    			    //Scroll_To(spt);
    			    break;
    		    }
    	    }
        }
    
    }
    */
}

