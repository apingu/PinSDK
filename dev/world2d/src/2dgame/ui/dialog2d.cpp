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
#include "Dialog2d.h"
#include "LoadDialog.cpp"
#include <pf-string.h>

namespace GSO
{
    namespace UI
    {
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Dialog2d::Dialog2d()
        {
            memset( m_CfgFilename, 0, 256 );
        }
        
        //=============================================================================
        // NAME:
        // DESC:
        //=============================================================================
        Dialog2d::~Dialog2d()
        {
            Destroy();
        }
        
        
//        //=============================================================================
//        //!  Name : writeNode()
//        //!  Desc : Save a node and its children(recursive)
//        //=============================================================================
//        void __writeNode(IWidget2d *root,GspNode *storage)
//        {
//            tchar_t buf[255];
//            bool dochild;
//
//            sprintf(buf,"Write ID=%d \n",root->GetId());    OutputDebugString(buf);
//
//            GspNode* item = new GspNode(root->GSPName());
//            dochild=root->AddPropToNode(item);
//            storage->AttachChild(item);
//
//            if(dochild){
//                for (uint_t i = 0; i < root->GetItemCount(); i++){
//                    IWidget2d* pWidget = root->operator [](i);
//                    if(pWidget)
//                        __writeNode(pWidget,item);
//                }
//            }
//        }
        //=============================================================================
        // NAME: Save()
        // DESC: save as pui file                 no finish
        //=============================================================================
        error_t Dialog2d::SaveCfg( const tchar_t* path )
        {
            tchar_t buf[255];
            CPGspParser writer;
            // 改大寫
            tchar_t* objname = new tchar_t[strlen( ObjectName() ) + 1];
            strcpy( objname, ObjectName() );
            strtoupper( objname );
            GspNode* dialog = new GspNode( objname, m_Expression.c_str() );
            SAFE_DELETE_ARRAY( objname );
            // 共通部分
            //if(!m_Expression.empty())
            //    dialog->WriteCommand(_T("EXPRESSION"), _T("="), m_Expression.c_str());
            lltoa( GetId(), buf, 10 );
            dialog->WriteCommand( _T( "ID" ), _T( "=" ), buf );
            lltoa( GetGId(), buf, 10 );
            dialog->WriteCommand( _T( "GID" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d,%d,%d" ), GetPos().x, GetPos().y, GetPos().z );
            dialog->WriteCommand( _T( "POSITION" ), _T( "=" ), buf );
            
            if ( !m_ResoureName.empty() )
            { dialog->WriteCommand( _T( "RESIDNAME" ), _T( "=" ), m_ResoureName.c_str() ); }
            
            pfstd::CString name = GetResfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name.c_str(), Element2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                { name = currname; }
                
                /*
                {
                    char path_buffer[_MAX_LPATH];
                    char new_buffer[_MAX_LPATH];
                    char drive[_MAX_DRIVE];
                    char dir[_MAX_LDIR];
                    char fname[_MAX_LFNAME];
                    char ext[_MAX_LEXT];
                
                    _splitpath(name.c_str(), drive, dir, fname, ext); // C4996
                
                    strcpy(new_buffer, fname);
                    memcpy(&(new_buffer[4]), &(fname[6]), 4);
                    memcpy(&(new_buffer[8]), &(fname[4]), 2);
                
                    _makepath(path_buffer, drive, dir, new_buffer, ext); // C4996
                
                    char oldfullpath_buffer[_MAX_LPATH];
                    char newfullpath_buffer[_MAX_LPATH];
                    sprintf(oldfullpath_buffer, "%s%s", Sprite2d::GetCurrentDirectory(), name.c_str());
                    sprintf(newfullpath_buffer, "%s%s", Sprite2d::GetCurrentDirectory(), path_buffer);
                    rename(oldfullpath_buffer, newfullpath_buffer);
                
                    name=path_buffer;
                
                }
                */
                dialog->WriteCommand( _T( "LOADRES" ), _T( "=" ) , name );
            }
            
            name = GetAnmfname();
            
            if ( !name.empty() )
            {
                const tchar_t* currname = strsplitstr( name.c_str(), Element2d::GetCurrentDirectory() );
                
                if ( currname != NULL )
                { name = currname; }
                
                dialog->WriteCommand( _T( "LOADANM" ), _T( "=" ) , name );
            }
            
            /*
            if (m_Font.GetSBCS() != NULL)
            {
                sprintf(buf,
                         "%s,%d,%d,%d,%d,0",
                         m_Font.GetSBCS()->GetFilename(),
                         m_Font.GetSBCS()->GetWidth(),
                         m_Font.GetSBCS()->GetHeight(),
                         m_Font.GetSBCS()->GetBytesPerPixel(),
                         m_Font.GetSBCS()->GetFmt());
                dialog->WriteCommand("AFONT", _T("="), buf);
            }
            
            if (m_Font.GetDBCS() != NULL)
            {
                sprintf(buf,
                         "%s,%d,%d,%d,%d,0",
                         m_Font.GetDBCS()->GetFilename(),
                         m_Font.GetDBCS()->GetWidth(),
                         m_Font.GetDBCS()->GetHeight(),
                         m_Font.GetDBCS()->GetBytesPerPixel(),
                         m_Font.GetDBCS()->GetFmt());
                dialog->WriteCommand("CFONT", _T("="), buf);
            }
            */
            sprintf( buf, _T( "%d,%d" ), m_Anim2d.GetPlayMode(), m_Anim2d.GetPlayInterrupt() );
            dialog->WriteCommand( _T( "PLAYMODE" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d" ), RenderAlpha );
            dialog->WriteCommand( _T( "ALPHA" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d" ), RenderLayer );
            dialog->WriteCommand( _T( "LAYER" ), _T( "=" ) , buf );
            sprintf( buf, _T( "%d,%d" ), GetPopupType(), GetPopupMsec() );
            dialog->WriteCommand( _T( "POPUPTYPE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetDragable() );
            dialog->WriteCommand( _T( "DRAGABLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetEnable() );
            dialog->WriteCommand( _T( "ENABLE" ), _T( "=" ), buf );
            sprintf( buf, _T( "%d" ), GetVisible() );
            dialog->WriteCommand( _T( "VISIBLE" ), _T( "=" ), buf );
            writer.AttachChildNode( dialog );
            
            for ( uint_t i = 0; i < GetItemCount(); i++ )
            {
                IWidget2d* pWidget = operator[]( i );
                pWidget->SaveCfg( dialog );
            }
            
            //tchar_t buf[255];
            /*
                        writer.Open(path);
            
                        writer.CreateObject("2DFORM");
                        writer.SetReadAttribute("LOADIMAGE", GetFileName());
            
                                 if (m_Font.GetSBCS() != NULL)
                                 {
                                     sprintf(buf,
                                              "%s,%d,%d,%d,%d,0",
                                              m_Font.GetSBCS()->GetFilename(),
                                              m_Font.GetSBCS()->GetWidth(),
                                              m_Font.GetSBCS()->GetHeight(),
                                              m_Font.GetSBCS()->GetBytesPerPixel(),
                                              m_Font.GetSBCS()->GetFmt());
                                     writer.SetReadAttribute("AFONT", buf);
                                 }
                                 if (m_Font.GetDBCS() != NULL)
                                 {
                                     sprintf(buf,
                                              "%s,%d,%d,%d,%d,0",
                                              m_Font.GetDBCS()->GetFilename(),
                                              m_Font.GetDBCS()->GetWidth(),
                                              m_Font.GetDBCS()->GetHeight(),
                                              m_Font.GetDBCS()->GetBytesPerPixel(),
                                              m_Font.GetDBCS()->GetFmt());
                                     writer.SetReadAttribute("CFONT", buf);
                                 }
                                 for (uint_t i = 0; i < GetItemCount(); i++)
                                 {
                                     tchar_t buf[255];
                                    IWidget2d* pWidget = operator[](i);
                                     writer.CreateObject(pWidget->ObjectName());
                                     tchar_t* name = (tchar_t*) (pWidget->GetFileName());
                                     if (Sprite2d::GetCurrentDirectory() != NULL)
                                     {
                                         tchar_t* s;
                                         s = strstr(name, Sprite2d::GetCurrentDirectory());
                                         if (s != NULL)
                                         {
                                             name = s;
                                             name += (strlen(Sprite2d::GetCurrentDirectory()));
                                         }
                                     }
                                     writer.SetReadAttribute("LOADIMAGE", name);
                                     itoa(pWidget->GetId(), buf, 10);
                                     writer.SetReadAttribute("Id", buf);
                                     itoa(pWidget->GetGId(), buf, 10);
                                     writer.SetReadAttribute("GId", buf);
                                     sprintf(buf,
                                              _T("%d,%d"),
                                              pWidget->GetPos().x,
                                              pWidget->GetPos().y);
                                     writer.SetReadAttribute(_T("POSITION"), buf);
                                     if(operator[](i)->GetText())
                                     {
                                         sprintf(buf,
                                                  "%s",
                                                 operator[](i)->GetText());
                                         writer.SetReadAttribute("CAPTION", buf);
                                    }
                                     writer.CloseObject();
                                 }
                                 writer.CloseObject();
            */
            writer.Save( path );
            return 1;
        }
        
        //=============================================================================
        // NAME: Load()
        // DESC: load .pui file
        //
        //  =====================================
        //           BACKGROUND.PELM
        //  =====================================
        //           ELEMENTS.LIST
        //  =====================================
        //=============================================================================
        GSO::Element2d* SortAllChildren( GSO::Element2d* elem, void* pData )
        {
            ( ( IWidget2d* )elem )->SetFocusWidget();
            return NULL;
        }
        
        error_t Dialog2d::LoadCfg( const tchar_t* path )
        {
            //CDialogConfigure reader(this);
            //int ret = reader.Load(path);
            Destroy();
            CPGspParser Loader;
            int ret = Loader.Load( path );
            CDialogConfigure reader( this );
            Loader.DoPass( &reader );
            strcpy( m_CfgFilename, path );
            // 先讓他排序一次
            ForAllChildren( SortAllChildren, NULL );
            SetFocusWidget( NULL );
            OnLoad();
            return ret;
            /*
            int elm_file_start_num = 0;
            //m_file_offset = offset;
            m_file_path   = path;
            CPFile f(path, "rb");
            //f.Lock_On(offset);
            int first_file_offset = f.Lock_On_F("BACKGROUND.PELM");  //if hum 0 file is .pelm file judg it is a backdrop
            if(first_file_offset != _ERR_FILE_NOT_FIND)
            {
                if(f.CheckFormat("PELM", 4) == _PK_OK)
                {
                    LoadBG(path, first_file_offset);
                    elm_file_start_num++;
                }
            }f.Lock_Return();
            if(f.Lock_On_F("ELEMENTS.LIST") != _ERR_FILE_NOT_FIND)
            {
                for(int i = 0; i < f.FileCount(); i++)
                {
                    CPG2Element* elm = new Button2d;
                    elm->Load(m_file_path(), f.Offset(i));
                    Object_Insert(elm);
                }
            }f.Lock_Return();
            m_pVmc->.Initiate(_VGA_PixelFormat);
            */
            return 0;
        }
        
        void Dialog2d::Destroy()
        {
            Clear();
            Initiate();
            DestroyAllItem();
        }
        
        void Dialog2d::Clear( int s )
        {
            memset( m_CfgFilename, 0, 256 );
            IWidget2d::Clear( s );
        }
        
        const char* Dialog2d::GetCfgFilename( void )
        {
            return m_CfgFilename;
        }
        
        
        ///////////////////////////////////////////////////////////////////////////////
        //
        /*
        void Dialog2d::OnNull(LG::Point point)
        {
            OnNormal(this, point);
        }
        
        void Dialog2d::OnMouseOver(int key, LG::Point point)
        {
            OnMouseOver(this, key, point);
        }
        void Dialog2d::OnLMouseDown(int key, LG::Point point)
        {
            OnLMouseDown(this, key, point);
        }
        void Dialog2d::OnRMouseDown(int key, LG::Point point)
        {
            OnRMouseDown(this, key, point);
        }
        void Dialog2d::OnLMouseUp(int key, LG::Point point)
        {
            OnLMouseUp(this, key, point);
        }
        void Dialog2d::OnRMouseUp(int key, LG::Point point)
        {
            OnRMouseUp(this, key, point);
        }
        void Dialog2d::OnLDBClick(int key, LG::Point point)
        {
            OnLDBClick(this, key, point);
        }
        void Dialog2d::OnRDBClick(int key, LG::Point point)
        {
            OnRDBClick(this, key, point);
        }
        
        void Dialog2d::OnDrag(LG::Point point)
        {
            OnDrag(this, point);
        }
        void Dialog2d::OnDragDrop(LG::Point point)
        {
            OnDragDrop(this, point);
        }
        void Dialog2d::OnDock(dword_t docktime, LG::Point point)
        {
            OnDock(this, docktime, point);
        }
        void Dialog2d::OnShow(bool f)
        {
            OnShow(this, f);
        }
        */
        
    };
};


